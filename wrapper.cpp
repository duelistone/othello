#include <iostream>
#include <cstdlib>
#include <cstring>
#include "player.h"
using namespace std;

// Global variables...I'll get rid of these if I have time
long long globalEndgameNodeCount = 0;
unordered_map< BoardWithSide, pair<int, int> > *um = new unordered_map< BoardWithSide, pair<int, int> >;
unordered_map< BoardWithSide, pair<int, int> > *ttable = new unordered_map< BoardWithSide, pair<int, int> >;
atomic_bool abortEndgameMinimax;
double minutesForMove = 1;
mutex um_lock;
mutex ttable_lock;

int main(int argc, char *argv[]) {    
    // Read in side the player is on.
    if (argc != 2)  {
        cerr << "usage: " << argv[0] << " side" << endl;
        exit(-1);
    }
    Side side = (!strcmp(argv[1], "Black")) ? BLACK : WHITE;

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
	
	// Delete hash table and player
	if (um != nullptr) delete um;
	if (ttable != nullptr) delete ttable;
	if (player != nullptr) delete player;
	
    return 0;
}
