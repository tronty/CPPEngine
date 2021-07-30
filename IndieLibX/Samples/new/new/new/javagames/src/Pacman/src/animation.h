#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

typedef struct
{
	int numImages;
	int freq;
	SDL_Surface *surfaces;
} Animation;

int animation_getframe(Animation *animation);

int animation_get_frame(int msBetweenFrames, int numFrames);

int animation_get_frame_dt(int currentDt, int msBetweenFrames, int numFrames);
