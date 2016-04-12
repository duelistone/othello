#ifndef __COMMON_H__
#define __COMMON_H__

#define BLACK 1
#define WHITE 0

typedef int Side;

typedef struct {
	int eval;
	int best;
	int alpha;
	int beta;
} um_data;

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

#endif
