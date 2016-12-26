#include "defs.h"
#include "board.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;
int **EDGE_VALUES;
size_t random_numbers[130];
uint64_t SINGLE_BIT[64];
uint8_t **STABLE_DISCS;
uint64_t **BYTE_TO_PATTERN;
int main() {
    fstream fil("game_database", ios_base::in);
    fstream outFile("edge_values", ios_base::out);
    // fstream nnFile("training_data", ios_base::out);

    int **edge_wins, **edge_total;
    edge_wins = (int **) malloc(sizeof(int *) * 65);
    edge_total = (int **) malloc(sizeof(int *) * 65);
    for (int i = 0; i < 65; i++) {
        edge_wins[i] = (int *) malloc(sizeof(int) * (1 << 16));
        edge_total[i] = (int *) malloc(sizeof(int) * (1 << 16));
    }
    
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

             edge_wins[tc][(reverse_byte[(uint8_t) COL(b.taken, 0)] << 8) | reverse_byte[(uint8_t) COL(b.black, 0)]] += result;
            edge_total[tc][(reverse_byte[(uint8_t) COL(b.taken, 0)] << 8) | reverse_byte[(uint8_t) COL(b.black, 0)]]++;
             edge_wins[tc][(reverse_byte[(uint8_t) COL(b.taken, 7)] << 8) | reverse_byte[(uint8_t) COL(b.black, 7)]] += result;
            edge_total[tc][(reverse_byte[(uint8_t) COL(b.taken, 7)] << 8) | reverse_byte[(uint8_t) COL(b.black, 7)]]++;
             edge_wins[tc][(reverse_byte[(uint8_t) b.taken] << 8)         | reverse_byte[(uint8_t) b.black]        ] += result;
            edge_total[tc][(reverse_byte[(uint8_t) b.taken] << 8)         | reverse_byte[(uint8_t) b.black]        ]++;
             edge_wins[tc][(reverse_byte[(uint8_t) (b.taken >> 56)] << 8) | reverse_byte[b.black >> 56]            ] += result;
            edge_total[tc][(reverse_byte[(uint8_t) (b.taken >> 56)] << 8) | reverse_byte[b.black >> 56]            ]++;
        }
    }

    // Print to outFile
    for (int i = 0; i < (1 << 16); i++) {
        double score = (edge_total[40][i] >= 8) ? (double) edge_wins[40][i] / edge_total[40][i] : 0;
        if (abs(score) < 0.01) score = 0;
        for (int j = 0; j < 65; j++) {
            outFile << j << ' ' << i << ' ' << score * (65 - j) / 24 << endl;
        }
    }

    return 0;
}
