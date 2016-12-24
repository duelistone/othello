#include "player.h"

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
        uint64_t corners = (1 << 7) | 1;
        STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
        PSEUDOSTABLE_DISCS[size - 1][(taken << 8) | black] = 0;
        ALL_STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
        EDGE_VALUES[(taken << 8) | black] = STABLE_WEIGHT * (2 * __builtin_popcountll(black) - 8) + CORNER_WEIGHT * (2 * __builtin_popcountll(black & corners) - 2);
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
    EDGE_VALUES[0] = 0;

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
    EDGE_VALUES[0] = 0;

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
    EDGE_VALUES[0] = 0;

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
    EDGE_VALUES[0] = 0;

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
    EDGE_VALUES[0] = 0;

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
    EDGE_VALUES[0] = 0;

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
    EDGE_VALUES[0] = 0;

}

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

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds if using the Java tester.
 */
Player::Player(Side s) : side(s), currBoard(Board()) {
    playerConstructorHelper();
    playerConstructorHelper2();
}

Player::~Player() {}

// Search functions

int alphabeta(Board b, const int depth, const Side s, int alpha, int beta, const int depth2, const bool prevPass) {
    if (depth <= 0) return b.pos_evaluate();

    uint64_t legalMoves = b.findLegalMoves(s);
    
    // Special case
    if (!legalMoves) {
        if (prevPass) {
            int blacks = __builtin_popcountll(b.black);
            int whites = __builtin_popcountll(b.taken & ~b.black); 
            return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
        }
        return alphabeta(b, depth - 1, !s, alpha, beta, depth2 + 1, true);
    }
    
    uint8_t besti = 64;
    if (s) {
        int v = INT_MIN;
        // Best move
        uint8_t index = 64;
        if (depth2 <= HASH_DEPTH) {
            index = tt[b.zobrist_hash];
            if (index < 64 && (BIT(index) & legalMoves)) {
                // The move is valid
                legalMoves ^= BIT(index);
                besti = index;
                v = alphabeta(b.doMoveOnNewBoardBlack(index), depth - 1, WHITE, alpha, beta, depth2 + 1);
                alpha = (v > alpha) ? v : alpha;
            }
        }
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzl(legalMoves);
            legalMoves ^= BIT(index);
            int val = alphabeta(b.doMoveOnNewBoardBlack(index), depth - 1, WHITE, alpha, beta, depth2 + 1);
            besti = (val > v) ? index : besti;
            v = (val > v) ? val : v;
            alpha = (v > alpha) ? v : alpha;
        }
        if (depth2 <= HASH_DEPTH) tt[b.zobrist_hash] = besti;
        return alpha;
    }
    else {
        int v = INT_MAX;
        // Best move
        uint8_t index = 64;
        if (depth2 <= HASH_DEPTH) {
            index = tt[b.zobrist_hash];
            if (index < 64 && (BIT(index) & legalMoves)) {
                // The move is valid
                legalMoves ^= BIT(index);
                besti = index;
                v = alphabeta(b.doMoveOnNewBoardWhite(index), depth - 1, BLACK, alpha, beta, depth2 + 1);
                beta = (v < beta) ? v : beta;
            }
        }
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzl(legalMoves);
            legalMoves ^= BIT(index);
            int val = alphabeta(b.doMoveOnNewBoardWhite(index), depth - 1, BLACK, alpha, beta, depth2 + 1);
            besti = (val < v) ? index : besti;
            v = (val < v) ? val : v;
            beta = (v < beta) ? v : beta;
        }
        if (depth2 <= HASH_DEPTH) tt[b.zobrist_hash] = besti;
        return beta;
    }
}

int pvsBlack(Board b, const int depth, int alpha = INT_MIN, const int beta = INT_MAX, const int depth2 = 0, const bool prevPass = false);
int pvsWhite(Board b, const int depth, const int alpha = INT_MIN, int beta = INT_MAX, const int depth2 = 0, const bool prevPass = false);
int pvsWhiteNull(Board b, const int depth, const int alpha = INT_MIN, const int depth2 = 0, const bool prevPass = false);

int pvs(Board b, const int depth, const Side s, int alpha = INT_MIN, int beta = INT_MAX, const int depth2 = 0, const bool prevPass = false) {
    if (s) return pvsBlack(b, depth, alpha, beta, depth2, prevPass);
    else return pvsWhite(b, depth, alpha, beta, depth2, prevPass);
}

int pvsBlack(Board b, const int depth, int alpha, const int beta, const int depth2, const bool prevPass) {
    //string padding;
    //for (int i = 0; i < depth2; i++) padding += " ";
    //cerr << padding + "Entering pvsBlack with depth " << depth << " and depth2 " << depth2 <<  endl;
    // Should rarely be called
    if (__builtin_expect(depth <= 0, 0)) {
        //cerr << padding + "Returning evaluation" << endl;
        return b.pos_evaluate();
    }
    
    uint64_t legalMoves = b.findLegalMovesBlack();
        
    // Special case
    if (__builtin_expect(!legalMoves, 0)) {
        if (prevPass) {
            int blacks = __builtin_popcountll(b.black);
            int whites = __builtin_popcountll(b.taken & ~b.black);
            return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
        }
        return pvsWhite(b, depth - 1, alpha, beta, depth2 + 1, true);
    }
    
    if (depth == 1) {
        // Fall back to alphabeta
        int v = INT_MIN;
        // Best move
        uint8_t index = 64;
        uint8_t besti = 64;
        if (depth2 <= HASH_DEPTH) {
            index = tt[b.zobrist_hash];
            if (BIT_SAFE(index) & legalMoves) {
                // The move is valid
                legalMoves ^= BIT(index);
                besti = index;
                v = b.doMoveOnNewBoardBlack(index).pos_evaluate();
                alpha = (v > alpha) ? v : alpha;
            }
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzl(legalMoves);
                legalMoves ^= BIT(index);
                int val = b.doMoveOnNewBoardBlack(index).pos_evaluate();
                bool temp = (val > v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                alpha = (v > alpha) ? v : alpha;
            }
            tt[b.zobrist_hash] = besti;
        }
        else {
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzl(legalMoves);
                legalMoves ^= BIT(index);
                int val = b.doMoveOnNewBoardBlack(index).pos_evaluate();
                alpha = (val > alpha) ? val : alpha;
            }
        }
        //cerr << padding + "Returning alpha when depth 1 " << alpha << endl;
        return alpha;
    }
    
    uint8_t besti = 64;
    int v = INT_MIN;
    // Best move
    uint8_t index = 64;
    if (depth2 <= HASH_DEPTH) {
        index = tt[b.zobrist_hash];
        if (BIT_SAFE(index) & legalMoves) {
            use_hash:
            // The move is valid
            legalMoves ^= BIT(index);
            besti = index;
            v = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
            alpha = (v > alpha) ? v : alpha;
            // Prove that it is indeed the best move
            while (alpha < beta && legalMoves) {    
                index = __builtin_clzl(legalMoves);
                legalMoves ^= BIT(index);
                Board b2 = b.doMoveOnNewBoardBlack(index);
                v = pvsWhite(b2, depth - 1, alpha, alpha + 1, depth2 + 1);
                // Either v = alpha + 1 or v <= alpha
                // Also, either beta = alpha + 1 or beta > alpha + 1
                if (__builtin_expect(v >= beta, 0)) {
                    besti = index;
                    alpha = v;
                    break;
                }
                else if (__builtin_expect(v >= alpha + 1, 0)) {
                    besti = index;
                    alpha = pvsWhite(b2, depth - 1, v, beta, depth2 + 1);
                }
            }
        }
        else {
            #if IID
            pvsBlack(b, depth - 1, alpha, beta, depth2); // Changing HASH_DEPTH will cause an infinite loop here
            index = tt[b.zobrist_hash];
            goto use_hash;
            #endif
            // Fall back to normal search
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzl(legalMoves);
                legalMoves ^= BIT(index);
                int val = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
                bool temp = (val > v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                alpha = (v > alpha) ? v : alpha;
            }
        }
        tt[b.zobrist_hash] = besti;
    }
    else {
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzl(legalMoves);
            legalMoves ^= BIT(index);
            int val = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
            alpha = (val > alpha) ? val : alpha;
        }
    }
    //cerr << padding + "Returning alpha " << alpha << endl;
    return alpha;
}

int pvsWhite(Board b, const int depth, const int alpha, int beta, const int depth2, const bool prevPass) {
    //string padding;
    //for (int i = 0; i < depth2; i++) padding += " ";
    //cerr << padding + "Entering pvsWhite with depth " << depth << " and depth2 " << depth2 << " and alpha " << alpha << " and beta " << beta << endl;
    // Should rarely be called
    if (__builtin_expect(depth <= 0, 0)) {
        return b.pos_evaluate();
    }
    
    uint64_t legalMoves = b.findLegalMovesWhite();
        
    // Special case
    if (__builtin_expect(!legalMoves, 0)) {
        if (prevPass) {
            int blacks = b.countBlack();
            int whites = b.countWhite();
            return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
        }
        return pvsBlack(b, depth - 1, alpha, beta, depth2 + 1, true);
    }

    // Note: What if there is a pass that jumps from depth 2 to depth 0?
    // Maybe this case is rare enough to not need to consider it.
    if (depth == 1) {
        int v = INT_MAX;
        // Best move
        uint8_t index = 64;
        uint8_t besti = 64;
        if (depth2 <= HASH_DEPTH) {
            index = tt[b.zobrist_hash];
            if (BIT_SAFE(index) & legalMoves) {
                // The move is valid
                legalMoves ^= BIT(index);
                besti = index;
                v = b.doMoveOnNewBoardWhite(index).pos_evaluate();
                beta = (v < beta) ? v : beta;
            }
            while (alpha < beta && legalMoves) {
                uint8_t index = __builtin_clzl(legalMoves);
                legalMoves ^= BIT(index);
                int val = b.doMoveOnNewBoardWhite(index).pos_evaluate();
                bool temp = (val < v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                beta = (v < beta) ? v : beta;
            }
            tt[b.zobrist_hash] = besti;
        }
        else {
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzl(legalMoves);
                legalMoves ^= BIT(index);
                int val = b.doMoveOnNewBoardWhite(index).pos_evaluate();
                beta = (val < beta) ? val : beta;
            }
        }
        //cerr << padding + "Returning depth1 beta " << beta << endl;
        return beta;
    }
    
    uint8_t besti = 64;
    uint8_t index = 64;
    // Best move
    if (depth2 <= HASH_DEPTH) {
        int v = INT_MAX;
        index = tt[b.zobrist_hash];
        if (BIT_SAFE(index) & legalMoves) {
            use_hash:
            // The move is valid
            legalMoves ^= BIT(index);
            besti = index;
            v = pvsBlack(b.doMoveOnNewBoardWhite(index), depth - 1, alpha, beta, depth2 + 1);
            beta = (v < beta) ? v : beta;
            // Prove that it is indeed the best move
            while (alpha < beta && legalMoves) {    
                index = __builtin_clzl(legalMoves);
                legalMoves ^= BIT(index);
                Board b2 = b.doMoveOnNewBoardWhite(index);
                v = pvsBlack(b2, depth - 1, beta - 1, beta, depth2 + 1);
                if (__builtin_expect(v <= alpha, 0)) {
                    besti = index;
                    beta = v;
                    break;
                }
                else if (__builtin_expect(v <= beta - 1, 0)) {
                    besti = index;
                    beta = pvsBlack(b2, depth - 1, alpha, v, depth2 + 1);
                }
            }
        }
        else {
            // Fall back to normal search
            #if IID
            pvsWhite(b, depth - 1, alpha, beta, depth2); // Changing HASH_DEPTH will cause an infinite loop here
            index = tt[b.zobrist_hash];
            goto use_hash;
            #endif
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzl(legalMoves);
                legalMoves ^= BIT(index);
                int val = pvsBlack(b.doMoveOnNewBoardWhite(index), depth - 1, alpha, beta, depth2 + 1);
                bool temp = (val < v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                beta = (val < beta) ? val : beta;
            }
        }
        tt[b.zobrist_hash] = besti;
    }
    else {
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzl(legalMoves);
            legalMoves ^= BIT(index);
            int val = pvsBlack(b.doMoveOnNewBoardWhite(index), depth - 1, alpha, beta, depth2 + 1);
            beta = (val < beta) ? val : beta;
        }
    }
    //cerr << padding + "Returning beta " << beta << endl;
    return beta;
}
int last_depth_4 = INT_MIN;
pair<int, int> main_minimax_aw(Board b, const Side s, int guess = -1) {
    Timer tim;

    pair<int, int> result;
    int eOdd, eEven;
    // First two plies
    eOdd = pvs(b, 1, s, INT_MIN, INT_MAX);
    eEven = pvs(b, 2, s, INT_MIN, INT_MAX);
    cerr << "Finished depth 1: " << eOdd << endl;
    cerr << "Finished depth 2: " << eEven << endl;
    
    // Other plies
    int d = 3;
    for (; ; d++) {
        if (d % 2) {
            int diff = abs(eOdd) / DIVISOR_FOR_AW + 5; 
            int lower = (eOdd == INT_MIN) ? INT_MIN : eOdd - diff;
            int upper = (eOdd == INT_MAX) ? INT_MAX : eOdd + diff;
            int counter = 1;
            try_again1:
            eOdd = pvs(b, d, s, lower, upper);
            result = make_pair(tt[b.zobrist_hash], eOdd);
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
            int diff = abs(eEven) / DIVISOR_FOR_AW + 5;
            int lower = (eEven == INT_MIN) ? INT_MIN : eEven - diff;
            int upper = (eEven == INT_MAX) ? INT_MAX : eEven + diff;
            int counter = 1;
            try_again2:
            eEven = pvs(b, d, s, lower, upper);
            if (d == 4) last_depth_4 = eEven;
            result = make_pair(tt[b.zobrist_hash], eEven);
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
        if (tim.getms() / 1000 > MAX_PENULTIMATE_DEPTH_TIME || d >= MAX_DEPTH) return result;
    }
    /*
    // Last one
    if (depth % 2) {
        int counter = 1;
        int diff = abs(eOdd) / DIVISOR_FOR_AW + 10;
        int lower = (eOdd == INT_MIN) ? INT_MIN : eOdd - diff;
        int upper = (eOdd == INT_MAX) ? INT_MAX : eOdd + diff;
        try_again3:
        eOdd = pvs(b, depth, s, lower, upper, false);
        // This is bound to run into issues one day
        pair<int, int> result = make_pair(tt[b.zobrist_hash], eOdd);
        if (result.second <= lower && lower != INT_MIN) {
            tim.endms("Recalculating (failed low) ");
            upper = eOdd + 1;
            lower = upper - (diff - 1 + totalCount / 4) * counter;
            counter++;
            if (counter > 4) lower = INT_MIN;
            goto try_again3;
        }
        else if (result.second >= upper && upper != INT_MAX) {
            tim.endms("Recalculating (failed high) ");
            lower = eOdd - 1;
            upper = lower - (diff - 1 + totalCount / 4) * counter;
            counter++;
            if (counter > 4) upper = INT_MAX;
            goto try_again3;
        }
        cerr << "Finished depth " << depth + 1 << " search: " << result.second << ' ';
        return result;
    }
    else {
        int counter = 1;
        int diff = (abs(eEven) / DIVISOR_FOR_AW + 10);
        int lower = (eEven == INT_MIN) ? INT_MIN : eEven - diff;
        int upper = (eEven == INT_MAX) ? INT_MAX : eEven + diff;
        try_again4:
        eEven = pvs(b, depth, s, lower, upper, false);
        // This is bound to run into issues one day
        pair<int, int> result = make_pair(tt[b.zobrist_hash], eEven);
        if (result.second <= lower && lower != INT_MIN) {
            tim.endms("Recalculating (failed low) ");
            diff *= 2;
            upper = eEven + 1;
            lower = upper - diff;
            counter++;
            if (counter > 4) lower = INT_MIN;
            goto try_again4;
        }
        else if (result.second >= upper && upper != INT_MAX) {
            tim.endms("Recalculating (failed high) ");
            diff *= 2;
            lower = eEven - 1;
            upper = lower + diff;
            counter++;
            if (counter > 4) upper = INT_MAX;
            goto try_again4;
        }
        cerr << "Finished depth " << depth << " search: " << result.second << ' ';
        tim.endms();
        return result;
    }*//*
    // Okay, maybe one more
    if ((depth  + 1) % 2) {
        int counter = 1;
        int diff = abs(eOdd) / DIVISOR_FOR_AW + 10;
        int lower = (eOdd == INT_MIN) ? INT_MIN : eOdd - diff;
        int upper = (eOdd == INT_MAX) ? INT_MAX : eOdd + diff;
        try_again5:
        eOdd = pvs(b, depth + 1, s, lower, upper, false, -1); // The -1 is to fool pvs into hashing
        // This is bound to run into issues one day
        pair<int, int> result = make_pair(tt[b.zobrist_hash], eOdd);
        if (result.second <= lower && lower != INT_MIN && eEven * sign < GOOD_ENOUGH) {
            tim.endms("Recalculating (failed low) ");
            upper = eOdd + 1;
            lower = upper - (diff - 1 + totalCount / 4) * counter;
            counter++;
            if (counter > 4) lower = INT_MIN;
            goto try_again5;
        }
        else if (result.second >= upper && upper != INT_MAX && eEven * sign < GOOD_ENOUGH) {
            tim.endms("Recalculating (failed high) ");
            lower = eOdd - 1;
            upper = lower + (diff - 1 + totalCount / 4) * counter;
            counter++;
            if (counter > 4) upper = INT_MAX;
            goto try_again5;
        }
        cerr << "Finished depth " << depth + 1 << " search: " << result.second << ' ';
        tim.endms();
        return result;
    }
    else {
        int counter = 1;
        int diff = abs(eEven) / DIVISOR_FOR_AW + 10;
        int lower = (eEven == INT_MIN) ? INT_MIN : eEven - diff;
        int upper = (eEven == INT_MAX) ? INT_MAX : eEven + diff;
        try_again6:
        eEven = pvs(b, depth + 1, s, lower, upper, false, -1); // The -1 is to fool pvs into hashing
        // This is bound to run into issues one day
        pair<int, int> result = make_pair(tt[b.zobrist_hash & (tt.mod - 1)], eEven);
        if (result.second <= lower && lower != INT_MIN && eEven * sign < GOOD_ENOUGH) {
            tim.endms("Recalculating (failed low) ");
            upper = eEven + 1;
            lower = upper - (diff - 1 + totalCount / 4) * counter;
            counter++;
            if (counter > 4) lower = INT_MIN;
            goto try_again6;
        }
        else if (result.second >= upper && upper != INT_MAX && eEven * sign < GOOD_ENOUGH) {
            tim.endms("Recalculating (failed high) ");
            lower = eEven - 1;
            upper = lower + (diff - 1 + totalCount / 4) * counter;
            counter++;
            if (counter > 4) upper = INT_MAX;
            goto try_again6;
        }
        cerr << "Finished depth " << depth + 1 << " search: " << result.second << ' ';
        tim.endms();
        return result;
    }*/
}

inline int deep_endgame_alphabeta_black(Board, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_black_59(Board, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_black_60(Board, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_black_61(Board, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_black_62(Board, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_black_63(Board, bool prevPass = false);
inline int deep_endgame_alphabeta_white(Board, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_white_59(Board, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_white_60(Board, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_white_61(Board, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_white_62(Board, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_white_63(Board, bool prevPass = false);

inline int deep_endgame_alphabeta_black_63(Board b, bool prevPass) {
    uint64_t newblack = b.doMoveOnNewBoardBlackWZH(__builtin_clzll(~b.taken)).black;

    if (b.black == newblack) {
        if (prevPass) {
            return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        }
        return deep_endgame_alphabeta_white_63(b, true);
    }
    return __builtin_popcountll(newblack) - 32;
}

inline int deep_endgame_alphabeta_white_63(Board b, bool prevPass) {
    uint64_t newblack = b.doMoveOnNewBoardWhiteWZH(__builtin_clzll(~b.taken)).black;

    if (b.black == newblack) {
        if (prevPass) {
            return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        }
        return deep_endgame_alphabeta_black_63(b, true);
    }
    return __builtin_popcountll(newblack) - 32;
}

inline int deep_endgame_alphabeta_black_62(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesBlack();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        return deep_endgame_alphabeta_white(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_white_63(b.doMoveOnNewBoardBlackWZH(index));
        alpha= (v > alpha) ? v : alpha;
    }

    return alpha;
}

inline int deep_endgame_alphabeta_white_62(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesWhite();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        return deep_endgame_alphabeta_black(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_black_63(b.doMoveOnNewBoardWhiteWZH(index));
        beta = (v < beta) ? v : beta;
    }

    return beta;
}

inline int deep_endgame_alphabeta_black_61(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesBlack();
    
    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        return deep_endgame_alphabeta_white(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_white_62(b.doMoveOnNewBoardBlackWZH(index), alpha, beta);
        alpha= (v > alpha) ? v : alpha;
    }

    return alpha;
}

inline int deep_endgame_alphabeta_white_61(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesWhite();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        return deep_endgame_alphabeta_black(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_black_62(b.doMoveOnNewBoardWhiteWZH(index), alpha, beta);
        beta = (v < beta) ? v : beta;
    }

    return beta;
}

inline int deep_endgame_alphabeta_black_60(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesBlack();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        return deep_endgame_alphabeta_white(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_white_61(b.doMoveOnNewBoardBlackWZH(index), alpha, beta);
        alpha= (v > alpha) ? v : alpha;
    }

    return alpha;
}

inline int deep_endgame_alphabeta_white_60(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesWhite();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        return deep_endgame_alphabeta_black(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_black_61(b.doMoveOnNewBoardWhiteWZH(index), alpha, beta);
        beta = (v < beta) ? v : beta;
    }

    return beta;
}

inline int deep_endgame_alphabeta_black_59(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesBlack();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        return deep_endgame_alphabeta_white(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_white_60(b.doMoveOnNewBoardBlackWZH(index), alpha, beta);
        alpha= (v > alpha) ? v : alpha;
    }

    return alpha;
}

inline int deep_endgame_alphabeta_white_59(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesWhite();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        return deep_endgame_alphabeta_black(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_black_60(b.doMoveOnNewBoardWhiteWZH(index), alpha, beta);
        beta = (v < beta) ? v : beta;
    }

    return beta;
}
inline int deep_endgame_alphabeta_black(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesBlack();
    
    if (legalMoves == 0) {
        if (prevPass) {
            return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        }
        return deep_endgame_alphabeta_white(b, alpha, beta, true);
    }

    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v;
        if (__builtin_popcountll(b.taken) == 58) v = deep_endgame_alphabeta_white_59(b.doMoveOnNewBoardBlackWZH(index), alpha, beta, false);
        else v = deep_endgame_alphabeta_white(b.doMoveOnNewBoardBlackWZH(index), alpha, beta, false);
        alpha = (v > alpha) ? v : alpha;
    }

    return alpha;
}


inline int deep_endgame_alphabeta_white(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesWhite();
    
    if (legalMoves == 0) {
        if (prevPass) {
            return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        }
        return deep_endgame_alphabeta_black(b, alpha, beta, true);
    }

    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v;
        if (__builtin_popcountll(b.taken) == 58) v = deep_endgame_alphabeta_black_59(b.doMoveOnNewBoardWhiteWZH(index), alpha, beta, false);
        else v = deep_endgame_alphabeta_black(b.doMoveOnNewBoardWhiteWZH(index), alpha, beta, false);
        beta = (v < beta) ? v : beta;
    }

    return beta;
}

int deep_endgame_alphabeta(Board b, const Side s, int alpha = -1, int beta = 1) {
    if (s) return deep_endgame_alphabeta_black(b, (alpha == 0) ? 0 : -1, (beta == 0) ? 0 : 1);
    return deep_endgame_alphabeta_white(b, (alpha == 0) ? 0 : -1, (beta == 0) ? 0 : 1);
}

int endgame_alphabeta(Board b, const Side s, int alpha = -1, int beta = 1) {
    BoardWithSide bws(b.taken, b.black, b.zobrist_hash, s);
    if (endgameTT[b.zobrist_hash].e < 64 && endgameTT[b.zobrist_hash].b == bws) {
        return endgameTT[b.zobrist_hash].e;
    }
    
    int totalCount = __builtin_popcountll(b.taken);
    
    if (s) {
        switch (totalCount) {
            case 63:
                return deep_endgame_alphabeta_black_63(b);
            case 62:
                return deep_endgame_alphabeta_black_62(b, alpha, beta);
            case 61:
                return deep_endgame_alphabeta_black_61(b, alpha, beta);
            case 60:
                return deep_endgame_alphabeta_black_60(b, alpha, beta);
            case 59:
                return deep_endgame_alphabeta_black_59(b, alpha, beta);
        }
    }
    else {
        switch (totalCount) {
            case 63:
                return deep_endgame_alphabeta_white_63(b);
            case 62:
                return deep_endgame_alphabeta_white_62(b, alpha, beta);
            case 61:
                return deep_endgame_alphabeta_white_61(b, alpha, beta);
            case 60:
                return deep_endgame_alphabeta_white_60(b, alpha, beta);
            case 59:
                return deep_endgame_alphabeta_white_59(b, alpha, beta);
        }
    }
    
    if (totalCount >= 42 + HASH_DEPTH) return deep_endgame_alphabeta(b, s, alpha, beta);
    
    uint64_t legalMoves = b.findLegalMoves(s);
    
    if (legalMoves == 0) {
        uint64_t legalMovesOther = b.findLegalMoves(!s);
        if (legalMovesOther == 0) {
            // Simple evaluation here
            return b.countBlack() - b.countWhite();
        }
        int ret = endgame_alphabeta(b, !s, alpha, beta);
        endgameTT[b.zobrist_hash].e = ret;
        endgameTT[b.zobrist_hash].b = bws;
        return ret;
    }
    
    if (s) {
        #if USE_HASH_IN_ENDGAME_ALPHABETA
        int index = tt[b.zobrist_hash];
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
        int index = tt[b.zobrist_hash];
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
    if (alpha != 0 || beta != 0) {
       endgameTT[b.zobrist_hash].e = ret;
       endgameTT[b.zobrist_hash].b = bws;
    }
    return ret;
}

pair<int, int> endgame_minimax(Board b, Side s, int guess = -1) {
    uint64_t legalMoves = b.findLegalMoves(s);
    if (legalMoves == 0) return make_pair(-1, -10000); // Second value is meaningless here
    
    int besti = -1;
    int v;
    if (s == BLACK) {
        v = -1;
        if (guess > -1) {
            legalMoves ^= SINGLE_BIT[guess];
            Board b2 = b.doMoveOnNewBoard(guess, s);
            int val = endgame_alphabeta(b2, !s, v, 1);
            besti = guess;
            v = val;
        }
        while (legalMoves && v <= 0) {
            int index = __builtin_clzl(legalMoves);
            legalMoves ^= BIT(index);
            
            Board b2 = b.doMoveOnNewBoard(index, s);
            int val = endgame_alphabeta(b2, !s, v, 1);
            if (val > v) {
                besti = index;
                v = val;
            }
        }
    }
    else {
        v = 1;
        if (guess > -1) {
            legalMoves ^= SINGLE_BIT[guess];
            Board b2 = b.doMoveOnNewBoard(guess, s);
            int val = endgame_alphabeta(b2, !s, -1, v);
            besti = guess;
            v = val;
        }
        while (legalMoves && v >= 0) {
            int index = __builtin_clzl(legalMoves);
            legalMoves ^= BIT(index);
            
            Board b2 = b.doMoveOnNewBoard(index, s);
            int val = endgame_alphabeta(b2, !s, -1, v);
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
    // Set minutesForMove to half the amount of time left
    if (msLeft > 0) minutesForMove = msLeft / 60.0 / 1000 / 2;
    
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

    // Set depth according to how far into game
    int depth;
    if (totalCount <= 20) depth = MAX_DEPTH;
    else if (totalCount <= 30) depth = MAX_DEPTH;
    else if (totalCount <= 41) depth = MAX_DEPTH;
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
    
    // Count collisions
    //~ int positions = 0;
    //~ for (size_t i = 0; i < tt.mod; i++) {
        //~ if (tt.table[i] != 64) positions++;
    //~ }
    
    // Output some useful info and return (remove for more speed)
    cerr << totalCount + 1 << " eval: " << eval << ' ' << endl;
    if (gameSolved) cerr << "(Endgame eval)" << endl;
    
    // Make move
    string letters = "abcdefgh";
    int x = FROM_INDEX_X(besti);
    int y = FROM_INDEX_Y(besti);
    cerr << ((side == BLACK) ? "Black " : "White ") << "Move: " << letters[x] << ' ' << y + 1 << endl;
    currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(x, y), side);
    currBoard.print_eval_stats();
    Move *move = new Move(x, y);
    return move;
    
}
