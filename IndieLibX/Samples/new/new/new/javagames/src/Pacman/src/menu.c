#include "menu.h"

#include <stdbool.h>

#include <SDL/SDL_keysym.h>

#include "input.h"
#include "ghost.h"
#include "main.h"
#include "renderer.h"

//time till ghost-rows start appearing
#define GHOST_START 1000

//time between each ghost-row appearance
#define GHOST_BETWEEN 2000

static void draw_vanity_screen(MenuSystem *menuSystem);
static void draw_info_screen(void);

static void draw_ghost_line(GhostDisplayRow *row, int y, unsigned int dt);
static void draw_player_info(void);

static GhostDisplayRow enemyRows[4] = {
	{Blinky, "-SHADOW",  "\"BLINKY\"", RedText},
	{Pinky,  "-SPEEDY",  "\"PINKY\"",  PinkText},
	{Inky,   "-BASHFUL", "\"INKY\"",   CyanText},
	{Clyde,  "-POKEY",   "\"CLYDE\"",  OrangeText}
};

void menu_init(MenuSystem *menuSystem)
{
	menuSystem->action = Nothing;
	menuSystem->ticksSinceModeChange = SDL_GetTicks();
}

void menu_tick(MenuSystem *menuSystem)
{
	bool startNew = key_held(SDLK_KP_ENTER) || key_held(SDLK_RETURN);

	if (startNew)
	{
		menuSystem->action = GoToGame;
	}
}

void menu_render(MenuSystem *menuSystem)
{
	if (num_credits() == 0) draw_vanity_screen(menuSystem);
	else draw_info_screen();
}

static void draw_vanity_screen(MenuSystem *menuSystem)
{
	unsigned int dt = SDL_GetTicks() - menuSystem->ticksSinceModeChange;

	draw_player_info();
	draw_vanity_charnickname();

	for (int i = 0; i < 4; i++)
	{
		unsigned int current = GHOST_START + i * GHOST_BETWEEN;
		if (dt < current) break;

		GhostDisplayRow r = enemyRows[i];
		draw_ghost_line(&r, 7 + 3 * i, dt - current);
	}

	if (dt > 9500) draw_vanity_pellet_info(false);
	if (dt > 10500) draw_vanity_corporate_info();
	if (dt > 11500) draw_vanity_animation(dt - 11500);
}

static void draw_info_screen(void)
{
	draw_player_info();
	draw_instrc_info();
	draw_instrc_corporate_info();
}

static void draw_player_info(void)
{
	draw_common_oneup(false, 0);
	draw_common_twoup(false, 0);
	draw_common_highscore(0);

	draw_credits(num_credits());
}

static void draw_ghost_line(GhostDisplayRow *row,  int y, unsigned int dt)
{
	bool drawDescription = dt > 1000;
	bool drawName = dt > 1500;

	draw_vanity_ghostline(row, y, drawDescription, drawName);
}
