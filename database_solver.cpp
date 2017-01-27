#include "search.h"
#include "player.h"
#include <iostream>

using namespace std;

double *EDGE_VALUES;
uint8_t **STABLE_DISCS;
uint8_t **PSEUDOSTABLE_DISCS;
uint8_t **ALL_STABLE_DISCS;
uint64_t** BYTE_TO_COL;
uint64_t** BYTE_TO_PATTERN;
size_t random_numbers[130];

// Hashes (smaller here)
BoardHash tt(4); // Instead of 256
BoardHash2 tt2(4);
EndgameBoardHash endgameTT(64);

int main() {
    EDGE_VALUES = (double *) malloc(sizeof(double) * 65 * (1 << 16));
    STABLE_DISCS = (uint8_t **) malloc(sizeof(uint8_t *) * 8);
    PSEUDOSTABLE_DISCS = (uint8_t **) malloc(sizeof(uint8_t *) * 8);
    ALL_STABLE_DISCS = (uint8_t **) malloc(sizeof(uint8_t *) * 8);
    BYTE_TO_COL = (uint64_t **) malloc(sizeof(uint64_t *) * 8);
    BYTE_TO_PATTERN = (uint64_t **) malloc(sizeof(uint64_t *) * NUM_PATTERNS);
    for (int i = 0; i < 8; i++) {
        STABLE_DISCS[i] = (uint8_t *) malloc(sizeof(uint8_t) * (1 << 16));
        PSEUDOSTABLE_DISCS[i] = (uint8_t *) malloc(sizeof(uint8_t) * (1 << 16));
        ALL_STABLE_DISCS[i] = (uint8_t *) malloc(sizeof(uint8_t) * (1 << 16));
        BYTE_TO_COL[i] = (uint64_t *) malloc(sizeof(uint64_t) * (1 << 8));
    }
    for (int i = 0; i < NUM_PATTERNS; i++) {
        BYTE_TO_PATTERN[i] = (uint64_t *) malloc(sizeof(uint64_t) * (1 << 8));
    }
    srand(0);
    for (int i = 0; i < 130; i++) random_numbers[i] = ((size_t) rand() << 33) | ((size_t) rand() << 2) | ((size_t) rand() & 3);
    
    Player initializer(true);

    for (int i = 0; i < (1 << 16) * 65; i++) EDGE_VALUES[i] = 0;

    int lineCount = 0;
    int firstMove = 41;
    fstream resultsFile("game_results", ios_base::in);
    if (resultsFile.good()) {
        string line;
        while (getline(resultsFile, line)) {
            lineCount++;
            stringstream ss(line);
            int move;
            while (ss >> move) {
                firstMove++;
            }
        }
    }
    resultsFile.close();

    fstream fil("game_database", ios_base::in);
    fstream outFile("game_results", ios_base::app);

    string line;
    int counter = 0;
    restart_loop:
    while (getline(fil, line)) {
        counter++;
        if (counter < lineCount) continue;
        cout << counter << endl;

        Board b;
        Side s = BLACK;
        stringstream ss(line);
        int moves[60];
        for (int i = 0; i < 60; i++) {
            ss >> moves[i];
        }
        
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
            if (tc <= 40) continue;
            else if (lineCount == counter && tc < firstMove) goto restart_loop;
            else {
                int result = endgame_alphabeta(b, s);
                if (result != 0) result = result / abs(result);
                outFile << ' ' << result;
            }
        }

        outFile << endl;
    }
    
    return 0;
}
