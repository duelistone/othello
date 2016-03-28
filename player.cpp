#include "player.h"

Side abortSide;

void playerConstructorHelper(int i0 = 0, int i1 = 0, int i2 = 0, int i3 = 0, int i4 = 0, int i5 = 0, int i6 = 0, int i7 = 0, int depth = 8) {
	if (depth > 0) {
		for (int i = -1; i <= 1; i++) {
			if (depth == 8) i0 = i;
			else if (depth == 7) i1 = i;
			else if (depth == 6) i2 = i;
			else if (depth == 5) i3 = i;
			else if (depth == 4) i4 = i;
			else if (depth == 3) i5 = i;
			else if (depth == 2) i6 = i;
			else if (depth == 1) i7 = i;
			playerConstructorHelper(i0, i1, i2, i3, i4, i5, i6, i7, depth - 1);
		}
		return;
	}
	
	int cornerWeight = 9;
    int stableWeight = 3;
    //int wedgeWeight = 2;
    int unstableWeightC = -6;
    
    uint16_t data = 0;
	int eval = 0;
	if (i0) data |= EDGE_BIT(0);
	if (i0 == 1) data |= EDGE_BIT(8);
	if (i1) data |= EDGE_BIT(1);
	if (i1 == 1) data |= EDGE_BIT(9);
	if (i2) data |= EDGE_BIT(2);
	if (i2 == 1) data |= EDGE_BIT(10);
	if (i3) data |= EDGE_BIT(3);
	if (i3 == 1) data |= EDGE_BIT(11);
	if (i4) data |= EDGE_BIT(4);
	if (i4 == 1) data |= EDGE_BIT(12);
	if (i5) data |= EDGE_BIT(5);
	if (i5 == 1) data |= EDGE_BIT(13);
	if (i6) data |= EDGE_BIT(6);
	if (i6 == 1) data |= EDGE_BIT(14);
	if (i7) data |= EDGE_BIT(7);
	if (i7 == 1) data |= EDGE_BIT(15);
	
	int count = __builtin_popcount(data >> 8);
	
	for (int i = 0; i < 8; i++) {
		// If empty, continue
		if ((data & EDGE_BIT(i)) == 0) continue;
		
		Side s = (data & EDGE_BIT(8 + i)) ? BLACK : WHITE;
		
		if (i == 0 || i == 7) {
			if (s == BLACK) eval += cornerWeight;
			else eval -= cornerWeight;
			continue;
		}
		
		// Is the disc stable?
		if (count == 8) {
			// Yes
			if (i != 0 && i != 7) {
				if (s == BLACK) eval += stableWeight;
				else eval -= stableWeight;
			}
			continue;
		}
		
		int x = i;
		x--;
		while (x >= 0 && ((data & EDGE_BIT(x)) == EDGE_BIT(x))) {
			bool iset = ((data & EDGE_BIT(8 + i)) == EDGE_BIT(8 + i));
			bool xset = ((data & EDGE_BIT(8 + x)) == EDGE_BIT(8 + x));
			if ((iset && !xset) || ((!iset) && xset)) break;
			x--;
		}
		if (x == -1) {
			if (s == BLACK) eval += stableWeight;
			else eval -= stableWeight;
			continue;
		}
		else if (i == 1 || i == 6) {
			if (s == BLACK) eval += unstableWeightC;
			else eval -= unstableWeightC;
			continue;
		}
		
		x = i;
		x++;
		while (x < 8 && ((data & EDGE_BIT(x)) == EDGE_BIT(x))) {
			bool iset = ((data & EDGE_BIT(8 + i)) == EDGE_BIT(8 + i));
			bool xset = ((data & EDGE_BIT(8 + x)) == EDGE_BIT(8 + x));
			if ((iset && !xset) || ((!iset) && xset)) break;
			x++;
		}
		if (x == 8) {
			if (s == BLACK) eval += stableWeight;
			else eval -= stableWeight;
		}
		else if (i == 1 || i == 6) {
			if (s == BLACK) eval += unstableWeightC;
			else eval -= unstableWeightC;
		}
	}
	
	EDGE_VALUES[data] = eval;
	
	// ./testgame freezes when you uncomment this for who knows what reason
	/*
	bitset<8> bs((unsigned char) (data >> 8));
	bitset<8> bs2((unsigned char) data);
	cerr << bs << ' ' << bs2 << ' ' << eval << endl;
	cerr << "---" << endl;
	*/
}

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side s) : side(s), currBoard(Board()) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;
    
    playerConstructorHelper();
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}

int alphabeta(Board b, int depth, Side s, int alpha = INT_MIN, int beta = INT_MAX, bool prevPass = false) {
	int totalCount = __builtin_popcount(b.taken >> 32) + __builtin_popcount(b.taken);

	int bound;
	
	if (depth == PROB_CUT_DEPTH2 && totalCount <= 50) {
		if (beta < 500) {
			bound = round(PERCENTILE * SIGMA + beta);
			if (alphabeta(b, PROB_CUT_DEPTH1, s, bound - 1, bound) >= bound) {
				return beta;
			}
		}
		
		if (alpha > -500) {
			bound = round(-PERCENTILE * SIGMA + alpha);
			if (alphabeta(b, PROB_CUT_DEPTH1, s, bound, bound + 1) <= bound) {
				return alpha;
			}
		}
	}
	
	uint64_t legalMoves = b.findLegalMoves(s);
	
	if (depth <= 0) {
		// Parity
		// int parityWeight = 0;
		return b.evaluate();// + ( totalCount % 2 ? ( s == BLACK ? parityWeight : -parityWeight) : ( s == BLACK ? -parityWeight : parityWeight ) );
	}
	
	BoardWithSide bws(b.taken, b.black, s);
	
	// Special case
	if (legalMoves == 0) {
		if (prevPass) {
			int blacks = b.countBlack();
			int whites = b.countWhite();
			return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
		}
		return alphabeta(b, depth - 1, OTHER_SIDE(s), alpha, beta, true);
	}
	
	int besti = -1;
	if (s == BLACK) {
		int v = INT_MIN;
		if (um->count(bws) > 0) {
			int guess = (*um)[bws];
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta(b2, depth - 1, OTHER_SIDE(s), alpha, beta);
			besti = guess;
			alpha = MAX(v, alpha);
		}
		while (legalMoves != 0 && alpha < beta) {	
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s), alpha, beta);
			if (val > v || besti == -1) besti = index;
			v = MAX(v, val);
			alpha = MAX(v, alpha);
		}
		if (depth > 3) (*um)[bws] = besti;
		return alpha;
	}
	else {
		int v = INT_MAX;
		if (um->count(bws) > 0) {
			int guess = (*um)[bws];
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta(b2, depth - 1, OTHER_SIDE(s), alpha, beta);
			besti = guess;
			beta = MIN(v, beta);
		}
		while (legalMoves != 0 && alpha < beta) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s), alpha, beta);
			if (val < v || besti == -1) besti = index;
			v = MIN(v, val);
			beta = MIN(v, beta);
		}
		if (depth > 3) (*um)[bws] = besti;
		return beta;
	}
	
}

int alphabeta2(Board b, int depth, Side s, int alpha = INT_MIN, int beta = INT_MAX, int otherDepth = 0, bool prevPass = false) {
	//int totalCount = __builtin_popcount(b.taken >> 32) + __builtin_popcount(b.taken);

	// Positional evaluation
	if (otherDepth > 5 && depth > 0) {
		int pos_eval = b.pos_evaluate();
		if (s == BLACK && pos_eval > MOBILITY_CAP + THRESHOLD) return INT_MAX; // Really good move
		if (s == BLACK && pos_eval > MOBILITY_CAP / 2 + THRESHOLD) return THRESHOLD; // Probably a good move
		if (s == BLACK && pos_eval < -MOBILITY_CAP - THRESHOLD) return INT_MIN; // Really bad move
		if (s == WHITE && pos_eval < -MOBILITY_CAP - THRESHOLD) return INT_MIN; 
		if (s == WHITE && pos_eval < -MOBILITY_CAP / 2 - THRESHOLD) return -THRESHOLD;
		if (s == WHITE && pos_eval > MOBILITY_CAP + THRESHOLD) return INT_MAX;
	}

	int bound;
	
	if (depth == PROB_CUT_DEPTH2) {
		if (beta < 500) {
			bound = round(PERCENTILE * SIGMA + beta);
			if (alphabeta2(b, PROB_CUT_DEPTH1, s, bound - 1, bound, otherDepth + PROB_CUT_DEPTH2 - PROB_CUT_DEPTH1) >= bound) {
				return beta;
			}
		}
		
		if (alpha > -500) {
			bound = round(-PERCENTILE * SIGMA + alpha);
			if (alphabeta2(b, PROB_CUT_DEPTH1, s, bound, bound + 1, otherDepth + PROB_CUT_DEPTH2 - PROB_CUT_DEPTH1) <= bound) {
				return alpha;
			}
		}
	}
	
	
	if (depth <= 0) {
		// Parity
		// int parityWeight = 0;
		return b.evaluate();// + ( totalCount % 2 ? ( s == BLACK ? parityWeight : -parityWeight) : ( s == BLACK ? -parityWeight : parityWeight ) );
	}

	uint64_t legalMoves = b.findLegalMoves(s);
	
	BoardWithSide bws(b.taken, b.black, s);
	
	// Special case
	if (legalMoves == 0) {
		if (prevPass) {
			int blacks = b.countBlack();
			int whites = b.countWhite();
			return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
		}
		if (s == BLACK) return b.pos_evaluate() + NO_MOVES_LEFT_BONUS;
		else return b.pos_evaluate() - NO_MOVES_LEFT_BONUS;
	}
	
	int besti = -1;
	if (s == BLACK) {
		int v = INT_MIN;
		if (um3->count(bws) > 0) {
			int guess = (*um3)[bws];
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta2(b2, depth - 1, OTHER_SIDE(s), alpha, beta, otherDepth + 1);
			besti = guess;
			alpha = MAX(v, alpha);
		}
		while (legalMoves != 0 && alpha < beta) {	
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta2(b2, depth - 1, OTHER_SIDE(s), alpha, beta, otherDepth + 1);
			if (val > v || besti == -1) besti = index;
			v = MAX(v, val);
			alpha = MAX(v, alpha);
		}
		if (depth > 5) (*um3)[bws] = besti;
		return alpha;
	}
	else {
		int v = INT_MAX;
		if (um3->count(bws) > 0) {
			int guess = (*um3)[bws];
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta2(b2, depth - 1, OTHER_SIDE(s), alpha, beta, otherDepth + 1);
			besti = guess;
			beta = MIN(v, beta);
		}
		while (legalMoves != 0 && alpha < beta) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta2(b2, depth - 1, OTHER_SIDE(s), alpha, beta, otherDepth + 1);
			if (val < v || besti == -1) besti = index;
			v = MIN(v, val);
			beta = MIN(v, beta);
		}
		if (depth > 5) (*um3)[bws] = besti;
		return beta;
	}
	
}

pair<int, int> main_minimax(Board b, Side s, int depth, int guess = -1) {
	auto start = chrono::high_resolution_clock::now();
	
	// Depth must be greater than or equal to 1
	uint64_t legalMoves = b.findLegalMoves(s);
	if (legalMoves == 0) return make_pair(-1, -100); // Evaluation here has no meaning
	
	int besti = -1;
	if (s == BLACK) {
		int v = INT_MIN;
		if (guess >= 0) {
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta(b2, depth - 1, OTHER_SIDE(s));
			besti = guess;
		}
		while (legalMoves != 0) {
			int secondsPast = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start).count();
			// Questionable
			if (secondsPast >= 30 && depth > 10) depth--;
			if (secondsPast >= 60) break;
			
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s), v, INT_MAX);
			if (val > v || besti == -1) {
				besti = index;
				v = val;
			}
		}
		return make_pair(besti, v);
	}
	else {
		int v = INT_MAX;
		if (guess >= 0) {
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta(b2, depth - 1, OTHER_SIDE(s));
			besti = guess;
		}
		while (legalMoves != 0) {
			int secondsPast = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start).count();
			// Questionable
			if (secondsPast > 30 && depth > 10) {depth--;}
			if (secondsPast > 60) {break;}
			
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s), INT_MIN, v);
			if (val < v || besti == -1) {
				besti = index;
				v = val;
			}
		}
		return make_pair(besti, v);
	}		
}

pair<int, int> main_minimax_aw_helper(Board b, Side s, int depth, int guess, int lower, int upper) {
	// Depth must be greater than or equal to 1
	uint64_t legalMoves = b.findLegalMoves(s);
	if (legalMoves == 0) return make_pair(-1, -100); // Evaluation here has no meaning
	
	int besti = -1;
	if (s == BLACK) {
		int v = INT_MIN;
		if (guess >= 0) {
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta(b2, depth - 1, OTHER_SIDE(s), lower, upper);
			besti = guess;
		}
		while (legalMoves != 0) {
			//int secondsPast = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start).count();
			// Questionable
			//if (secondsPast >= 30 && depth > 10) depth--;
			//if (secondsPast >= 60) break;
			
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s), MAX(v, lower), upper);
			if (val > v || besti == -1) {
				besti = index;
				v = val;
			}
		}
		return make_pair(besti, v);
	}
	else {
		int v = INT_MAX;
		if (guess >= 0) {
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta(b2, depth - 1, OTHER_SIDE(s), lower, upper);
			besti = guess;
		}
		while (legalMoves != 0) {
			//int secondsPast = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start).count();
			// Questionable
			//if (secondsPast > 30 && depth > 10) {depth--;}
			//if (secondsPast > 60) {break;}
			
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s), lower, MIN(v, upper));
			if (val < v || besti == -1) {
				besti = index;
				v = val;
			}
		}
		return make_pair(besti, v);
	}
}

pair<int, int> main_minimax_aw(Board b, Side s, int depth, int guess = -1) {
	int e = alphabeta(b, depth - 6, s);
	int lower = e - ERROR1;
	int upper = e + ERROR1;
	pair<int, int> result = main_minimax_aw_helper(b, s, depth, guess, lower, upper);
	if (result.second <= lower || result.second >= upper) {
		cerr << "Recalculating" << endl;
		lower = e - ERROR2;
		upper = e + ERROR2;
		result = main_minimax_aw_helper(b, s, depth, guess, lower, upper);
		if (result.second <= lower || result.second >= upper) {
			cerr << "Recalculating again" << endl;
			lower = e - ERROR3;
			upper = e + ERROR3;
			result = main_minimax_aw_helper(b, s, depth, guess, lower, upper);
			if (result.second <= lower || result.second >= upper) {
				cerr << "Recalculating again again" << endl;
				result = main_minimax_aw_helper(b, s, depth, guess, lower, upper);
			}
		}
	}
	return result;
}

pair<int, int> main_minimax2(Board b, Side s, int depth, int guess = -1) {
	// Depth must be greater than or equal to 1
	uint64_t legalMoves = b.findLegalMoves(s);
	if (legalMoves == 0) return make_pair(-1, -100); // Evaluation here has no meaning
	
	int besti = -1;
	if (s == BLACK) {
		int v = INT_MIN;
		if (guess >= 0) {
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta2(b2, depth - 1, OTHER_SIDE(s));
			besti = guess;
		}
		while (legalMoves != 0) {		
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta2(b2, depth - 1, OTHER_SIDE(s), v, INT_MAX);
			if (val > v || besti == -1) {
				besti = index;
				v = val;
			}
		}
		return make_pair(besti, v);
	}
	else {
		int v = INT_MAX;
		if (guess >= 0) {
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta2(b2, depth - 1, OTHER_SIDE(s));
			besti = guess;
		}
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta2(b2, depth - 1, OTHER_SIDE(s), INT_MIN, v);
			if (val < v || besti == -1) {
				besti = index;
				v = val;
			}
		}
		return make_pair(besti, v);
	}		
}

long long mobilityNodes = 0;

int mobility_minimax_with_depth(Board b, Side s, int depth, int threshold = THRESHOLD) {
	// Function should be called so that depth 0 corresponds to 
	// opponent
	
	if (depth <= 0) {
		mobilityNodes++;
		if (!b.hasLegalMoves(s)) {
			if (s == BLACK && b.pos_evaluate() > -threshold) return INT_MAX;
			if (s == WHITE && b.pos_evaluate() < threshold) return INT_MAX;
		}
		return INT_MIN;
	}
	
	uint64_t lm = b.findLegalMoves(s);
	int numLegalMoves = __builtin_popcount(lm) + __builtin_popcount(lm >> 32);
	//int lm_threshold = 5;
	
	if (depth % 2) {
		// My turn
		while (lm != 0) {
			int index = __builtin_clzl(lm);
			lm &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			
				int pos_eval = b2.pos_evaluate();
				if (s == BLACK && pos_eval > threshold && depth <= 8) {
					uint64_t lm2 = b2.findLegalMoves(OTHER_SIDE(s));
					if (__builtin_popcount(lm2) + __builtin_popcount(lm2 >> 32) <= numLegalMoves) {
						return INT_MAX;
					}
				}
				if (s == BLACK && pos_eval < -threshold) continue;
				if (s == WHITE && pos_eval < -threshold && depth <= 8) {
					uint64_t lm2 = b2.findLegalMoves(OTHER_SIDE(s));
					if (__builtin_popcount(lm2) + __builtin_popcount(lm2 >> 32) <= numLegalMoves) {
						return INT_MAX;
					}
				}
				if (s == WHITE && pos_eval > threshold) continue;
			
			int eval = mobility_minimax_with_depth(b2, OTHER_SIDE(s), depth - 1);
			if (eval == INT_MAX) return INT_MAX;
		}
		return INT_MIN;
	}
	
	else {
		// Opponent's turn
		// Mobility usually doesn't decrease by 5 or more in one turn
		if (depth < 7 && numLegalMoves > 5 * (depth / 2)) return INT_MIN;
		while (lm != 0) {
			int index = __builtin_clzl(lm);
			lm &= ~BIT(index);
			//auto start = chrono::high_resolution_clock::now();
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			//cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
			//auto start = chrono::high_resolution_clock::now();
				
				int pos_eval = b2.pos_evaluate();
				if (s == BLACK && pos_eval > threshold && depth <= 8) {
					uint64_t lm2 = b2.findLegalMoves(OTHER_SIDE(s));
					if (__builtin_popcount(lm2) + __builtin_popcount(lm2 >> 32) <= numLegalMoves) {
						return INT_MAX;
					}
				}
				if (s == WHITE && pos_eval < -threshold && depth <= 8) {
					uint64_t lm2 = b2.findLegalMoves(OTHER_SIDE(s));
					if (__builtin_popcount(lm2) + __builtin_popcount(lm2 >> 32) <= numLegalMoves) {
						return INT_MAX;
					}
				}
				if (s == BLACK && pos_eval < -threshold) continue;
				if (s == WHITE && pos_eval > threshold) continue;
				
			int eval = mobility_minimax_with_depth(b2, OTHER_SIDE(s), depth - 1);
			if (eval == INT_MIN) return INT_MIN;
		}
		return INT_MAX;
	}
}

int pure_mobility_minimax_with_depth(Board b, Side s, int depth) {
	// Function should be called so that depth 0 corresponds to 
	// opponent
	
	uint64_t lm = b.findLegalMoves(s);
	
	if (depth <= 0) {
		if (lm == 0) return INT_MAX;
		return INT_MIN;
	}
	
	if (depth % 2) {
		// My turn
		while (lm != 0) {
			int index = __builtin_clzl(lm);
			lm &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int eval = pure_mobility_minimax_with_depth(b2, OTHER_SIDE(s), depth - 1);
			if (eval == INT_MAX) return INT_MAX;
		}
		return INT_MIN;
	}
	
	else {
		// Opponent's turn
		while (lm != 0) {
			int index = __builtin_clzl(lm);
			lm &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int eval = pure_mobility_minimax_with_depth(b2, OTHER_SIDE(s), depth - 1);
			if (eval == INT_MIN) return INT_MIN;
		}
		return INT_MAX;
	}
}

// First value is best move, second is like evaluation at that move
// If no mobility killing move found, first argument is -2.
pair<int, int> mobility_minimax(Board b, Side s, int maxDepth = 15) {
	uint64_t legalMoves = b.findLegalMoves(s);
	// Depths 9, 11, ..., maxDepth?
	for (int d = 11; d < maxDepth + 1; d += 2) {
		uint64_t lm = legalMoves;
		while (lm != 0) {
			int index = __builtin_clzl(lm);
			lm &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			auto start = chrono::high_resolution_clock::now();
			int eval = mobility_minimax_with_depth(b2, OTHER_SIDE(s), d - 1, THRESHOLD);
			cerr << d - 1 << ' ' << chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
			if (eval == INT_MAX) return make_pair(index, (s == BLACK) ? INT_MAX : INT_MIN);
		}
		cerr << d << endl;
	}
	
	// Second value meaningless here, should not be used
	if (legalMoves == 0) return make_pair(-1, -1000); 
	return make_pair(-2, -1000);
}

int endgame_alphabeta(Board b, Side s, int alpha = INT_MIN, int beta = INT_MAX) {
	if (abortEndgameMinimax) return (abortSide == BLACK) ? INT_MIN : INT_MAX;
	
	BoardWithSide bws(b.taken, b.black, s);
	if (um2->count(bws) > 0) {
		int result = (*um2)[bws];
		return result;
	}
	
	int totalCount = __builtin_popcount(b.taken) + __builtin_popcount(b.taken >> 32);
	uint64_t legalMoves = b.findLegalMoves(s);
	
	if (legalMoves == 0) {
		uint64_t legalMovesOther = b.findLegalMoves(OTHER_SIDE(s));
		if (legalMovesOther == 0) {
			globalEndgameNodeCount++;
			// if (globalEndgameNodeCount % (DEFAULT_MAX_NODES / 10) == 0) cerr << globalEndgameNodeCount << endl;
			if (globalEndgameNodeCount > minutesForMove * DEFAULT_MAX_NODES) abortEndgameMinimax = true;
			// Simple evaluation here
			int diff = b.countBlack() - b.countWhite();
			if (diff > 0) return INT_MAX;
			else if (diff < 0) return INT_MIN;
			else return 0;
		}
		int ret = endgame_alphabeta(b, OTHER_SIDE(s));
		if (um2->size() < MAX_HASH_SIZE && totalCount < STOP_SAVING_THRESHOLD) {
			(*um2)[bws] = ret;
		}
		return ret;
	}
	
	if (s == BLACK) {
		int v = INT_MIN;
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s), alpha, beta);
			v = MAX(v, val);
			alpha = MAX(v, alpha);
			if (beta <= alpha) break;
		}
	}
	else {
		int v = INT_MAX;
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s), alpha, beta);
			v = MIN(v, val);
			beta = MIN(v, beta);
			if (beta <= alpha) break;
		}
	}
	
	int ret = (s == BLACK) ? alpha : beta;
	if (um2->size() < MAX_HASH_SIZE && totalCount < STOP_SAVING_THRESHOLD) {
		if (alpha != 0 || beta != 0) (*um2)[bws] = ret;
	}
	return ret;
}

pair<int, int> endgame_minimax(Board b, Side s, int guess = -1) {
	uint64_t legalMoves = b.findLegalMoves(s);
	if (legalMoves == 0) return make_pair(-1, -10000); // Second value is meaningless here
	
	int besti = -1;
	int v;
	if (s == BLACK) {
		v = INT_MIN;
		if (guess > -1) {
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s), v, INT_MAX);
			besti = guess;
			v = val;
		}
		while (legalMoves != 0 && v != INT_MAX) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s), v, INT_MAX);
			if (val > v) {
				besti = index;
				v = val;
			}
		}
	}
	else {
		v = INT_MAX;
		if (guess > -1) {
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s), INT_MIN, v);
			besti = guess;
			v = val;
		}
		while (legalMoves != 0 && v != INT_MIN) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s), INT_MIN, v);
			if (val < v) {
				besti = index;
				v = val;
			}
		}
	}
	return make_pair(besti, v);
}

bool gameSolved = false;

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be NULL.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return NULL.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {        
    // Set minutesForMove to half the amount of time left
    if (msLeft > 0) minutesForMove = msLeft / 60.0 / 1000 / 2;
    
    if (opponentsMove != NULL) {
		currBoard.doMove(opponentsMove->x, opponentsMove->y, OTHER_SIDE(side));
	}
    currBoard.findLegalMoves(side);
    if (currBoard.legalMoves == 0) return NULL;
    
    int pm_black, pm_white;
    currBoard.findLegalMoves(BLACK);
    pm_black = currBoard.potentialMobility;
    currBoard.findLegalMoves(WHITE);
    pm_white = currBoard.potentialMobility;
	cerr << "Potential mobility: " << pm_black << ' ' << pm_white << endl;
	currBoard.findLegalMoves(side);
    
    int totalCount = currBoard.countBlack() + currBoard.countWhite();
    if (totalCount == 4) {
		// Move e6 without thinking
		currBoard.doMove(4, 5, side);
		Move *move = new Move(4, 5);
		return move;
	}
	else if (totalCount == 5) {
		// Choose diagonal opening
		int index = __builtin_clzl(currBoard.legalMoves & (BIT(18) | BIT(21) | BIT(42) | BIT(45)));
		int x = FROM_INDEX_X(index);
		int y = FROM_INDEX_Y(index);
		if (x == -1) return NULL;
		currBoard.doMove(x, y, side);
		Move *move = new Move(x, y);
		return move;
	}
    
    /*
    pair<int, int> ret_value = make_pair(-3, -1000);
    thread t([&ret_value, totalCount] (Board b, Side s) {
		// mobilityNodes = 0;
		cerr << "Starting thread" << endl;
		if (totalCount <= 25) ret_value = main_minimax2(b, s, 11);
		else if (totalCount <= 41) ret_value = main_minimax2(b, s, 11);
		else ret_value = make_pair(-3, -1000);
		if (ret_value.first < 0) return;
		string letters = "abcdefgh";
		cerr << "Second thread result: " << letters[FROM_INDEX_X(ret_value.first)] << ' ' << FROM_INDEX_Y(ret_value.first) + 1 << ' ' << ret_value.second << endl; //' ' << mobilityNodes << endl;
	}, currBoard, side);
    */
    
    // Set depth according to how far into game
    int depth;
    if (totalCount <= 20) depth = 12;
    else if (totalCount <= 41) depth = 12;
    else depth = INT_MAX; // Search to end (much faster)
	
    // Set counter, reset abort variables
    globalEndgameNodeCount = 0;
    abortEndgameMinimax = false;
    abortSide = side;
    
    int eval = -100; // Just a value
    // Find index of best move via search
    int besti = -1;
    pair<int, int> p;
    if (depth != INT_MAX) {
		BoardWithSide bws(currBoard.taken, currBoard.black, side);
		if (um->count(bws) > 0) besti = (*um)[bws];
		p = main_minimax_aw(currBoard, side, depth, besti);
		besti = p.first;
		eval = p.second;
	}
	else {
		if (!gameSolved) {
			pair<int, int> p2 = main_minimax(currBoard, side, 12);
			p = endgame_minimax(currBoard, side, p2.first);
			besti = p.first;
			eval = p.second;
			// If could not calculate a win or draw, fall back to other algorithm
			if (p.second == ((side == BLACK) ? INT_MIN : INT_MAX)) {
				besti = p2.first;
				eval = p2.second;
				um2->clear(); // For now, some values may be incorrect if search not done, later we may want to prune the hash table, if it's worth it
			}
			else {
				gameSolved = true;
			}
		}
		else {
			p = endgame_minimax(currBoard, side);
			besti = p.first;
			eval = p.second;
		}
	}
    
    
	
    // Temporary
    //int v1 = main_minimax(currBoard, 4, side).second;
    //int v2 = main_minimax(currBoard, 8, side).second;
    //fil << totalCount << ' ' << v1 << ' ' << v2 << endl;
    
    
    // Output some useful info and return (remove for more speed)
    cerr << totalCount + 1 << " eval: " << eval << endl;
	// if (depth == INT_MAX) cerr << ' ' << globalEndgameNodeCount;
	if (um->size() > MAX_HASH_SIZE) um->clear(); // Don't want to lose due to too much memory!
	
	// Make move
	string letters = "abcdefgh";
	// cerr << ((side == BLACK) ? "Black " : "White ") << "tentative move: " << letters[FROM_INDEX_X(besti)] << ' ' << FROM_INDEX_Y(besti) + 1 << endl; 
	//t.join();
    //if (totalCount <= 41 && ret_value.first >= 0 && ((side == BLACK) ? (eval < ret_value.second) : (eval > ret_value.second))) besti = ret_value.first; // Better move
	int x = FROM_INDEX_X(besti);
	int y = FROM_INDEX_Y(besti);
	cerr << ((side == BLACK) ? "Black " : "White ") << "Move: " << letters[x] << ' ' << y + 1 << endl;
	currBoard.doMove(x, y, side);
    Move *move = new Move(x, y);
    return move;
    
}
