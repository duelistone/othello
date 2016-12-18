#ifndef __TT_H__
#define __TT_H__

#include <algorithm>
#include "common.h"
#include "board.h"

using namespace std;

struct BoardHash {
	size_t mod; // Should be a power of 2!
	uint8_t *table;
	
	BoardHash(const int &size_in_mb) : mod(size_in_mb * 1024 * 1024) {
		table = new uint8_t[mod];
		fill_n(table, mod, 64); // Set to invalid value to mark unused
	}
    inline uint8_t operator[] (const size_t &zh) const {
        return table[zh & (mod - 1)];
    }
    inline uint8_t & operator[] (const size_t &zh) {
        return table[zh & (mod - 1)];
    }/*
	inline uint8_t operator[] (const BoardWithSide &bws) const {
		return table[bws.hash_value & (mod - 1)];
	}
	inline uint8_t & operator[] (const BoardWithSide &bws) {
		return table[bws.hash_value & (mod - 1)];
	}*/
	~BoardHash() { delete table; }
	
};

#endif
