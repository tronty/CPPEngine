#include "fps.h"

#include <SDL/SDL_framerate.h>

static FPSmanager fpsManager;
static int numTicks;

void fps_init(int hz)
{
	SDL_initFramerate(&fpsManager);
	fps_sethz(hz);
}

void fps_sethz(int hz)
{
	SDL_setFramerate(&fpsManager, hz);
}

void fps_sleep(void)
{
	numTicks++;
	SDL_framerateDelay(&fpsManager);
}

unsigned int ticks_game(void)
{
	return SDL_GetTicks();
}

unsigned int ticks_startup()
{
	return SDL_GetTicks();
}

unsigned int frames_game(void)
{
	return numTicks;
}

unsigned int frames_startup(void)
{
	return numTicks;
}
