#include "board2.h"
#include <iostream>


/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() : taken(0), black(0), legalMovesComputed(false), legalMoves(0), evaluation(-1), frontierDiscs(0) {
    OCCUPY_WHITE(3 + 8 * 3, taken, black);
    OCCUPY_BLACK(3 + 8 * 4, taken, black);
    OCCUPY_BLACK(4 + 8 * 3, taken, black);
    OCCUPY_WHITE(4 + 8 * 4, taken, black);
}

/*
 * Make a board given a taken and black uint64_t.
 */
Board::Board(uint64_t t, uint64_t b) : taken(t), black(b), legalMovesComputed(false), legalMoves(0), evaluation(-1), frontierDiscs(0) {}

uint64_t Board::findLegalMoves(Side side) {
	legalMoves = 0;
	frontierDiscs = 0;
	
	for (int X = 0; X < 8; X++) {
		for (int Y = 0; Y < 8; Y++) {
			// Make sure the square hasn't already been taken.
			if (OCCUPIED(X, Y, taken)) {
				// Check if frontier disc
				// Don't count edge discs as frontier discs (simplification)
				int index = TO_INDEX(X, Y);
				if (index < 8 || index > 56 || index % 8 == 0 || index % 8 == 7) {
					continue;
				}
				for (int dx = -1; dx <= 1; dx++) {
					for (int dy = -1; dy <= 1; dy++) {
						if (dx == 0 && dy == 0) continue;
						int x = X + dx;
						int y = Y + dy;
						if (ON_BOARD(x, y) && !OCCUPIED(x, y, taken)) {
							frontierDiscs++;
						}
					}
				}
				continue;
			}
			
			// Else, check if legal move
			Side other = OTHER_SIDE(side);
			for (int dx = -1; dx <= 1; dx++) {
				for (int dy = -1; dy <= 1; dy++) {
					if (dy == 0 && dx == 0) continue;

					// Is there a capture in that direction?
					int x = X + dx;
					int y = Y + dy;
					if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
						do {
							x += dx;
							y += dy;
						} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

						if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
							//cerr << X << ' ' << Y << " is valid " << dx << ' ' << dy << ' ' << x << ' ' << y << endl;
							legalMoves |= BIT(TO_INDEX(X, Y));
						}
					}
				}
			}
		}
	}
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
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
                do {
                    x += dx;
                    y += dy;
                } while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

                if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
					//cerr << X << ' ' << Y << " is valid " << dx << ' ' << dy << ' ' << x << ' ' << y << endl;
					return true;
				}
            }
        }
    }
    return false;
}

/*
 * Modifies the board to reflect the specified move.
 */
void Board::doMove(int X, int Y, Side side) {
    // A NULL move means pass.
    if (X == -1) return;

    // Ignore if move is invalid.
    // Might be removed later for efficiency
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

Board Board::doMoveOnNewBoard(int X, int Y, Side side) {
	// A NULL move means pass.
    if (X == -1) return Board(taken, black);

    // Ignore if move is invalid.
    // Might be removed later for efficiency
    if (!checkMove(X, Y, side)) return Board(taken, black);
	
	uint64_t newtaken = taken;
	uint64_t newblack = black;
	
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
                    PLACE_DISC(side, x, y, newtaken, newblack);
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    PLACE_DISC(side, X, Y, newtaken, newblack);
    return Board(newtaken, newblack);
}

/*
 * Evaluation function. findLegalMoves must be called first to get a 
 * nonzero value.
 */
int Board::evaluate() {
	int greedyPoint = 60;
	int totalCount = __builtin_popcount(taken >> 32) + __builtin_popcount(taken);
	
	// Game over if no discs left
	if (totalCount == countBlack()) {
		evaluation = INT_MAX;
		return evaluation;
	}
	else if (totalCount == countWhite()) {
		evaluation = INT_MIN;
		return evaluation;
	}
	
	uint64_t white = taken & ~black;
	if (totalCount < greedyPoint) {
		// Constants to tweak
		int cornerWeight = 90 - totalCount;
		int frontierDiscWeight = 2;
		int penaltyWeight = 10;
		int wedgeWeight = 10;
		
		// Computations
		findLegalMoves(BLACK);
		evaluation = __builtin_popcount(legalMoves) + __builtin_popcount(legalMoves >> 32) - frontierDiscWeight * frontierDiscs;
		findLegalMoves(WHITE);
		evaluation -= __builtin_popcount(legalMoves) + __builtin_popcount(legalMoves >> 32) - frontierDiscWeight * frontierDiscs;
		uint64_t bc = black & CORNERS;
		uint64_t wc = white & CORNERS;
		evaluation += cornerWeight * (__builtin_popcount(bc >> 32) + __builtin_popcount(bc));
		evaluation -= cornerWeight * (__builtin_popcount(bc >> 32) + __builtin_popcount(wc));
		
		// Idea: There are exceptions where we don't want a penalty
		// Penalty for risky squares if corner not filled
		if (totalCount < 50) {
			if ((CORNER_TL & taken) == 0) {
				evaluation -= (black & NEXT_TO_CORNER_TL) ? penaltyWeight : 0;
				evaluation += (white & NEXT_TO_CORNER_TL) ? penaltyWeight : 0;
			}
			if ((CORNER_TR & taken) == 0) {
				evaluation -= (black & NEXT_TO_CORNER_TR) ? penaltyWeight : 0;
				evaluation += (white & NEXT_TO_CORNER_TR) ? penaltyWeight : 0;
			}
			if ((CORNER_BL & taken) == 0) {
				evaluation -= (black & NEXT_TO_CORNER_BL) ? penaltyWeight : 0;
				evaluation += (white & NEXT_TO_CORNER_BL) ? penaltyWeight : 0;
			}
			if ((CORNER_BR & taken) == 0) {
				evaluation -= (black & NEXT_TO_CORNER_BR) ? penaltyWeight : 0;
				evaluation += (white & NEXT_TO_CORNER_BR) ? penaltyWeight : 0;
			}
		}
		
		if (totalCount < 56) {
			// Wedges
			for (int i = 1; i <= 6; i++) {
				// Check if occupied adjacent squares
				if ((taken & (BIT(i - 1) | BIT(i + 1))) == (BIT(i - 1) | BIT(i + 1))) {
					if ((black & BIT(i)) == 0 && (black & (BIT(i - 1) | BIT(i + 1))) == (BIT(i - 1) | BIT(i + 1))) {
						evaluation -= wedgeWeight;
					}
					else if ((white & BIT(i)) == 0 && (white & (BIT(i - 1) | BIT(i + 1))) == (BIT(i - 1) | BIT(i + 1))) {
						evaluation += wedgeWeight;
					}
				}
			}
			for (int i = 57; i <= 62; i++) {
				if ((taken & (BIT(i - 1) | BIT(i + 1))) == (BIT(i - 1) | BIT(i + 1))) {
					if ((black & BIT(i)) == 0 && (black & (BIT(i - 1) | BIT(i + 1))) == (BIT(i - 1) | BIT(i + 1))) {
						evaluation -= wedgeWeight;
					}
					else if ((white & BIT(i)) == 0 && (white & (BIT(i - 1) | BIT(i + 1))) == (BIT(i - 1) | BIT(i + 1))) {
						evaluation += wedgeWeight;
					}
				}
			}
			for (int i = 8; i <= 48; i += 8) {
				if ((taken & (BIT(i - 8) | BIT(i + 8))) == (BIT(i - 8) | BIT(i + 8))) {
					if ((black & BIT(i)) == 0 && (black & (BIT(i - 8) | BIT(i + 8))) == (BIT(i - 8) | BIT(i + 8))) {
						evaluation -= wedgeWeight;
					}
					else if ((white & BIT(i)) == 0 && (white & (BIT(i - 8) | BIT(i + 8))) == (BIT(i - 8) | BIT(i + 8))) {
						evaluation += wedgeWeight;
					}
				}
			}
			for (int i = 15; i <= 55; i += 8) {
				if ((taken & (BIT(i - 8) | BIT(i + 8))) == (BIT(i - 8) | BIT(i + 8))) {
					if ((black & BIT(i)) == 0 && (black & (BIT(i - 8) | BIT(i + 8))) == (BIT(i - 8) | BIT(i + 8))) {
						evaluation -= wedgeWeight;
					}
					else if ((white & BIT(i)) == 0 && (white & (BIT(i - 8) | BIT(i + 8))) == (BIT(i - 8) | BIT(i + 8))) {
						evaluation += wedgeWeight;
					}
				}
			}
		}
		else {
			// Disc count should start mattering a little more at this point
			evaluation += 2 * (countBlack() - countWhite());
		}
	}
	else {
		// Become greedy in the end
		evaluation = countBlack() - countWhite();
	}
	return evaluation;
}

/*
 * Pure greedy evaluation for test
 */
int Board::evaluateTest() {
	evaluation = countBlack() - countWhite();
	return evaluation;
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
    return __builtin_popcount(black >> 32) + __builtin_popcount(black);
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return __builtin_popcount(taken >> 32) + __builtin_popcount(taken) - __builtin_popcount(black >> 32) - __builtin_popcount(black);
}

/*
 * Sets the board state given an 8x8 char array where 'w' indicates a white
 * piece and 'b' indicates a black piece. Mainly for testing purposes.
 */
void Board::setBoard(char data[]) {
    taken = 0;
    black = 0;
    for (int i = 0; i < 64; i++) {
        if (data[i] != ' ') taken |= BIT(i);
		if (data[i] == 'b') black |= BIT(i);
	}
}

