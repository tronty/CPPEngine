#pragma once

#include <stdbool.h>

#include <SDL/SDL.h>

#define NUM_PELLETS 244

//The different pellet types.
typedef enum
{
	SmallPellet,
	LargePellet
} PelletType;

//Defines a single pellet. Includes pellet-type, pellet location, etc.
typedef struct
{
	int x, y;
	bool eaten;
	PelletType type;
	SDL_Surface *image;
} Pellet;

//Holds all the pellets in the game. Records how many are eaten and are left.
typedef struct
{
	int numLeft;
	int totalNum;
	Pellet pellets[NUM_PELLETS];
} PelletHolder;

//Inits the pellets to non-eaten, and resets the pellet counters.
void pellets_init(PelletHolder *pelletHolder);

//Returns how many points a pellet is worth (defined by its pellet-type).
int pellet_points(Pellet *pellet);

//Returns the number of movement frames this pellet makes pacman miss when eaten.
int pellet_nop_frames(Pellet *pellet);
