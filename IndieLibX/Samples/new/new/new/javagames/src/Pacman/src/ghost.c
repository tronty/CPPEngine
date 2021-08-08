#include "ghost.h"

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

void ghosts_init(Ghost ghosts[4])
{
	ghosts[0].ghostType = Blinky;
	ghosts[1].ghostType = Inky;
	ghosts[2].ghostType = Pinky;
	ghosts[3].ghostType = Clyde;

	for (int i = 0; i < 4; i++)
	{
		reset_ghost(&ghosts[i], ghosts[i].ghostType);
	}
}

void reset_ghost(Ghost *ghost, GhostType type)
{
	int x, y;
	int ox, oy;
	Direction dir;
	Direction next;
	MovementMode mode;

	switch (type)
	{
		case Blinky: { x = 14; y = 11; ox = -8; oy =  0; mode = Scatter; dir = Left; next = Left; break; }
		//case Inky: { x = 12; y = 14; ox = -8; oy =  0; mode = InPen; dir = Up;   next = Down; break; }
		//case Clyde: { x = 16; y = 14; ox = -8; oy =  0; mode = InPen; dir = Up;   next = Down; break; }
		//case Pinky: { x = 14; y = 14; ox = -8; oy =  0; mode = InPen; dir = Down; next = Down; break; }

		//testing
		case Inky:  { x = 14; y = 11; ox = -8; oy =  0; mode = Scatter; dir = Left; next = Left; break; }
		case Clyde: { x = 14; y = 11; ox = -8; oy =  0; mode = Scatter; dir = Left; next = Left; break; }
		case Pinky: { x = 14; y = 11; ox = -8; oy =  0; mode = Scatter; dir = Left; next = Left; break; }

		default: printf("error ghost\naborting\n"); exit(1);
	}

	ghost->body = (PhysicsBody) { x, y, ox, oy, dir, next, 100, 0, 0};
	ghost->body.xOffsetInternal = 0;
	ghost->body.yOffsetInternal = 0;
	ghost->body.velocity = 80;

	ghost->movementMode = mode;
	ghost->movementMode = mode;
	ghost->transDirection = Left;
	ghost->nextDirection = Left;
}

void send_to_home(Ghost *ghost, GhostType type)
{
	int targetX;
	int targetY;

	switch (type)
	{
		case Blinky: { targetX = 25; targetY =  -2; break; }
		case Inky:   { targetX = 27; targetY =  31; break; }
		case Clyde:  { targetX =  0; targetY =  31; break; }
		case Pinky:  { targetX =  2; targetY =  -2; break; }
		default:
			printf("error ghost\naborting\n");
			exit(1);
	}

	ghost->targetX = targetX;
	ghost->targetY = targetY;
}

typedef struct
{
	Direction dir;
	int distance;
} Targetcalc;

typedef struct
{
	int x;
	int y;
} Boardoffset;

Direction next_direction(Ghost *ghost, Board *board)
{
	//ghost has just entered a new tile
	//we look ahead to their next tile
	//then we look at all the surrounding valid tiles
	//we get the distance from each of those to the ghosts target tile
	//we then select the one with the smallest distance
	//if there is a tie, we break it in order Up, Left, Down, Right

	Targetcalc targets[4] =  { {Up, INT_MAX}, {Left, INT_MAX}, {Down, INT_MAX}, {Right, INT_MAX}};
	Boardoffset offsets[4] = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} };

	int x, y;
	dir_xy(ghost->nextDirection, &x, &y);

	//calculate the distances between the squares (or if it is even valid)
	for (int i = 0; i < 4; i++)
	{
		int testX = ghost->body.x + x + offsets[i].x;
		int testY = ghost->body.y + y + offsets[i].y;

		//allow for when ghost is going through teleporter
		if (testX == 0) testX = 26;
		if (testX == 27) testX = 1;

		//make sure the square is a valid walkable square
		if (!(is_valid_square(board, testX, testY) || is_tele_square(testX, testY))) continue;

		//a further condition is that ghosts cannot enter certain squares moving upwards
		if (targets[i].dir == Up && is_ghost_noup_square(testX, testY)) continue;

		int dx = testX - ghost->targetX;
		int dy = testY - ghost->targetY;

		//really the square root, but don't take to keep precision
		targets[i].distance = (dx * dx + dy * dy);
	}

	//a ghost can never turn around, so exclude the opposite direction to what they are facing
	Direction reverseDir = dir_opposite(ghost->nextDirection);

	Targetcalc shortest = {Right, INT_MAX};

	//iterate backwards so that we get the tie-breaking logic for free
	for (int i = 3; i >= 0; i--)
	{
		if (targets[i].dir == reverseDir) continue;

		if (targets[i].distance <= shortest.distance)
		{
			shortest.distance = targets[i].distance;
			shortest.dir = targets[i].dir;
		}
	}

	return shortest.dir;
}

void execute_ghost_logic(Ghost *targetGhost, GhostType type, Ghost *redGhost, Pacman *pacman)
{
	if (targetGhost->movementMode == Scatter)
	{
		send_to_home(targetGhost, type);
		return;
	}

	switch (type)
	{
		case Blinky: execute_red_logic(targetGhost, pacman);            break;
		case Inky:   execute_blue_logic(targetGhost, redGhost, pacman); break;
		case Clyde:  execute_orange_logic(targetGhost, pacman);         break;
		case Pinky:  execute_pink_logic(targetGhost, pacman);           break;
	}
}

void execute_red_logic(Ghost *redGhost, Pacman *pacman)
{
	// Red's AI is to set his target position to pacmans
	redGhost->targetX = pacman->body.x;
	redGhost->targetY = pacman->body.y;
}

void execute_pink_logic(Ghost *pinkGhost, Pacman *pacman)
{
	// Pinks's AI is to set his target position to pacmans, plus a few more in the distance
	int targetOffsetX = 0;
	int targetOffsetY = 0;

	//use dir_xy_buggy to get 4 up AND 4 left, as per bug in original game
	dir_xy_buggy(pacman->body.curDir, &targetOffsetX, &targetOffsetY);

	targetOffsetX *= 4;
	targetOffsetY *= 4;

	pinkGhost->targetX = pacman->body.x + targetOffsetX;
	pinkGhost->targetY = pacman->body.y + targetOffsetY;
}

void execute_orange_logic(Ghost *orangeGhost, Pacman *pacman)
{
	// Orange's logic is in two parts:
	// If Pacmans distance is 8 or more squares away, his target is pacman
	// If Pacman is less than 8 squares, his target is his home

	int dx = orangeGhost->body.x - pacman->body.x;
	int dy = orangeGhost->body.y - pacman->body.y;

	//really the square root, but can compare with square of other number
	int distance = dx * dx + dy * dy;

	if (distance >= (8 * 8))
	{
		execute_red_logic(orangeGhost, pacman);
	}
	else
	{
		send_to_home(orangeGhost, orangeGhost->ghostType);
	}
}

void execute_blue_logic(Ghost *blueGhost, Ghost *redGhost, Pacman *pacman)
{
	int offsetX = 0;
	int offsetY = 0;

	//use dir_xy_buggy to get 2 up AND 2 left, as per bug in original game
	dir_xy_buggy(pacman->body.curDir, &offsetX, &offsetY);

	offsetX *= 2;
	offsetY *= 2;

	int tx = pacman->body.x + offsetX;
	int ty = pacman->body.y + offsetY;

	int rx = redGhost->body.x;
	int ry = redGhost->body.y;

	int targetX = 2 * tx - rx;
	int targetY = 2 * ty - ry;

	blueGhost->targetX = targetX;
	blueGhost->targetY = targetY;
}

int ghost_speed_normal(int level)
{
	if (level == 1)  return 75;
	if (level <= 4)  return 85;
	if (level <= 20) return 95;
	return 95;
}
int ghost_speed_fright(int level)
{
	if (level == 1)  return 50;
	if (level <= 4)  return 55;
	if (level <= 20) return 60;
	return 100; //ghosts don't get into frightened mode past this
}
int ghost_speed_tunnel(int level)
{
	if (level == 1)  return 40;
	if (level <= 4)  return 45;
	if (level <= 20) return 50;
	return 50;
}
