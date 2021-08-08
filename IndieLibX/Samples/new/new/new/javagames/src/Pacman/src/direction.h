#pragma once

//Defines the four cardinal directions.
typedef enum
{
	Up,
	Down,
	Left,
	Right
} Direction;

//Defines the four ordinal directions.
typedef enum
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
} DiagDirection;

//Copies -1, 0, 1 to the given x and y ints depending on what direction is given.
void dir_xy(Direction dir, int *x, int *y);

//Same behaviour as dir_xy() except this emulates the bug for ghosts where
//AI based on up direction is both Up *and* Left.
void dir_xy_buggy(Direction dir, int *x, int *y);

//Returns the opposite direction to the one given.
Direction dir_opposite(Direction dir);
