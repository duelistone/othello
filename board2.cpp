#include "board2.h"

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() : taken(0), black(0), legalMovesComputed(false), legalMoves(0), potentialMobility(0), evaluation(0) {
    OCCUPY_WHITE(3 + 8 * 3, taken, black);
    OCCUPY_BLACK(3 + 8 * 4, taken, black);
    OCCUPY_BLACK(4 + 8 * 3, taken, black);
    OCCUPY_WHITE(4 + 8 * 4, taken, black);
    black |= BIT(18) | BIT(19) | BIT(20) | BIT(21) | BIT(26) | BIT(29) | BIT(34) | BIT(37) | BIT(42) | BIT(43) | BIT(44) | BIT(45);
}

/*
 * Make a board given a taken and black uint64_t.
 */
Board::Board(uint64_t t, uint64_t b) : taken(t), black(b), legalMovesComputed(false), legalMoves(0), potentialMobility(0), evaluation(0) {}

uint64_t Board::findLegalMoves(Side side) {
	auto start = chrono::high_resolution_clock::now();
	legalMoves = 0;
	potentialMobility = 0;
	uint64_t empty = black & ~taken;
	
	while (empty != 0) {
		int index = __builtin_clzl(empty);
		empty &= ~BIT(index);
		int X = FROM_INDEX_X(index);
		int Y = FROM_INDEX_Y(index);

		// Potential mobility
		bool increaseMobility = false;
		bool notEdge = !(EDGES & BIT(index)); //index > 8 && index < 55 && temp && (temp != 7);
		
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
				continue;
			}
			else if (ON_BOARD(x, y) && !OCCUPIED(x, y, taken) && notEdge) {
				increaseMobility = true;
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
				continue;
			}
			else if (ON_BOARD(x, y) && !OCCUPIED(x, y, taken) && notEdge) {
				increaseMobility = true;
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
				continue;
			}
			else if (ON_BOARD(x, y) && !OCCUPIED(x, y, taken) && notEdge) {
				increaseMobility = true;
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
				continue;
			}
			else if (ON_BOARD(x, y) && !OCCUPIED(x, y, taken) && notEdge) {
				increaseMobility = true;
			}
		}
		
		dx = 0; dy = 1;
		x = X;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				y += dy;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				legalMoves |= BIT(index);
				continue;
			}
			else if (ON_BOARD(x, y) && !OCCUPIED(x, y, taken) && notEdge) {
				increaseMobility = true;
			}
		}
		
		dx = -1; dy = 0;
		x = X + dx;
		y = Y;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				x += dx;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				legalMoves |= BIT(index);
				continue;
			}
			else if (ON_BOARD(x, y) && !OCCUPIED(x, y, taken) && notEdge) {
				increaseMobility = true;
			}
		}
		
		dx = 0; dy = -1;
		x = X;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				y += dy;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				legalMoves |= BIT(index);
				continue;
			}
			else if (ON_BOARD(x, y) && !OCCUPIED(x, y, taken) && notEdge) {
				increaseMobility = true;
			}
		}
		
		dx = 1; dy = 0;
		x = X + dx;
		y = Y;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				x += dx;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				legalMoves |= BIT(index);
				continue;
			}
			else if (ON_BOARD(x, y) && !OCCUPIED(x, y, taken) && notEdge) {
				increaseMobility = true;
			}
		}
		
		if (increaseMobility) potentialMobility++;
	}
	// cerr << "LM time: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
	legalMovesComputed = true;
	return legalMoves;
}

bool Board::hasLegalMoves(Side side) {
	auto start = chrono::high_resolution_clock::now();
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
				// cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
				return true;
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
				// cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
				return true;
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
				// cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
				return true;
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
				// cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
				return true;
			}
		}
		
		dx = 0; dy = 1;
		x = X + dx;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				y += dy;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				// cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
				return true;
			}
		}
		
		dx = -1; dy = 0;
		x = X + dx;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				x += dx;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				// cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
				return true;
			}
		}
		
		dx = 0; dy = -1;
		x = X + dx;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				y += dy;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				// cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
				return true;
			}
		}
		
		dx = 1; dy = 0;
		x = X + dx;
		y = Y + dy;
		if (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black)) {
			do {
				x += dx;
			} while (ON_BOARD(x, y) && OCCUPIED_SIDE(other, x, y, taken, black));

			if (ON_BOARD(x, y) && OCCUPIED_SIDE(side, x, y, taken, black)) {
				// cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
				return true;
			}
		}
	}
	return false;
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
    
    int dx, dy;
    int x, y, xx, yy;
    uint64_t newtakenCopy, newblackCopy;
    
    dx = -1; dy = -1;
    
            x = X;
            y = Y;
            xx = X + dx;
            yy = Y + dy;
			
			if (ON_BOARD(xx, yy) && !OCCUPIED(xx, yy, taken)) {
				newblack |= BIT(TO_INDEX(xx, yy));
			}
            
            newtakenCopy = newtaken;
            newblackCopy = newblack;
            
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
			
    dx = -1; dy = 1;
    
            x = X;
            y = Y;
            xx = X + dx;
            yy = Y + dy;
			
			if (ON_BOARD(xx, yy) && !OCCUPIED(xx, yy, taken)) {
				newblack |= BIT(TO_INDEX(xx, yy));
			}
            
            newtakenCopy = newtaken;
            newblackCopy = newblack;
            
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
			
    dx = 1; dy = -1;
    
            x = X;
            y = Y;
            xx = X + dx;
            yy = Y + dy;
			
			if (ON_BOARD(xx, yy) && !OCCUPIED(xx, yy, taken)) {
				newblack |= BIT(TO_INDEX(xx, yy));
			}
            
            newtakenCopy = newtaken;
            newblackCopy = newblack;
            
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
			
    dx = 1; dy = 1;
    
    
            x = X;
            y = Y;
            xx = X + dx;
            yy = Y + dy;
			
			if (ON_BOARD(xx, yy) && !OCCUPIED(xx, yy, taken)) {
				newblack |= BIT(TO_INDEX(xx, yy));
			}
            
            newtakenCopy = newtaken;
            newblackCopy = newblack;
            
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
			
    dx = -1; dy = 0;
    
            x = X;
            y = Y;
            xx = X + dx;
            yy = Y + dy;
			
			if (ON_BOARD(xx, yy) && !OCCUPIED(xx, yy, taken)) {
				newblack |= BIT(TO_INDEX(xx, yy));
			}
            
            newtakenCopy = newtaken;
            newblackCopy = newblack;
            
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
			
    dx = 0; dy = -1;
    
    
            x = X;
            y = Y;
            xx = X + dx;
            yy = Y + dy;
			
			if (ON_BOARD(xx, yy) && !OCCUPIED(xx, yy, taken)) {
				newblack |= BIT(TO_INDEX(xx, yy));
			}
            
            newtakenCopy = newtaken;
            newblackCopy = newblack;
            
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
			
    dx = 1; dy = 0;
    
            x = X;
            y = Y;
            xx = X + dx;
            yy = Y + dy;
			
			if (ON_BOARD(xx, yy) && !OCCUPIED(xx, yy, taken)) {
				newblack |= BIT(TO_INDEX(xx, yy));
			}
            
            newtakenCopy = newtaken;
            newblackCopy = newblack;
            
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
			
    dx = 0; dy = 1;
    
            x = X;
            y = Y;
            xx = X + dx;
            yy = Y + dy;
			
			if (ON_BOARD(xx, yy) && !OCCUPIED(xx, yy, taken)) {
				newblack |= BIT(TO_INDEX(xx, yy));
			}
            
            newtakenCopy = newtaken;
            newblackCopy = newblack;
            
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
			
    PLACE_DISC(side, X, Y, newtaken, newblack);
    return Board(newtaken, newblack);
}

/*
 * Evaluation function. findLegalMoves must be called first to get a 
 * nonzero value.
 */
int Board::evaluate() {
	auto start = chrono::high_resolution_clock::now();
	
	int greedyPoint = 60;
	int totalCount = __builtin_popcount(taken >> 32) + __builtin_popcount(taken);
	int blackPM, whitePM; // Potential mobility
	
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
		int potentialMobilityWeight = 1;
		int mobilityBoost = 5;
		int penaltyWeight = 10;
		//int edgePenalty = 5;
		
		// Computations
		findLegalMoves(BLACK);
		blackPM = potentialMobility;
		int blackMoves = __builtin_popcount(legalMoves) + __builtin_popcount(legalMoves >> 32);
		findLegalMoves(WHITE);
		whitePM = potentialMobility;
		// cerr << "Evaluation time LM + PM: " << chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
		int whiteMoves = __builtin_popcount(legalMoves) + __builtin_popcount(legalMoves >> 32);
		evaluation = 15 * mobilityWeight * (blackMoves - whiteMoves) / (blackMoves + whiteMoves + 2); //Iago mobility evaluation //round(mobilityWeight * ((blackMoves + 1) / (double) (whiteMoves + 1) - (whiteMoves + 1) / (double) (blackMoves + 1)));
		if (abs(evaluation) > MOBILITY_CAP) evaluation = (evaluation > 0) ? MOBILITY_CAP : -MOBILITY_CAP; // Cap the influence of mobility
		if (whiteMoves < 3 && blackMoves >= 5) evaluation += mobilityBoost;
		if (whiteMoves < 2) evaluation += mobilityBoost;
		if (whiteMoves == 0) evaluation += mobilityBoost;
		if (blackMoves < 3 && whiteMoves >= 5) evaluation -= mobilityBoost;
		if (blackMoves < 2) evaluation -= mobilityBoost;
		if (blackMoves == 0) evaluation -= mobilityBoost;
		
		// Potential mobility
		int temp = 10 * potentialMobilityWeight * (blackPM - whitePM) / (blackPM + whitePM + 2);
		if (abs(temp) > PM_CAP) temp = (temp > 0) ? PM_CAP : -PM_CAP;
		evaluation += temp;
		
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
		/*
		if (totalCount < 45) {
			// Edge penalties
			if ((INNER_EDGE_TOP & b) != 0) evaluation -= edgePenalty;
			if ((INNER_EDGE_BOTTOM & b) != 0) evaluation -= edgePenalty;
			if ((INNER_EDGE_LEFT & b) != 0) evaluation -= edgePenalty;
			if ((INNER_EDGE_RIGHT & b) != 0) evaluation -= edgePenalty;
			
			if ((INNER_EDGE_TOP & white) != 0) evaluation += edgePenalty;
			if ((INNER_EDGE_BOTTOM & white) != 0) evaluation += edgePenalty;
			if ((INNER_EDGE_LEFT & white) != 0) evaluation += edgePenalty;
			if ((INNER_EDGE_RIGHT & white) != 0) evaluation += edgePenalty;
		}
		*/
		if (totalCount < 40) {
			// Minimize discs early
			int discdiff = (countWhite() - countBlack());
		    if (abs(discdiff) > 15) discdiff = (discdiff > 0) ? 15 : -15;
		    evaluation += discdiff / 4;
		}
		if (totalCount > 20) {
			// Get top edge into uint16
			uint16_t u16 = ((taken >> 56) << 8) | (b >> 56);
			evaluation += EDGE_VALUES[u16];
			
			u16 = ((taken << 56) >> 48) | ((b << 56) >> 56);
			evaluation += EDGE_VALUES[u16];
			
			// Get left edge
			uint64_t tempTaken = taken & EDGE_LEFT;
			uint64_t tempB = b & EDGE_LEFT;
			tempTaken = tempTaken >> 7;
			tempTaken = (tempTaken | (tempTaken >> 1)) & 0x3333333333333333ull;
			tempTaken = (tempTaken | (tempTaken >> 2)) & 0x0f0f0f0f0f0f0f0full;
			tempTaken = (tempTaken | (tempTaken >> 4)) & 0x00ff00ff00ff00ffull;
			tempTaken = (tempTaken | (tempTaken >> 8)) & 0x0000ffff0000ffffull;
			tempTaken = (tempTaken | (tempTaken >> 16)) & 0x00000000ffffffffull;
			tempTaken = (tempTaken | (tempTaken >> 1)) & 0x0000000033333333ull;
			tempTaken = (tempTaken | (tempTaken >> 2)) & 0x000000000f0f0f0full;
			tempTaken = (tempTaken | (tempTaken >> 4)) & 0x0000000000ff00ffull;
			tempTaken = (tempTaken | (tempTaken >> 8)) & 0x000000000000ffffull;
			tempTaken = (tempTaken | (tempTaken >> 1)) & 0x0000000000003333ull;
			tempTaken = (tempTaken | (tempTaken >> 2)) & 0x0000000000000f0full;
			tempTaken = (tempTaken | (tempTaken >> 4)) & 0x00000000000000ffull;
			tempB = tempB >> 7;
			tempB = (tempB | (tempB >> 1)) & 0x3333333333333333ull;
			tempB = (tempB | (tempB >> 2)) & 0x0f0f0f0f0f0f0f0full;
			tempB = (tempB | (tempB >> 4)) & 0x00ff00ff00ff00ffull;
			tempB = (tempB | (tempB >> 8)) & 0x0000ffff0000ffffull;
			tempB = (tempB | (tempB >> 16)) & 0x00000000ffffffffull;
			tempB = (tempB | (tempB >> 1)) & 0x0000000033333333ull;
			tempB = (tempB | (tempB >> 2)) & 0x000000000f0f0f0full;
			tempB = (tempB | (tempB >> 4)) & 0x0000000000ff00ffull;
			tempB = (tempB | (tempB >> 8)) & 0x000000000000ffffull;
			tempB = (tempB | (tempB >> 1)) & 0x0000000000003333ull;
			tempB = (tempB | (tempB >> 2)) & 0x0000000000000f0full;
			tempB = (tempB | (tempB >> 4)) & 0x00000000000000ffull;
			u16 = (tempTaken << 8) | tempB;
			
			/*
			uint16_t u16_2 = 0;
			for (unsigned int i = 0; i < 64; i+=8) {
				if ((taken & BIT(i)) == BIT(i)) {
					u16_2 |= EDGE_BIT(i >> 3);
					if ((b & BIT(i)) == BIT(i)) u16_2 |= EDGE_BIT(8 + i / 8);
				}
			}
			bitset<16> bs(u16), bs2(u16_2);
			cerr << bs << ' ' << bs2 << endl;*/
			evaluation += EDGE_VALUES[u16];
			
			// Get right edge
			tempTaken = taken & EDGE_RIGHT;
			tempB = b & EDGE_RIGHT;
			tempTaken = (tempTaken | (tempTaken >> 1)) & 0x3333333333333333ull;
			tempTaken = (tempTaken | (tempTaken >> 2)) & 0x0f0f0f0f0f0f0f0full;
			tempTaken = (tempTaken | (tempTaken >> 4)) & 0x00ff00ff00ff00ffull;
			tempTaken = (tempTaken | (tempTaken >> 8)) & 0x0000ffff0000ffffull;
			tempTaken = (tempTaken | (tempTaken >> 16)) & 0x00000000ffffffffull;
			tempTaken = (tempTaken | (tempTaken >> 1)) & 0x0000000033333333ull;
			tempTaken = (tempTaken | (tempTaken >> 2)) & 0x000000000f0f0f0full;
			tempTaken = (tempTaken | (tempTaken >> 4)) & 0x0000000000ff00ffull;
			tempTaken = (tempTaken | (tempTaken >> 8)) & 0x000000000000ffffull;
			tempTaken = (tempTaken | (tempTaken >> 1)) & 0x0000000000003333ull;
			tempTaken = (tempTaken | (tempTaken >> 2)) & 0x0000000000000f0full;
			tempTaken = (tempTaken | (tempTaken >> 4)) & 0x00000000000000ffull;
			tempB = (tempB | (tempB >> 1)) & 0x3333333333333333ull;
			tempB = (tempB | (tempB >> 2)) & 0x0f0f0f0f0f0f0f0full;
			tempB = (tempB | (tempB >> 4)) & 0x00ff00ff00ff00ffull;
			tempB = (tempB | (tempB >> 8)) & 0x0000ffff0000ffffull;
			tempB = (tempB | (tempB >> 16)) & 0x00000000ffffffffull;
			tempB = (tempB | (tempB >> 1)) & 0x0000000033333333ull;
			tempB = (tempB | (tempB >> 2)) & 0x000000000f0f0f0full;
			tempB = (tempB | (tempB >> 4)) & 0x0000000000ff00ffull;
			tempB = (tempB | (tempB >> 8)) & 0x000000000000ffffull;
			tempB = (tempB | (tempB >> 1)) & 0x0000000000003333ull;
			tempB = (tempB | (tempB >> 2)) & 0x0000000000000f0full;
			tempB = (tempB | (tempB >> 4)) & 0x00000000000000ffull;
			u16 = (tempTaken << 8) | tempB;
			/*
			uint16_t u16_2 = 0;
			for (unsigned int i = 7; i < 64; i+=8) {
				if ((taken & BIT(i)) == BIT(i)) {
					u16_2 |= EDGE_BIT(i >> 3);
					if ((b & BIT(i)) == BIT(i)) u16_2 |= EDGE_BIT(8 + (i >> 3));
				}
			}
			bitset<16> bs(u16), bs2(u16_2);
			cerr << bs << ' ' << bs2 << endl;*/
			evaluation += EDGE_VALUES[u16];
		}
	}
	else {
		// Become greedy in the end
		evaluation = countBlack() - countWhite();
	}
	//if (totalCount > 20) cerr << "Evaluation time: " << chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
	return evaluation;
}

int Board::pos_evaluate() {
	auto start = chrono::high_resolution_clock::now();
	int evaluation; // Don't touch the evaluation member variable here
	
	int greedyPoint = 60;
	int totalCount = __builtin_popcount(taken >> 32) + __builtin_popcount(taken);
	
	// Game over if no discs left
	if (totalCount == countBlack()) return INT_MAX;
	else if (totalCount == countWhite()) return INT_MIN;
	
	uint64_t b = taken & black;
	uint64_t white = taken & ~black;
	if (totalCount < greedyPoint) {
		// Constants to tweak
		int penaltyWeight = 10;
		int edgePenalty = 5;
		
		// Computations
		evaluation = 0;
		
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
			// Edge penalties
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
			// Minimize discs early
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
	//cerr << chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
	
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
