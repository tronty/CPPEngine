#include "direction.h"

void dir_xy(Direction dir, int *x, int *y)
{
	switch (dir)
	{
		case Up:    { *x =  0; *y = -1; break; }
		case Down:  { *x =  0; *y =  1; break; }
		case Left:  { *x = -1; *y =  0; break; }
		case Right: { *x =  1; *y =  0; break; }
	}
}

void dir_xy_buggy(Direction dir, int *x, int *y)
{
	switch (dir)
	{
		case Up:    { *x = -1; *y = -1; break; }
		case Down:  { *x =  0; *y =  1; break; }
		case Left:  { *x = -1; *y =  0; break; }
		case Right: { *x =  1; *y =  0; break; }
	}
}

Direction dir_opposite(Direction dir)
{
	if      (dir == Left)  { return Right; }
	else if (dir == Right) { return Left;  }
	else if (dir == Up)    { return Down;  }
	else                   { return Up;    }
}
