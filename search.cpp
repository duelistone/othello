#include "search.h"

// Comparison function for pairs based on second value
bool pairCmpBlack(pair<Board, int> a, pair<Board, int> b) {
    return a.second > b.second;
}

bool pairCmpWhite(pair<Board, int> a, pair<Board, int> b) {
    return a.second < b.second;
}

pair<int, vector<uint8_t> > alphabeta(Board b, const int depth, const Side s, int alpha, int beta, const int depth2, const bool prevPass) {
    if (depth <= 0) return make_pair(b.pos_evaluate(), vector<uint8_t>());

    uint64_t legalMoves = b.findLegalMoves(s);
    
    // Special case
    if (!legalMoves) {
        if (prevPass) {
            int blacks = __builtin_popcountll(b.black);
            int whites = __builtin_popcountll(b.taken & ~b.black); 
            return make_pair((blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0), vector<uint8_t>());
        }
        return alphabeta(b, depth - 1, !s, alpha, beta, depth2 + 1, true);
    }
    
    int besti = 64;
    vector<uint8_t> bestpv;
    if (s) {
        int v = INT_MIN;
        uint8_t index = 64;
        if (depth2 <= HASH_DEPTH) {
            index = tt[b.zobrist_hash];
            if (index < 64 && (BIT(index) & legalMoves)) {
                // The move is valid
                legalMoves ^= BIT(index);
                besti = index;
                v = alphabeta(b.doMoveOnNewBoardBlack(index), depth - 1, WHITE, alpha, beta, depth2 + 1).first;
                alpha = (v > alpha) ? v : alpha;
            }
        }
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzll(legalMoves);
            legalMoves ^= BIT(index);
            pair< int, vector<uint8_t> > p = alphabeta(b.doMoveOnNewBoardBlackWZH(index), depth - 1, WHITE, alpha, beta, depth2 + 1);
            int val = p.first;
            bestpv = (val > v) ? p.second : bestpv;
            besti = (val > v) ? index : besti;
            v = (val > v) ? val : v;
            alpha = (v > alpha) ? v : alpha;
        }
        if (depth2 <= HASH_DEPTH) tt[b.zobrist_hash] = besti;
        bestpv.push_back(besti);
        return make_pair(alpha, bestpv);
    }
    else {
        int v = INT_MAX;
        uint8_t index = 64;
        if (depth2 <= HASH_DEPTH) {
            index = tt[b.zobrist_hash];
            if (index < 64 && (BIT(index) & legalMoves)) {
                // The move is valid
                legalMoves ^= BIT(index);
                besti = index;
                v = alphabeta(b.doMoveOnNewBoardWhite(index), depth - 1, BLACK, alpha, beta, depth2 + 1).first;
                beta = (v < beta) ? v : beta;
            }
        }
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzll(legalMoves);
            legalMoves ^= BIT(index);
            pair<int, vector<uint8_t> > p = alphabeta(b.doMoveOnNewBoardWhiteWZH(index), depth - 1, BLACK, alpha, beta, depth2 + 1);
            int val = p.first;
            besti = (val < v) ? index : besti;
            bestpv = (val < v) ? p.second : bestpv;
            v = (val < v) ? val : v;
            beta = (v < beta) ? v : beta;
        }
        if (depth2 <= HASH_DEPTH) tt[b.zobrist_hash] = besti;
        bestpv.push_back(besti);
        return make_pair(beta, bestpv);
    }
}

int pvs(Board b, const int depth, const Side s, int alpha, int beta, const int depth2, const bool prevPass) {
    if (s) return pvsBlack(b, depth, alpha, beta, depth2, prevPass);
    else return pvsWhite(b, depth, alpha, beta, depth2, prevPass);
}

pair<int, int> pvsMove(Board b, const int depth, const Side s, int alpha, int beta, const int depth2, const bool prevPass) {
    if (s) return pvsBlackMove(b, depth, alpha, beta, depth2, prevPass);
    else return pvsWhiteMove(b, depth, alpha, beta, depth2, prevPass);
}

int pvsBlack(Board b, const int depth, int alpha, const int beta, const int depth2, const bool prevPass) {
    int original_alpha = alpha;
    //int expected_result = INT_MAX;
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
    
    if (depth <= 1) {
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
                v = b.doMoveOnNewBoardBlackWZH(index).pos_evaluate();
                alpha = (v > alpha) ? v : alpha;
            }
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzll(legalMoves);
                legalMoves ^= BIT(index);
                int val = b.doMoveOnNewBoardBlackWZH(index).pos_evaluate();
                bool temp = (val > v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                alpha = (v > alpha) ? v : alpha;
            }
            tt[b.zobrist_hash] = besti;
        }
        else {
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzll(legalMoves);
                legalMoves ^= BIT(index);
                int val = b.doMoveOnNewBoardBlackWZH(index).pos_evaluate();
                alpha = (val > alpha) ? val : alpha;
            }
        }

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
            
            if (depth2 <= HASH_DEPTH2) {
                TTStruct hash_result = tt2[b.zobrist_hash];
                if (hash_result.black == b.black && hash_result.taken == b.taken && depth <= hash_result.d && hash_result.s == BLACK) {
                    if ((hash_result.e >= hash_result.alpha && hash_result.e <= hash_result.beta) || // In range, true eval
                     (hash_result.e >= hash_result.beta && hash_result.e >= beta)) {                 // If fail high with both, then good enough
                        return hash_result.e;
                        //expected_result = hash_result.e;
                    }
                    if (hash_result.e <= hash_result.alpha && hash_result.e <= alpha) {              // If both fail low, return alpha
                        //expected_result = alpha;
                        return alpha;
                    }
                }
            }
            legalMoves ^= BIT(index);
            besti = index;
            v = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
            alpha = (v > alpha) ? v : alpha;
            // Prove that it is indeed the best move
            while (alpha < beta && legalMoves) {    
                index = __builtin_clzll(legalMoves);
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
            index = pvsBlackMove(b, depth - 1, alpha, beta, depth2).first; 
            goto use_hash;
            #else
            // Fall back to normal search
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzll(legalMoves);
                legalMoves ^= BIT(index);
                int val = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
                bool temp = (val > v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                alpha = (v > alpha) ? v : alpha;
            }
            #endif
        }
        if (depth2 <= HASH_DEPTH2) tt2[b.zobrist_hash] = TTStruct(b, BLACK, alpha, depth, original_alpha, beta);
        tt[b.zobrist_hash] = besti;
    }
    else {
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzll(legalMoves);
            legalMoves ^= BIT(index);
            int val = pvsWhite(b.doMoveOnNewBoardBlackWZH(index), depth - 1, alpha, beta, depth2 + 1);
            alpha = (val > alpha) ? val : alpha;
        }
    }
    return alpha;
}

int pvsWhite(Board b, const int depth, const int alpha, int beta, const int depth2, const bool prevPass) {
    int original_beta = beta;
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
            int blacks = __builtin_popcountll(b.black);
            int whites = __builtin_popcountll(b.taken & ~b.black);
            return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
        }
        return pvsBlack(b, depth - 1, alpha, beta, depth2 + 1, true);
    }

    if (depth <= 1) {
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
                v = b.doMoveOnNewBoardWhiteWZH(index).pos_evaluate();
                beta = (v < beta) ? v : beta;
            }
            while (alpha < beta && legalMoves) {
                uint8_t index = __builtin_clzll(legalMoves);
                legalMoves ^= BIT(index);
                int val = b.doMoveOnNewBoardWhiteWZH(index).pos_evaluate();
                bool temp = (val < v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                beta = (v < beta) ? v : beta;
            }
            tt[b.zobrist_hash] = besti;
        }
        else {
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzll(legalMoves);
                legalMoves ^= BIT(index);
                int val = b.doMoveOnNewBoardWhiteWZH(index).pos_evaluate();
                beta = (val < beta) ? val : beta;
            }
        }

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
            if (depth2 <= HASH_DEPTH2) {
                TTStruct hash_result = tt2[b.zobrist_hash];
                if (hash_result.black == b.black && hash_result.taken == b.taken && depth <= hash_result.d && hash_result.s == WHITE) { 
                    if ((hash_result.e >= hash_result.alpha && hash_result.e <= hash_result.beta) ||
                        (hash_result.e <= hash_result.alpha && hash_result.e <= alpha)) {
                        return hash_result.e;
                    }
                    if (hash_result.e >= hash_result.beta && hash_result.e >= beta) {
                        return beta;
                    }
                }
            }
            legalMoves ^= BIT(index);
            besti = index;
            v = pvsBlack(b.doMoveOnNewBoardWhite(index), depth - 1, alpha, beta, depth2 + 1);
            beta = (v < beta) ? v : beta;
            // Prove that it is indeed the best move
            while (alpha < beta && legalMoves) {    
                index = __builtin_clzll(legalMoves);
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
            index = pvsWhiteMove(b, depth - 1, alpha, beta, depth2).first;
            goto use_hash;
            #else
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzll(legalMoves);
                legalMoves ^= BIT(index);
                int val = pvsBlack(b.doMoveOnNewBoardWhite(index), depth - 1, alpha, beta, depth2 + 1);
                bool temp = (val < v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                beta = (val < beta) ? val : beta;
            }
            #endif
        }
        if (depth2 <= HASH_DEPTH2) tt2[b.zobrist_hash] = TTStruct(b, WHITE, beta, depth, alpha, original_beta);
        tt[b.zobrist_hash] = besti;
    }
    else {
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzll(legalMoves);
            legalMoves ^= BIT(index);
            int val = pvsBlack(b.doMoveOnNewBoardWhiteWZH(index), depth - 1, alpha, beta, depth2 + 1);
            beta = (val < beta) ? val : beta;
        }
    }

    return beta;
}

pair<int, int> pvsBlackMove(Board b, const int depth, int alpha, const int beta, const int depth2, const bool prevPass) {
    // Assumes there is a legal move available, and that alpha < beta

    uint64_t legalMoves = b.findLegalMovesBlack();
        
    int besti = __builtin_clzll(legalMoves);
    int index = besti;
    if (depth <= 1) return make_pair(besti, -1000000);

    // Best move
    if (depth2 <= HASH_DEPTH) {
        int v = INT_MIN;
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
                index = __builtin_clzll(legalMoves);
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
            index = pvsBlackMove(b, depth - 1, alpha, beta, depth2).first; 
            goto use_hash;
            #else
            // Fall back to normal search
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzll(legalMoves);
                legalMoves ^= BIT(index);
                int val = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
                bool temp = (val > v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                alpha = (v > alpha) ? v : alpha;
            }
            #endif
        }
        tt[b.zobrist_hash] = besti;
    }
    else {
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzll(legalMoves);
            legalMoves ^= BIT(index);
            int val = pvsWhite(b.doMoveOnNewBoardBlackWZH(index), depth - 1, alpha, beta, depth2 + 1);
            besti = (val > alpha) ? index : besti;
            alpha = (val > alpha) ? val : alpha;
        }
    }

    return make_pair(besti, alpha);
}

pair<int, int> pvsWhiteMove(Board b, const int depth, const int alpha, int beta, const int depth2, const bool prevPass) {
    // Assumes there is a legal move available, and that alpha < beta

    uint64_t legalMoves = b.findLegalMovesWhite();
        
    int besti = __builtin_clzll(legalMoves);
    int index = besti;
    if (depth <= 1) return make_pair(besti, -1000000);

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
                index = __builtin_clzll(legalMoves);
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
            index = pvsWhiteMove(b, depth - 1, alpha, beta, depth2).first; // Changing HASH_DEPTH will cause an infinite loop here
            goto use_hash;
            #endif
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzll(legalMoves);
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
            uint8_t index = __builtin_clzll(legalMoves);
            legalMoves ^= BIT(index);
            int val = pvsBlack(b.doMoveOnNewBoardWhiteWZH(index), depth - 1, alpha, beta, depth2 + 1);
            besti = (val < beta) ? index : besti;
            beta = (val < beta) ? val : beta;
        }
    }

    return make_pair(besti, beta);
}


int deep_endgame_alphabeta_black(Board, int alpha = -1, int beta = 1, bool prevPass = false);
int deep_endgame_alphabeta_black_59(Board, int alpha = -1, int beta = 1, bool prevPass = false);
int deep_endgame_alphabeta_black_60(Board, int alpha = -1, int beta = 1, bool prevPass = false);
int deep_endgame_alphabeta_black_61(Board, int alpha = -1, int beta = 1, bool prevPass = false);
int deep_endgame_alphabeta_black_62(Board, int alpha = -1, int beta = 1, bool prevPass = false);
int deep_endgame_alphabeta_black_63(Board);
int deep_endgame_alphabeta_white(Board, int alpha = -1, int beta = 1, bool prevPass = false);
int deep_endgame_alphabeta_white_59(Board, int alpha = -1, int beta = 1, bool prevPass = false);
int deep_endgame_alphabeta_white_60(Board, int alpha = -1, int beta = 1, bool prevPass = false);
int deep_endgame_alphabeta_white_61(Board, int alpha = -1, int beta = 1, bool prevPass = false);
int deep_endgame_alphabeta_white_62(Board, int alpha = -1, int beta = 1, bool prevPass = false);
int deep_endgame_alphabeta_white_63(Board);

int deep_endgame_alphabeta_black_63(Board b) {
    uint64_t newblack = b.doMoveOnNewBoardBlackWZH(__builtin_clzll(~b.taken)).black;

    if (b.black == newblack) {
        newblack = b.doMoveOnNewBoardWhiteWZH(__builtin_clzll(~b.taken)).black;
        if (b.black == newblack) {
            return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        }
    }
    return __builtin_popcountll(newblack) - 32;
}

int deep_endgame_alphabeta_white_63(Board b) {
    uint64_t newblack = b.doMoveOnNewBoardWhiteWZH(__builtin_clzll(~b.taken)).black;

    if (b.black == newblack) {
        newblack = b.doMoveOnNewBoardBlackWZH(__builtin_clzll(~b.taken)).black;
        if (b.black == newblack) {
            return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        }
    }
    return __builtin_popcountll(newblack) - 32;
}

int deep_endgame_alphabeta_black_62(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesBlack();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - 31;
        return deep_endgame_alphabeta_white_62(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_white_63(b.doMoveOnNewBoardBlackWZH(index));
        alpha= (v > alpha) ? v : alpha;
    }

    return alpha;
}

int deep_endgame_alphabeta_white_62(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesWhite();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - 31; 
        return deep_endgame_alphabeta_black_62(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_black_63(b.doMoveOnNewBoardWhiteWZH(index));
        beta = (v < beta) ? v : beta;
    }

    return beta;
}

int deep_endgame_alphabeta_black_61(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesBlack();
    
    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        return deep_endgame_alphabeta_white_61(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_white_62(b.doMoveOnNewBoardBlackWZH(index), alpha, beta);
        alpha= (v > alpha) ? v : alpha;
    }

    return alpha;
}

int deep_endgame_alphabeta_white_61(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesWhite();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        return deep_endgame_alphabeta_black_61(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_black_62(b.doMoveOnNewBoardWhiteWZH(index), alpha, beta);
        beta = (v < beta) ? v : beta;
    }

    return beta;
}

int deep_endgame_alphabeta_black_60(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesBlack();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - 30;
        return deep_endgame_alphabeta_white_60(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_white_61(b.doMoveOnNewBoardBlackWZH(index), alpha, beta);
        alpha= (v > alpha) ? v : alpha;
    }

    return alpha;
}

int deep_endgame_alphabeta_white_60(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesWhite();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - 30;
        return deep_endgame_alphabeta_black_60(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_black_61(b.doMoveOnNewBoardWhiteWZH(index), alpha, beta);
        beta = (v < beta) ? v : beta;
    }

    return beta;
}

int deep_endgame_alphabeta_black_59(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesBlack();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        return deep_endgame_alphabeta_white_59(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_white_60(b.doMoveOnNewBoardBlackWZH(index), alpha, beta);
        alpha= (v > alpha) ? v : alpha;
    }

    return alpha;
}

int deep_endgame_alphabeta_white_59(Board b, int alpha, int beta, bool prevPass) {
    uint64_t legalMoves = b.findLegalMovesWhite();

    if (legalMoves == 0) {
        if (prevPass) return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        return deep_endgame_alphabeta_black_59(b, alpha, beta, true);
    }
    
    while (alpha < beta && legalMoves) {
        int index = __builtin_clzll(legalMoves);
        legalMoves ^= BIT(index);
        int v = deep_endgame_alphabeta_black_60(b.doMoveOnNewBoardWhiteWZH(index), alpha, beta);
        beta = (v < beta) ? v : beta;
    }

    return beta;
}

int deep_endgame_alphabeta_black(Board b, int alpha, int beta, bool prevPass) {
    int totalCount = __builtin_popcountll(b.taken);
    if (totalCount == 59) return deep_endgame_alphabeta_black_59(b, alpha, beta, prevPass);

    uint64_t legalMoves = b.findLegalMovesBlack();
    
    if (legalMoves == 0) {
        if (prevPass) {
            return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        }
        return deep_endgame_alphabeta_white(b, alpha, beta, true);
    }
    if (alpha < beta) {
        vector< pair<Board, int> > boards_and_evals;
        while (legalMoves) {
            int index = __builtin_clzll(legalMoves);
            legalMoves ^= BIT(index);
            Board b2 = b.doMoveOnNewBoardBlackWZH(index);
            boards_and_evals.push_back(make_pair(b2, __builtin_popcountll(b2.findLegalMovesBlack()) - __builtin_popcountll(b2.findLegalMovesWhite()))); // pvsWhite(b2, DEEP_ENDGAME_PRESEARCH_DEPTH)));
        }
        sort(boards_and_evals.begin(), boards_and_evals.end(), pairCmpBlack);
        for (size_t i = 0; alpha < beta && i < boards_and_evals.size(); i++) {
            int v = deep_endgame_alphabeta_white(boards_and_evals[i].first, alpha, beta);
            alpha = (v > alpha) ? v : alpha;
        }
    }

    return alpha;
}


int deep_endgame_alphabeta_white(Board b, int alpha, int beta, bool prevPass) {
    int totalCount = __builtin_popcountll(b.taken);
    if (totalCount == 59) return deep_endgame_alphabeta_white_59(b, alpha, beta, prevPass);

    uint64_t legalMoves = b.findLegalMovesWhite();
    
    if (legalMoves == 0) {
        if (prevPass) {
            return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        }
        return deep_endgame_alphabeta_black(b, alpha, beta, true);
    }
    if (alpha < beta) {
        vector< pair<Board, int> > boards_and_evals;
        while (legalMoves) {
            int index = __builtin_clzll(legalMoves);
            legalMoves ^= BIT(index);
            Board b2 = b.doMoveOnNewBoardWhiteWZH(index);
            boards_and_evals.push_back(make_pair(b2, __builtin_popcountll(b2.findLegalMovesBlack()) - __builtin_popcountll(b2.findLegalMovesWhite()))); // pvsBlack(b2, DEEP_ENDGAME_PRESEARCH_DEPTH, BLACK)));
        }
        sort(boards_and_evals.begin(), boards_and_evals.end(), pairCmpWhite);
        for (size_t i = 0; alpha < beta && i < boards_and_evals.size(); i++) {
            int v = deep_endgame_alphabeta_black(boards_and_evals[i].first, alpha, beta);
            beta = (v < beta) ? v : beta;
        }
    }

    return beta;
}

int deep_endgame_alphabeta(Board b, const Side s, int alpha = -1, int beta = 1) {
    if (s) return deep_endgame_alphabeta_black(b, alpha, beta);
    return deep_endgame_alphabeta_white(b, alpha, beta);
}

int endgame_alphabeta(Board b, const Side s, int alpha, int beta) {
    #if USE_HASH_IN_ENDGAME_ALPHABETA
    BoardWithSide bws(b.taken, b.black, b.zobrist_hash, s);
    if (endgameTT[b.zobrist_hash].e < 64 && endgameTT[b.zobrist_hash].b == bws) {
        return endgameTT[b.zobrist_hash].e;
    }
    #endif
    
    int totalCount = __builtin_popcountll(b.taken);
    
    if (totalCount >= DEEP_ENDGAME_START) return deep_endgame_alphabeta(b, s, alpha, beta);

    int presearch_depth = ENDGAME_PRESEARCH_DEPTH - (totalCount - 42) / 3;

    uint64_t legalMoves = b.findLegalMoves(s);
    
    if (legalMoves == 0) {
        uint64_t legalMovesOther = b.findLegalMoves(!s);
        if (legalMovesOther == 0) {
            // Simple evaluation here
            return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        }
        int ret = endgame_alphabeta(b, !s, alpha, beta);
        #if USE_HASH_IN_ENDGAME_ALPHABETA
        endgameTT[b.zobrist_hash].e = ret;
        endgameTT[b.zobrist_hash].b = bws;
        #endif
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
        if (alpha < beta) {
            vector< pair<Board, int> > boards_and_evals;
            while (legalMoves) {
                int index = __builtin_clzll(legalMoves);
                legalMoves ^= BIT(index);
                Board b2 = b.doMoveOnNewBoardBlack(index);
                boards_and_evals.push_back(make_pair(b2, pvs(b2, presearch_depth, WHITE)));
            }
            sort(boards_and_evals.begin(), boards_and_evals.end(), pairCmpBlack);
            for (size_t i = 0; alpha < beta && i < boards_and_evals.size(); i++) {
                int v = endgame_alphabeta(boards_and_evals[i].first, WHITE, alpha, beta);
                alpha = (v > alpha) ? v : alpha;
            }
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
        if (alpha < beta) {
            vector< pair<Board, int> > boards_and_evals;
            while (legalMoves) {
                int index = __builtin_clzll(legalMoves);
                legalMoves ^= BIT(index);
                Board b2 = b.doMoveOnNewBoardWhite(index);
                boards_and_evals.push_back(make_pair(b2, pvs(b2, presearch_depth, BLACK)));
            }
            sort(boards_and_evals.begin(), boards_and_evals.end(), pairCmpWhite);
            for (size_t i = 0; alpha < beta && i < boards_and_evals.size(); i++) {
                int v = endgame_alphabeta(boards_and_evals[i].first, BLACK, alpha, beta);
                beta = (v < beta) ? v : beta;
            }
        }
    }
    
    int ret = s ? alpha : beta;
    #if USE_HASH_IN_ENDGAME_ALPHABETA
    if (alpha >= 1 || beta <= -1) {
       endgameTT[b.zobrist_hash].e = ret;
       endgameTT[b.zobrist_hash].b = bws;
    }
    #endif
    return ret;
}

pair<int, int> endgame_minimax(Board b, Side s, int guess) {
    uint64_t legalMoves = b.findLegalMoves(s);
    if (legalMoves == 0) return make_pair(-1, -10000); // Second value is meaningless here
    
    int besti = -1;
    int v;
    if (s) {
        v = -1;
        if (guess > -1) {
            legalMoves ^= BIT(guess);
            Board b2 = b.doMoveOnNewBoard(guess, s);
            int val = endgame_alphabeta(b2, !s, v, 1);
            besti = guess;
            v = val;
        }
        while (legalMoves && v <= 0) {
            int index = __builtin_clzll(legalMoves);
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
            legalMoves ^= BIT(guess);
            Board b2 = b.doMoveOnNewBoard(guess, s);
            int val = endgame_alphabeta(b2, !s, -1, v);
            besti = guess;
            v = val;
        }
        while (legalMoves && v >= 0) {
            int index = __builtin_clzll(legalMoves);
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

// Extracts principal variation from hash
pair< vector<uint8_t>, Board> get_pv(Board b, Side side) {
    vector<uint8_t> result;
    
    Side s = side;
    for (int i = 0; i < 60; i++) {
        uint8_t hash_move = tt[b.zobrist_hash];
        if (hash_move >= 64) break;
        uint64_t lm = b.findLegalMoves(s);
        if (BIT(hash_move) & lm) {
            result.push_back(hash_move);
            b = b.doMoveOnNewBoard(hash_move, s);
        }
        else {
            result.push_back(64); // A pass
        }
        s = !s;
    }

    for (int i = result.size() - 1; i >= 0; i--) {
        if (result[i] == 64) result.pop_back();
    }

    return make_pair(result, b);
}
