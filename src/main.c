#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/board.h"

//cd "/home/aatricks/Documents/CChess/" && gcc -I/include/util.h ./src/main.c  ./src/board.c  -o ./build/debug && "/home/aatricks/Documents/CChess/build/"debug

int main(){
    uint64_t bitboard = 0x0000000000000000;
    print_bitboard(bitboard);
    return 0;
}