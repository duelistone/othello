#ifndef __COMMON_H__
#define __COMMON_H__

enum Side { 
    WHITE, BLACK
};

class Move {
   
public:
    int x, y;
    Move(int x, int y) {
        this->x = x;
        this->y = y;        
    }
    ~Move() {}

    int getX() { return x; }
    int getY() { return y; }

    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
};

class Node {
	Node *parent;
	int moveTo;
	Board board;
	Side toMove;
	
	Node(Node *p, int m, Board b, Side s) : parent(p), moveTo(m), board(b), toMove(s) {}
}

#endif
