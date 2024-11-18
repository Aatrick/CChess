# CChess

CChess is a simple chess engine written in pure C. It features a basic implementation of the minimax algorithm with alpha-beta pruning to evaluate and generate the best moves for the bot. The engine supports standard chess rules and can play against a human player.

## Features

- Basic chess engine written in C
- Minimax algorithm with alpha-beta pruning for move generation
- Evaluation function to assess board positions
- Supports standard chess rules including castling and pawn promotion
- Command-line interface for playing against the bot

## Getting Started

### Prerequisites

- GCC (GNU Compiler Collection)

### Building the Project

To build the project, navigate to the project directory and run the following command:

```sh
gcc -O3 -I/include/util.h ./src/main.c ./src/board.c -o ./build/release && ./build/release
```