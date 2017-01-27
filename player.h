#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.h"
#include "board.h"
#include "search.h"
#include <bitset>
#include <unistd.h>
#include <climits>
#include <ctime>
#include <chrono>
#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "timer.h"
#include "tt.h"

using namespace std;

extern double minutesForMove;
extern fstream fil;

extern uint64_t ** BYTE_TO_COL;

class Player {
private:
	Side side;
    int last_depth;

public:
    Player(Side);
    ~Player();
    
    Board currBoard;
    
    pair<int, int> main_minimax_aw(Board b, Side s, int guess = -1);

    Move *doMove(Move *opponentsMove, int msLeft);
};

#endif
