#pragma once

#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#define BOARD_LENGTH 28
#define BOARD_HEIGHT 31

typedef enum
{
	DoubleCorner,
	SingleCorner,
	Middle,
	Hallway,
	TLeft,
	TRight,
	PenCorner,
	PenSide
} SquareType;

typedef struct
{
	int x, y;
	bool walkable;
	SDL_Surface *image;
} BoardSquare;

typedef struct
{
	BoardSquare boardSquares[BOARD_LENGTH][BOARD_HEIGHT];
} Board;

bool is_valid_square(Board *board, int x, int y);

//TODO: decide if this uses the board or not
//Returns true if the square is a teleport square
bool is_tele_square(int x, int y);

//Returns true if this is a square in which ghosts cannot move Up to enter in Chase mode
bool is_ghost_noup_square(int x, int y);
