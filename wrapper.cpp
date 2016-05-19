#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <list>
#include "player.h"
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
int ordered_moves[64][64];
uint64_t SINGLE_BIT[64];
// fstream fil("c.txt", ios_base::app);

vector< unordered_map< BoardWithSide, int > * > vec_of_ums;
unordered_map< BoardWithSide, int > *um_1 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um_2 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um_3 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um_4 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um_5 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um_6 = new unordered_map< BoardWithSide, int >;

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
	
	// Allocate for EDGE_VALUES
	EDGE_VALUES = (int *) malloc(sizeof(int) * (1 << 16));
	
	// Initialize hashes
	initialize_hashes();
	
    // Initialize player.
    Player *player = new Player(side);
    
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
