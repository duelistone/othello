#ifndef __COMMON_H__
#define __COMMON_H__

#include <iostream>

#define BLACK 1
#define WHITE 0

#define DEFAULT_INITIAL_EVAL -10000

using namespace std;

typedef int Side;

typedef struct {
	int eval;
	int best;
	int alpha;
	int beta;
} um_data;

class Evaluation {
public:
	int position;
	int mobility;
	Evaluation() : position(DEFAULT_INITIAL_EVAL), mobility(DEFAULT_INITIAL_EVAL) {}
	Evaluation(const Evaluation &eval) : position(eval.position), mobility(eval.mobility) {}
	Evaluation(int position, int mobility) : position(position), mobility(mobility) {}
};

inline bool operator<(const Evaluation e1, const Evaluation e2) {
	return (e1.position < e2.position) || (e1.position == e2.position && e1.mobility < e2.mobility);
}

inline bool operator<=(const Evaluation e1, const Evaluation e2) {
	return (e1.position < e2.position) || (e1.position == e2.position && e1.mobility <= e2.mobility);
}

inline bool operator>(const Evaluation e1, const Evaluation e2) {
	return (e1.position > e2.position) || (e1.position == e2.position && e1.mobility > e2.mobility);
}

inline bool operator>=(const Evaluation e1, const Evaluation e2) {
	return (e1.position > e2.position) || (e1.position == e2.position && e1.mobility >= e2.mobility);
}

inline Evaluation operator+=(Evaluation e1, const int n) {
	return Evaluation(e1.position + n, e1.mobility + n);
}

inline Evaluation operator-=(Evaluation e1, const int n) {
	return Evaluation(e1.position - n, e1.mobility - n);
}

inline ostream &operator<<(ostream &os, const Evaluation e) {
	os << '(' << e.position << ", " << e.mobility << ')';
	return os;
}

inline bool operator!=(const Evaluation e1, const int n) {
	return e1.position != n || e1.position != n;
}

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
