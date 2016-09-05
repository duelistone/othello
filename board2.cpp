#include "board2.h"

#define USE_MAGIC 1
#define X_SQUARE_PENALTY 1

// rotate bitboard 90° to the left
uint64_t reflect(uint64_t b) {
    uint64_t t; // temporary
    // reflect b against diagonal line going through bits 1<<7 and 1<<56
    t = (b ^ (b >> 63)) & 0x0000000000000001; b ^= t ^ (t << 63);
    t = (b ^ (b >> 54)) & 0x0000000000000102; b ^= t ^ (t << 54);
    t = (b ^ (b >> 45)) & 0x0000000000010204; b ^= t ^ (t << 45);
    t = (b ^ (b >> 36)) & 0x0000000001020408; b ^= t ^ (t << 36);
    t = (b ^ (b >> 27)) & 0x0000000102040810; b ^= t ^ (t << 27);
    t = (b ^ (b >> 18)) & 0x0000010204081020; b ^= t ^ (t << 18);
    t = (b ^ (b >>  9)) & 0x0001020408102040; b ^= t ^ (t <<  9);
        
    return b;
}

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() : taken(0), black(0) {
    OCCUPY_WHITE(3 + 8 * 3, taken, black);
    OCCUPY_BLACK(3 + 8 * 4, taken, black);
    OCCUPY_BLACK(4 + 8 * 3, taken, black);
    OCCUPY_WHITE(4 + 8 * 4, taken, black);
    zobrist_hash = make_zobrist_hash();
}

/*
 * Make a board given a taken and black uint64_t.
 */
Board::Board(uint64_t t, uint64_t b) : taken(t), black(b), zobrist_hash(make_zobrist_hash()) {}
Board::Board(uint64_t t, uint64_t b, size_t z) : taken(t), black(b), zobrist_hash(z) {}

size_t Board::make_zobrist_hash(const Side &s) const {
	uint64_t blackCopy = black;
	uint64_t whiteCopy = taken & ~black;
	size_t result = 0;
	
	while (blackCopy) {
		int index = __builtin_clzl(blackCopy);
		blackCopy ^= BIT(index);
		result ^= random_numbers[index];
	}
	while (whiteCopy) {
		int index = __builtin_clzl(whiteCopy);
		whiteCopy ^= BIT(index);
		result ^= random_numbers[index + 64];
	}
	
	return result;
}

uint64_t doMove(uint64_t taken, uint64_t black, Side side, int index) {
	uint64_t white = ~black & taken;
	const uint64_t bi = SINGLE_BIT[index];
	uint64_t n = bi;
	
	if (side == BLACK) {
		// LEFT
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		//~ n |= white & LEFT_FILTER & (n << 1);
		if (black & LEFT_FILTER & (n << 1)) black |= n;
		
		// RIGHT
		n = bi;
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		//~ n |= white & RIGHT_FILTER & (n >> 1);
		if (black & RIGHT_FILTER & (n >> 1)) black |= n;
		
		// DOWN
		n = bi;
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		//~ n |= white & DOWN_FILTER & (n >> 8);
		if (black & DOWN_FILTER & (n >> 8)) black |= n;
		
		// UP
		n = bi;
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		//~ n |= white & UP_FILTER & (n << 8);
		if (black & UP_FILTER & (n << 8)) black |= n;
		
		// UP_LEFT
		n = bi;
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		//~ n |= white & UP_LEFT_FILTER & (n << 9);
		if (black & UP_LEFT_FILTER & (n << 9)) black |= n;
		
		// DOWN_RIGHT
		n = bi;
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		//~ n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		if (black & DOWN_RIGHT_FILTER & (n >> 9)) black |= n;
		
		// UP_RIGHT
		n = bi;
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		//~ n |= white & UP_RIGHT_FILTER & (n << 7);
		if (black & UP_RIGHT_FILTER & (n << 7)) black |= n;
		
		// DOWN_LEFT
		n = bi;
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		//~ n |= white & DOWN_LEFT_FILTER & (n >> 7);
		if (black & DOWN_LEFT_FILTER & (n >> 7)) black |= n;
		
	}
	else {
		// LEFT
		n |= black & LEFT_FILTER & (n << 1);
		n |= black & LEFT_FILTER & (n << 1);
		n |= black & LEFT_FILTER & (n << 1);
		n |= black & LEFT_FILTER & (n << 1);
		n |= black & LEFT_FILTER & (n << 1);
		n |= black & LEFT_FILTER & (n << 1);
		//~ n |= black & LEFT_FILTER & (n << 1);
		if (white & LEFT_FILTER & (n << 1)) black &= ~n;
		
		// RIGHT
		n = bi;
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		//~ n |= black & RIGHT_FILTER & (n >> 1);
		if (white & RIGHT_FILTER & (n >> 1)) black &= ~n;
		
		// DOWN
		n = bi;
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		//~ n |= black & DOWN_FILTER & (n >> 8);
		if (white & DOWN_FILTER & (n >> 8)) black &= ~n;
		
		// UP
		n = bi;
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		//~ n |= black & UP_FILTER & (n << 8);
		if (white & UP_FILTER & (n << 8)) black &= ~n;
		
		// UP_LEFT
		n = bi;
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		//~ n |= black & UP_LEFT_FILTER & (n << 9);
		if (white & UP_LEFT_FILTER & (n << 9)) black &= ~n;
		
		// DOWN_RIGHT
		n = bi;
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		//~ n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		if (white & DOWN_RIGHT_FILTER & (n >> 9)) black &= ~n;
		
		// UP_RIGHT
		n = bi;
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		//~ n |= black & UP_RIGHT_FILTER & (n << 7);
		if (white & UP_RIGHT_FILTER & (n << 7)) black &= ~n;
		
		// DOWN_LEFT
		n = bi;
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		//~ n |= black & DOWN_LEFT_FILTER & (n >> 7);
		if (white & DOWN_LEFT_FILTER & (n >> 7)) black &= ~n;
	}
	return black;
}

uint64_t Board::findLegalMoves(const Side &side) const {
	//~ auto start = chrono::high_resolution_clock::now();
	
	uint64_t moves = 0;
	uint64_t white = ~black & taken;
	uint64_t w, opp, own, t;
	
	// Set opponent
	if (side == BLACK) {
		own = black;
		opp = white;
	}
	else {
		own = white;
		opp = black;
	}
	
	// RIGHT
	w = opp & RIGHT_FILTER;
	t = w & (own >> 1);
	t |= (w & (t >> 1));
	t |= (w & (t >> 1));
	t |= (w & (t >> 1));
	t |= (w & (t >> 1));
	t |= (w & (t >> 1));
	//~ t |= (w & (t >> 1));
	moves |= RIGHT_FILTER & (t >> 1);
	
	// LEFT
	w = opp & LEFT_FILTER;
	t = w & (own << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	//~ t |= w & (t << 1);
	
	moves |= LEFT_FILTER & (t << 1);
	
	// DOWN
	w = opp & DOWN_FILTER;
	t = w & (own >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	//~ t |= w & (t >> 8);
	
	moves |= DOWN_FILTER & (t >> 8);
	
	// UP
	w = opp & UP_FILTER;
	t = w & (own << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	//~ t |= w & (t << 8);
	
	moves |= UP_FILTER & (t << 8);
	
	// UP_LEFT
	w = opp & UP_LEFT_FILTER;
	t = w & (own << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	//~ t |= w & (t << 9);
	
	moves |= UP_LEFT_FILTER & (t << 9);
	
	// DOWN_RIGHT
	w = opp & DOWN_RIGHT_FILTER;
	t = w & (own >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	//~ t |= w & (t >> 9);
	
	moves |= DOWN_RIGHT_FILTER & (t >> 9);
	
	// UP_RIGHT
	w = opp & UP_RIGHT_FILTER;
	t = w & (own << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	//~ t |= w & (t << 7);
	
	moves |= UP_RIGHT_FILTER & (t << 7);
	
	// DOWN_LEFT
	w = opp & DOWN_LEFT_FILTER;
	t = w & (own >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	//~ t |= w & (t >> 7);
	
	moves |= DOWN_LEFT_FILTER & (t >> 7);
	
	//~ cerr << "LM time: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<< endl;
	return moves & ~taken;
}

uint64_t Board::findLegalMovesBlack() const {
	//~ auto start = chrono::high_resolution_clock::now();
	
	uint64_t moves = 0;
	uint64_t white = ~black & taken;
	uint64_t w, t;
	
	// RIGHT
	w = white & RIGHT_FILTER;
	t = w & (black >> 1);
	t |= (w & (t >> 1));
	t |= (w & (t >> 1));
	t |= (w & (t >> 1));
	t |= (w & (t >> 1));
	t |= (w & (t >> 1));
	//~ t |= (w & (t >> 1));
	moves |= RIGHT_FILTER & (t >> 1);
	
	// LEFT
	w = white & LEFT_FILTER;
	t = w & (black << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	//~ t |= w & (t << 1);
	
	moves |= LEFT_FILTER & (t << 1);
	
	// DOWN
	w = white & DOWN_FILTER;
	t = w & (black >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	//~ t |= w & (t >> 8);
	
	moves |= DOWN_FILTER & (t >> 8);
	
	// UP
	w = white & UP_FILTER;
	t = w & (black << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	//~ t |= w & (t << 8);
	
	moves |= UP_FILTER & (t << 8);
	
	// UP_LEFT
	w = white & UP_LEFT_FILTER;
	t = w & (black << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	//~ t |= w & (t << 9);
	
	moves |= UP_LEFT_FILTER & (t << 9);
	
	// DOWN_RIGHT
	w = white & DOWN_RIGHT_FILTER;
	t = w & (black >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	//~ t |= w & (t >> 9);
	
	moves |= DOWN_RIGHT_FILTER & (t >> 9);
	
	// UP_RIGHT
	w = white & UP_RIGHT_FILTER;
	t = w & (black << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	//~ t |= w & (t << 7);
	
	moves |= UP_RIGHT_FILTER & (t << 7);
	
	// DOWN_LEFT
	w = white & DOWN_LEFT_FILTER;
	t = w & (black >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	//~ t |= w & (t >> 7);
	
	moves |= DOWN_LEFT_FILTER & (t >> 7);
	
	//~ cerr << "LM time: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<< endl;
	return moves & ~taken;
}

uint64_t Board::findLegalMovesWhite() const {
	//~ auto start = chrono::high_resolution_clock::now();
	uint64_t moves = 0;
	uint64_t white = ~black & taken;
	uint64_t w, t;
	
	// RIGHT
	w = black & RIGHT_FILTER;
	t = w & (white >> 1);
	t |= (w & (t >> 1));
	t |= (w & (t >> 1));
	t |= (w & (t >> 1));
	t |= (w & (t >> 1));
	t |= (w & (t >> 1));
	//~ t |= (w & (t >> 1));
	moves |= RIGHT_FILTER & (t >> 1);
	
	// LEFT
	w = black & LEFT_FILTER;
	t = w & (white << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	//~ t |= w & (t << 1);
	
	moves |= LEFT_FILTER & (t << 1);
	
	// DOWN
	w = black & DOWN_FILTER;
	t = w & (white >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	//~ t |= w & (t >> 8);
	
	moves |= DOWN_FILTER & (t >> 8);
	
	// UP
	w = black & UP_FILTER;
	t = w & (white << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	//~ t |= w & (t << 8);
	
	moves |= UP_FILTER & (t << 8);
	
	// UP_LEFT
	w = black & UP_LEFT_FILTER;
	t = w & (white << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	//~ t |= w & (t << 9);
	
	moves |= UP_LEFT_FILTER & (t << 9);
	
	// DOWN_RIGHT
	w = black & DOWN_RIGHT_FILTER;
	t = w & (white >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	//~ t |= w & (t >> 9);
	
	moves |= DOWN_RIGHT_FILTER & (t >> 9);
	
	// UP_RIGHT
	w = black & UP_RIGHT_FILTER;
	t = w & (white << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	//~ t |= w & (t << 7);
	
	moves |= UP_RIGHT_FILTER & (t << 7);
	
	// DOWN_LEFT
	w = black & DOWN_LEFT_FILTER;
	t = w & (white >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	//~ t |= w & (t >> 7);
	
	moves |= DOWN_LEFT_FILTER & (t >> 7);
	
	//~ cerr << "LM time: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<< endl;
	return moves & ~taken;
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
				black |= SINGLE_BIT[TO_INDEX(xx, yy)];
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

Board Board::doMoveOnNewBoard(const int &index, const Side &side) const {
	if (side) return doMoveOnNewBoardBlack(index);
	else return doMoveOnNewBoardWhite(index);
}

Board Board::doMoveOnNewBoardBlack(const int &index) const {
	// Makes move on new board using bit operations
	// This leads to no jumping when converted to assembly
	// (once we branch into side == BLACK or side == WHITE)
	
	// Observe that if the move is illegal, taken will be updated, 
	// but black will remain the same.
	
	//~ assert(SINGLE_BIT[index] & findLegalMoves(side));
	
	//~ auto start = chrono::high_resolution_clock::now();
	uint64_t white = ~black & taken;
	
	const uint64_t bi = SINGLE_BIT[index];
	uint64_t n = bi;
	
	uint64_t newblack = black;
	uint64_t filtered;
	
	// LEFT
	filtered = white & LEFT_FILTER;
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	//~ n |= white & LEFT_FILTER & (n << 1);
	newblack = (black & LEFT_FILTER & (n << 1)) ? newblack | n : newblack;
	// RIGHT
	n = bi;
	filtered = white & RIGHT_FILTER;
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	//~ n |= white & RIGHT_FILTER & (n >> 1);
	newblack = (black & RIGHT_FILTER & (n >> 1)) ? newblack | n : newblack;
	
	// DOWN
	n = bi;
	filtered = white & DOWN_FILTER;
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	//~ n |= white & DOWN_FILTER & (n >> 8);
	newblack = (black & DOWN_FILTER & (n >> 8)) ? newblack | n : newblack;
	
	// UP
	n = bi;
	filtered = white & UP_FILTER;
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	//~ n |= white & UP_FILTER & (n << 8);
	newblack = (black & UP_FILTER & (n << 8)) ? newblack | n : newblack;
	
	// UP_LEFT
	n = bi;
	filtered = white & UP_LEFT_FILTER;
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	//~ n |= white & UP_LEFT_FILTER & (n << 9);
	newblack = (black & UP_LEFT_FILTER & (n << 9)) ? newblack | n : newblack;
	
	// DOWN_RIGHT
	n = bi;
	filtered = white & DOWN_RIGHT_FILTER;
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	//~ n |= white & DOWN_RIGHT_FILTER & (n >> 9);
	newblack = (black & DOWN_RIGHT_FILTER & (n >> 9)) ? newblack | n : newblack;
	
	// UP_RIGHT
	n = bi;
	filtered = white & UP_RIGHT_FILTER;
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	//~ n |= white & UP_RIGHT_FILTER & (n << 7);
	newblack = (black & UP_RIGHT_FILTER & (n << 7)) ? newblack | n : newblack;
	
	// DOWN_LEFT
	n = bi;
	filtered = white & DOWN_LEFT_FILTER;
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	//~ n |= white & DOWN_LEFT_FILTER & (n >> 7);
	newblack = (black & DOWN_LEFT_FILTER & (n >> 7)) ? newblack | n : newblack;

	// Zobrist hash stuff
	size_t zobrist_hash_copy = zobrist_hash;
	zobrist_hash_copy ^= random_numbers[index];
	uint64_t flipped = black ^ newblack ^ BIT(index);
	while (flipped) {
		int index = __builtin_clzl(flipped);
		flipped ^= BIT(index);
		zobrist_hash_copy ^= random_numbers[index + 64];
		zobrist_hash_copy ^= random_numbers[index];
	}

	//~ cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
	return Board(taken | bi, newblack, zobrist_hash_copy);
}

Board Board::doMoveOnNewBoardWhite(const int &index) const {
	// Makes move on new board using bit operations
	// This leads to no jumping when converted to assembly
	// (once we branch into side == BLACK or side == WHITE)
	
	// Observe that if the move is illegal, taken will be updated, 
	// but black will remain the same.
	
	//~ assert(SINGLE_BIT[index] & findLegalMoves(side));
	
	uint64_t white = ~black & taken;
	
	const uint64_t bi = SINGLE_BIT[index];
	uint64_t n = bi;
	
	uint64_t newwhite = white;
	uint64_t filtered;
	
	// LEFT
	filtered = black & LEFT_FILTER;
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	//~ n |= black & LEFT_FILTER & (n << 1);
	newwhite = (white & LEFT_FILTER & (n << 1)) ? newwhite | n : newwhite;
	
	// RIGHT
	n = bi;
	filtered = black & RIGHT_FILTER;
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	//~ n |= black & RIGHT_FILTER & (n >> 1);
	newwhite = (white & RIGHT_FILTER & (n >> 1)) ? newwhite | n : newwhite;
	
	// DOWN
	n = bi;
	filtered = black & DOWN_FILTER;
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	//~ n |= black & DOWN_FILTER & (n >> 8);
	newwhite = (white & DOWN_FILTER & (n >> 8)) ? newwhite | n : newwhite;
	
	// UP
	n = bi;
	filtered = black & UP_FILTER;
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	//~ n |= black & UP_FILTER & (n << 8);
	newwhite = (white & UP_FILTER & (n << 8)) ? newwhite | n : newwhite;
	
	// UP_LEFT
	n = bi;
	filtered = black & UP_LEFT_FILTER;
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	//~ n |= black & UP_LEFT_FILTER & (n << 9);
	newwhite = (white & UP_LEFT_FILTER & (n << 9)) ? newwhite | n : newwhite;
	
	// DOWN_RIGHT
	n = bi;
	filtered = black & DOWN_RIGHT_FILTER;
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	//~ n |= black & DOWN_RIGHT_FILTER & (n >> 9);
	newwhite = (white & DOWN_RIGHT_FILTER & (n >> 9)) ? newwhite | n : newwhite;
			
	// UP_RIGHT
	n = bi;
	filtered = black & UP_RIGHT_FILTER;
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	//~ n |= black & UP_RIGHT_FILTER & (n << 7);
	newwhite = (white & UP_RIGHT_FILTER & (n << 7)) ? newwhite | n : newwhite;
	
	// DOWN_LEFT
	n = bi;
	filtered = black & DOWN_LEFT_FILTER;
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	//~ n |= filtered & (n >> 7);
	newwhite = (white & DOWN_LEFT_FILTER & (n >> 7)) ? newwhite | n : newwhite;
	
	// Zobrist hash stuff
	//~ Timer tim;
	size_t zobrist_hash_copy = zobrist_hash;
	zobrist_hash_copy ^= random_numbers[index + 64];
	uint64_t flipped = white ^ newwhite ^ BIT(index);
	//~ int count = __builtin_popcountll(flipped);
	while (flipped) {
		int index = __builtin_clzl(flipped);
		flipped ^= BIT(index);
		zobrist_hash_copy ^= random_numbers[index + 64];
		zobrist_hash_copy ^= random_numbers[index];
	}
	//~ tim.end(to_string(count) + " ");
	
	//~ cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
	return Board(taken | bi, ~newwhite & taken, zobrist_hash_copy);
}

Board Board::doMoveOnNewBoardBlackWZH(const int &index) const {
	// Makes move on new board using bit operations
	// This leads to no jumping when converted to assembly
	// (once we branch into side == BLACK or side == WHITE)
	
	// Observe that if the move is illegal, taken will be updated, 
	// but black will remain the same.
	
	//~ assert(SINGLE_BIT[index] & findLegalMoves(side));
	
	//~ auto start = chrono::high_resolution_clock::now();
	uint64_t white = ~black & taken;
	
	const uint64_t bi = SINGLE_BIT[index];
	uint64_t n = bi;
	
	uint64_t newblack = black;
	uint64_t filtered;
	
	// LEFT
	filtered = white & LEFT_FILTER;
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	//~ n |= white & LEFT_FILTER & (n << 1);
	newblack = (black & LEFT_FILTER & (n << 1)) ? newblack | n : newblack;
	// RIGHT
	n = bi;
	filtered = white & RIGHT_FILTER;
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	//~ n |= white & RIGHT_FILTER & (n >> 1);
	newblack = (black & RIGHT_FILTER & (n >> 1)) ? newblack | n : newblack;
	
	// DOWN
	n = bi;
	filtered = white & DOWN_FILTER;
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	//~ n |= white & DOWN_FILTER & (n >> 8);
	newblack = (black & DOWN_FILTER & (n >> 8)) ? newblack | n : newblack;
	
	// UP
	n = bi;
	filtered = white & UP_FILTER;
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	//~ n |= white & UP_FILTER & (n << 8);
	newblack = (black & UP_FILTER & (n << 8)) ? newblack | n : newblack;
	
	// UP_LEFT
	n = bi;
	filtered = white & UP_LEFT_FILTER;
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	//~ n |= white & UP_LEFT_FILTER & (n << 9);
	newblack = (black & UP_LEFT_FILTER & (n << 9)) ? newblack | n : newblack;
	
	// DOWN_RIGHT
	n = bi;
	filtered = white & DOWN_RIGHT_FILTER;
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	//~ n |= white & DOWN_RIGHT_FILTER & (n >> 9);
	newblack = (black & DOWN_RIGHT_FILTER & (n >> 9)) ? newblack | n : newblack;
	
	// UP_RIGHT
	n = bi;
	filtered = white & UP_RIGHT_FILTER;
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	//~ n |= white & UP_RIGHT_FILTER & (n << 7);
	newblack = (black & UP_RIGHT_FILTER & (n << 7)) ? newblack | n : newblack;
	
	// DOWN_LEFT
	n = bi;
	filtered = white & DOWN_LEFT_FILTER;
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	//~ n |= white & DOWN_LEFT_FILTER & (n >> 7);
	newblack = (black & DOWN_LEFT_FILTER & (n >> 7)) ? newblack | n : newblack;

	//~ cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
	return Board(taken | bi, newblack, 0);
}

Board Board::doMoveOnNewBoardWhiteWZH(const int &index) const {
	// Makes move on new board using bit operations
	// This leads to no jumping when converted to assembly
	// (once we branch into side == BLACK or side == WHITE)
	
	// Observe that if the move is illegal, taken will be updated, 
	// but black will remain the same.
	
	//~ assert(SINGLE_BIT[index] & findLegalMoves(side));
	
	//~ auto start = chrono::high_resolution_clock::now();
	uint64_t white = ~black & taken;
	
	const uint64_t bi = SINGLE_BIT[index];
	uint64_t n = bi;
	
	uint64_t newwhite = white;
	uint64_t filtered;
	
	// LEFT
	filtered = black & LEFT_FILTER;
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	n |= filtered & (n << 1);
	//~ n |= black & LEFT_FILTER & (n << 1);
	newwhite = (white & LEFT_FILTER & (n << 1)) ? newwhite | n : newwhite;
	
	// RIGHT
	n = bi;
	filtered = black & RIGHT_FILTER;
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	n |= filtered & (n >> 1);
	//~ n |= black & RIGHT_FILTER & (n >> 1);
	newwhite = (white & RIGHT_FILTER & (n >> 1)) ? newwhite | n : newwhite;
	
	// DOWN
	n = bi;
	filtered = black & DOWN_FILTER;
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	n |= filtered & (n >> 8);
	//~ n |= black & DOWN_FILTER & (n >> 8);
	newwhite = (white & DOWN_FILTER & (n >> 8)) ? newwhite | n : newwhite;
	
	// UP
	n = bi;
	filtered = black & UP_FILTER;
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	n |= filtered & (n << 8);
	//~ n |= black & UP_FILTER & (n << 8);
	newwhite = (white & UP_FILTER & (n << 8)) ? newwhite | n : newwhite;
	
	// UP_LEFT
	n = bi;
	filtered = black & UP_LEFT_FILTER;
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	n |= filtered & (n << 9);
	//~ n |= black & UP_LEFT_FILTER & (n << 9);
	newwhite = (white & UP_LEFT_FILTER & (n << 9)) ? newwhite | n : newwhite;
	
	// DOWN_RIGHT
	n = bi;
	filtered = black & DOWN_RIGHT_FILTER;
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	n |= filtered & (n >> 9);
	//~ n |= black & DOWN_RIGHT_FILTER & (n >> 9);
	newwhite = (white & DOWN_RIGHT_FILTER & (n >> 9)) ? newwhite | n : newwhite;
			
	// UP_RIGHT
	n = bi;
	filtered = black & UP_RIGHT_FILTER;
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	n |= filtered & (n << 7);
	//~ n |= black & UP_RIGHT_FILTER & (n << 7);
	newwhite = (white & UP_RIGHT_FILTER & (n << 7)) ? newwhite | n : newwhite;
	
	// DOWN_LEFT
	n = bi;
	filtered = black & DOWN_LEFT_FILTER;
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	n |= filtered & (n >> 7);
	//~ n |= filtered & (n >> 7);
	newwhite = (white & DOWN_LEFT_FILTER & (n >> 7)) ? newwhite | n : newwhite;
		
	//~ cerr << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
	return Board(taken | bi, ~newwhite & taken, 0);
}

/*
 * Evaluation function. findLegalMoves must be called first to get a 
 * nonzero value.
 */
int Board::evaluate(int blackMoves, int whiteMoves, int position) const {
	//~ int totalCount = __builtin_popcountll(b.taken);
	uint64_t white = taken & ~black;
	
	// Game over if no discs left
	if (!white) {
		//~ evaluation = INT_MAX;
		return INT_MAX;
	}
	else if (!black) {
		return INT_MIN;
	}
	
	int ee = 0;
	
	// Constants to tweak
	int mobilityWeight = 4;
	
	ee = 15 * mobilityWeight * (blackMoves - whiteMoves) / (blackMoves + whiteMoves + 2) + position; //Iago mobility ee //round(mobilityWeight * ((blackMoves + 1) / (double) (whiteMoves + 1) - (whiteMoves + 1) / (double) (blackMoves + 1)));
	
	return ee;
	
}

int Board::pos_evaluate() const {
	int ee = 0;
	uint64_t white = taken & ~black;
	//~ int shiftRowCount = 48;
	
	#if X_SQUARE_PENALTY
	{
		const int penaltyWeight = 10;
		// Penalty for risky squares if corner not filled
		ee -= (!(taken & CORNER_TL) && (BIT(9) & black));
		ee += (!(taken & CORNER_TL) && (BIT(9) & white));
		ee -= (!(taken & CORNER_TR) && (BIT(14) & black));
		ee += (!(taken & CORNER_TR) && (BIT(14) & white));
		ee -= (!(taken & CORNER_BL) && (BIT(49) & black));
		ee += (!(taken & CORNER_BL) && (BIT(49) & white));
		ee -= (!(taken & CORNER_BR) && (BIT(54) & black));
		ee += (!(taken & CORNER_BR) && (BIT(54) & white));
		ee *= penaltyWeight;
	}
	#endif
	
	const int totalCount = __builtin_popcountll(taken);
	// Minimize discs early
	if (totalCount < 40) {
		int discdiff = (__builtin_popcountll(white) - __builtin_popcountll(black));
		ee += discdiff / 5;
	}
	
	// Get top and bottom edge into uint16
	uint16_t u16 = ((taken >> 56) << 8) | (black >> 56);
	ee += EDGE_VALUES[u16];
	u16 = ((taken << 56) >> 48) | ((black << 56) >> 56);
	ee += EDGE_VALUES[u16];

	// Get left and right edges
	#if USE_MAGIC
	// Idea from http://stackoverflow.com/questions/14537831/isolate-specific-row-column-diagonal-from-a-64-bit-number
	{
		const uint64_t column_mask = 0x8080808080808080ull;
		const uint64_t magic = 0x2040810204081ull;
		uint64_t col_taken = (((taken & column_mask) * magic) >> 56) & 0xff;
		uint64_t col_black = (((black & column_mask) * magic) >> 56) & 0xff;
		ee += EDGE_VALUES[(col_taken << 8) | col_black];
		col_taken = ((((taken << 7) & column_mask) * magic) >> 56) & 0xff;
		col_black = ((((black << 7) & column_mask) * magic) >> 56) & 0xff;
		ee += EDGE_VALUES[(col_taken << 8) | col_black];
	}
	#else
	{
		// Get left edge
		uint64_t tempTaken = taken & EDGE_LEFT;
		uint64_t tempB = black & EDGE_LEFT;
		tempTaken >>= 7;
		tempB >>= 7;
		// Now same as right edge
		tempTaken |= (tempTaken >> 7);
		tempTaken |= (tempTaken >> 7);
		tempTaken |= (tempTaken >> 7);
		tempTaken |= (tempTaken >> 7);
		tempTaken |= (tempTaken >> 7);
		tempTaken |= (tempTaken >> 7);
		tempTaken |= (tempTaken >> 7);
		tempB |= (tempB >> 7);
		tempB |= (tempB >> 7);
		tempB |= (tempB >> 7);
		tempB |= (tempB >> 7);
		tempB |= (tempB >> 7);
		tempB |= (tempB >> 7);
		tempB |= (tempB >> 7);
		
		u16 = (tempTaken << 8) | tempB;
		ee += EDGE_VALUES[u16];
		
		// Get right edge
		tempTaken = taken & EDGE_RIGHT;
		tempB = black & EDGE_RIGHT;
		tempTaken |= (tempTaken >> 7);
		tempTaken |= (tempTaken >> 7);
		tempTaken |= (tempTaken >> 7);
		tempTaken |= (tempTaken >> 7);
		tempTaken |= (tempTaken >> 7);
		tempTaken |= (tempTaken >> 7);
		tempTaken |= (tempTaken >> 7);
		tempB |= (tempB >> 7);
		tempB |= (tempB >> 7);
		tempB |= (tempB >> 7);
		tempB |= (tempB >> 7);
		tempB |= (tempB >> 7);
		tempB |= (tempB >> 7);
		tempB |= (tempB >> 7);
		
		u16 = (tempTaken << 8) | tempB;
		ee += EDGE_VALUES[u16];
	}
	#endif
	
	return ee;
}

/*
 * Pure greedy evaluation for test
 */
int Board::evaluateTest() const {
	int evaluation = countBlack() - countWhite();
	return evaluation;
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) const {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() const {
    return __builtin_popcountll(black & taken);
}

/*
 * Current count of white stones.
 */
int Board::countWhite() const {
    return __builtin_popcountll(taken & ~black);
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
