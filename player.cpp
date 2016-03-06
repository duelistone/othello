#include "player.h"
#include <bitset>
#include <unistd.h>
#include <climits>
#include <ctime>

// A minor change

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side s) : currBoard(Board()), side(s) {
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
     
    if (opponentsMove != NULL) {
		currBoard.doMove(opponentsMove->x, opponentsMove->y, OTHER_SIDE(side));
		cerr << TO_INDEX(opponentsMove->x, opponentsMove->y) << endl;
	}
    currBoard.findLegalMoves(side);
    if (currBoard.legalMoves == 0) return NULL;
    
    // Loop through board
    int besti = -1;
    int bestBlack = INT_MIN;
    int bestWhite = INT_MAX;
    for (int i = 0; i < 64; i++) {
		if (currBoard.legalMoves & BIT(i)) {
			Board b = currBoard.doMoveOnNewBoard(FROM_INDEX_X(i), FROM_INDEX_Y(i), side);
			b.evaluate(); // Positive evaluation better for black
			if (testingMinimax) b.evaluateTest();
			if (side == BLACK) {
				if (b.evaluation > bestBlack) {
					besti = i;
					bestBlack = b.evaluation;
					cerr << i << ' ' << b.evaluation << endl;
				}
			}
			else {
				if (b.evaluation < bestWhite) {
					besti = i;
					bestWhite = b.evaluation;
					cerr << i << ' ' << b.evaluation << endl;
				}
			}
		}
	}
	// cerr << besti << endl;
	
	// Make move
	int x = FROM_INDEX_X(besti);
	int y = FROM_INDEX_Y(besti);
    currBoard.doMove(x, y, side);
    Move *move = new Move(x, y);
    return move;
    
}
