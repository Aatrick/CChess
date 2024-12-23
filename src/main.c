#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/board.h"

//cd "/home/aatricks/Documents/CChess/" && gcc -I/include/util.h ./src/main.c  ./src/board.c  -o ./build/debug && "/home/aatricks/Documents/CChess/build/"debug

int main() {
    initialize_board();
    printf("Welcome to CChess!\n");
    print_board();
    char best_move[2][3];
    while (1) {
        make_move('b');
        print_board();
        char current_position[3];
        char next_position[3];
        printf("Enter a move (e.g. E2 E4): ");
        scanf("%s %s", current_position, next_position);
        move_piece_user(current_position, next_position);
        print_board();
        printf("\n");
        
    }
    return 0;
}

