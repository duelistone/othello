#ifndef __DEFS_H__
#define __DEFS_H__

#define BLACK 1
#define WHITE 0

#define BIT(x) ((uint64_t) 1 << (63 - (x)))
#define RBIT(x) ((uint64_t) 1 << (x))
#define BIT_SAFE(x) ((x) < 64 ? (uint64_t) 1 << (63 - (x)) : 0)
#define OCCUPY_BLACK(x, t, b) {(t) |= BIT(x); (b) |= BIT(x);}
#define OCCUPY_WHITE(x, t, b) {(t) |= BIT(x);}
#define TO_INDEX(x, y) ((x) + 8 * (y))
#define OCCUPIED(x, y, t) (((t) & BIT(TO_INDEX(x, y))) != 0)
#define OCCUPIED_SIDE(side, x, y, t, b) ( OCCUPIED(x, y, t) && ( ( (b) & BIT(TO_INDEX(x, y)) ) == ( ((side) == BLACK) ? BIT(TO_INDEX(x, y)) : 0 ) ) )
#define OCCUPIED_BI(t) (((t) & bi) != 0)
#define ON_BOARD(x, y) (0 <= (x) && (x) < 8 && 0 <= (y) && (y) < 8)
#define PLACE_DISC(side, x, y, t, b) {(t) |= BIT(TO_INDEX(x, y)); if ((side) == BLACK) (b) |= BIT(TO_INDEX(x, y)); else (b) &= ~BIT(TO_INDEX(x, y));}
#define FROM_INDEX_X(index) ((index) % 8)
#define FROM_INDEX_Y(index) ((index) / 8)
#define OTHER_SIDE(side) ((int) (!(side)))
#define CORNER_TL (BIT(0))
#define CORNER_TR (BIT(7))
#define CORNER_BL (BIT(56))
#define CORNER_BR (BIT(63))
#define CORNERS (BIT(0) | BIT(7) | BIT(56) | BIT(63))
#define DIAGONAL_FROM_CORNERS (BIT(TO_INDEX(1, 1)) | BIT(TO_INDEX(1, 6)) | BIT(TO_INDEX(6, 1)) | BIT(TO_INDEX(6, 6)))
#define NEXT_TO_CORNERS (BIT(1) | BIT(6) | BIT(62) | BIT(57) | BIT(8) | BIT(48) | BIT(15) | BIT(55))
#define X_SQUARES (BIT(9) | BIT(14) | BIT(49) | BIT(54))
#define NEXT_TO_CORNER_TL (BIT(1) | BIT(8) | BIT(9))
#define NEXT_TO_CORNER_TR (BIT(6) | BIT(15) | BIT(14))
#define NEXT_TO_CORNER_BL (BIT(48) | BIT(49) | BIT(57))
#define NEXT_TO_CORNER_BR (BIT(62) | BIT(55) | BIT(54))
#define INNER_EDGE_TOP 0b0111111000000000000000000000000000000000000000000000000000000000ull
#define INNER_EDGE_BOTTOM 0b0000000000000000000000000000000000000000000000000000000001111110ull
#define INNER_EDGE_LEFT 0b0000000010000000100000001000000010000000100000001000000000000000ull
#define INNER_EDGE_RIGHT 0b0000000000000001000000010000000100000001000000010000000100000000ull
#define INNER_EDGES (INNER_EDGE_TOP | INNER_EDGE_BOTTOM | INNER_EDGE_LEFT | INNER_EDGE_RIGHT)
#define EDGE_TOP 0b1111111100000000000000000000000000000000000000000000000000000000ull
#define EDGE_BOTTOM 0b0000000000000000000000000000000000000000000000000000000011111111ull
#define EDGE_LEFT 0b1000000010000000100000001000000010000000100000001000000010000000ull
#define EDGE_LEFT2 0b0100000001000000010000000100000001000000010000000100000001000000ull
#define EDGE_LEFT3 0b0010000000100000001000000010000000100000001000000010000000100000ull
#define EDGE_RIGHT 0b0000000100000001000000010000000100000001000000010000000100000001ull
#define EDGE_RIGHT2 0b0000000010000000100000001000000010000000100000001000000010000001ull
#define EDGE_RIGHT3 0b0000000001000000010000000100000001000000010000000100000001000001ull
#define EDGES 0b1111111110000001100000011000000110000001100000011000000111111111ull
#define ENTIRE_BOARD 0xFFFFFFFFFFFFFFFFull
#define SAFE_SQUARES (~X_SQUARES)

#define COL_MASK(x) (EDGE_LEFT >> (x))
#define ROW_MASK(x) (EDGE_TOP >> (8 * (x)))
#define COL(bits, x) (((bits & COL_MASK(x)) * MAGIC_COL(x)) >> 56) // The parentheses around bits would actually causes C++14 to ask for its address

#define EDGE_BIT(x) ((uint16_t) 1 << (15 - (x)))
#define JUST_EDGE_BIT(x) ((uint8_t) 1 << (7 - (x)))

#define MAGIC_COL_NUMBER 0x0002040810204081ull
#define MAGIC_COL(x) (MAGIC_COL_NUMBER << (x))

#define NW_DIAGONAL_3_MASK 0x2040800000000000ull
#define NE_DIAGONAL_3_MASK 0x0402010000000000ull
#define SW_DIAGONAL_3_MASK 0x0000000000804020ull
#define SE_DIAGONAL_3_MASK 0x0000000000010204ull
#define NW_DIAGONAL_4_MASK ((NW_DIAGONAL_3_MASK >> 1) | BIT(24))
#define NE_DIAGONAL_4_MASK ((NE_DIAGONAL_3_MASK << 1) | BIT(31))
#define SW_DIAGONAL_4_MASK ((SW_DIAGONAL_3_MASK >> 1) | BIT(32))
#define SE_DIAGONAL_4_MASK ((SE_DIAGONAL_3_MASK << 1) | BIT(39))
#define NW_DIAGONAL_5_MASK ((NW_DIAGONAL_4_MASK >> 1) | BIT(32))
#define NE_DIAGONAL_5_MASK ((NE_DIAGONAL_4_MASK << 1) | BIT(39))
#define SW_DIAGONAL_5_MASK ((SW_DIAGONAL_4_MASK >> 1) | BIT(24))
#define SE_DIAGONAL_5_MASK ((SE_DIAGONAL_4_MASK << 1) | BIT(31))
#define NW_DIAGONAL_6_MASK ((NW_DIAGONAL_5_MASK >> 1) | BIT(40))
#define NE_DIAGONAL_6_MASK ((NE_DIAGONAL_5_MASK << 1) | BIT(47))
#define SW_DIAGONAL_6_MASK ((SW_DIAGONAL_5_MASK >> 1) | BIT(16))
#define SE_DIAGONAL_6_MASK ((SE_DIAGONAL_5_MASK << 1) | BIT(23))
#define NW_DIAGONAL_7_MASK ((NW_DIAGONAL_6_MASK >> 1) | BIT(48))
#define NE_DIAGONAL_7_MASK ((NE_DIAGONAL_6_MASK << 1) | BIT(55))
#define SW_DIAGONAL_7_MASK ((SW_DIAGONAL_6_MASK >> 1) | BIT(8))
#define SE_DIAGONAL_7_MASK ((SE_DIAGONAL_6_MASK << 1) | BIT(15))
#define NW_DIAGONAL_8_MASK 0x0102040810204080ull
#define NE_DIAGONAL_8_MASK 0x8040201008040201ull

#define MAGIC_NW_DIAGONAL_3 (RBIT(16) | RBIT(8) | RBIT(0))
#define MAGIC_NE_DIAGONAL_3 (RBIT(5) | RBIT(13) | RBIT(21))
#define MAGIC_SW_DIAGONAL_3 (RBIT(40) | RBIT(48) | RBIT(56))
#define MAGIC_SE_DIAGONAL_3 (RBIT(40) | RBIT(48) | RBIT(56))
#define MAGIC_NW_DIAGONAL_4 ((MAGIC_NW_DIAGONAL_3 << 8) | RBIT(0))
#define MAGIC_NE_DIAGONAL_4 ((MAGIC_NE_DIAGONAL_3 >> 1) | RBIT(24 + 4))
#define MAGIC_SW_DIAGONAL_4 ((MAGIC_SW_DIAGONAL_3 >> 8) | RBIT(56))
#define MAGIC_SE_DIAGONAL_4 ((MAGIC_SE_DIAGONAL_3 >> 8) | RBIT(56)) 
#define MAGIC_NW_DIAGONAL_5 ((MAGIC_NW_DIAGONAL_4 << 8) | RBIT(0))
#define MAGIC_NE_DIAGONAL_5 ((MAGIC_NE_DIAGONAL_4 >> 1) | RBIT(32 + 3))
#define MAGIC_SW_DIAGONAL_5 ((MAGIC_SW_DIAGONAL_4 >> 8) | RBIT(56))
#define MAGIC_SE_DIAGONAL_5 ((MAGIC_SE_DIAGONAL_4 >> 8) | RBIT(56))
#define MAGIC_NW_DIAGONAL_6 ((MAGIC_NW_DIAGONAL_5 << 8) | RBIT(0))
#define MAGIC_NE_DIAGONAL_6 ((MAGIC_NE_DIAGONAL_5 >> 1) | RBIT(40 + 2))
#define MAGIC_SW_DIAGONAL_6 ((MAGIC_SW_DIAGONAL_5 >> 8) | RBIT(56))
#define MAGIC_SE_DIAGONAL_6 ((MAGIC_SE_DIAGONAL_5 >> 8) | RBIT(56))
#define MAGIC_NW_DIAGONAL_7 ((MAGIC_NW_DIAGONAL_6 << 8) | RBIT(0))
#define MAGIC_NE_DIAGONAL_7 ((MAGIC_NE_DIAGONAL_6 >> 1) | RBIT(48 + 1))
#define MAGIC_SW_DIAGONAL_7 ((MAGIC_SW_DIAGONAL_6 >> 8) | RBIT(56))
#define MAGIC_SE_DIAGONAL_7 ((MAGIC_SE_DIAGONAL_6 >> 8) | RBIT(56))
#define MAGIC_NW_DIAGONAL_8 ((MAGIC_NW_DIAGONAL_7 << 8) | RBIT(0))
#define MAGIC_NE_DIAGONAL_8 ((MAGIC_NE_DIAGONAL_7 >> 1) | RBIT(56))

#define NUM_PATTERNS 34

#define DOWN_FILTER 0x00FFFFFFFFFFFFFFULL
#define RIGHT_FILTER 0x7F7F7F7F7F7F7F7FULL
#define DOWN_RIGHT_FILTER 0x007F7F7F7F7F7F7FULL
#define DOWN_LEFT_FILTER 0x00FEFEFEFEFEFEFEULL
#define UP_FILTER 0xFFFFFFFFFFFFFF00ULL
#define LEFT_FILTER 0xFEFEFEFEFEFEFEFEULL
#define UP_RIGHT_FILTER 0x7F7F7F7F7F7F7F00ULL
#define UP_LEFT_FILTER 0xFEFEFEFEFEFEFE00ULL

#define NUM_NORMAL_PATTERNS 23
#define NUM_SPECIAL_PATTERNS 11

#define PATTERN_TO_BYTE(p, x) (((x) * pattern_magic_number[p]) >> (64 - pattern_size[p]))
#define PATTERN_TO_BYTE_NW(p, x) (reverse_byte[((x) * pattern_magic_number[p]) >> 56])
#define PATTERN_TO_BYTE_SE(p, x) ((reverse_byte[((x) * pattern_magic_number[p]) >> 56]) >> (8 - pattern_size[p]))

#define PRINTBS(b) {cerr << bitset<8>((b) >> 56) << endl << bitset<8>((b) >> 48) << endl << bitset<8>((b) >> 40) << endl << bitset<8>((b) >> 32) << endl << bitset<8>((b) >> 24) << endl << bitset<8>((b) >> 16) << endl << bitset<8>((b) >> 8) << endl << bitset<8>(b) << endl;}

// For stable discs function
#define ROW_CHECK_HELPER(i) ((uint64_t) STABLE_DISCS[7][((taken >> (40 - 8 * i)) & 0xFF00) | (0xFF & (black >> (48 - 8 * i)))] << (48 - 8 * i))
#define COL_CHECK_HELPER(i) ((uint64_t) STABLE_DISCS[7][((reflected_taken >> (40 - 8 * i)) & 0xFF00) | (0xFF & (reflected_black >> (48 - 8 * i)))] << (48 - 8 * i))
#define ROW_CHECK(i) stable &= ROW_CHECK_HELPER(i) | ~pattern_mask[i];
#define COL_CHECK(i) reflected_stable &= COL_CHECK_HELPER(i) | ~pattern_mask[i];

#define LOOP_INNER(i) stable &= ~pattern_mask[i] | BYTE_TO_PATTERN[i][STABLE_DISCS[pattern_size[i] - 1][((uint16_t) PATTERN_TO_BYTE(i, taken & pattern_mask[i]) << 8) | PATTERN_TO_BYTE(i, black & pattern_mask[i])]]; 
#define LOOP_INNER_SE(i) stable &= ~pattern_mask[i] | BYTE_TO_PATTERN[i][STABLE_DISCS[pattern_size[i] - 1][((uint16_t) PATTERN_TO_BYTE_SE(i, taken & pattern_mask[i]) << 8) | PATTERN_TO_BYTE_SE(i, black & pattern_mask[i])]]; 
#define LOOP_INNER_NW(i) stable &= ~pattern_mask[i] | BYTE_TO_PATTERN[i][STABLE_DISCS[pattern_size[i] - 1][((uint16_t) PATTERN_TO_BYTE_NW(i, taken & pattern_mask[i]) << 8) | PATTERN_TO_BYTE_NW(i, black & pattern_mask[i])]];

// To speed things up in the Player constructor's helper
#define HELPER8(s, n) {for (taken = (1 << (n)) - 1; taken < 1 << (s); taken = snoob(taken)) {uint64_t black = 0; uint8_t corners = 1 | (1 << (s - 1)); do {RowBoard<(s)> rb(taken, black); rb.set_stable(); EDGE_VALUES[(taken << 8) | black] = STABLE_WEIGHT * (__builtin_popcount(rb.all_stable() & black) - __builtin_popcount(rb.all_stable() & ~black)) + CORNER_WEIGHT * (__builtin_popcount(black & corners) - __builtin_popcount(taken & ~black & corners)); black = (black - taken) & taken;} while (black);}}
#define HELPER(s, n) {for (taken = (1 << (n)) - 1; taken < 1 << (s); taken = snoob(taken)) {uint64_t black = 0; do {RowBoard<(s)> rb(taken, black); rb.set_stable(); black = (black - taken) & taken;} while (black);}}

// Configuration
#define X_SQUARE_PENALTY 40
#define MOBILITY_WEIGHT 70
#define FRONTIER_WEIGHT 45
#define DISC_DIFFERENCE_WEIGHT 10
#define STABLE_NONEDGES_WEIGHT 70 
#define INBALANCED_POSITION_PENALTY 0.3
#define CORNER_HANGING_PENALTY 60
#define EDGE_WEIGHT 450
#define HASH_DEPTH (MAX_DEPTH - 1)
#define CORNER_WEIGHT 6
#define STABLE_WEIGHT 3
#define DIVISOR_FOR_AW 13
#define MAX_PENULTIMATE_DEPTH_TIME 25
#define SIMPLE_EVAL 1
#define KILLER_HEURISTIC 0 
#define IID 1
#define USE_HASH_IN_ENDGAME_ALPHABETA 1
#define MAX_DEPTH 15

#endif
