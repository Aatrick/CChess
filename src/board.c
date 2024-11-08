#include <stdio.h>
#include <stdint.h>
#include <string.h>
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

// Global board state
char board[64];

// Helper function to place pieces on the board
void place_pieces(char* board, uint64_t bitboard, char piece) {
    for (int i = 0; i < 64; i++) {
        if ((bitboard >> i) & 1) {
            board[63 - i] = piece;
        }
    }
}

// Function to initialize the board with the standard starting positions
void initialize_board() {
    memset(board, '.', sizeof(board)); // Initialize the board with empty squares

    place_pieces(board, white_pawns, 'P');
    place_pieces(board, white_knights, 'C');
    place_pieces(board, white_bishops, 'B');
    place_pieces(board, white_rooks, 'R');
    place_pieces(board, white_queens, 'Q');
    place_pieces(board, white_kings, 'K');

    place_pieces(board, black_pawns, 'p');
    place_pieces(board, black_knights, 'c');
    place_pieces(board, black_bishops, 'b');
    place_pieces(board, black_rooks, 'r');
    place_pieces(board, black_queens, 'q');
    place_pieces(board, black_kings, 'k');
}

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

// Function to move a piece on the board
void move_piece(const char* current_position, const char* next_position) {
    int current_index = chess_to_bitboard_index(current_position);
    int next_index = chess_to_bitboard_index(next_position);
    if (current_index != -1 && next_index != -1) {
        board[63 - next_index] = board[63 - current_index];
        board[63 - current_index] = '.';
    }
}

// Function to print the board
void print_board() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            printf("%c ", board[square]);
        }
        printf("\n");
    }
    printf("\n");
}
