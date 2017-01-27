#include "player.h"

#if STABILITY
void playerConstructorHelper() {
    int size = 8;
    // get next greater value with same number of one bits
    // Taken from "Hacker's Delight" by Henry S. Warren, Jr.
    // originally appeared as  HAKMEM ITEM 175 (Bill Gosper)
    auto snoob = [] (uint64_t x) -> uint64_t {
       uint64_t smallest, ripple, ones;
       smallest = x & -x;
       ripple = x + smallest;
       ones = x ^ ripple;
       ones = (ones >> 2) / smallest;
       return ripple | ones;
    };

    // 8 filled
    uint64_t taken = (1 << 8) - 1;
    for (uint64_t black = 0; black < 1 << 8; black++) {
        // uint64_t corners = (1 << 7) | 1;
        STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
        PSEUDOSTABLE_DISCS[size - 1][(taken << 8) | black] = 0;
        ALL_STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
    }
    HELPER8(8, 7);
    HELPER8(8, 6);
    HELPER8(8, 5);
    HELPER8(8, 4);
    HELPER8(8, 3);
    HELPER8(8, 2);
    HELPER8(8, 1);
    // 0 filled
    STABLE_DISCS[size - 1][0] = 0;
    PSEUDOSTABLE_DISCS[size - 1][0] = 0;

    size = 7;

    // 7 filled
    taken = (1 << 7) - 1;
    for (uint64_t black = 0; black < 1 << 7; black++) {
        STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
        PSEUDOSTABLE_DISCS[size - 1][(taken << 8) | black] = 0;
        ALL_STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
    }
    HELPER(7, 6);
    HELPER(7, 5);
    HELPER(7, 4);
    HELPER(7, 3);
    HELPER(7, 2);
    HELPER(7, 1);
    // 0 filled
    STABLE_DISCS[size - 1][0] = 0;
    PSEUDOSTABLE_DISCS[size - 1][0] = 0;

    size = 6;

    // 6 filled
    taken = (1 << 6) - 1;
    for (uint64_t black = 0; black < 1 << 6; black++) {
        STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
        PSEUDOSTABLE_DISCS[size - 1][(taken << 8) | black] = 0;
        ALL_STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
    }
    // Less filled
    HELPER(6, 5);
    HELPER(6, 4);
    HELPER(6, 3);
    HELPER(6, 2);
    HELPER(6, 1);
    // 0
    STABLE_DISCS[size - 1][0] = 0;
    PSEUDOSTABLE_DISCS[size - 1][0] = 0;
    ALL_STABLE_DISCS[size - 1][0] = 0;

    size = 5;

    // 5 filled
    taken = (1 << 5) - 1;
    for (uint64_t black = 0; black < 1 << 5; black++) {
        STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
        PSEUDOSTABLE_DISCS[size - 1][(taken << 8) | black] = 0;
        ALL_STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
    }
    // Less filled
    HELPER(5, 4);
    HELPER(5, 3);
    HELPER(5, 2);
    HELPER(5, 1);
    // 0
    STABLE_DISCS[size - 1][0] = 0;
    PSEUDOSTABLE_DISCS[size - 1][0] = 0;
    ALL_STABLE_DISCS[size - 1][0] = 0;

    size = 4;

    // 4 filled
    taken = (1 << 4) - 1;
    for (uint64_t black = 0; black < 1 << 4; black++) {
        STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
        PSEUDOSTABLE_DISCS[size - 1][(taken << 8) | black] = 0;
        ALL_STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
    }
    // Less filled
    HELPER(4, 3);
    HELPER(4, 2);
    HELPER(4, 1);
    // 0
    STABLE_DISCS[size - 1][0] = 0;
    PSEUDOSTABLE_DISCS[size - 1][0] = 0;
    ALL_STABLE_DISCS[size - 1][0] = 0;

    size = 3;

    // 3 filled
    taken = (1 << 3) - 1;
    for (uint64_t black = 0; black < 1 << 3; black++) {
        STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
        PSEUDOSTABLE_DISCS[size - 1][(taken << 8) | black] = 0;
        ALL_STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
    }
    // Less filled
    HELPER(3, 2);
    HELPER(3, 1);
    // 0
    STABLE_DISCS[size - 1][0] = 0;
    PSEUDOSTABLE_DISCS[size - 1][0] = 0;
    ALL_STABLE_DISCS[size - 1][0] = 0;

    size = 2;

    // 2 filled
    taken = (1 << 2) - 1;
    for (uint64_t black = 0; black < 1 << 2; black++) {
        STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
        PSEUDOSTABLE_DISCS[size - 1][(taken << 8) | black] = 0;
        ALL_STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
    }
    // Less filled
    HELPER(2, 1);
    // 0
    STABLE_DISCS[size - 1][0] = 0;
    PSEUDOSTABLE_DISCS[size - 1][0] = 0;
    ALL_STABLE_DISCS[size - 1][0] = 0;
}
#endif

/* Sets arrays that map byte to column or other important bit pattern */
void playerConstructorHelper2() {
    cerr << "Starting second helper." << endl;
    for (int i = 0; i < 8; i++) {
        BYTE_TO_COL[i][0] = 0;
    }

    for (int i = 0; i < 8; i++) {
        uint64_t bit_pattern = COL_MASK(i);
        uint64_t curr = 0;
        for (uint8_t j = 1; j != 0; j++) {
            curr = (curr - bit_pattern) & bit_pattern;
            BYTE_TO_COL[i][j] = curr;
        }
    }

    // BYTE_TO_PATTERN
    for (int i = 0; i < NUM_PATTERNS; i++) {
        BYTE_TO_PATTERN[i][0] = 0;
    }
    for (int i = 0; i < NUM_PATTERNS; i++) {
        uint64_t bit_pattern = pattern_mask[i];
        uint64_t curr = 0;
        if (pattern_size[i] == 8) {
            for (uint8_t j = 1; j != 0; j++) {
                curr = (curr - bit_pattern) & bit_pattern;
                BYTE_TO_PATTERN[i][j] = curr;
            }
        }
        else {
            for (uint8_t j = 1; j < (1 << pattern_size[i]); j++) {
                curr = (curr - bit_pattern) & bit_pattern;
                BYTE_TO_PATTERN[i][j] = curr;
            }
        }
    }
    
    cerr << "Ending second helper." << endl;
}

// Load EDGE_VALUES from file
void playerConstructorHelper3() {
    fstream edge_file("edge_values", ios_base::in);
    int n, a;
    double b;
    while (edge_file >> n >> a >> b) {
        EDGE_VALUES[(1 << 16) * n + a] = b;
    }
    edge_file.close();
}

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds if using the Java tester.
 */
Player::Player(Side s) : side(s), last_depth(4), currBoard(Board()) {
    #if STABILITY
    playerConstructorHelper();
    #endif
    playerConstructorHelper2();
    playerConstructorHelper3();
}

Player::~Player() {}

// Search functions
pair<int, int> Player::main_minimax_aw(Board b, const Side s, int guess) {
    Timer tim;

    pair<int, int> result;
    int eOdd, eEven;
    // First two plies
    eOdd = pvs(b, last_depth - 3, s, INT_MIN, INT_MAX);
    eEven = pvs(b, last_depth - 2, s, INT_MIN, INT_MAX);
    cerr << "Finished depth -3: " << eOdd << endl;
    cerr << "Finished depth -2: " << eEven << endl;
    
    // Other plies
    int d = last_depth - 1;
    for (; ; d++) {
        if (d % 2) {
            int diff = abs(eOdd) / DIVISOR_FOR_AW + 400; 
            int lower = (eOdd == INT_MIN) ? INT_MIN : eOdd - diff;
            int upper = (eOdd == INT_MAX) ? INT_MAX : eOdd + diff;
            int counter = 1;
            try_again1:
            result = pvsMove(b, d, s, lower, upper);
            eOdd = result.second;
            if (eOdd <= lower && lower != INT_MIN) {
                tim.endms("Recalculating (failed low) ");
                diff *= 2;
                upper = eOdd + 1;
                lower = upper - diff;
                counter++;
                if (counter > 4) lower = INT_MIN;
                goto try_again1;
            }
            else if (eOdd >= upper && upper != INT_MAX) {
                tim.endms("Recalculating (failed high) ");
                diff *= 2;
                lower = eOdd - 1;
                upper = lower + diff; 
                counter++;
                if (counter > 4) upper = INT_MAX;
                goto try_again1;
            }
            cerr << "Finished depth " << d << " search: " << eOdd << ' ';
            tim.endms();
        }
        else {
            int diff = abs(eEven) / DIVISOR_FOR_AW + 400;
            int lower = (eEven == INT_MIN) ? INT_MIN : eEven - diff;
            int upper = (eEven == INT_MAX) ? INT_MAX : eEven + diff;
            int counter = 1;
            try_again2:
            result = pvsMove(b, d, s, lower, upper);
            eEven = result.second;
            if (eEven <= lower && lower != INT_MIN) {
                tim.endms("Recalculating (failed low) ");
                diff *= 2;
                upper = eEven + 1;
                lower = upper - diff;
                counter++;
                if (counter > 4) lower = INT_MIN;
                goto try_again2;
            }
            else if (eEven >= upper && upper != INT_MAX) {
                tim.endms("Recalculating (failed high) ");
                diff *= 2;
                lower = eEven - 1;
                upper = lower + diff;
                counter++;
                if (counter > 4) upper = INT_MAX;
                goto try_again2;
            }
            cerr << "Finished depth " << d << " search: " << eEven << ' ';
            tim.endms();
        }
        if (tim.getms() / 1000 > MAX_PENULTIMATE_DEPTH_TIME - 1 || d >= MAX_DEPTH) {
            last_depth = d;
            return result;
        }
    }
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
Move * Player::doMove(Move *opponentsMove, int msLeft) {
    if (opponentsMove != NULL) {
        currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(opponentsMove->x, opponentsMove->y), !side);
    }
    
    uint64_t legalMoves = currBoard.findLegalMoves(side);
    if (!legalMoves) return NULL;
    
    int totalCount = __builtin_popcountll(currBoard.taken);
    if (totalCount == 4) {
        // Move e6 without thinking
        currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(4, 5), side);
        Move *move = new Move(4, 5);
        return move;
    }
    else if (totalCount == 5) {
        // Choose diagonal opening
        int index = __builtin_clzll(legalMoves & (BIT(18) | BIT(21) | BIT(42) | BIT(45)));
        int x = FROM_INDEX_X(index);
        int y = FROM_INDEX_Y(index);
        currBoard = currBoard.doMoveOnNewBoard(index, side);
        Move *move = new Move(x, y);
        return move;
    }
    else if (totalCount == 7 && __builtin_popcountll(legalMoves) == 2) {
        // Symmetric position in parallel opening, either move is equivalent
        int index = __builtin_clzll(legalMoves);
        int x = FROM_INDEX_X(index);
        int y = FROM_INDEX_Y(index);
        currBoard = currBoard.doMoveOnNewBoard(index, side);
        Move *move = new Move(x, y);
        return move;
    }
    else if (totalCount == 6 && (currBoard.taken & ~currBoard.black & (BIT(18) | BIT(21) | BIT(42) | BIT(45)))) {
        // Diagonal opening
        uint64_t mask = BIT(19) | BIT(26) | BIT(37) | BIT(44);
        uint64_t bit = currBoard.black & mask;
        if ((bit << 7) & mask) bit <<= 7;
        else bit >>= 7;
        int index = __builtin_clzll(bit);
        currBoard = currBoard.doMoveOnNewBoard(index, side);
        Move *move = new Move(FROM_INDEX_X(index), FROM_INDEX_Y(index));
        return move;
    }

    // Set depth according to how far into game
    int depth;
    if (totalCount < ENDGAME_START) depth = MAX_DEPTH;
    else depth = INT_MAX; // Search to end (much faster)
    
    int eval = -10000; // Just a value

    // Find index of best move via search
    int besti = -1;
    pair<int, int> p;
    if (depth != INT_MAX) {
        p = main_minimax_aw(currBoard, side, besti);
        besti = p.first;
        eval = p.second;
    }
    else {
        cerr << "Endgame search" << endl;
        if (!gameSolved) {
            pair<int, int> p2 = main_minimax_aw(currBoard, side, MAX_DEPTH);
            auto start = chrono::high_resolution_clock::now();
            p = endgame_minimax(currBoard, side, p2.first);
            cerr << "Endgame search time: " << chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
            besti = p.first;
            eval = p.second;
            // If could not calculate a win or draw, fall back to other algorithm
            if ((side && p.second < 0) || (!side && p.second > 0)) {
                besti = p2.first;
                eval = p2.second;
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
    
    // Make move and output helpful info
    string letters = "abcdefgh";
    int x = FROM_INDEX_X(besti);
    int y = FROM_INDEX_Y(besti);
    cerr << ((side == BLACK) ? "Black " : "White ") << "Move: " << letters[x] << ' ' << y + 1 << " (" << besti << ") " << endl;
    currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(x, y), side);
    currBoard.print_eval_stats();
    pair< vector<uint8_t>, Board > pv_and_board = get_pv(currBoard, !side);
    cerr << "PV (might contain errors): ";
    for (size_t i = 0; i < pv_and_board.first.size(); i++) cerr << (int) pv_and_board.first[i] << ' ';
    cerr << endl;
    cerr << pv_and_board.second << endl;
    cerr << totalCount + 1 << " eval: " << eval << ' ' << endl;
    if (gameSolved) cerr << "(Endgame eval)" << endl;
    Move *move = new Move(x, y);

    return move;
}
