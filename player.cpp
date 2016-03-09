#include "player.h"
#include <bitset>
#include <unistd.h>
#include <climits>
#include <ctime>
#include <cassert>
#include <functional>
#include <thread> 	

// Should correspond to at most 50 sec
#define DEFAULT_MAX_NODES (10000000)

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side s) : side(s), currBoard(Board()) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

    /* 
     * TODO: Do any initialization you need to do here (setting up the board,
     * precalculating things, etc.) However, remember that you will only have
     * 30 seconds.
     */
    
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}

// Returns index of best move for side s, or -2 if depth is 0, for first part
// Second part is the best evaluation found for side s
pair<int, int> minimax(Board b, int depth, Side s, bool prevPass = false, bool useThreads = false) {
	// Base case
	if (depth == 0) {
		b.evaluate();
		return make_pair(-2, b.evaluation); 
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
		int val = minimax(b, depth - 1, OTHER_SIDE(s), true, false).second;
		return make_pair(-1, val);
	}
	
	// Main case
	if (useThreads) {
		pair<int, int> retValue;
		thread t([&retValue, &b, legalMoves, depth, s] () {
			// Almost identical to below, for first half of legalMoves
			uint64_t lm = (legalMoves >> 32) << 32;
			int besti = -1;
			int bestWhite = INT_MAX;
			int bestBlack = INT_MIN;
			while (lm != 0) {
				int index = __builtin_clzl(lm);
				lm &= ~BIT(index);
				
				Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
				int val = minimax(b2, depth - 1, OTHER_SIDE(s), false, false).second;
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
				if ((s == BLACK && bestBlack == INT_MAX) || (s == WHITE && bestWhite == INT_MIN)) break;
			}
			retValue = make_pair(besti, (s == BLACK) ? bestBlack : bestWhite);
		});
		
		// Extract a legal move (from second half of legalMoves)
		uint64_t lm = (legalMoves << 32) >> 32;
		int besti = -1;
		int bestWhite = INT_MAX;
		int bestBlack = INT_MIN;
		while (lm != 0) {
		
			int index = __builtin_clzl(lm);
			lm &= ~BIT(index);
			
			// Make move on new board
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = minimax(b2, depth - 1, OTHER_SIDE(s), false, false).second;
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
			if ((s == BLACK && bestBlack == INT_MAX) || (s == WHITE && bestWhite == INT_MIN)) break;
		}
		t.join();
		
		// Combine results
		// Not null moves get priority
		if (s == BLACK && retValue.second >= bestBlack && retValue.first > -1) {
			return retValue;
		}
		if (s == WHITE && retValue.second <= bestWhite && retValue.first > -1) {
			return retValue;
		}
		if (besti == -1) cerr << "Something's wrong" << endl;
		return make_pair(besti, (s == BLACK) ? bestBlack : bestWhite);
	} // end thread case
	else {
		uint64_t lm = legalMoves;
		int besti = -1;
		int bestWhite = INT_MAX;
		int bestBlack = INT_MIN;
		while (lm != 0) {
			int index = __builtin_clzl(lm);
			lm &= ~BIT(index);
			
			// Make move on new board
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = minimax(b2, depth - 1, OTHER_SIDE(s), false, false).second;
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
			if ((s == BLACK && bestBlack == INT_MAX) || (s == WHITE && bestWhite == INT_MIN)) break;
		}
		if (besti == -1) cerr << "Something's wrong" << endl;
		return make_pair(besti, (s == BLACK) ? bestBlack : bestWhite);
	}
}

// Global counter for how many final nodes endgameMinimax is searching
long long globalEndgameNodeCount;
bool abortEndgameMinimax = false;
Side abortSide;
double minutesForMove = 1;

// Similar function as minimax geared for the endgame
// A first part of -3 means that this computation takes too long and should be aborted
pair<int, int> endgameMinimax(Board b, Side s, bool useThreads) {
	// Abort quickly
	if (abortEndgameMinimax) return make_pair(-3, (abortSide == BLACK) ? INT_MIN : INT_MAX); // Go with worst case scenario
	
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
		return make_pair(-1, endgameMinimax(b, OTHER_SIDE(s), false).second);
	}
	
	// Main case
	if (useThreads) {
		pair<int, int> retValue;
		bool stop = false;
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
			return retValue;
		}
		if (s == WHITE && retValue.second <= bestWhite && retValue.first > -1) {
			return retValue;
		}
		return make_pair(besti, (s == BLACK) ? bestBlack : bestWhite);
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
		if (besti == -1) cerr << "Something's wrong" << endl;
		return make_pair(besti, (s == BLACK) ? bestBlack : bestWhite);
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
	// Code for testing minimax
	if (testingMinimax) {
		pair<int, int> p = minimax(currBoard, 2, side, false, false);
		int x = FROM_INDEX_X(p.first);
		int y = FROM_INDEX_Y(p.first);
		if (p.first == -1) {x = -1; y = -1;}
		currBoard.doMove(x, y, side);
		Move *move = new Move(x, y);
		return move;
	}
	
	// Usual code
    
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
    
    // Set depth according to how far into game
    int depth;
    if (totalCount <= 29) depth = 6;
    else if (totalCount <= 41) depth = 7;
    else depth = INT_MAX; // Search to end (much faster)
    
    // Set counter, reset abort variables
    globalEndgameNodeCount = 0;
    abortEndgameMinimax = false;
    abortSide = side;
    
    // Find index of best move via search
    pair<int, int> p;
    if (depth != INT_MAX) {
		p = minimax(currBoard, depth, side, false, true);
	}
	else {
		p = endgameMinimax(currBoard, side, false);
		// If could not calculate a win or draw, fall back to other algorithm
		if (p.second == ((side == BLACK) ? INT_MIN : INT_MAX)) {
			cerr << "Just minimaxing depth 7" << endl;
			p = minimax(currBoard, 7, side, false, true);
		}
	}
    int besti = p.first;
    
    // Output some useful info and return
    
	cerr << totalCount + 1 << ' ' << msLeft - difftime(time(NULL), startTime) << ' ' << p.second;
	if (depth == INT_MAX) cerr << ' ' << globalEndgameNodeCount;
	cerr << endl;
	// Make move
	int x = FROM_INDEX_X(besti);
	int y = FROM_INDEX_Y(besti);
	cerr << "Move: " << x << ' ' << y << endl;
    currBoard.doMove(x, y, side);
    Move *move = new Move(x, y);
    return move;
    
}

// Hash function idea: (taken << 2) ^ (black << 1) ^ side
