#pragma once

#include <stdbool.h>

#include "direction.h"
#include "physics.h"

//pacman has a location (x,y) on the board. This is the tile his center is, and it defines his actual tile

//he also has x/y tile offset, which can be between (-8, 7)
//this defines his exact position on each individual tile, and his position for purposes of collision

//typedef for pacman when he is moving
//if he moved against a wall and the player doesn't move, he is considered "stuck"
//this is needed for animation purposes
typedef enum
{
	Stuck,
	Unstuck
} MovementType;

//Defines Pacman. Includes all his information such as location, lives, score, etc.
typedef struct
{
	PhysicsBody body;
	int missedFrames;
	int livesLeft;
	int score;
	Direction lastAttemptedMoveDirection;
	MovementType movementType;
} Pacman;

//Initializes pacman's location, lives, score. Call at start of the game.
void pacman_init(Pacman *pacman);

//Initializes pacman's location. Call at the start of each level.
void pacman_level_init(Pacman *pacman);

//Initializes pacman's location to the starting point (as well as direction and other values).
void pacman_location_init(Pacman *pacman);

//Returns pacman speed for given level when not in fright mode.
int pacman_speed_normal(int level);

//Returns pacman speed for given level when in fright mode.
int pacman_speed_fright(int level);
