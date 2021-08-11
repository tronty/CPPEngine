#pragma once

//28 across, 36 down, 16 pixels each square.
#define SCREEN_TITLE "Tphakumahn"
#define SCREEN_WIDTH 448
#define SCREEN_HEIGHT 576

//Defines the main states the program can be in.
typedef enum
{
	Menu,
	Game,
	Intermission
} ProgramState;

//Returns the number of credits the user currently has.
int num_credits(void);
