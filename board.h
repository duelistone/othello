#ifndef __BOARD2_H__
#define __BOARD2_H__

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

#define BIT(x) ((uint64_t) 1 << (63 - (x)))
#define BIT_SAFE(x) ((x) < 64 ? (uint64_t) 1 << (63 - (x)) : 0)
#define OCCUPY_BLACK(x, t, b) {(t) |= BIT(x); (b) |= BIT(x);}
#define OCCUPY_WHITE(x, t, b) {(t) |= BIT(x);}
#define TO_INDEX(x, y) ((x) + 8 * (y))
#define OCCUPIED(x, y, t) (((t) & BIT(TO_INDEX(x, y))) != 0)
#define OCCUPIED_SIDE(side, x, y, t, b) ( OCCUPIED(x, y, t) && ( ( (b) & BIT(TO_INDEX(x, y)) ) == ( ((side) == BLACK) ? BIT(TO_INDEX(x, y)) : 0 ) ) )
#define OCCUPIED_BI(t) (((t) & bi) != 0)
#define ON_BOARD(x, y) (0 <= (x) && (x) < 8 && 0 <= (y) && (y) < 8)
#define PLACE_DISC(side, x, y, t, b) {(t) |= BIT(TO_INDEX(x, y)); if ((side) == BLACK) (b) |= BIT(TO_INDEX(x, y)); else (b) &= ~BIT(TO_INDEX(x, y));}
#define FROM_INDEX_X(index) ((index) % 8)
#define FROM_INDEX_Y(index) ((index) / 8)
#define OTHER_SIDE(side) ((int) (!(side)))
#define CORNER_TL (BIT(0))
#define CORNER_TR (BIT(7))
#define CORNER_BL (BIT(56))
#define CORNER_BR (BIT(63))
#define CORNERS (BIT(0) | BIT(7) | BIT(56) | BIT(63))
#define DIAGONAL_FROM_CORNERS (BIT(TO_INDEX(1, 1)) | BIT(TO_INDEX(1, 6)) | BIT(TO_INDEX(6, 1)) | BIT(TO_INDEX(6, 6)))
#define NEXT_TO_CORNERS (BIT(1) | BIT(6) | BIT(62) | BIT(57) | BIT(8) | BIT(48) | BIT(15) | BIT(55))
#define X_SQUARES (BIT(9) | BIT(14) | BIT(49) | BIT(54))
#define NEXT_TO_CORNER_TL (BIT(1) | BIT(8) | BIT(9))
#define NEXT_TO_CORNER_TR (BIT(6) | BIT(15) | BIT(14))
#define NEXT_TO_CORNER_BL (BIT(48) | BIT(49) | BIT(57))
#define NEXT_TO_CORNER_BR (BIT(62) | BIT(55) | BIT(54))
#define INNER_EDGE_TOP 0b0111111000000000000000000000000000000000000000000000000000000000ull
#define INNER_EDGE_BOTTOM 0b0000000000000000000000000000000000000000000000000000000001111110ull
#define INNER_EDGE_LEFT 0b0000000010000000100000001000000010000000100000001000000000000000ull
#define INNER_EDGE_RIGHT 0b0000000000000001000000010000000100000001000000010000000100000000ull
#define INNER_EDGES (INNER_EDGE_TOP | INNER_EDGE_BOTTOM | INNER_EDGE_LEFT | INNER_EDGE_RIGHT)
#define EDGE_TOP 0b1111111100000000000000000000000000000000000000000000000000000000ull
#define EDGE_BOTTOM 0b0000000000000000000000000000000000000000000000000000000011111111ull
#define EDGE_LEFT 0b1000000010000000100000001000000010000000100000001000000010000000ull
#define EDGE_LEFT2 0b0100000001000000010000000100000001000000010000000100000001000000ull
#define EDGE_LEFT3 0b0010000000100000001000000010000000100000001000000010000000100000ull
#define EDGE_RIGHT 0b0000000100000001000000010000000100000001000000010000000100000001ull
#define EDGE_RIGHT2 0b0000000010000000100000001000000010000000100000001000000010000001ull
#define EDGE_RIGHT3 0b0000000001000000010000000100000001000000010000000100000001000001ull
#define EDGES 0b1111111110000001100000011000000110000001100000011000000111111111ull
#define SAFE_SQUARES (~X_SQUARES)

#define COL_MASK(x) (EDGE_LEFT >> (x))
#define ROW_MASK(x) (EDGE_TOP >> (8 * (x)))
#define COL(bits, x) ((((bits) & COL_MASK(x)) * MAGIC_COL(x)) >> 56)

#define EDGE_BIT(x) ((uint16_t) 1 << (15 - (x)))
#define JUST_EDGE_BIT(x) ((uint8_t) 1 << (7 - (x)))

#define MAGIC_COL_NUMBER 0x0002040810204081ull
#define MAGIC_COL(x) (MAGIC_COL_NUMBER << (x))

#define DOWN_FILTER 0x00FFFFFFFFFFFFFFULL
#define RIGHT_FILTER 0x7F7F7F7F7F7F7F7FULL
#define DOWN_RIGHT_FILTER 0x007F7F7F7F7F7F7FULL
#define DOWN_LEFT_FILTER 0x00FEFEFEFEFEFEFEULL
#define UP_FILTER 0xFFFFFFFFFFFFFF00ULL
#define LEFT_FILTER 0xFEFEFEFEFEFEFEFEULL
#define UP_RIGHT_FILTER 0x7F7F7F7F7F7F7F00ULL
#define UP_LEFT_FILTER 0xFEFEFEFEFEFEFE00ULL


#define MOBILITY_WEIGHT 60
#define MOBILITY_BOOST 15
#define MOBILITY_CAP 30
#define PM_CAP 10

using namespace std;

extern "C" int other_side(int);

extern int *EDGE_VALUES;
extern pair<uint64_t, uint64_t> **STABLE_DISCS;
extern pair<uint64_t, uint64_t> **PSEUDOSTABLE_DISCS;
extern uint64_t** BYTE_TO_COL;
extern uint64_t SINGLE_BIT[64];
extern size_t random_numbers[130];

um_data make_hash_data(int, int, int, int);
uint64_t doMove(uint64_t taken, uint64_t black, Side side, int index);

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
    uint64_t findLegalMoves2(Side side) const;
    //~ uint64_t findLegalMoves3(Side side) const;
    //~ uint64_t onlyFindLegalMoves(Side side) const;
    int evaluate(int, int, int) const;
    //~ int evaluate_mobility() const;
    int pos_evaluate() const;
    int evaluateTest() const;
     
    //~ bool isDone() const;
    //~ bool hasMoves(Side side) const;
    //bool hasLegalMoves(Side side);
    //~ bool checkMove(const int &index, Side side) const;
    void doMove(int x, int y, Side side);
    //~ Board doMoveOnNewBoard(int x, int y, Side side);
    Board doMoveOnNewBoard(const int &index, const Side &side) const;
    Board doMoveOnNewBoardBlack(const int &index) const;
    Board doMoveOnNewBoardBlackWZH(const int &index) const;
	Board doMoveOnNewBoardWhite(const int &index) const;
	Board doMoveOnNewBoardWhiteWZH(const int &index) const;
    Board doMoveOnNewBoard2(int x, int y, Side side) const;
    Board doMoveOnNewBoard3(int x, int y, Side side) const;
    int count (Side side) const;
    int countBlack() const;
    int countWhite() const;
    pair<uint64_t, uint64_t> stable_discs() const;
    
    size_t make_zobrist_hash(const Side &s = BLACK) const;

    void setBoard(char data[]);
};
#define BSP(x) (bitset<8>((unsigned int) (x)))
template <unsigned int size> class RowBoard {
public:
    uint8_t taken;
    uint8_t black;
    RowBoard() : taken(0), black(0) {}
    RowBoard(uint8_t taken, uint8_t black) : taken(taken), black(black) {}
    RowBoard(uint16_t data) : taken(data >> 8), black(data) {}
    RowBoard doMoveOnNewBoardBlack(uint8_t bit) const {
        RowBoard rb(*this);
        uint8_t original_black = black;
        uint8_t leftBit = bit << 1;
        while (leftBit & rb.taken & ~rb.black) {
            rb.black |= leftBit;
            leftBit <<= 1;
        }
        if (leftBit == 0) rb.black = original_black;
        original_black = rb.black;
        uint8_t rightBit = bit >> 1;
        while (rightBit & rb.taken & ~rb.black) {
            rb.black |= rightBit;
            rightBit >>= 1;
        }
        if (rightBit == 0) rb.black = original_black;
        rb.taken |= bit;
        rb.black |= bit;
        return rb;
    }
    RowBoard doMoveOnNewBoardWhite(uint8_t bit) const {
        RowBoard rb(*this);
        uint8_t original_black = black;
        uint8_t leftBit = bit << 1;
        while (leftBit & rb.black) {
            rb.black &= ~leftBit;
            leftBit <<= 1;
        }
        if (leftBit == 0) rb.black = original_black;
        original_black = rb.black;
        uint8_t rightBit = bit >> 1;
        while (rightBit & rb.black) {
            rb.black &= ~rightBit;
            rightBit >>= 1;
        }
        if (rightBit == 0) rb.black = original_black;
        rb.taken |= bit;
        return rb;
    }
    pair<uint8_t, uint8_t> stable() const {return STABLE_DISCS[size - 1][((uint16_t) taken << 8 | black)];}
    pair<uint8_t, uint8_t> pseudostable() const {return PSEUDOSTABLE_DISCS[size - 1][((uint16_t) taken << 8 | black)];}
    pair<uint8_t, uint8_t> all_stable() const {
        pair<uint8_t, uint8_t> s = stable();
        pair<uint8_t, uint8_t> ps = pseudostable();
        return make_pair(s.first | ps.first, s.second | ps.second);
    }
    void set_stable() const {
        uint8_t black_stable = black;
        uint8_t white_stable = taken & ~black;
        uint8_t empty = ~taken;
        int counter = 0;
        while (empty && (black_stable || white_stable)) {
            uint8_t bit = 1 << (31 - __builtin_clz(empty));
            empty ^= bit;
            RowBoard rb = doMoveOnNewBoardBlack(bit);
            black_stable &= rb.all_stable().first;
            white_stable &= rb.all_stable().second;
            rb = doMoveOnNewBoardWhite(bit);
            black_stable &= rb.all_stable().first;
            white_stable &= rb.all_stable().second;
            counter++;
        }
        PSEUDOSTABLE_DISCS[size - 1][((uint16_t) taken << 8) | black] = make_pair(black_stable, white_stable);
    }
};

class BoardWithSide {
public:
	uint64_t taken;
	uint64_t black;
	Side side;
	
	BoardWithSide(uint64_t t, uint64_t b, Side s) : taken(t), black(b), side(s) {}
	BoardWithSide(const BoardWithSide& bws) : taken(bws.taken), black(bws.black), side(bws.side) {}
	
	inline size_t hash_value() const {
		uint64_t blackCopy = black;
		uint64_t whiteCopy = taken & ~black;
		size_t result = side ? random_numbers[128] : random_numbers[129];
		
		while (blackCopy) {
			int index = __builtin_clzl(blackCopy);
			blackCopy ^= BIT(index);
			result ^= random_numbers[index];
		}
		while (whiteCopy) {
			int index = __builtin_clzl(whiteCopy);
			whiteCopy ^= BIT(index);
			result ^= random_numbers[index + 64];
		}
		return result;
		
		// Independent of side?!
		//~ return 0;
		/*
		size_t result = __builtin_popcountll(taken) - 1;
		size_t seed = 0;
		boost::hash_combine(seed, taken);
		boost::hash_combine(seed, black);
		result |= seed << 6;
		return result;
		*/
		/*
		size_t seed = 0;
		boost::hash_combine(seed, taken);
		boost::hash_combine(seed, black);
		return seed;
		*/
	}
	int count() const {return __builtin_popcountll(taken);}
};

bool operator==(const BoardWithSide&, const BoardWithSide&);

namespace std {	
	template<> struct hash<BoardWithSide> {
		size_t operator()(const BoardWithSide & b) const {
			size_t seed = 0;
			boost::hash_combine(seed, b.taken);
			boost::hash_combine(seed, b.black);
			return hash<unsigned long long>()(seed);
		}
	};
}

// unordered_map< pair<Board, Side>, int> hashTable;
// Need to make hash function
// Hash function idea: (taken << 2) ^ (black << 1) ^ side
// Or, have unordered_map< uint64_t, unordered_map< uint64_t, unordered_map<Side, int> > >

#endif

