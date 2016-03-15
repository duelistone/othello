#include "board2.h"

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() : taken(0), black(0), legalMovesComputed(false), legalMoves(0), evaluation(0) {
    OCCUPY_WHITE(3 + 8 * 3, taken, black);
    OCCUPY_BLACK(3 + 8 * 4, taken, black);
    OCCUPY_BLACK(4 + 8 * 3, taken, black);
    OCCUPY_WHITE(4 + 8 * 4, taken, black);
    black |= BIT(18) | BIT(19) | BIT(20) | BIT(21) | BIT(26) | BIT(29) | BIT(34) | BIT(37) | BIT(42) | BIT(43) | BIT(44) | BIT(45);
}

/*
 * Make a board given a taken and black uint64_t.
 */
Board::Board(uint64_t t, uint64_t b) : taken(t), black(b), legalMovesComputed(false), legalMoves(0), evaluation(-1) {}

uint64_t Board::findLegalMoves(Side side) {
	legalMoves = 0;
	uint64_t empty = black & ~taken;
	
	while (empty != 0) {
		int index = __builtin_clzl(empty);
		empty &= ~BIT(index);
		int X = FROM_INDEX_X(index);
		int Y = FROM_INDEX_Y(index);

		// Else, check if legal move
		Side other = OTHER_SIDE(side);
		
		int dx, dy, x, y;
		
		dx = -1; dy = -1;
		x = X + dx;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				x += dx;
				y += dy;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				legalMoves |= BIT(index);
			}
		}
		
		dy = 1;
		x = X + dx;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				x += dx;
				y += dy;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				legalMoves |= BIT(index);
			}
		}
		
		dx = 1; dy = -1;
		x = X + dx;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				x += dx;
				y += dy;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				legalMoves |= BIT(index);
			}
		}
		
		dx = 1; dy = 1;
		x = X + dx;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				x += dx;
				y += dy;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				legalMoves |= BIT(index);
			}
		}
		
		dx = 0; dy = 1;
		x = X + dx;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				x += dx;
				y += dy;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				legalMoves |= BIT(index);
			}
		}
		
		dx = -1; dy = 0;
		x = X + dx;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				x += dx;
				y += dy;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				legalMoves |= BIT(index);
			}
		}
		
		dx = 0; dy = -1;
		x = X + dx;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				x += dx;
				y += dy;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				legalMoves |= BIT(index);
			}
		}
		
		dx = 1; dy = 0;
		x = X + dx;
		y = Y + dy;
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
            int xx = X + dx;
            int yy = Y + dy;
            
            // For legal moves function
			// A square adjacent to an unoccupied square should be 1 in black
			if (ON_BOARD(xx, yy) && !OCCUPIED(xx, yy, taken)) {
				black |= BIT(TO_INDEX(xx, yy));
			}
			
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
            int xx = X + dx;
            int yy = Y + dy;
			
			if (ON_BOARD(xx, yy) && !OCCUPIED(xx, yy, taken)) {
				newblack |= BIT(TO_INDEX(xx, yy));
			}
            
            uint64_t newtakenCopy = newtaken;
            uint64_t newblackCopy = newblack;
            
            do {
                x += dx;
                y += dy;
                if (!(ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black))) break;
                PLACE_DISC(side, x, y, newtakenCopy, newblackCopy);
            } while (true);

            if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
                newtaken = newtakenCopy;
                newblack = newblackCopy;
            }
        }
    }
    
    PLACE_DISC(side, X, Y, newtaken, newblack);
    Board b(*this);
    b.doMove(X, Y, side);
    return Board(newtaken, newblack);
}

/*
 * Evaluation function. findLegalMoves must be called first to get a 
 * nonzero value.
 */
int Board::evaluate() {
	// auto start = chrono::high_resolution_clock::now();
	
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
	
	uint64_t b = taken & black;
	uint64_t white = taken & ~black;
	if (totalCount < greedyPoint) {
		// Constants to tweak
		int mobilityWeight = 4;
		int mobilityBoost = 5;
		int penaltyWeight = 10;
		int edgePenalty = 5;
		
		// Computations
		findLegalMoves(BLACK);
		int blackMoves = __builtin_popcount(legalMoves) + __builtin_popcount(legalMoves >> 32);
		findLegalMoves(WHITE);
		int whiteMoves = __builtin_popcount(legalMoves) + __builtin_popcount(legalMoves >> 32);
		evaluation = mobilityWeight * (blackMoves - whiteMoves);//round(mobilityWeight * ((blackMoves + 1) / (double) (whiteMoves + 1) - (whiteMoves + 1) / (double) (blackMoves + 1)));
		if (abs(evaluation) > 20) evaluation = (evaluation > 0) ? 20 : -20; // Cap the influence of mobility
		if (whiteMoves < 3 && blackMoves >= 5) evaluation += mobilityBoost;
		if (blackMoves < 3 && whiteMoves >= 5) evaluation -= mobilityBoost;
		
		// Idea: There are exceptions where we don't want a penalty
		// Penalty for risky squares if corner not filled
		if (!(taken & CORNER_TL)) {
			if (BIT(9) & b) evaluation -= penaltyWeight;
			if (BIT(9) & white) evaluation += penaltyWeight;
		}
		if (!(taken & CORNER_TR)) {
			if (BIT(14) & b) evaluation -= penaltyWeight;
			if (BIT(14) & white) evaluation += penaltyWeight;
		}
		if (!(taken & CORNER_BL)) {
			if (BIT(49) & b) evaluation -= penaltyWeight;
			if (BIT(49) & white) evaluation += penaltyWeight;
		}
		if (!(taken & CORNER_BR)) {
			if (BIT(54) & b) evaluation -= penaltyWeight;
			if (BIT(54) & white) evaluation += penaltyWeight;
		}
		
		if (totalCount < 45) {
			// Corners
						
			if ((INNER_EDGE_TOP & b) != 0) evaluation -= edgePenalty;
			if ((INNER_EDGE_BOTTOM & b) != 0) evaluation -= edgePenalty;
			if ((INNER_EDGE_LEFT & b) != 0) evaluation -= edgePenalty;
			if ((INNER_EDGE_RIGHT & b) != 0) evaluation -= edgePenalty;
			
			if ((INNER_EDGE_TOP & white) != 0) evaluation += edgePenalty;
			if ((INNER_EDGE_BOTTOM & white) != 0) evaluation += edgePenalty;
			if ((INNER_EDGE_LEFT & white) != 0) evaluation += edgePenalty;
			if ((INNER_EDGE_RIGHT & white) != 0) evaluation += edgePenalty;
			
		}
		
		if (totalCount < 40) {
			int discdiff = (countWhite() - countBlack());
		    if (abs(discdiff) > 15) discdiff = (discdiff > 0) ? 15 : -15;
		    evaluation += discdiff / 5;
		}
		if (totalCount > 20) {
			// Get top edge into uint16
			uint16_t u16 = ((taken >> 56) << 8) | (b >> 56);
			if (taken & BIT(1)) assert(u16 & EDGE_BIT(1));
			evaluation += EDGE_VALUES[u16];
			
			u16 = ((taken << 56) >> 48) | ((b << 56) >> 56);
			evaluation += EDGE_VALUES[u16];
			
			// Get left edge
			u16 = 0;
			for (int i = 0; i < 64; i+=8) {
				if ((taken | BIT(i)) == BIT(i)) {
					u16 |= EDGE_BIT(i / 8);
					if ((b | BIT(i)) == BIT(i)) u16 |= EDGE_BIT(8 + i / 8);
				}
			}
			evaluation += EDGE_VALUES[u16];
			
			// Get right edge
			u16 = 0;
			for (int i = 7; i < 64; i+=8) {
				if ((taken | BIT(i)) == BIT(i)) {
					u16 |= EDGE_BIT(i / 8);
					if ((b | BIT(i)) == BIT(i)) u16 |= EDGE_BIT(8 + i / 8);
				}
			}
			evaluation += EDGE_VALUES[u16];
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
	uint64_t b = black & taken;
    return __builtin_popcount(b >> 32) + __builtin_popcount(b);
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
