#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/board.h"
#include "../include/generator.h"

//cd "/home/aatricks/Documents/CChess/" && gcc -I/include/util.h ./src/main.c  ./src/board.c  -o ./build/debug && "/home/aatricks/Documents/CChess/build/"debug

int main() {
    initialize_board();
    printf("Welcome to CChess!\n");
    print_board();
    while (1) {
        char current_position[3];
        char next_position[3];
        printf("Enter a move (e.g. E2 E4): ");
        scanf("%s %s", current_position, next_position);
        move_piece(current_position, next_position);
        print_board();
        printf("\n");
        printf("Evaluation: %d\n", evaluate('w'));
    }
    return 0;
}

