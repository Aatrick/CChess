#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

int chess_to_bitboard_index(const char* position);
void place_pieces(char* board, uint64_t bitboard, char piece);
void print_board();
void move_piece(const char* current_position, const char* next_position);
void initialize_board();
int val_in_array(int val, int *arr, size_t n);
int is_square_under_attack(int square, char attacking_side);
int evaluate(const char side);


#endif