#include <stdio.h>
#include <stdlib.h>

#include "board.h"

bool is_valid_square(Board *board, int x, int y)
{
	if (x < 0 || y < 0) return false;
	if (x >= BOARD_LENGTH || y >= BOARD_HEIGHT) return false;

	return board->boardSquares[x][y].walkable;
}

bool is_tele_square(int x, int y)
{
	if (x == -1 && y == 14) return true;
	else if (x == 28 && y == 14) return true;
	else return false;
}

bool is_ghost_noup_square(int x, int y)
{
	//squares are:
	// (12, 22) (14, 22)
	// (12, 10) (14, 10)
	return (x == 12 && y == 22) || (x == 15 && y == 22) ||
	       (x == 12 && y == 10) || (x == 15 && y == 10);
}
