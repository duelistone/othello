#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <list>
#include "player.h"
#include "tt.h"

using namespace std;

// Global variables...I'll get rid of these if I have time
long long globalEndgameNodeCount = 0;
unordered_map< BoardWithSide, vector<int> * > *um = new unordered_map< BoardWithSide, vector<int> * >;
unordered_map< BoardWithSide, int > *um2 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um3 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um4 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um5 = new unordered_map< BoardWithSide, int >;
atomic_bool abortEndgameMinimax;
double minutesForMove = 1;
int *EDGE_VALUES;
uint8_t **STABLE_DISCS;
uint8_t **PSEUDOSTABLE_DISCS;
uint8_t **ALL_STABLE_DISCS;
uint64_t** BYTE_TO_COL;
uint64_t** BYTE_TO_PATTERN;
int ordered_moves[64][64];
uint64_t SINGLE_BIT[64];
fstream fil("c.txt", ios_base::out);

vector< unordered_map< BoardWithSide, int > * > vec_of_ums;
unordered_map< BoardWithSide, int > *um_1 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um_2 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um_3 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um_4 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um_5 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um_6 = new unordered_map< BoardWithSide, int >;

BoardHash tt(1024);

size_t random_numbers[130];

void initialize_hashes() {
    vec_of_ums.push_back(um_1);
    //~ vec_of_ums.push_back(um_2);
    //~ vec_of_ums.push_back(um_3);
    //~ vec_of_ums.push_back(um_4);
    //~ vec_of_ums.push_back(um_5);
    //~ vec_of_ums.push_back(um_6);
}

void delete_hashes() {
    delete um_1;
    delete um_2;
    delete um_3;
    delete um_4;
    delete um_5;
    delete um_6;
}

int main(int argc, char *argv[]) {
    // Read in side the player is on.
    if (argc != 2)  {
        cerr << "usage: " << argv[0] << " side" << endl;
        exit(-1);
    }
    Side side = (!strcmp(argv[1], "Black")) ? BLACK : WHITE;

    // Initialize SINGLE_BIT array
    for (int i = 0; i < 64; i++) {
        SINGLE_BIT[i] = BIT(i);
    }
    
    // Allocate
    EDGE_VALUES = (int *) malloc(sizeof(int) * (1 << 16));
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

    // Initialize hashes
    //~ initialize_hashes();
    
    // Initialize random numbers
    srand(0);
    for (int i = 0; i < 130; i++) random_numbers[i] = ((size_t) rand() << 33) | ((size_t) rand() << 2) | ((size_t) rand() & 3);
    
    // Initialize player.
    Player *player = new Player(side);

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
    for (int i = 0; i < NUM_REPETITIONS; i++) {alphabeta(b, 0, WHITE);}
tim.end(); cerr << NUM_REPETITIONS << endl;   exit(0);
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
    delete_hashes();
    if (um != nullptr) delete um;
    if (um2 != nullptr) delete um2;
    if (um3 != nullptr) delete um3;
    if (um4 != nullptr) delete um4;
    if (player != nullptr) delete player;
    
    
    return 0;
}
