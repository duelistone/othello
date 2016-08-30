#include "player.h"

#define SIMPLE_EVAL 1
#define IID 1
#define USE_HASH_IN_ENDGAME_ALPHABETA 1
#define QSEARCH 1

// Note: More work necessary to make killer heuristic work, and
// (when it was working) there was no noticeable speed increase.

#define HASH_DEPTH (MAX_DEPTH - 1)

// 58.5, 413 MB depth 12 no PC
// 3:11, 1.2 GB depth 13 no PC
// 1:19, 750 MB depth 13 no PC simple eval
// 3:35, 1.9 GB depth 14 no PC simple eval
// 2:39 after improved column extractor

// With no x square penalty (different game),
// 3:05, 600 MB depth 14 no PC simple eval
// 3:03 after splitting up legalMoves function
// 2:57 after removing branching from doMoveOnNewBoard
#define MAX_DEPTH 14
#define MAX_DEPTH2 16

#define THREAT_THRESHOLD 20

struct Eval {
	Board pvBoard;
	int e;
	Eval(const Board &b, const int &ev) {
		pvBoard = b;
		e = ev;
	}
};

Side abortSide;
int currentDiscs;
int pVariations[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Board poisonPositions[1000];
int numberOfPoisonPositions = 0;

void playerConstructorHelper(int i0 = 0, int i1 = 0, int i2 = 0, int i3 = 0, int i4 = 0, int i5 = 0, int i6 = 0, int i7 = 0, int depth = 8) {
	if (depth > 0) {
		for (int i = -1; i <= 1; i++) {
			if (depth == 8) i0 = i;
			else if (depth == 7) i1 = i;
			else if (depth == 6) i2 = i;
			else if (depth == 5) i3 = i;
			else if (depth == 4) i4 = i;
			else if (depth == 3) i5 = i;
			else if (depth == 2) i6 = i;
			else if (depth == 1) i7 = i;
			playerConstructorHelper(i0, i1, i2, i3, i4, i5, i6, i7, depth - 1);
		}
		return;
	}
	
	int cornerWeight = 9;
	int stableWeight = 3;
	int cornerWedgeWeight_1 = 7;
	int cornerWedgeWeight_3 = 4;
	int cornerWedgeWeight_5 = 1;
	//~ int unstableWeightC = -4;
	int unbalancedEdgeWeight = -6;
    
	uint16_t data = 0;
	int eval = 0;
	if (i0) data |= EDGE_BIT(0);
	if (i0 == 1) data |= EDGE_BIT(8);
	if (i1) data |= EDGE_BIT(1);
	if (i1 == 1) data |= EDGE_BIT(9);
	if (i2) data |= EDGE_BIT(2);
	if (i2 == 1) data |= EDGE_BIT(10);
	if (i3) data |= EDGE_BIT(3);
	if (i3 == 1) data |= EDGE_BIT(11);
	if (i4) data |= EDGE_BIT(4);
	if (i4 == 1) data |= EDGE_BIT(12);
	if (i5) data |= EDGE_BIT(5);
	if (i5 == 1) data |= EDGE_BIT(13);
	if (i6) data |= EDGE_BIT(6);
	if (i6 == 1) data |= EDGE_BIT(14);
	if (i7) data |= EDGE_BIT(7);
	if (i7 == 1) data |= EDGE_BIT(15);
	
	EDGE_VOLATILITY[data] = 0;
	
	int count = __builtin_popcount(data >> 8);
	
	function<int(int)> f = [data] (int index) -> int {
		if ((data & EDGE_BIT(index)) == 0) return -1;
		else if (data & EDGE_BIT(index + 8)) return BLACK;
		else return WHITE;
	};
	
	for (int i = 0; i < 8; i++) {
		// If empty, continue
		if (f(i) == -1) continue;
		
		Side s = f(i);
		
		if (i == 0 || i == 7) {
			if (s == BLACK) eval += cornerWeight;
			else eval -= cornerWeight;
			continue;
		}
		
		// Is the disc stable?
		if (count == 8) {
			// Yes
			if (i != 0 && i != 7) {
				if (s == BLACK) eval += stableWeight;
				else eval -= stableWeight;
			}
			continue;
		}
		
		int x = i;
		x--;
		while (x >= 0 && f(x) == f(i)) x--;
		if (x == -1) {
			if (s == BLACK) eval += stableWeight;
			else eval -= stableWeight;
			continue;
		}
		
		x = i;
		x++;
		while (x < 8 && f(x) == f(i)) x++;
		if (x == 8) {
			if (s == BLACK) eval += stableWeight;
			else eval -= stableWeight;
		}
	}
	
	if (f(0) == -1) {
		Side s = f(1);
		int counter = 0;
		if (s != -1) {
			Side other = other_side(s);
			for (int i = 2; i < 8; i++) {
				// Odd number of empty squares
				if (counter == 1 && f(i) == s) {
					eval -= (s ? cornerWedgeWeight_1 : -cornerWedgeWeight_1);
					break;
				}
				else if (counter == 3 && f(i) == s) {
					eval -= (s ? cornerWedgeWeight_3 : -cornerWedgeWeight_3);
					break;
				}
				else if (counter == 5 && f(i) == s) {
					eval -= (s ? cornerWedgeWeight_5 : -cornerWedgeWeight_5);
					break;
				}
				// Any other case with nonzero counter
				if (counter) break;
				// Corner can be immediately taken
				if (f(i) == other) {
					EDGE_VOLATILITY[data] = 1;
					break;
				}
				// Empty square
				if (f(i) == -1) {
					counter++;
				}
			}
		}
	}
	
	if (f(7) == -1 && !EDGE_VOLATILITY[data]) {
		Side s = f(6);
		int counter = 0;
		if (s != -1) {
			Side other = other_side(s);
			for (int i = 5; i >= 0; i--) {
				// Odd number of empty squares
				if (counter == 1 && f(i) == s) {
					eval -= (s ? cornerWedgeWeight_1 : -cornerWedgeWeight_1);
					break;
				}
				else if (counter == 3 && f(i) == s) {
					eval -= (s ? cornerWedgeWeight_3 : -cornerWedgeWeight_3);
					break;
				}
				else if (counter == 5 && f(i) == s) {
					eval -= (s ? cornerWedgeWeight_5 : -cornerWedgeWeight_5);
					break;
				}
				// Any other case with nonzero counter
				if (counter) break;
				// Corner can be immediately taken
				if (f(i) == other) {
					EDGE_VOLATILITY[data] = 1;
					break;
				}
				// Empty square
				if (f(i) == -1) {
					counter++;
				}
			}
		}
	}
	
	// Unbalanced edge
	if (f(0) == -1 && f(1) == BLACK && f(2) == BLACK && f(3) == BLACK &&
		f(4) == BLACK && f(5) == BLACK && f(6) == -1 && f(7) == -1) {
		eval += unbalancedEdgeWeight;
	}
	else if (f(0) == -1 && f(1) == WHITE && f(2) == WHITE && f(3) == WHITE &&
		f(4) == WHITE && f(5) == WHITE && f(6) == -1 && f(7) == -1) {
		eval -= unbalancedEdgeWeight;
	}
	else if (f(0) == -1 && f(1) == -1 && f(2) == BLACK && f(3) == BLACK &&
		f(4) == BLACK && f(5) == BLACK && f(6) == BLACK && f(7) == -1) {
		eval += unbalancedEdgeWeight;
	}
	else if (f(0) == -1 && f(1) == -1 && f(2) == WHITE && f(3) == WHITE &&
		f(4) == WHITE && f(5) == WHITE && f(6) == WHITE && f(7) == -1) {
		eval -= unbalancedEdgeWeight;
	}
	
	// Forced unbalanced edge (or worse)
	if (f(0) == -1 && f(1) == -1 && f(2) == BLACK && f(3) == -1 &&
		f(4) == BLACK && f(5) == WHITE && f(6) == -1 && f(7) == -1) {
		eval -= unbalancedEdgeWeight;
	}
	else if (f(0) == -1 && f(1) == -1 && f(2) == WHITE && f(3) == -1 &&
			 f(4) == WHITE && f(5) == BLACK && f(6) == -1 && f(7) == -1) {
		eval += unbalancedEdgeWeight;
	}
	// Symmetry
	else if (f(0) == -1 && f(1) == -1 && f(2) == WHITE && f(3) == BLACK &&
			 f(4) == -1 && f(5) == BLACK && f(6) == -1 && f(7) == -1) {
		eval -= unbalancedEdgeWeight;
	}
	else if (f(0) == -1 && f(1) == -1 && f(2) == BLACK && f(3) == WHITE &&
			 f(4) == -1 && f(5) == WHITE && f(6) == -1 && f(7) == -1) {
		eval += unbalancedEdgeWeight;
	}
	
	EDGE_VALUES[data] = eval;
	
	
	/*
	for (int i = 0; i < 8; i++) {
		if (f(i) == -1) fil << '-';
		else if (f(i) == BLACK) fil << 'X';
		else fil << 'O';
	}
	fil << ' ' << bitset<16>(data) << ' ' << f(5) << f(6) << f(7) << ' ' << eval << endl;
	*/
	// ./testgame freezes when you uncomment this for who knows what reason
	/*
	bitset<8> bs((unsigned char) (data >> 8));
	bitset<8> bs2((unsigned char) data);
	cerr << bs << ' ' << bs2 << ' ' << eval << endl;
	cerr << "---" << endl;
	*/
}

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side s) : side(s), currBoard(Board()) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;
    
    playerConstructorHelper();
}

Player::~Player() {}

// Static evaluation function
inline int eval(const Board &b2) {
	int blackMoves = __builtin_popcountll(b2.findLegalMovesBlack() & SAFE_SQUARES);
	int whiteMoves = __builtin_popcountll(b2.findLegalMovesWhite() & SAFE_SQUARES);
	return MOBILITY_WEIGHT * (blackMoves - whiteMoves) / (blackMoves + whiteMoves + 2) + b2.pos_evaluate();
}

// alphabeta declaration
Eval alphabeta(const Board &b, const int &depth, const Side &s, int alpha = INT_MIN, int beta = INT_MAX, const int &depth2 = 0, const bool &prevPass = false, const uint64_t &mask = 0b1111111111111111111111111111111111111111111111111111111111111111);
int endgame_alphabeta(const Board &b, const Side &s, int alpha = INT_MIN, int beta = INT_MAX);

// Quivaluate
int quivaluate(const Board &b, const Side &s, const int &alpha, const int &beta, const int &depth2, const int &e, const uint64_t &mask = 0b1111111111111111111111111111111111111111111111111111111111111111) {
	if (depth2 >= MAX_DEPTH2) {
		return e;
	}
	// if (mask != 0b1111111111111111111111111111111111111111111111111111111111111111) return e;
	
	uint64_t lm = b.findLegalMoves(s);
	if (__builtin_popcountll(lm) < 5) return alphabeta(b, 2, s, alpha, beta, depth2).e;
	
	// Extract edges
	// Get top and bottom edge into uint16
	uint64_t new_mask = 0;
	uint16_t u16 = ((b.taken >> 56) << 8) | (b.black >> 56);
	if (EDGE_VOLATILITY[u16]) new_mask |= EDGE_TOP;
	u16 = ((b.taken << 56) >> 48) | ((b.black << 56) >> 56);
	if (EDGE_VOLATILITY[u16]) new_mask |= EDGE_BOTTOM;

	// Get left and right edges
	// Idea from http://stackoverflow.com/questions/14537831/isolate-specific-row-column-diagonal-from-a-64-bit-number
	{
		const uint64_t column_mask = 0x8080808080808080ull;
		const uint64_t magic = 0x2040810204081ull;
		uint64_t col_taken = (((b.taken & column_mask) * magic) >> 56) & 0xff;
		uint64_t col_black = (((b.black & column_mask) * magic) >> 56) & 0xff;
		if (EDGE_VOLATILITY[(col_taken << 8) | col_black]) new_mask |= EDGE_LEFT;
		col_taken = ((((b.taken << 7) & column_mask) * magic) >> 56) & 0xff;
		col_black = ((((b.black << 7) & column_mask) * magic) >> 56) & 0xff;
		if (EDGE_VOLATILITY[(col_taken << 8) | col_black]) new_mask |= EDGE_RIGHT;
	}
	
	// new_mask &= mask;
	
	//~ if (new_mask != 0 && depth2 == 10) cerr << bitset<64>(b.taken) << endl << bitset<64>(b.black) << endl; 
	
	// Return
	if (new_mask != 0) return alphabeta(b, 2, s, alpha, beta, depth2, false, new_mask).e;
	return e;
}

int abCalls = 0;
int timeWasted = 0;
Eval alphabeta(const Board &b, const int &depth, const Side &s, int alpha, int beta, const int &depth2, const bool &prevPass, const uint64_t &mask) {
	abCalls++;
	
	if (depth <= 0 || depth2 >= MAX_DEPTH2) {
		int blackMoves = __builtin_popcountll(b.findLegalMovesBlack() & SAFE_SQUARES);
		int whiteMoves = __builtin_popcountll(b.findLegalMovesWhite() & SAFE_SQUARES);
		int result = MOBILITY_WEIGHT * (blackMoves - whiteMoves) / (blackMoves + whiteMoves + 2) + b.pos_evaluate();
		return Eval(b, result);
	}		

	BoardWithSide bws(b.taken, b.black, s);
	uint64_t legalMoves = b.findLegalMoves(s);
	
	// Special case
	if (!legalMoves) {
		if (prevPass) {
			int blacks = b.countBlack();
			int whites = b.countWhite();
			return (blacks > whites) ? Eval(b, INT_MAX) : ((whites > blacks) ? Eval(b, INT_MIN) : Eval(b, 0));
		}
		return alphabeta(b, depth - 1, other_side(s), alpha, beta, depth2 + 1, true, mask);
	}
	// No legal moves with mask taken into account
	if (!(legalMoves & mask)) {
		return alphabeta(b, depth - 1, other_side(s), alpha, beta, depth2 + 1, false, mask);
	}
	
	legalMoves &= mask;
	
	// Depth 1
	#if QSEARCH
	int loss = (s ? INT_MIN : INT_MAX);
	int lower = (s ? INT_MIN : 0);
	int upper = (s ? 0 : INT_MAX);
	Board pvBoard;
	if (depth == 1) {
		uint8_t besti = 64;
		if (s) {
			int v = INT_MIN;
			// Best move
			while (alpha < beta && legalMoves) {	
				uint8_t index = __builtin_clzl(legalMoves);
				legalMoves ^= BIT(index);
				Board b2 = b.doMoveOnNewBoardBlack(index);
				Eval val = alphabeta(b2, 0, WHITE, alpha, beta, depth2 + 1, false, mask);
				if (depth2 >= MAX_DEPTH - 1) val.e = quivaluate(b2, WHITE, alpha, beta, depth2 + 1, val.e, mask);
				if (val.e > v && depth2 >= MAX_DEPTH - 1 && __builtin_popcountll(b2.taken) > 48) {
					// Endgame search on PV
					if (endgame_alphabeta(b2, WHITE, lower, upper) != loss) {
						pvBoard = b2;
						besti = index;
						v = val.e;
					}
				}
				else if (val.e > v) {
					pvBoard = b2;
					besti = index;
					v = val.e;
				}
				alpha = (v > alpha) ? v : alpha;
			}
			return Eval(pvBoard, alpha);
		}
		else {
			int v = INT_MAX;
			// Best move
			while (alpha < beta && legalMoves) {	
				uint8_t index = __builtin_clzl(legalMoves);
				legalMoves ^= BIT(index);
				Board b2 = b.doMoveOnNewBoardBlack(index);
				Eval val = alphabeta(b2, 0, BLACK, alpha, beta, depth2 + 1, false, mask);
				if (depth2 >= MAX_DEPTH - 1) val.e = quivaluate(b2, BLACK, alpha, beta, depth2 + 1, val.e, mask);
				if (val.e < v && depth2 >= MAX_DEPTH - 1 && __builtin_popcountll(b2.taken) > 48) {
					// Endgame search on PV
					if (endgame_alphabeta(b2, BLACK, lower, upper) != loss) {
						pvBoard = b2;
						besti = index;
						v = val.e;
					}
				}
				else if (val.e < v) {
					pvBoard = b2;
					besti = index;
					v = val.e;
				}
				beta = (v < beta) ? v : beta;
			}
			return Eval(pvBoard, beta);
		}
	}
	else {
	#endif
		uint8_t besti = 64;
		if (s) {
			int v = INT_MIN;
			// Best move
			while (alpha < beta && legalMoves) {	
				uint8_t index = __builtin_clzl(legalMoves);
				legalMoves ^= BIT(index);
				Eval val = alphabeta(b.doMoveOnNewBoardBlack(index), depth - 1, WHITE, alpha, beta, depth2 + 1, false, mask);
				if (val.e > v) {
					pvBoard = val.pvBoard;
					besti = index;
					v = val.e;
				}
				alpha = (v > alpha) ? v : alpha;
			}
			return Eval(pvBoard, alpha);
		}
		else {
			int v = INT_MAX;
			// Best move
			while (alpha < beta && legalMoves) {	
				uint8_t index = __builtin_clzl(legalMoves);
				legalMoves ^= BIT(index);
				Eval val = alphabeta(b.doMoveOnNewBoardWhite(index), depth - 1, BLACK, alpha, beta, depth2 + 1, false, mask);
				if (val.e < v) {
					pvBoard = val.pvBoard;
					besti = index;
					v = val.e;
				}
				beta = (v < beta) ? v : beta;
			}
			return Eval(pvBoard, beta);
		}
	#if QSEARCH
	}
	#endif
}

Eval pvsBlack(const Board &b, const int &depth, int alpha = INT_MIN, const int &beta = INT_MAX, const int &depth2 = 0, const bool &prevPass = false);
Eval pvsWhite(const Board &b, const int &depth, const int &alpha = INT_MIN, int beta = INT_MAX, const int &depth2 = 0, const bool &prevPass = false);
int pvsCalls = 0;
int tthits = 0;
uint8_t killerMoves[MAX_DEPTH][3];

bool isEndgame(const int &currentDiscs, const int &depth) {
	return false;
}

Eval pvs(const Board &b, const int &depth, const Side &s, int alpha = INT_MIN, int beta = INT_MAX, const int &depth2 = 0, const bool &prevPass = false) {
	if (s) return pvsBlack(b, depth, alpha, beta, depth2, prevPass);
	else return pvsWhite(b, depth, alpha, beta, depth2, prevPass);
}

Eval pvsBlack(const Board &b, const int &depth, int alpha, const int &beta, const int &depth2, const bool &prevPass) {
	pvsCalls++;
	
	//~ bool endgame_search = isEndgame(currentDiscs, depth2);
	//~ if (endgame_search) return endgame_alphabeta(b, depth, BLACK);
	
	if (depth <= 1) return alphabeta(b, depth, BLACK, alpha, beta, depth2, prevPass);
		
	BoardWithSide bws(b.taken, b.black, BLACK);
	uint64_t legalMoves = b.findLegalMovesBlack();
	
	// Special case
	if (__builtin_expect(!legalMoves, 0)) {
		if (prevPass) {
			int blacks = b.countBlack();
			int whites = b.countWhite();
			return (blacks > whites) ? Eval(b, INT_MAX) : ((whites > blacks) ? Eval(b, INT_MIN) : Eval(b, 0));
		}
		return pvsWhite(b, depth - 1, alpha, beta, depth2 + 1, true);
	}
	
	uint8_t besti = 64;
	int v = INT_MIN;
	Board pvBoard;
	// Best move
	uint8_t index = 64;
	if (depth2 <= HASH_DEPTH) {
		size_t hash_index = b.zobrist_hash & (tt.mod - 1);
		index = tt.table[hash_index];
		if (BIT_SAFE(index) & legalMoves) {
			#if IID
			use_hash:
			#endif
			// The move is valid
			tthits++;
			pVariations[depth2]++;
			legalMoves ^= BIT(index);
			besti = index;
			Eval e = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
			v = e.e;
			pvBoard = e.pvBoard;
			alpha = (v > alpha) ? v : alpha;
			// Prove that it is indeed the best move
			while (alpha < beta && legalMoves) {	
				index = __builtin_clzl(legalMoves);
				legalMoves ^= BIT(index);
				Board b2 = b.doMoveOnNewBoardBlack(index);
				Eval e = pvsWhite(b2, depth - 1, alpha, alpha + 1, depth2 + 1);
				v = e.e;
				// Either v = alpha + 1 or v <= alpha
				// Also, either beta = alpha + 1 or beta > alpha + 1
				if (__builtin_expect(v >= beta, 0)) {
					pVariations[depth2]++;
					pvBoard = e.pvBoard;
					besti = index;
					alpha = v;
					break;
				}
				else if (__builtin_expect(v >= alpha + 1, 0)) {
					pVariations[depth2]++;
					e = pvsWhite(b2, depth - 1, v, beta, depth2 + 1);
					pvBoard = e.pvBoard;
					besti = index;
					alpha = e.e;
				}
			}
		}
		else {
			#if IID
			if (depth > 2) {
				pvsBlack(b, depth - 1, alpha, beta, depth2); // Changing HASH_DEPTH will cause an infinite loop here
				index = tt.table[b.zobrist_hash & (tt.mod - 1)];
				goto use_hash;
			}
			#endif
			// Fall back to normal search
			while (alpha < beta && legalMoves) {	
				uint8_t index = __builtin_clzl(legalMoves);
				legalMoves ^= BIT(index);
				Eval e = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
				int val = e.e;
				if (val > v) {
					pVariations[depth2]++;
					pvBoard = e.pvBoard;
					besti = index;
					v = val;
				}	
				alpha = (v > alpha) ? v : alpha;
			}
		}
		if (besti == 64) {
			// Completely lost...make any legal move
			besti = __builtin_clzl(b.findLegalMovesBlack());
		}
		tt.table[hash_index] = besti;
	}
	else {
		while (alpha < beta && legalMoves) {	
			uint8_t index = __builtin_clzl(legalMoves);
			legalMoves ^= BIT(index);
			Eval e = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
			if (e.e > alpha) {
				pvBoard = e.pvBoard;
				alpha = e.e;
			}
		}
	}
	return Eval(pvBoard, alpha);
}

Eval pvsWhite(const Board &b, const int &depth, const int &alpha, int beta, const int &depth2, const bool &prevPass) {
	pvsCalls++;
	
	if (depth <= 1) return alphabeta(b, depth, WHITE, alpha, beta, depth2, prevPass);
	
	BoardWithSide bws(b.taken, b.black, WHITE);
	uint64_t legalMoves = b.findLegalMovesWhite();
		
	// Special case
	if (__builtin_expect(!legalMoves, 0)) {
		if (prevPass) {
			int blacks = b.countBlack();
			int whites = b.countWhite();
			return (blacks > whites) ? Eval(b, INT_MAX) : ((whites > blacks) ? Eval(b, INT_MIN) : Eval(b, 0));
		}
		return pvsBlack(b, depth - 1, alpha, beta, depth2 + 1, true);
	}
	
	uint8_t besti = 64;
	uint8_t index = 64;
	Board pvBoard;
	// Best move
	if (depth2 <= HASH_DEPTH) {
		int v = INT_MAX;
		size_t hash_index = b.zobrist_hash & (tt.mod - 1);
		index = tt.table[hash_index];
		if (BIT_SAFE(index) & legalMoves) {
			#if IID
			use_hash:
			#endif
			// The move is valid
			tthits++;
			pVariations[depth2]++;
			legalMoves ^= BIT(index);
			besti = index;
			Eval e = pvsBlack(b.doMoveOnNewBoardWhite(index), depth - 1, alpha, beta, depth2 + 1);
			v = e.e;
			pvBoard = e.pvBoard;
			beta = (v < beta) ? v : beta;
			// Prove that it is indeed the best move
			while (alpha < beta && legalMoves) {	
				index = __builtin_clzl(legalMoves);
				legalMoves ^= BIT(index);
				Board b2 = b.doMoveOnNewBoardWhite(index);
				Eval e = pvsBlack(b2, depth - 1, beta - 1, beta, depth2 + 1);
				v = e.e;
				if (__builtin_expect(v <= alpha, 0)) {
					pVariations[depth2]++;
					pvBoard = e.pvBoard;
					besti = index;
					beta = v;
					break;
				}
				else if (__builtin_expect(v <= beta - 1, 0)) {
					pVariations[depth2]++;
					e = pvsBlack(b2, depth - 1, alpha, v, depth2 + 1);
					pvBoard = e.pvBoard;
					besti = index;
					beta = e.e;
				}
			}
		}
		else {
			// Fall back to normal search
			#if IID
			if (depth > 2) {
				pvsWhite(b, depth - 1, alpha, beta, depth2); // Changing HASH_DEPTH will cause an infinite loop here
				index = tt.table[b.zobrist_hash & (tt.mod - 1)];
				goto use_hash;
			}
			#endif
			while (alpha < beta && legalMoves) {	
				uint8_t index = __builtin_clzl(legalMoves);
				legalMoves ^= BIT(index);
				Eval e = pvsBlack(b.doMoveOnNewBoardWhite(index), depth - 1, alpha, beta, depth2 + 1);
				int val = e.e;
				if (val < v) {
					pVariations[depth2]++;
					pvBoard = e.pvBoard;
					besti = index;
					v = val;
				}
				beta = (v < beta) ? v : beta;
			}
		}
		if (besti == 64) {
			// Completely lost...make any move
			besti = __builtin_clzl(b.findLegalMovesWhite());
		}
		tt.table[hash_index] = besti;
	}
	else {
		while (alpha < beta && legalMoves) {
			uint8_t index = __builtin_clzl(legalMoves);
			legalMoves ^= BIT(index);
			Eval e = pvsBlack(b.doMoveOnNewBoardWhite(index), depth - 1, alpha, beta, depth2 + 1);
			if (e.e < beta) {
				pvBoard = e.pvBoard;
				beta = e.e;
			}
		}
	}
	return Eval(pvBoard, beta);
}

void print_counts() {
	cerr << "[";
	for (int i = 0; i < 14; i++) cerr << pVariations[i] << ", ";
	cerr << pVariations[14] << "}" << endl;
}

void clear_counts() {
	for (int i = 0; i < 15; i++) pVariations[i] = 0;
}

pair<int, int> main_minimax_aw(const Board &b, const Side &s, const int &depth, int guess = -1) {
	Timer tim;
	
	clear_counts();
	
	int eOdd, eEven;
	// First two plies
	eOdd = pvs(b, 1, s, INT_MIN, INT_MAX).e;
	print_counts(); clear_counts();
	cerr << "Finished depth 1: " << eOdd << endl;
	eEven = pvs(b, 2, s, INT_MIN, INT_MAX).e;
	print_counts(); clear_counts();
	cerr << "Finished depth 2: " << eEven << endl;
	
	// Middle plies
	for (int d = 3; d < depth; d++) {
		if (d % 2) {
			int diff = abs(eOdd) / 8 + 2;
			int lower = (eOdd == INT_MIN) ? INT_MIN : eOdd - diff;
			int upper = (eOdd == INT_MAX) ? INT_MAX : eOdd + diff;
			int counter = 1;
			try_again1:
			eOdd = pvs(b, d, s, lower, upper).e;
			print_counts(); clear_counts();
			if (eOdd <= lower && lower != INT_MIN) {
				tim.endms("Recalculating (failed low) ");
				lower -= (diff - 1) * counter;
				counter++;
				if (counter > 3) lower = INT_MIN;
				goto try_again1;
			}
			else if (eOdd >= upper && upper != INT_MAX) {
				tim.endms("Recalculating (failed high) ");
				upper += (diff - 1) * counter;
				counter++;
				if (counter > 3) upper = INT_MAX;
				goto try_again1;
			}
			cerr << "Finished depth " << d << " search: " << eOdd << ' ';
			tim.endms();
		}
		else {
			int diff = abs(eEven) / 8 + 2;
			int lower = (eEven == INT_MIN) ? INT_MIN : eEven - diff;
			int upper = (eEven == INT_MAX) ? INT_MAX : eEven + diff;
			int counter = 1;
			try_again2:
			eEven = pvs(b, d, s, lower, upper).e;
			print_counts(); clear_counts();
			if (eEven <= lower && lower != INT_MIN) {
				tim.endms("Recalculating (failed low) ");
				lower -= (diff - 1) * counter;
				counter++;
				if (counter > 3) lower = INT_MIN;
				goto try_again2;
			}
			else if (eEven >= upper && upper != INT_MAX) {
				tim.endms("Recalculating (failed high) ");
				upper += (diff - 1) * counter;
				counter++;
				if (counter > 3) upper = INT_MAX;
				goto try_again2;
			}
			cerr << "Finished depth " << d << " search: " << eEven << ' ';
			tim.endms();
		}
	}
	// Last one
	if (depth % 2) {
		int counter = 1;
		int diff = abs(eOdd) / 8 + 2;
		int lower = (eOdd == INT_MIN) ? INT_MIN : eOdd - diff;
		int upper = (eOdd == INT_MAX) ? INT_MAX : eOdd + diff;
		try_again3:
		eOdd = pvs(b, depth, s, lower, upper, false).e;
		// This is bound to run into issues one day
		pair<int, int> result = make_pair(tt.table[b.zobrist_hash & (tt.mod - 1)], eOdd);
		if (result.second <= lower && lower != INT_MIN) {
			tim.endms("Recalculating (failed low) ");
			lower -= (diff - 1) * counter;
			counter++;
			if (counter > 3) lower = INT_MIN;
			goto try_again3;
		}
		else if (result.second >= upper && upper != INT_MAX) {
			tim.endms("Recalculating (failed high) ");
			upper += (diff - 1) * counter;
			counter++;
			if (counter > 3) upper = INT_MAX;
			goto try_again3;
		}
		print_counts(); clear_counts();
		cerr << "Finished depth " << depth + 1 << " search: " << result.second << ' ';
		tim.endms();
		return result;
	}
	else {
		int counter = 1;
		int diff = abs(eEven) / 8 + 2;
		int lower = (eEven == INT_MIN) ? INT_MIN : eEven - diff;
		int upper = (eEven == INT_MAX) ? INT_MAX : eEven + diff;
		try_again4:
		eEven = pvs(b, depth, s, lower, upper, false).e;
		// This is bound to run into issues one day
		pair<int, int> result = make_pair(tt.table[b.zobrist_hash & (tt.mod - 1)], eEven);
		if (result.second <= lower && lower != INT_MIN) {
			tim.endms("Recalculating (failed low) ");
			lower -= (diff - 1) * counter;
			counter++;
			if (counter > 3) lower = INT_MIN;
			goto try_again4;
		}
		else if (result.second >= upper && upper != INT_MAX) {
			tim.endms("Recalculating (failed high) ");
			upper += (diff - 1) * counter;
			counter++;
			if (counter > 3) upper = INT_MAX;
			goto try_again4;
		}
		print_counts(); clear_counts();
		cerr << "Finished depth " << depth << " search: " << result.second << ' ';
		tim.endms();
		return result;
	}
}

int deep_endgame_alphabeta(const Board &b, const Side &s, int alpha = INT_MIN, int beta = INT_MAX) {
	int totalCount = __builtin_popcountll(b.taken);
	
	if (totalCount == 64) {
		//auto start = chrono::high_resolution_clock::now();
		globalEndgameNodeCount++;
		if (globalEndgameNodeCount % (DEFAULT_MAX_NODES / 10) == 0) cerr << globalEndgameNodeCount << endl;
		double maxNodes = minutesForMove * DEFAULT_MAX_NODES;
		if (globalEndgameNodeCount > maxNodes) abortEndgameMinimax = true;
		// Simple evaluation here
		int diff = __builtin_popcountll(b.black);
		if (diff > 32) return INT_MAX;
		else if (diff < 32) return INT_MIN;
		else return 0;
	}
	
	uint64_t legalMoves = b.findLegalMoves(s);
	
	if (legalMoves == 0) {
		uint64_t legalMovesOther = b.findLegalMoves(other_side(s));
		if (legalMovesOther == 0) {
			globalEndgameNodeCount++;
			if (globalEndgameNodeCount % (DEFAULT_MAX_NODES / 10) == 0) cerr << globalEndgameNodeCount << endl;
			double maxNodes = minutesForMove * DEFAULT_MAX_NODES;
			if (globalEndgameNodeCount > maxNodes) abortEndgameMinimax = true;
			// Simple evaluation here
			int diff = b.countBlack() - b.countWhite();
			if (diff > 0) return INT_MAX;
			else if (diff < 0) return INT_MIN;
			else return 0;
		}
		return deep_endgame_alphabeta(b, other_side(s), alpha, beta);
	}
	
	/*
	if (totalCount == 63) {
		int blackCount = __builtin_popcountll(b.black);
		int diff = 2 * blackCount - 63;
		if (s == BLACK) {
			// Black has a legal move, so will gain at 3 disc differential
			if (diff + 3 > 0) return INT_MAX;
			if (diff + 37 < 0) return INT_MIN;
		}
		else {
			// White has a legal move, so will gain at most 
			// 2 * 18 + 1 = 37 differential
			// Equivalently, we can check that blackCount > 50
			if (diff - 37 > 0) return INT_MAX;
			if (diff - 3 < 0) return INT_MIN;
		}
	}
	*/

	if (s) {
		while (alpha < beta && legalMoves) {
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= BIT(index);
			int v = deep_endgame_alphabeta(b.doMoveOnNewBoardBlackWZH(index), WHITE, alpha, beta);
			alpha = (v > alpha) ? v : alpha;
		}
	}
	else {
		while (alpha < beta && legalMoves) {
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= BIT(index);
			int v = deep_endgame_alphabeta(b.doMoveOnNewBoardWhiteWZH(index), BLACK, alpha, beta);
			beta = (v < beta) ? v : beta;
		}
	}
	
	return s ? alpha : beta;
}

int endgame_alphabeta(const Board &b, const Side &s, int alpha, int beta) {
	if (abortEndgameMinimax) return (abortSide == BLACK) ? INT_MIN : INT_MAX;
	
	BoardWithSide bws(b.taken, b.black, s);
	if (um2->count(bws) > 0) {
		return (*um2)[bws];
	}
	
	int original_alpha = alpha;
	int original_beta = beta;
	
	int totalCount = __builtin_popcountll(b.taken);
	
	if (totalCount >= 42 + HASH_DEPTH) return deep_endgame_alphabeta(b, s, alpha, beta);
	
	double maxNodes = minutesForMove * DEFAULT_MAX_NODES;
	
	uint64_t legalMoves = b.findLegalMoves(s);
	
	if (legalMoves == 0) {
		uint64_t legalMovesOther = b.findLegalMoves(other_side(s));
		if (legalMovesOther == 0) {
			globalEndgameNodeCount++;
			if (globalEndgameNodeCount % (DEFAULT_MAX_NODES / 10) == 0) cerr << globalEndgameNodeCount << endl;
			if (globalEndgameNodeCount > maxNodes) abortEndgameMinimax = true;
			// Simple evaluation here
			int diff = b.countBlack() - b.countWhite();
			if (diff > 0) return INT_MAX;
			else if (diff < 0) return INT_MIN;
			else return 0;
		}
		int ret = endgame_alphabeta(b, other_side(s), alpha, beta);
		if (totalCount < STOP_SAVING_THRESHOLD) {
			(*um2)[bws] = ret;
		}
		return ret;
	}
	
	if (s) {
		#if USE_HASH_IN_ENDGAME_ALPHABETA
		size_t hash_index = b.zobrist_hash & (tt.mod - 1);
		int index = tt.table[hash_index];
		if (BIT_SAFE(index) & legalMoves) {
			// The move is valid
			legalMoves ^= BIT(index);
			int v = endgame_alphabeta(b.doMoveOnNewBoardBlack(index), WHITE, alpha, beta);
			alpha = (v > alpha) ? v : alpha;
		}
		#endif
		while (alpha < beta && legalMoves) {
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= BIT(index);
			int v = endgame_alphabeta(b.doMoveOnNewBoardBlack(index), WHITE, alpha, beta);
			alpha = (v > alpha) ? v : alpha;
		}
	}
	else {
		#if USE_HASH_IN_ENDGAME_ALPHABETA
		size_t hash_index = b.zobrist_hash & (tt.mod - 1);
		int index = tt.table[hash_index];
		if (BIT_SAFE(index) & legalMoves) {
			legalMoves ^= BIT(index);
			int v = endgame_alphabeta(b.doMoveOnNewBoardWhite(index), BLACK, alpha, beta);
			beta = (v < beta) ? v : beta;
		}
		#endif
		while (alpha < beta && legalMoves) {
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= BIT(index);
			int v = endgame_alphabeta(b.doMoveOnNewBoardWhite(index), BLACK, alpha, beta);
			beta = (v < beta) ? v : beta;
		}
	}
	
	int ret = s ? alpha : beta;
	// Reasoning:
	// If alpha, beta = -inf, inf, any result is trustworthy. Otherwise... 
	// ...if black:
	// -inf, 0 -> -inf, inf
	// 0, inf -> 0, inf
	// ...if white:
	// -inf, 0 -> -inf, 0
	// 0, inf -> -inf, inf
	if ((original_alpha == INT_MIN && original_beta == INT_MAX) || 
		(s && (alpha == INT_MIN || original_alpha == alpha)) || 
		(!s && (beta == INT_MAX || original_beta == beta))) {
		(*um2)[bws] = ret;
	}
	return ret;
}

pair<int, int> endgame_minimax(Board &b, Side s, int guess = -1) {
	uint64_t legalMoves = b.findLegalMoves(s);
	if (legalMoves == 0) return make_pair(-1, -10000); // Second value is meaningless here
	
	int besti = -1;
	int v;
	if (s == BLACK) {
		v = INT_MIN;
		if (guess > -1) {
			legalMoves ^= SINGLE_BIT[guess];
			Board b2 = b.doMoveOnNewBoard(guess, s);
			int val = endgame_alphabeta(b2, other_side(s), v, INT_MAX);
			if (val > v) {
				besti = guess;
				v = val;
			}
		}
		while (legalMoves && v != INT_MAX) {
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(index, s);
			int val = endgame_alphabeta(b2, other_side(s), v, INT_MAX);
			if (val > v) {
				besti = index;
				v = val;
			}
		}
	}
	else {
		v = INT_MAX;
		if (guess > -1) {
			legalMoves ^= SINGLE_BIT[guess];
			Board b2 = b.doMoveOnNewBoard(guess, s);
			int val = endgame_alphabeta(b2, other_side(s), INT_MIN, v);
			if (val < v) {
				besti = guess;
				v = val;
			}
		}
		while (legalMoves && v != INT_MIN) {
			int index = __builtin_clzl(legalMoves);
			legalMoves ^= BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(index, s);
			int val = endgame_alphabeta(b2, other_side(s), INT_MIN, v);
			if (val < v) {
				besti = index;
				v = val;
			}
		}
	}
	if (besti == -1) cerr << "Endgame minimax: no good move " << v << endl;
	return make_pair(besti, v);
}

bool gameSolved = false;

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
    // Set minutesForMove to half the amount of time left
    if (msLeft > 0) minutesForMove = msLeft / 60.0 / 1000 / 2;
    
    if (opponentsMove != NULL) {
		currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(opponentsMove->x, opponentsMove->y), other_side(side));
	}
    
    uint64_t legalMoves = currBoard.findLegalMoves(side);
    if (!legalMoves) return NULL;
    
    currentDiscs = __builtin_popcountll(currBoard.taken);
    if (currentDiscs == 4) {
		// Move e6 without thinking
		currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(4, 5), side);
		Move *move = new Move(4, 5);
		return move;
	}
	else if (currentDiscs == 5) {
		// Choose diagonal opening
		int index = __builtin_clzl(legalMoves & (BIT(18) | BIT(21) | BIT(42) | BIT(45)));
		int x = FROM_INDEX_X(index);
		int y = FROM_INDEX_Y(index);
		if (x == -1) return NULL;
		currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(x, y), side);
		Move *move = new Move(x, y);
		return move;
	}
    
    /*
    pair<int, int> ret_value = make_pair(-3, -1000);
    thread t([&ret_value, totalCount] (Board b, Side s) {
		// mobilityNodes = 0;
		cerr << "Starting thread" << endl;
		if (totalCount <= 25) ret_value = main_minimax2(b, s, 11);
		else if (totalCount <= 41) ret_value = main_minimax2(b, s, 11);
		else ret_value = make_pair(-3, -1000);
		if (ret_value.first < 0) return;
		string letters = "abcdefgh";
		cerr << "Second thread result: " << letters[FROM_INDEX_X(ret_value.first)] << ' ' << FROM_INDEX_Y(ret_value.first) + 1 << ' ' << ret_value.second << endl; //' ' << mobilityNodes << endl;
	}, currBoard, side);
    */
    
    // Set depth according to how far into game
    int depth;
    if (currentDiscs <= 20) depth = MAX_DEPTH;
    else if (currentDiscs <= 30) depth = MAX_DEPTH;
    else if (currentDiscs <= 41) depth = MAX_DEPTH;
    else depth = INT_MAX; // Search to end (much faster)
	
    // Set counter, reset abort variables
    globalEndgameNodeCount = 0;
    abCalls = 0;
    pvsCalls = 0;
    tthits = 0;
    um2->clear();
    abortEndgameMinimax = false;
    abortSide = side;
    
    int eval = -100; // Just a value
    // Find index of best move via search
    int besti = -1;
    pair<int, int> p;
    if (depth != INT_MAX) {
		//BoardWithSide bws(currBoard.taken, currBoard.black, side);
		//if (um->count(bws) > 0) besti = (*um)[bws];
		p = main_minimax_aw(currBoard, side, depth, besti);
		besti = p.first;
		eval = p.second;
	}
	else {
		cerr << "Endgame search" << endl;
		if (!gameSolved) {
			pair<int, int> p2 = main_minimax_aw(currBoard, side, MAX_DEPTH);
			auto start = chrono::high_resolution_clock::now();
			p = endgame_minimax(currBoard, side, p2.first);
			cerr << chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
			besti = p.first;
			eval = p.second;
			// If could not calculate a win or draw, fall back to other algorithm
			if (besti < 0) {
				besti = p2.first;
				eval = p2.second;
				um2->clear(); // For now, some values may be incorrect if search not done, later we may want to prune the hash table, if it's worth it
			}
			else {
				gameSolved = true;
				cerr << "Game solved" << endl;
			}
		}
		else {
			p = endgame_minimax(currBoard, side);
			besti = p.first;
			eval = p.second;
		}
	}
    
    // Prune um
    /*
    cerr << "Pruning um..." << endl;
    unordered_map<BoardWithSide, vector<int> *>::iterator um_it = um->begin();
    while (um_it != um->end()) {
		auto pr = *um_it;
		if (pr.first.count() <= totalCount && pr.second) {
			delete pr.second;
			um_it = um->erase(um_it);
		}
		else um_it++;
	}
	cerr << "...done pruning." << endl;
	*/
	
    // Temporary
    //int v1 = main_minimax(currBoard, 4, side).second;
    //int v2 = main_minimax(currBoard, 8, side).second;
    //fil << totalCount << ' ' << v1 << ' ' << v2 << endl;
    
    // Count collisions
    //~ int positions = 0;
    //~ for (size_t i = 0; i < tt.mod; i++) {
		//~ if (tt.table[i] != 64) positions++;
	//~ }
    
    // Output some useful info and return (remove for more speed)
    cerr << currentDiscs + 1 << " eval: " << eval << ' ' << pvsCalls << ' ' << tthits << endl; //' ' << positions << endl;
	if (depth == INT_MAX) cerr << ' ' << globalEndgameNodeCount << endl;
	//~ cerr << "Time wasted " << timeWasted << endl;
	// if (um->size() > MAX_HASH_SIZE) um->clear(); // Don't want to lose due to too much memory!
	
	// Make move
	string letters = "abcdefgh";
	// cerr << ((side == BLACK) ? "Black " : "White ") << "tentative move: " << letters[FROM_INDEX_X(besti)] << ' ' << FROM_INDEX_Y(besti) + 1 << endl; 
	//t.join();
    //if (totalCount <= 41 && ret_value.first >= 0 && ((side == BLACK) ? (eval < ret_value.second) : (eval > ret_value.second))) besti = ret_value.first; // Better move
	int x = FROM_INDEX_X(besti);
	int y = FROM_INDEX_Y(besti);
	cerr << ((side == BLACK) ? "Black " : "White ") << "Move: " << letters[x] << ' ' << y + 1 << "       " << besti << endl;
	currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(x, y), side);
    //bitset<64> bsTaken(currBoard.taken), bsBlack(currBoard.black);
    //cerr << bsTaken << endl << bsBlack << endl;
    Move *move = new Move(x, y);
    return move;
    
}
