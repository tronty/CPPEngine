#pragma once

//Initialises the FPS manager to the given frequency hz.
void fps_init(int hz);

//Resets the FPS manager to the given frequency hz.
void fps_sethz(int hz);

//Sleeps until 60hz is reached.
void fps_sleep(void);

//A simple wrapper around SDL_GetTicks().
//This allows us to pause the game though, by subtracting our pause time from SDL_GetTicks().
unsigned int ticks_game(void);

//Returns SDL_GetTicks().
//This allows us to know the actual time the game has been running for.
unsigned int ticks_startup(void);

//Returns the number of frames that have happened of actual game-time.
unsigned int frames_game(void);
//Returns the number of frames that have happened since the program starting.
unsigned int frames_startup(void);
