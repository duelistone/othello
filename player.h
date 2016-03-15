#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.h"
#include "board2.h"
#include <bitset>
#include <unistd.h>
#include <climits>
#include <ctime>
#include <cassert>
#include <functional>
#include <thread>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>

#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define MIN(x, y) ((x) <= (y) ? (x) : (y))

#define PERCENTILE (1.5)
#define SIGMA 3
#define PROB_CUT_DEPTH1 4
#define PROB_CUT_DEPTH2 8


// Global counter for how many final nodes endgameMinimax is searching
extern long long globalEndgameNodeCount;
extern std::atomic_bool abortEndgameMinimax;
extern double minutesForMove;
extern std::mutex um_lock;
extern std::mutex um2_lock;

// For memoization
// Question: Is it worth it?
// Can run through the values it can store pretty quickly
extern std::unordered_map< BoardWithSide, int > *um;
extern std::unordered_map< BoardWithSide, int > *um2;

extern fstream fil;

extern unordered_map<int, double> sigma;
extern unordered_map<int, double> constant_terms;
extern unordered_map<int, double> coefficients;

extern unordered_map<uint16_t, int> EDGE_VALUES;

// To avoid um getting too big
#define MAX_HASH_SIZE 3000000
#define STOP_SAVING_THRESHOLD 56
// At this point it should be quick to compute to the end

// Should correspond to at most a minute of computation
#define DEFAULT_MAX_NODES (30000000)

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
