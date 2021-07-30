#pragma once

#include <stdbool.h>

#include <SDL/SDL.h>

#define NUM_FONTS 7

//The different font-colors supported.
typedef enum
{
	WhiteText,
	YellowText,
	PeachText,
	RedText,
	PinkText,
	CyanText,
	OrangeText,
} TextColor;

//Loads the fonts. Call this before doing any other operation.
void load_text(void);

//Disposes of font images. Call this once you are finished rendering text.
void dispose_text(void);

//Sets the color the font renders at.
void set_text_color(TextColor textColor);

//Draws the text at a given pixel coordinate on screen.
void draw_text(SDL_Surface *surface, const char *text, int x, int y);

//Draws the text at the given square on screen.
void draw_text_coord(SDL_Surface *surface, const char *text, int x, int y);

//Draws text at the given square, but offset on y axis by 1 text pixel (used by numbers on main screens)
void draw_numtext_coord(SDL_Surface *surface, const char *text, int x, int y);

//Returns true if the font supports the given character.
bool is_valid(char c);

//Returns true if the font supports the string (does a check on each character).
bool is_valid_string(const char *text);
