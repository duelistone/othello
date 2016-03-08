#include "player.h"
#include <bitset>
#include <unistd.h>
#include <climits>
#include <ctime>
#include <cassert>
#include <functional>
#include <thread> 	

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
pair<int, int> minimax(Board b, int depth, Side s, bool prevPass, bool useThreads) {
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
		if (s == BLACK && retValue.second == bestBlack) {
			return retValue;
		}
		if (s == WHITE && retValue.second == bestWhite)	 {
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
		return make_pair(besti, (s == BLACK) ? bestBlack : bestWhite);
	}
}

// Similar function as minimax geared for the endgame
pair<int, int> endgameMinimax(Board b, Side s, bool useThreads) {
	// Find legal moves
	uint64_t legalMoves = b.findLegalMoves(s);

	// Special case of pass
	if (legalMoves == 0) {
		uint64_t legalMovesOther = b.findLegalMoves(OTHER_SIDE(s));
		if (legalMovesOther == 0) {
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
		auto f = [&retValue, &b, legalMoves, s] () {
			// Almost identical to below, for first half of legalMoves
			uint64_t lm = (legalMoves >> 32) << 32;
			int besti = -1;
			int bestWhite = INT_MAX;
			int bestBlack = INT_MIN;
			while (lm != 0) {
				int index = __builtin_clzl(lm);
				lm &= ~BIT(index);
				
				Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
				int val = endgameMinimax(b2, OTHER_SIDE(s), false).second;
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
		};
		
		thread t(f);
		
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
			int val = endgameMinimax(b2, OTHER_SIDE(s), false).second;
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
			int val = endgameMinimax(b2, OTHER_SIDE(s), false).second;
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
	
    /* 
     * TODO: Implement how moves your AI should play here. You should first
     * process the opponent's opponents move before calculating your own move
     */
    
    // Time!
    time_t startTime = time(NULL);
    
    if (opponentsMove != NULL) {
		currBoard.doMove(opponentsMove->x, opponentsMove->y, OTHER_SIDE(side));
	}
    currBoard.findLegalMoves(side);
    if (currBoard.legalMoves == 0) return NULL;
    
    int totalCount = currBoard.countBlack() + currBoard.countWhite();
    
    // Really worst case
    //int msForMove = msLeft / (64 - totalCount);
    
    // Set depth according to how far into game
    int depth;
    if (totalCount <= 36) depth = 6;
    else if (totalCount <= 42) depth = 7;
    else depth = INT_MAX; // Search to end (much faster)
    
    // Find index of best move via search
    pair<int, int> p;
    if (depth != INT_MAX) {
		p = minimax(currBoard, depth, side, false, true);
	}
	else {
		p = endgameMinimax(currBoard, side, true);
	}
    int besti = p.first;
    
    // Output some useful info and return
	cerr << totalCount + 1 << ' ' << difftime(time(NULL), startTime) << ' ' << p.second << endl;
	// Make move
	int x = FROM_INDEX_X(besti);
	int y = FROM_INDEX_Y(besti);
	if (besti == -1) {x = -1; y = -1;}
	cerr << "Move: " << x << ' ' << y << endl;
    //bitset<64> bs(currBoard.findLegalMoves(side));
    //cerr << bs << endl;
    currBoard.doMove(x, y, side);
    Move *move = new Move(x, y);
    return move;
    
}

// Hash function idea: (taken << 2) ^ (black << 1) ^ side
