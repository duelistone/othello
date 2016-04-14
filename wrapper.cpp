#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include "player.h"
using namespace std;

// Global variables...I'll get rid of these if I have time
long long globalEndgameNodeCount = 0;
unordered_map< BoardWithSide, um_data > *um = new unordered_map< BoardWithSide, um_data >;
unordered_map< BoardWithSide, int > *um2 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um3 = new unordered_map< BoardWithSide, int >;
unordered_map< BoardWithSide, int > *um4 = new unordered_map< BoardWithSide, int >;
atomic_bool abortEndgameMinimax;
double minutesForMove = 1;
int *EDGE_VALUES;
uint64_t SINGLE_BIT[64];
// fstream fil("c.txt", ios_base::app);

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
	if (um != nullptr) delete um;
	if (um2 != nullptr) delete um2;
	if (um3 != nullptr) delete um3;
	if (um4 != nullptr) delete um4;
	if (player != nullptr) delete player;
	
	
    return 0;
}
