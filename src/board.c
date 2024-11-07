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

// Function to print the bitboard with pieces
void print_bitboard(uint64_t bitboard) {
    char board[64];
    memset(board, '.', sizeof(board)); // Initialize the board with empty squares

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

    // Print the board
    for (int letter = 0; letter < 8; letter++) {
        for (int number = 0; number < 8; number++) {
            int square = letter * 8 + number;
            printf("%c ", board[square]);
        }
        printf("\n");
    }
    printf("\n");
}