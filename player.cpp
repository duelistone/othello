#include "player.h"

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side s) : side(s), currBoard(Board()) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;    
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}

Side abortSide; 

// From black perspective, a move of less than -PRUNE_ABS will be deemed terrible,
// and a move of more than PRUNE_ABS2 will be deemed great
// Right now only PRUNE_ABS is being used
#define PRUNE_ABS (70)
#define PRUNE_ABS2 (70)

int alphabeta(Board b, int depth, Side s, int alpha = INT_MIN, int beta = INT_MAX, bool prevPass = false) {
	uint64_t legalMoves = b.findLegalMoves(s);
	
	if (depth == 0) {
		// Parity
		int totalCount = __builtin_popcount(b.taken >> 32) + __builtin_popcount(b.taken);
		int parityWeight = 2;
		return b.evaluate() + ( totalCount % 2 ? ( s == BLACK ? parityWeight : -parityWeight) : ( s == BLACK ? -parityWeight : parityWeight ) );
	}
	
	// Special case
	if (legalMoves == 0) {
		if (prevPass) {
			int blacks = b.countBlack();
			int whites = b.countWhite();
			return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
		}
		return alphabeta(b, depth - 1, OTHER_SIDE(s), alpha, beta, true);
	}
	
	if (s == BLACK) {
		int v = INT_MIN;
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s), alpha, beta);
			v = MAX(v, val);
			alpha = MAX(v, alpha);
			if (beta <= alpha) break;
		}
		return alpha;
	}
	else {
		int v = INT_MAX;
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s), alpha, beta);
			v = MIN(v, val);
			beta = MIN(v, beta);
			if (beta <= alpha) break;
		}
		return beta;
	}
}


pair<int, int> main_minimax(Board b, int depth, Side s, bool useThreads = false) {
	// Depth must be greater than or equal to 1
	uint64_t legalMoves = b.findLegalMoves(s);
	if (legalMoves == 0) return make_pair(-1, -100); // Evaluation here has no meaning
	
	if (false) {
		
	}
	
	int besti = -1;
	if (s == BLACK) {
		int v = INT_MIN;
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s));
			if (val >= v) {
				besti = index;
				v = val;
			}
		}
		return make_pair(besti, v);
	}
	else {
		int v = INT_MAX;
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s));
			if (val <= v) {
				besti = index;
				v = val;
			}
		}
		return make_pair(besti, v);
	}		
}


pair<int, int> simple_minimax(Board b, int depth, Side s, bool prevPass = false){
	if (depth == 0) {
		// Parity
		int totalCount = __builtin_popcount(b.taken >> 32) + __builtin_popcount(b.taken);
		int parityWeight = 5;
		return make_pair(-2, b.evaluate() + ( totalCount % 2 ? ( s == BLACK ? parityWeight : -parityWeight) : ( s == BLACK ? -parityWeight : parityWeight ) ));
	}
	
	// Special case
	uint64_t legalMoves = b.findLegalMoves(s);
	if (legalMoves == 0) {
		if (prevPass) {
			int blacks = b.countBlack();
			int whites = b.countWhite();
			return make_pair(-1, (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0));
		}
		return make_pair(-1, simple_minimax(b, depth - 1, OTHER_SIDE(s), true).second);
	}
	
	int besti = -1;
	if (s == BLACK) {
		int v = INT_MIN;
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = simple_minimax(b2, depth - 1, OTHER_SIDE(s)).second;
			if (val >= v) {
				besti = index;
				v = val;
			}
		}
		return make_pair(besti, v);
	}
	else {
		int v = INT_MAX;
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = simple_minimax(b2, depth - 1, OTHER_SIDE(s)).second;
			if (val <= v) {
				besti = index;
				v = val;
			}
		}
		return make_pair(besti, v);
	}
	return make_pair(-3, INT_MIN); // Should never happen
}


/*
// Optimization to avoid finding multiple winning lines
		//if (s == BLACK && v > PRUNE_ABS) break;
		//if (s == WHITE && v < -PRUNE_ABS) break;

// Returns index of best move for side s, or -2 if depth is 0, for first part
// Second part is the best evaluation found for side s
pair<int, int> minimax(Board b, int depth, Side s, bool prevPass = false, bool useThreads = false, int alpha = INT_MIN, int beta = INT_MAX) {
	// Check if result stored in table
	//BoardWithSide bws(b.taken, b.black, s);
	//ttable_lock.lock();
	//if (ttable->count(bws) > 0) {
	//	pair<int, int> result = (*ttable)[bws];
	//	ttable_lock.unlock();
	//	return result;
	//}
	//ttable_lock.unlock();
	
	// Base case
	if (depth == 0) {
		// Parity
		int totalCount = __builtin_popcount(b.taken >> 32) + __builtin_popcount(b.taken);
		int parityWeight = 5;
		return make_pair(-2, b.evaluate() + ( totalCount % 2 ? ( s == BLACK ? parityWeight : -parityWeight) : ( s == BLACK ? -parityWeight : parityWeight ) ));
	}
	
	// Special case
	b.findLegalMoves(s);
	uint64_t legalMoves = b.legalMoves;
	if (legalMoves == 0) {
		if (prevPass) {
			int blacks = b.countBlack();
			int whites = b.countWhite();
			int val = (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
			return make_pair(-1, val);
		}
		pair<int, int> val = minimax(b, depth - 1, OTHER_SIDE(s), true, false, alpha, beta);
		//ttable_lock.lock();
		//if (ttable->size() < MAX_HASH_SIZE) (*ttable)[bws] = val;
		//ttable_lock.unlock();
		return make_pair(-1, val.second);
	}
	
	// Main case
	if (useThreads) {
		pair<int, int> retValue;
		thread t([&retValue, &b, legalMoves, depth, s, alpha, beta] () {
			// Make copies
			int a = alpha;
			int bb = beta;
			
			// Almost identical to below, for first half of legalMoves
			uint64_t lm = (legalMoves >> 32) << 32;
			int besti = -1;
			while (lm != 0) {
				int index = __builtin_clzl(lm);
				lm &= ~BIT(index);
				
				Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
				int val = minimax(b2, depth - 1, OTHER_SIDE(s), false, false, a, bb).second;
				if (s == BLACK) {
					if (val >= a) {
						besti = index;
						a = val;
					}
				}
				else {
					if (val <= beta) {
						besti = index;
						bb = val;
					}
				}
				if (bb <= a) {
					break;
				}
				
				// Stop if found a really good move
				if (s == BLACK && a > PRUNE_ABS) break;
				if (s == WHITE && bb < -PRUNE_ABS) break;
			}
			retValue = make_pair(besti, (s == BLACK) ? a : bb);
		});
		
		// Extract a legal move (from second half of legalMoves)
		uint64_t lm = (legalMoves << 32) >> 32;
		int besti = -1;
		while (lm != 0) {
			int index = __builtin_clzl(lm);
			lm &= ~BIT(index);
			
			// Make move on new board
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = minimax(b2, depth - 1, OTHER_SIDE(s), false, false, alpha, beta).second;
			if (s == BLACK) {
				if (val >= alpha) {
					besti = index;
					alpha = val;
				}
			}
			else {
				if (val <= beta) {
					besti = index;
					beta = val;
				}
			}
			if (beta <= alpha) break;
			
			// Optimization to avoid finding multiple winning lines
			if (s == BLACK && alpha > PRUNE_ABS) break;
			if (s == WHITE && beta < -PRUNE_ABS) break;
		}
		t.join();
		
		// Combine results
		// Not null moves get priority
		if ((s == BLACK && retValue.second >= alpha && retValue.first > -1) || (s == WHITE && retValue.second <= beta && retValue.first > -1)) {
			//ttable_lock.lock();
			//if (ttable->size() < MAX_HASH_SIZE) (*ttable)[bws] = retValue;
			//ttable_lock.unlock();
			return retValue;
		}
		pair<int, int> p = make_pair(besti, (s == BLACK) ? alpha : beta);
		//ttable_lock.lock();
		//if (ttable->size() > DEFAULT_MAX_NODES) (*ttable)[bws] = p;
		//ttable_lock.unlock();
		return p;
	} // end thread case
	else {
		uint64_t lm = legalMoves;
		int besti = -1;
		while (lm != 0) {
			int index = __builtin_clzl(lm);
			lm &= ~BIT(index);
			
			// Make move on new board
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = minimax(b2, depth - 1, OTHER_SIDE(s), false, false, alpha, beta).second;
			if (s == BLACK) {
				if (val >= alpha) {
					besti = index;
					alpha = val;
				}
			}
			else {
				if (val <= beta) {
					besti = index;
					beta = val;
				}
			}
			if (beta <= alpha	) {
				break;
			}
			// Optimization to avoid finding multiple winning lines
			if (s == BLACK && alpha > PRUNE_ABS) break;
			if (s == WHITE && beta < -PRUNE_ABS) break;
		}
		pair<int, int> p = make_pair(besti, (s == BLACK) ? alpha : beta);
		//ttable_lock.lock();
		//if (ttable->size() < MAX_HASH_SIZE) (*ttable)[bws] = p;
		//ttable_lock.unlock();
		return p;
	}
}
*/

// endgame_alphabeta
int endgame_alphabeta(Board b, Side s, int alpha = INT_MIN, int beta = INT_MAX) {
	if (abortEndgameMinimax) return (abortSide == BLACK) ? INT_MIN : INT_MAX;
	
	BoardWithSide bws(b.taken, b.black, s);
	um_lock.lock();
	if (um2->count(bws) > 0) {
		int result = (*um2)[bws];
		um_lock.unlock();
		return result;
	}
	um_lock.unlock();
	
	int totalCount = __builtin_popcount(b.taken) + __builtin_popcount(b.taken >> 32);
	uint64_t legalMoves = b.findLegalMoves(s);
	
	if (legalMoves == 0) {
		uint64_t legalMovesOther = b.findLegalMoves(OTHER_SIDE(s));
		if (legalMovesOther == 0) {
			globalEndgameNodeCount++;
			if (globalEndgameNodeCount % (DEFAULT_MAX_NODES / 10) == 0) cerr << globalEndgameNodeCount << endl;
			if (globalEndgameNodeCount > minutesForMove * DEFAULT_MAX_NODES) abortEndgameMinimax = true;
			// Simple evaluation here
			int diff = b.countBlack() - b.countWhite();
			if (diff > 0) return INT_MAX;
			else if (diff < 0) return INT_MIN;
			else return 0;
		}
		int ret = endgame_alphabeta(b, OTHER_SIDE(s));
		um_lock.lock();
		if (um2->size() < MAX_HASH_SIZE && totalCount < STOP_SAVING_THRESHOLD) {
			(*um2)[bws] = ret;
		}
		um_lock.unlock();
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
	um_lock.lock();
	if (um2->size() < MAX_HASH_SIZE && totalCount < STOP_SAVING_THRESHOLD) {
		(*um2)[bws] = ret;
	}
	um_lock.unlock();
	return ret;
}

pair<int, int> endgame_minimax(Board b, Side s) {
	uint64_t legalMoves = b.findLegalMoves(s);
	if (legalMoves == 0) return make_pair(-1, -100); // Second value is meaningless here
	
	int besti = -1;
	int v;
	if (s == BLACK) {
		v = INT_MIN;
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s));
			if (val >= v) {
				besti = index;
				v = val;
			}
			if (val == INT_MAX) break;
		}
	}
	else {
		v = INT_MAX;
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s));
			if (val <= v) {
				besti = index;
				v = val;
			}
			if (val == INT_MIN) break;
		}
	}
	return make_pair(besti, v);
}


// Similar function as minimax geared for the endgame
// A first part of -3 means that this computation takes too long and should be aborted
pair<int, int> endgameMinimax(Board b, Side s, bool useThreads) {
	// Abort quickly
	if (abortEndgameMinimax) return make_pair(-3, (abortSide == BLACK) ? INT_MIN : INT_MAX); // Go with worst case scenario
		
	// Memoization
	BoardWithSide bws(b.taken, b.black, s);
	um_lock.lock();
	if (um->count(bws) > 0) {
		pair<int, int> result = (*um)[bws];
		um_lock.unlock();
		return result;
	}
	um_lock.unlock();
	int totalCount = __builtin_popcount(b.taken) + __builtin_popcount(b.taken >> 32);
	
	// Find legal moves
	uint64_t legalMoves = b.findLegalMoves(s);

	// Special case of pass
	if (legalMoves == 0) {
		uint64_t legalMovesOther = b.findLegalMoves(OTHER_SIDE(s));
		if (legalMovesOther == 0) {
			globalEndgameNodeCount++;
			if (globalEndgameNodeCount % 1000000 == 0) cerr << globalEndgameNodeCount << endl;
			if (globalEndgameNodeCount > minutesForMove * DEFAULT_MAX_NODES) abortEndgameMinimax = true;
			// Simple evaluation here
			int diff = b.countBlack() - b.countWhite();
			if (diff > 0) return make_pair(-2, INT_MAX);
			else if (diff < 0) return make_pair(-2, INT_MIN);
			else return make_pair(-2, 0);
		}
		
		pair<int, int> ret = make_pair(-1, endgameMinimax(b, OTHER_SIDE(s), false).second);
		um_lock.lock();
		if (um->size() < MAX_HASH_SIZE && totalCount < STOP_SAVING_THRESHOLD) {
			(*um)[bws] = ret;
		}
		um_lock.unlock();
		return ret; 
	}
	
	
	// Main case
	if (useThreads) {
		pair<int, int> retValue;
		atomic_bool stop;
		stop = false;
		auto f = [&retValue, &b, &stop, legalMoves, s] () {
			// Almost identical to below, for first half of legalMoves
			uint64_t lm = (legalMoves >> 32) << 32;
			int besti = -1;
			int bestWhite = INT_MAX;
			int bestBlack = INT_MIN;
			while (lm != 0 && !stop) {
				int index = __builtin_clzl(lm);
				lm &= ~BIT(index);
				
				if (abortEndgameMinimax) {
					// Need a non-null move
					if (besti == -1) besti = index;
					
					// Take into account worse case scenario
					if (abortSide == BLACK && s == WHITE) {
						bestWhite = INT_MIN;
						besti = index;
					}
					else if (abortSide == WHITE && s == BLACK) {
						bestBlack = INT_MAX;
						besti = index;
					}
					break;
				}
				
				Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
				pair<int, int> p = endgameMinimax(b2, OTHER_SIDE(s), false);
				int val = p.second;
				if (p.first == -3) break; // Abort, go with best move so far
				if (s == BLACK) {
					if (val >= bestBlack) {
						besti = index;
						bestBlack = val;
					}
				}
				else {
					if (val <= bestWhite) {
						besti = index;
						bestWhite = val;
					}
				}
				if ((s == BLACK && bestBlack == INT_MAX) || (s == WHITE && bestWhite == INT_MIN)) {
					stop = true;
					break;
				}
			}
			retValue = make_pair(besti, (s == BLACK) ? bestBlack : bestWhite);
		};
		
		thread t(f);
		
		// Extract a legal move (from second half of legalMoves)
		uint64_t lm = (legalMoves << 32) >> 32;
		int besti = -1;
		int bestWhite = INT_MAX;
		int bestBlack = INT_MIN;
		while (lm != 0 && !stop) {
			// Extract legal move
			int index = __builtin_clzl(lm);
			lm &= ~BIT(index);
			
			if (abortEndgameMinimax) {
				// Need a non-null move
				if (besti == -1) besti = index;
				
				// Take into account worse case scenario
				if (abortSide == BLACK && s == WHITE) {
					bestWhite = INT_MIN;
					besti = index;
				}
				else if (abortSide == WHITE && s == BLACK) {
					bestBlack = INT_MAX;
					besti = index;
				}
				break;
			}
			
			// Make move on new board
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			pair<int, int> p = endgameMinimax(b2, OTHER_SIDE(s), false);
			int val = p.second;
			if (p.first == -3) break; // Abort, go with best move so far
			if (s == BLACK) {
				if (val >= bestBlack) {
					besti = index;
					bestBlack = val;
				}
			}
			else {
				if (val <= bestWhite) {
					besti = index;
					bestWhite = val;
				}
			}
			// Optimization to avoid finding multiple winning lines
			if ((s == BLACK && bestBlack == INT_MAX) || (s == WHITE && bestWhite == INT_MIN)) {
				stop = true;
				break;
			}
		}
		t.join();
		
		// Combine results
		// Not null moves get priority
		if (s == BLACK && retValue.second >= bestBlack && retValue.first > -1) {
			um_lock.lock();
			if (um->size() < MAX_HASH_SIZE && totalCount < STOP_SAVING_THRESHOLD) (*um)[bws] = retValue;
			um_lock.unlock();
			return retValue;
		}
		if (s == WHITE && retValue.second <= bestWhite && retValue.first > -1) {
			um_lock.lock();
			if (um->size() < MAX_HASH_SIZE && totalCount < STOP_SAVING_THRESHOLD) (*um)[bws] = retValue;
			um_lock.unlock();
			return retValue;
		}
		pair<int, int> ret = make_pair(besti, (s == BLACK) ? bestBlack : bestWhite);
		if (um->size() < MAX_HASH_SIZE && totalCount < STOP_SAVING_THRESHOLD) {
			um_lock.lock();
			(*um)[bws] = ret;
			um_lock.unlock();
		}
		return ret;
	} // end thread case
	else {
		uint64_t lm = legalMoves;
		int besti = -1;
		int bestWhite = INT_MAX;
		int bestBlack = INT_MIN;
		while (lm != 0) {
			// Extract legal move
			int index = __builtin_clzl(lm);
			lm &= ~BIT(index);
			
			if (abortEndgameMinimax) {
				// Need a non-null move
				if (besti == -1) besti = index;
				
				// Take into account worse case scenario
				if (abortSide == BLACK && s == WHITE) {
					bestWhite = INT_MIN;
					besti = index;
				}
				else if (abortSide == WHITE && s == BLACK) {
					bestBlack = INT_MAX;
					besti = index;
				}
				break;
			}
			
			// Make move on new board
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			// Recurse
			pair<int, int> p = endgameMinimax(b2, OTHER_SIDE(s), false);
			int val = p.second;
			if (p.first == -3) break; // Abort, go with best move so far
			// Find best
			if (s == BLACK) {
				if (val >= bestBlack) {
					besti = index;
					bestBlack = val;
				}
			}
			else {
				if (val <= bestWhite) {
					besti = index;
					bestWhite = val;
				}
			}
			// Optimization to avoid finding multiple winning lines
			if ((s == BLACK && bestBlack == INT_MAX) || (s == WHITE && bestWhite == INT_MIN)) {
				break;
			}
			
		}
		pair<int, int> ret = make_pair(besti, (s == BLACK) ? bestBlack : bestWhite);
		um_lock.lock();
		if (um->size() < MAX_HASH_SIZE && totalCount < STOP_SAVING_THRESHOLD) {
			(*um)[bws] = ret;
		}
		um_lock.unlock();
		return ret;
	}
}

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
    // Time!
    time_t startTime = time(NULL);
    
    // Set minutesForMove to half the amount of time left
    if (msLeft > 0) minutesForMove = msLeft / 60.0 / 1000 / 2;
    
    if (opponentsMove != NULL) {
		currBoard.doMove(opponentsMove->x, opponentsMove->y, OTHER_SIDE(side));
	}
    currBoard.findLegalMoves(side);
    if (currBoard.legalMoves == 0) return NULL;
    
    int totalCount = currBoard.countBlack() + currBoard.countWhite();
    if (totalCount == 4) {
		// Move e6 without thinking
		currBoard.doMove(4, 5, side);
		Move *move = new Move(4, 5);
		return move;
	}
	else if (totalCount == 5) {
		// Choose diagonal opening
		int index = __builtin_clzl(currBoard.legalMoves & (BIT(18) | BIT(21) | BIT(42) | BIT(45));
		int x = FROM_INDEX_X(index);
		int y = FROM_INDEX_Y(index);
		Move *move = new Move(x, y);
		return move;
	}
    
    // Set depth according to how far into game
    int depth;
    if (totalCount <= 10) depth = 10;
    else if (totalCount <= 33) depth = 9; 
    else if (totalCount <= 41) depth = 10; // 41 seems to be good
    else depth = INT_MAX; // Search to end (much faster)
	
    // Set counter, reset abort variables
    globalEndgameNodeCount = 0;
    abortEndgameMinimax = false;
    abortSide = side;
    // Clear transposition table
    ttable->clear();
    
    int eval = -100; // Just a value
    // Find index of best move via search
    int besti;
    pair<int, int> p;
    if (depth != INT_MAX) {
		p = main_minimax(currBoard, depth, side);
		besti = p.first;
		eval = p.second;
	}
	else {
		p = endgame_minimax(currBoard, side);
		besti = p.first;
		eval = p.second;
		// If could not calculate a win or draw, fall back to other algorithm
		if (p.second == ((side == BLACK) ? INT_MIN : INT_MAX)) {
			cerr << "Just minimaxing" << endl;
			p = main_minimax(currBoard, msLeft > 90000 ? 10 : (msLeft > 30000 ? 9 : (msLeft > 10000 ? 8 : 7)), side);
			besti = p.first;
			eval = p.second;
			cerr << "Done minimaxing" << endl;
			um2->clear(); // For now, some values may be incorrect if search not done, later we may want to prune the hash table, if it's worth it
		}
	}
    
    // Output some useful info and return (remove for more speed)
    //int eval = alphabeta(currBoard.doMoveOnNewBoard(FROM_INDEX_X(besti), FROM_INDEX_Y(besti), side), depth - 1, OTHER_SIDE(side));
	cerr << totalCount + 1 << ' ' << msLeft - 1000 * difftime(time(NULL), startTime) << " eval: " << eval;
	if (depth == INT_MAX) cerr << ' ' << globalEndgameNodeCount;
	cerr << endl;
	ttable->clear();
	
	// Make move
	int x = FROM_INDEX_X(besti);
	int y = FROM_INDEX_Y(besti);
	string letters = "abcdefgh";
	cerr << "Move: " << letters[x] << ' ' << y + 1 << ' ' << besti << endl;
    currBoard.doMove(x, y, side);
    Move *move = new Move(x, y);
    return move;
    
}
