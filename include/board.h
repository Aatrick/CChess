#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

void print_bitboard(const char* current_position, const char* next_position);
char* bitboard_to_chess_position(uint64_t bitboard);
uint64_t chess_position_to_bitboard(const char* position);

#endif