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
	void end(string message = "") {
		cerr << message << chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count() << endl;
	}
};
