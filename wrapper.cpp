#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include "player.h"
#include "tt.h"
using namespace std;

// Global variables
double *EDGE_VALUES;
uint8_t **STABLE_DISCS;
uint8_t **PSEUDOSTABLE_DISCS;
uint8_t **ALL_STABLE_DISCS;
uint64_t** BYTE_TO_COL;
uint64_t** BYTE_TO_PATTERN;

BoardHash tt(256);
BoardHash2 tt2(4);
EndgameBoardHash endgameTT(64);

size_t random_numbers[130];

int main(int argc, char *argv[]) {
    // Read in side the player is on.
    if (argc != 2)  {
        cerr << "usage: " << argv[0] << " side" << endl;
        exit(-1);
    }
    Side side = (!strcmp(argv[1], "Black")) ? BLACK : WHITE;

    // Allocate
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
    cerr << "Arrays allocated." << endl;

    // Initialize random numbers
    srand(0);
    for (int i = 0; i < 130; i++) random_numbers[i] = ((size_t) rand() << 33) | ((size_t) rand() << 2) | ((size_t) rand() & 3);
    
    // Initialize player.
    Player *player = new Player(side);

    // Test stable discs
    #define PS(s, x) cerr << x << endl;  PRINTBS((uint64_t) STABLE_DISCS[s - 1][x])
   // PS(7, 0b0000100000001000);
   // PS(6, 0b0010100000000000);
   // PS(7, 0b0111100000000000);
   // exit(0);

    /*
    cerr << "Further testing: " <<endl;
    for (int i = 12; i < 16; i++) {
        for (int j = 0; j < (1 << pattern_size[i]) - 1; j++) {
            cerr << i << ' ' << bitset<8>(j) << ' ' << bitset<64>(BYTE_TO_PATTERN[pattern_size[i] - 1][j]) << endl;
        }
    }*/
    // Test masks
    //for (int i = 0; i < NUM_PATTERNS; i++) {
    //    cerr << bitset<64>(pattern_mask[i]) << endl;
    //    cerr << bitset<64>(pattern_magic_number[i]) << endl;
    //    cerr << "product " << bitset<64>(pattern_mask[i] * pattern_magic_number[i]) << endl;
    //}
/*
    sleep(1);
    Board b(ENTIRE_BOARD ^ BIT(0) ^ BIT(1), ENTIRE_BOARD ^ BIT(0) ^ BIT(1) ^ BIT(63)), b2(ENTIRE_BOARD ^ BIT(0) ^ BIT(1), BIT(63));
Timer tim;
    #define NUM_REPETITIONS 10000000
    for (int i = 0; i < NUM_REPETITIONS; i++) {b.doMoveOnNewBoardBlackWZH(i % 64);}
tim.end(); cerr << NUM_REPETITIONS << endl; // exit(0);
*/

    // Tell java wrapper that we are done initializing.
    cout << "Init done" << endl;
    cout.flush();    
    
    int moveX, moveY, msLeft;

    // Get opponent's move and time left for player each turn.
    while (cin >> moveX >> moveY >> msLeft) {
        Move *opponentsMove = NULL;
        if (moveX >= 0 && moveY >= 0) {
            opponentsMove = new Move(moveX, moveY);
        }
        
        // Get player's move and output to java wrapper.
        Move *playersMove = player->doMove(opponentsMove, msLeft);                        
        if (playersMove != NULL) {                  
            cout << playersMove->x << " " << playersMove->y << endl;
        } else {
            cout << "-1 -1" << endl;
        }
        cout.flush();
        cerr.flush();
        
        // Delete move objects.
        if (opponentsMove != NULL) delete opponentsMove;
        if (playersMove != NULL) delete playersMove; 
    }

    // fil.close();
    
    // Delete hash table and player
    if (player != nullptr) delete player;
}
