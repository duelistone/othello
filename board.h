#ifndef __BOARD_H__
#define __BOARD_H__

#include <cstdint>
#include <vector>
#include <climits>
#include <unordered_map>
#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cassert>
#include <cmath>
#include <atomic>
#include <bitset>
#include <boost/functional/hash.hpp>
#include "common.h"
#include "timer.h"
#include "defs.h"

enum Pattern {
    ROW1, ROW2, ROW3, ROW4, ROW5, ROW6,
    COL1, COL2, COL3, COL4, COL5, COL6,
    NW_DIAGONAL_3, NE_DIAGONAL_3, SW_DIAGONAL_3, SE_DIAGONAL_3,
    NW_DIAGONAL_4, NE_DIAGONAL_4, SW_DIAGONAL_4, SE_DIAGONAL_4,
    NW_DIAGONAL_5, NE_DIAGONAL_5, SW_DIAGONAL_5, SE_DIAGONAL_5,
    NW_DIAGONAL_6, NE_DIAGONAL_6, SW_DIAGONAL_6, SE_DIAGONAL_6,
    NW_DIAGONAL_7, NE_DIAGONAL_7, SW_DIAGONAL_7, SE_DIAGONAL_7,
    NW_DIAGONAL_8, SE_DIAGONAL_8
};

constexpr Pattern normal_patterns[NUM_NORMAL_PATTERNS] = {
    ROW1, ROW2, ROW3, ROW4, ROW5, ROW6,
    COL1, COL2, COL3, COL4, COL5, COL6,
    NE_DIAGONAL_3, SW_DIAGONAL_3,
    NE_DIAGONAL_4, SW_DIAGONAL_4,
    NE_DIAGONAL_5, SW_DIAGONAL_5,
    NE_DIAGONAL_6, SW_DIAGONAL_6,
    NE_DIAGONAL_7, SW_DIAGONAL_7,
};

constexpr Pattern special_patterns[NUM_SPECIAL_PATTERNS] = {
    NW_DIAGONAL_3, SE_DIAGONAL_3,
    NW_DIAGONAL_4, SE_DIAGONAL_4,
    NW_DIAGONAL_5, SE_DIAGONAL_5,
    NW_DIAGONAL_6, SE_DIAGONAL_6,
    NW_DIAGONAL_7, SE_DIAGONAL_7,
    NW_DIAGONAL_8
};

constexpr uint64_t pattern_mask[NUM_PATTERNS] = {
    ROW_MASK(1), ROW_MASK(2), ROW_MASK(3), ROW_MASK(4), ROW_MASK(5), ROW_MASK(6),
    COL_MASK(1), COL_MASK(2), COL_MASK(3), COL_MASK(4), COL_MASK(5), COL_MASK(6),
    NW_DIAGONAL_3_MASK, NE_DIAGONAL_3_MASK, SW_DIAGONAL_3_MASK, SE_DIAGONAL_3_MASK,
    NW_DIAGONAL_4_MASK, NE_DIAGONAL_4_MASK, SW_DIAGONAL_4_MASK, SE_DIAGONAL_4_MASK,
    NW_DIAGONAL_5_MASK, NE_DIAGONAL_5_MASK, SW_DIAGONAL_5_MASK, SE_DIAGONAL_5_MASK,
    NW_DIAGONAL_6_MASK, NE_DIAGONAL_6_MASK, SW_DIAGONAL_6_MASK, SE_DIAGONAL_6_MASK,
    NW_DIAGONAL_7_MASK, NE_DIAGONAL_7_MASK, SW_DIAGONAL_7_MASK, SE_DIAGONAL_7_MASK,
    NW_DIAGONAL_8_MASK, NE_DIAGONAL_8_MASK
};

constexpr uint64_t pattern_magic_number[NUM_PATTERNS] = {
    RBIT(8), RBIT(16), RBIT(24), RBIT(32), RBIT(40), RBIT(48),
    MAGIC_COL(1), MAGIC_COL(2), MAGIC_COL(3), MAGIC_COL(4), MAGIC_COL(5), MAGIC_COL(6),
    MAGIC_NW_DIAGONAL_3, MAGIC_NE_DIAGONAL_3, MAGIC_SW_DIAGONAL_3, MAGIC_SE_DIAGONAL_3,
    MAGIC_NW_DIAGONAL_4, MAGIC_NE_DIAGONAL_4, MAGIC_SW_DIAGONAL_4, MAGIC_SE_DIAGONAL_4,
    MAGIC_NW_DIAGONAL_5, MAGIC_NE_DIAGONAL_5, MAGIC_SW_DIAGONAL_5, MAGIC_SE_DIAGONAL_5,
    MAGIC_NW_DIAGONAL_6, MAGIC_NE_DIAGONAL_6, MAGIC_SW_DIAGONAL_6, MAGIC_SE_DIAGONAL_6,
    MAGIC_NW_DIAGONAL_7, MAGIC_NE_DIAGONAL_7, MAGIC_SW_DIAGONAL_7, MAGIC_SE_DIAGONAL_7,
    MAGIC_NW_DIAGONAL_8, MAGIC_NE_DIAGONAL_8
};

constexpr uint64_t pattern_size[NUM_PATTERNS] = {
    8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8,
    3, 3, 3, 3,
    4, 4, 4, 4,
    5, 5, 5, 5,
    6, 6, 6, 6,
    7, 7, 7, 7,
    8, 8
};

constexpr uint8_t reverse_byte[256] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};

using namespace std;

extern "C" int other_side(int);

extern int **EDGE_VALUES;
extern uint8_t** STABLE_DISCS;
extern uint8_t** PSEUDOSTABLE_DISCS;
extern uint8_t** ALL_STABLE_DISCS;
extern uint64_t** BYTE_TO_COL;
extern uint64_t** BYTE_TO_PATTERN; // Pattern, byte
extern uint64_t SINGLE_BIT[64];
extern size_t random_numbers[130];

uint64_t reflect(uint64_t);

class Board {
   
private:
    
      
public:
    uint64_t taken;
	uint64_t black;
	size_t zobrist_hash;
    
    Board();
    Board(uint64_t, uint64_t);
    Board(uint64_t t, uint64_t b, size_t z);
    
    uint64_t findLegalMoves(const Side &side) const;
    uint64_t findLegalMovesBlack() const;
    uint64_t findLegalMovesWhite() const;
    int pos_evaluate() const;
     
    Board doMoveOnNewBoard(const int &index, const Side &side) const;
    Board doMoveOnNewBoardWZH(const int &index, const Side &side) const {
        if (side) return doMoveOnNewBoardBlackWZH(index);
        return doMoveOnNewBoardWhiteWZH(index);
    }
    Board doMoveOnNewBoardBlack(const int &index) const;
    Board doMoveOnNewBoardBlackWZH(const int &index) const;
	Board doMoveOnNewBoardWhite(const int &index) const;
	Board doMoveOnNewBoardWhiteWZH(const int &index) const;
    uint64_t stable_discs() const;
    
    size_t make_zobrist_hash() const;

    void print_eval_stats() const;
    uint64_t stable_discs_log() const;
};

template <unsigned int size> class RowBoard {
public:
    uint8_t taken;
    uint8_t black;
    RowBoard() : taken(0), black(0) {}
    RowBoard(uint8_t taken, uint8_t black) : taken(taken), black(black) {}
    RowBoard(uint16_t data) : taken(data >> 8), black(data) {}
    RowBoard doMoveOnNewBoardBlack(uint8_t bit) const {
        RowBoard rb(*this);
        uint8_t white = ~black & taken;
        uint8_t n = bit;
        uint8_t newblack = black;
        
        // LEFT
        n |= white & (n << 1);
        n |= white & (n << 1);
        n |= white & (n << 1);
        n |= white & (n << 1);
        n |= white & (n << 1);
        n |= white & (n << 1);
        if (black & (n << 1)) newblack |= n;
        
        n = bit;
        // RIGHT
        n |= white & (n >> 1);
        n |= white & (n >> 1);
        n |= white & (n >> 1);
        n |= white & (n >> 1);
        n |= white & (n >> 1);
        n |= white & (n >> 1);
        if (black & (n >> 1)) newblack |= n;

        rb.black = newblack;
        rb.taken |= bit;
        rb.black |= bit;
        return rb;
    }
    RowBoard doMoveOnNewBoardWhite(uint8_t bit) const {
        RowBoard rb(*this);
        uint8_t white = ~black & taken;
        uint8_t n = bit;
        uint8_t newwhite = white;
        
        // LEFT
        n |= black & (n << 1);
        n |= black & (n << 1);
        n |= black & (n << 1);
        n |= black & (n << 1);
        n |= black & (n << 1);
        n |= black & (n << 1);
        if (white & (n << 1)) newwhite |= n;
        
        n = bit;
        // RIGHT
        n |= black & (n >> 1);
        n |= black & (n >> 1);
        n |= black & (n >> 1);
        n |= black & (n >> 1);
        n |= black & (n >> 1);
        n |= black & (n >> 1);
        if (white & (n >> 1)) newwhite |= n;
        
        rb.black = taken & ~newwhite;
        rb.taken |= bit;
        return rb;
    }
    uint8_t stable() const {return STABLE_DISCS[size - 1][((uint16_t) taken << 8 | black)];}
    uint8_t pseudostable() const {return PSEUDOSTABLE_DISCS[size - 1][((uint16_t) taken << 8 | black)];}
    uint8_t all_stable() const {return stable() | pseudostable();}
    void set_stable() const {
        uint8_t stable = taken;
        uint8_t empty = ~taken;
        while (empty && stable) {
            uint8_t bit = 1 << (31 - __builtin_clz(empty));
            empty ^= bit;
            RowBoard rb = doMoveOnNewBoardBlack(bit);
            stable &= ~(rb.black ^ black) & rb.all_stable();
            rb = doMoveOnNewBoardWhite(bit);
            stable &= ~(rb.black ^ black) & rb.all_stable();
        }
        uint8_t really_stable = 0;
        uint8_t bit = 1 << (size - 1);
        // Left stable chain?
        while (bit & black) {
            really_stable |= bit;
            stable &= ~bit;
            bit >>= 1;
        }
        bit = 1 << (size - 1);
        while (bit & taken & ~black) {
            really_stable |= bit;
            stable &= ~bit;
            bit >>= 1;
        }
        // Right chain?
        bit = 1;
        while (bit & black) {
            really_stable |= bit;
            stable &= ~bit;
            bit <<= 1;
        }
        bit = 1;
        while (bit & taken & ~black) {
            really_stable |= bit;
            stable &= ~bit;
            bit <<= 1;
        }
        PSEUDOSTABLE_DISCS[size - 1][((uint16_t) taken << 8) | black] = stable;
        STABLE_DISCS[size - 1][((uint16_t) taken << 8) | black] = really_stable;
        ALL_STABLE_DISCS[size - 1][((uint16_t) taken << 8) | black] = stable | really_stable;
    }
};

class BoardWithSide {
public:
	uint64_t taken;
	uint64_t black;
    uint64_t hash_value;
	Side side;
	
    BoardWithSide() {}
	BoardWithSide(uint64_t t, uint64_t b, size_t zh, Side s) : taken(t), black(b), hash_value(zh), side(s) {}
	BoardWithSide(const BoardWithSide& bws) : taken(bws.taken), black(bws.black), side(bws.side) {}
};

bool operator==(const BoardWithSide&, const BoardWithSide&);

ostream & operator<<(ostream & os, const Board &b);

#endif

