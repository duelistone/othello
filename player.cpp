#include "player.h"

Side abortSide;

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
    //int wedgeWeight = 2;
    int unstableWeightC = -6;
    
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
	
	int count = __builtin_popcount(data >> 8);
	
	for (int i = 0; i < 8; i++) {
		// If empty, continue
		if ((data & EDGE_BIT(i)) == 0) continue;
		
		Side s = (data & EDGE_BIT(8 + i)) ? BLACK : WHITE;
		
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
		while (x >= 0 && ((data & EDGE_BIT(x)) == EDGE_BIT(x))) {
			bool iset = ((data & EDGE_BIT(8 + i)) == EDGE_BIT(8 + i));
			bool xset = ((data & EDGE_BIT(8 + x)) == EDGE_BIT(8 + x));
			if ((iset && !xset) || ((!iset) && xset)) break;
			x--;
		}
		if (x == -1) {
			if (s == BLACK) eval += stableWeight;
			else eval -= stableWeight;
			continue;
		}
		else if (i == 1 || i == 6) {
			if (s == BLACK) eval += unstableWeightC;
			else eval -= unstableWeightC;
			continue;
		}
		
		x = i;
		x++;
		while (x < 8 && ((data & EDGE_BIT(x)) == EDGE_BIT(x))) {
			bool iset = ((data & EDGE_BIT(8 + i)) == EDGE_BIT(8 + i));
			bool xset = ((data & EDGE_BIT(8 + x)) == EDGE_BIT(8 + x));
			if ((iset && !xset) || ((!iset) && xset)) break;
			x++;
		}
		if (x == 8) {
			if (s == BLACK) eval += stableWeight;
			else eval -= stableWeight;
		}
		else if (i == 1 || i == 6) {
			if (s == BLACK) eval += unstableWeightC;
			else eval -= unstableWeightC;
		}
	}
	
	EDGE_VALUES[data] = eval;
	
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

/*
 * Destructor for the player.
 */
Player::~Player() {
}

int alphabeta(Board b, int depth, Side s, int alpha = INT_MIN, int beta = INT_MAX, bool prevPass = false) {
	uint64_t legalMoves = b.findLegalMoves(s);
	//int totalCount = __builtin_popcount(b.taken >> 32) + __builtin_popcount(b.taken);
	
	if (depth == 0) {
		// Parity
		// int parityWeight = 0;
		return b.evaluate();// + ( totalCount % 2 ? ( s == BLACK ? parityWeight : -parityWeight) : ( s == BLACK ? -parityWeight : parityWeight ) );
	}
	
	BoardWithSide bws(b.taken, b.black, s);
	
	// Special case
	if (legalMoves == 0) {
		if (prevPass) {
			int blacks = b.countBlack();
			int whites = b.countWhite();
			return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
		}
		return alphabeta(b, depth - 1, OTHER_SIDE(s), alpha, beta, true);
	}
	
	int besti = -1;
	if (s == BLACK) {
		int v = INT_MIN;
		if (um->count(bws) > 0) {
			int guess = (*um)[bws];
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta(b2, depth - 1, OTHER_SIDE(s), alpha, beta);
			besti = guess;
			alpha = MAX(v, alpha);
		}
		while (legalMoves != 0 && alpha < beta) {	
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s), alpha, beta);
			if (val > v || besti == -1) besti = index;
			v = MAX(v, val);
			alpha = MAX(v, alpha);
		}
		if (depth > 3) (*um)[bws] = besti;
		return alpha;
	}
	else {
		int v = INT_MAX;
		if (um->count(bws) > 0) {
			int guess = (*um)[bws];
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta(b2, depth - 1, OTHER_SIDE(s), alpha, beta);
			besti = guess;
			beta = MIN(v, beta);
		}
		while (legalMoves != 0 && alpha < beta) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s), alpha, beta);
			if (val < v || besti == -1) besti = index;
			v = MIN(v, val);
			beta = MIN(v, beta);
		}
		if (depth > 3) (*um)[bws] = besti;
		return beta;
	}
}

pair<int, int> main_minimax(Board b, int depth, Side s, int guess = -1) {
	cerr << "Calling main_minimax with guess " << guess << endl;
	// Depth must be greater than or equal to 1
	uint64_t legalMoves = b.findLegalMoves(s);
	if (legalMoves == 0) return make_pair(-1, -100); // Evaluation here has no meaning
	
	int besti = -1;
	if (s == BLACK) {
		int v = INT_MIN;
		if (guess >= 0) {
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta(b2, depth - 1, OTHER_SIDE(s));
			besti = guess;
			
			cerr << besti << ' ' << v << endl;
		}
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s), v, INT_MAX);
			if (val > v || besti == -1) {
				besti = index;
				v = val;
			}
			cerr << besti << ' ' << v << endl;
		}
		return make_pair(besti, v);
	}
	else {
		int v = INT_MAX;
		if (guess >= 0) {
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			v = alphabeta(b2, depth - 1, OTHER_SIDE(s));
			besti = guess;
		}
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = alphabeta(b2, depth - 1, OTHER_SIDE(s), INT_MIN, v);
			if (val < v || besti == -1) {
				besti = index;
				v = val;
			}
		}
		return make_pair(besti, v);
	}		
}

int endgame_alphabeta(Board b, Side s, int alpha = INT_MIN, int beta = INT_MAX) {
	if (abortEndgameMinimax) return (abortSide == BLACK) ? INT_MIN : INT_MAX;
	
	BoardWithSide bws(b.taken, b.black, s);
	if (um2->count(bws) > 0) {
		int result = (*um2)[bws];
		return result;
	}
	
	int totalCount = __builtin_popcount(b.taken) + __builtin_popcount(b.taken >> 32);
	uint64_t legalMoves = b.findLegalMoves(s);
	
	if (legalMoves == 0) {
		uint64_t legalMovesOther = b.findLegalMoves(OTHER_SIDE(s));
		if (legalMovesOther == 0) {
			globalEndgameNodeCount++;
			if (globalEndgameNodeCount % (DEFAULT_MAX_NODES / 10) == 0) cerr << globalEndgameNodeCount << endl;
			if (globalEndgameNodeCount > minutesForMove * DEFAULT_MAX_NODES) abortEndgameMinimax = true;
			// Simple evaluation here
			int diff = b.countBlack() - b.countWhite();
			if (diff > 0) return INT_MAX;
			else if (diff < 0) return INT_MIN;
			else return 0;
		}
		int ret = endgame_alphabeta(b, OTHER_SIDE(s));
		if (um2->size() < MAX_HASH_SIZE && totalCount < STOP_SAVING_THRESHOLD) {
			(*um2)[bws] = ret;
		}
		return ret;
	}
	
	if (s == BLACK) {
		int v = INT_MIN;
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s), alpha, beta);
			v = MAX(v, val);
			alpha = MAX(v, alpha);
			if (beta <= alpha) break;
		}
	}
	else {
		int v = INT_MAX;
		while (legalMoves != 0) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s), alpha, beta);
			v = MIN(v, val);
			beta = MIN(v, beta);
			if (beta <= alpha) break;
		}
	}
	
	int ret = (s == BLACK) ? alpha : beta;
	if (um2->size() < MAX_HASH_SIZE && totalCount < STOP_SAVING_THRESHOLD) {
		if (alpha != 0 || beta != 0) (*um2)[bws] = ret;
	}
	return ret;
}

pair<int, int> endgame_minimax(Board b, Side s, int guess = -1) {
	uint64_t legalMoves = b.findLegalMoves(s);
	if (legalMoves == 0) return make_pair(-1, -10000); // Second value is meaningless here
	
	int besti = -1;
	int v;
	if (s == BLACK) {
		v = INT_MIN;
		if (guess > -1) {
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s), v, INT_MAX);
			besti = guess;
			v = val;
		}
		while (legalMoves != 0 && v != INT_MAX) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s), v, INT_MAX);
			if (val > v) {
				besti = index;
				v = val;
			}
		}
	}
	else {
		v = INT_MAX;
		if (guess > -1) {
			legalMoves &= ~BIT(guess);
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(guess), FROM_INDEX_Y(guess), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s), INT_MIN, v);
			besti = guess;
			v = val;
		}
		while (legalMoves != 0 && v != INT_MIN) {
			int index = __builtin_clzl(legalMoves);
			legalMoves &= ~BIT(index);
			
			Board b2 = b.doMoveOnNewBoard(FROM_INDEX_X(index), FROM_INDEX_Y(index), s);
			int val = endgame_alphabeta(b2, OTHER_SIDE(s), INT_MIN, v);
			if (val < v) {
				besti = index;
				v = val;
			}
		}
	}
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
    // Time!
    time_t startTime = time(NULL);
    
    // Set minutesForMove to half the amount of time left
    if (msLeft > 0) minutesForMove = msLeft / 60.0 / 1000 / 2;
    
    if (opponentsMove != NULL) {
		currBoard.doMove(opponentsMove->x, opponentsMove->y, OTHER_SIDE(side));
	}
    currBoard.findLegalMoves(side);
    if (currBoard.legalMoves == 0) return NULL;
    
    int totalCount = currBoard.countBlack() + currBoard.countWhite();
    if (totalCount == 4) {
		// Move e6 without thinking
		currBoard.doMove(4, 5, side);
		Move *move = new Move(4, 5);
		return move;
	}
	else if (totalCount == 5) {
		// Choose diagonal opening
		int index = __builtin_clzl(currBoard.legalMoves & (BIT(18) | BIT(21) | BIT(42) | BIT(45)));
		int x = FROM_INDEX_X(index);
		int y = FROM_INDEX_Y(index);
		currBoard.doMove(x, y, side);
		Move *move = new Move(x, y);
		return move;
	}
    
    // Set depth according to how far into game
    int depth;
    if (totalCount <= 46) depth = 10; //41
    else depth = INT_MAX; // Search to end (much faster)
	
    // Set counter, reset abort variables
    globalEndgameNodeCount = 0;
    abortEndgameMinimax = false;
    abortSide = side;
    
    int eval = -100; // Just a value
    // Find index of best move via search
    int besti = -1;
    pair<int, int> p;
    if (depth != INT_MAX) {
		BoardWithSide bws(currBoard.taken, currBoard.black, side);
		if (um->count(bws) > 0) besti = (*um)[bws];
		else cerr << "Unexpected position" << endl;
		p = main_minimax(currBoard, depth, side, besti);
		besti = p.first;
		eval = p.second;
	}
	else {
		if (!gameSolved) {
			pair<int, int> p2 = main_minimax(currBoard, 10, side);
			p = endgame_minimax(currBoard, side, p2.first);
			besti = p.first;
			eval = p.second;
			// If could not calculate a win or draw, fall back to other algorithm
			if (p.second == ((side == BLACK) ? INT_MIN : INT_MAX)) {
				cerr << "Just minimaxing" << endl;
				// p = main_minimax(currBoard, msLeft > 90000 ? 11 : (msLeft > 30000 ? 10 : (msLeft > 10000 ? 9 : 8)), side);
				besti = p2.first;
				eval = p2.second;
				cerr << "Done minimaxing" << endl;
				um2->clear(); // For now, some values may be incorrect if search not done, later we may want to prune the hash table, if it's worth it
			}
			else {
				gameSolved = true;
			}
		}
		else {
			p = endgame_minimax(currBoard, side);
			besti = p.first;
			eval = p.second;
		}
	}
    
    
    // Output some useful info and return (remove for more speed)
	cerr << totalCount + 1 << ' ' << msLeft - 1000 * difftime(time(NULL), startTime) << " eval: " << eval << " umsize: " << um->size();
	if (depth == INT_MAX) cerr << ' ' << globalEndgameNodeCount;
	cerr << endl;
	
	if (um->size() > MAX_HASH_SIZE) um->clear(); // Don't want to lose due to too much memory!
	
	// Make move
	int x = FROM_INDEX_X(besti);
	int y = FROM_INDEX_Y(besti);
	string letters = "abcdefgh";
	cerr << "Move: " << letters[x] << ' ' << y + 1 << ' ' << besti << endl;
    currBoard.doMove(x, y, side);
    Move *move = new Move(x, y);
    return move;
    
}
