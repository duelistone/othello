#include "board2.h"

um_data make_hash_data(int eval, int best, int alpha, int beta) {
	um_data data;
	data.eval = eval;
	data.best = best;
	data.alpha = alpha;
	data.beta = beta;
	return data;
}

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() : taken(0), black(0) {
    OCCUPY_WHITE(3 + 8 * 3, taken, black);
    OCCUPY_BLACK(3 + 8 * 4, taken, black);
    OCCUPY_BLACK(4 + 8 * 3, taken, black);
    OCCUPY_WHITE(4 + 8 * 4, taken, black);
    // black |= BIT(18) | BIT(19) | BIT(20) | BIT(21) | BIT(26) | BIT(29) | BIT(34) | BIT(37) | BIT(42) | BIT(43) | BIT(44) | BIT(45);
}

/*
 * Make a board given a taken and black uint64_t.
 */
Board::Board(uint64_t t, uint64_t b) : taken(t), black(b) {}

uint64_t doMove(uint64_t taken, uint64_t black, Side side, int index) {
	uint64_t white = ~black & taken;
	//~ uint64_t empty = ~taken;
	
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

uint64_t Board::findLegalMoves2(Side side) /*const*/ {
	//auto start = chrono::high_resolution_clock::now();
	
	uint64_t moves = 0;
	uint64_t b = black & taken;
	uint64_t white = ~black & taken;
	uint64_t empty = ~taken;
	
	uint64_t w, opp, own, tNew, t;
	
	// Set opponent
	if (side == BLACK) {
		own = b;
		opp = white;
	}
	else {
		own = white;
		opp = b;
	}
	
	// RIGHT
	w = opp & RIGHT_FILTER;
	t = w & (own >> 1);
	tNew = t | (w & (t >> 1));
	while (tNew != t) {
		t = tNew;
		tNew |= w & (t >> 1);
	}
	
	moves |= empty & RIGHT_FILTER & (t >> 1);
	
	// LEFT
	w = opp & LEFT_FILTER;
	t = w & (own << 1);
	tNew = t | (w & (t << 1));
	while (tNew != t) {
		t = tNew;
		tNew |= w & (t << 1);
	}
	moves |= empty & LEFT_FILTER & (t << 1);
	
	// DOWN
	w = opp & DOWN_FILTER;
	t = w & (own >> 8);
	tNew = t | (w & (t >> 8));
	while (tNew != t) {
		t = tNew;
		tNew |= w & (t >> 8);
	}
	moves |= empty & DOWN_FILTER & (t >> 8);
	
	// UP
	w = opp & UP_FILTER;
	t = w & (own << 8);
	tNew = t | (w & (t << 8));
	while (tNew != t) {
		t = tNew;
		tNew |= w & (t << 8);
	}
	moves |= empty & UP_FILTER & (t << 8);
	
	// UP_LEFT
	w = opp & UP_LEFT_FILTER;
	t = w & (own << 9);
	tNew = t | (w & (t << 9));
	while (tNew != t) {
		t = tNew;
		tNew |= w & (t << 9);
	}
	moves |= empty & UP_LEFT_FILTER & (t << 9);
	
	// DOWN_RIGHT
	w = opp & DOWN_RIGHT_FILTER;
	t = w & (own >> 9);
	tNew = t | (w & (t >> 9));
	while (tNew != t) {
		t = tNew;
		tNew |= w & (t >> 9);
	}
	moves |= empty & DOWN_RIGHT_FILTER & (t >> 9);
	
	// UP_RIGHT
	w = opp & UP_RIGHT_FILTER;
	t = w & (own << 7);
	tNew = t | (w & (t << 7));
	while (tNew != t) {
		t = tNew;
		tNew |= w & (t << 7);
	}
	moves |= empty & UP_RIGHT_FILTER & (t << 7);
	
	// DOWN_LEFT
	w = opp & DOWN_LEFT_FILTER;
	t = w & (own >> 7);
	tNew = t | (w & (t >> 7));
	while (tNew != t) {
		t = tNew;
		tNew |= w & (t >> 7);
	}
	moves |= empty & DOWN_LEFT_FILTER & (t >> 7);
	
	//cerr << "LM time: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<< endl;
	return moves;
}

uint64_t Board::findLegalMoves(Side side) /*const*/ {
	//~ auto start = chrono::high_resolution_clock::now();
	
	uint64_t moves = 0;
	//uint64_t b = black & taken;
	uint64_t white = ~black & taken;
	uint64_t empty = ~taken;
	
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
	moves |= empty & RIGHT_FILTER & (t >> 1);
	
	// LEFT
	w = opp & LEFT_FILTER;
	t = w & (own << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	t |= w & (t << 1);
	//~ t |= w & (t << 1);
	
	moves |= empty & LEFT_FILTER & (t << 1);
	
	// DOWN
	w = opp & DOWN_FILTER;
	t = w & (own >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	t |= w & (t >> 8);
	//~ t |= w & (t >> 8);
	
	moves |= empty & DOWN_FILTER & (t >> 8);
	
	// UP
	w = opp & UP_FILTER;
	t = w & (own << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	t |= w & (t << 8);
	//~ t |= w & (t << 8);
	
	moves |= empty & UP_FILTER & (t << 8);
	
	// UP_LEFT
	w = opp & UP_LEFT_FILTER;
	t = w & (own << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	t |= w & (t << 9);
	//~ t |= w & (t << 9);
	
	moves |= empty & UP_LEFT_FILTER & (t << 9);
	
	// DOWN_RIGHT
	w = opp & DOWN_RIGHT_FILTER;
	t = w & (own >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	t |= w & (t >> 9);
	//~ t |= w & (t >> 9);
	
	moves |= empty & DOWN_RIGHT_FILTER & (t >> 9);
	
	// UP_RIGHT
	w = opp & UP_RIGHT_FILTER;
	t = w & (own << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	t |= w & (t << 7);
	//~ t |= w & (t << 7);
	
	moves |= empty & UP_RIGHT_FILTER & (t << 7);
	
	// DOWN_LEFT
	w = opp & DOWN_LEFT_FILTER;
	t = w & (own >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	t |= w & (t >> 7);
	//~ t |= w & (t >> 7);
	
	moves |= empty & DOWN_LEFT_FILTER & (t >> 7);
	
	//~ cerr << "LM time: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<< endl;
	return moves;
}

#if 0
uint64_t Board::findLegalMoves3(Side side) /*const*/ {
	//~ legalMoves = findLegalMoves3(side);
	//~ return legalMoves;
	//auto start = chrono::high_resolution_clock::now();
	uint64_t legalMoves = 0;
	int potentialMobility = 0;
	uint64_t empty = black & ~taken;
	uint64_t b = black & taken;
	uint64_t white = ~black & taken;
	
	if (side == BLACK) {
		while (empty) {
			int index = __builtin_clzll(empty);
			uint64_t bi = SINGLE_BIT[index];
			empty &= ~bi;
			int X = FROM_INDEX_X(index);
			int Y = FROM_INDEX_Y(index);

			// Potential mobility
			bool increaseMobility = false;
			bool notEdge = !(EDGES & bi); //index > 8 && index < 55 && temp && (temp != 7);
			
			int x, y;
			//int dx, dy;
			uint64_t bi2; // For BIT(TO_INDEX(x, y))
			
			//auto start2 = chrono::high_resolution_clock::now();
			// dx = -1; dy = -1;
			x = X - 1;
			y = Y - 1;
			bi2 = bi << 9;
			if (x >= 0 && y >= 0 && (white & bi2)) {
				do {
					x--;
					y--;
					bi2 <<= 9;
					if (x < 0 || y < 0) goto lm_case1_end;
				} while (white & bi2);
				
				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
				// Else if x, y is not taken, we have potential mobility due to X, Y
				increaseMobility = true;
				lm_case1_end: ;
			}
			//cerr << "LM inner time: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start2).count()<< endl;
			//dy = 1; // dx = -1;
			x = X + 1;
			y = Y - 1;
			bi2 = bi >> 7;
			if (y < 8 && x >= 0 && (white & bi2)) {
				do {
					x--;
					y++;
					bi2 >>= 7;
					if (y >= 8 || x < 0) goto lm_case2_end;
				} while (white & bi2);
				
				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case2_end: ;
			}
			
			//dx = 1; dy = -1;
			x = X + 1;
			y = Y - 1;
			bi2 = bi << 7;
			if (x < 8 && y >= 0 && (white & bi2)) {
				do {
					x++;
					y--;
					bi2 <<= 7;
					if (x >= 8 || y < 0) goto lm_case3_end;
				} while (white & bi2);

				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case3_end: ;
			}
			
			//dy = 1; // dx = 1;
			x = X + 1;
			y = Y + 1;
			bi2 = bi >> 9;
			if (x < 8 && y < 8 && (white & bi2)) {
				do {
					x++;
					y++;
					bi2 >>= 9;
					if (x >= 8 || y >= 8) goto lm_case4_end;
				} while (white & bi2);

				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case4_end: ;
			}
			
			// dx = 0; dy = 1;
			//x = X;
			y = Y + 1;
			bi2 = bi >> 8;
			if (y < 8 && (white & bi2)) {
				do {
					y++;
					bi2 >>= 8;
					if (y >= 8) goto lm_case5_end;
				} while (white & bi2);

				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case5_end: ;
			}
			
			// dx = -1; dy = 0;
			x = X - 1;
			//y = Y;
			bi2 = bi << 1;
			if (x >= 0 && (white & bi2)) {
				do {
					x--;
					bi2 <<= 1;
					if (x < 0) goto lm_case6_end;
				} while (white & bi2);

				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case6_end: ;
			}
			
			// dx = 0; dy = -1;
			//x = X;
			y = Y - 1;
			bi2 = bi << 8;
			if (y >= 0 && (white & bi2)) {
				do {
					y--;
					bi2 <<= 8;
					if (y < 0) goto lm_case7_end;
				} while (white & bi2);

				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case7_end: ;
			}
			
			//dx = 1; dy = 0;
			x = X + 1;
			//y = Y;
			bi2 = bi >> 1;
			if (x < 8 && (white & bi2)) {
				do {
					x++;
					bi2 >>= 1;
					if (x >= 8) goto lm_case8_end;
				} while (white & bi2);

				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case8_end: ;
			}
			
			if (increaseMobility && notEdge) potentialMobility++;
		}
	}
	else {
		while (empty) {
			int index = __builtin_clzll(empty);
			uint64_t bi = SINGLE_BIT[index];
			empty &= ~bi;
			int X = FROM_INDEX_X(index);
			int Y = FROM_INDEX_Y(index);

			// Potential mobility
			bool increaseMobility = false;
			bool notEdge = !(EDGES & bi); //index > 8 && index < 55 && temp && (temp != 7);
			
			//int dx, dy;
			int x, y;
			uint64_t bi2; // For BIT(TO_INDEX(x, y))
			
			// dx = -1; dy = -1;
			x = X - 1;
			y = Y - 1;
			bi2 = bi << 9;
			if (x >= 0 && y >= 0 && (b & bi2)) {
				do {
					x--;
					y--;
					bi2 <<= 9;
					if (x < 0 || y < 0) goto lm_case9_end;
				} while (b & bi2);
				
				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case9_end: ;
			}
				
			// dy = 1; // dx = -1;
			x = X + 1;
			y = Y - 1;
			bi2 = bi >> 7;
			if (x >= 0 && y < 8 && (b & bi2)) {
				do {
					x--;
					y++;
					bi2 >>= 7;
					if (y >= 8 || x < 0) goto lm_case10_end;
				} while (b & bi2);

				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case10_end: ;
			}
			
			// dx = 1; dy = -1;
			x = X + 1;
			y = Y - 1;
			bi2 = bi << 7;
			if (x < 8 && y >= 0 && (b & bi2)) {
				do {
					x++;
					y--;
					bi2 <<= 7;
					if (x >= 8 || y < 0) goto lm_case11_end;
				} while (b & bi2);
				
				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case11_end: ;
			}
			
			// dy = 1; // dx = 1;
			x = X + 1;
			y = Y + 1;
			bi2 = bi >> 9;
			if (x < 8 && y < 8 && (b & bi2)) {
				do {
					x++;
					y++;
					bi2 >>= 9;
					if (x >= 8 || y >= 8) goto lm_case12_end;
				} while (b & bi2);

				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case12_end: ;
			}
			
			//dx = 0; dy = 1;
			//x = X;
			y = Y + 1;
			bi2 = bi >> 8;
			if (y < 8 && (b & bi2)) {
				do {
					y++;
					bi2 >>= 8;
					if (y >= 8) goto lm_case13_end;
				} while (b & bi2);

				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case13_end: ;
			}
			
			//dx = -1; dy = 0;
			x = X - 1;
			//y = Y;
			bi2 = bi << 1;
			if (x >= 0 && (b & bi2)) {
				do {
					x--;
					bi2 <<= 1;
					if (x < 0) goto lm_case14_end;
				} while (b & bi2);
				
				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case14_end: ;
			}
			
			// dx = 0; dy = -1;
			//x = X;
			y = Y - 1;
			bi2 = bi << 8;
			if (y >= 0 && (b & bi2)) {
				do {
					y--;
					bi2 <<= 8;
					if (y < 0) goto lm_case15_end;
				} while (b & bi2);
				
				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case15_end: ;
			}
			
			// dx = 1; dy = 0;
			x = X + 1;
			//y = Y;
			bi2 = bi >> 1;
			if (x < 8 && (b & bi2)) {
				do {
					x++;
					bi2 >>= 1;
					if (x >= 8) goto lm_case16_end;
				} while (b & bi2);
				
				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
				increaseMobility = true;
				lm_case16_end: ;
			}
			
			if (increaseMobility && notEdge) potentialMobility++;
		}
	}
	//cerr << "LM time: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<< endl;
	//~ legalMovesComputed = true;
	return legalMoves;
}

uint64_t Board::onlyFindLegalMoves(Side side) const {
	uint64_t legalMoves = findLegalMoves2(side);
	return legalMoves;
	
	//auto start = chrono::high_resolution_clock::now();
	legalMoves = 0;
	uint64_t empty = black & ~taken;
	uint64_t b = black & taken;
	uint64_t white = ~black & taken;
	
	if (side == BLACK) {
		while (empty != 0) {
			int index = __builtin_clzll(empty);
			uint64_t bi = SINGLE_BIT[index];
			empty &= ~bi;
			int X = FROM_INDEX_X(index);
			int Y = FROM_INDEX_Y(index);

			// int dx, dy;
			int x, y;
			uint64_t bi2; // For BIT(TO_INDEX(x, y))
			
			// dx = -1; dy = -1;
			x = X - 1;
			y = Y + 1;
			bi2 = bi << 9;
			if (x >= 0 && y >= 0 && (white & bi2)) {
				do {
					x--;
					y--;
					bi2 <<= 9;
					if (x < 0 || y < 0) goto olm_case1_end;
				} while (white & bi2);
				
				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case1_end:
			
			// dy = 1; // dx = -1;
			x = X - 1;
			y = Y + 1;
			bi2 = bi >> 7;
			if (y < 8 && x >= 0 && (white & bi2)) {
				do {
					x--;
					y++;
					bi2 >>= 7;
					if (y >= 8 || x < 0) goto olm_case2_end;
				} while (white & bi2);
				
				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case2_end:
			
			//dx = 1; dy = -1;
			x = X + 1;
			y = Y - 1;
			bi2 = bi << 7;
			if (x < 8 && y >= 0 && (white & bi2)) {
				do {
					x++;
					y--;
					bi2 <<= 7;
					if (x >= 8 || y < 0) goto olm_case3_end;
				} while (white & bi2);

				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
				
			}
			olm_case3_end: 
			
			// dy = 1; // dx = 1;
			x = X + 1;
			y = Y + 1;
			bi2 = bi >> 9;
			if (x < 8 && y < 8 && (white & bi2)) {
				do {
					x++;
					y++;
					bi2 >>= 9;
					if (x >= 8 || y >= 8) goto olm_case4_end;
				} while (white & bi2);

				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case4_end:
			
			// dx = 0; dy = 1;
			x = X;
			y = Y + 1;
			bi2 = bi >> 8;
			if (y < 8 && (white & bi2)) {
				do {
					y++;
					bi2 >>= 8;
					if (y >= 8) goto olm_case5_end;
				} while (white & bi2);

				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case5_end:
			
			//dx = -1; dy = 0;
			x = X - 1;
			y = Y;
			bi2 = bi << 1;
			if (x >= 0 && (white & bi2)) {
				do {
					x--;
					bi2 <<= 1;
					if (x < 0) goto olm_case6_end;
				} while (white & bi2);

				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case6_end:
			
			// dx = 0; dy = -1;
			x = X;
			y = Y - 1;
			bi2 = bi << 8;
			if (y >= 0 && (white & bi2)) {
				do {
					y--;
					bi2 <<= 8;
					if (y < 0) goto olm_case7_end;
				} while (white & bi2);

				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case7_end:
			
			// dx = 1; dy = 0;
			x = X + 1;
			y = Y;
			bi2 = bi >> 1;
			if (x < 8 && (white & bi2)) {
				do {
					x++;
					bi2 >>= 1;
					if (x >= 8) goto olm_case8_end;
				} while (white & bi2);

				if (b & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case8_end: ;
		}
	}
	else {
				while (empty != 0) {
			int index = __builtin_clzll(empty);
			uint64_t bi = BIT(index);
			empty &= ~bi;
			int X = FROM_INDEX_X(index);
			int Y = FROM_INDEX_Y(index);

			// int dx, dy;
			int x, y;
			uint64_t bi2; // For BIT(TO_INDEX(x, y))
			
			// dx = -1; dy = -1;
			x = X - 1;
			y = Y + 1;
			bi2 = bi << 9;
			if (x >= 0 && y >= 0 && (b & bi2)) {
				do {
					x--;
					y--;
					bi2 <<= 9;
					if (x < 0 || y < 0) goto olm_case9_end;
				} while (b & bi2);
				
				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case9_end:
			
			// dy = 1; // dx = -1;
			x = X - 1;
			y = Y + 1;
			bi2 = bi >> 7;
			if (y < 8 && x >= 0 && (b & bi2)) {
				do {
					x--;
					y++;
					bi2 >>= 7;
					if (y >= 8 || x < 0) goto olm_case10_end;
				} while (b & bi2);
				
				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case10_end:
			
			//dx = 1; dy = -1;
			x = X + 1;
			y = Y - 1;
			bi2 = bi << 7;
			if (x < 8 && y >= 0 && (b & bi2)) {
				do {
					x++;
					y--;
					bi2 <<= 7;
					if (x >= 8 || y < 0) goto olm_case11_end;
				} while (b & bi2);

				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
				
			}
			olm_case11_end: 
			
			// dy = 1; // dx = 1;
			x = X + 1;
			y = Y + 1;
			bi2 = bi >> 9;
			if (x < 8 && y < 8 && (b & bi2)) {
				do {
					x++;
					y++;
					bi2 >>= 9;
					if (x >= 8 || y >= 8) goto olm_case12_end;
				} while (b & bi2);

				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case12_end:
			
			// dx = 0; dy = 1;
			x = X;
			y = Y + 1;
			bi2 = bi >> 8;
			if (y < 8 && (b & bi2)) {
				do {
					y++;
					bi2 >>= 8;
					if (y >= 8) goto olm_case13_end;
				} while (b & bi2);

				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case13_end:
			
			//dx = -1; dy = 0;
			x = X - 1;
			y = Y;
			bi2 = bi << 1;
			if (x >= 0 && (b & bi2)) {
				do {
					x--;
					bi2 <<= 1;
					if (x < 0) goto olm_case14_end;
				} while (b & bi2);

				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case14_end:
			
			// dx = 0; dy = -1;
			x = X;
			y = Y - 1;
			bi2 = bi << 8;
			if (y >= 0 && (b & bi2)) {
				do {
					y--;
					bi2 <<= 8;
					if (y < 0) goto olm_case15_end;
				} while (b & bi2);

				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case15_end:
			
			// dx = 1; dy = 0;
			x = X + 1;
			y = Y;
			bi2 = bi >> 1;
			if (x < 8 && (b & bi2)) {
				do {
					x++;
					bi2 >>= 1;
					if (x >= 8) goto olm_case16_end;
				} while (b & bi2);

				if (white & bi2) {
					legalMoves |= bi;
					continue;
				}
			}
			olm_case16_end: ;
		}
	}
	//cerr << "LegalMoves: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
	return legalMoves;

}


bool Board::hasLegalMoves(Side side) {
	//auto start = chrono::high_resolution_clock::now();
	uint64_t legalMoves = 0;
	uint64_t empty = black & ~taken;
	
	while (empty != 0) {
		int index = __builtin_clzll(empty);
		empty &= ~BIT(index);
		int X = FROM_INDEX_X(index);
		int Y = FROM_INDEX_Y(index);

		// Else, check if legal move
		Side other = other_side(side);
		
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
bool Board::isDone() const {
    return !(hasMoves(BLACK) || hasMoves(WHITE));
}

/*
 * Returns true if there are legal moves for the given side.
 */
bool Board::hasMoves(Side side) const {
	return findLegalMoves(side);
}

/*
 * Returns true if a move is legal for the given side; false otherwise.
 */
bool Board::checkMove(int X, int Y, Side side) const {
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
#endif
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

Board Board::doMoveOnNewBoard2(int x, int y, Side side) /*const*/ {
	// Makes move on new board using bit operations
	
	// auto start = chrono::high_resolution_clock::now();
	//uint64_t b = black & taken; // Should be unnecessary now
	uint64_t white = ~black & taken;
	uint64_t empty = ~taken;
	
	int index = TO_INDEX(x, y);
	uint64_t n = SINGLE_BIT[index];
	
	uint64_t newtaken = taken;
	uint64_t newblack = black;
	
	newtaken |= n;
	
	int counter;
	
	if (side == BLACK) {
		newblack |= n;
		
		// LEFT
		counter = 0;
		do {
			n <<= 1;
			n &= LEFT_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case1_end;
			counter++;
		} while (white & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n >>= 1;
			newblack |= n;
			counter--;
		}
		dmonb2_case1_end:
		
		// RIGHT
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n >>= 1;
			n &= RIGHT_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case2_end;
			counter++;
		} while (white & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n <<= 1;
			newblack |= n;
			counter--;
		}
		dmonb2_case2_end:
		
		// DOWN
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n >>= 8;
			n &= DOWN_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case3_end;
			counter++;
		} while (white & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n <<= 8;
			newblack |= n;
			counter--;
		}
		dmonb2_case3_end:
		
		// UP
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n <<= 8;
			n &= UP_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case4_end;
			counter++;
		} while (white & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n >>= 8;
			newblack |= n;
			counter--;
		}
		dmonb2_case4_end:
		
		// UP_LEFT
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n <<= 9;
			n &= UP_LEFT_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case5_end;
			counter++;
		} while (white & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n >>= 9;
			newblack |= n;
			counter--;
		}
		dmonb2_case5_end:
		
		// DOWN_RIGHT
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n >>= 9;
			n &= DOWN_RIGHT_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case6_end;
			counter++;
		} while (white & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n <<= 9;
			newblack |= n;
			counter--;
		}
		dmonb2_case6_end:
		
		// UP_RIGHT
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n <<= 7;
			n &= UP_RIGHT_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case7_end;
			counter++;
		} while (white & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n >>= 7;
			newblack |= n;
			counter--;
		}
		dmonb2_case7_end:
		
		// DOWN_LEFT
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n >>= 7;
			n &= DOWN_LEFT_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case8_end;
			counter++;
		} while (white & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n <<= 7;
			newblack |= n;
			counter--;
		}
		dmonb2_case8_end: ;
	}
	else {
		// LEFT
		counter = 0;
		do {
			n <<= 1;
			n &= LEFT_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case9_end;
			counter++;
		} while (black & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n >>= 1;
			newblack ^= n;
			counter--;
		}
		dmonb2_case9_end:
		
		// RIGHT
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n >>= 1;
			n &= RIGHT_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case10_end;
			counter++;
		} while (black & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n <<= 1;
			newblack ^= n;
			counter--;
		}
		dmonb2_case10_end:
		
		// DOWN
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n >>= 8;
			n &= DOWN_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case11_end;
			counter++;
		} while (black & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n <<= 8;
			newblack ^= n;
			counter--;
		}
		dmonb2_case11_end:
		
		// UP
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n <<= 8;
			n &= UP_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case12_end;
			counter++;
		} while (black & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n >>= 8;
			newblack ^= n;
			counter--;
		}
		dmonb2_case12_end:
		
		// UP_LEFT
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n <<= 9;
			n &= UP_LEFT_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case13_end;
			counter++;
		} while (black & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n >>= 9;
			newblack ^= n;
			counter--;
		}
		dmonb2_case13_end:
		
		// DOWN_RIGHT
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n >>= 9;
			n &= DOWN_RIGHT_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case14_end;
			counter++;
		} while (black & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n <<= 9;
			newblack ^= n;
			counter--;
		}
		dmonb2_case14_end:
		
		// UP_RIGHT
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n <<= 7;
			n &= UP_RIGHT_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case15_end;
			counter++;
		} while (black & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n >>= 7;
			newblack ^= n;
			counter--;
		}
		dmonb2_case15_end:
		
		// DOWN_LEFT
		counter = 0;
		n = SINGLE_BIT[index];
		do {
			n >>= 7;
			n &= DOWN_LEFT_FILTER;
			// Goto end of case if no capture will occur
			if ((empty & n) || !n) goto dmonb2_case16_end;
			counter++;
		} while (black & n);
		
		// Run through in opposite direction to flip
		while (counter > 1) {
			n <<= 7;
			newblack ^= n;
			counter--;
		}
		dmonb2_case16_end: ;
	}
	
	//cerr << "DoMoveOnNewBoard2: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
	return Board(newtaken, newblack);
}

#if 0
Board Board::doMoveOnNewBoard(int x, int y, Side side) /*const*/ {
	// Makes move on new board using bit operations
	// This leads to no jumping when converted to assembly
	// (once we branch into side == BLACK or side == WHITE)
	
	//~ auto start = chrono::high_resolution_clock::now();
	//uint64_t b = black & taken; // Should be unnecessary now
	uint64_t white = ~black & taken;
	//~ uint64_t empty = ~taken;
	
	int index = TO_INDEX(x, y);
	const uint64_t bi = SINGLE_BIT[index];
	uint64_t n = bi;
	
	uint64_t newtaken = taken;
	uint64_t newblack = black;
	
	newtaken |= n;
	
	if (side == BLACK) {
		// LEFT
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		//~ n |= white & LEFT_FILTER & (n << 1);
		if (black & LEFT_FILTER & (n << 1)) newblack |= n;
		
		// RIGHT
		n = bi;
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		//~ n |= white & RIGHT_FILTER & (n >> 1);
		if (black & RIGHT_FILTER & (n >> 1)) newblack |= n;
		
		// DOWN
		n = bi;
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		//~ n |= white & DOWN_FILTER & (n >> 8);
		if (black & DOWN_FILTER & (n >> 8)) newblack |= n;
		
		// UP
		n = bi;
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		//~ n |= white & UP_FILTER & (n << 8);
		if (black & UP_FILTER & (n << 8)) newblack |= n;
		
		// UP_LEFT
		n = bi;
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		//~ n |= white & UP_LEFT_FILTER & (n << 9);
		if (black & UP_LEFT_FILTER & (n << 9)) newblack |= n;
		
		// DOWN_RIGHT
		n = bi;
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		//~ n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		if (black & DOWN_RIGHT_FILTER & (n >> 9)) newblack |= n;
		
		// UP_RIGHT
		n = bi;
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		//~ n |= white & UP_RIGHT_FILTER & (n << 7);
		if (black & UP_RIGHT_FILTER & (n << 7)) newblack |= n;
		
		// DOWN_LEFT
		n = bi;
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		//~ n |= white & DOWN_LEFT_FILTER & (n >> 7);
		if (black & DOWN_LEFT_FILTER & (n >> 7)) newblack |= n;
		
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
		if (white & LEFT_FILTER & (n << 1)) newblack &= ~n;
		
		// RIGHT
		n = bi;
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		//~ n |= black & RIGHT_FILTER & (n >> 1);
		if (white & RIGHT_FILTER & (n >> 1)) newblack &= ~n;
		
		// DOWN
		n = bi;
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		//~ n |= black & DOWN_FILTER & (n >> 8);
		if (white & DOWN_FILTER & (n >> 8)) newblack &= ~n;
		
		// UP
		n = bi;
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		//~ n |= black & UP_FILTER & (n << 8);
		if (white & UP_FILTER & (n << 8)) newblack &= ~n;
		
		// UP_LEFT
		n = bi;
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		//~ n |= black & UP_LEFT_FILTER & (n << 9);
		if (white & UP_LEFT_FILTER & (n << 9)) newblack &= ~n;
		
		// DOWN_RIGHT
		n = bi;
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		//~ n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		if (white & DOWN_RIGHT_FILTER & (n >> 9)) newblack &= ~n;
		
		// UP_RIGHT
		n = bi;
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		//~ n |= black & UP_RIGHT_FILTER & (n << 7);
		if (white & UP_RIGHT_FILTER & (n << 7)) newblack &= ~n;
		
		// DOWN_LEFT
		n = bi;
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		//~ n |= black & DOWN_LEFT_FILTER & (n >> 7);
		if (white & DOWN_LEFT_FILTER & (n >> 7)) newblack &= ~n;
	}
	//~ cerr << "DoMoveOnNewBoard3: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
	return Board(newtaken, newblack);
}
#endif

Board Board::doMoveOnNewBoard(int index, Side side) /*const*/ {
	// Makes move on new board using bit operations
	// This leads to no jumping when converted to assembly
	// (once we branch into side == BLACK or side == WHITE)
	
	//~ assert(SINGLE_BIT[index] & findLegalMoves(side));
	
	//~ auto start = chrono::high_resolution_clock::now();
	//uint64_t b = black & taken; // Should be unnecessary now
	uint64_t white = ~black & taken;
	//~ uint64_t empty = ~taken;
	
	const uint64_t bi = SINGLE_BIT[index];
	uint64_t n = bi;
	
	uint64_t newtaken = taken;
	uint64_t newblack = black;
	
	newtaken |= n;
	
	if (side == BLACK) {
		// LEFT
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		n |= white & LEFT_FILTER & (n << 1);
		//~ n |= white & LEFT_FILTER & (n << 1);
		if (black & LEFT_FILTER & (n << 1)) newblack |= n;
		
		// RIGHT
		n = bi;
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		n |= white & RIGHT_FILTER & (n >> 1);
		//~ n |= white & RIGHT_FILTER & (n >> 1);
		if (black & RIGHT_FILTER & (n >> 1)) newblack |= n;
		
		// DOWN
		n = bi;
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		n |= white & DOWN_FILTER & (n >> 8);
		//~ n |= white & DOWN_FILTER & (n >> 8);
		if (black & DOWN_FILTER & (n >> 8)) newblack |= n;
		
		// UP
		n = bi;
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		n |= white & UP_FILTER & (n << 8);
		//~ n |= white & UP_FILTER & (n << 8);
		if (black & UP_FILTER & (n << 8)) newblack |= n;
		
		// UP_LEFT
		n = bi;
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		n |= white & UP_LEFT_FILTER & (n << 9);
		//~ n |= white & UP_LEFT_FILTER & (n << 9);
		if (black & UP_LEFT_FILTER & (n << 9)) newblack |= n;
		
		// DOWN_RIGHT
		n = bi;
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		//~ n |= white & DOWN_RIGHT_FILTER & (n >> 9);
		if (black & DOWN_RIGHT_FILTER & (n >> 9)) newblack |= n;
		
		// UP_RIGHT
		n = bi;
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		n |= white & UP_RIGHT_FILTER & (n << 7);
		//~ n |= white & UP_RIGHT_FILTER & (n << 7);
		if (black & UP_RIGHT_FILTER & (n << 7)) newblack |= n;
		
		// DOWN_LEFT
		n = bi;
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		n |= white & DOWN_LEFT_FILTER & (n >> 7);
		//~ n |= white & DOWN_LEFT_FILTER & (n >> 7);
		if (black & DOWN_LEFT_FILTER & (n >> 7)) newblack |= n;
		
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
		if (white & LEFT_FILTER & (n << 1)) newblack &= ~n;
		
		// RIGHT
		n = bi;
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		n |= black & RIGHT_FILTER & (n >> 1);
		//~ n |= black & RIGHT_FILTER & (n >> 1);
		if (white & RIGHT_FILTER & (n >> 1)) newblack &= ~n;
		
		// DOWN
		n = bi;
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		n |= black & DOWN_FILTER & (n >> 8);
		//~ n |= black & DOWN_FILTER & (n >> 8);
		if (white & DOWN_FILTER & (n >> 8)) newblack &= ~n;
		
		// UP
		n = bi;
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		n |= black & UP_FILTER & (n << 8);
		//~ n |= black & UP_FILTER & (n << 8);
		if (white & UP_FILTER & (n << 8)) newblack &= ~n;
		
		// UP_LEFT
		n = bi;
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		n |= black & UP_LEFT_FILTER & (n << 9);
		//~ n |= black & UP_LEFT_FILTER & (n << 9);
		if (white & UP_LEFT_FILTER & (n << 9)) newblack &= ~n;
		
		// DOWN_RIGHT
		n = bi;
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		//~ n |= black & DOWN_RIGHT_FILTER & (n >> 9);
		if (white & DOWN_RIGHT_FILTER & (n >> 9)) newblack &= ~n;
		
		// UP_RIGHT
		n = bi;
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		n |= black & UP_RIGHT_FILTER & (n << 7);
		//~ n |= black & UP_RIGHT_FILTER & (n << 7);
		if (white & UP_RIGHT_FILTER & (n << 7)) newblack &= ~n;
		
		// DOWN_LEFT
		n = bi;
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		n |= black & DOWN_LEFT_FILTER & (n >> 7);
		//~ n |= black & DOWN_LEFT_FILTER & (n >> 7);
		if (white & DOWN_LEFT_FILTER & (n >> 7)) newblack &= ~n;
	}
	//~ cerr << "DoMoveOnNewBoard3: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
	return Board(newtaken, newblack);
}

Board Board::doMoveOnNewBoard3(int X, int Y, Side side) /*const*/ {
	return doMoveOnNewBoard3(X, Y, side);
	//auto start = chrono::high_resolution_clock::now();
	// A NULL move means pass.
    if (X == -1) return Board(taken, black);
	
	uint64_t newtaken = taken;
	uint64_t newblack = black;
	
    //~ Side other = (side == BLACK) ? WHITE : BLACK;
    
    int dx, dy;
    int x, y, xx, yy;
    uint64_t newtakenCopy, newblackCopy;
    
    uint64_t bi = SINGLE_BIT[TO_INDEX(X, Y)];
    
    if (bi & ~EDGES) {
		dx = -1; dy = -1;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (!(taken & SINGLE_BIT[TO_INDEX(xx, yy)])) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case1_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case1_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case1_end:
				
		dx = -1; dy = 1;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (!(taken & SINGLE_BIT[TO_INDEX(xx, yy)])) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case2_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case2_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case2_end:
				
		dx = 1; dy = -1;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (!(taken & SINGLE_BIT[TO_INDEX(xx, yy)])) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case3_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case3_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case3_end:
				
		dx = 1; dy = 1;
		
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (!(taken & SINGLE_BIT[TO_INDEX(xx, yy)])) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case4_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case4_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case4_end:
				
		dx = -1; dy = 0;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (!(taken & SINGLE_BIT[TO_INDEX(xx, yy)])) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case5_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case5_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case5_end:
				
		dx = 0; dy = -1;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (!(taken & SINGLE_BIT[TO_INDEX(xx, yy)])) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case6_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case6_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case6_end:
				
		dx = 1; dy = 0;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (!(taken & SINGLE_BIT[TO_INDEX(xx, yy)])) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case7_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case7_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case7_end:
				
		dx = 0; dy = 1;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (!(taken & SINGLE_BIT[TO_INDEX(xx, yy)])) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case8_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case8_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case8_end: ;
				
	}
    else {
		dx = -1; dy = -1;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (ON_BOARD(xx, yy) && !(taken & SINGLE_BIT[TO_INDEX(xx, yy)]) && !(bi & EDGES)) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case9_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case9_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case9_end:
				
		dx = -1; dy = 1;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (ON_BOARD(xx, yy) && !(taken & SINGLE_BIT[TO_INDEX(xx, yy)]) && !(bi & EDGES)) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case10_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case10_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case10_end:
				
		dx = 1; dy = -1;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (ON_BOARD(xx, yy) && !(taken & SINGLE_BIT[TO_INDEX(xx, yy)]) && !(bi & EDGES)) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case11_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case11_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case11_end: 
				
		dx = 1; dy = 1;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (ON_BOARD(xx, yy) && !(taken & SINGLE_BIT[TO_INDEX(xx, yy)]) && !(bi & EDGES)) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case12_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case12_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case12_end:
				
		dx = -1; dy = 0;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (ON_BOARD(xx, yy) && !(taken & SINGLE_BIT[TO_INDEX(xx, yy)]) && !(bi & CORNERS) && !(bi & INNER_EDGE_RIGHT)) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case13_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case13_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case13_end:
				
		dx = 0; dy = -1;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (ON_BOARD(xx, yy) && !(taken & SINGLE_BIT[TO_INDEX(xx, yy)]) && !(bi & CORNERS) && !(bi & INNER_EDGE_BOTTOM)) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case14_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case14_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case14_end:
				
		dx = 1; dy = 0;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (ON_BOARD(xx, yy) && !(taken & SINGLE_BIT[TO_INDEX(xx, yy)]) && !(bi & CORNERS) && !(bi & INNER_EDGE_LEFT)) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case15_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case15_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case15_end:
				
		dx = 0; dy = 1;
		
				x = X;
				y = Y;
				xx = X + dx;
				yy = Y + dy;
				
				if (ON_BOARD(xx, yy) && !(taken & SINGLE_BIT[TO_INDEX(xx, yy)]) && !(bi & CORNERS) && !(bi & INNER_EDGE_TOP)) {
					// newblack |= SINGLE_BIT[TO_INDEX(xx, yy)];
					goto dmonb_case16_end;
				}
				
				newtakenCopy = newtaken;
				newblackCopy = newblack;
				
				do {
					x += dx;
					y += dy;
					int index = TO_INDEX(x, y);
					if (!(ON_BOARD(x, y)) || !(taken & SINGLE_BIT[index])) goto dmonb_case16_end;
					else if (( black & SINGLE_BIT[index] ) == ( (side == BLACK) ? SINGLE_BIT[index] : 0 )) break;
					{if (side == BLACK) newblackCopy |= SINGLE_BIT[index]; else newblackCopy &= ~SINGLE_BIT[index];}
				} while (true);

				
				newtaken = newtakenCopy;
				newblack = newblackCopy;
				
				dmonb_case16_end: ;
	}			
    PLACE_DISC(side, X, Y, newtaken, newblack);
    Board b2 = doMoveOnNewBoard2(X, Y, side);
    if (b2.black != newblack) {
		bitset<64> bs3(newtaken), bs(newblack), bs2(b2.black);
		cerr << "---" << endl;
		cerr << bs3 << endl << bs << endl << bs2 << endl;
	}
    //cerr << "DoMoveOnNewBoard: " << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count()<<endl;
    return Board(newtaken, newblack);
}

/*
 * Evaluation function. findLegalMoves must be called first to get a 
 * nonzero value.
 */
int Board::evaluate(int blackMoves, int whiteMoves, int position) /*const*/ {
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
	//~ int potentialMobilityWeight = 1;
	int mobilityBoost = 5;
	// Computations
	//~ uint64_t legalMoves = b.findLegalMoves(BLACK);
	//~ int blackMoves = __builtin_popcountll(legalMoves);
	//~ legalMoves = b.findLegalMoves(WHITE);
	//~ int whiteMoves = __builtin_popcountll(legalMoves);
	
	ee = 15 * mobilityWeight * (blackMoves - whiteMoves) / (blackMoves + whiteMoves + 2) + position; //Iago mobility ee //round(mobilityWeight * ((blackMoves + 1) / (double) (whiteMoves + 1) - (whiteMoves + 1) / (double) (blackMoves + 1)));
	
	
	
}

int Board::evaluate_mobility() {
	uint64_t legalMoves = findLegalMoves(BLACK);
	int blackMoves = __builtin_popcountll(legalMoves);
	legalMoves = findLegalMoves(WHITE);
	int whiteMoves = __builtin_popcountll(legalMoves);		
	return 15 * 4 * (blackMoves - whiteMoves) / (blackMoves + whiteMoves + 2);
}

int Board::pos_evaluate() /*const*/ {
	int ee = 0;
	int penaltyWeight = 10;
	uint64_t white = taken & ~black;
	
	// Penalty for risky squares if corner not filled
	if (!(taken & CORNER_TL) && (BIT(9) & black)) {
		ee -= penaltyWeight;
	}
	else if (!(taken & CORNER_TL) && (BIT(9) & white)) {
		ee += penaltyWeight;
	}
	if (!(taken & CORNER_TR) && (BIT(14) & black)) {
		ee -= penaltyWeight;
	}
	else if (!(taken & CORNER_TR) && (BIT(14) & white)) {
		ee += penaltyWeight;
	}
	if (!(taken & CORNER_BL) && (BIT(49) & black)) {
		ee -= penaltyWeight;
	}
	else if (!(taken & CORNER_BL) && (BIT(49) & white)) {
		ee += penaltyWeight;
	}
	if (!(taken & CORNER_BR) && (BIT(54) & black)) {
		ee -= penaltyWeight;
	}
	else if (!(taken & CORNER_BR) && (BIT(54) & white)) {
		ee += penaltyWeight;
	}
	
	// Minimize discs early
	int discdiff = (__builtin_popcountll(white) - __builtin_popcountll(black));
	discdiff = (discdiff > 16) ? 16 : discdiff;
	discdiff = (discdiff < -16) ? -16 : discdiff;
	ee += discdiff / 4;

	// Get top edge into uint16
	uint16_t u16 = ((taken >> 56) << 8) | (black >> 56);
	ee += EDGE_VALUES[u16];
	
	u16 = ((taken << 56) >> 48) | ((black << 56) >> 56);
	ee += EDGE_VALUES[u16];
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
	tempB &= EDGE_BOTTOM;
	
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
	tempB &= EDGE_BOTTOM;
	
	u16 = (tempTaken << 8) | tempB;
	
	ee += EDGE_VALUES[u16];
	return ee;
}

/*
 * Pure greedy evaluation for test
 */
int Board::evaluateTest() /*const*/ {
	int evaluation = countBlack() - countWhite();
	return evaluation;
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) /*const*/ {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() /*const*/ {
    return __builtin_popcountll(black & taken);
}

/*
 * Current count of white stones.
 */
int Board::countWhite() /*const*/ {
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
