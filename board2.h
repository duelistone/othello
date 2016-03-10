#ifndef __BOARD2_H__
#define __BOARD2_H__

#include <cstdint>
#include <vector>
#include <climits>
#include <unordered_map>
#include "common.h"

#define BIT(x) ((uint64_t) 1 << (63 - (x)))
#define OCCUPY_BLACK(x, t, b) {(t) |= BIT(x); (b) |= BIT(x);}
#define OCCUPY_WHITE(x, t, b) {(t) |= BIT(x);}
#define TO_INDEX(x, y) ((x) + 8 * (y))
#define OCCUPIED(x, y, t) (((t) & BIT(TO_INDEX(x, y))) != 0)
#define OCCUPIED_SIDE(side, x, y, t, b) ( OCCUPIED(x, y, t) && ( ( (b) & BIT(TO_INDEX(x, y)) ) == ( (side) == BLACK ? BIT(TO_INDEX(x, y)) : 0 ) ) )
#define ON_BOARD(x, y) (0 <= (x) && (x) < 8 && 0 <= (y) && (y) < 8)
#define PLACE_DISC(side, x, y, t, b) {(t) |= BIT(TO_INDEX(x, y)); if ((side) == BLACK) (b) |= BIT(TO_INDEX(x, y)); else (b) &= ~BIT(TO_INDEX(x, y));}
#define FROM_INDEX_X(index) ((index) % 8)
#define FROM_INDEX_Y(index) ((index) / 8)
#define OTHER_SIDE(side) (side == BLACK ? WHITE : BLACK)
#define CORNER_TL (BIT(0))
#define CORNER_TR (BIT(7))
#define CORNER_BL (BIT(56))
#define CORNER_BR (BIT(63))
#define CORNERS (BIT(0) | BIT(7) | BIT(56) | BIT(63))
#define DIAGONAL_FROM_CORNERS (BIT(TO_INDEX(1, 1)) | BIT(TO_INDEX(1, 6)) | BIT(TO_INDEX(6, 1)) | BIT(TO_INDEX(6, 6)))
#define NEXT_TO_CORNERS (BIT(1) | BIT(6) | BIT(62) | BIT(57) | BIT(8) | BIT(48) | BIT(15) | BIT(55))
#define NEXT_TO_CORNER_TL (BIT(1) | BIT(8) | BIT(9))
#define NEXT_TO_CORNER_TR (BIT(6) | BIT(15) | BIT(14))
#define NEXT_TO_CORNER_BL (BIT(48) | BIT(49) | BIT(57))
#define NEXT_TO_CORNER_BR (BIT(62) | BIT(55) | BIT(54))

using namespace std;

class Board {
   
private:
    
      
public:
    uint64_t taken;
	uint64_t black;
    bool legalMovesComputed;
    uint64_t legalMoves;
    int evaluation;
    
    Board();
    Board(uint64_t, uint64_t);
    
    uint64_t findLegalMoves(Side side);
    int evaluate();
    int evaluateTest();
     
    bool isDone();
    bool hasMoves(Side side);
    bool checkMove(int x, int y, Side side);
    void doMove(int x, int y, Side side);
    Board doMoveOnNewBoard(int x, int y, Side side);
    int count(Side side);
    int countBlack();
    int countWhite();

    void setBoard(char data[]);
};

class BoardWithSide {
public:
	uint64_t taken;
	uint64_t black;
	Side side;
	
	BoardWithSide(uint64_t t, uint64_t b, Side s) : taken(t), black(b), side(s) {}
	BoardWithSide(const BoardWithSide& bws) : taken(bws.taken), black(bws.black), side(bws.side) {}
};

bool operator==(const BoardWithSide&, const BoardWithSide&);

namespace std {	
	template<> struct hash<BoardWithSide> {
		size_t operator()(const BoardWithSide & b) const {
			return hash<unsigned long long>()(b.black);
		}
	};
}

// unordered_map< pair<Board, Side>, int> hashTable;
// Need to make hash function
// Hash function idea: (taken << 2) ^ (black << 1) ^ side
// Or, have unordered_map< uint64_t, unordered_map< uint64_t, unordered_map<Side, int> > >

#endif

