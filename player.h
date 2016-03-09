#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.h"
#include "board2.h"

#define MAX(x, y) ((x) >= (y) ? x : y)
#define MIN(x, y) ((x) >= (y) ? x : y)

using namespace std;



class Player {
private:
	Side side;

public:
    Player(Side);
    ~Player();
    
    Board currBoard;
    
    Move *doMove(Move *opponentsMove, int msLeft);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
};

#endif
