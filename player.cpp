#include "player.h"

#define MAX_DEPTH 16

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
    //~ int wedgeWeight = 3;
    int unstableWeightC = -4;
    //~ int unbalancedEdgeWeight = -10;
    
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
	
	function<int(int)> f = [data] (int index) -> int {
		if ((data & EDGE_BIT(index)) == 0) return -1;
		else if (data & EDGE_BIT(index + 8)) return BLACK;
		else return WHITE;
	};
	
	// Wedges (these are complicated, maybe we shouldn't deal with
	// them so generally.
	/*
	for (int i = 0; i < 8; i++) {
		for (int j = i + 4; j < 8; j++) {
			if (f(i) == -1 && f(j) == -1) {
				bool isWedge = true;
				if (f(i + 1) == BLACK && f(j - 1) == BLACK) {
					for (int k = i + 2; k < j - 1; k++) {
						if (f(k) != WHITE) isWedge = false;
					}
					if (isWedge) {
						eval -= wedgeWeight;
						if (i == 0 || i == 8) eval -= cornerWeight;
						else if (f(i - 1) == -1 && f(i + 1) == -1) {
							// Not a very useful wedge
							eval += wedgeWeight;
						}
					}
				}
				else if (f(i + 1) == WHITE && f(j - 1) == WHITE) {
					for (int k = i + 2; k < j - 1; k++) {
						if (f(k) != BLACK) isWedge = false;
					}
					if (isWedge) {
						eval += wedgeWeight;
						if (i == 0 || i == 8) eval -= cornerWeight;
						else if (f(i - 1) == -1 && f(i + 1) == -1) {
							// Not a very useful wedge
							eval -= wedgeWeight;
						}
					}
				}
			}
		}
	}
	*/
	
	// Forced corner capture (only count if to corner)
	// Won't assume it's the turn players turn for now, so this 
	// requires a wedge to be guaranteed
	// Shouldn't overlap with the wedge case covered earlier
	bool cornerCapture = true;
	int i = 0;
	if (f(i) != -1) cornerCapture = false;
	// WHITE advantage
	if (f(i + 1) != BLACK) cornerCapture = false;
	int emptySquares = 0;
	int k;
	for (k = i + 2; f(k) == -1 && k <= 7; k++) {
		emptySquares++;
	}
	if (f(k + 1) != BLACK) cornerCapture = false;
	if (emptySquares % 2 == 0) cornerCapture = false;
	if (cornerCapture) {
		eval -= 2 * cornerWeight;
		eval -= stableWeight * (emptySquares + 2);
	}
	// BLACK advantage
	if (f(i + 1) != WHITE) cornerCapture = false;
	emptySquares = 0;
	for (k = i + 2; f(k) == -1 && k <= 7; k++) {
		emptySquares++;
	}
	if (f(k + 1) != WHITE) cornerCapture = false;
	if (emptySquares % 2 == 0) cornerCapture = false;
	if (cornerCapture) {
		eval += 2 * cornerWeight;
		eval += stableWeight * (emptySquares + 2);
	}
	
	cornerCapture = true;
	i = 7;
	if (f(i) != -1) cornerCapture = false;
	// WHITE advantage
	if (f(i - 1) != BLACK) cornerCapture = false;
	emptySquares = 0;
	for (k = i - 2; f(k) == -1 && k >= 0; k--) {
		emptySquares++;
	}
	if (f(k - 1) != BLACK) cornerCapture = false;
	if (emptySquares % 2 == 0) cornerCapture = false;
	if (cornerCapture) {
		eval -= 2 * cornerWeight;
		eval -= stableWeight * (emptySquares + 2);
	}
	// BLACK advantage
	if (f(i - 1) != WHITE) cornerCapture = false;
	emptySquares = 0;
	for (int k = i - 2; f(k) == -1 && k >= 0; k--) {
		emptySquares++;
	}
	if (f(k - 1) != WHITE) cornerCapture = false;
	if (emptySquares % 2 == 0) cornerCapture = false;
	if (cornerCapture) {
		eval += 2 * cornerWeight;
		eval += stableWeight * (emptySquares + 2);
	}
	
	// Even distance, different color discs, forced corner capture
	
	cornerCapture = true;
	i = 0;
	if (f(i) != -1) cornerCapture = false;
	// WHITE advantage
	if (f(i + 1) != BLACK) cornerCapture = false;
	emptySquares = 0;
	for (k = i + 2; f(k) == -1 && k <= 7; k++) {
		emptySquares++;
	}
	if (f(k + 1) != WHITE) cornerCapture = false;
	if (emptySquares % 2) cornerCapture = false;
	if (cornerCapture) {
		eval -= 2 * cornerWeight;
		eval -= stableWeight * (emptySquares + 2);
	}
	// BLACK advantage
	if (f(i + 1) != WHITE) cornerCapture = false;
	emptySquares = 0;
	for (k = i + 2; f(k) == -1 && k <= 7; k++) {
		emptySquares++;
	}
	if (f(k + 1) != BLACK) cornerCapture = false;
	if (emptySquares % 2) cornerCapture = false;
	if (cornerCapture) {
		eval += 2 * cornerWeight;
		eval += stableWeight * (emptySquares + 2);
	}
	
	cornerCapture = true;
	i = 7;
	if (f(i) != -1) cornerCapture = false;
	// WHITE advantage
	if (f(i - 1) != BLACK) cornerCapture = false;
	emptySquares = 0;
	for (k = i - 2; f(k) == -1 && k >= 0; k--) {
		emptySquares++;
	}
	if (f(k - 1) != WHITE) cornerCapture = false;
	if (emptySquares % 2) cornerCapture = false;
	if (cornerCapture) {
		eval -= 2 * cornerWeight;
		eval -= stableWeight * (emptySquares + 2);
	}
	// BLACK advantage
	if (f(i - 1) != WHITE) cornerCapture = false;
	emptySquares = 0;
	for (k = i - 2; f(k) == -1 && k >= 0; k--) {
		emptySquares++;
	}
	if (f(k - 1) != BLACK) cornerCapture = false;
	if (emptySquares % 2) cornerCapture = false;
	if (cornerCapture) {
		eval += 2 * cornerWeight;
		eval += stableWeight * (emptySquares + 2);
	}
	
	/*
	// Special case
	if (f(0) == -1 && f(1) == BLACK && f(2) == BLACK && f(3) == -1 &&
		f(4) == -1 && f(5) == BLACK && f(6) == -1 && f(7) == BLACK) {
		eval -= cornerWeight;
		eval -= 5 * stableWeight;
	}
	else if (f(0) == -1 && f(1) == WHITE && f(2) == WHITE && f(3) == -1 &&
		f(4) == -1 && f(5) == WHITE && f(6) == -1 && f(7) == WHITE) {
		eval += cornerWeight;
		eval += 5 * stableWeight;
	}
	else if (f(7) == -1 && f(6) == BLACK && f(5) == BLACK && f(4) == -1 &&
		f(3) == -1 && f(2) == BLACK && f(1) == -1 && f(0) == BLACK) {
		eval -= cornerWeight;
		eval -= 5 * stableWeight;
	}
	else if (f(7) == -1 && f(6) == WHITE && f(5) == WHITE && f(4) == -1 &&
		f(3) == -1 && f(2) == WHITE && f(1) == -1 && f(0) == WHITE) {
		eval += cornerWeight;
		eval += 5 * stableWeight;
	}
	
	// Unbalanced edge
	if (f(0) == -1 && f(1) == BLACK && f(2) == BLACK && f(3) == BLACK &&
		f(4) == BLACK && f(5) == BLACK && f(6) == -1 && f(7) == -1) {
		eval += unbalancedEdgeWeight;
	}
	else if (f(0) == -1 && f(1) == WHITE && f(2) == WHITE && f(3) == WHITE &&
		f(4) == WHITE && f(5) == WHITE && f(6) == -1 && f(7) == -1) {
		eval -= unbalancedEdgeWeight;
	}
	else if (f(0) == -1 && f(1) == -1 && f(2) == BLACK && f(3) == BLACK &&
		f(4) == BLACK && f(5) == BLACK && f(6) == BLACK && f(7) == -1) {
		eval += unbalancedEdgeWeight;
	}
	else if (f(0) == -1 && f(1) == -1 && f(2) == WHITE && f(3) == WHITE &&
		f(4) == WHITE && f(5) == WHITE && f(6) == WHITE && f(7) == -1) {
		eval -= unbalancedEdgeWeight;
	}
	
	// Forced unbalanced edge (or worse)
	if (f(0) == -1 && f(1) == -1 && f(2) == BLACK && f(3) == -1 &&
		f(4) == BLACK && f(5) == WHITE && f(6) == -1 && f(7) == -1) {
		eval -= unbalancedEdgeWeight;
	}
	else if (f(0) == -1 && f(1) == -1 && f(2) == WHITE && f(3) == -1 &&
			 f(4) == WHITE && f(5) == BLACK && f(6) == -1 && f(7) == -1) {
		eval += unbalancedEdgeWeight;
	}
	// Symmetry
	else if (f(0) == -1 && f(1) == -1 && f(2) == WHITE && f(3) == BLACK &&
			 f(4) == -1 && f(5) == BLACK && f(6) == -1 && f(7) == -1) {
		eval -= unbalancedEdgeWeight;
	}
	else if (f(0) == -1 && f(1) == -1 && f(2) == BLACK && f(3) == WHITE &&
			 f(4) == -1 && f(5) == WHITE && f(6) == -1 && f(7) == -1) {
		eval += unbalancedEdgeWeight;
	}*/
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

int abCalls = 0;

int alphabeta(Board b, int depth, Side s, int alpha = INT_MIN, int beta = INT_MAX, bool prevPass = false) {
	//~ abCalls++;
	
	if (depth <= 0) {
		int position = b.pos_evaluate();
		int mobilityWeight = 4;
		int mobilityBoost = 5;
		if (s == BLACK) {
			uint64_t lm = b.findLegalMoves(BLACK);
			int blackMoves = __builtin_popcountll(lm & SAFE_SQUARES);
			int v = INT_MAX;
			int eval = INT_MIN;
			while (lm && eval < beta) {
				int index = __builtin_clzl(lm);
				lm ^= SINGLE_BIT[index];
				int val = __builtin_popcountll(b.doMoveOnNewBoard(index, BLACK).findLegalMoves(WHITE) & SAFE_SQUARES);
				if (val < v) {
					eval = 15 * mobilityWeight * (blackMoves - val) / (blackMoves + val + 2) + position + ((val == 0) ? mobilityBoost : 0);
				}
			}
			return eval;
		}
		else {
			uint64_t lm = b.findLegalMoves(WHITE);
			int whiteMoves = __builtin_popcountll(lm & SAFE_SQUARES);
			int v = INT_MAX;
			int eval = INT_MAX;
			while (lm && eval > alpha) {
				int index = __builtin_clzl(lm);
				lm ^= SINGLE_BIT[index];
				int val = __builtin_popcountll(b.doMoveOnNewBoard(index, WHITE).findLegalMoves(BLACK) & SAFE_SQUARES);
				if (val < v) {
					eval = 15 * mobilityWeight * (val - whiteMoves) / (whiteMoves + val + 2) + position - ((val == 0) ? mobilityBoost : 0);
				}
			}
			return eval;
		}
		
		
	}

	BoardWithSide bws(b.taken, b.black, s);
	int totalDiscs = __builtin_popcountll(b.taken);
	uint64_t legalMoves = b.findLegalMoves(s);
	
	// Prob cut
	double sigma;
	if (alpha == INT_MIN || beta == INT_MAX) goto skip_pc_ab;
	int bound, secondDepth;
	sigma = SIGMA + totalDiscs / 15;
	secondDepth = depth - 14;
	if (secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma + 6) + beta);
			if (alphabeta(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma + 6) + alpha);
			if (alphabeta(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	secondDepth = depth - 12;
	if (secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma + 5) + beta);
			if (alphabeta(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma + 5) + alpha);
			if (alphabeta(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	secondDepth = depth - 10;
	if (depth <= 16 && secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma + 4) + beta);
			if (alphabeta(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma + 4) + alpha);
			if (alphabeta(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	secondDepth = depth - 8;
	if (depth <= 14 && secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma + 3) + beta);
			if (alphabeta(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma + 3) + alpha);
			if (alphabeta(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	secondDepth = depth - 6;
	if (depth <= 12 && secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma + 2) + beta);
			if (alphabeta(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma + 2) + alpha);
			if (alphabeta(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	secondDepth = depth - 4;
	if (depth <= 10 && secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma + 1) + beta);
			if (alphabeta(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma + 1) + alpha);
			if (alphabeta(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	secondDepth = depth - 2;
	if (depth <= 8 && secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma) + beta);
			if (alphabeta(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma) + alpha);
			if (alphabeta(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	skip_pc_ab:
	

	// Special case
	if (!legalMoves) {
		if (prevPass) {
			int blacks = b.countBlack();
			int whites = b.countWhite();
			return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
		}
		return alphabeta(b, depth - 1, other_side(s), alpha, beta, true);
	}
	
	int besti = -1;
	if (depth > 3 && s && um4->count(bws)) {
			int v;
			int guess = (*um4)[bws];
			legalMoves ^= SINGLE_BIT[guess];
			v = alphabeta(b.doMoveOnNewBoard(guess, s), depth - 1, other_side(s), alpha, beta);
			besti = guess;
			alpha = (v > alpha) ? v : alpha;
			while (legalMoves && alpha < beta) {	
				int index = __builtin_clzl(legalMoves);
				legalMoves ^= SINGLE_BIT[index];
				int val = alphabeta(b.doMoveOnNewBoard(index, s), depth - 1, other_side(s), alpha, beta);
				besti = (val > v) ? index : besti;
				v = (v > val) ? v : val;
				alpha = (v > alpha) ? v : alpha;
			}
			if (besti != -1) {
				(*um4)[bws] = besti;
			}
			return alpha;
	}
	
	else if (depth > 3 && s) {
			int v = INT_MIN;
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= SINGLE_BIT[index];
			v = alphabeta(b.doMoveOnNewBoard(index, s), depth - 1, other_side(s), alpha, beta);
			besti = index;
			alpha = (v > alpha) ? v : alpha;
			while (legalMoves && alpha < beta) {	
				int index = __builtin_clzl(legalMoves);
				legalMoves ^= SINGLE_BIT[index];
				int val = alphabeta(b.doMoveOnNewBoard(index, s), depth - 1, other_side(s), alpha, beta);
				besti = (val > v) ? index : besti;
				v = (val > v) ? val : v;
				alpha = (v > alpha) ? v : alpha;
			}
			if (besti != -1) {
				(*um4)[bws] = besti;
			}
			return alpha;
	}
	else if (depth > 3 && um4->count(bws)) {
			// White & um4
			int v;
			int guess = (*um4)[bws];
			legalMoves ^= SINGLE_BIT[guess];
			v = alphabeta(b.doMoveOnNewBoard(guess, s), depth - 1, other_side(s), alpha, beta);
			besti = guess;
			beta = (v < beta) ? v : beta;
			while (legalMoves && alpha < beta) {
				int index = __builtin_clzl(legalMoves);
				legalMoves ^= SINGLE_BIT[index];
				int val = alphabeta(b.doMoveOnNewBoard(index, s), depth - 1, other_side(s), alpha, beta);
				besti = (val < v) ? index : besti;
				v = (val < v) ? val : v;
				beta = (v < beta) ? v : beta;
			}
			if (besti != -1) {
				(*um4)[bws] = besti;
			}
			return beta;
	}
	else if (depth > 3) {
			// White & no um4
			int v = INT_MAX;
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= SINGLE_BIT[index];
			v = alphabeta(b.doMoveOnNewBoard(index, s), depth - 1, other_side(s), alpha, beta);
			besti = index;
			beta = (v < beta) ? v : beta;
			while (legalMoves && alpha < beta) {
				int index = __builtin_clzl(legalMoves);
				legalMoves ^= SINGLE_BIT[index];
				int val = alphabeta(b.doMoveOnNewBoard(index, s), depth - 1, other_side(s), alpha, beta);
				besti = (val < v) ? index : besti;
				v = (val < v) ? val : v;
				beta = (v < beta) ? v : beta;
			}
			if (besti != -1) {
				(*um4)[bws] = besti;
			}
			return beta;
	}
	else if (s) {
			int v = INT_MIN;
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, s);
			v = alphabeta(b2, depth - 1, other_side(s), alpha, beta);
			alpha = (v > alpha) ? v : alpha;
			while (legalMoves && alpha < beta) {	
				int index = __builtin_clzl(legalMoves);
				legalMoves ^= SINGLE_BIT[index];
				int val = alphabeta(b.doMoveOnNewBoard(index, s), depth - 1, other_side(s), alpha, beta);
				v = (val > v) ? val : v;
				alpha = (v > alpha) ? v : alpha;
			}
			return alpha;
	}
	else {
			int v = INT_MAX;
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, s);
			v = alphabeta(b2, depth - 1, other_side(s), alpha, beta);
			beta = (v < beta) ? v : beta;			
			while (legalMoves && alpha < beta) {
				int index = __builtin_clzl(legalMoves);
				legalMoves ^= SINGLE_BIT[index];
				int val = alphabeta(b.doMoveOnNewBoard(index, s), depth - 1, other_side(s), alpha, beta);
				v = (v < val) ? v : val;
				beta = (v < beta) ? v : beta;
			}
			return beta;
	}
}

int alphabeta_pure(Board b, int depth, Side s, int alpha = INT_MIN, int beta = INT_MAX, bool prevPass = false) {
	abCalls++;
	//~ cerr << depth << endl;
	
	
	if (depth <= 0) {
		int position = b.pos_evaluate();
		int mobilityWeight = 4;
		int mobilityBoost = 5;

		if (s == BLACK) {
			uint64_t lm = b.findLegalMoves(BLACK);
			int blackMoves = __builtin_popcountll(lm & SAFE_SQUARES);
			int v = INT_MAX;
			int eval = INT_MIN;
			while (lm && eval < beta) {
				int index = __builtin_clzl(lm);
				lm ^= SINGLE_BIT[index];
				int val = __builtin_popcountll(b.doMoveOnNewBoard(index, BLACK).findLegalMoves(WHITE) & SAFE_SQUARES);
				v = (v < val) ? v : val;
				eval = 15 * mobilityWeight * (blackMoves - v) / (blackMoves + v + 2) + position;
				eval += (v == 0) ? mobilityBoost : 0;
			}
			return eval;
		}
		else {
			uint64_t lm = b.findLegalMoves(WHITE);
			int whiteMoves = __builtin_popcountll(lm & SAFE_SQUARES);
			int v = INT_MAX;
			int eval = INT_MAX;
			while (lm && eval > alpha) {
				int index = __builtin_clzl(lm);
				lm ^= SINGLE_BIT[index];
				int val = __builtin_popcountll(b.doMoveOnNewBoard(index, WHITE).findLegalMoves(BLACK) & SAFE_SQUARES);
				v = (v < val) ? v : val;
				eval = 15 * mobilityWeight * (v - whiteMoves) / (whiteMoves + v + 2) + position;
				eval -= (v == 0) ? mobilityBoost : 0;
			}
			return eval;
		}
	}
	
	
	BoardWithSide bws(b.taken, b.black, s);
	int totalDiscs = __builtin_popcountll(b.taken);
	// Prob cut (5, 3)
	int bound;
	if (depth == 5) {
		double sigma = SIGMA + totalDiscs / 15;
		if (beta < 500) {
			bound = round(PERCENTILE * sigma + beta);
			if (alphabeta_pure(b, 3, s, bound - 1, bound) >= bound) {
				return beta;
			}
		}
		
		if (alpha > -500) {
			bound = round(-PERCENTILE * sigma + alpha);
			if (alphabeta_pure(b, 3, s, bound, bound + 1) <= bound) {
				return alpha;
			}
		}
	}
	
	// Let e = (*um3)[bws], suppose s
	// If e < alpha - something, then we don't expect to beat alpha
	// so just use e.
	// If e > beta + something, then we expect to beat beta, use e
	// In between, we want to know the exact value.
	/*
	int something = SOMETHING_VALUE;
	int e;
	if (um3->count(bws)) {
		e = (*um3)[bws];
		something += e / 10;
		if (s && e > beta + something) {
			return beta;
		}
		else if (!s && e < alpha - something) {
			return alpha;
		}
	}
	*/
	uint64_t legalMoves = b.findLegalMoves(s);
	
	// Special case
	if (!legalMoves) {
		if (prevPass) {
			int blacks = b.countBlack();
			int whites = b.countWhite();
			return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
		}
		return alphabeta_pure(b, depth - 1, other_side(s), alpha, beta, true);
	}

	
	int v;
	uint64_t lm1, lm2;
	if (s && depth > 3 && um4->count(bws)) {
		int guess = (*um4)[bws];
		legalMoves &= ~SINGLE_BIT[guess];
		lm1 = legalMoves & ~X_SQUARES;
		lm2 = legalMoves & X_SQUARES;
		Board b2 = b.doMoveOnNewBoard(guess, BLACK);
		v = alphabeta_pure(b2, depth - 1, WHITE, alpha, beta);
		alpha = (v > alpha) ? v : alpha;
		while (lm1 && alpha < beta) {	
			int index = __builtin_clzl(lm1);
			lm1 &= ~SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, BLACK);
			int val = alphabeta_pure(b2, depth - 1, WHITE, alpha, beta);
			v = (val > v) ? val : v;
			alpha = (v > alpha) ? v : alpha;
		}
		while (lm2 && alpha < beta) {	
			int index = __builtin_clzl(lm2);
			lm2 &= ~SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, BLACK);
			int val = alphabeta_pure(b2, depth - 1, WHITE, alpha, beta);
			v = (val > v) ? val : v;
			alpha = (v > alpha) ? v : alpha;
		}
		return alpha;
	}
	else if (s) {
		int index = __builtin_clzl(legalMoves);
		legalMoves &= ~SINGLE_BIT[index];
		lm1 = legalMoves & ~X_SQUARES;
		lm2 = legalMoves & X_SQUARES;
		Board b2 = b.doMoveOnNewBoard(index, BLACK);
		v = alphabeta_pure(b2, depth - 1, WHITE, alpha, beta);
		alpha = (v > alpha) ? v : alpha;
		while (lm1 && alpha < beta) {	
			int index = __builtin_clzl(lm1);
			lm1 &= ~SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, BLACK);
			int val = alphabeta_pure(b2, depth - 1, WHITE, alpha, beta);
			v = (val > v) ? val : v;
			alpha = (v > alpha) ? v : alpha;
		}
		while (lm2 && alpha < beta) {	
			int index = __builtin_clzl(lm2);
			lm2 &= ~SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, BLACK);
			int val = alphabeta_pure(b2, depth - 1, WHITE, alpha, beta);
			v = (val > v) ? val : v;
			alpha = (v > alpha) ? v : alpha;
		}
		return alpha;
	}
	else if (depth > 3 && um4->count(bws)) {
		// white & um
		int guess = (*um4)[bws];
		legalMoves &= ~SINGLE_BIT[guess];
		lm1 = legalMoves & ~X_SQUARES;
		/*lm1_inner = lm1 & ~EDGES;
		lm1_outer = lm1 & EDGES;*/
		lm2 = legalMoves & X_SQUARES;
		Board b2 = b.doMoveOnNewBoard(guess, WHITE);
		v = alphabeta_pure(b2, depth - 1, BLACK, alpha, beta);
		beta = (v < beta) ? v : beta;		
		while (lm1 && alpha < beta) {
			int index = __builtin_clzl(lm1);
			lm1 &= ~SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, WHITE);
			int val = alphabeta_pure(b2, depth - 1, BLACK, alpha, beta);
			v = (val < v) ? val : v;
			beta = (v < beta) ? v : beta;
		}
		while (lm2 && alpha < beta) {
			int index = __builtin_clzl(lm2);
			lm2 &= ~SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, WHITE);
			int val = alphabeta_pure(b2, depth - 1, BLACK, alpha, beta);
			v = (val < v) ? val : v;
			beta = (v < beta) ? v : beta;
		}
		return beta;
	}
	else {
		// white & !um
		int index = __builtin_clzl(legalMoves);
		legalMoves &= ~SINGLE_BIT[index];
		lm1 = legalMoves & ~X_SQUARES;
		lm2 = legalMoves & X_SQUARES;
		Board b2 = b.doMoveOnNewBoard(index, WHITE);
		v = alphabeta_pure(b2, depth - 1, BLACK, alpha, beta);
		beta = (v < beta) ? v : beta;
		while (lm1 && alpha < beta) {
			int index = __builtin_clzl(lm1);
			lm1 &= ~SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, WHITE);
			int val = alphabeta_pure(b2, depth - 1, BLACK, alpha, beta);
			v = (val < v) ? val : v;
			beta = (v < beta) ? v : beta;
		}
		while (lm2 && alpha < beta) {
			int index = __builtin_clzl(lm2);
			lm2 &= ~SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, WHITE);
			int val = alphabeta_pure(b2, depth - 1, BLACK, alpha, beta);
			v = (val < v) ? val : v;
			beta = (v < beta) ? v : beta;
		}
		return beta;
	}
}

int pvsCalls = 0;
int pvs(Board b, int depth, Side s, int alpha = INT_MIN, int beta = INT_MAX, bool prevPass = false) {
	pvsCalls++;
	//~ if (beta - alpha > 1) cerr << alpha << ' ' << beta << endl;
	
	// 10: 11.5 sec
	// 9: 9.9 sec
	// 8: 9.2 sec        22 sec 
	// 7: 8.3 sec 45 sec 21 sec 1:57
	// 6: 8.6 sec 44 sec 21 sec 1:55
	// 5: 8.6 sec
	if (depth < 6) {
		return alphabeta(b, depth, s, alpha, beta, prevPass);
	}
	
	int totalDiscs = __builtin_popcountll(b.taken);
	BoardWithSide bws(b.taken, b.black, s);
	
	// Prob cut
	int bound, secondDepth;
	double sigma;
	if (alpha == INT_MIN || beta == INT_MAX) goto skip_pc;
	sigma = SIGMA + totalDiscs / 15;
	secondDepth = depth - 14;
	if (secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma + 6) + beta);
			if (pvs(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma + 6) + alpha);
			if (pvs(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	secondDepth = depth - 12;
	if (secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma + 5) + beta);
			if (pvs(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma + 5) + alpha);
			if (pvs(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	secondDepth = depth - 10;
	if (depth <= 16 && secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma + 4) + beta);
			if (pvs(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma + 4) + alpha);
			if (pvs(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	secondDepth = depth - 8;
	if (depth <= 14 && secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma + 3) + beta);
			if (pvs(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma + 3) + alpha);
			if (pvs(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	secondDepth = depth - 6;
	if (depth <= 12 && secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma + 2) + beta);
			if (alphabeta(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma + 2) + alpha);
			if (pvs(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	secondDepth = depth - 4;
	if (depth <= 10 && secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma + 1) + beta);
			if (pvs(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma + 1) + alpha);
			if (pvs(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	secondDepth = depth - 2;
	if (depth <= 8 && secondDepth > 0) {
		if (beta < 500) {
			bound = round(PERCENTILE * (sigma) + beta);
			if (pvs(b, secondDepth, s, bound - 1, bound) >= bound) return beta;
		}
		if (alpha > -500) {
			bound = round(-PERCENTILE * (sigma) + alpha);
			if (pvs(b, secondDepth, s, bound, bound + 1) <= bound) return alpha;
		}
	}
	skip_pc:
	
	// Transposition table
	//~ if (um5->count(bws)) return (*um5)[bws];	
	
	uint64_t legalMoves = b.findLegalMoves(s);
		
	// Special case
	if (!legalMoves) {
		if (prevPass) {
			int blacks = b.countBlack();
			int whites = b.countWhite();
			return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
		}
		return pvs(b, depth - 1, other_side(s), alpha, beta, true);
	}
	
	int v, index, besti;
	uint64_t lm1, lm2;
	if (s && um4->count(bws)) {
		index = (*um4)[bws];
		besti = -1; // No need to replace same value
		legalMoves ^= SINGLE_BIT[index];
		lm1 = legalMoves & ~X_SQUARES;
		lm2 = legalMoves & X_SQUARES;
		Board b2 = b.doMoveOnNewBoard(index, s);
		v = pvs(b2, depth - 1, other_side(s), alpha, beta);
		alpha = (v > alpha) ? v : alpha;
		if (alpha >= beta) {
			return alpha;
		}
		
		// Prove that it is indeed the best move
		while (lm1 && alpha < beta) {	
			index = __builtin_clzl(lm1);
			lm1 ^= SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, s);
			v = pvs(b2, depth - 1, other_side(s), alpha, alpha + 1);
			if (v >= alpha + 1) {
				v = pvs(b2, depth - 1, other_side(s), v, beta);
				besti = index;
				alpha = v;
			}
		}
		while (lm2 && alpha < beta) {	
			index = __builtin_clzl(lm2);
			lm2 ^= SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, s);
			v = pvs(b2, depth - 1, other_side(s), alpha, alpha + 1);
			if (v >= alpha + 1) {
				v = pvs(b2, depth - 1, other_side(s), v, beta);
				besti = index;
				alpha = v;
			}
		}
		if (besti != -1) (*um4)[bws] = besti;
		return alpha;
	}
	else if (s) {
		index = __builtin_clzl(legalMoves);
		besti = index;
		legalMoves ^= SINGLE_BIT[index];
		lm1 = legalMoves & ~X_SQUARES;
		lm2 = legalMoves & X_SQUARES;
		Board b2 = b.doMoveOnNewBoard(index, s);
		v = pvs(b2, depth - 1, other_side(s), alpha, beta);
		alpha = (v > alpha) ? v : alpha;
		if (alpha >= beta) {
			return alpha;
		}
		
		// Prove that it is indeed the best move
		while (lm1 && alpha < beta) {	
			index = __builtin_clzl(lm1);
			lm1 ^= SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, s);
			v = pvs(b2, depth - 1, other_side(s), alpha, beta);
			besti = (v > alpha) ? index : besti;
			alpha = (v > alpha) ? v : alpha;
		}
		while (lm2 && alpha < beta) {	
			index = __builtin_clzl(lm2);
			lm2 ^= SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, s);
			v = pvs(b2, depth - 1, other_side(s), alpha, beta);
			besti = (v > alpha) ? index : besti;
			alpha = (v > alpha) ? v : alpha;
		}
		if (besti != -1) (*um4)[bws] = besti;
		return alpha;
	}
	else if (um4->count(bws)) {
		index = (*um4)[bws];
		besti = -1;
		legalMoves ^= SINGLE_BIT[index];
		lm1 = legalMoves & ~X_SQUARES;
		lm2 = legalMoves & X_SQUARES;
		Board b2 = b.doMoveOnNewBoard(index, s);
		v = pvs(b2, depth - 1, other_side(s), alpha, beta);
		beta = (v < beta) ? v : beta;
		besti = index;
		if (alpha >= beta) {
			return beta;
		}
		
		// Prove that it is indeed the best move
		while (lm1 && alpha < beta) {	
			index = __builtin_clzl(lm1);
			lm1 ^= SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, s);
			v = pvs(b2, depth - 1, other_side(s), beta - 1, beta);
			if (v <= beta - 1) {
				v = pvs(b2, depth - 1, other_side(s), alpha, v);
				besti = index;
				beta = v;
			}
		}
		while (lm2 && alpha < beta) {	
			index = __builtin_clzl(lm2);
			lm2 ^= SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, s);
			v = pvs(b2, depth - 1, other_side(s), beta - 1, beta);
			if (v <= beta - 1) {
				v = pvs(b2, depth - 1, other_side(s), alpha, v);
				besti = index;
				beta = v;
			}
		}
		if (besti != -1) (*um4)[bws] = besti;
		return beta;
	}
	else {
		index = __builtin_clzl(legalMoves);
		besti = index;
		legalMoves ^= SINGLE_BIT[index];
		lm1 = legalMoves & ~X_SQUARES;
		lm2 = legalMoves & X_SQUARES;
		Board b2 = b.doMoveOnNewBoard(index, s);
		v = pvs(b2, depth - 1, other_side(s), alpha, beta);
		beta = (v < beta) ? v : beta;
		besti = index;
		if (alpha >= beta) {
			return beta;
		}
		
		// Prove that it is indeed the best move
		while (lm1 && alpha < beta) {	
			index = __builtin_clzl(lm1);
			lm1 ^= SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, s);
			v = pvs(b2, depth - 1, other_side(s), alpha, beta);
			besti = (v < alpha) ? index : besti;
			beta = (v < beta) ? v : beta;
		}
		while (lm2 && alpha < beta) {	
			index = __builtin_clzl(lm2);
			lm2 ^= SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, s);
			v = pvs(b2, depth - 1, other_side(s), alpha, beta);
			besti = (v > alpha) ? index : besti;
			alpha = (v > alpha) ? v : alpha;
		}
		if (besti != -1) (*um4)[bws] = besti;
		return beta;
	}
	
}

pair<int, int> main_minimax_aw_helper(Board b, Side s, int depth, int guess, int lower, int upper) {
	// Depth must be greater than or equal to 1
	uint64_t legalMoves = b.findLegalMoves(s);
	if (!legalMoves) return make_pair(-1, -100); // Evaluation here has no meaning
	
	int besti = -1;
	if (s == BLACK) {
		int v = INT_MIN;
		if (guess >= 0) {
			legalMoves ^= BIT(guess);
			Board b2 = b.doMoveOnNewBoard(guess, s);
			v = pvs(b2, depth - 1, other_side(s), lower, upper);
			besti = guess;
			lower = (v > lower) ? v : lower;
		}
		while (legalMoves) {
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= SINGLE_BIT[index];
			
			Board b2 = b.doMoveOnNewBoard(index, s);
			int val;
			val = pvs(b2, depth - 1, other_side(s), v, v + 1); 
			if (val >= v + 1) {
				val = pvs(b2, depth - 1, other_side(s), v + 1, upper);
			}
			if (val > v || besti == -1) {
				besti = index;
				v = val;
			}
			lower = (v > lower) ? v : lower;
		}
		return make_pair(besti, lower);
	}
	else {
		int v = INT_MAX;
		if (guess >= 0) {
			legalMoves ^= BIT(guess);
			Board b2 = b.doMoveOnNewBoard(guess, s);
			v = pvs(b2, depth - 1, other_side(s), lower, upper);
			besti = guess;
			upper = (v < upper) ? v : upper;
		}
		while (legalMoves) {
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= SINGLE_BIT[index];
			
			Board b2 = b.doMoveOnNewBoard(index, s);
			int val;
			val = pvs(b2, depth - 1, other_side(s), v - 1, v);
			if (val <= v - 1) val = pvs(b2, depth - 1, other_side(s), lower, upper);
			if (val < v || besti == -1) {
				besti = index;
				v = val;
			}
			upper = (v < upper) ? v : upper;
		}
		return make_pair(besti, upper);
	}
}

pair<int, int> main_minimax_aw(Board b, Side s, int depth, int guess = -1) {
	// Best time: 1:58
	// DEPTH CANNOT BE ZERO!
	auto start = chrono::high_resolution_clock::now();
	int d = 2;
	int e, lower, upper, counter;
	um5->clear();
	//~ d = alphabeta(b, depth - 6, s);
	//~ cerr << "Finished depth " << depth - 6 << " search: " << e << ' ' << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
	e = pvs(b, depth - 4, s);
	cerr << "Finished depth " << depth - 4 << " search: " << e << ' ' << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
	//~ ee = alphabeta_odd(b, depth - 3, s);
	//~ cerr << "Finished depth " << depth - 3 << " search: " << ee << ' ' << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
	// Denominator 15: 2:21
	// Denominator 17: 2:21
	// Denominator 12: 2:21
	// Denominator 20: 2:21
	// Denominator 25: 2:28
	// Denominator 10: 2:14!
	// Denominator 8: 2:13!
	// Denominator 7: 2:13 
	// Denominator 6: 2:13
	// Denominator 5: 2:14
	d = abs(e) / 8 + 3;
	lower = e - d;
	upper = e + d;
	counter = 1;
	try_again3:
	um5->clear();
	e = pvs(b, depth - 2, s, lower, upper);
	// Using 2 * counter: 2:27
	// Using 4: 2:29
	// Using d * counter: 2:28
	// Using (d - 1) * counter: 2:27
	// Using (d - 2) * counter: 2:25
	if (e <= lower && lower != INT_MIN) {
		cerr << "Recalculating (failed low)" << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
		lower -= (d - 2) * counter;
		counter++;
		if (counter > 3) lower = INT_MIN;
		goto try_again3;
	}
	else if (e >= upper && upper != INT_MAX) {
		cerr << "Recalculating (failed high)" << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
		upper += (d - 2) * counter;
		counter++;
		if (counter > 3) upper = INT_MAX;
		goto try_again3;
	}
	cerr << "Finished depth " << depth - 2 << " search: " << e << ' ' << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
	lower = e - d; // Put bounds at -25, 25 too?
	upper = e + d;
	counter = 1;
	//~ besti = (*um4)[BoardWithSide(b.taken, b.black, s)];
	try_again2:
	um5->clear();
	pair<int, int> result;
	e = pvs(b, depth, s, lower, upper);
	// This is bound to segfault one day
	result = make_pair((*um4)[BoardWithSide(b.taken, b.black, s)], e);
	if (result.second <= lower && lower != INT_MIN) {
		cerr << "Recalculating (failed low)" << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
		lower -= (d - 2) * counter;
		counter++;
		if (counter > 3) lower = INT_MIN;
		goto try_again2;
	}
	else if (result.second >= upper && upper != INT_MAX) {
		cerr << "Recalculating (failed high)" << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
		upper += (d - 2) * counter;
		counter++;
		if (counter > 3) upper = INT_MAX;
		goto try_again2;
	}
	cerr << "Finished depth " << depth << " search: " << result.second << ' ' << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
	return result;
}

int endgame_alphabeta(Board b, Side s, int alpha = INT_MIN, int beta = INT_MAX) {
	// TODO: Make find win function
	if (abortEndgameMinimax) return (abortSide == BLACK) ? INT_MIN : INT_MAX;
	
	BoardWithSide bws(b.taken, b.black, s);
	if (um2->count(bws) > 0) {
		return (*um2)[bws];
	}
	
	int totalCount = __builtin_popcountll(b.taken);
	double maxNodes = minutesForMove * DEFAULT_MAX_NODES;
	
	if (totalCount == 64) {
		//auto start = chrono::high_resolution_clock::now();
		globalEndgameNodeCount++;
		//if (globalEndgameNodeCount % (DEFAULT_MAX_NODES / 10) == 0) cerr << globalEndgameNodeCount << endl;
		if (globalEndgameNodeCount > maxNodes) abortEndgameMinimax = true;
		// Simple evaluation here
		int diff = __builtin_popcountll(b.black);
		//cerr << "Endgame time: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
		// TODO: Try using array
		if (diff > 32) return INT_MAX;
		else if (diff < 32) return INT_MIN;
		else return 0;
	}
	
	uint64_t legalMoves = b.findLegalMoves(s);
	
	if (legalMoves == 0) {
		uint64_t legalMovesOther = b.findLegalMoves(other_side(s));
		if (legalMovesOther == 0) {
			globalEndgameNodeCount++;
			//if (globalEndgameNodeCount % (DEFAULT_MAX_NODES / 10) == 0) cerr << globalEndgameNodeCount << endl;
			if (globalEndgameNodeCount > maxNodes) abortEndgameMinimax = true;
			// Simple evaluation here
			int diff = b.countBlack() - b.countWhite();
			if (diff > 0) return INT_MAX;
			else if (diff < 0) return INT_MIN;
			else return 0;
		}
		int ret = endgame_alphabeta(b, other_side(s), alpha, beta);
		if (um2->size() < MAX_HASH_SIZE && totalCount < STOP_SAVING_THRESHOLD) {
			(*um2)[bws] = ret;
		}
		return ret;
	}
	
	if (totalCount == 63) {
		int blackCount = __builtin_popcountll(b.black);
		int diff = 2 * blackCount - 63;
		if (s == BLACK) {
			// Black has a legal move, so will gain at 3 disc differential
			if (diff + 3 > 0) return INT_MAX;
			if (diff + 37 < 0) return INT_MIN;
		}
		else {
			// White has a legal move, so will gain at most 
			// 2 * 18 + 1 = 37 differential
			// Equivalently, we can check that blackCount > 50
			if (diff - 37 > 0) return INT_MAX;
			if (diff - 3 < 0) return INT_MIN;
		}
	}
	
	int guess = -1;
	if (totalCount < 58 && um4->count(bws)) guess = (*um4)[bws];
	
	if (s == BLACK) {
		int v = INT_MIN;
		if (guess > -1) {
			legalMoves ^= SINGLE_BIT[guess];
			Board b2 = b.doMoveOnNewBoard(guess, s);
			v = endgame_alphabeta(b2, other_side(s), alpha, beta);
			alpha = (v > alpha) ? v : alpha;
		}
		while (legalMoves && alpha < beta) {
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, s);
			v = endgame_alphabeta(b2, other_side(s), alpha, beta);
			alpha = (v > alpha) ? v : alpha;
		}
	}
	else {
		int v = INT_MAX;
		if (guess > -1) {
			legalMoves ^= SINGLE_BIT[guess];
			Board b2 = b.doMoveOnNewBoard(guess, s);
			v = endgame_alphabeta(b2, other_side(s), alpha, beta);
			beta = (v < beta) ? v : beta;
		}
		while (legalMoves && alpha < beta) {
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= SINGLE_BIT[index];
			Board b2 = b.doMoveOnNewBoard(index, s);
			v = endgame_alphabeta(b2, other_side(s), alpha, beta);
			beta = (v < beta) ? v : beta;
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
			legalMoves ^= SINGLE_BIT[guess];
			Board b2 = b.doMoveOnNewBoard(guess, s);
			int val = endgame_alphabeta(b2, other_side(s), v, INT_MAX);
			besti = guess;
			v = val;
		}
		while (legalMoves && v != INT_MAX) {
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= SINGLE_BIT[index];
			
			Board b2 = b.doMoveOnNewBoard(index, s);
			int val = endgame_alphabeta(b2, other_side(s), v, INT_MAX);
			if (val > v) {
				besti = index;
				v = val;
			}
		}
	}
	else {
		v = INT_MAX;
		if (guess > -1) {
			legalMoves ^= SINGLE_BIT[guess];
			Board b2 = b.doMoveOnNewBoard(guess, s);
			int val = endgame_alphabeta(b2, other_side(s), INT_MIN, v);
			besti = guess;
			v = val;
		}
		while (legalMoves && v != INT_MIN) {
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= SINGLE_BIT[index];
			
			Board b2 = b.doMoveOnNewBoard(index, s);
			int val = endgame_alphabeta(b2, other_side(s), INT_MIN, v);
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
		currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(opponentsMove->x, opponentsMove->y), other_side(side));
	}
    
    uint64_t legalMoves = currBoard.findLegalMoves(side);
    if (!legalMoves) return NULL;
    
    int totalCount = __builtin_popcountll(currBoard.taken);
    if (totalCount == 4) {
		// Move e6 without thinking
		currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(4, 5), side);
		Move *move = new Move(4, 5);
		return move;
	}
	else if (totalCount == 5) {
		// Choose diagonal opening
		int index = __builtin_clzl(legalMoves & (BIT(18) | BIT(21) | BIT(42) | BIT(45)));
		int x = FROM_INDEX_X(index);
		int y = FROM_INDEX_Y(index);
		if (x == -1) return NULL;
		currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(x, y), side);
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
    if (totalCount <= 20) depth = MAX_DEPTH;
    else if (totalCount <= 30) depth = MAX_DEPTH;
    else if (totalCount <= 41) depth = MAX_DEPTH;
    else depth = INT_MAX; // Search to end (much faster)
	
    // Set counter, reset abort variables
    globalEndgameNodeCount = 0;
    abCalls = 0;
    pvsCalls = 0;
    um2->clear();
    abortEndgameMinimax = false;
    abortSide = side;
    
    int eval = -100; // Just a value
    // Find index of best move via search
    int besti = -1;
    pair<int, int> p;
    if (depth != INT_MAX) {
		//BoardWithSide bws(currBoard.taken, currBoard.black, side);
		//if (um->count(bws) > 0) besti = (*um)[bws];
		p = main_minimax_aw(currBoard, side, depth, besti);
		besti = p.first;
		eval = p.second;
	}
	else {
		cerr << "Endgame search" << endl;
		if (!gameSolved) {
			pair<int, int> p2 = main_minimax_aw(currBoard, side, MAX_DEPTH);
			auto start = chrono::high_resolution_clock::now();
			p = endgame_minimax(currBoard, side, p2.first);
			cerr << chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
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
    
    // Prune um
    /*
     * This doesn't necessarily work as I'd expect
    cerr << "Pruning um..." << endl;
    for (auto pr : *um) {
		if (pr.first.count() <= totalCount) um->erase(pr.first);
	}
	cerr << "...done pruning." << endl;
	*/
	
    // Temporary
    //int v1 = main_minimax(currBoard, 4, side).second;
    //int v2 = main_minimax(currBoard, 8, side).second;
    //fil << totalCount << ' ' << v1 << ' ' << v2 << endl;
    
    
    // Output some useful info and return (remove for more speed)
    cerr << totalCount + 1 << " eval: " << eval << ' ' << abCalls << ' ' << pvsCalls << endl;
	if (depth == INT_MAX) cerr << ' ' << globalEndgameNodeCount << endl;
	// if (um->size() > MAX_HASH_SIZE) um->clear(); // Don't want to lose due to too much memory!
	
	// Make move
	string letters = "abcdefgh";
	// cerr << ((side == BLACK) ? "Black " : "White ") << "tentative move: " << letters[FROM_INDEX_X(besti)] << ' ' << FROM_INDEX_Y(besti) + 1 << endl; 
	//t.join();
    //if (totalCount <= 41 && ret_value.first >= 0 && ((side == BLACK) ? (eval < ret_value.second) : (eval > ret_value.second))) besti = ret_value.first; // Better move
	int x = FROM_INDEX_X(besti);
	int y = FROM_INDEX_Y(besti);
	cerr << ((side == BLACK) ? "Black " : "White ") << "Move: " << letters[x] << ' ' << y + 1 << endl;
	currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(x, y), side);
    //bitset<64> bsTaken(currBoard.taken), bsBlack(currBoard.black);
    //cerr << bsTaken << endl << bsBlack << endl;
    Move *move = new Move(x, y);
    return move;
    
}
