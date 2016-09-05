#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "common.h"
#include "board2.h"
#include <iostream>
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

// Assembly functions
extern "C" int min(int x, int y);
extern "C" int max(int x, int y);

#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define MIN(x, y) ((x) <= (y) ? (x) : (y))

// For memoization
extern std::unordered_map< BoardWithSide, int > *um2;

// For writing edge configurations
extern fstream fil;
extern int *EDGE_VALUES;
extern int *EDGE_VOLATILITY;

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

struct Eval {
	Board pvBoard;
	int e;
	Eval(const Board &b, const int &ev) {
		pvBoard = b;
		e = ev;
	}
	bool operator==(const Eval &b) const {
		return pvBoard == b.pvBoard && e == b.e;
	}
};

int endgame_alphabeta(const Board &b, const Side &s, int alpha = INT_MIN, int beta = INT_MAX);
int deep_endgame_alphabeta(const Board &b, const Side &s, int alpha = INT_MIN, int beta = INT_MAX);
Eval pvsBlack(const Board &b, const int &depth, int alpha = INT_MIN, const int &beta = INT_MAX, const int &depth2 = 0, const bool &prevPass = false);
Eval pvsWhite(const Board &b, const int &depth, const int &alpha = INT_MIN, int beta = INT_MAX, const int &depth2 = 0, const bool &prevPass = false);

#endif
