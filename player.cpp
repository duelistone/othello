#include "player.h"

#define SIMPLE_EVAL 1
#define KILLER_HEURISTIC 0 
#define IID 1
#define USE_HASH_IN_ENDGAME_ALPHABETA 1

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
#define CORNER_WEIGHT 6
#define STABLE_WEIGHT 3

Side abortSide;

void playerConstructorHelper() {
    // To speed things up
    #define HELPER8(s, n) {for (taken = (1 << (n)) - 1; taken < 1 << (s); taken = snoob(taken)) {uint64_t black = 0; do {RowBoard<(s)> rb(taken, black); rb.set_stable(); EDGE_VALUES[(taken << 8) | black] = STABLE_WEIGHT * (__builtin_popcount(rb.all_stable() & black) - __builtin_popcount(rb.all_stable() & ~black)) + CORNER_WEIGHT * (__builtin_popcount(black & CORNERS) - __builtin_popcount(taken & ~black & CORNERS)); black = (black - taken) & taken;} while (black);}}
    #define HELPER(s, n) {for (taken = (1 << (n)) - 1; taken < 1 << (s); taken = snoob(taken)) {uint64_t black = 0; do {RowBoard<(s)> rb(taken, black); rb.set_stable(); black = (black - taken) & taken;} while (black);}}
    
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
        STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
        PSEUDOSTABLE_DISCS[size - 1][(taken << 8) | black] = 0;
        ALL_STABLE_DISCS[size - 1][(taken << 8) | black] = taken;
        EDGE_VALUES[(taken << 8) | black] = STABLE_WEIGHT * (2 * __builtin_popcountll(black) - 8) + CORNER_WEIGHT * (2 * __builtin_popcountll(black & CORNERS) - 2);
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

    /*
    uint8_t black_stable_discs = 0;
    uint8_t white_stable_discs = 0;
    uint8_t black_pseudostable_discs = 0;
    uint8_t white_pseudostable_discs = 0;
    int cornerWeight = 9;
    int stableWeight = 3;
    //~ int wedgeWeight = 3;
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
            if (s == BLACK) {
                eval += cornerWeight;
                black_stable_discs |= JUST_EDGE_BIT(i);
            }
            else {
                eval -= cornerWeight;
                white_stable_discs |= JUST_EDGE_BIT(i);
            }
            continue;
        }
        
        // Is the disc stable?
        if (count == 8) {
            // Yes
            if (i != 0 && i != 7) {
                if (s == BLACK) {
                    eval += stableWeight;
                    black_stable_discs |= JUST_EDGE_BIT(i);
                }
                else {
                    eval -= stableWeight;
                    white_stable_discs |= JUST_EDGE_BIT(i);
                }
            }
            continue;
        }
        
        int x = i;
        x--;
        while (x >= 0 && f(x) == f(i)) x--;
        if (x == -1) {
            if (s == BLACK) {
                eval += stableWeight;
                black_stable_discs |= JUST_EDGE_BIT(i);
            }
            else {
                eval -= stableWeight;
                white_stable_discs |= JUST_EDGE_BIT(i);
            }
            continue;
        }
        
        x = i;
        x++;
        while (x < 8 && f(x) == f(i)) x++;
        if (x == 8) {
            if (s == BLACK) {
                eval += stableWeight;
                black_stable_discs |= JUST_EDGE_BIT(i);
            }
            else {
                eval -= stableWeight;
                white_stable_discs |= JUST_EDGE_BIT(i);
            }
            continue;
        }

        // Pseudo stable discs
        x = i - 1;
        int changes = 0;
        while (x >= 0 && f(x) != -1) {
            if (f(x) != f(x + 1)) changes++;
            x--;
        }
        // Now x + 1 is the number of empty squares
        // If the number of changes is >= number of empty squares, we're good
        if (changes > x) {
            int x = i + 1;
            int changes = 0;
            while (x < 8 && f(x) != -1) {
                if (f(x) != f(x - 1)) changes++;
                x++;
            }
            // Now (7 - x) + 1 is the number of empty squares
            // If the number of changes is >= number of empty squares, we're good
            if (changes > 7 - x) {
                if (s == BLACK) {
                    eval += stableWeight;
                    black_pseudostable_discs |= JUST_EDGE_BIT(i);
                }
                else {
                    eval -= stableWeight;
                    white_pseudostable_discs |= JUST_EDGE_BIT(i);
                }
                continue;
            }
        }

        
    }
    */
    /*
    // Wedges (these are complicated, maybe we shouldn't deal with
    // them so generally.
    for (int i = 0; i < 8; i++) {
        for (int j = i + 4; j < 8; j++) {
            if (f(i) == -1 && f(j) == -1) {
                bool isWedge = true;
                if (f(i + 1) == BLACK && f(j - 1) == BLACK) {
                    for (int k = i + 2; k < j - 1; k++) {
                        if (f(k) != WHITE) isWedge = false;
                    }
                    if (isWedge) {
                        eval -= wedgeWeight;
                        if (i == 0 || i == 8) eval -= cornerWeight;
                    }
                }
                else if (f(i + 1) == WHITE && f(j - 1) == WHITE) {
                    for (int k = i + 2; k < j - 1; k++) {
                        if (f(k) != BLACK) isWedge = false;
                    }
                    if (isWedge) {
                        eval += wedgeWeight;
                        if (i == 0 || i == 8) eval -= cornerWeight;
                    }
                }
            }
        }
    }
    */
    /*
    // Forced corner capture (only count if to corner)
    // Won't assume it's the turn players turn for now, so this 
    // requires a wedge to be guaranteed
    // Shouldn't overlap with the wedge case covered earlier
    bool cornerCapture = true;
    int i = 0;
    if (f(i) != -1) cornerCapture = false;
    // WHITE advantage
    if (f(i + 1) != BLACK) cornerCapture = false;
    int emptySquares = 0;
    int k;
    for (k = i + 2; f(k) == -1 && k <= 7; k++) {
        emptySquares++;
    }
    if (f(k + 1) != BLACK) cornerCapture = false;
    if (emptySquares % 2) cornerCapture = false;
    if (cornerCapture) {
        eval -= 2 * cornerWeight;
        eval -= stableWeight * (emptySquares + 2);
    }
    // BLACK advantage
    if (f(i + 1) != WHITE) cornerCapture = false;
    emptySquares = 0;
    for (k = i + 2; f(k) == -1 && k <= 7; k++) {
        emptySquares++;
    }
    if (f(k + 1) != WHITE) cornerCapture = false;
    if (emptySquares % 2) cornerCapture = false;
    if (cornerCapture) {
        eval += 2 * cornerWeight;
        eval += stableWeight * (emptySquares + 2);
    }
    
    cornerCapture = true;
    i = 7;
    if (f(i) != -1) cornerCapture = false;
    // WHITE advantage
    if (f(i - 1) != BLACK) cornerCapture = false;
    emptySquares = 0;
    for (k = i - 2; f(k) == -1 && k >= 0; k--) {
        emptySquares++;
    }
    if (f(k - 1) != BLACK) cornerCapture = false;
    if (emptySquares % 2) cornerCapture = false;
    if (cornerCapture) {
        eval -= 2 * cornerWeight;
        eval -= stableWeight * (emptySquares + 2);
    }
    // BLACK advantage
    if (f(i - 1) != WHITE) cornerCapture = false;
    emptySquares = 0;
    for (int k = i - 2; f(k) == -1 && k >= 0; k--) {
        emptySquares++;
    }
    if (f(k - 1) != WHITE) cornerCapture = false;
    if (emptySquares % 2) cornerCapture = false;
    if (cornerCapture) {
        eval += 2 * cornerWeight;
        eval += stableWeight * (emptySquares + 2);
    }
    */
    /*
    // Even distance, different color discs, forced corner capture
    
    cornerCapture = true;
    i = 0;
    if (f(i) != -1) cornerCapture = false;
    // WHITE advantage
    if (f(i + 1) != BLACK) cornerCapture = false;
    emptySquares = 0;
    for (k = i + 2; f(k) == -1 && k <= 7; k++) {
        emptySquares++;
    }
    if (f(k + 1) != WHITE) cornerCapture = false;
    if (emptySquares % 2) cornerCapture = false;
    if (cornerCapture) {
        eval -= 2 * cornerWeight;
        eval -= stableWeight * (emptySquares + 2);
    }
    // BLACK advantage
    if (f(i + 1) != WHITE) cornerCapture = false;
    emptySquares = 0;
    for (k = i + 2; f(k) == -1 && k <= 7; k++) {
        emptySquares++;
    }
    if (f(k + 1) != BLACK) cornerCapture = false;
    if (emptySquares % 2) cornerCapture = false;
    if (cornerCapture) {
        eval += 2 * cornerWeight;
        eval += stableWeight * (emptySquares + 2);
    }
    
    cornerCapture = true;
    i = 7;
    if (f(i) != -1) cornerCapture = false;
    // WHITE advantage
    if (f(i - 1) != BLACK) cornerCapture = false;
    emptySquares = 0;
    for (k = i - 2; f(k) == -1 && k >= 0; k--) {
        emptySquares++;
    }
    if (f(k - 1) != WHITE) cornerCapture = false;
    if (emptySquares % 2) cornerCapture = false;
    if (cornerCapture) {
        eval -= 2 * cornerWeight;
        eval -= stableWeight * (emptySquares + 2);
    }
    // BLACK advantage
    if (f(i - 1) != WHITE) cornerCapture = false;
    emptySquares = 0;
    for (k = i - 2; f(k) == -1 && k >= 0; k--) {
        emptySquares++;
    }
    if (f(k - 1) != BLACK) cornerCapture = false;
    if (emptySquares % 2) cornerCapture = false;
    if (cornerCapture) {
        eval += 2 * cornerWeight;
        eval += stableWeight * (emptySquares + 2);
    }
    */
    /*
    // Special case
    if (f(0) == -1 && f(1) == BLACK && f(2) == BLACK && f(3) == -1 &&
        f(4) == -1 && f(5) == BLACK && f(6) == -1 && f(7) == BLACK) {
        eval -= cornerWeight;
        eval -= 5 * stableWeight;
    }
    else if (f(0) == -1 && f(1) == WHITE && f(2) == WHITE && f(3) == -1 &&
        f(4) == -1 && f(5) == WHITE && f(6) == -1 && f(7) == WHITE) {
        eval += cornerWeight;
        eval += 5 * stableWeight;
    }
    else if (f(7) == -1 && f(6) == BLACK && f(5) == BLACK && f(4) == -1 &&
        f(3) == -1 && f(2) == BLACK && f(1) == -1 && f(0) == BLACK) {
        eval -= cornerWeight;
        eval -= 5 * stableWeight;
    }
    else if (f(7) == -1 && f(6) == WHITE && f(5) == WHITE && f(4) == -1 &&
        f(3) == -1 && f(2) == WHITE && f(1) == -1 && f(0) == WHITE) {
        eval += cornerWeight;
        eval += 5 * stableWeight;
    }
    */
    /*
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
    STABLE_DISCS[7][data] = make_pair(black_stable_discs, white_stable_discs);
    PSEUDOSTABLE_DISCS[7][data] = make_pair(black_pseudostable_discs, white_pseudostable_discs);
    if (f(0) == -1) {
        STABLE_DISCS[6][data] = make_pair(black_stable_discs, white_stable_discs);
        PSEUDOSTABLE_DISCS[6][data] = make_pair(black_pseudostable_discs, white_pseudostable_discs);
        if (f(1) == -1) {
            STABLE_DISCS[5][data] = make_pair(black_stable_discs, white_stable_discs);
            PSEUDOSTABLE_DISCS[5][data] = make_pair(black_pseudostable_discs, white_pseudostable_discs);
            if (f(2) == -1) {
                STABLE_DISCS[4][data] = make_pair(black_stable_discs, white_stable_discs);
                PSEUDOSTABLE_DISCS[4][data] = make_pair(black_pseudostable_discs, white_pseudostable_discs);
                if (f(3) == -1) {
                    STABLE_DISCS[3][data] = make_pair(black_stable_discs, white_stable_discs);
                    PSEUDOSTABLE_DISCS[3][data] = make_pair(black_pseudostable_discs, white_pseudostable_discs);
                    if (f(4) == -1) {
                        STABLE_DISCS[2][data] = make_pair(black_stable_discs, white_stable_discs);
                        PSEUDOSTABLE_DISCS[2][data] = make_pair(black_pseudostable_discs, white_pseudostable_discs);
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < 8; i++) {
        if (f(i) == -1) fil << '-';
        else if (f(i) == BLACK) fil << 'X';
        else fil << 'O';
    }
    fil << ' ' << bitset<8>(black_pseudostable_discs) << ' ' << bitset<8>(white_pseudostable_discs) << ' ' << eval << endl;
    */
    // ./testgame freezes when you uncomment this for who knows what reason
    /*
    bitset<8> bs((unsigned char) (data >> 8));
    bitset<8> bs2((unsigned char) data);
    cerr << bs << ' ' << bs2 << ' ' << eval << endl;
    cerr << "---" << endl;
    */
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
 * within 30 seconds.
 */
Player::Player(Side s) : side(s), currBoard(Board()) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;
    
    playerConstructorHelper();
    playerConstructorHelper2();
}

Player::~Player() {}

// Static evaluation function
inline int eval(const Board &b2) {
    int blackMoves = __builtin_popcountll(b2.findLegalMovesBlack() & SAFE_SQUARES);
    int whiteMoves = __builtin_popcountll(b2.findLegalMovesWhite() & SAFE_SQUARES);
    return MOBILITY_WEIGHT * (blackMoves - whiteMoves) / (blackMoves + whiteMoves + 2) + b2.pos_evaluate();
}

int abCalls = 0;
int alphabeta(const Board &b, const int &depth, const Side &s, int alpha, int beta, const int &depth2, bool prevPass) {
    abCalls++;
    
    //~ assert(alpha < beta);
    
    //~ cerr << "ab called with " << depth << ' ' << alpha << ' ' << beta << endl;
    //~ int totalCount = __builtin_popcountll(b.taken);
    if (depth <= 0) {
        #if SIMPLE_EVAL
        int blackMoves = __builtin_popcountll(b.findLegalMovesBlack() & SAFE_SQUARES);
        int whiteMoves = __builtin_popcountll(b.findLegalMovesWhite() & SAFE_SQUARES);
        int result = MOBILITY_WEIGHT * (blackMoves - whiteMoves) / (blackMoves + whiteMoves + 2) + b.pos_evaluate();
        return result;
        #else
        int position = b.pos_evaluate();
        int mobilityBoost = MOBILITY_BOOST;
        if (s == BLACK) {
            uint64_t lm = b.findLegalMovesBlack();
            int blackMoves = __builtin_popcountll(lm & SAFE_SQUARES);
            int v = INT_MAX;
            int eval = INT_MIN;
            while (lm && eval < beta) {
                int index = __builtin_clzl(lm);
                lm ^= BIT(index);
                int val = __builtin_popcountll(b.doMoveOnNewBoard(index, BLACK).findLegalMoves(WHITE) & SAFE_SQUARES);
                if (val < v) {
                    eval = MOBILITY_WEIGHT * (blackMoves - val) / (blackMoves + val + 2) + position + ((val == 0) ? mobilityBoost : 0);
                    v = val;
                }
            }
            if (eval == INT_MIN) eval = -mobilityWeight + position - mobilityBoost;
            return eval;
        }
        else {
            uint64_t lm = b.findLegalMovesWhite();
            int whiteMoves = __builtin_popcountll(lm & SAFE_SQUARES);
            int v = INT_MAX;
            int eval = INT_MAX;
            while (lm && eval > alpha) {
                int index = __builtin_clzl(lm);
                lm ^= BIT(index);
                int val = __builtin_popcountll(b.doMoveOnNewBoard(index, WHITE).findLegalMoves(BLACK) & SAFE_SQUARES);
                if (val < v) {
                    eval = MOBILITY_WEIGHT * (val - whiteMoves) / (whiteMoves + val + 2) + position - ((val == 0) ? mobilityBoost : 0);
                    v = val;
                }
            }
            if (eval == INT_MAX) eval = mobilityWeight + position + mobilityBoost;
            return eval;
        }
        #endif
    }        

    BoardWithSide bws(b.taken, b.black, b.zobrist_hash, s);
    size_t hash_index = b.zobrist_hash;
    uint64_t legalMoves = b.findLegalMoves(s);
    
    // Special case
    if (!legalMoves) {
        if (prevPass) {
            int blacks = b.countBlack();
            int whites = b.countWhite();
            return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
        }
        return alphabeta(b, depth - 1, other_side(s), alpha, beta, depth2 + 1, true);
    }
    
    // TODO: Avoid saving every single position's best move
    uint8_t besti = 64;
    if (s) {
        int v = INT_MIN;
        // Best move
        uint8_t index = 64;
        if (depth2 <= HASH_DEPTH) {
            index = tt[hash_index];
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
        if (depth2 <= HASH_DEPTH) tt[hash_index] = besti;
        return alpha;
    }
    else {
        int v = INT_MAX;
        // Best move
        uint8_t index = 64;
        if (depth2 <= HASH_DEPTH) {
            index = tt[hash_index];
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
        if (depth2 <= HASH_DEPTH) tt[hash_index] = besti;
        return beta;
    }
}

int pvsBlack(const Board &b, const int &depth, int alpha = INT_MIN, const int &beta = INT_MAX, const int &depth2 = 0, const bool &prevPass = false);
int pvsBlackNull(const Board &b, const int &depth, const int &beta = INT_MAX, const int &depth2 = 0, const bool &prevPass = false);
int pvsWhite(const Board &b, const int &depth, const int &alpha = INT_MIN, int beta = INT_MAX, const int &depth2 = 0, const bool &prevPass = false);
int pvsWhiteNull(const Board &b, const int &depth, const int &alpha = INT_MIN, const int &depth2 = 0, const bool &prevPass = false);
int pvsCalls = 0;
int tthits = 0;
uint8_t killerMoves[MAX_DEPTH][3];

int pvs(const Board &b, const int &depth, const Side &s, int alpha = INT_MIN, int beta = INT_MAX, const int &depth2 = 0, const bool &prevPass = false) {
    if (s) return pvsBlack(b, depth, alpha, beta, depth2, prevPass);
    else return pvsWhite(b, depth, alpha, beta, depth2, prevPass);
}

int pvsBlack(const Board &b, const int &depth, int alpha, const int &beta, const int &depth2, const bool &prevPass) {
    pvsCalls++;
    
    // Should rarely be called
    if (__builtin_expect(depth <= 0, 0)) {
        #if SIMPLE_EVAL
        int blackMoves = __builtin_popcountll(b.findLegalMovesBlack() & SAFE_SQUARES);
        int whiteMoves = __builtin_popcountll(b.findLegalMovesWhite() & SAFE_SQUARES);
        int result = MOBILITY_WEIGHT * (blackMoves - whiteMoves) / (blackMoves + whiteMoves + 2) + b.pos_evaluate();
        return result;
        #else
        int position = b.pos_evaluate();
        int mobilityBoost = MOBILITY_BOOST;
        if (s == BLACK) {
            uint64_t lm = b.findLegalMovesBlack();
            int blackMoves = __builtin_popcountll(lm & SAFE_SQUARES);
            int v = INT_MAX;
            int eval = INT_MIN;
            while (lm && eval < beta) {
                int index = __builtin_clzl(lm);
                lm ^= BIT(index);
                int val = __builtin_popcountll(b.doMoveOnNewBoard(index, BLACK).findLegalMoves(WHITE) & SAFE_SQUARES);
                if (val < v) {
                    eval = MOBILITY_WEIGHT * (blackMoves - val) / (blackMoves + val + 2) + position + ((val == 0) ? mobilityBoost : 0);
                    v = val;
                }
            }
            if (eval == INT_MIN) eval = -mobilityWeight + position - mobilityBoost;
            return eval;
        }
        else {
            uint64_t lm = b.findLegalMovesWhite();
            int whiteMoves = __builtin_popcountll(lm & SAFE_SQUARES);
            int v = INT_MAX;
            int eval = INT_MAX;
            while (lm && eval > alpha) {
                int index = __builtin_clzl(lm);
                lm ^= BIT(index);
                int val = __builtin_popcountll(b.doMoveOnNewBoard(index, WHITE).findLegalMoves(BLACK) & SAFE_SQUARES);
                if (val < v) {
                    eval = MOBILITY_WEIGHT * (val - whiteMoves) / (whiteMoves + val + 2) + position - ((val == 0) ? mobilityBoost : 0);
                    v = val;
                }
            }
            if (eval == INT_MAX) eval = mobilityWeight + position + mobilityBoost;
            return eval;
        }
        #endif
    }
    
    BoardWithSide bws(b.taken, b.black, b.zobrist_hash, BLACK);
    uint64_t legalMoves = b.findLegalMovesBlack();
        
    // Special case
    if (__builtin_expect(!legalMoves, 0)) {
        if (prevPass) {
            int blacks = b.countBlack();
            int whites = b.countWhite();
            return (blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0);
        }
        return pvsWhite(b, depth - 1, alpha, beta, depth2 + 1, true);
    }
    
    #if KILLER_HEURISTIC
    uint8_t km1 = killerMoves[depth2][0];
    uint8_t km2 = killerMoves[depth2][1];
    //~ uint8_t km3 = killerMoves[depth2][2];
    #endif
    
    if (depth == 1) {
        // Fall back to alphabeta
        int v = INT_MIN;
        // Best move
        uint8_t index = 64;
        uint8_t besti = 64;
        if (depth2 <= HASH_DEPTH) {
            size_t hash_index = b.zobrist_hash;
            index = tt[hash_index];
            if (BIT_SAFE(index) & legalMoves) {
                // The move is valid
                tthits++;
                legalMoves ^= BIT(index);
                besti = index;
                v = eval(b.doMoveOnNewBoardBlack(index));
                alpha = (v > alpha) ? v : alpha;
            }
            #if KILLER_HEURISTIC
            index = km1;
            if (alpha < beta && (BIT(index) & legalMoves)) {
                // The move is valid
                legalMoves ^= BIT(index);
                int val = eval(b.doMoveOnNewBoardBlack(index));
                bool temp = (val > v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                alpha = (v > alpha) ? v : alpha;
            }
            index = km2;
            if (alpha < beta && (BIT(index) & legalMoves)) {
                // The move is valid
                legalMoves ^= BIT(index);
                int val = eval(b.doMoveOnNewBoardBlack(index));
                bool temp = (val > v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                alpha = (v > alpha) ? v : alpha;
            }
            //~ index = km3;
            //~ if (alpha < beta && (BIT(index) & legalMoves)) {
                //~ // The move is valid
                //~ legalMoves ^= BIT(index);
                //~ int val = eval(b.doMoveOnNewBoardBlack(index));
                //~ bool temp = (val > v);
                //~ besti = temp ? index : besti;
                //~ v = temp ? val : v;
                //~ alpha = (v > alpha) ? v : alpha;
            //~ }
            #endif
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzl(legalMoves);
                legalMoves ^= BIT(index);
                int val = eval(b.doMoveOnNewBoardBlack(index));
                bool temp = (val > v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                alpha = (v > alpha) ? v : alpha;
            }
            tt[hash_index] = besti;
        }
        else {
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzl(legalMoves);
                legalMoves ^= BIT(index);
                int val = eval(b.doMoveOnNewBoardBlack(index));
                #if KILLER_HEURISTIC
                besti = (val > alpha) ? index : besti;
                #endif
                alpha = (val > alpha) ? val : alpha;
            }
        }
        #if KILLER_HEURISTIC
        if (alpha >= beta && km1 != besti) {
            //~ killerMoves[depth2][2] = km2;
            killerMoves[depth2][1] = km1;
            killerMoves[depth2][0] = besti;
        }
        #endif
        return alpha;
    }
    
    uint8_t besti = 64;
    int v = INT_MIN;
    // Best move
    uint8_t index = 64;
    if (depth2 <= HASH_DEPTH) {
        size_t hash_index = b.zobrist_hash;
        index = tt[hash_index];
        if (BIT_SAFE(index) & legalMoves) {
            use_hash:
            // The move is valid
            tthits++;
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
            #if KILLER_HEURISTIC
            index = km1;
            if (BIT(index) & legalMoves) {
                legalMoves ^= BIT(index);
                int val = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
                bool temp = (val > v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                alpha = (v > alpha) ? v : alpha;
            }
            index = km2;
            if (alpha < beta && (BIT(index) & legalMoves)) {
                legalMoves ^= BIT(index);
                int val = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
                bool temp = (val > v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                alpha = (v > alpha) ? v : alpha;
            }
            //~ index = km3;
            //~ if (alpha < beta && (BIT(index) & legalMoves)) {
                //~ legalMoves ^= BIT(index);
                //~ int val = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
                //~ bool temp = (val > v);
                //~ besti = temp ? index : besti;
                //~ v = temp ? val : v;
                //~ alpha = (v > alpha) ? v : alpha;
            //~ }
            #endif
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
        tt[hash_index] = besti;
    }
    else {
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzl(legalMoves);
            legalMoves ^= BIT(index);
            int val = pvsWhite(b.doMoveOnNewBoardBlack(index), depth - 1, alpha, beta, depth2 + 1);
            #if KILLER_HEURISTIC
            besti = (val > alpha) ? index : besti;
            #endif
            alpha = (val > alpha) ? val : alpha;
        }
    }
    #if KILLER_HEURISTIC
    if (alpha >= beta && km1 != besti) {
        //~ killerMoves[depth2][2] = km2;
        killerMoves[depth2][1] = km1;
        killerMoves[depth2][0] = besti;
    }
    #endif
    return alpha;
}

int pvsWhite(const Board &b, const int &depth, const int &alpha, int beta, const int &depth2, const bool &prevPass) {
    pvsCalls++;
    
    // Should rarely be called
    if (__builtin_expect(depth <= 0, 0)) {
        #if SIMPLE_EVAL
        int blackMoves = __builtin_popcountll(b.findLegalMovesBlack() & SAFE_SQUARES);
        int whiteMoves = __builtin_popcountll(b.findLegalMovesWhite() & SAFE_SQUARES);
        int result = MOBILITY_WEIGHT * (blackMoves - whiteMoves) / (blackMoves + whiteMoves + 2) + b.pos_evaluate();
        return min(result, beta);
        #else
        int position = b.pos_evaluate();
        int mobilityBoost = MOBILITY_BOOST;
        if (s == BLACK) {
            uint64_t lm = b.findLegalMovesBlack();
            int blackMoves = __builtin_popcountll(lm & SAFE_SQUARES);
            int v = INT_MAX;
            int eval = INT_MIN;
            while (lm && eval < beta) {
                int index = __builtin_clzl(lm);
                lm ^= BIT(index);
                int val = __builtin_popcountll(b.doMoveOnNewBoard(index, BLACK).findLegalMoves(WHITE) & SAFE_SQUARES);
                if (val < v) {
                    eval = MOBILITY_WEIGHT * (blackMoves - val) / (blackMoves + val + 2) + position + ((val == 0) ? mobilityBoost : 0);
                    v = val;
                }
            }
            if (eval == INT_MIN) eval = -mobilityWeight + position - mobilityBoost;
            return eval;
        }
        else {
            uint64_t lm = b.findLegalMovesWhite();
            int whiteMoves = __builtin_popcountll(lm & SAFE_SQUARES);
            int v = INT_MAX;
            int eval = INT_MAX;
            while (lm && eval > alpha) {
                int index = __builtin_clzl(lm);
                lm ^= BIT(index);
                int val = __builtin_popcountll(b.doMoveOnNewBoard(index, WHITE).findLegalMoves(BLACK) & SAFE_SQUARES);
                if (val < v) {
                    eval = MOBILITY_WEIGHT * (val - whiteMoves) / (whiteMoves + val + 2) + position - ((val == 0) ? mobilityBoost : 0);
                    v = val;
                }
            }
            if (eval == INT_MAX) eval = mobilityWeight + position + mobilityBoost;
            return eval;
        }
        #endif
    }
    
    BoardWithSide bws(b.taken, b.black, b.zobrist_hash, WHITE);
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

    #if KILLER_HEURISTIC
    uint8_t km1 = killerMoves[depth2][0];
    uint8_t km2 = killerMoves[depth2][1];
    //~ uint8_t km3 = killerMoves[depth2][2];
    #endif
    
    // Note: What if there is a pass that jumps from depth 2 to depth 0?
    // Maybe this case is rare enough to not need to consider it.
    if (depth == 1) {
        int v = INT_MAX;
        size_t hash_index = b.zobrist_hash;
        // Best move
        uint8_t index = 64;
        uint8_t besti = 64;
        if (depth2 <= HASH_DEPTH) {
            index = tt[hash_index];
            if (BIT_SAFE(index) & legalMoves) {
                // The move is valid
                tthits++;
                legalMoves ^= BIT(index);
                besti = index;
                v = eval(b.doMoveOnNewBoardWhite(index));
                beta = (v < beta) ? v : beta;
            }
            #if KILLER_HEURISTIC
            index = km1;
            if (alpha < beta && (BIT(index) & legalMoves)) {
                // The move is valid
                legalMoves ^= BIT(index);
                int val = eval(b.doMoveOnNewBoardWhite(index));
                bool temp = (val < v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                beta = (v < beta) ? v : beta;
            }
            index = km2;
            if (alpha < beta && (BIT(index) & legalMoves)) {
                // The move is valid
                legalMoves ^= BIT(index);
                int val = eval(b.doMoveOnNewBoardWhite(index));
                bool temp = (val < v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                beta = (v < beta) ? v : beta;
            }
            //~ index = km3;
            //~ if (alpha < beta && (BIT(index) & legalMoves)) {
                //~ // The move is valid
                //~ legalMoves ^= BIT(index);
                //~ int val = eval(b.doMoveOnNewBoardWhite(index));
                //~ bool temp = (val < v);
                //~ besti = temp ? index : besti;
                //~ v = temp ? val : v;
                //~ beta = (v < beta) ? v : beta;
            //~ }
            #endif
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzl(legalMoves);
                legalMoves ^= BIT(index);
                int val = eval(b.doMoveOnNewBoardWhite(index));
                bool temp = (val < v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                beta = (v < beta) ? v : beta;
            }
            tt[hash_index] = besti;
        }
        else {
            while (alpha < beta && legalMoves) {    
                uint8_t index = __builtin_clzl(legalMoves);
                legalMoves ^= BIT(index);
                int val = eval(b.doMoveOnNewBoardWhite(index));
                #if KILLER_HEURISTIC
                besti = (val < beta) ? index : besti;
                #endif
                beta = (val < beta) ? val : beta;
            }
        }
        #if KILLER_HEURISTIC
        if (alpha >= beta && km1 != besti) {
            //~ killerMoves[depth2][2] = km2;
            killerMoves[depth2][1] = km1;
            killerMoves[depth2][0] = besti;
        }
        #endif
        return beta;
    }
    
    uint8_t besti = 64;
    uint8_t index = 64;
    // Best move
    if (depth2 <= HASH_DEPTH) {
        int v = INT_MAX;
        size_t hash_index = b.zobrist_hash;
        index = tt[hash_index];
        if (BIT_SAFE(index) & legalMoves) {
            use_hash:
            // The move is valid
            tthits++;
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
            #if KILLER_HEURISTIC
            index = km1;
            if (BIT(index) & legalMoves) {
                legalMoves ^= BIT(index);
                int val = pvsBlack(b.doMoveOnNewBoardWhite(index), depth - 1, alpha, beta, depth2 + 1);
                bool temp = (val < v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                beta = (val < beta) ? val : beta;
            }
            index = km2;
            if (alpha < beta && (BIT(index) & legalMoves)) {
                legalMoves ^= BIT(index);
                int val = pvsBlack(b.doMoveOnNewBoardWhite(index), depth - 1, alpha, beta, depth2 + 1);
                bool temp = (val < v);
                besti = temp ? index : besti;
                v = temp ? val : v;
                beta = (val < beta) ? val : beta;
            }
            //~ index = km3;
            //~ if (alpha < beta && (BIT(index) & legalMoves)) {
                //~ legalMoves ^= BIT(index);
                //~ int val = pvsBlack(b.doMoveOnNewBoardWhite(index), depth - 1, alpha, beta, depth2 + 1);
                //~ bool temp = (val < v);
                //~ besti = temp ? index : besti;
                //~ v = temp ? val : v;
                //~ beta = (val < beta) ? val : beta;
            //~ }
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
        tt[hash_index] = besti;
    }
    else {
        #if IID
        pvsWhite(b, depth - 1, alpha, beta, depth2); // Changing HASH_DEPTH will cause an infinite loop here
        #endif
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzl(legalMoves);
            legalMoves ^= BIT(index);
            int val = pvsBlack(b.doMoveOnNewBoardWhite(index), depth - 1, alpha, beta, depth2 + 1);
            #if KILLER_HEURISTIC
            besti = (val < beta) ? index : besti;
            #endif
            beta = (val < beta) ? val : beta;
        }
    }
    #if KILLER_HEURISTIC
    if (alpha >= beta && km1 != besti) {
        //~ killerMoves[depth2][2] = km2;
        killerMoves[depth2][1] = km1;
        killerMoves[depth2][0] = besti;
    }
    #endif
    return beta;
}

pair<int, int> main_minimax_aw(const Board &b, const Side &s, const int &depth, int guess = -1) {
    Timer tim;
    int totalCount = __builtin_popcountll(b.taken);

    #if KILLER_HEURISTIC
    // Reset killerMoves
    for (int i = 0; i < MAX_DEPTH; i++) {
        killerMoves[i][0] = 0;
        killerMoves[i][1] = 0;
        killerMoves[i][2] = 0;
    }
    #endif
    
    int eOdd, eEven;
    // First two plies
    eOdd = pvs(b, 1, s, INT_MIN, INT_MAX);
    eEven = pvs(b, 2, s, INT_MIN, INT_MAX);
    cerr << "Finished depth 1: " << eOdd << endl;
    cerr << "Finished depth 2: " << eEven << endl;
    
    // Middle plies
    for (int d = 3; d < depth; d++) {
        if (d % 2) {
            int diff = abs(eOdd) / 8 + 2 + (totalCount > 25 ? 2 : 0);
            int lower = (eOdd == INT_MIN) ? INT_MIN : eOdd - diff;
            int upper = (eOdd == INT_MAX) ? INT_MAX : eOdd + diff;
            int counter = 1;
            try_again1:
            eOdd = pvs(b, d, s, lower, upper);
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
            int diff = abs(eEven) / 8 + 2 + (totalCount > 25 ? 2 : 0);
            int lower = (eEven == INT_MIN) ? INT_MIN : eEven - diff;
            int upper = (eEven == INT_MAX) ? INT_MAX : eEven + diff;
            int counter = 1;
            try_again2:
            eEven = pvs(b, d, s, lower, upper);
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
        int diff = abs(eOdd) / 8 + 2 + (totalCount > 25 ? 2 : 0);
        int lower = (eOdd == INT_MIN) ? INT_MIN : eOdd - diff;
        int upper = (eOdd == INT_MAX) ? INT_MAX : eOdd + diff;
        try_again3:
        eOdd = pvs(b, depth, s, lower, upper, false);
        // This is bound to run into issues one day
        pair<int, int> result = make_pair(tt[b.zobrist_hash], eOdd);
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
        cerr << "Finished depth " << depth + 1 << " search: " << result.second << ' ';
        tim.endms();
        if (counter < 3) return result;
    }
    else {
        int counter = 1;
        int diff = abs(eEven) / 8 + 2 + (totalCount > 25 ? 2 : 0);
        int lower = (eEven == INT_MIN) ? INT_MIN : eEven - diff;
        int upper = (eEven == INT_MAX) ? INT_MAX : eEven + diff;
        try_again4:
        eEven = pvs(b, depth, s, lower, upper, false);
        // This is bound to run into issues one day
        pair<int, int> result = make_pair(tt[b.zobrist_hash], eEven);
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
        cerr << "Finished depth " << depth << " search: " << result.second << ' ';
        tim.endms();
        if (counter < 3) return result;
    }
    // Okay, maybe one more
    if ((depth  + 1) % 2) {
        int counter = 1;
        int diff = abs(eOdd) / 8 + 2 + (totalCount > 25 ? 2 : 0);
        int lower = (eOdd == INT_MIN) ? INT_MIN : eOdd - diff;
        int upper = (eOdd == INT_MAX) ? INT_MAX : eOdd + diff;
        try_again5:
        eOdd = pvs(b, depth + 1, s, lower, upper, false, -1); // The -1 is to fool pvs into hashing
        // This is bound to run into issues one day
        pair<int, int> result = make_pair(tt[b.zobrist_hash], eOdd);
        if (result.second <= lower && lower != INT_MIN) {
            tim.endms("Recalculating (failed low) ");
            lower -= (diff - 1) * counter;
            counter++;
            if (counter > 3) lower = INT_MIN;
            goto try_again5;
        }
        else if (result.second >= upper && upper != INT_MAX) {
            tim.endms("Recalculating (failed high) ");
            upper += (diff - 1) * counter;
            counter++;
            if (counter > 3) upper = INT_MAX;
            goto try_again5;
        }
        cerr << "Finished depth " << depth + 1 << " search: " << result.second << ' ';
        tim.endms();
        return result;
    }
    else {
        int counter = 1;
        int diff = abs(eEven) / 8 + 2 + (totalCount > 25 ? 2 : 0);
        int lower = (eEven == INT_MIN) ? INT_MIN : eEven - diff;
        int upper = (eEven == INT_MAX) ? INT_MAX : eEven + diff;
        try_again6:
        eEven = pvs(b, depth + 1, s, lower, upper, false, -1); // The -1 is to fool pvs into hashing
        // This is bound to run into issues one day
        pair<int, int> result = make_pair(tt[b.zobrist_hash & (tt.mod - 1)], eEven);
        if (result.second <= lower && lower != INT_MIN) {
            tim.endms("Recalculating (failed low) ");
            lower -= (diff - 1) * counter;
            counter++;
            if (counter > 3) lower = INT_MIN;
            goto try_again6;
        }
        else if (result.second >= upper && upper != INT_MAX) {
            tim.endms("Recalculating (failed high) ");
            upper += (diff - 1) * counter;
            counter++;
            if (counter > 3) upper = INT_MAX;
            goto try_again6;
        }
        cerr << "Finished depth " << depth + 1 << " search: " << result.second << ' ';
        tim.endms();
        return result;
    }
    /*
    // Best time: 1:58
    // DEPTH CANNOT BE ZERO!
    auto start = chrono::high_resolution_clock::now();
    int d = 2;
    int e, lower, upper, counter;
    um5->clear();
    e = pvs(b, depth - 6, s, INT_MIN, INT_MAX);
    cerr << "Finished depth " << depth - 6 << " search: " << e << ' ' << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
    // Denominator 15: 2:21
    // Denominator 17: 2:21
    // Denominator 12: 2:21
    // Denominator 20: 2:21
    // Denominator 25: 2:28
    // Denominator 10: 2:14!
    // Denominator 8: 2:13!
    // Denominator 7: 2:13 
    // Denominator 6: 2:13
    // Denominator 5: 2:14
    d = abs(e) / 8 + 2;
    lower = e - d;
    upper = e + d;
    counter = 1;
    try_again4:
    e = pvs(b, depth - 4, s, lower, upper);
    // Using 2 * counter: 2:27
    // Using 4: 2:29
    // Using d * counter: 2:28
    // Using (d - 1) * counter: 2:27
    // Using (d - 2) * counter: 2:25
    if (e <= lower && lower != INT_MIN) {
        cerr << "Recalculating (failed low)" << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
        lower -= (d - 1) * counter;
        counter++;
        if (counter > 3) lower = INT_MIN;
        goto try_again4;
    }
    else if (e >= upper && upper != INT_MAX) {
        cerr << "Recalculating (failed high)" << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
        upper += (d - 1) * counter;
        counter++;
        if (counter > 3) upper = INT_MAX;
        goto try_again4;
    }
    cerr << "Finished depth " << depth - 4 << " search: " << e << ' ' << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
    lower = e - d;
    upper = e + d;
    counter = 1;
    try_again3:
    e = pvs(b, depth - 2, s, lower, upper, false);
    // Using 2 * counter: 2:27
    // Using 4: 2:29
    // Using d * counter: 2:28
    // Using (d - 1) * counter: 2:27
    // Using (d - 2) * counter: 2:25
    if (e <= lower && lower != INT_MIN) {
        cerr << "Recalculating (failed low)" << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
        lower -= (d - 1) * counter;
        counter++;
        if (counter > 3) lower = INT_MIN;
        goto try_again3;
    }
    else if (e >= upper && upper != INT_MAX) {
        cerr << "Recalculating (failed high)" << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
        upper += (d - 1) * counter;
        counter++;
        if (counter > 3) upper = INT_MAX;
        goto try_again3;
    }
    cerr << "Finished depth " << depth - 2 << " search: " << e << ' ' << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
    lower = e - d; // Put bounds at -25, 25 too?
    upper = e + d;
    counter = 1;
    //~ besti = (*um4)[BoardWithSide(b.taken, b.black, s)];
    try_again2:
    pair<int, int> result;
    pvsCalls = 0; // Temporary
    tthits = 0; // Temporary
    um->clear();
    e = pvs(b, depth, s, lower, upper, false);
    // This is bound to run into issues one day
    result = make_pair(tt.table[b.zobrist_hash & (tt.mod - 1)], e);
    if (result.second <= lower && lower != INT_MIN) {
        cerr << "Recalculating (failed low)" << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
        lower -= (d - 1) * counter;
        counter++;
        if (counter > 3) lower = INT_MIN;
        goto try_again2;
    }
    else if (result.second >= upper && upper != INT_MAX) {
        cerr << "Recalculating (failed high)" << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
        upper += (d - 1) * counter;
        counter++;
        if (counter > 3) upper = INT_MAX;
        goto try_again2;
    }
    cerr << "Finished depth " << depth << " search: " << result.second << ' ' << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
    return result;*/
}

size_t hits63 = 0;
size_t hits62 = 0;
size_t hits61 = 0;
size_t hits60 = 0;

inline int deep_endgame_alphabeta_black(const Board &, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_black_59(const Board &, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_black_60(const Board &, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_black_61(const Board &, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_black_62(const Board &, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_black_63(const Board &, bool prevPass = false);
inline int deep_endgame_alphabeta_white(const Board &, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_white_59(const Board &, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_white_60(const Board &, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_white_61(const Board &, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_white_62(const Board &, int alpha = -1, int beta = 1, bool prevPass = false);
inline int deep_endgame_alphabeta_white_63(const Board &, bool prevPass = false);

inline int deep_endgame_alphabeta_black_63(const Board &b, bool prevPass) {
    uint64_t newblack = b.doMoveOnNewBoardBlackWZH(__builtin_clzll(~b.taken)).black;

    if (b.black == newblack) {
        if (prevPass) {
            return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        }
        return deep_endgame_alphabeta_white_63(b, true);
    }
    return __builtin_popcountll(newblack) - 32;
}

inline int deep_endgame_alphabeta_white_63(const Board &b, bool prevPass) {
    uint64_t newblack = b.doMoveOnNewBoardWhiteWZH(__builtin_clzll(~b.taken)).black;

    if (b.black == newblack) {
        if (prevPass) {
            return __builtin_popcountll(b.black) - __builtin_popcountll(b.taken & ~b.black);
        }
        return deep_endgame_alphabeta_black_63(b, true);
    }
    return __builtin_popcountll(newblack) - 32;
}

inline int deep_endgame_alphabeta_black_62(const Board &b, int alpha, int beta, bool prevPass) {
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

inline int deep_endgame_alphabeta_white_62(const Board &b, int alpha, int beta, bool prevPass) {
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

inline int deep_endgame_alphabeta_black_61(const Board &b, int alpha, int beta, bool prevPass) {
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

inline int deep_endgame_alphabeta_white_61(const Board &b, int alpha, int beta, bool prevPass) {
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

inline int deep_endgame_alphabeta_black_60(const Board &b, int alpha, int beta, bool prevPass) {
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

inline int deep_endgame_alphabeta_white_60(const Board &b, int alpha, int beta, bool prevPass) {
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

inline int deep_endgame_alphabeta_black_59(const Board &b, int alpha, int beta, bool prevPass) {
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

inline int deep_endgame_alphabeta_white_59(const Board &b, int alpha, int beta, bool prevPass) {
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
inline int deep_endgame_alphabeta_black(const Board &b, int alpha, int beta, bool prevPass) {
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


inline int deep_endgame_alphabeta_white(const Board &b, int alpha, int beta, bool prevPass) {
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

int deep_endgame_alphabeta(const Board &b, const Side &s, int alpha = -1, int beta = 1) {
    if (s) return deep_endgame_alphabeta_black(b, (alpha == 0) ? 0 : -1, (beta == 0) ? 0 : 1);
    return deep_endgame_alphabeta_white(b, (alpha == 0) ? 0 : -1, (beta == 0) ? 0 : 1);
}

int endgame_alphabeta(const Board &b, const Side &s, int alpha = -1, int beta = 1) {
    BoardWithSide bws(b.taken, b.black, b.zobrist_hash, s);
    if (um2->count(bws) > 0) {
        return (*um2)[bws];
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
        uint64_t legalMovesOther = b.findLegalMoves(other_side(s));
        if (legalMovesOther == 0) {
            // Simple evaluation here
            return b.countBlack() - b.countWhite();
        }
        int ret = endgame_alphabeta(b, other_side(s), alpha, beta);
        if (totalCount < STOP_SAVING_THRESHOLD) {
            (*um2)[bws] = ret;
        }
        return ret;
    }
    
    if (s) {
        #if USE_HASH_IN_ENDGAME_ALPHABETA
        size_t hash_index = b.zobrist_hash;
        int index = tt[hash_index];
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
        int index = tt[hash_index];
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
        v = -1;
        if (guess > -1) {
            legalMoves ^= SINGLE_BIT[guess];
            Board b2 = b.doMoveOnNewBoard(guess, s);
            int val = endgame_alphabeta(b2, other_side(s), v, 1);
            besti = guess;
            v = val;
        }
        while (legalMoves && v <= 0) {
            int index = __builtin_clzl(legalMoves);
            legalMoves ^= BIT(index);
            
            Board b2 = b.doMoveOnNewBoard(index, s);
            int val = endgame_alphabeta(b2, other_side(s), v, 1);
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
            int val = endgame_alphabeta(b2, other_side(s), -1, v);
            besti = guess;
            v = val;
        }
        while (legalMoves && v >= 0) {
            int index = __builtin_clzl(legalMoves);
            legalMoves ^= BIT(index);
            
            Board b2 = b.doMoveOnNewBoard(index, s);
            int val = endgame_alphabeta(b2, other_side(s), -1, v);
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
        currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(opponentsMove->x, opponentsMove->y), other_side(side));
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
    if (totalCount <= 20) depth = MAX_DEPTH;
    else if (totalCount <= 30) depth = MAX_DEPTH;
    else if (totalCount <= 41) depth = MAX_DEPTH;
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
            if ((side && p.second < 0) || (!side && p.second > 0)) {
                besti = p2.first;
                eval = p2.second;
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
    cerr << totalCount + 1 << " eval: " << eval << ' ' << pvsCalls << ' ' << tthits << ' ' << um->size() << endl; //' ' << positions << endl;
    if (gameSolved) cerr << "(Endgame eval)" << endl;
    if (depth == INT_MAX) cerr << ' ' << globalEndgameNodeCount << endl;
    // if (um->size() > MAX_HASH_SIZE) um->clear(); // Don't want to lose due to too much memory!
    
    // Make move
    string letters = "abcdefgh";
    // cerr << ((side == BLACK) ? "Black " : "White ") << "tentative move: " << letters[FROM_INDEX_X(besti)] << ' ' << FROM_INDEX_Y(besti) + 1 << endl; 
    //t.join();
    //if (totalCount <= 41 && ret_value.first >= 0 && ((side == BLACK) ? (eval < ret_value.second) : (eval > ret_value.second))) besti = ret_value.first; // Better move
    int x = FROM_INDEX_X(besti);
    int y = FROM_INDEX_Y(besti);
    cerr << ((side == BLACK) ? "Black " : "White ") << "Move: " << letters[x] << ' ' << y + 1 << endl;
    currBoard = currBoard.doMoveOnNewBoard(TO_INDEX(x, y), side);
    uint64_t stable = currBoard.stable_discs();
    //cerr << "<61-hits " << hits61 << " 62-hits " << hits62 << " 63-hits " << hits63 << endl;
    //cerr << "Stability returns" << stability_returns << '/' << stability_tries << endl;
    cerr << "Stable discs: " << __builtin_popcountll(stable & currBoard.black) << ' ' << __builtin_popcountll(stable & ~currBoard.black) << endl;
    cerr << bitset<64>(stable & currBoard.black) << endl << bitset<64>(stable & ~currBoard.black) << endl;
    //bitset<64> bsTaken(currBoard.taken), bsBlack(currBoard.black);
    //cerr << bsTaken << endl << bsBlack << endl;
    Move *move = new Move(x, y);
    return move;
    
}
