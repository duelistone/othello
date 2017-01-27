#include "defs.h"
#include "board.h"
#include "tt.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;
double *EDGE_VALUES;
size_t random_numbers[130];
uint64_t SINGLE_BIT[64];
uint8_t **STABLE_DISCS;
uint64_t **BYTE_TO_PATTERN;

pair<int, vector<uint8_t> > alphabeta(Board b, const int depth, const Side s, int alpha = INT_MIN, int beta = INT_MAX, const int depth2 = 0, const bool prevPass = false) {
    if (depth <= 0) return make_pair(b.pos_evaluate(), vector<uint8_t>());

    uint64_t legalMoves = b.findLegalMoves(s);
    
    // Special case
    if (!legalMoves) {
        if (prevPass) {
            int blacks = __builtin_popcountll(b.black);
            int whites = __builtin_popcountll(b.taken & ~b.black); 
            return make_pair((blacks > whites) ? INT_MAX : ((whites > blacks) ? INT_MIN : 0), vector<uint8_t>());
        }
        return alphabeta(b, depth - 1, !s, alpha, beta, depth2 + 1, true);
    }
    
    int besti = 64;
    vector<uint8_t> bestpv;
    if (s) {
        int v = INT_MIN;
        // uint8_t index = 64;
        // if (depth2 <= HASH_DEPTH) {
        //     index = tt[b.zobrist_hash];
        //     if (index < 64 && (BIT(index) & legalMoves)) {
        //         // The move is valid
        //         legalMoves ^= BIT(index);
        //         besti = index;
        //         v = alphabeta(b.doMoveOnNewBoardBlack(index), depth - 1, WHITE, alpha, beta, depth2 + 1);
        //         alpha = (v > alpha) ? v : alpha;
        //     }
        // }
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzll(legalMoves);
            legalMoves ^= BIT(index);
            pair< int, vector<uint8_t> > p = alphabeta(b.doMoveOnNewBoardBlackWZH(index), depth - 1, WHITE, alpha, beta, depth2 + 1);
            int val = p.first;
            bestpv = (val > v) ? p.second : bestpv;
            besti = (val > v) ? index : besti;
            v = (val > v) ? val : v;
            alpha = (v > alpha) ? v : alpha;
        }
        // if (depth2 <= HASH_DEPTH) tt[b.zobrist_hash] = besti;
        bestpv.push_back(besti);
        return make_pair(alpha, bestpv);
    }
    else {
        int v = INT_MAX;
        // uint8_t index = 64;
        // if (depth2 <= HASH_DEPTH) {
        //     index = tt[b.zobrist_hash];
        //     if (index < 64 && (BIT(index) & legalMoves)) {
        //         // The move is valid
        //         legalMoves ^= BIT(index);
        //         besti = index;
        //         v = alphabeta(b.doMoveOnNewBoardWhite(index), depth - 1, BLACK, alpha, beta, depth2 + 1);
        //         beta = (v < beta) ? v : beta;
        //     }
        // }
        while (alpha < beta && legalMoves) {    
            uint8_t index = __builtin_clzll(legalMoves);
            legalMoves ^= BIT(index);
            pair<int, vector<uint8_t> > p = alphabeta(b.doMoveOnNewBoardWhiteWZH(index), depth - 1, BLACK, alpha, beta, depth2 + 1);
            int val = p.first;
            besti = (val < v) ? index : besti;
            bestpv = (val < v) ? p.second : bestpv;
            v = (val < v) ? val : v;
            beta = (v < beta) ? v : beta;
        }
        // if (depth2 <= HASH_DEPTH) tt[b.zobrist_hash] = besti;
        bestpv.push_back(besti);
        return make_pair(beta, bestpv);
    }
}

int main(int argc, char **argv) {
    int AB_LEARN_DEPTH = 2;
    if (argc > 1) AB_LEARN_DEPTH = atoi(argv[1]);
    fstream fil("game_database", ios_base::in);
    // fstream nnFile("training_data", ios_base::out);

    double **edge_wins;
    int **edge_total;
    edge_wins = (double **) malloc(sizeof(double *) * 65);
    edge_total = (int **) malloc(sizeof(int *) * 65);
    for (int i = 0; i < 65; i++) {
        edge_wins[i] = (double *) malloc(sizeof(double) * (1 << 16));
        edge_total[i] = (int *) malloc(sizeof(int) * (1 << 16));
    }
    EDGE_VALUES = (double *) malloc(sizeof(double) * (1 << 16) * 65);
    for (int i = 0; i < (1 << 16) * 65; i++) EDGE_VALUES[i] = 0;
    
    for (int j = 0; j < 65; j++) {
        for (int i = 0; i < (1 << 16); i++) {
            edge_wins[j][i] = 0;
            edge_total[j][i] = 0;
        }
    }

    string line;
    while (getline(fil, line)) {
        Board b;
        Side s = BLACK;
        stringstream ss(line);
        int moves[60];
        for (int i = 0; i < 60; i++) {
            ss >> moves[i];
        }
        
        int score, result;
        ss >> score;
        if (score > 32) result = 1;
        else if (score < 32) result = -1;
        else result = 0;
        
        for (int i = 0; i < 60; i++) {
            // if (i == 38) {
            //     // 42 discs on board currently
            //     for (int j = 0; j < 64; j++) {
            //         if (BIT(j) & b.black) nnFile << 1.0 << ' ';
            //         else if (BIT(j) & b.taken & ~b.black) nnFile << 0.0 << ' ';
            //         else nnFile << 0.5 << ' ';
            //     }
            //     nnFile << score << endl;
            // }

            if (moves[i] == -9) break;
            if (BIT(moves[i]) & b.findLegalMoves(s)) {
                b = b.doMoveOnNewBoardWZH(moves[i], s);
                s = !s;
            }
            else if (BIT(moves[i]) & b.findLegalMoves(!s)) {
                b = b.doMoveOnNewBoardWZH(moves[i], !s);
            }
            else {
                break;
            }
            
            int tc = __builtin_popcountll(b.taken);
            if (tc < 40) continue;
            else if (tc > 40) break;

            // Extract edges and save scores
            edge_wins[tc][(uint16_t) ((COL(b.taken, 0) << 8) | COL(b.black, 0))] += result;
            edge_total[tc][(uint16_t) ((COL(b.taken, 0) << 8) | COL(b.black, 0))]++;
            edge_wins[tc][(uint16_t) ((COL(b.taken, 7) << 8) | COL(b.black, 7))] += result;
            edge_total[tc][(uint16_t) ((COL(b.taken, 7) << 8) | COL(b.black, 7))]++;
            edge_wins[tc][(uint16_t) ((b.taken << 8) | (uint8_t) b.black)] += result;
            edge_total[tc][(uint16_t) ((b.taken << 8) | (uint8_t) b.black)]++;
            edge_wins[tc][(uint16_t) (((b.taken >> 56) << 8) | (b.black >> 56))] += result;
            edge_total[tc][(uint16_t) (((b.taken >> 56) << 8) | (b.black >> 56))]++;

            edge_wins[tc][ (reverse_byte[(uint8_t) COL(b.taken, 0)] << 8) | reverse_byte[(uint8_t) COL(b.black, 0)]] += result;
            edge_total[tc][(reverse_byte[(uint8_t) COL(b.taken, 0)] << 8) | reverse_byte[(uint8_t) COL(b.black, 0)]]++;
            edge_wins[tc][ (reverse_byte[(uint8_t) COL(b.taken, 7)] << 8) | reverse_byte[(uint8_t) COL(b.black, 7)]] += result;
            edge_total[tc][(reverse_byte[(uint8_t) COL(b.taken, 7)] << 8) | reverse_byte[(uint8_t) COL(b.black, 7)]]++;
            edge_wins[tc][ (reverse_byte[(uint8_t) b.taken] << 8)         | reverse_byte[(uint8_t) b.black]        ] += result;
            edge_total[tc][(reverse_byte[(uint8_t) b.taken] << 8)         | reverse_byte[(uint8_t) b.black]        ]++;
            edge_wins[tc][ (reverse_byte[(uint8_t) (b.taken >> 56)] << 8) | reverse_byte[b.black >> 56]            ] += result;
            edge_total[tc][(reverse_byte[(uint8_t) (b.taken >> 56)] << 8) | reverse_byte[b.black >> 56]            ]++;

            uint64_t white = b.taken & ~b.black;

            edge_wins[tc][(uint16_t) ((COL(b.taken, 0) << 8) | COL(white, 0))] -= result;
            edge_total[tc][(uint16_t) ((COL(b.taken, 0) << 8) | COL(white, 0))]++;
            edge_wins[tc][(uint16_t) ((COL(b.taken, 7) << 8) | COL(white, 7))] -= result;
            edge_total[tc][(uint16_t) ((COL(b.taken, 7) << 8) | COL(white, 7))]++;
            edge_wins[tc][(uint16_t) ((b.taken << 8) | (uint8_t) white)] -= result;
            edge_total[tc][(uint16_t) ((b.taken << 8) | (uint8_t) white)]++;
            edge_wins[tc][(uint16_t) (((b.taken >> 56) << 8) | (white >> 56))] -= result;
            edge_total[tc][(uint16_t) (((b.taken >> 56) << 8) | (white >> 56))]++;

            edge_wins[tc][ (reverse_byte[(uint8_t) COL(b.taken, 0)] << 8) | reverse_byte[(uint8_t) COL(white, 0)]] -= result;
            edge_total[tc][(reverse_byte[(uint8_t) COL(b.taken, 0)] << 8) | reverse_byte[(uint8_t) COL(white, 0)]]++;
            edge_wins[tc][ (reverse_byte[(uint8_t) COL(b.taken, 7)] << 8) | reverse_byte[(uint8_t) COL(white, 7)]] -= result;
            edge_total[tc][(reverse_byte[(uint8_t) COL(b.taken, 7)] << 8) | reverse_byte[(uint8_t) COL(white, 7)]]++;
            edge_wins[tc][ (reverse_byte[(uint8_t) b.taken] << 8)         | reverse_byte[(uint8_t) white]        ] -= result;
            edge_total[tc][(reverse_byte[(uint8_t) b.taken] << 8)         | reverse_byte[(uint8_t) white]        ]++;
            edge_wins[tc][ (reverse_byte[(uint8_t) (b.taken >> 56)] << 8) | reverse_byte[white >> 56]            ] -= result;
            edge_total[tc][(reverse_byte[(uint8_t) (b.taken >> 56)] << 8) | reverse_byte[white >> 56]            ]++;
        }
    }

    fil.close();

    // Load EDGE_VALUES
    for (int i = 0; i < (1 << 16); i++) {
        double score = (edge_total[40][i] >= 16) ? (double) edge_wins[40][i] / edge_total[40][i] : 0;
        if (abs(score) < 0.01) score = 0;
        for (int j = 0; j < 65; j++) {
            double score2 = score * (80 - j) / 40;
            if (score2 > 1) score2 = 1;
            else if (score2 < -1) score2 = -1;
            EDGE_VALUES[(1 << 16) * j + i] = score2; // The default
        }
    }

    // Lower values
    for (int k = 39; k >= 4; k--) {
        cout << "Move " << k << endl;
        fstream fil("game_database", ios_base::in);
        string line;
        while (getline(fil, line)) {
            Side s = BLACK;
            stringstream ss(line);
            int moves[60];
            for (int i = 0; i < 60; i++) {
                ss >> moves[i];
            }

            Board b;
            for (int i = 0; i < 60; i++) {
                if (moves[i] == -9) break;
                if (BIT(moves[i]) & b.findLegalMoves(s)) {
                    b = b.doMoveOnNewBoardWZH(moves[i], s);
                    s = !s;
                }
                else if (BIT(moves[i]) & b.findLegalMoves(!s)) {
                    b = b.doMoveOnNewBoardWZH(moves[i], !s);
                }
                else {
                    break;
                }
                
                int tc = __builtin_popcountll(b.taken);
                if (tc == k) {
                    // Search
                    uint64_t lm = b.findLegalMoves(s);
                    if (lm == 0) break;
                    int depth = (40 - k <= AB_LEARN_DEPTH) ? 40 - k : AB_LEARN_DEPTH;
                    pair<int, vector<uint8_t> > p = alphabeta(b, depth, s);
                    reverse(p.second.begin(), p.second.end());
                    Board b2 = b;
                    assert(p.second.size() > 0);
                    for (vector<uint8_t>::iterator it = p.second.begin(); it != p.second.end(); it++) {
                        int move = *it;
                        if (move == 64) {
                            s = !s;
                            continue;
                        }
                        if (!(BIT(move) & b2.findLegalMoves(s))) {
                            s = !s;
                        }
                        assert(BIT(move) & b2.findLegalMoves(s));
                        b2 = b2.doMoveOnNewBoardWZH(move, s);
                        s = !s;
                    }
                        
                    int start = (1 << 16) * (tc + depth);

                    edge_wins[tc][(uint16_t) ((COL(b.taken, 0) << 8) | COL(b.black, 0))] += EDGE_VALUES[start + ((uint16_t) ((COL(b2.taken, 0) << 8) | COL(b2.black, 0)))];
                    edge_total[tc][(uint16_t) ((COL(b.taken, 0) << 8) | COL(b.black, 0))]++;
                    edge_wins[tc][(uint16_t) ((COL(b.taken, 7) << 8) | COL(b.black, 7))] += EDGE_VALUES[start + ((uint16_t) ((COL(b2.taken, 7) << 8) | COL(b2.black, 7)))];
                    edge_total[tc][(uint16_t) ((COL(b.taken, 7) << 8) | COL(b.black, 7))]++;
                    edge_wins[tc][(uint16_t) ((b.taken << 8) | (uint8_t) b.black)] += EDGE_VALUES[start + ((uint16_t) ((b2.taken << 8) | (uint8_t) b2.black))];
                    edge_total[tc][(uint16_t) ((b.taken << 8) | (uint8_t) b.black)]++;
                    edge_wins[tc][(uint16_t) (((b.taken >> 56) << 8) | (b.black >> 56))] += EDGE_VALUES[start + ((uint16_t) (((b2.taken >> 56) << 8) | (b2.black >> 56)))];
                    edge_total[tc][(uint16_t) (((b.taken >> 56) << 8) | (b.black >> 56))]++;

                    edge_wins[tc][ (reverse_byte[(uint8_t) COL(b.taken, 0)] << 8) | reverse_byte[(uint8_t) COL(b.black, 0)]] += EDGE_VALUES[start + ( (reverse_byte[(uint8_t) COL(b2.taken, 0)] << 8) | reverse_byte[(uint8_t) COL(b2.black, 0)])];
                    edge_total[tc][(reverse_byte[(uint8_t) COL(b.taken, 0)] << 8) | reverse_byte[(uint8_t) COL(b.black, 0)]]++;
                    edge_wins[tc][ (reverse_byte[(uint8_t) COL(b.taken, 7)] << 8) | reverse_byte[(uint8_t) COL(b.black, 7)]] += EDGE_VALUES[start + ( (reverse_byte[(uint8_t) COL(b2.taken, 7)] << 8) | reverse_byte[(uint8_t) COL(b2.black, 7)])];
                    edge_total[tc][(reverse_byte[(uint8_t) COL(b.taken, 7)] << 8) | reverse_byte[(uint8_t) COL(b.black, 7)]]++;
                    edge_wins[tc][ (reverse_byte[(uint8_t) b.taken] << 8)         | reverse_byte[(uint8_t) b.black]        ] += EDGE_VALUES[start + ( (reverse_byte[(uint8_t) b2.taken] << 8)         | reverse_byte[(uint8_t) b2.black]       )];
                    edge_total[tc][(reverse_byte[(uint8_t) b.taken] << 8)         | reverse_byte[(uint8_t) b.black]        ]++;
                    edge_wins[tc][ (reverse_byte[(uint8_t) (b.taken >> 56)] << 8) | reverse_byte[b.black >> 56]            ] += EDGE_VALUES[start + ( (reverse_byte[(uint8_t) (b2.taken >> 56)] << 8) | reverse_byte[b2.black >> 56]           )];
                    edge_total[tc][(reverse_byte[(uint8_t) (b.taken >> 56)] << 8) | reverse_byte[b.black >> 56]            ]++;
/*
                    uint64_t white = b.taken & ~b.black;
                    uint64_t white2 = b2.taken & ~b2.black;

                    edge_wins[tc][(uint16_t) ((COL(b.taken, 0) << 8) | COL(white, 0))] -= EDGE_VALUES[start + ((uint16_t) ((COL(b2.taken, 0) << 8) | COL(white2, 0)))];
                    edge_total[tc][(uint16_t) ((COL(b.taken, 0) << 8) | COL(white, 0))]++;
                    edge_wins[tc][(uint16_t) ((COL(b.taken, 7) << 8) | COL(white, 7))] -= EDGE_VALUES[start + ((uint16_t) ((COL(b2.taken, 7) << 8) | COL(white2, 7)))];
                    edge_total[tc][(uint16_t) ((COL(b.taken, 7) << 8) | COL(white, 7))]++;
                    edge_wins[tc][(uint16_t) ((b.taken << 8) | (uint8_t) white)] -= EDGE_VALUES[start + ((uint16_t) ((b2.taken << 8) | (uint8_t) white2))];
                    edge_total[tc][(uint16_t) ((b.taken << 8) | (uint8_t) white)]++;
                    edge_wins[tc][(uint16_t) (((b.taken >> 56) << 8) | (white >> 56))] -= EDGE_VALUES[start + ((uint16_t) (((b2.taken >> 56) << 8) | (white2 >> 56)))];
                    edge_total[tc][(uint16_t) (((b.taken >> 56) << 8) | (white >> 56))]++;

                    edge_wins[tc][ (reverse_byte[(uint8_t) COL(b.taken, 0)] << 8) | reverse_byte[(uint8_t) COL(white, 0)]] -= EDGE_VALUES[start + ( (reverse_byte[(uint8_t) COL(b2.taken, 0)] << 8) | reverse_byte[(uint8_t) COL(white2, 0)])];
                    edge_total[tc][(reverse_byte[(uint8_t) COL(b.taken, 0)] << 8) | reverse_byte[(uint8_t) COL(white, 0)]]++;
                    edge_wins[tc][ (reverse_byte[(uint8_t) COL(b.taken, 7)] << 8) | reverse_byte[(uint8_t) COL(white, 7)]] -= EDGE_VALUES[start + ( (reverse_byte[(uint8_t) COL(b2.taken, 7)] << 8) | reverse_byte[(uint8_t) COL(white2, 7)])];
                    edge_total[tc][(reverse_byte[(uint8_t) COL(b.taken, 7)] << 8) | reverse_byte[(uint8_t) COL(white, 7)]]++;
                    edge_wins[tc][ (reverse_byte[(uint8_t) b.taken] << 8)         | reverse_byte[(uint8_t) white]        ] -= EDGE_VALUES[start + ( (reverse_byte[(uint8_t) b2.taken] << 8)         | reverse_byte[(uint8_t) white2]       )];
                    edge_total[tc][(reverse_byte[(uint8_t) b.taken] << 8)         | reverse_byte[(uint8_t) white]        ]++;
                    edge_wins[tc][ (reverse_byte[(uint8_t) (b.taken >> 56)] << 8) | reverse_byte[white >> 56]            ] -= EDGE_VALUES[start + ( (reverse_byte[(uint8_t) (b2.taken >> 56)] << 8) | reverse_byte[white2 >> 56]           )];
                    edge_total[tc][(reverse_byte[(uint8_t) (b.taken >> 56)] << 8) | reverse_byte[white >> 56]            ]++;
*/
                    break;
                }
            }
        }

        // Load EDGE_VALUES
        for (int i = 0; i < (1 << 16); i++) {
            double score;
            if (edge_total[k][i] >= 20) score = (double) edge_wins[k][i] / edge_total[k][i];
            else {
                EDGE_VALUES[(1 << 16) * k + i] = EDGE_VALUES[(1 << 16) * (k + 1) + i] * 1.01;
                if (EDGE_VALUES[(1 << 16) * k + i] > 1) EDGE_VALUES[(1 << 16) * k + i] = 1;
                else if (EDGE_VALUES[(1 << 16) * k + i] < -1) EDGE_VALUES[(1 << 16) * k + i] = -1;
                continue;
            }
            if (abs(score) < 0.05) score = 0;
            EDGE_VALUES[(1 << 16) * k + i] = score;
        }
    }
    
    fstream outFile("edge_values", ios_base::out);
    for (int j = 0; j < (1 << 16); j++) {
        for (int i = 0; i < 65; i++) {
            outFile << i << ' ' << j << ' ' << EDGE_VALUES[(1 << 16) * i + j] << endl;
        }
    }

    return 0;
}