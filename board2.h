#ifndef __BOARD2_H__
#define __BOARD2_H__

#include <cstdint>
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

using namespace std;

class Board {
   
private:
    
      
public:
	uint64_t black;
    uint64_t taken;
    Board();
    Board(uint64_t, uint64_t);
    
    uint64_t legalMoves;
    bool legalMovesComputed;
    uint64_t findLegalMoves(Side side);
     
    bool isDone();
    bool hasMoves(Side side);
    bool checkMove(int x, int y, Side side);
    void doMove(int x, int y, Side side);
    int count(Side side);
    int countBlack();
    int countWhite();

    void setBoard(char data[]);
};

#endif

