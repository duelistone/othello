#ifndef __SEARCH_H__
#define __SEARCH_H__
#include "board.h"
#include <iostream>
#include "common.h"
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
#include "timer.h"
#include "tt.h"

using namespace std;

extern BoardHash tt;
extern BoardHash2 tt2;
extern EndgameBoardHash endgameTT;

// Alphabeta
pair<int, vector<uint8_t> > alphabeta(Board b, const int depth, const Side s, int alpha = INT_MIN, int beta = INT_MAX, const int depth2 = 0, const bool prevPass = false);

// Principal variation search
int pvs(Board b, const int depth, const Side s, int alpha = INT_MIN, int beta = INT_MAX, const int depth2 = 0, const bool prevPass = false);
pair<int, int> pvsMove(Board b, const int depth, const Side s, int alpha = INT_MIN, int beta = INT_MAX, const int depth2 = 0, const bool prevPass = false);

int pvsBlack(Board b, const int depth, int alpha = INT_MIN, const int beta = INT_MAX, const int depth2 = 0, const bool prevPass = false);
int pvsWhite(Board b, const int depth, const int alpha = INT_MIN, int beta = INT_MAX, const int depth2 = 0, const bool prevPass = false);
pair<int, int> pvsBlackMove(Board b, const int depth, int alpha = INT_MIN, const int beta = INT_MAX, const int depth2 = 0, const bool prevPass = false);
pair<int, int> pvsWhiteMove(Board b, const int depth, const int alpha = INT_MIN, int beta = INT_MAX, const int depth2 = 0, const bool prevPass = false);

// Get principal variation using hash
pair< vector<uint8_t>, Board> get_pv(Board b, Side side);

// Endgame
int endgame_alphabeta(Board b, const Side s, int alpha = -1, int beta = 1);
pair<int, int> endgame_minimax(Board b, const Side s, int guess = -1);
#endif
