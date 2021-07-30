#pragma once

#include <stdbool.h>

#include <SDL/SDL.h>

//Initializes the window to the given length, width and title.
bool init_window(const char* title, int width, int height);

//Disposes the rendering window.
void dispose_window(void);

//Gets the SDL_Surface which represents the entire screen.
SDL_Surface *get_screen(void);

//Cleans the screen to the given color.
void clear_screen(int r, int g, int b, int a);

//Applys a surface to the screen at the given coordinates.
void apply_surface(int x, int y, SDL_Surface* source);

//Used for double buffering. Flips the buffers.
void flip_screen(void);
