/*****************************************************************************************
/* File: IO.cpp
/* Desc: Class for handling inpung & drawing, it uses SDL for the rendering. Change the methods 
/* of this class in order to use a different renderer
/*       
/*
/* gametuto.com - Javier López López (javilop.com)
/*
/*****************************************************************************************
/*
/* Creative Commons - Attribution 3.0 Unported
/* You are free:
/*	to Share — to copy, distribute and transmit the work
/*	to Remix — to adapt the work
/*
/* Under the following conditions:
/* Attribution. You must attribute the work in the manner specified by the author or licensor 
/* (but not in any way that suggests that they endorse you or your use of the work).
/*
/*****************************************************************************************/

// ------ Includes -----

#include "IO.h"

//static SDL_Surface *mScreen;										// Screen
static Uint32 mColors [COLOR_MAX] = {0x000000ff,					// Colors
                               0xff0000ff, 0x00ff00ff, 0x0000ffff,
                               0x00ffffff, 0xff00ffff, 0xffff00ff,
                               0xffffffff};

/* 
======================================									
Init
====================================== 
*/
IO::IO() 
{
	InitGraph ();
}


/* 
======================================									
Clear the screen to black
====================================== 
*/
void IO::ClearScreen() 
{
	//boxColor (0, 0, STX_Service::GetWindowInstance()->Getwidth() - 1, STX_Service::GetWindowInstance()->GetHeight() - 1, mColors[BLACK]);
	const D3DXFROMWINEVECTOR4 color(0.0f, 0.0f, 0.0f, 1.0f);
	IRenderer::GetRendererInstance()->drawRect(0, 0, STX_Service::GetWindowInstance()->Getwidth() - 1, STX_Service::GetWindowInstance()->GetHeight() - 1, color, 1.0f);
}


/* 
======================================									
Draw a rectangle of a given color

Parameters:
>> pX1, pY1: 		Upper left corner of the rectangle
>> pX2, pY2: 		Lower right corner of the rectangle
>> pC				Rectangle color
====================================== 
*/
void IO::DrawRectangle (int pX1, int pY1, int pX2, int pY2, enum color pC)
{
	//boxColor (pX1, pY1, pX2, pY2-1, mColors[pC]);
	D3DXFROMWINEVECTOR4 color(0.0f, 0.0f, 0.0f, 1.0f);
	switch(pC)
	{
		case BLACK:
			color=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		case RED:
			color=D3DXFROMWINEVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		case GREEN:
			color=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
			break;
		case BLUE:
			color=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
			break;
		
		case CYAN:
			color=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 1.0f, 1.0f);
			break;
		case MAGENTA:
			color=D3DXFROMWINEVECTOR4(1.0f, 0.0f, 1.0f, 1.0f);
			break;
		case YELLOW:
			color=D3DXFROMWINEVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		
		case WHITE:
			color=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		default:
			color=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
	};
	IRenderer::GetRendererInstance()->drawRect(pX1, pY1, pX2, pY2-1, color, 1.0f);
}


/* 
======================================									
Return the screen height
====================================== 
*/
int IO::GetScreenHeight()
{
	return STX_Service::GetWindowInstance()->GetHeight();
}


/* 
======================================									
Update screen
====================================== 
*/
void IO::UpdateScreen()
{
	//SDL_Flip(mScreen);
}


/* 
======================================									
Keyboard Input
====================================== 
*/
int IO::Pollkey()
{
/*	SDL_Event event;
	while ( SDL_PollEvent(&event) ) 
	{
		switch (event.type) {
			case SDL_KEYDOWN:
				return event.key.keysym.sym;
			case SDL_QUIT:
				exit(3);
		}
	}
	return -1;
*/
	return STX_Service::GetInputInstance()->getKeyID();
}

/* 
======================================									
Keyboard Input
====================================== 
*/
int IO::Getkey()
{
/*
	SDL_Event event;
	while (true)
	{
	  SDL_WaitEvent(&event);
	  if (event.type == SDL_KEYDOWN)
		  break;
      if (event.type == SDL_QUIT)
		  exit(3);
	};
	return event.key.keysym.sym;
*/
	return STX_Service::GetInputInstance()->getKeyID();
}

/* 
======================================									
Keyboard Input
====================================== 
*/
int IO::IsKeyDown (int pKey)
{/*
	Uint8* mKeytable;
	int mNumkeys;
	SDL_PumpEvents();
	mKeytable = SDL_GetKeyState(&mNumkeys);
	return mKeytable[pKey];
*/
	return STX_Service::GetInputInstance()->getKeyID();
}

/* 
======================================									
SDL Graphical Initialization
====================================== 
*/
int IO::InitGraph()
{
/*
	const SDL_VideoInfo *info;
	Uint8  video_bpp;
	Uint32 videoflags;
        
	// Initialize SDL
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);

	// Alpha blending doesn't work well at 8-bit color
	info = SDL_GetVideoInfo();
	if ( info->vfmt->BitsPerPixel > 8 ) {
		video_bpp = info->vfmt->BitsPerPixel;
	} else {
		video_bpp = 16;
	}
	videoflags = SDL_SWSURFACE | SDL_DOUBLEBUF;
	
	// Set 640x480 video mode
	if ( (mScreen=SDL_SetVideoMode(640,480,video_bpp,videoflags)) == NULL ) {
		fprintf(stderr, "Couldn't set %ix%i video mode: %s\n",640,480,SDL_GetError());
		return 2;
	}
*/
    return 0;
}
