#include "board.h"


// 6.7 ns average time
uint64_t reflect(uint64_t x) {
    x = ((x >> 36) & 0xF0F0F0Full) | ((x << 36) & 0xF0F0F0F000000000ull) | (0x0F0F0F0FF0F0F0F0ull & x);
    x = ((x << 18) & 0xCCCC0000CCCC0000ull) | ((x >> 18) & 0x0000333300003333ull) | (x & 0x3333CCCC3333CCCCull);
    return ((x << 9) & 0xAA00AA00AA00AA00ull) | ((x >> 9) & 0x0055005500550055ull) | (x & 0x55AA55AA55AA55AAull);
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
    // black |= BIT(18) | BIT(19) | BIT(20) | BIT(21) | BIT(26) | BIT(29) | BIT(34) | BIT(37) | BIT(42) | BIT(43) | BIT(44) | BIT(45);
}

/*
 * Make a board given a taken and black uint64_t.
 */
Board::Board(uint64_t t, uint64_t b) : taken(t), black(b), zobrist_hash(make_zobrist_hash()) {}
Board::Board(uint64_t t, uint64_t b, size_t z) : taken(t), black(b), zobrist_hash(z) {}

size_t Board::make_zobrist_hash() const {
    uint64_t blackCopy = black;
    uint64_t whiteCopy = taken & ~black;
    size_t result = 0;
    
    while (blackCopy) {
        int index = __builtin_clzll(blackCopy);
        blackCopy ^= BIT(index);
        result ^= random_numbers[index];
    }
    while (whiteCopy) {
        int index = __builtin_clzll(whiteCopy);
        whiteCopy ^= BIT(index);
        result ^= random_numbers[index + 64];
    }
    /*
    if (s) result ^= random_numbers[128];
    else result ^= random_numbers[129];
    */
    return result;
}


uint64_t Board::findLegalMoves(const Side &side) const {
    if (side) return findLegalMovesBlack();
    return findLegalMovesWhite();
}

uint64_t Board::findLegalMovesBlack() const {
    
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
    moves |= RIGHT_FILTER & (t >> 1);
    
    // LEFT
    w = white & LEFT_FILTER;
    t = w & (black << 1);
    t |= w & (t << 1);
    t |= w & (t << 1);
    t |= w & (t << 1);
    t |= w & (t << 1);
    t |= w & (t << 1);
    
    moves |= LEFT_FILTER & (t << 1);
    
    // DOWN
    w = white & DOWN_FILTER;
    t = w & (black >> 8);
    t |= w & (t >> 8);
    t |= w & (t >> 8);
    t |= w & (t >> 8);
    t |= w & (t >> 8);
    t |= w & (t >> 8);
    
    moves |= DOWN_FILTER & (t >> 8);
    
    // UP
    w = white & UP_FILTER;
    t = w & (black << 8);
    t |= w & (t << 8);
    t |= w & (t << 8);
    t |= w & (t << 8);
    t |= w & (t << 8);
    t |= w & (t << 8);
    
    moves |= UP_FILTER & (t << 8);
    
    // UP_LEFT
    w = white & UP_LEFT_FILTER;
    t = w & (black << 9);
    t |= w & (t << 9);
    t |= w & (t << 9);
    t |= w & (t << 9);
    t |= w & (t << 9);
    t |= w & (t << 9);
    
    moves |= UP_LEFT_FILTER & (t << 9);
    
    // DOWN_RIGHT
    w = white & DOWN_RIGHT_FILTER;
    t = w & (black >> 9);
    t |= w & (t >> 9);
    t |= w & (t >> 9);
    t |= w & (t >> 9);
    t |= w & (t >> 9);
    t |= w & (t >> 9);
    
    moves |= DOWN_RIGHT_FILTER & (t >> 9);
    
    // UP_RIGHT
    w = white & UP_RIGHT_FILTER;
    t = w & (black << 7);
    t |= w & (t << 7);
    t |= w & (t << 7);
    t |= w & (t << 7);
    t |= w & (t << 7);
    t |= w & (t << 7);
    
    moves |= UP_RIGHT_FILTER & (t << 7);
    
    // DOWN_LEFT
    w = white & DOWN_LEFT_FILTER;
    t = w & (black >> 7);
    t |= w & (t >> 7);
    t |= w & (t >> 7);
    t |= w & (t >> 7);
    t |= w & (t >> 7);
    t |= w & (t >> 7);
    
    moves |= DOWN_LEFT_FILTER & (t >> 7);
    
    return moves & ~taken;
}

uint64_t Board::findLegalMovesWhite() const {
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
    moves |= RIGHT_FILTER & (t >> 1);
    
    // LEFT
    w = black & LEFT_FILTER;
    t = w & (white << 1);
    t |= w & (t << 1);
    t |= w & (t << 1);
    t |= w & (t << 1);
    t |= w & (t << 1);
    t |= w & (t << 1);
    
    moves |= LEFT_FILTER & (t << 1);
    
    // DOWN
    w = black & DOWN_FILTER;
    t = w & (white >> 8);
    t |= w & (t >> 8);
    t |= w & (t >> 8);
    t |= w & (t >> 8);
    t |= w & (t >> 8);
    t |= w & (t >> 8);
    
    moves |= DOWN_FILTER & (t >> 8);
    
    // UP
    w = black & UP_FILTER;
    t = w & (white << 8);
    t |= w & (t << 8);
    t |= w & (t << 8);
    t |= w & (t << 8);
    t |= w & (t << 8);
    t |= w & (t << 8);
    
    moves |= UP_FILTER & (t << 8);
    
    // UP_LEFT
    w = black & UP_LEFT_FILTER;
    t = w & (white << 9);
    t |= w & (t << 9);
    t |= w & (t << 9);
    t |= w & (t << 9);
    t |= w & (t << 9);
    t |= w & (t << 9);
    
    moves |= UP_LEFT_FILTER & (t << 9);
    
    // DOWN_RIGHT
    w = black & DOWN_RIGHT_FILTER;
    t = w & (white >> 9);
    t |= w & (t >> 9);
    t |= w & (t >> 9);
    t |= w & (t >> 9);
    t |= w & (t >> 9);
    t |= w & (t >> 9);
    
    moves |= DOWN_RIGHT_FILTER & (t >> 9);
    
    // UP_RIGHT
    w = black & UP_RIGHT_FILTER;
    t = w & (white << 7);
    t |= w & (t << 7);
    t |= w & (t << 7);
    t |= w & (t << 7);
    t |= w & (t << 7);
    t |= w & (t << 7);
    
    moves |= UP_RIGHT_FILTER & (t << 7);
    
    // DOWN_LEFT
    w = black & DOWN_LEFT_FILTER;
    t = w & (white >> 7);
    t |= w & (t >> 7);
    t |= w & (t >> 7);
    t |= w & (t >> 7);
    t |= w & (t >> 7);
    t |= w & (t >> 7);
    
    moves |= DOWN_LEFT_FILTER & (t >> 7);
    
    return moves & ~taken;
}


Board Board::doMoveOnNewBoard(const int &index, const Side &side) const {
    if (side) return doMoveOnNewBoardBlack(index);
    else return doMoveOnNewBoardWhite(index);
}

Board Board::doMoveOnNewBoardBlack(const int &index) const {
    // Makes move on new board using bit operations
    // This leads to no jumping when converted to assembly
    
    // Observe that if the move is illegal, taken will be updated, 
    // but black will remain the same.
    
    // Note: the if statements here lead to a surprising slight increase
    // in speed. The compiler apparently know how to optimize them
    // better than spelling out to the compiler how not to branch.

    uint64_t white = ~black & taken;
    
    const uint64_t bi = BIT(index);
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
    if (black & LEFT_FILTER & (n << 1)) newblack |= n;

    // RIGHT
    n = bi;
    filtered = white & RIGHT_FILTER;
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    if (black & RIGHT_FILTER & (n >> 1)) newblack |= n;
    
    // DOWN
    n = bi;
    filtered = white & DOWN_FILTER;
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    if (black & DOWN_FILTER & (n >> 8)) newblack |= n;
    
    // UP
    n = bi;
    filtered = white & UP_FILTER;
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    if (black & UP_FILTER & (n << 8)) newblack |= n;
    
    // UP_LEFT
    n = bi;
    filtered = white & UP_LEFT_FILTER;
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    if (black & UP_LEFT_FILTER & (n << 9)) newblack |= n;
    
    // DOWN_RIGHT
    n = bi;
    filtered = white & DOWN_RIGHT_FILTER;
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    if (black & DOWN_RIGHT_FILTER & (n >> 9)) newblack |= n;
    
    // UP_RIGHT
    n = bi;
    filtered = white & UP_RIGHT_FILTER;
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    if (black & UP_RIGHT_FILTER & (n << 7)) newblack |= n;
    
    // DOWN_LEFT
    n = bi;
    filtered = white & DOWN_LEFT_FILTER;
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    if (black & DOWN_LEFT_FILTER & (n >> 7)) newblack |= n;

    // Zobrist hash stuff
    size_t zobrist_hash_copy = zobrist_hash;
    zobrist_hash_copy ^= random_numbers[index];
    uint64_t flipped = black ^ newblack ^ BIT(index);
    while (flipped) {
        int index = __builtin_clzll(flipped);
        flipped ^= BIT(index);
        zobrist_hash_copy ^= random_numbers[index + 64];
        zobrist_hash_copy ^= random_numbers[index];
    }

    return Board(taken | bi, newblack, zobrist_hash_copy);
}

Board Board::doMoveOnNewBoardWhite(const int &index) const {
    // Makes move on new board using bit operations
    // This leads to no jumping when converted to assembly
    
    // Observe that if the move is illegal, taken will be updated, 
    // but black will remain the same.
    
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
    if (white & LEFT_FILTER & (n << 1)) newwhite |= n;
    
    // RIGHT
    n = bi;
    filtered = black & RIGHT_FILTER;
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    if (white & RIGHT_FILTER & (n >> 1)) newwhite |= n;
    
    // DOWN
    n = bi;
    filtered = black & DOWN_FILTER;
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    if (white & DOWN_FILTER & (n >> 8)) newwhite |= n;
    
    // UP
    n = bi;
    filtered = black & UP_FILTER;
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    if (white & UP_FILTER & (n << 8)) newwhite |= n;
    
    // UP_LEFT
    n = bi;
    filtered = black & UP_LEFT_FILTER;
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    if (white & UP_LEFT_FILTER & (n << 9)) newwhite |= n;
    
    // DOWN_RIGHT
    n = bi;
    filtered = black & DOWN_RIGHT_FILTER;
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    if (white & DOWN_RIGHT_FILTER & (n >> 9)) newwhite |= n;
            
    // UP_RIGHT
    n = bi;
    filtered = black & UP_RIGHT_FILTER;
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    if (white & UP_RIGHT_FILTER & (n << 7)) newwhite |= n;
    
    // DOWN_LEFT
    n = bi;
    filtered = black & DOWN_LEFT_FILTER;
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    if (white & DOWN_LEFT_FILTER & (n >> 7)) newwhite |= n;
    
    // Zobrist hash stuff
    size_t zobrist_hash_copy = zobrist_hash;
    zobrist_hash_copy ^= random_numbers[index + 64];
    uint64_t flipped = white ^ newwhite ^ BIT(index);
    while (flipped) {
        int index = __builtin_clzll(flipped);
        flipped ^= BIT(index);
        zobrist_hash_copy ^= random_numbers[index + 64];
        zobrist_hash_copy ^= random_numbers[index];
    }

    return Board(taken | bi, ~newwhite & taken, zobrist_hash_copy);
}

Board Board::doMoveOnNewBoardBlackWZH(const int &index) const {
    // Makes move on new board using bit operations
    // This leads to no jumping when converted to assembly
    
    // Observe that if the move is illegal, taken will be updated, 
    // but black will remain the same.
    
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
    if (black & LEFT_FILTER & (n << 1)) newblack |= n;

    // RIGHT
    n = bi;
    filtered = white & RIGHT_FILTER;
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    if (black & RIGHT_FILTER & (n >> 1)) newblack |= n;
    
    // DOWN
    n = bi;
    filtered = white & DOWN_FILTER;
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    if (black & DOWN_FILTER & (n >> 8)) newblack |= n;
    
    // UP
    n = bi;
    filtered = white & UP_FILTER;
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    if (black & UP_FILTER & (n << 8)) newblack |= n;
    
    // UP_LEFT
    n = bi;
    filtered = white & UP_LEFT_FILTER;
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    if (black & UP_LEFT_FILTER & (n << 9)) newblack |= n;
    
    // DOWN_RIGHT
    n = bi;
    filtered = white & DOWN_RIGHT_FILTER;
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    if (black & DOWN_RIGHT_FILTER & (n >> 9)) newblack |= n;
    
    // UP_RIGHT
    n = bi;
    filtered = white & UP_RIGHT_FILTER;
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    if (black & UP_RIGHT_FILTER & (n << 7)) newblack |= n;
    
    // DOWN_LEFT
    n = bi;
    filtered = white & DOWN_LEFT_FILTER;
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    if (black & DOWN_LEFT_FILTER & (n >> 7)) newblack |= n;

    return Board(taken | bi, newblack, 0);
}

Board Board::doMoveOnNewBoardWhiteWZH(const int &index) const {
    // Makes move on new board using bit operations
    // This leads to no jumping when converted to assembly
    
    // Observe that if the move is illegal, taken will be updated, 
    // but black will remain the same.

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
    if (white & LEFT_FILTER & (n << 1)) newwhite |= n;
    
    // RIGHT
    n = bi;
    filtered = black & RIGHT_FILTER;
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    n |= filtered & (n >> 1);
    if (white & RIGHT_FILTER & (n >> 1)) newwhite |= n;
    
    // DOWN
    n = bi;
    filtered = black & DOWN_FILTER;
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    n |= filtered & (n >> 8);
    if (white & DOWN_FILTER & (n >> 8)) newwhite |= n;
    
    // UP
    n = bi;
    filtered = black & UP_FILTER;
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    n |= filtered & (n << 8);
    if (white & UP_FILTER & (n << 8)) newwhite |= n;
    
    // UP_LEFT
    n = bi;
    filtered = black & UP_LEFT_FILTER;
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    n |= filtered & (n << 9);
    if (white & UP_LEFT_FILTER & (n << 9)) newwhite |= n;
    
    // DOWN_RIGHT
    n = bi;
    filtered = black & DOWN_RIGHT_FILTER;
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    n |= filtered & (n >> 9);
    if (white & DOWN_RIGHT_FILTER & (n >> 9)) newwhite |= n;
            
    // UP_RIGHT
    n = bi;
    filtered = black & UP_RIGHT_FILTER;
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    n |= filtered & (n << 7);
    if (white & UP_RIGHT_FILTER & (n << 7)) newwhite |= n;
    
    // DOWN_LEFT
    n = bi;
    filtered = black & DOWN_LEFT_FILTER;
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    n |= filtered & (n >> 7);
    if (white & DOWN_LEFT_FILTER & (n >> 7)) newwhite |= n;
        
    return Board(taken | bi, ~newwhite & taken, 0);
}


int Board::pos_evaluate() const {
    int ee = 0;
    uint64_t white = taken & ~black;
    
    #if X_SQUARE_PENALTY
    {
        // Penalty for risky squares if corner not filled
        ee -= (!(taken & CORNER_TL) && (BIT(9) & black));
        ee += (!(taken & CORNER_TL) && (BIT(9) & white));
        ee -= (!(taken & CORNER_TR) && (BIT(14) & black));
        ee += (!(taken & CORNER_TR) && (BIT(14) & white));
        ee -= (!(taken & CORNER_BL) && (BIT(49) & black));
        ee += (!(taken & CORNER_BL) && (BIT(49) & white));
        ee -= (!(taken & CORNER_BR) && (BIT(54) & black));
        ee += (!(taken & CORNER_BR) && (BIT(54) & white));
        ee *= X_SQUARE_PENALTY;
        
        /*
        uint64_t temp = ((taken & CORNER_TL) >> 63) ^ 1;
        ee -= (temp & ((BIT(9) & black) >> (63 - 9)));
        ee += (temp & ((BIT(9) & white) >> (63 - 9)));
        
        temp = ((taken & CORNER_TR) >> (63 - 7)) ^ 1;
        ee -= (temp & ((BIT(14) & black) >> (63 - 14)));
        ee += (temp & ((BIT(14) & white) >> (63 - 14)));
        
        temp = ((taken & CORNER_BL) >> (63 - 56)) ^ 1;
        ee -= (temp & ((BIT(49) & black) >> (63 - 49)));
        ee += (temp & ((BIT(49) & white) >> (63 - 49)));
        
        temp = (taken & CORNER_BR) ^ 1;
        ee -= (temp & ((BIT(54) & black) >> (63 - 54)));
        ee += (temp & ((BIT(54) & white) >> (63 - 54)));
        */
    }
    #endif
    
    const int totalCount = __builtin_popcountll(taken);
    // Minimize discs early
    if (totalCount < 40) {
        int discdiff = (__builtin_popcountll(white) - __builtin_popcountll(black));
        ee += discdiff / 5;
    }
    else if (totalCount > 45) {
        // Consider some non-edge stable discs toward the end
        uint64_t stable_not_edge = stable_discs() & ~EDGES;
        ee += 3 * __builtin_popcountll(stable_not_edge & black);
        ee -= 3 * __builtin_popcountll(stable_not_edge & taken & ~black);
    }

    // Get top and bottom edge into uint16
    uint16_t u16 = ((taken >> 56) << 8) | (black >> 56);
    ee += EDGE_VALUES[u16];
    u16 = (taken << 8) | black;
    ee += EDGE_VALUES[u16];

    // Get left and right edges
    ee += EDGE_VALUES[(COL(taken, 0) << 8) | COL(black, 0)];
    ee += EDGE_VALUES[(COL(taken, 7) << 8) | COL(black, 7)];
    
    return ee;
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


// 64-68 ns
uint64_t Board::stable_discs() const {
    uint64_t reflected_taken = reflect(taken);
    uint64_t reflected_black = reflect(black);
    
    // Pattern recognition on edges
    // Get top and bottom edge into uint16
    uint64_t stable = (uint64_t) ALL_STABLE_DISCS[7][((taken >> 48) & 0xFF00) | (black >> 56)] << 56;
    stable |= ALL_STABLE_DISCS[7][((taken << 8) & 0xFF00) | (black & 0xFF)];
    
    // Get left and right edges
    uint64_t reflected_stable = ALL_STABLE_DISCS[7][((reflected_taken << 8) & 0xFF00) | (reflected_black & 0xFF)];
    reflected_stable |= (uint64_t) ALL_STABLE_DISCS[7][((reflected_taken >> 48) & 0xFF00) | (reflected_black >> 56)] << 56;

    // Other row/column stability 
    #define ROW_COL_CHECK_HELPER(i) ((uint64_t) STABLE_DISCS[7][((taken >> (40 - 8 * i)) & 0xFF00) | (0xFF & (black >> (48 - 8 * i)))] << (48 - 8 * i));
    #define ROW_CHECK(i) stable |= ROW_COL_CHECK_HELPER(i)
    #define COL_CHECK(i) reflected_stable |= ROW_COL_CHECK_HELPER(i)
    ROW_CHECK(0);
    ROW_CHECK(1);
    ROW_CHECK(2);
    ROW_CHECK(3);
    ROW_CHECK(4);
    ROW_CHECK(5);
    COL_CHECK(0);
    COL_CHECK(1);
    COL_CHECK(2);
    COL_CHECK(3);
    COL_CHECK(4);
    COL_CHECK(5);
    stable &= reflect(reflected_stable);

    // Diagonals
    #define LOOP_INNER(i) stable &= ~pattern_mask[i] | BYTE_TO_PATTERN[i][STABLE_DISCS[pattern_size[i] - 1][((uint16_t) PATTERN_TO_BYTE(i, taken & pattern_mask[i]) << 8) | PATTERN_TO_BYTE(i, black & pattern_mask[i])]]; 
    #define LOOP_INNER_SE(i) stable &= ~pattern_mask[i] | BYTE_TO_PATTERN[i][STABLE_DISCS[pattern_size[i] - 1][((uint16_t) PATTERN_TO_BYTE_SE(i, taken & pattern_mask[i]) << 8) | PATTERN_TO_BYTE_SE(i, black & pattern_mask[i])]]; 
    #define LOOP_INNER_NW(i) stable &= ~pattern_mask[i] | BYTE_TO_PATTERN[i][STABLE_DISCS[pattern_size[i] - 1][((uint16_t) PATTERN_TO_BYTE_NW(i, taken & pattern_mask[i]) << 8) | PATTERN_TO_BYTE_NW(i, black & pattern_mask[i])]];
    LOOP_INNER_NW(12);
    LOOP_INNER(13);
    LOOP_INNER(14);
    LOOP_INNER_SE(15);
    LOOP_INNER_NW(16);
    LOOP_INNER(17);
    LOOP_INNER(18);
    LOOP_INNER_SE(19);
    LOOP_INNER_NW(20);
    LOOP_INNER(21);
    LOOP_INNER(22);
    LOOP_INNER_SE(23);
    LOOP_INNER_NW(24);
    LOOP_INNER(25);
    LOOP_INNER(26);
    LOOP_INNER_SE(27);
    LOOP_INNER_NW(28);
    LOOP_INNER(29);
    LOOP_INNER(30);
    LOOP_INNER_SE(31);
    LOOP_INNER_NW(32);
    LOOP_INNER(33);
    return stable;
}
