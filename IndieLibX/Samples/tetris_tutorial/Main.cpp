/*****************************************************************************************
/* Desc: Tetris tutorial
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

#include "Game.h"

/*
==================
Main
==================
*/
int init(const char* aTitle)
{
	return 0;
}
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Tetris");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	// ----- Vars -----

	// Class for drawing staff, it uses SDL for the rendering. Change the methods of this class
	// in order to use a different renderer
	IO mIO;
	int mScreenHeight = mIO.GetScreenHeight();

	// Pieces
	Pieces mPieces;

	// Board
	Board mBoard (&mPieces, mScreenHeight);

	// Game
	Game mGame (&mBoard, &mPieces, &mIO, mScreenHeight);

	// Get the actual clock milliseconds (SDL)
	unsigned int mTime1 = timeGetTime();

	// ----- Main Loop -----

	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
		i->Update();
		// ----- Draw -----

		mIO.ClearScreen (); 		// Clear screen
		mGame.DrawScene ();			// Draw staff
		mIO.UpdateScreen ();		// Put the graphic context in the screen

		// ----- Input -----

		/* int mKey = mIO.Pollkey();

		switch (mKey) */
		{
			if (STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_RIGHT)) 
			{
				if (mBoard.IsPossibleMovement (mGame.mPosX + 1, mGame.mPosY, mGame.mPiece, mGame.mRotation))
					mGame.mPosX++;
					//break;
			}

			else if (STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_LEFT)) 
			{
				if (mBoard.IsPossibleMovement (mGame.mPosX - 1, mGame.mPosY, mGame.mPiece, mGame.mRotation))
					mGame.mPosX--;	
				//break;
			}

			else if (STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_DOWN))
			{
				if (mBoard.IsPossibleMovement (mGame.mPosX, mGame.mPosY + 1, mGame.mPiece, mGame.mRotation))
					mGame.mPosY++;	
				//break;
			}

			else if (STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_x))
			{
				// Check collision from up to down
				while (mBoard.IsPossibleMovement(mGame.mPosX, mGame.mPosY, mGame.mPiece, mGame.mRotation)) { mGame.mPosY++; }
	
				mBoard.StorePiece (mGame.mPosX, mGame.mPosY - 1, mGame.mPiece, mGame.mRotation);

				mBoard.DeletePossibleLines ();

				if (mBoard.IsGameOver())
				{
					mIO.Getkey();
					exit(0);
				}

				mGame.CreateNewPiece();

				//break;
			}

			else if (STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_z))
			{
				if (mBoard.IsPossibleMovement (mGame.mPosX, mGame.mPosY, mGame.mPiece, (mGame.mRotation + 1) % 4))
					mGame.mRotation = (mGame.mRotation + 1) % 4;

				//break;
			}
		}

		// ----- Vertical movement -----

		unsigned int mTime2 = timeGetTime();

		if ((mTime2 - mTime1) > WAIT_TIME)
		{
			if (mBoard.IsPossibleMovement (mGame.mPosX, mGame.mPosY + 1, mGame.mPiece, mGame.mRotation))
			{
				mGame.mPosY++;
			}
			else
			{
				mBoard.StorePiece (mGame.mPosX, mGame.mPosY, mGame.mPiece, mGame.mRotation);

				mBoard.DeletePossibleLines ();

				if (mBoard.IsGameOver())
				{
					mIO.Getkey();
					exit(0);
				}

				mGame.CreateNewPiece();
			}

			mTime1 = timeGetTime();
		}
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
	}

	return 0;
}
