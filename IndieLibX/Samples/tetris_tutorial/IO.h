/*****************************************************************************************
/* File: IO.h
/* Desc: Class for handling inpung & drawing, it uses SDL for the rendering. Change the methods of this class
/*       in order to use a different renderer
/*
/* gametuto.com - Javier L�pez L�pez (javilop.com)
/*
/*****************************************************************************************
/*
/* Creative Commons - Attribution 3.0 Unported
/* You are free:
/*	to Share � to copy, distribute and transmit the work
/*	to Remix � to adapt the work
/*
/* Under the following conditions:
/* Attribution. You must attribute the work in the manner specified by the author or licensor 
/* (but not in any way that suggests that they endorse you or your use of the work).
/*
/*****************************************************************************************/

#ifndef _IO_
#define _IO_

#include <Framework3/IRenderer.h>

// ------ Includes -----

// ------ Enums -----

enum color {BLACK, RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE, COLOR_MAX}; // Colors


// --------------------------------------------------------------------------------
//									 IO
// --------------------------------------------------------------------------------

class IO
{
public:

	IO						();

	void DrawRectangle		(int pX1, int pY1, int pX2, int pY2, enum color pC);
	void ClearScreen		();
	int GetScreenHeight		();
	int InitGraph			();
	int Pollkey				();
	int Getkey				();
	int IsKeyDown			(int pKey);
	void UpdateScreen		();

};

#endif // _IO_
