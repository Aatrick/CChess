#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../include/board.h"

// Define bitboards for each piece type and side
uint64_t white_pawns = 0x000000000000FF00;
uint64_t white_knights = 0x0000000000000042;
uint64_t white_bishops = 0x0000000000000024;
uint64_t white_rooks = 0x0000000000000081;
uint64_t white_queens = 0x0000000000000008;
uint64_t white_kings = 0x0000000000000010;

uint64_t black_pawns = 0x00FF000000000000;
uint64_t black_knights = 0x4200000000000000;
uint64_t black_bishops = 0x2400000000000000;
uint64_t black_rooks = 0x8100000000000000;
uint64_t black_queens = 0x0800000000000000;
uint64_t black_kings = 0x1000000000000000;

// Function to convert chess notation to bitboard index
int chess_to_bitboard_index(const char* position) {
    if (strlen(position) != 2) return -1; // Invalid position

    char letter = position[0];
    char number = position[1];

    if (letter < 'A' || letter > 'H' || number < '1' || number > '8') return -1; // Invalid position

    int letter_index = letter - 'A';
    int number_index = number - '1';

    return number_index * 8 + letter_index;
}

// Function to convert chess position to uint64_t bitboard
uint64_t chess_position_to_bitboard(const char* position) {
    int index = chess_to_bitboard_index(position);
    if (index == -1) return 0; // Invalid position

    return 1ULL << index;
}

// Function to print the bitboard with pieces
void print_bitboard(const char* current_position, const char* next_position) {
    char board[64];
    memset(board, '.', sizeof(board)); // Initialize the board with empty squares

    //works because a uint64 and a chessboard both have 64 informations
    // Place white pieces
    for (int i = 0; i < 64; i++) {
        if ((white_pawns >> i) & 1) board[63 - i] = 'P';
        if ((white_knights >> i) & 1) board[63 - i] = 'N';
        if ((white_bishops >> i) & 1) board[63 - i] = 'B';
        if ((white_rooks >> i) & 1) board[63 - i] = 'R';
        if ((white_queens >> i) & 1) board[63 - i] = 'Q';
        if ((white_kings >> i) & 1) board[63 - i] = 'K';
    }

    // Place black pieces
    for (int i = 0; i < 64; i++) {
        if ((black_pawns >> i) & 1) board[63 - i] = 'p';
        if ((black_knights >> i) & 1) board[63 - i] = 'n';
        if ((black_bishops >> i) & 1) board[63 - i] = 'b';
        if ((black_rooks >> i) & 1) board[63 - i] = 'r';
        if ((black_queens >> i) & 1) board[63 - i] = 'q';
        if ((black_kings >> i) & 1) board[63 - i] = 'k';
    }

    // Move piece from current_position to next_position
    int current_index = chess_to_bitboard_index(current_position);
    int next_index = chess_to_bitboard_index(next_position);
    if (current_index != -1 && next_index != -1) {
        board[63 - next_index] = board[63 - current_index];
        board[63 - current_index] = '.';
    }

    // Print the board
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            printf("%c ", board[square]);
        }
        printf("\n");
    }
    printf("\n");
}