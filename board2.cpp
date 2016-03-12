#include "board2.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cassert>
#include <cmath>

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() : taken(0), black(0), legalMovesComputed(false), legalMoves(0), evaluation(0) {
    OCCUPY_WHITE(3 + 8 * 3, taken, black);
    OCCUPY_BLACK(3 + 8 * 4, taken, black);
    OCCUPY_BLACK(4 + 8 * 3, taken, black);
    OCCUPY_WHITE(4 + 8 * 4, taken, black);
}

/*
 * Make a board given a taken and black uint64_t.
 */
Board::Board(uint64_t t, uint64_t b) : taken(t), black(b), legalMovesComputed(false), legalMoves(0), evaluation(-1) {}

uint64_t Board::findLegalMoves(Side side) {
	legalMoves = 0;
	uint64_t empty = ~taken;
	
	while (empty != 0) {
		int index = __builtin_clzl(empty);
		empty &= ~BIT(index);
		int X = FROM_INDEX_X(index);
		int Y = FROM_INDEX_Y(index);
		/*
				// Check if frontier disc (adds about 1 microsecond)
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
		*/
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
						legalMoves |= BIT(index);
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
	findLegalMoves(side);
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
	//auto start = chrono::high_resolution_clock::now();
	
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
		// int wedgeWeight = 10;
		int cornerWeight = 20;
		int mobilityWeight = 4;
		int penaltyWeight = 3;
		
		// Computations
		findLegalMoves(BLACK);
		int blackMoves = __builtin_popcount(legalMoves) + __builtin_popcount(legalMoves >> 32);
		findLegalMoves(WHITE);
		int whiteMoves = __builtin_popcount(legalMoves) + __builtin_popcount(legalMoves >> 32);
		evaluation = round(mobilityWeight * ((blackMoves + 1) / (double) (whiteMoves + 1) - (whiteMoves + 1) / (double) (blackMoves + 1)));
		// if (evaluation > 20) cerr << whiteMoves << ' ' << blackMoves << endl;
		uint64_t bc = black & CORNERS;
		uint64_t wc = white & CORNERS;
		int blackCorners = __builtin_popcount(bc >> 32) + __builtin_popcount(bc);
		int whiteCorners = __builtin_popcount(wc >> 32) + __builtin_popcount(wc);
		//if (abs(evaluation) > 20 && blackCorners + whiteCorners > 0) {evaluation /= abs(evaluation); evaluation *= 20;} // Mobility worth at most a corner
		evaluation += cornerWeight * blackCorners;
		evaluation -= cornerWeight * whiteCorners;
		
		//cerr << "Legal moves: " << chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
		
		// Idea: There are exceptions where we don't want a penalty
		// Penalty for risky squares if corner not filled
		if (totalCount < 45) {
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
		
		//cerr << "Corners: " << chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
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
	uint64_t white = taken & ~black;
    return __builtin_popcount(white >> 32) + __builtin_popcount(white);
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

bool operator==(const BoardWithSide& l, const BoardWithSide& r) {
	return (l.taken == r.taken) && (l.black == r.black) && (l.side == r.side);
}
