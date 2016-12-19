#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.h"
#include "board.h"
#include <bitset>
#include <unistd.h>
#include <climits>
#include <ctime>
#include <chrono>
#include <cassert>
#include <functional>
#include <thread>
#include <unordered_map>
#include <map>
#include <atomic>
#include <mutex>
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

extern BoardHash tt;
extern EndgameBoardHash endgameTT;
extern int *EDGE_VALUES;
extern uint64_t ** BYTE_TO_COL;

// To avoid um getting too big
#define STOP_SAVING_THRESHOLD 50

using namespace std;


class Player {
private:
	Side side;

public:
    Player(Side);
    ~Player();
    
    Board currBoard;
    
    Move *doMove(Move *opponentsMove, int msLeft);
};


int alphabeta(const Board &b, const int &depth, const Side &s, int alpha = INT_MIN, int beta = INT_MAX, const int &depth2 = 0, bool prevPass = false);

#endif
