#include "board2.h"
#include <iostream>


/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() {
	taken = 0;
	black = 0;
	legalMovesComputed = false;
	legalMoves = 0;
	
    OCCUPY_WHITE(3 + 8 * 3, taken, black);
    OCCUPY_BLACK(3 + 8 * 4, taken, black);
    OCCUPY_BLACK(4 + 8 * 3, taken, black);
    OCCUPY_WHITE(4 + 8 * 4, taken, black);
}

Board::Board(uint64_t t, uint64_t b) {
	taken = t;
	black = b;
	legalMovesComputed = false;
	legalMoves = 0;
	
    OCCUPY_WHITE(3 + 8 * 3, taken, black);
    OCCUPY_BLACK(3 + 8 * 4, taken, black);
    OCCUPY_BLACK(4 + 8 * 3, taken, black);
    OCCUPY_WHITE(4 + 8 * 4, taken, black);
}	

uint64_t Board::findLegalMoves(Side side) {
	legalMoves = 0;
	
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			if (checkMove(x, y, side)) {
				cerr << "Checkmove true" << endl;
				legalMoves |= BIT(TO_INDEX(x, y));
			}
		}
	}
	cerr << "findlegalmoves returning" << endl;
	legalMovesComputed = true;
	return legalMoves;
}

/*
 * Returns true if the game is finished; false otherwise. The game is finished 
 * if neither side has a legal move.
 */
bool Board::isDone() {
    return !(hasMoves(BLACK) || hasMoves(WHITE));
}

/*
 * Returns true if there are legal moves for the given side.
 */
bool Board::hasMoves(Side side) {
	if (!legalMovesComputed) findLegalMoves(side);
	return legalMoves != 0;
}

/*
 * Returns true if a move is legal for the given side; false otherwise.
 */
bool Board::checkMove(int X, int Y, Side side) {
    // Passing is only legal if you have no moves.
    if (X == -1) return !hasMoves(side);

    // Make sure the square hasn't already been taken.
    if (OCCUPIED(X, Y, taken)) return false;

    Side other = (side == BLACK) ? WHITE : BLACK;
    cerr << "Starting checkMove loops " << X << ' ' << Y << endl;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;
			cerr << "Inside checkMove loop dy " << dy << " dx " << dx << endl;	

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
                do {
                    x += dx;
                    y += dy;
                } while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

                if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
					cerr << X << ' ' << Y << " is valid " << dx << ' ' << dy << ' ' << x << ' ' << y << endl;
					return true;
				}
            }
        }
    }
    cerr << "checkMove Returning" << endl;
    return false;
}

/*
 * Modifies the board to reflect the specified move.
 */
void Board::doMove(int X, int Y, Side side) {
    // A NULL move means pass.
    if (X == -1) return;

    // Ignore if move is invalid.
    if (!checkMove(X, Y, side)) return;

    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            int x = X;
            int y = Y;
            do {
                x += dx;
                y += dy;
            } while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

            if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
                x = X;
                y = Y;
                x += dx;
                y += dy;
                while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
                    PLACE_DISC(side, x, y, taken, black);
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    PLACE_DISC(side, X, Y, taken, black);
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() {
    return __builtin_popcount(black);
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return __builtin_popcount(taken) - __builtin_popcount(black);
}

/*
 * Sets the board state given an 8x8 char array where 'w' indicates a white
 * piece and 'b' indicates a black piece. Mainly for testing purposes.
 */
void Board::setBoard(char data[]) {
    taken = 0;
    black = 0;
    for (int i = 0; i < 64; i++) {
        taken |= BIT(i);
        if (data[i] == 'b') {
            black |= BIT(i);
        }
    }
}

