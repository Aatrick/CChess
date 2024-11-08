#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/board.h"

//cd "/home/aatricks/Documents/CChess/" && gcc -I/include/util.h ./src/main.c  ./src/board.c  -o ./build/debug && "/home/aatricks/Documents/CChess/build/"debug

int main(){
    print_bitboard("A1", "A3");
    return 0;
}