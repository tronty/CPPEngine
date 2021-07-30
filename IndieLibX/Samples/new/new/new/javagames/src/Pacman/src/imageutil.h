#pragma once

#include <SDL/SDL.h>

//Clones the image, creating and returning a new copy
//Warning: this allocates memory and the created surface will need to be disposed
SDL_Surface *clone_image(SDL_Surface* toClone);

//Replaces all non-transparent pixels of an image with the given color.
void replace_pixel(SDL_Surface *image, SDL_Color newColor);

//Gets the colour pixel as a Uint32 of an image at a given coordinate
Uint32 get_pixel(SDL_Surface *surface, int x, int y);
