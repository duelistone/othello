#ifndef __BOARD2_H__
#define __BOARD2_H__

#include <cstdint>
#include <vector>
#include <climits>
#include <unordered_map>
#include <thread>
#include <iostream>
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

#define EDGE_BIT(x) ((uint16_t) 1 << (15 - (x)))

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
extern uint64_t SINGLE_BIT[64];
extern size_t random_numbers[130];

uint64_t doMove(uint64_t taken, uint64_t black, Side side, int index);

class Board {
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
    int evaluate(int, int, int) const;
    int pos_evaluate() const;
    int evaluateTest() const;
     
    void doMove(int x, int y, Side side);
    Board doMoveOnNewBoard(const int &index, const Side &side) const;
    Board doMoveOnNewBoardBlack(const int &index) const;
    Board doMoveOnNewBoardBlackWZH(const int &index) const;
	Board doMoveOnNewBoardWhite(const int &index) const;
	Board doMoveOnNewBoardWhiteWZH(const int &index) const;
    int count (Side side) const;
    int countBlack() const;
    int countWhite() const;
    
    size_t make_zobrist_hash(const Side &s = BLACK) const;

    void setBoard(char data[]);
    
    bool operator==(const Board &b) const {return b.taken == taken && b.black == black;}
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
			boost::hash_combine(seed, b.side); // Should we remove this line?
			return hash<unsigned long long>()(seed);
		}
	};
}

// unordered_map< pair<Board, Side>, int> hashTable;
// Need to make hash function
// Hash function idea: (taken << 2) ^ (black << 1) ^ side
// Or, have unordered_map< uint64_t, unordered_map< uint64_t, unordered_map<Side, int> > >

#endif

