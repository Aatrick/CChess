#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
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

// Track if the king and rooks have moved
int white_king_moved = 0;
int white_rook_kingside_moved = 0;
int white_rook_queenside_moved = 0;
int black_king_moved = 0;
int black_rook_kingside_moved = 0;
int black_rook_queenside_moved = 0;


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
    place_pieces(board, white_knights, 'N');
    place_pieces(board, white_bishops, 'B');
    place_pieces(board, white_rooks, 'R');
    place_pieces(board, white_queens, 'Q');
    place_pieces(board, white_kings, 'K');

    place_pieces(board, black_pawns, 'p');
    place_pieces(board, black_knights, 'n');
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

int val_in_array(int val, int *arr, size_t n){
    for (size_t i = 0; i<n; i++){
        if(arr[i] == val){
            return 1;
        }
    }
    return 0;
}

// Function to check if a square is under attack
int is_square_under_attack(int square, char attacking_side) {
    int rank = square / 8;
    int file = square % 8;

    // Check for pawn attacks
    if (attacking_side == 'w') {
        if (rank > 0 && file > 0 && board[63 - (square - 9)] == 'p') return 1;
        if (rank > 0 && file < 7 && board[63 - (square - 7)] == 'p') return 1;
    } else {
        if (rank < 7 && file > 0 && board[63 - (square + 7)] == 'P') return 1;
        if (rank < 7 && file < 7 && board[63 - (square + 9)] == 'P') return 1;
    }

    // Check for knight attacks
    int knight_moves[] = {17, 15, 10, 6, -6, -10, -15, -17};
    for (int i = 0; i < 8; i++) {
        int target_square = square + knight_moves[i];
        if (target_square >= 0 && target_square < 64) {
            if (attacking_side == 'w' && board[63 - target_square] == 'n') return 1;
            if (attacking_side == 'b' && board[63 - target_square] == 'N') return 1;
        }
    }
    // Check for bishop/queen attacks (diagonals)
    int bishop_moves[] = {9, 7, -7, -9};
    for (int i = 0; i < 4; i++) {
        int target_square = square;
        while (1) {
            target_square += bishop_moves[i];
            if (target_square < 0 || target_square >= 64) break;
            if (board[63 - target_square] != '.') {
                if (attacking_side == 'w' && (board[63 - target_square] == 'b' || board[63 - target_square] == 'q')) return 1;
                if (attacking_side == 'b' && (board[63 - target_square] == 'B' || board[63 - target_square] == 'Q')) return 1;
                break;
            }
        }
    }

    // Check for rook/queen attacks (files and ranks)
    int rook_moves[] = {8, -8, 1, -1};
    for (int i = 0; i < 4; i++) {
        int target_square = square;
        while (1) {
            target_square += rook_moves[i];
            if (target_square < 0 || target_square >= 64) break;
            if (board[63 - target_square] != '.') {
                if (attacking_side == 'w' && (board[63 - target_square] == 'r' || board[63 - target_square] == 'q')) return 1;
                if (attacking_side == 'b' && (board[63 - target_square] == 'R' || board[63 - target_square] == 'Q')) return 1;
                break;
            }
        }
    }
    // Check for king attacks
    int king_moves[] = {1, -1, 8, -8, 9, 7, -7, -9};
    for (int i = 0; i < 8; i++) {
        int target_square = square + king_moves[i];
        if (target_square >= 0 && target_square < 64) {
            if (attacking_side == 'w' && board[63 - target_square] == 'k') return 1;
            if (attacking_side == 'b' && board[63 - target_square] == 'K') return 1;
        }
    }

    return 0;
}

// Function to check if the king is in check
int is_king_in_check(char king) {
    // you would need to check all possible attacks from the opponent's pieces
    return 0;
}

// Function to move a piece on the board
void move_piece(const char* current_position, const char* next_position) {
    int current_index = chess_to_bitboard_index(current_position);
    int next_index = chess_to_bitboard_index(next_position);
    if (current_index != -1 && next_index != -1) {//let's play by the rules
        if (isupper(board[63 - next_index]) && isupper(board[63 - current_index]) || islower(board[63 - next_index]) && islower(board[63 - current_index])){
            printf("illegal move\n\n");
        }
        else {
            // PAWNS
            if (board[63 - current_index] == 'p' || board[63 - current_index] == 'P') {
                int end_line[] = {63,62,61,60,59,58,57,56,0,1,2,3,4,5,6,7};
                if (board[63 - next_index] != '.' && ((63-next_index == (63-current_index) - 7 || 63-next_index == (63-current_index) - 9 || 63-next_index == (63-current_index) + 7 || 63-next_index == (63-current_index) + 9))){
                    if (val_in_array(next_index, end_line, 16)){
                        if (board[63 - current_index] == 'p'){
                            board[63 - next_index] = 'q';
                        }
                        else {
                            board[63 - next_index] = 'Q';
                        }
                        board[63 - current_index] = '.';
                    }
                    else {
                        board[63 - next_index] = board[63 - current_index];
                        board[63 - current_index] = '.';
                    }
                }
                if (board[63 - next_index] == '.' && ((63-next_index == (63-current_index) - 8) || (63-next_index == (63-current_index) + 8))){
                    if (val_in_array(next_index, end_line, 16)){
                        if (board[63 - current_index] == 'p'){
                            board[63 - next_index] = 'q';
                        }
                        else {
                            board[63 - next_index] = 'Q';
                        }
                        board[63 - current_index] = '.';
                    }
                    else {
                        board[63 - next_index] = board[63 - current_index];
                        board[63 - current_index] = '.';
                    }
                }
                int starters[] = {8,9,10,11,12,13,14,15,55,54,53,52,51,50,49,48};
                if ((val_in_array(current_index, starters, 16) == 1) && board[63 - next_index] == '.' && ((63-next_index == (63-current_index) - 16) || (63-next_index == (63-current_index) + 16))){
                    board[63 - next_index] = board[63 - current_index];
                    board[63 - current_index] = '.';
                }
                else {
                    printf("Illegal move\n\n");
                }
            }
            // ROOKS
            if (board[63 - current_index] == 'r' || board[63 - current_index] == 'R'){
                int vertical[] = {current_index + 8, current_index + 16, current_index + 24, current_index + 32, current_index + 40, current_index + 48, current_index + 56, current_index + 64, current_index - 8, current_index - 16, current_index - 24, current_index - 32, current_index - 40, current_index - 48, current_index - 56, current_index - 64};
                int horizontal[] = {current_index + 8, current_index + 1, current_index + 2, current_index + 3, current_index + 4, current_index + 5, current_index + 6, current_index + 7, current_index - 8, current_index - 1, current_index - 2, current_index - 3, current_index - 4, current_index - 5, current_index - 6, current_index - 7};
                if (val_in_array(next_index, vertical, 16) == 1 || val_in_array(next_index, horizontal, 16)){
                    int shift_curr = (current_index % 8);
                    int shift_next = (next_index % 8);
                    int all_g = 0;
                    if (current_index < next_index){
                        for (int i = current_index + 8; i < next_index; i += 8){
                            if (board[63 - i] != '.'){
                                all_g = 1;
                            }
                        }
                    }
                    if (shift_next > shift_curr){
                            for (int i = current_index + 1; i < next_index; i++){
                                if (board[63 - i] != '.'){
                                    all_g = 1;
                                }
                            }
                        }
                    if (shift_next < shift_curr){
                        for (int i = current_index - 1; i < next_index; i--){
                            if (board[63 - i] != '.'){
                                all_g = 1;
                            }
                        }
                    }
                    if (current_index > next_index) {
                        for (int i = current_index - 8; i > next_index; i -= 8){
                            if (board[63 - i] != '.'){
                                all_g = 1;
                            }
                        }
                    }
                    if (all_g == 0){
                        board[63 - next_index] = board[63 - current_index];
                        board[63 - current_index] = '.';
                    }
                    else {
                        printf("Illegal move\n\n");
                    }
                }
            }
            // KNIGHTS
            if (board[63 - current_index] == 'n' || board[63 - current_index] == 'N'){
                int circle[] = {current_index + 17, current_index + 15, current_index - 17, current_index - 15, current_index + 6, current_index - 6, current_index + 10, current_index - 10};
                if (val_in_array(next_index, circle, 8) == 1){
                    board[63 - next_index] = board[63 - current_index];
                    board[63 - current_index] = '.';
                } else {
                    printf("Illegal move\n\n");
                }
            }
            // BISHOPS
            if (board[63 - current_index] == 'b' || board[63 - current_index] == 'B'){
                int left_top[] = {current_index + 9, current_index + 18, current_index + 27, current_index + 36, current_index + 45, current_index + 54, current_index + 63};
                int right_top[] = {current_index + 7, current_index + 14, current_index + 21, current_index + 28, current_index + 35, current_index + 42, current_index + 49, current_index + 56, current_index + 63};
                int left_bot[] = {current_index - 7, current_index - 14, current_index - 21, current_index - 28, current_index - 35, current_index - 42, current_index - 49, current_index - 56, current_index - 63};
                int right_bot[] = {current_index - 9, current_index - 18, current_index - 27, current_index - 36, current_index - 45, current_index - 54, current_index - 63};
                
                if (val_in_array(next_index, left_top, 7) == 1 || val_in_array(next_index, right_top, 9) == 1 || val_in_array(next_index, left_bot, 9) == 1 || val_in_array(next_index, right_bot, 7)){
                    int shift_curr = (current_index % 8);
                    int shift_next = (next_index % 8);
                    int all_g = 0; // if a piece is in the way, this will be 1
                    if (current_index < next_index){
                        if (shift_next > shift_curr){
                            for (int i = current_index + 9; i < next_index; i += 9){
                                if (board[63 - i] != '.'){
                                    all_g = 1;
                                }
                            }
                        } if (shift_next < shift_curr){
                            for (int i = current_index + 7; i < next_index; i += 7){
                                if (board[63 - i] != '.'){
                                    all_g = 1;
                                }
                            }
                        }
                    } else {
                        if (shift_next > shift_curr){
                            for (int i = current_index - 7; i > next_index; i -= 7){
                                if (board[63 - i] != '.'){
                                    all_g = 1;
                                }
                            }
                        } if (shift_next < shift_curr){
                            for (int i = current_index - 9; i > next_index; i -= 9){
                                if (board[63 - i] != '.'){
                                    all_g = 1;
                                }
                            }
                        }
                    } if (all_g == 0){
                        board[63 - next_index] = board[63 - current_index];
                        board[63 - current_index] = '.';
                    } else {
                    printf("Illegal move\n\n");
                    }
                } else {
                    printf("Illegal move\n\n");
                }
            }
            // QUEEN
            if (board[63 - current_index] == 'q' || board[63 - current_index] == 'Q') {
                int vertical[] = {current_index + 8, current_index + 16, current_index + 24, current_index + 32, current_index + 40, current_index + 48, current_index + 56, current_index + 64, current_index - 8, current_index - 16, current_index - 24, current_index - 32, current_index - 40, current_index - 48, current_index - 56, current_index - 64};
                int horizontal[] = {current_index + 1, current_index + 2, current_index + 3, current_index + 4, current_index + 5, current_index + 6, current_index + 7, current_index - 1, current_index - 2, current_index - 3, current_index - 4, current_index - 5, current_index - 6, current_index - 7};
                int left_top[] = {current_index + 9, current_index + 18, current_index + 27, current_index + 36, current_index + 45, current_index + 54, current_index + 63};
                int right_top[] = {current_index + 7, current_index + 14, current_index + 21, current_index + 28, current_index + 35, current_index + 42, current_index + 49, current_index + 56, current_index + 63};
                int left_bot[] = {current_index - 7, current_index - 14, current_index - 21, current_index - 28, current_index - 35, current_index - 42, current_index - 49, current_index - 56, current_index - 63};
                int right_bot[] = {current_index - 9, current_index - 18, current_index - 27, current_index - 36, current_index - 45, current_index - 54, current_index - 63};

                if (val_in_array(next_index, vertical, 16) == 1 || val_in_array(next_index, horizontal, 16) == 1 || val_in_array(next_index, left_top, 7) == 1 || val_in_array(next_index, right_top, 9) == 1 || val_in_array(next_index, left_bot, 9) == 1 || val_in_array(next_index, right_bot, 7)) {
                    int shift_curr = (current_index % 8);
                    int shift_next = (next_index % 8);
                    int all_g = 0;

                    // Check vertical movement
                    if (current_index < next_index && shift_curr == shift_next) {
                        for (int i = current_index + 8; i < next_index; i += 8) {
                            if (board[63 - i] != '.') {
                                all_g = 1;
                                break;
                            }
                        }
                    } else if (current_index > next_index && shift_curr == shift_next) {
                        for (int i = current_index - 8; i > next_index; i -= 8) {
                            if (board[63 - i] != '.') {
                                all_g = 1;
                                break;
                            }
                        }
                    }

                    // Check horizontal movement
                    if (shift_next > shift_curr && current_index / 8 == next_index / 8) {
                        for (int i = current_index + 1; i < next_index; i++) {
                            if (board[63 - i] != '.') {
                                all_g = 1;
                                break;
                            }
                        }
                    } else if (shift_next < shift_curr && current_index / 8 == next_index / 8) {
                        for (int i = current_index - 1; i > next_index; i--) {
                            if (board[63 - i] != '.') {
                                all_g = 1;
                                break;
                            }
                        }
                    }

                    // Check diagonal movement
                    if (current_index < next_index) {
                        if (shift_next > shift_curr) {
                            for (int i = current_index + 9; i < next_index; i += 9) {
                                if (board[63 - i] != '.') {
                                    all_g = 1;
                                    break;
                                }
                            }
                        } else if (shift_next < shift_curr) {
                            for (int i = current_index + 7; i < next_index; i += 7) {
                                if (board[63 - i] != '.') {
                                    all_g = 1;
                                    break;
                                }
                            }
                        }
                    } else {
                        if (shift_next > shift_curr) {
                            for (int i = current_index - 7; i > next_index; i -= 7) {
                                if (board[63 - i] != '.') {
                                    all_g = 1;
                                    break;
                                }
                            }
                        } else if (shift_next < shift_curr) {
                            for (int i = current_index - 9; i > next_index; i -= 9) {
                                if (board[63 - i] != '.') {
                                    all_g = 1;
                                    break;
                                }
                            }
                        }
                    }

                    if (all_g == 0) {
                        board[63 - next_index] = board[63 - current_index];
                        board[63 - current_index] = '.';
                    } else {
                        printf("Illegal move\n\n");
                    }
                } else {
                    printf("Illegal move\n\n");
                }
            }
            // KING
            if (board[63 - current_index] == 'k' || board[63 - current_index] == 'K'){// TODO: Rock the king
                int circle[] = {current_index + 1, current_index - 1, current_index + 8, current_index + 9, current_index + 7, current_index - 9, current_index - 8, current_index - 7};
                if (val_in_array(next_index, circle, 8) == 1){
                    board[63 - next_index] = board[63 - current_index];
                    board[63 - current_index] = '.';
                } // King side castling
                if (current_index == chess_to_bitboard_index("E8") && next_index == chess_to_bitboard_index("G8") && board[63 - current_index] == 'k' && board[63 - next_index] == '.' && board[63 - chess_to_bitboard_index("F8")] == '.' && board[63 - chess_to_bitboard_index("H8")] == 'r' && black_king_moved == 0 && black_rook_kingside_moved == 0 && is_square_under_attack(chess_to_bitboard_index("F8"), 'w') == 0 && is_square_under_attack(chess_to_bitboard_index("G8"), 'w') == 0){
                    board[63 - next_index] = board[63 - current_index];
                    board[63 - current_index] = '.';
                    board[63 - chess_to_bitboard_index("F8")] = board[63 - chess_to_bitboard_index("H8")];
                    board[63 - chess_to_bitboard_index("H8")] = '.';
                    black_king_moved = 1;
                    black_rook_kingside_moved = 1;
                } // Queen side castling
                if (current_index == chess_to_bitboard_index("E8") && next_index == chess_to_bitboard_index("C8") && board[63 - current_index] == 'k' && board[63 - next_index] == '.' && board[63 - chess_to_bitboard_index("D8")] == '.' && board[63 - chess_to_bitboard_index("A8")] == 'r' && board[63 - chess_to_bitboard_index("B8")] == '.' && black_king_moved == 0 && black_rook_queenside_moved == 0, is_square_under_attack(chess_to_bitboard_index("D8"), 'w') == 0 && is_square_under_attack(chess_to_bitboard_index("C8"), 'w') == 0){
                    board[63 - next_index] = board[63 - current_index];
                    board[63 - current_index] = '.';
                    board[63 - chess_to_bitboard_index("D8")] = board[63 - chess_to_bitboard_index("A8")];
                    board[63 - chess_to_bitboard_index("A8")] = '.';
                    black_king_moved = 1;
                    black_rook_queenside_moved = 1;
                } // King side castling
                if (current_index == chess_to_bitboard_index("E1") && next_index == chess_to_bitboard_index("G1") && board[63 - current_index] == 'K' && board[63 - next_index] == '.' && board[63 - chess_to_bitboard_index("F1")] == '.' && board[63 - chess_to_bitboard_index("H1")] == 'R' && white_king_moved == 0 && white_rook_kingside_moved == 0 && is_square_under_attack(chess_to_bitboard_index("F1"), 'b') == 0 && is_square_under_attack(chess_to_bitboard_index("G1"), 'b') == 0){
                    board[63 - next_index] = board[63 - current_index];
                    board[63 - current_index] = '.';
                    board[63 - chess_to_bitboard_index("F1")] = board[63 - chess_to_bitboard_index("H1")];
                    board[63 - chess_to_bitboard_index("H1")] = '.';
                    white_king_moved = 1;
                    white_rook_kingside_moved = 1;
                } // Queen side castling
                if (current_index == chess_to_bitboard_index("E1") && next_index == chess_to_bitboard_index("C1") && board[63 - current_index] == 'K' && board[63 - next_index] == '.' && board[63 - chess_to_bitboard_index("D1")] == '.' && board[63 - chess_to_bitboard_index("A1")] == 'R' && board[63 - chess_to_bitboard_index("B1")] == '.' && white_king_moved == 0 && white_rook_queenside_moved == 0, is_square_under_attack(chess_to_bitboard_index("D1"), 'b') == 0 && is_square_under_attack(chess_to_bitboard_index("C1"), 'b') == 0){
                    board[63 - next_index] = board[63 - current_index];
                    board[63 - current_index] = '.';
                    board[63 - chess_to_bitboard_index("D1")] = board[63 - chess_to_bitboard_index("A1")];
                    board[63 - chess_to_bitboard_index("A1")] = '.';
                    white_king_moved = 1;
                    white_rook_queenside_moved = 1;
                } else {
                    printf("Illegal move\n\n");
                }
                else {
                    printf("Illegal move\n\n");
                }
            }
        }
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
