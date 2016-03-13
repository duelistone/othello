#include "player.h"

Side abortSide;

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side s) : side(s), currBoard(Board()) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;    
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}

int alphabeta(Board b, int depth, Side s, int alpha = INT_MIN, int beta = INT_MAX, bool prevPass = false) {
	uint64_t legalMoves = b.findLegalMoves(s);
	int totalCount = __builtin_popcount(b.taken >> 32) + __builtin_popcount(b.taken);
	
	if (depth == 0) {
		// Parity
		int parityWeight = 2;
		return b.evaluate() + ( totalCount % 2 ? ( s == BLACK ? parityWeight : -parityWeight) : ( s == BLACK ? -parityWeight : parityWeight ) );
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
			v = MAX(v, val);
			if (val > v || besti == -1) besti = index;
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
			v = MIN(v, val);
			if (val < v || besti == -1) besti = index;
			beta = MIN(v, beta);
		}
		if (depth > 3) (*um)[bws] = besti;
		return beta;
	}
}

pair<int, int> main_minimax(Board b, int depth, Side s, int guess = -1) {
	// Depth must be greater than or equal to 1
	uint64_t legalMoves = b.findLegalMoves(s);
	if (legalMoves == 0) return make_pair(-1, -100); // Evaluation here has no meaning
	//int totalCount = __builtin_popcount(b.taken >> 32) + __builtin_popcount(b.taken);
	
	int besti = -1;
	if (s == BLACK) {
		int v = INT_MIN;
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
    if (totalCount <= 33) depth = 10; 
    else if (totalCount <= 41) depth = 10; // 41 seems to be good
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
		p = main_minimax(currBoard, depth, side, besti);
		besti = p.first;
		eval = p.second;
		while (depth < 9) {
			depth = 9;
			cerr << "Going depth " << depth << endl;
			p = main_minimax(currBoard, depth + 1, side, besti);
			besti = p.first;
			eval = p.second;
		}
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
