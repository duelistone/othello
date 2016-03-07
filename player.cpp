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
    
    // Returns index of best move for side s, or -2 if depth is 0, for first part
	// Second part is the best evaluation found for side s
	function<pair<int, int>(Board, int, Side, bool, bool)> minimax = [&] (Board b, int depth, Side s, bool prevPass, bool useThreads) -> pair<int, int> {
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
			thread t([&retValue, &b, &minimax, legalMoves, depth, s] () {
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
			}
			t.join();
			
			// Combine results
			if (s == BLACK && retValue.second >= bestBlack) {
				return retValue;
			}
			if (s == WHITE && retValue.second <= bestWhite) {
				return retValue;
			}
			return make_pair(besti, (s == BLACK) ? bestBlack : bestWhite);
		}
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
			}
			return make_pair(besti, (s == BLACK) ? bestBlack : bestWhite);
		}
	};
    
    // Set depth according to how far into game
    int depth;
    if (totalCount <= 36) depth = 6;
    else if (totalCount <= 46) depth = 7;
    else if (totalCount <= 52) depth = 8;
    else depth = 64 - totalCount + 2;
    
    
    pair<int, int> p = minimax(currBoard, depth, side, false, true);
    int besti = p.first;
    /*
    double msElapsed = 1000 * difftime(time(NULL), startTime);
    double msSearch = msElapsed;
    int factor = 10; // Estimate for how much longer one depth larger could take
    while (msSearch * factor < msForMove - msElapsed) {
		depth++;
		time_t start_search_time = time(NULL);
		besti = minimax(currBoard, depth, side, false).first;
		msSearch = 1000 * difftime(time(NULL), start_search_time);
		msElapsed = 1000 * difftime(time(NULL), startTime);
	}*/
    
	cerr << totalCount + 1 << ' ' << difftime(time(NULL), startTime) << ' ' << p.second << endl;
	
	// Make move
	int x = FROM_INDEX_X(besti);
	int y = FROM_INDEX_Y(besti);
    currBoard.doMove(x, y, side);
    Move *move = new Move(x, y);
    return move;
    
}
