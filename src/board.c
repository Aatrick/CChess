#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stddef.h>
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
char temp_board[64];
char board_memory[64];

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

// Function to check if a square is under attack
// Helper function to check for pawn attacks
int is_pawn_attack(int square, char attacking_side) {
    int rank = square / 8;
    int file = square % 8;

    if (attacking_side == 'w') {
        if (rank > 0 && file > 0 && board[63 - (square - 9)] == 'p') return 1;
        if (rank > 0 && file < 7 && board[63 - (square - 7)] == 'p') return 1;
    } else {
        if (rank < 7 && file > 0 && board[63 - (square + 7)] == 'P') return 1;
        if (rank < 7 && file < 7 && board[63 - (square + 9)] == 'P') return 1;
    }
    return 0;
}

// Helper function to check for knight attacks
int is_knight_attack(int square, char attacking_side) {
    int knight_moves[] = {17, 15, 10, 6, -6, -10, -15, -17};
    for (int i = 0; i < 8; i++) {
        int target_square = square + knight_moves[i];
        if (target_square >= 0 && target_square < 64) {
            if (attacking_side == 'w' && board[63 - target_square] == 'n') return 1;
            if (attacking_side == 'b' && board[63 - target_square] == 'N') return 1;
        }
    }
    return 0;
}

// Helper function to check for bishop or queen attacks (diagonals)
int is_bishop_or_queen_attack(int square, char attacking_side) {
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
    return 0;
}

// Helper function to check for rook or queen attacks (files and ranks)
int is_rook_or_queen_attack(int square, char attacking_side) {
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
    return 0;
}

// Helper function to check for king attacks
int is_king_attack(int square, char attacking_side) {
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

// Function to check if a square is under attack
int is_square_under_attack(int square, char attacking_side) {
    if (is_pawn_attack(square, attacking_side)) return 1;
    if (is_knight_attack(square, attacking_side)) return 1;
    if (is_bishop_or_queen_attack(square, attacking_side)) return 1;
    if (is_rook_or_queen_attack(square, attacking_side)) return 1;
    if (is_king_attack(square, attacking_side)) return 1;
    return 0;
}

// Helper function to check if a value is in an array
int val_in_array(int val, int *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (arr[i] == val) {
            return 1;
        }
    }
    return 0;
}

// Helper function to check if two pieces are of the same color
int is_same_color(char piece1, char piece2) {
    return (isupper(piece1) && isupper(piece2)) || (islower(piece1) && islower(piece2));
}

// Helper function to check if the path is clear for sliding pieces (rook, bishop, queen)
int is_path_clear(int current_index, int next_index, const char* board, int step) {
    for (int i = current_index + step; i != next_index; i += step) {
        if (board[63 - i] != '.') {
            return 0;
        }
    }
    return 1;
}

// Helper function to validate pawn moves
int is_valid_pawn_move(int current_index, int next_index, const char* board) { // TODO: fix pawn going through pieces, en passant, promotion
    int end_line[] = {63, 62, 61, 60, 59, 58, 57, 56, 0, 1, 2, 3, 4, 5, 6, 7};
    int starters[] = {8, 9, 10, 11, 12, 13, 14, 15, 55, 54, 53, 52, 51, 50, 49, 48};

    if (board[63 - next_index] != '.' && 
        (63 - next_index == (63 - current_index) - 7 || 63 - next_index == (63 - current_index) - 9 || 
         63 - next_index == (63 - current_index) + 7 || 63 - next_index == (63 - current_index) + 9)) {
        return 1;
    } else if (board[63 - next_index] == '.' && 
               (63 - next_index == (63 - current_index) - 8 || 63 - next_index == (63 - current_index) + 8)) {
        return 1;
    } else if (val_in_array(current_index, starters, 16) && board[63 - next_index] == '.' && 
               (63 - next_index == (63 - current_index) - 16 || 63 - next_index == (63 - current_index) + 16)) {
        return 1;
    } else if (val_in_array(next_index, end_line, 16) && board[63 - next_index] == '.' && 
               (63 - next_index == (63 - current_index) - 8 || 63 - next_index == (63 - current_index) + 8)) {
        return 1;
    } else {
        return 0;
    }
    
}

// Helper function to validate rook moves
int is_valid_rook_move(int current_index, int next_index, const char* board) {
    int vertical[] = {8, 16, 24, 32, 40, 48, 56, 64, -8, -16, -24, -32, -40, -48, -56, -64};
    int horizontal[] = {1, 2, 3, 4, 5, 6, 7, -1, -2, -3, -4, -5, -6, -7};

    if (val_in_array(next_index - current_index, vertical, 16)) {
        return is_path_clear(current_index, next_index, board, 8) || is_path_clear(current_index, next_index, board, -8);
    } else if (val_in_array(next_index - current_index, horizontal, 16)) {
        return is_path_clear(current_index, next_index, board, 1) || is_path_clear(current_index, next_index, board, -1);
    } else {
        return 0;
    }
}

// Helper function to validate knight moves
int is_valid_knight_move(int current_index, int next_index) {
    int circle[] = {17, 15, -17, -15, 6, -6, 10, -10};
    return val_in_array(next_index - current_index, circle, 8);
}

// Helper function to validate bishop moves
int is_valid_bishop_move(int current_index, int next_index, const char* board) {
    int left_top[] = {9, 18, 27, 36, 45, 54, 63};
    int right_top[] = {7, 14, 21, 28, 35, 42, 49, 56, 63};
    int left_bot[] = {-7, -14, -21, -28, -35, -42, -49, -56, -63};
    int right_bot[] = {-9, -18, -27, -36, -45, -54, -63};

    if (val_in_array(next_index - current_index, left_top, 7)) {
        return is_path_clear(current_index, next_index, board, 9);
    } else if (val_in_array(next_index - current_index, right_top, 9)) {
        return is_path_clear(current_index, next_index, board, 7);
    } else if (val_in_array(next_index - current_index, left_bot, 9)) {
        return is_path_clear(current_index, next_index, board, -7);
    } else if (val_in_array(next_index - current_index, right_bot, 7)) {
        return is_path_clear(current_index, next_index, board, -9);
    } else {
        return 0;
    }
}

// Helper function to validate queen moves
int is_valid_queen_move(int current_index, int next_index, const char* board) {
    return is_valid_rook_move(current_index, next_index, board) || is_valid_bishop_move(current_index, next_index, board);
}

// Helper function to validate king moves
// TODO: castling
int is_valid_king_move(int current_index, int next_index, const char* board) {
    int king_moves[] = {1, -1, 8, -8, 9, 7, -7, -9};
    return val_in_array(next_index - current_index, king_moves, 8);
}

int is_legal_move(const char* current_position, const char* next_position, const char* board) {
    int current_index = chess_to_bitboard_index(current_position);
    int next_index = chess_to_bitboard_index(next_position);
    if (current_index == -1 || next_index == -1) {
        return 0;
    }

    if (is_same_color(board[63 - current_index], board[63 - next_index])) {
        return 0;
    }

    char piece = board[63 - current_index];
    switch (tolower(piece)) {
        case 'p':
            return is_valid_pawn_move(current_index, next_index, board);
        case 'r':
            return is_valid_rook_move(current_index, next_index, board);
        case 'n':
            return is_valid_knight_move(current_index, next_index);
        case 'b':
            return is_valid_bishop_move(current_index, next_index, board);
        case 'q':
            return is_valid_queen_move(current_index, next_index, board);
        case 'k':
            return is_valid_king_move(current_index, next_index, board);
        default:
            return 0;
    }
}

void move_piece(const char* current_position, const char* next_position, char board[]) {
    int current_index = chess_to_bitboard_index(current_position);
    int next_index = chess_to_bitboard_index(next_position);
    if (current_index != -1 && next_index != -1) {// promotion
        int end_line[] = {63, 62, 61, 60, 59, 58, 57, 56, 0, 1, 2, 3, 4, 5, 6, 7};
        if (board[63 - current_index] == 'P' && next_index < 8 && val_in_array(next_index, end_line, 16)) {
            board[63 - next_index] = 'Q';
            board[63 - current_index] = '.';
        } else if (board[63 - current_index] == 'p' && next_index > 55 && val_in_array(next_index, end_line, 16)) {
            board[63 - next_index] = 'q';
            board[63 - current_index] = '.';
        }
        if (is_legal_move(current_position, next_position, board)) {
            board[63 - next_index] = board[63 - current_index];
            board[63 - current_index] = '.';
        } else {
            printf("Illegal move\n\n");
        }
    }
}

void move_piece_user(const char* current_position, const char* next_position){
    move_piece(current_position, next_position, board);
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

void print_temp_board() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            printf("%c ", temp_board[square]);
        }
        printf("\n");
    }
    printf("\n");
}

// Helper function to convert a board index to a chess notation string
void index_to_chess_notation(int index, char* notation) {
    int rank = index / 8;
    int file = index % 8;
    notation[0] = 'A' + file;
    notation[1] = '1' + rank;
    notation[2] = '\0';
}

int min(int a, int b){
    if (a < b){
        return a;
    }
    else {
        return b;
    }
}

int max(int a, int b){
    if (a > b){
        return a;
    }
    else {
        return b;
    }
}

// each node is a move and the children are the possible moves
// for each move, make a copy of the board, make the move, evaluate the board, store the move and then undo the move,
// when the best move is found, make the move on the real board

// Helper function to check if a piece belongs to the current side
int is_valid_piece(char piece, char side) {
    if (side == 'w') {
        return piece == 'P' || piece == 'N' || piece == 'B' || piece == 'R' || piece == 'Q' || piece == 'K';
    } else {
        return piece == 'p' || piece == 'n' || piece == 'b' || piece == 'r' || piece == 'q' || piece == 'k';
    }
}

// Helper function to check if a target square is valid for a move
int is_valid_target(char target, char side) {
    if (side == 'w') {
        return target == '.' || islower(target);
    } else {
        return target == '.' || isupper(target);
    }
}

int minimax(int depth, int alpha, int beta, char side);

void copy_board(){
    for (int i = 0; i<64; i++){
        temp_board[i]=board[i];
    }
}

void copy_temp_board(){
    for (int i = 0; i<64; i++){
        board[i]=temp_board[i];
    }
}

void reset_to_memory(){
    for (int i = 0; i<64; i++){
        board[i]=board_memory[i];
    }
}

void update_board_memory(){
    for (int i = 0; i<64; i++){
        board_memory[i]=board[i];
    }
}


int piece_values[128] = {0};
int piece_square_table[128][64] = {0};

// Initialize piece values and piece-square tables
void initialize_evaluation() {
    piece_values['P'] = 100;
    piece_values['N'] = 320;
    piece_values['B'] = 330;
    piece_values['R'] = 500;
    piece_values['Q'] = 900;
    piece_values['K'] = 20000;
    piece_values['p'] = 100;
    piece_values['n'] = 320;
    piece_values['b'] = 330;
    piece_values['r'] = 500;
    piece_values['q'] = 900;
    piece_values['k'] = 20000;

    // Piece-square tables for positional bonuses, symmetrically mirrored
    int pawn_table[] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    int knight_table[] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };

    int bishop_table[] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };

    int rook_table[] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
       -5,  0,  0,  0,  0,  0,  0, -5,
       -5,  0,  0,  0,  0,  0,  0, -5,
       -5,  0,  0,  0,  0,  0,  0, -5,
       -5,  0,  0,  0,  0,  0,  0, -5,
       -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0
    };

    int queen_table[] = {
       -20,-10,-10, -5, -5,-10,-10,-20,
       -10,  0,  0,  0,  0,  0,  0,-10,
       -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
         0,  0,  5,  5,  5,  5,  0, -5,
       -10,  5,  5,  5,  5,  5,  0,-10,
       -10,  0,  5,  0,  0,  0,  0,-10,
       -20,-10,-10, -5, -5,-10,-10,-20
    };

    int king_table[] = {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10,  0,   0,  0,  0,   0,   0, -10,
        -10,  0,   5,  10,  10,   5,   0, -10,
        -10,  5,   5,  10,  10,   5,   5, -10,
        -10,  0,   10, 10,  10,  10,   0, -10,
        -10,  10, 10, 10,  10,  10,  10, -10,
        -10,  5,  0,  0,  0,  0,  5, -10,
        -20, -10, -10, -5, -5, -10, -10, -20
    };


    for (int i = 0; i < 64; i++) {
        piece_square_table['p'][i] = pawn_table[i];
        piece_square_table['n'][i] = knight_table[i];
        piece_square_table['b'][i] = bishop_table[i];
        piece_square_table['r'][i] = rook_table[i];
        piece_square_table['q'][i] = queen_table[i];
        piece_square_table['k'][i] = king_table[i];
        piece_square_table['P'][i] = pawn_table[63 - i];
        piece_square_table['N'][i] = knight_table[63 - i];
        piece_square_table['B'][i] = bishop_table[63 - i];
        piece_square_table['R'][i] = rook_table[63 - i];
        piece_square_table['Q'][i] = queen_table[63 - i];
        piece_square_table['K'][i] = king_table[63 - i];
    }
}

int evaluate(const char side, const char* board) {
    int score_white = 0;
    int score_black = 0;

    // Calculate the total score for both sides
    for (int i = 0; i < 64; i++) {
        char piece = board[i];
        if (piece >= 'A' && piece <= 'Z') {
            score_white += piece_values[piece];
            score_white += piece_square_table[piece][i];
        } else if (piece >= 'a' && piece <= 'z') {
            score_black += piece_values[piece];
            score_black += piece_square_table[piece][i];
        }
    }

    // Add bonuses for pieces protecting other pieces
    for (int i = 0; i < 64; i++) {
        char piece = board[i];
        if (piece == '.') continue;

        int rank = i / 8;
        int file = i % 8;
        int directions[] = {1, -1, 8, -8, 9, -9, 7, -7};
        for (int j = 0; j < 8; j++) {
            int target_square = i + directions[j];
            if (target_square >= 0 && target_square < 64) {
                char target_piece = board[target_square];
                if (piece >= 'A' && piece <= 'Z' && target_piece >= 'A' && target_piece <= 'Z') {
                    score_white += 10;
                } else if (piece >= 'a' && piece <= 'z' && target_piece >= 'a' && target_piece <= 'z') {
                    score_black += 10;
                }
            }
        }
    }

    // Add penalties for pieces attacking other pieces
    for (int i = 0; i < 64; i++) {
        char piece = board[i];
        if (piece == '.') continue;

        int rank = i / 8;
        int file = i % 8;
        int directions[] = {1, -1, 8, -8, 9, -9, 7, -7};
        for (int j = 0; j < 8; j++) {
            int target_square = i + directions[j];
            if (target_square >= 0 && target_square < 64) {
                char target_piece = board[target_square];
                if (piece >= 'A' && piece <= 'Z' && target_piece >= 'a' && target_piece <= 'z') {
                    score_white += piece_values[target_piece] / 10;
                } else if (piece >= 'a' && piece <= 'z' && target_piece >= 'A' && target_piece <= 'Z') {
                    score_black += piece_values[target_piece] / 10;
                }
            }
        }
    }

    // Return the difference between the scores of the two sides
    if (side == 'w') {
        return score_white - score_black;
    } else {
        return score_black - score_white;
    }
}


// Helper function to evaluate a move
void initialize_evaluation();

int evaluate_move(int i, int j, int depth, int alpha, int beta, char side) {
    char current_position[3];
    char next_position[3];
    index_to_chess_notation(i, current_position);
    index_to_chess_notation(j, next_position);
    if (is_legal_move(current_position, next_position, board)) {
        copy_board();
        move_piece(current_position, next_position, temp_board);
        int eval = minimax(depth - 1, alpha, beta, (side == 'w') ? 'b' : 'w');
        print_temp_board();
        return eval;
    }
    return (side == 'w') ? -1000000 : 1000000;
}

int minimax(int depth, int alpha, int beta, char side) {
    int current_eval = evaluate(side, temp_board);
    if (depth == 0) {
        return current_eval;
    }

    int best_eval = (side == 'w') ? -1000000 : 1000000;

    for (int i = 0; i < 64; i++) {
        if (is_valid_piece(board[i], side)) {
            for (int j = 0; j < 64; j++) {
                if (is_valid_target(board[j], side)) {
                    int eval = evaluate_move(i, j, depth, alpha, beta, side);
                    if (side == 'w') {
                        best_eval = max(max(best_eval, eval), current_eval);
                        alpha = max(alpha, eval);
                    } else {
                        best_eval = min(best_eval, eval);
                        beta = min(beta, eval);
                    }
                    if (beta <= alpha) {
                        break;
                    }
                    // use current_eval to determine if the move is good
                    if (side == 'w' && eval > current_eval) {
                        return eval;
                    } else if (side == 'b' && eval < current_eval) {
                        return eval;
                    }
                }
            }
        }
    }
    return best_eval;
}

void make_move(char side) {
    char next_position[3];
    char current_position[3];
    int best_eval = (side == 'w') ? -1000000 : 1000000; // Initialize best_eval based on the side
    int best_move[2] = {-1, -1}; // Initialize best_move to invalid positions
    update_board_memory();

    for (int i = 0; i < 64; i++) {
        if (is_valid_piece(board[i], side)) {
            for (int j = 0; j < 64; j++) {
                if (is_valid_target(board[j], side)) {
                    index_to_chess_notation(i, current_position);
                    index_to_chess_notation(j, next_position);
                    if (is_legal_move(current_position, next_position, board)) {
                        printf("Checking move %s to %s\n", current_position, next_position);
                        copy_board();
                        move_piece(current_position, next_position, temp_board);
                        int eval = minimax(4, -1000000, 1000000, (side == 'w') ? 'b' : 'w');
                        copy_temp_board(); // Restore the board from temp_board
                        
                        if ((side == 'w' && eval > best_eval) || (side == 'b' && eval < best_eval)) {
                            best_eval = eval;
                            best_move[0] = i;
                            best_move[1] = j;
                        }
                    }
                }
            }
        }
    }

    if (best_move[0] != -1 && best_move[1] != -1) {
        index_to_chess_notation(best_move[0], current_position);
        index_to_chess_notation(best_move[1], next_position);
        printf("Making move %s to %s\n", current_position, next_position);
        reset_to_memory();
        move_piece(current_position, next_position, board);
    } else {
        printf("No valid moves found\n");
    }
}