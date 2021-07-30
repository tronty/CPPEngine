#pragma once

#include <stdbool.h>

#include "direction.h"

//Defines a physics entity which can move in a 2d grid.
typedef struct
{
	int x;
	int y;
	int xOffset;
	int yOffset;
	Direction curDir;    //current direction we are moving
	Direction nextDir;   //direction to move once we reach the center of the next tile
	int velocity;
	int xOffsetInternal; //accurate offset used for internal calculation
	int yOffsetInternal; //accurate offset used for internal calculation
} PhysicsBody;

//Results of a movement.
typedef enum
{
	NewSquare,  //Entity has moved into a new square
	OverCenter, //Entity has moved over the center of its current tile
	SameSquare  //Entity has moved within square without moving over the center
} MovementResult;

//Determines if the bodies collide based soley on their square on the board.
bool collides(PhysicsBody *body1, PhysicsBody *body2);

//Determines if the bodies collide based on their square and position offset within the square.
bool collides_accurate(PhysicsBody *body1, PhysicsBody *body2);

bool collides_obj(PhysicsBody *body, int otherX, int otherY);

//Advances the body 1 step based on current direction and velocity.
//Returns true if the body changed square (a complete tile) during the move, false otherwise
MovementResult move_ghost(PhysicsBody *body);

bool move_pacman(PhysicsBody *body, bool canMoveCur, bool canMoveNext);

//Returns true if the body is in the center of a tile.
bool on_center(PhysicsBody *body);

//Returns true if the body is in the center of a tile on the vertical axis.
bool on_vert(PhysicsBody *body);

//Returns true if the body is in the center of a tile on the horozontal axis.
bool on_horo(PhysicsBody *body);
