#ifndef __TIMER_H__
#define __TIMER_H__

#include <chrono>
#include <string>
#include <iostream>

using namespace std;

class Timer {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
public:
	Timer() {
		start = chrono::high_resolution_clock::now();
	}
	int getms() {
		return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count();
	}
	void end(const string &message = "") {
		cerr << message << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
	}
};

#endif
