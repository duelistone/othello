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


#if NEW_WZH
Board Board::doMoveOnNewBoardBlackWZH(const int &index) const {
    // Makes move on new board using bit operations
    // This leads to no jumping when converted to assembly
    
    // Observe that if the move is illegal, taken will be updated, 
    // but black will remain the same.
    
    uint64_t white = ~black & taken;
    
    const uint64_t bi = BIT(index);
    uint64_t n = bi;
    
    uint64_t newblack = black;
    uint64_t filtered;
    
    #define DMB(x, filter, shifter, shift_amount) n = bi; filtered = white & filter; for (size_t i = 0; i < (x); i++) n |= filtered & (n shifter shift_amount); if (black & filter & (n shifter shift_amount)) newblack |= n;
    #define RIGHT(x) DMB(x, RIGHT_FILTER, >>, 1)
    #define DOWN(x) DMB(x, DOWN_FILTER, >>, 8)
    #define DOWN_RIGHT(x) DMB(x, DOWN_RIGHT_FILTER, >>, 9)
    #define DOWN_LEFT(x) DMB(x, DOWN_LEFT_FILTER, >>, 7)
    #define UP(x) DMB(x, UP_FILTER, <<, 8)
    #define UP_LEFT(x) DMB(x, UP_LEFT_FILTER, <<, 9)
    #define UP_RIGHT(x) DMB(x, UP_RIGHT_FILTER, <<, 7)
    #define LEFT(x) DMB(x, LEFT_FILTER, << , 1)

    switch (index) {
        case 0:
            RIGHT(6)
            DOWN(6)
            DOWN_RIGHT(6)
            return Board(taken | bi, newblack, 0);
        case 1:
            RIGHT(5)
            DOWN(6)
            DOWN_RIGHT(5)
            return Board(taken | bi, newblack, 0);
        case 2:
            RIGHT(4)
            DOWN(6)
            DOWN_RIGHT(4)
            DOWN_LEFT(1)
            LEFT(1)
            return Board(taken | bi, newblack, 0);
        case 3:
            RIGHT(3)
            DOWN(6)
            DOWN_RIGHT(3)
            DOWN_LEFT(2)
            LEFT(2)
            return Board(taken | bi, newblack, 0);
        case 4:
            RIGHT(2)
            DOWN(6)
            DOWN_RIGHT(2)
            DOWN_LEFT(3)
            LEFT(3)
            return Board(taken | bi, newblack, 0);
        case 5:
            RIGHT(1)
            DOWN(6)
            DOWN_RIGHT(1)
            DOWN_LEFT(4)
            LEFT(4)
            return Board(taken | bi, newblack, 0);
        case 6:
            DOWN(6)
            DOWN_LEFT(5)
            LEFT(5)
            return Board(taken | bi, newblack, 0);
        case 7:
            DOWN(6)
            DOWN_LEFT(6)
            LEFT(6)
            return Board(taken | bi, newblack, 0);
        case 8:
            RIGHT(6)
            DOWN(5)
            DOWN_RIGHT(5)
            return Board(taken | bi, newblack, 0);
        case 9:
            RIGHT(5)
            DOWN(5)
            DOWN_RIGHT(5)
            return Board(taken | bi, newblack, 0);
        case 10:
            RIGHT(4)
            DOWN(5)
            DOWN_RIGHT(4)
            DOWN_LEFT(1)
            LEFT(1)
            return Board(taken | bi, newblack, 0);
        case 11:
            RIGHT(3)
            DOWN(5)
            DOWN_RIGHT(3)
            DOWN_LEFT(2)
            LEFT(2)
            return Board(taken | bi, newblack, 0);
        case 12:
            RIGHT(2)
            DOWN(5)
            DOWN_RIGHT(2)
            DOWN_LEFT(3)
            LEFT(3)
            return Board(taken | bi, newblack, 0);
        case 13:
            RIGHT(1)
            DOWN(5)
            DOWN_RIGHT(1)
            DOWN_LEFT(4)
            LEFT(4)
            return Board(taken | bi, newblack, 0);
        case 14:
            DOWN(5)
            DOWN_LEFT(5)
            LEFT(5)
            return Board(taken | bi, newblack, 0);
        case 15:
            DOWN(5)
            DOWN_LEFT(5)
            LEFT(6)
            return Board(taken | bi, newblack, 0);
        case 16:
            RIGHT(6)
            DOWN(4)
            DOWN_RIGHT(4)
            UP(1)
            UP_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 17:
            RIGHT(5)
            DOWN(4)
            DOWN_RIGHT(4)
            UP(1)
            UP_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 18:
            RIGHT(4)
            DOWN(4)
            DOWN_RIGHT(4)
            DOWN_LEFT(1)
            LEFT(1)
            UP(1)
            UP_LEFT(1)
            UP_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 19:
            RIGHT(3)
            DOWN(4)
            DOWN_RIGHT(3)
            DOWN_LEFT(2)
            LEFT(2)
            UP(1)
            UP_LEFT(1)
            UP_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 20:
            RIGHT(2)
            DOWN(4)
            DOWN_RIGHT(2)
            DOWN_LEFT(3)
            LEFT(3)
            UP(1)
            UP_LEFT(1)
            UP_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 21:
            RIGHT(1)
            DOWN(4)
            DOWN_RIGHT(1)
            DOWN_LEFT(4)
            LEFT(4)
            UP(1)
            UP_LEFT(1)
            UP_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 22:
            DOWN(4)
            DOWN_LEFT(4)
            LEFT(5)
            UP(1)
            UP_LEFT(1)
            return Board(taken | bi, newblack, 0);
        case 23:
            DOWN(4)
            DOWN_LEFT(4)
            LEFT(6)
            UP(1)
            UP_LEFT(1)
            return Board(taken | bi, newblack, 0);
        case 24:
            RIGHT(6)
            DOWN(3)
            DOWN_RIGHT(3)
            UP(2)
            UP_RIGHT(2)
            return Board(taken | bi, newblack, 0);
        case 25:
            RIGHT(5)
            DOWN(3)
            DOWN_RIGHT(3)
            UP(2)
            UP_RIGHT(2)
            return Board(taken | bi, newblack, 0);
        case 26:
            RIGHT(4)
            DOWN(3)
            DOWN_RIGHT(3)
            DOWN_LEFT(1)
            LEFT(1)
            UP(2)
            UP_LEFT(1)
            UP_RIGHT(2)
            return Board(taken | bi, newblack, 0);
        case 27:
            RIGHT(3)
            DOWN(3)
            DOWN_RIGHT(3)
            DOWN_LEFT(2)
            LEFT(2)
            UP(2)
            UP_LEFT(2)
            UP_RIGHT(2)
            return Board(taken | bi, newblack, 0);
        case 28:
            RIGHT(2)
            DOWN(3)
            DOWN_RIGHT(2)
            DOWN_LEFT(3)
            LEFT(3)
            UP(2)
            UP_LEFT(2)
            UP_RIGHT(2)
            return Board(taken | bi, newblack, 0);
        case 29:
            RIGHT(1)
            DOWN(3)
            DOWN_RIGHT(1)
            DOWN_LEFT(3)
            LEFT(4)
            UP(2)
            UP_LEFT(2)
            UP_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 30:
            DOWN(3)
            DOWN_LEFT(3)
            LEFT(5)
            UP(2)
            UP_LEFT(2)
            return Board(taken | bi, newblack, 0);
        case 31:
            DOWN(3)
            DOWN_LEFT(3)
            LEFT(6)
            UP(2)
            UP_LEFT(2)
            return Board(taken | bi, newblack, 0);
        case 32:
            RIGHT(6)
            UP(3)
            UP_RIGHT(3)
            DOWN(2)
            DOWN_RIGHT(2)
            return Board(taken | bi, newblack, 0);
        case 33:
            RIGHT(5)
            UP(3)
            UP_RIGHT(3)
            DOWN(2)
            DOWN_RIGHT(2)
            return Board(taken | bi, newblack, 0);
        case 34:
            RIGHT(4)
            UP(3)
            UP_RIGHT(3)
            UP_LEFT(1)
            LEFT(1)
            DOWN(2)
            DOWN_LEFT(1)
            DOWN_RIGHT(2)
            return Board(taken | bi, newblack, 0);
        case 35:
            RIGHT(3)
            UP(3)
            UP_RIGHT(3)
            UP_LEFT(2)
            LEFT(2)
            DOWN(2)
            DOWN_LEFT(2)
            DOWN_RIGHT(2)
            return Board(taken | bi, newblack, 0);
        case 36:
            RIGHT(2)
            UP(3)
            UP_RIGHT(2)
            UP_LEFT(3)
            LEFT(3)
            DOWN(2)
            DOWN_LEFT(2)
            DOWN_RIGHT(2)
            return Board(taken | bi, newblack, 0);
        case 37:
            RIGHT(1)
            UP(3)
            UP_RIGHT(1)
            UP_LEFT(3)
            LEFT(4)
            DOWN(2)
            DOWN_LEFT(2)
            DOWN_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 38:
            UP(3)
            UP_LEFT(3)
            LEFT(5)
            DOWN(2)
            DOWN_LEFT(2)
            return Board(taken | bi, newblack, 0);
        case 39:
            UP(3)
            UP_LEFT(3)
            LEFT(6)
            DOWN(2)
            DOWN_LEFT(2)
            return Board(taken | bi, newblack, 0);
        case 40:
            RIGHT(6)
            UP(4)
            UP_RIGHT(4)
            DOWN(1)
            DOWN_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 41:
            RIGHT(5)
            UP(4)
            UP_RIGHT(4)
            DOWN(1)
            DOWN_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 42:
            RIGHT(4)
            UP(4)
            UP_RIGHT(4)
            UP_LEFT(1)
            LEFT(1)
            DOWN(1)
            DOWN_LEFT(1)
            DOWN_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 43:
            RIGHT(3)
            UP(4)
            UP_RIGHT(3)
            UP_LEFT(2)
            LEFT(2)
            DOWN(1)
            DOWN_LEFT(1)
            DOWN_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 44:
            RIGHT(2)
            UP(4)
            UP_RIGHT(2)
            UP_LEFT(4)
            LEFT(3)
            DOWN(1)
            DOWN_LEFT(1)
            DOWN_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 45:
            RIGHT(1)
            UP(4)
            UP_RIGHT(1)
            UP_LEFT(4)
            LEFT(4)
            DOWN(1)
            DOWN_LEFT(1)
            DOWN_RIGHT(1)
            return Board(taken | bi, newblack, 0);
        case 46:
            UP(4)
            UP_LEFT(4)
            LEFT(5)
            DOWN(1)
            DOWN_LEFT(1)
            return Board(taken | bi, newblack, 0);
        case 47:
            UP(4)
            UP_LEFT(4)
            LEFT(6)
            DOWN(1)
            DOWN_LEFT(1)
            return Board(taken | bi, newblack, 0);
        case 48:
            RIGHT(6)
            UP(5)
            UP_RIGHT(5)
            return Board(taken | bi, newblack, 0);
        case 49:
            RIGHT(5)
            UP(5)
            UP_RIGHT(5)
            return Board(taken | bi, newblack, 0);
        case 50:
            RIGHT(4)
            UP(5)
            UP_RIGHT(4)
            UP_LEFT(1)
            LEFT(1)
            return Board(taken | bi, newblack, 0);
        case 51:
            RIGHT(3)
            UP(5)
            UP_RIGHT(3)
            UP_LEFT(2)
            LEFT(2)
            return Board(taken | bi, newblack, 0);
        case 52:
            RIGHT(2)
            UP(5)
            UP_RIGHT(2)
            UP_LEFT(3)
            LEFT(3)
            return Board(taken | bi, newblack, 0);
        case 53:
            RIGHT(1)
            UP(5)
            UP_RIGHT(1)
            UP_LEFT(4)
            LEFT(4)
            return Board(taken | bi, newblack, 0);
        case 54:
            UP(5)
            UP_LEFT(5)
            LEFT(5)
            return Board(taken | bi, newblack, 0);
        case 55:
            UP(5)
            UP_LEFT(5)
            LEFT(6)
            return Board(taken | bi, newblack, 0);
        case 56:
            RIGHT(6)
            UP(6)
            UP_RIGHT(6)
            return Board(taken | bi, newblack, 0);
        case 57:
            RIGHT(5)
            UP(6)
            UP_RIGHT(5)
            return Board(taken | bi, newblack, 0);
        case 58:
            RIGHT(4)
            UP(6)
            UP_RIGHT(4)
            UP_LEFT(1)
            LEFT(1)
            return Board(taken | bi, newblack, 0);
        case 59:
            RIGHT(3)
            UP(6)
            UP_RIGHT(3)
            UP_LEFT(2)
            LEFT(2)
            return Board(taken | bi, newblack, 0);
        case 60:
            RIGHT(2)
            UP(6)
            UP_RIGHT(2)
            UP_LEFT(3)
            LEFT(3)
            return Board(taken | bi, newblack, 0);
        case 61:
            RIGHT(1)
            UP(6)
            UP_RIGHT(1)
            UP_LEFT(4)
            LEFT(4)
            return Board(taken | bi, newblack, 0);
        case 62:
            UP(6)
            UP_LEFT(5)
            LEFT(5)
            return Board(taken | bi, newblack, 0);
        case 63:
            UP(6)
            UP_LEFT(6)
            LEFT(6)
    }
    return Board(taken | bi, newblack, 0);
}

Board Board::doMoveOnNewBoardWhiteWZH(const int &index) const {
    // Makes move on new board using bit operations
    // This leads to no jumping when converted to assembly
    
    // Observe that if the move is illegal, taken will be updated, 
    // but black will remain the same.
    
    uint64_t white = ~black & taken;
    
    const uint64_t bi = BIT(index);
    uint64_t n = bi;
    
    uint64_t newwhite = white;
    uint64_t filtered;
        
    #define DMW(x, filter, shifter, shift_amount) n = bi; filtered = black & filter; for (size_t i = 0; i < (x); i++) n |= filtered & (n shifter shift_amount); if (white & filter & (n shifter shift_amount)) newwhite |= n;
    #define RIGHTW(x) DMW(x, RIGHT_FILTER, >>, 1)
    #define DOWNW(x) DMW(x, DOWN_FILTER, >>, 8)
    #define DOWN_RIGHTW(x) DMW(x, DOWN_RIGHT_FILTER, >>, 9)
    #define DOWN_LEFTW(x) DMW(x, DOWN_LEFT_FILTER, >>, 7)
    #define UPW(x) DMW(x, UP_FILTER, <<, 8)
    #define UP_LEFTW(x) DMW(x, UP_LEFT_FILTER, <<, 9)
    #define UP_RIGHTW(x) DMW(x, UP_RIGHT_FILTER, <<, 7)
    #define LEFTW(x) DMW(x, LEFT_FILTER, << , 1)
    switch (index) {
        case 0:
            RIGHTW(6)
            DOWNW(6)
            DOWN_RIGHTW(6)
            return Board(taken | bi, ~newwhite & black, 0);
        case 1:
            RIGHTW(5)
            DOWNW(6)
            DOWN_RIGHTW(5)
            return Board(taken | bi, ~newwhite & black, 0);
        case 2:
            RIGHTW(4)
            DOWNW(6)
            DOWN_RIGHTW(4)
            DOWN_LEFTW(1)
            LEFTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 3:
            RIGHTW(3)
            DOWNW(6)
            DOWN_RIGHTW(3)
            DOWN_LEFTW(2)
            LEFTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 4:
            RIGHTW(2)
            DOWNW(6)
            DOWN_RIGHTW(2)
            DOWN_LEFTW(3)
            LEFTW(3)
            return Board(taken | bi, ~newwhite & black, 0);
        case 5:
            RIGHTW(1)
            DOWNW(6)
            DOWN_RIGHTW(1)
            DOWN_LEFTW(4)
            LEFTW(4)
            return Board(taken | bi, ~newwhite & black, 0);
        case 6:
            DOWNW(6)
            DOWN_LEFTW(5)
            LEFTW(5)
            return Board(taken | bi, ~newwhite & black, 0);
        case 7:
            DOWNW(6)
            DOWN_LEFTW(6)
            LEFTW(6)
            return Board(taken | bi, ~newwhite & black, 0);
        case 8:
            RIGHTW(6)
            DOWNW(5)
            DOWN_RIGHTW(5)
            return Board(taken | bi, ~newwhite & black, 0);
        case 9:
            RIGHTW(5)
            DOWNW(5)
            DOWN_RIGHTW(5)
            return Board(taken | bi, ~newwhite & black, 0);
        case 10:
            RIGHTW(4)
            DOWNW(5)
            DOWN_RIGHTW(4)
            DOWN_LEFTW(1)
            LEFTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 11:
            RIGHTW(3)
            DOWNW(5)
            DOWN_RIGHTW(3)
            DOWN_LEFTW(2)
            LEFTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 12:
            RIGHTW(2)
            DOWNW(5)
            DOWN_RIGHTW(2)
            DOWN_LEFTW(3)
            LEFTW(3)
            return Board(taken | bi, ~newwhite & black, 0);
        case 13:
            RIGHTW(1)
            DOWNW(5)
            DOWN_RIGHTW(1)
            DOWN_LEFTW(4)
            LEFTW(4)
            return Board(taken | bi, ~newwhite & black, 0);
        case 14:
            DOWNW(5)
            DOWN_LEFTW(5)
            LEFTW(5)
            return Board(taken | bi, ~newwhite & black, 0);
        case 15:
            DOWNW(5)
            DOWN_LEFTW(5)
            LEFTW(6)
            return Board(taken | bi, ~newwhite & black, 0);
        case 16:
            RIGHTW(6)
            DOWNW(4)
            DOWN_RIGHTW(4)
            UPW(1)
            UP_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 17:
            RIGHTW(5)
            DOWNW(4)
            DOWN_RIGHTW(4)
            UPW(1)
            UP_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 18:
            RIGHTW(4)
            DOWNW(4)
            DOWN_RIGHTW(4)
            DOWN_LEFTW(1)
            LEFTW(1)
            UPW(1)
            UP_LEFTW(1)
            UP_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 19:
            RIGHTW(3)
            DOWNW(4)
            DOWN_RIGHTW(3)
            DOWN_LEFTW(2)
            LEFTW(2)
            UPW(1)
            UP_LEFTW(1)
            UP_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 20:
            RIGHTW(2)
            DOWNW(4)
            DOWN_RIGHTW(2)
            DOWN_LEFTW(3)
            LEFTW(3)
            UPW(1)
            UP_LEFTW(1)
            UP_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 21:
            RIGHTW(1)
            DOWNW(4)
            DOWN_RIGHTW(1)
            DOWN_LEFTW(4)
            LEFTW(4)
            UPW(1)
            UP_LEFTW(1)
            UP_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 22:
            DOWNW(4)
            DOWN_LEFTW(4)
            LEFTW(5)
            UPW(1)
            UP_LEFTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 23:
            DOWNW(4)
            DOWN_LEFTW(4)
            LEFTW(6)
            UPW(1)
            UP_LEFTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 24:
            RIGHTW(6)
            DOWNW(3)
            DOWN_RIGHTW(3)
            UPW(2)
            UP_RIGHTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 25:
            RIGHTW(5)
            DOWNW(3)
            DOWN_RIGHTW(3)
            UPW(2)
            UP_RIGHTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 26:
            RIGHTW(4)
            DOWNW(3)
            DOWN_RIGHTW(3)
            DOWN_LEFTW(1)
            LEFTW(1)
            UPW(2)
            UP_LEFTW(1)
            UP_RIGHTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 27:
            RIGHTW(3)
            DOWNW(3)
            DOWN_RIGHTW(3)
            DOWN_LEFTW(2)
            LEFTW(2)
            UPW(2)
            UP_LEFTW(2)
            UP_RIGHTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 28:
            RIGHTW(2)
            DOWNW(3)
            DOWN_RIGHTW(2)
            DOWN_LEFTW(3)
            LEFTW(3)
            UPW(2)
            UP_LEFTW(2)
            UP_RIGHTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 29:
            RIGHTW(1)
            DOWNW(3)
            DOWN_RIGHTW(1)
            DOWN_LEFTW(3)
            LEFTW(4)
            UPW(2)
            UP_LEFTW(2)
            UP_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 30:
            DOWNW(3)
            DOWN_LEFTW(3)
            LEFTW(5)
            UPW(2)
            UP_LEFTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 31:
            DOWNW(3)
            DOWN_LEFTW(3)
            LEFTW(6)
            UPW(2)
            UP_LEFTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 32:
            RIGHTW(6)
            UPW(3)
            UP_RIGHTW(3)
            DOWNW(2)
            DOWN_RIGHTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 33:
            RIGHTW(5)
            UPW(3)
            UP_RIGHTW(3)
            DOWNW(2)
            DOWN_RIGHTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 34:
            RIGHTW(4)
            UPW(3)
            UP_RIGHTW(3)
            UP_LEFTW(1)
            LEFTW(1)
            DOWNW(2)
            DOWN_LEFTW(1)
            DOWN_RIGHTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 35:
            RIGHTW(3)
            UPW(3)
            UP_RIGHTW(3)
            UP_LEFTW(2)
            LEFTW(2)
            DOWNW(2)
            DOWN_LEFTW(2)
            DOWN_RIGHTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 36:
            RIGHTW(2)
            UPW(3)
            UP_RIGHTW(2)
            UP_LEFTW(3)
            LEFTW(3)
            DOWNW(2)
            DOWN_LEFTW(2)
            DOWN_RIGHTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 37:
            RIGHTW(1)
            UPW(3)
            UP_RIGHTW(1)
            UP_LEFTW(3)
            LEFTW(4)
            DOWNW(2)
            DOWN_LEFTW(2)
            DOWN_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 38:
            UPW(3)
            UP_LEFTW(3)
            LEFTW(5)
            DOWNW(2)
            DOWN_LEFTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 39:
            UPW(3)
            UP_LEFTW(3)
            LEFTW(6)
            DOWNW(2)
            DOWN_LEFTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 40:
            RIGHTW(6)
            UPW(4)
            UP_RIGHTW(4)
            DOWNW(1)
            DOWN_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 41:
            RIGHTW(5)
            UPW(4)
            UP_RIGHTW(4)
            DOWNW(1)
            DOWN_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 42:
            RIGHTW(4)
            UPW(4)
            UP_RIGHTW(4)
            UP_LEFTW(1)
            LEFTW(1)
            DOWNW(1)
            DOWN_LEFTW(1)
            DOWN_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 43:
            RIGHTW(3)
            UPW(4)
            UP_RIGHTW(3)
            UP_LEFTW(2)
            LEFTW(2)
            DOWNW(1)
            DOWN_LEFTW(1)
            DOWN_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 44:
            RIGHTW(2)
            UPW(4)
            UP_RIGHTW(2)
            UP_LEFTW(4)
            LEFTW(3)
            DOWNW(1)
            DOWN_LEFTW(1)
            DOWN_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 45:
            RIGHTW(1)
            UPW(4)
            UP_RIGHTW(1)
            UP_LEFTW(4)
            LEFTW(4)
            DOWNW(1)
            DOWN_LEFTW(1)
            DOWN_RIGHTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 46:
            UPW(4)
            UP_LEFTW(4)
            LEFTW(5)
            DOWNW(1)
            DOWN_LEFTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 47:
            UPW(4)
            UP_LEFTW(4)
            LEFTW(6)
            DOWNW(1)
            DOWN_LEFTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 48:
            RIGHTW(6)
            UPW(5)
            UP_RIGHTW(5)
            return Board(taken | bi, ~newwhite & black, 0);
        case 49:
            RIGHTW(5)
            UPW(5)
            UP_RIGHTW(5)
            return Board(taken | bi, ~newwhite & black, 0);
        case 50:
            RIGHTW(4)
            UPW(5)
            UP_RIGHTW(4)
            UP_LEFTW(1)
            LEFTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 51:
            RIGHTW(3)
            UPW(5)
            UP_RIGHTW(3)
            UP_LEFTW(2)
            LEFTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 52:
            RIGHTW(2)
            UPW(5)
            UP_RIGHTW(2)
            UP_LEFTW(3)
            LEFTW(3)
            return Board(taken | bi, ~newwhite & black, 0);
        case 53:
            RIGHTW(1)
            UPW(5)
            UP_RIGHTW(1)
            UP_LEFTW(4)
            LEFTW(4)
            return Board(taken | bi, ~newwhite & black, 0);
        case 54:
            UPW(5)
            UP_LEFTW(5)
            LEFTW(5)
            return Board(taken | bi, ~newwhite & black, 0);
        case 55:
            UPW(5)
            UP_LEFTW(5)
            LEFTW(6)
            return Board(taken | bi, ~newwhite & black, 0);
        case 56:
            RIGHTW(6)
            UPW(6)
            UP_RIGHTW(6)
            return Board(taken | bi, ~newwhite & black, 0);
        case 57:
            RIGHTW(5)
            UPW(6)
            UP_RIGHTW(5)
            return Board(taken | bi, ~newwhite & black, 0);
        case 58:
            RIGHTW(4)
            UPW(6)
            UP_RIGHTW(4)
            UP_LEFTW(1)
            LEFTW(1)
            return Board(taken | bi, ~newwhite & black, 0);
        case 59:
            RIGHTW(3)
            UPW(6)
            UP_RIGHTW(3)
            UP_LEFTW(2)
            LEFTW(2)
            return Board(taken | bi, ~newwhite & black, 0);
        case 60:
            RIGHTW(2)
            UPW(6)
            UP_RIGHTW(2)
            UP_LEFTW(3)
            LEFTW(3)
            return Board(taken | bi, ~newwhite & black, 0);
        case 61:
            RIGHTW(1)
            UPW(6)
            UP_RIGHTW(1)
            UP_LEFTW(4)
            LEFTW(4)
            return Board(taken | bi, ~newwhite & black, 0);
        case 62:
            UPW(6)
            UP_LEFTW(5)
            LEFTW(5)
            return Board(taken | bi, ~newwhite & black, 0);
        case 63:
            UPW(6)
            UP_LEFTW(6)
            LEFTW(6)
    }
    return Board(taken | bi, ~newwhite & black, 0);
}

Board Board::doMoveOnNewBoardBlack(const int &index) const {
    uint64_t white = ~black & taken;
    
    const uint64_t bi = BIT(index);
    uint64_t n = bi;
    
    uint64_t newblack = black;
    uint64_t filtered;
    
    switch (index) {
        case 0:
            RIGHT(6)
            DOWN(6)
            DOWN_RIGHT(6)
            break;
        case 1:
            RIGHT(5)
            DOWN(6)
            DOWN_RIGHT(5)
            break;
        case 2:
            RIGHT(4)
            DOWN(6)
            DOWN_RIGHT(4)
            DOWN_LEFT(1)
            LEFT(1)
            break;
        case 3:
            RIGHT(3)
            DOWN(6)
            DOWN_RIGHT(3)
            DOWN_LEFT(2)
            LEFT(2)
            break;
        case 4:
            RIGHT(2)
            DOWN(6)
            DOWN_RIGHT(2)
            DOWN_LEFT(3)
            LEFT(3)
            break;
        case 5:
            RIGHT(1)
            DOWN(6)
            DOWN_RIGHT(1)
            DOWN_LEFT(4)
            LEFT(4)
            break;
        case 6:
            DOWN(6)
            DOWN_LEFT(5)
            LEFT(5)
            break;
        case 7:
            DOWN(6)
            DOWN_LEFT(6)
            LEFT(6)
            break;
        case 8:
            RIGHT(6)
            DOWN(5)
            DOWN_RIGHT(5)
            break;
        case 9:
            RIGHT(5)
            DOWN(5)
            DOWN_RIGHT(5)
            break;
        case 10:
            RIGHT(4)
            DOWN(5)
            DOWN_RIGHT(4)
            DOWN_LEFT(1)
            LEFT(1)
            break;
        case 11:
            RIGHT(3)
            DOWN(5)
            DOWN_RIGHT(3)
            DOWN_LEFT(2)
            LEFT(2)
            break;
        case 12:
            RIGHT(2)
            DOWN(5)
            DOWN_RIGHT(2)
            DOWN_LEFT(3)
            LEFT(3)
            break;
        case 13:
            RIGHT(1)
            DOWN(5)
            DOWN_RIGHT(1)
            DOWN_LEFT(4)
            LEFT(4)
            break;
        case 14:
            DOWN(5)
            DOWN_LEFT(5)
            LEFT(5)
            break;
        case 15:
            DOWN(5)
            DOWN_LEFT(5)
            LEFT(6)
            break;
        case 16:
            RIGHT(6)
            DOWN(4)
            DOWN_RIGHT(4)
            UP(1)
            UP_RIGHT(1)
            break;
        case 17:
            RIGHT(5)
            DOWN(4)
            DOWN_RIGHT(4)
            UP(1)
            UP_RIGHT(1)
            break;
        case 18:
            RIGHT(4)
            DOWN(4)
            DOWN_RIGHT(4)
            DOWN_LEFT(1)
            LEFT(1)
            UP(1)
            UP_LEFT(1)
            UP_RIGHT(1)
            break;
        case 19:
            RIGHT(3)
            DOWN(4)
            DOWN_RIGHT(3)
            DOWN_LEFT(2)
            LEFT(2)
            UP(1)
            UP_LEFT(1)
            UP_RIGHT(1)
            break;
        case 20:
            RIGHT(2)
            DOWN(4)
            DOWN_RIGHT(2)
            DOWN_LEFT(3)
            LEFT(3)
            UP(1)
            UP_LEFT(1)
            UP_RIGHT(1)
            break;
        case 21:
            RIGHT(1)
            DOWN(4)
            DOWN_RIGHT(1)
            DOWN_LEFT(4)
            LEFT(4)
            UP(1)
            UP_LEFT(1)
            UP_RIGHT(1)
            break;
        case 22:
            DOWN(4)
            DOWN_LEFT(4)
            LEFT(5)
            UP(1)
            UP_LEFT(1)
            break;
        case 23:
            DOWN(4)
            DOWN_LEFT(4)
            LEFT(6)
            UP(1)
            UP_LEFT(1)
            break;
        case 24:
            RIGHT(6)
            DOWN(3)
            DOWN_RIGHT(3)
            UP(2)
            UP_RIGHT(2)
            break;
        case 25:
            RIGHT(5)
            DOWN(3)
            DOWN_RIGHT(3)
            UP(2)
            UP_RIGHT(2)
            break;
        case 26:
            RIGHT(4)
            DOWN(3)
            DOWN_RIGHT(3)
            DOWN_LEFT(1)
            LEFT(1)
            UP(2)
            UP_LEFT(1)
            UP_RIGHT(2)
            break;
        case 27:
            RIGHT(3)
            DOWN(3)
            DOWN_RIGHT(3)
            DOWN_LEFT(2)
            LEFT(2)
            UP(2)
            UP_LEFT(2)
            UP_RIGHT(2)
            break;
        case 28:
            RIGHT(2)
            DOWN(3)
            DOWN_RIGHT(2)
            DOWN_LEFT(3)
            LEFT(3)
            UP(2)
            UP_LEFT(2)
            UP_RIGHT(2)
            break;
        case 29:
            RIGHT(1)
            DOWN(3)
            DOWN_RIGHT(1)
            DOWN_LEFT(3)
            LEFT(4)
            UP(2)
            UP_LEFT(2)
            UP_RIGHT(1)
            break;
        case 30:
            DOWN(3)
            DOWN_LEFT(3)
            LEFT(5)
            UP(2)
            UP_LEFT(2)
            break;
        case 31:
            DOWN(3)
            DOWN_LEFT(3)
            LEFT(6)
            UP(2)
            UP_LEFT(2)
            break;
        case 32:
            RIGHT(6)
            UP(3)
            UP_RIGHT(3)
            DOWN(2)
            DOWN_RIGHT(2)
            break;
        case 33:
            RIGHT(5)
            UP(3)
            UP_RIGHT(3)
            DOWN(2)
            DOWN_RIGHT(2)
            break;
        case 34:
            RIGHT(4)
            UP(3)
            UP_RIGHT(3)
            UP_LEFT(1)
            LEFT(1)
            DOWN(2)
            DOWN_LEFT(1)
            DOWN_RIGHT(2)
            break;
        case 35:
            RIGHT(3)
            UP(3)
            UP_RIGHT(3)
            UP_LEFT(2)
            LEFT(2)
            DOWN(2)
            DOWN_LEFT(2)
            DOWN_RIGHT(2)
            break;
        case 36:
            RIGHT(2)
            UP(3)
            UP_RIGHT(2)
            UP_LEFT(3)
            LEFT(3)
            DOWN(2)
            DOWN_LEFT(2)
            DOWN_RIGHT(2)
            break;
        case 37:
            RIGHT(1)
            UP(3)
            UP_RIGHT(1)
            UP_LEFT(3)
            LEFT(4)
            DOWN(2)
            DOWN_LEFT(2)
            DOWN_RIGHT(1)
            break;
        case 38:
            UP(3)
            UP_LEFT(3)
            LEFT(5)
            DOWN(2)
            DOWN_LEFT(2)
            break;
        case 39:
            UP(3)
            UP_LEFT(3)
            LEFT(6)
            DOWN(2)
            DOWN_LEFT(2)
            break;
        case 40:
            RIGHT(6)
            UP(4)
            UP_RIGHT(4)
            DOWN(1)
            DOWN_RIGHT(1)
            break;
        case 41:
            RIGHT(5)
            UP(4)
            UP_RIGHT(4)
            DOWN(1)
            DOWN_RIGHT(1)
            break;
        case 42:
            RIGHT(4)
            UP(4)
            UP_RIGHT(4)
            UP_LEFT(1)
            LEFT(1)
            DOWN(1)
            DOWN_LEFT(1)
            DOWN_RIGHT(1)
            break;
        case 43:
            RIGHT(3)
            UP(4)
            UP_RIGHT(3)
            UP_LEFT(2)
            LEFT(2)
            DOWN(1)
            DOWN_LEFT(1)
            DOWN_RIGHT(1)
            break;
        case 44:
            RIGHT(2)
            UP(4)
            UP_RIGHT(2)
            UP_LEFT(4)
            LEFT(3)
            DOWN(1)
            DOWN_LEFT(1)
            DOWN_RIGHT(1)
            break;
        case 45:
            RIGHT(1)
            UP(4)
            UP_RIGHT(1)
            UP_LEFT(4)
            LEFT(4)
            DOWN(1)
            DOWN_LEFT(1)
            DOWN_RIGHT(1)
            break;
        case 46:
            UP(4)
            UP_LEFT(4)
            LEFT(5)
            DOWN(1)
            DOWN_LEFT(1)
            break;
        case 47:
            UP(4)
            UP_LEFT(4)
            LEFT(6)
            DOWN(1)
            DOWN_LEFT(1)
            break;
        case 48:
            RIGHT(6)
            UP(5)
            UP_RIGHT(5)
            break;
        case 49:
            RIGHT(5)
            UP(5)
            UP_RIGHT(5)
            break;
        case 50:
            RIGHT(4)
            UP(5)
            UP_RIGHT(4)
            UP_LEFT(1)
            LEFT(1)
            break;
        case 51:
            RIGHT(3)
            UP(5)
            UP_RIGHT(3)
            UP_LEFT(2)
            LEFT(2)
            break;
        case 52:
            RIGHT(2)
            UP(5)
            UP_RIGHT(2)
            UP_LEFT(3)
            LEFT(3)
            break;
        case 53:
            RIGHT(1)
            UP(5)
            UP_RIGHT(1)
            UP_LEFT(4)
            LEFT(4)
            break;
        case 54:
            UP(5)
            UP_LEFT(5)
            LEFT(5)
            break;
        case 55:
            UP(5)
            UP_LEFT(5)
            LEFT(6)
            break;
        case 56:
            RIGHT(6)
            UP(6)
            UP_RIGHT(6)
            break;
        case 57:
            RIGHT(5)
            UP(6)
            UP_RIGHT(5)
            break;
        case 58:
            RIGHT(4)
            UP(6)
            UP_RIGHT(4)
            UP_LEFT(1)
            LEFT(1)
            break;
        case 59:
            RIGHT(3)
            UP(6)
            UP_RIGHT(3)
            UP_LEFT(2)
            LEFT(2)
            break;
        case 60:
            RIGHT(2)
            UP(6)
            UP_RIGHT(2)
            UP_LEFT(3)
            LEFT(3)
            break;
        case 61:
            RIGHT(1)
            UP(6)
            UP_RIGHT(1)
            UP_LEFT(4)
            LEFT(4)
            break;
        case 62:
            UP(6)
            UP_LEFT(5)
            LEFT(5)
            break;
        case 63:
            UP(6)
            UP_LEFT(6)
            LEFT(6)
    }
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
    uint64_t white = ~black & taken;
    
    const uint64_t bi = BIT(index);
    uint64_t n = bi;
    
    uint64_t newwhite = white;
    uint64_t filtered;

    switch (index) {
        case 0:
            RIGHTW(6)
            DOWNW(6)
            DOWN_RIGHTW(6)
            break;
        case 1:
            RIGHTW(5)
            DOWNW(6)
            DOWN_RIGHTW(5)
            break;
        case 2:
            RIGHTW(4)
            DOWNW(6)
            DOWN_RIGHTW(4)
            DOWN_LEFTW(1)
            LEFTW(1)
            break;
        case 3:
            RIGHTW(3)
            DOWNW(6)
            DOWN_RIGHTW(3)
            DOWN_LEFTW(2)
            LEFTW(2)
            break;
        case 4:
            RIGHTW(2)
            DOWNW(6)
            DOWN_RIGHTW(2)
            DOWN_LEFTW(3)
            LEFTW(3)
            break;
        case 5:
            RIGHTW(1)
            DOWNW(6)
            DOWN_RIGHTW(1)
            DOWN_LEFTW(4)
            LEFTW(4)
            break;
        case 6:
            DOWNW(6)
            DOWN_LEFTW(5)
            LEFTW(5)
            break;
        case 7:
            DOWNW(6)
            DOWN_LEFTW(6)
            LEFTW(6)
            break;
        case 8:
            RIGHTW(6)
            DOWNW(5)
            DOWN_RIGHTW(5)
            break;
        case 9:
            RIGHTW(5)
            DOWNW(5)
            DOWN_RIGHTW(5)
            break;
        case 10:
            RIGHTW(4)
            DOWNW(5)
            DOWN_RIGHTW(4)
            DOWN_LEFTW(1)
            LEFTW(1)
            break;
        case 11:
            RIGHTW(3)
            DOWNW(5)
            DOWN_RIGHTW(3)
            DOWN_LEFTW(2)
            LEFTW(2)
            break;
        case 12:
            RIGHTW(2)
            DOWNW(5)
            DOWN_RIGHTW(2)
            DOWN_LEFTW(3)
            LEFTW(3)
            break;
        case 13:
            RIGHTW(1)
            DOWNW(5)
            DOWN_RIGHTW(1)
            DOWN_LEFTW(4)
            LEFTW(4)
            break;
        case 14:
            DOWNW(5)
            DOWN_LEFTW(5)
            LEFTW(5)
            break;
        case 15:
            DOWNW(5)
            DOWN_LEFTW(5)
            LEFTW(6)
            break;
        case 16:
            RIGHTW(6)
            DOWNW(4)
            DOWN_RIGHTW(4)
            UPW(1)
            UP_RIGHTW(1)
            break;
        case 17:
            RIGHTW(5)
            DOWNW(4)
            DOWN_RIGHTW(4)
            UPW(1)
            UP_RIGHTW(1)
            break;
        case 18:
            RIGHTW(4)
            DOWNW(4)
            DOWN_RIGHTW(4)
            DOWN_LEFTW(1)
            LEFTW(1)
            UPW(1)
            UP_LEFTW(1)
            UP_RIGHTW(1)
            break;
        case 19:
            RIGHTW(3)
            DOWNW(4)
            DOWN_RIGHTW(3)
            DOWN_LEFTW(2)
            LEFTW(2)
            UPW(1)
            UP_LEFTW(1)
            UP_RIGHTW(1)
            break;
        case 20:
            RIGHTW(2)
            DOWNW(4)
            DOWN_RIGHTW(2)
            DOWN_LEFTW(3)
            LEFTW(3)
            UPW(1)
            UP_LEFTW(1)
            UP_RIGHTW(1)
            break;
        case 21:
            RIGHTW(1)
            DOWNW(4)
            DOWN_RIGHTW(1)
            DOWN_LEFTW(4)
            LEFTW(4)
            UPW(1)
            UP_LEFTW(1)
            UP_RIGHTW(1)
            break;
        case 22:
            DOWNW(4)
            DOWN_LEFTW(4)
            LEFTW(5)
            UPW(1)
            UP_LEFTW(1)
            break;
        case 23:
            DOWNW(4)
            DOWN_LEFTW(4)
            LEFTW(6)
            UPW(1)
            UP_LEFTW(1)
            break;
        case 24:
            RIGHTW(6)
            DOWNW(3)
            DOWN_RIGHTW(3)
            UPW(2)
            UP_RIGHTW(2)
            break;
        case 25:
            RIGHTW(5)
            DOWNW(3)
            DOWN_RIGHTW(3)
            UPW(2)
            UP_RIGHTW(2)
            break;
        case 26:
            RIGHTW(4)
            DOWNW(3)
            DOWN_RIGHTW(3)
            DOWN_LEFTW(1)
            LEFTW(1)
            UPW(2)
            UP_LEFTW(1)
            UP_RIGHTW(2)
            break;
        case 27:
            RIGHTW(3)
            DOWNW(3)
            DOWN_RIGHTW(3)
            DOWN_LEFTW(2)
            LEFTW(2)
            UPW(2)
            UP_LEFTW(2)
            UP_RIGHTW(2)
            break;
        case 28:
            RIGHTW(2)
            DOWNW(3)
            DOWN_RIGHTW(2)
            DOWN_LEFTW(3)
            LEFTW(3)
            UPW(2)
            UP_LEFTW(2)
            UP_RIGHTW(2)
            break;
        case 29:
            RIGHTW(1)
            DOWNW(3)
            DOWN_RIGHTW(1)
            DOWN_LEFTW(3)
            LEFTW(4)
            UPW(2)
            UP_LEFTW(2)
            UP_RIGHTW(1)
            break;
        case 30:
            DOWNW(3)
            DOWN_LEFTW(3)
            LEFTW(5)
            UPW(2)
            UP_LEFTW(2)
            break;
        case 31:
            DOWNW(3)
            DOWN_LEFTW(3)
            LEFTW(6)
            UPW(2)
            UP_LEFTW(2)
            break;
        case 32:
            RIGHTW(6)
            UPW(3)
            UP_RIGHTW(3)
            DOWNW(2)
            DOWN_RIGHTW(2)
            break;
        case 33:
            RIGHTW(5)
            UPW(3)
            UP_RIGHTW(3)
            DOWNW(2)
            DOWN_RIGHTW(2)
            break;
        case 34:
            RIGHTW(4)
            UPW(3)
            UP_RIGHTW(3)
            UP_LEFTW(1)
            LEFTW(1)
            DOWNW(2)
            DOWN_LEFTW(1)
            DOWN_RIGHTW(2)
            break;
        case 35:
            RIGHTW(3)
            UPW(3)
            UP_RIGHTW(3)
            UP_LEFTW(2)
            LEFTW(2)
            DOWNW(2)
            DOWN_LEFTW(2)
            DOWN_RIGHTW(2)
            break;
        case 36:
            RIGHTW(2)
            UPW(3)
            UP_RIGHTW(2)
            UP_LEFTW(3)
            LEFTW(3)
            DOWNW(2)
            DOWN_LEFTW(2)
            DOWN_RIGHTW(2)
            break;
        case 37:
            RIGHTW(1)
            UPW(3)
            UP_RIGHTW(1)
            UP_LEFTW(3)
            LEFTW(4)
            DOWNW(2)
            DOWN_LEFTW(2)
            DOWN_RIGHTW(1)
            break;
        case 38:
            UPW(3)
            UP_LEFTW(3)
            LEFTW(5)
            DOWNW(2)
            DOWN_LEFTW(2)
            break;
        case 39:
            UPW(3)
            UP_LEFTW(3)
            LEFTW(6)
            DOWNW(2)
            DOWN_LEFTW(2)
            break;
        case 40:
            RIGHTW(6)
            UPW(4)
            UP_RIGHTW(4)
            DOWNW(1)
            DOWN_RIGHTW(1)
            break;
        case 41:
            RIGHTW(5)
            UPW(4)
            UP_RIGHTW(4)
            DOWNW(1)
            DOWN_RIGHTW(1)
            break;
        case 42:
            RIGHTW(4)
            UPW(4)
            UP_RIGHTW(4)
            UP_LEFTW(1)
            LEFTW(1)
            DOWNW(1)
            DOWN_LEFTW(1)
            DOWN_RIGHTW(1)
            break;
        case 43:
            RIGHTW(3)
            UPW(4)
            UP_RIGHTW(3)
            UP_LEFTW(2)
            LEFTW(2)
            DOWNW(1)
            DOWN_LEFTW(1)
            DOWN_RIGHTW(1)
            break;
        case 44:
            RIGHTW(2)
            UPW(4)
            UP_RIGHTW(2)
            UP_LEFTW(4)
            LEFTW(3)
            DOWNW(1)
            DOWN_LEFTW(1)
            DOWN_RIGHTW(1)
            break;
        case 45:
            RIGHTW(1)
            UPW(4)
            UP_RIGHTW(1)
            UP_LEFTW(4)
            LEFTW(4)
            DOWNW(1)
            DOWN_LEFTW(1)
            DOWN_RIGHTW(1)
            break;
        case 46:
            UPW(4)
            UP_LEFTW(4)
            LEFTW(5)
            DOWNW(1)
            DOWN_LEFTW(1)
            break;
        case 47:
            UPW(4)
            UP_LEFTW(4)
            LEFTW(6)
            DOWNW(1)
            DOWN_LEFTW(1)
            break;
        case 48:
            RIGHTW(6)
            UPW(5)
            UP_RIGHTW(5)
            break;
        case 49:
            RIGHTW(5)
            UPW(5)
            UP_RIGHTW(5)
            break;
        case 50:
            RIGHTW(4)
            UPW(5)
            UP_RIGHTW(4)
            UP_LEFTW(1)
            LEFTW(1)
            break;
        case 51:
            RIGHTW(3)
            UPW(5)
            UP_RIGHTW(3)
            UP_LEFTW(2)
            LEFTW(2)
            break;
        case 52:
            RIGHTW(2)
            UPW(5)
            UP_RIGHTW(2)
            UP_LEFTW(3)
            LEFTW(3)
            break;
        case 53:
            RIGHTW(1)
            UPW(5)
            UP_RIGHTW(1)
            UP_LEFTW(4)
            LEFTW(4)
            break;
        case 54:
            UPW(5)
            UP_LEFTW(5)
            LEFTW(5)
            break;
        case 55:
            UPW(5)
            UP_LEFTW(5)
            LEFTW(6)
            break;
        case 56:
            RIGHTW(6)
            UPW(6)
            UP_RIGHTW(6)
            break;
        case 57:
            RIGHTW(5)
            UPW(6)
            UP_RIGHTW(5)
            break;
        case 58:
            RIGHTW(4)
            UPW(6)
            UP_RIGHTW(4)
            UP_LEFTW(1)
            LEFTW(1)
            break;
        case 59:
            RIGHTW(3)
            UPW(6)
            UP_RIGHTW(3)
            UP_LEFTW(2)
            LEFTW(2)
            break;
        case 60:
            RIGHTW(2)
            UPW(6)
            UP_RIGHTW(2)
            UP_LEFTW(3)
            LEFTW(3)
            break;
        case 61:
            RIGHTW(1)
            UPW(6)
            UP_RIGHTW(1)
            UP_LEFTW(4)
            LEFTW(4)
            break;
        case 62:
            UPW(6)
            UP_LEFTW(5)
            LEFTW(5)
            break;
        case 63:
            UPW(6)
            UP_LEFTW(6)
            LEFTW(6)
    }
    size_t zobrist_hash_copy = zobrist_hash;
    zobrist_hash_copy ^= random_numbers[index + 64];
    uint64_t flipped = white ^ newwhite ^ BIT(index);
    while (flipped) {
        int index = __builtin_clzll(flipped);
        flipped ^= BIT(index);
        zobrist_hash_copy ^= random_numbers[index + 64];
        zobrist_hash_copy ^= random_numbers[index];
    }
    return Board(taken | bi, ~newwhite & black, zobrist_hash_copy);
}
#else
Board Board::doMoveOnNewBoardBlackWZH(const int &index) const {
    // Makes move on new board using bit operations
    // This leads to no jumping when converted to assembly
    
    // Observe that if the move is illegal, taken will be updated, 
    // but black will remain the same.
    
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

    return Board(taken | bi, newblack, 0);
}

Board Board::doMoveOnNewBoardWhiteWZH(const int &index) const {
    // Makes move on new board using bit operations
    // This leads to no jumping when converted to assembly
    
    // Observe that if the move is illegal, taken will be updated, 
    // but black will remain the same.

    uint64_t white = ~black & taken;
    
    const uint64_t bi = BIT(index);
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
    
    const uint64_t bi = BIT(index);
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
#endif


void Board::print_eval_stats() const {
    cerr << "Position: " << endl;
    cerr << *this << endl;

    uint64_t white = taken & ~black;
    int totalCount = __builtin_popcountll(taken);
    uint64_t stable_not_edge = 0;
    #if STABILITY
    if (taken & CORNERS) stable_not_edge = stable_discs();
    #endif

    // Compute frontier + stability + mobility
    uint64_t empty = ~taken;
    empty &= ~((empty & CORNER_TL) >> 9); // Excluding dangerous X squares
    empty &= ~((empty & CORNER_TR) >> 7);
    empty &= ~((empty & CORNER_BL) << 7);
    empty &= ~((empty & CORNER_BR) << 9);
    uint64_t empty2 = empty; // For excluding c squares
    empty2 &= ~((empty & (CORNER_TL | CORNER_BL)) >> 1);
    empty2 &= ~((empty & (CORNER_TL | CORNER_TR)) >> 8);
    empty2 &= ~((empty & (CORNER_BL | CORNER_BR)) << 8);
    empty2 &= ~((empty & (CORNER_TR | CORNER_BR)) << 1);
    uint64_t frontier = (empty2 >> 1) & taken;
    frontier |= (empty2 << 1) & taken;
    frontier |= (empty2 << 8) & taken;
    frontier |= (empty2 >> 8) & taken;
    frontier |= (empty2 << 9) & taken;
    frontier |= (empty2 >> 9) & taken;
    frontier |= (empty2 << 7) & taken;
    frontier |= (empty2 >> 7) & taken;
    frontier &= ~EDGES;
    frontier &= ~stable_not_edge;
    
    int blackFrontiers = __builtin_popcountll(black & frontier);
    int whiteFrontiers = __builtin_popcountll(white & frontier);
    uint64_t blackLM = findLegalMovesBlack();
    uint64_t whiteLM = findLegalMovesWhite();
    uint64_t commonLM = blackLM & whiteLM;
    int blackMoves = 2 * __builtin_popcountll(blackLM & empty2) - __builtin_popcountll(commonLM);
    int whiteMoves = 2 * __builtin_popcountll(whiteLM & empty2) - __builtin_popcountll(commonLM);
    double eeF = FRONTIER_WEIGHT * (whiteFrontiers - blackFrontiers) / (whiteFrontiers + blackFrontiers + 1.0);
    double eeM = MOBILITY_WEIGHT * (blackMoves - whiteMoves) / (blackMoves + whiteMoves + 1.0);
    double ee = abs(eeF + eeM) * (eeF + eeM) / (abs(eeF) + abs(eeM)) * (1 - 0.2 * (totalCount / 50.0));

    cerr << "Frontier score: " << eeF << endl;
    cerr << "Mobility score: " << eeM << endl;
    cerr << "ee: " << ee << endl;

    // Penalty for risky squares if corner not filled
    uint64_t bad_x_squares = ~empty ^ taken;
    double ee2 = (1 - (totalCount / 40)) * (__builtin_popcountll(bad_x_squares & black) - __builtin_popcountll(bad_x_squares & white)) * X_SQUARE_PENALTY / 4.0;
    cerr << "x square score: " << ee2 << endl;

    // (Possibly) stable nonedges
    #if STABILITY
    double stable_nonedges_score = STABLE_NONEDGES_WEIGHT * (__builtin_popcountll(stable_not_edge & black) - __builtin_popcountll(stable_not_edge & taken & ~black)) / 36.0; 
    cerr << "Stable nonedges: " << stable_nonedges_score << endl;
    ee2 += stable_nonedges_score;
    #endif

    // Penalty for leaving corner hanging
    double corner_hanging_score = CORNER_HANGING_PENALTY * (1 - (totalCount / 70.0)) * (__builtin_popcountll(blackLM & CORNERS) - __builtin_popcountll(whiteLM & CORNERS)) / 4.0;
    cerr << "Corner hanging: " << corner_hanging_score << endl;
    ee2 += corner_hanging_score;
    
    // Get top and bottom edge into uint16
    int start = (1 << 16) * totalCount;
    ee2 += EDGE_WEIGHT * 2 * (8 * (EDGE_VALUES[start + (((taken >> 56) << 8) | (black >> 56))] / 2 + 0.5) * (EDGE_VALUES[start + ((uint16_t) ((taken << 8) | (uint8_t) black))] / 2 + 0.5) * (EDGE_VALUES[start + ((COL(taken, 0) << 8) | COL(black, 0))] / 2 + 0.5) * (EDGE_VALUES[start + ((COL(taken, 7) << 8) | COL(black, 7))] / 2 + 0.5) - 0.5);

    // Edges
    cerr << "Edges: " << EDGE_WEIGHT * 2 * (8 * (EDGE_VALUES[start + (((taken >> 56) << 8) | (black >> 56))] / 2 + 0.5) * (EDGE_VALUES[start + ((uint16_t) ((taken << 8) | (uint8_t) black))] / 2 + 0.5) * (EDGE_VALUES[start + ((COL(taken, 0) << 8) | COL(black, 0))] / 2 + 0.5) * (EDGE_VALUES[start + ((COL(taken, 7) << 8) | COL(black, 7))] / 2 + 0.5) - 0.5) << endl;
    cerr << "ee2: " << ee2 << endl;

    // Minimize discs early
    int discdiff = __builtin_popcountll(white) - __builtin_popcountll(black);
    ee2 += ((79 - totalCount) / 40) * discdiff * DISC_DIFFERENCE_WEIGHT / 60.0;

    // Maximize internal discs late
    ee2 += (totalCount / 40) * ((totalCount - 40) / 26.0) * (__builtin_popcountll(black & INTERNAL_SQUARES) - __builtin_popcountll(white & INTERNAL_SQUARES)) * INTERNAL_DISCS_WEIGHT / 16.0;

    cerr << "Disc difference: " << ((79 - totalCount) / 40) * discdiff * DISC_DIFFERENCE_WEIGHT / 60.0 << endl;
    cerr << "Internal discs: " << (totalCount / 40) * ((totalCount - 40) / 13.0) * (__builtin_popcountll(black & INTERNAL_SQUARES) - __builtin_popcountll(white & INTERNAL_SQUARES)) * INTERNAL_DISCS_WEIGHT / 16.0 << endl;

    // Prefer balanced positions if winning
    ee = abs(ee + ee2) * (ee + ee2) / (abs(ee) + abs(ee2)); 
    cerr << "ee and ee2 combined: " << ee << endl;

    cerr << "Actual eval: " << pos_evaluate() << endl;
}

int Board::pos_evaluate() const {
    uint64_t white = taken & ~black;
    int totalCount = __builtin_popcountll(taken);
    uint64_t stable_not_edge = 0;
    double ee2 = 0;
    #if STABILITY && !LEARNING
    if (taken & CORNERS) {
        stable_not_edge = stable_discs(); // Only calculate if there is a corner
        ee2 += STABLE_NONEDGES_WEIGHT * (__builtin_popcountll(stable_not_edge & black) - __builtin_popcountll(stable_not_edge & white)) / 36.0; 
    }
    #endif

    // Compute frontier + stability + mobility
    uint64_t empty = ~taken;
    empty &= ~((empty & CORNER_TL) >> 9); // Excluding dangerous X squares
    empty &= ~((empty & CORNER_TR) >> 7);
    empty &= ~((empty & CORNER_BL) << 7);
    empty &= ~((empty & CORNER_BR) << 9);
    uint64_t empty2 = empty; // For excluding c squares
    empty2 &= ~((empty & (CORNER_TL | CORNER_BL)) >> 1);
    empty2 &= ~((empty & (CORNER_TL | CORNER_TR)) >> 8);
    empty2 &= ~((empty & (CORNER_BL | CORNER_BR)) << 8);
    empty2 &= ~((empty & (CORNER_TR | CORNER_BR)) << 1);
    uint64_t frontier = (empty2 >> 1) & taken;
    frontier |= (empty2 << 1) & taken;
    frontier |= (empty2 << 8) & taken;
    frontier |= (empty2 >> 8) & taken;
    frontier |= (empty2 << 9) & taken;
    frontier |= (empty2 >> 9) & taken;
    frontier |= (empty2 << 7) & taken;
    frontier |= (empty2 >> 7) & taken;
    frontier &= ~EDGES;
    frontier &= ~stable_not_edge;
    
    int blackFrontiers = __builtin_popcountll(black & frontier);
    int whiteFrontiers = __builtin_popcountll(white & frontier);
    uint64_t blackLM = findLegalMovesBlack();
    uint64_t whiteLM = findLegalMovesWhite();
    uint64_t commonLM = blackLM & whiteLM;
    int blackMoves = 2 * __builtin_popcountll(blackLM & empty2) - __builtin_popcountll(commonLM);
    int whiteMoves = 2 * __builtin_popcountll(whiteLM & empty2) - __builtin_popcountll(commonLM);
    double eeF = FRONTIER_WEIGHT * (whiteFrontiers - blackFrontiers) / (whiteFrontiers + blackFrontiers + 2.0);
    double eeM = MOBILITY_WEIGHT * (blackMoves - whiteMoves) / (blackMoves + whiteMoves + 1.0);
    double ee = abs(eeF + eeM) * (eeF + eeM) / (abs(eeF) + abs(eeM)) * (1 - 0.2 * (totalCount / 50.0));

    // Penalty for risky squares if corner not filled
    uint64_t bad_x_squares = ~empty ^ taken;
    ee2 += (1 - (totalCount / 40)) * (__builtin_popcountll(bad_x_squares & black) - __builtin_popcountll(bad_x_squares & white)) * X_SQUARE_PENALTY / 4.0;
    
    // Penalty for leaving corner hanging
    ee2 += CORNER_HANGING_PENALTY * (1 - (totalCount / 70.0)) * (__builtin_popcountll(blackLM & CORNERS) - __builtin_popcountll(whiteLM & CORNERS)) / 4.0;
    
    // Edges
    int start = (1 << 16) * (totalCount);
    ee2 += EDGE_WEIGHT * 2 * (8 * (EDGE_VALUES[start + (((taken >> 56) << 8) | (black >> 56))] / 2 + 0.5) * (EDGE_VALUES[start + ((uint16_t) ((taken << 8) | (uint8_t) black))] / 2 + 0.5) * (EDGE_VALUES[start + ((COL(taken, 0) << 8) | COL(black, 0))] / 2 + 0.5) * (EDGE_VALUES[start + ((COL(taken, 7) << 8) | COL(black, 7))] / 2 + 0.5) - 0.5);

    // Minimize discs early
    int discdiff = __builtin_popcountll(white) - __builtin_popcountll(black);
    ee2 += ((79 - totalCount) / 40) * discdiff * DISC_DIFFERENCE_WEIGHT / 60.0;

    // Maximize internal discs late
    ee2 += (totalCount / 40) * ((totalCount - 40) / 13.0) * (__builtin_popcountll(black & INTERNAL_SQUARES) - __builtin_popcountll(white & INTERNAL_SQUARES)) * INTERNAL_DISCS_WEIGHT / 16.0;

    // Prefer balanced positions if winning
    ee = abs(ee + ee2) * (ee + ee2) / (abs(ee) + abs(ee2)); 

    return 100 * ee;
}

bool operator==(const BoardWithSide& l, const BoardWithSide& r) {
    return (l.taken == r.taken) && (l.black == r.black) && (l.side == r.side);
}

#if STABILITY
// 64-68 ns, maybe less after edge stability no longer computed
uint64_t Board::stable_discs() const {
    uint64_t reflected_taken = reflect(taken);
    uint64_t reflected_black = reflect(black);
    uint64_t reflected_stable = ~EDGES;
    uint64_t stable = ~EDGES;

    /*
    // Pattern recognition on edges
    // Get top and bottom edge into uint16
    uint64_t stable = (uint64_t) ALL_STABLE_DISCS[7][((taken >> 48) & 0xFF00) | (black >> 56)] << 56;
    stable |= ALL_STABLE_DISCS[7][((taken << 8) & 0xFF00) | (black & 0xFF)];
    
    // Get left and right edges
    uint64_t reflected_stable = ALL_STABLE_DISCS[7][((reflected_taken << 8) & 0xFF00) | (reflected_black & 0xFF)];
    reflected_stable |= (uint64_t) ALL_STABLE_DISCS[7][((reflected_taken >> 48) & 0xFF00) | (reflected_black >> 56)] << 56;
    */

    // Other row/column stability 
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
#endif

// For debugging and convenience
ostream & operator<<(ostream & os, const Board &b) {
    os << "+--------+" << endl;
    for (int i = 0; i < 8; i++) {
        os << "|";
        for (int j = 0; j < 8; j++) {
            int k = 8 * i + j;
            if (b.taken & BIT(k)) {
                if (b.black & BIT(k)) {
                    os << "X";
                }
                else {
                    os << "O";
                }
            }
            else {
                os << " ";
            }
        }
        os << "|" << endl;
    }
    os << "+--------+" << endl;

    return os;
}
