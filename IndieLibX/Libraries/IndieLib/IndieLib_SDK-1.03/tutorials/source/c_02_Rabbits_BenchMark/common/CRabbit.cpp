/*****************************************************************************************
/* File: CRabbit.h
/* Desc: Rabbit class
/*****************************************************************************************/

// ----- Includes -----

#include "CRabbit.h"


// --------------------------------------------------------------------------------
//								Initialization / Destruction
// --------------------------------------------------------------------------------

/*
==================
Init
==================
*/
CRabbit::CRabbit ()
{
	// Get IndieLib singleton object
	mI = CIndieLib::Instance();

	mScreenWidth = mI->Render->GetViewPortWidth();
	mScreenHeight = mI->Render->GetViewPortHeight();
}


/*
==================
Initializes the attributes of a rabbit
==================
*/
void CRabbit::Init (IND_Surface *pSurfaceRabbit, int pZ)
{
	// Depth
	mZ = pZ;

	// Add the entity
	mI->Entity2dManager->Add (&mRabbit);	
	mRabbit.SetSurface (pSurfaceRabbit);

	// Set a random position to the rabbit
	mRabbit.SetPosition ((float) mI->Math->Rand (0, mScreenWidth), (float) mI->Math->Rand (0, mScreenHeight), mZ);

	// Set a random tint color to some of the rabbits
	if (mI->Math->Rand (0, 5) != 5)
		mRabbit.SetTint ((unsigned char) mI->Math->Rand (128, 255),
						(unsigned char) mI->Math->Rand (128, 255),
						(unsigned char) mI->Math->Rand (128, 255));

	// Hot spot in the middle of the rabbit
	mRabbit.SetHotSpot (0.5f, 0.5f);

	// Random scale
	int mScale = mI->Math->Rand (50, 150);
	mRabbit.SetScale ((float) mScale / 100.0f, (float) mScale / 100.0f);

	// Random direction
	mDir = mI->Math->Rand (0, 3);

	// Random speed rotation
	mSpeedRotation = (float) mI->Math->Rand (-200, 200);

	// Random Speed
	mSpeed = (float) mI->Math->Rand (100, 200);
}


// --------------------------------------------------------------------------------
//										Public Methods
// --------------------------------------------------------------------------------

/*
==================
Update the position of the rabbits on the screen. This method will make the rabbits bounce
in the screen
==================
*/
void CRabbit::Update ()
{
	float mDirX, mDirY;

	// Get the elapsed time of the last frame
	float mDelta =  (float) mI->Render->GetFrameTime() / 1000.f;
	
	switch (mDir)
	{
		case RIGHT_DOWN:
		{
			mDirX = mDelta * mSpeed;
			mDirY = mDelta * mSpeed;

			if (mRabbit.GetPosX() > mScreenWidth)
				mDir = LEFT_DOWN;

			if (mRabbit.GetPosY() > mScreenHeight)
				mDir = RIGHT_UP;

			break;
		}

		case LEFT_DOWN:
		{
			mDirX = -mDelta * mSpeed;
			mDirY = mDelta * mSpeed;

			if (mRabbit.GetPosX() < 0)
				mDir = RIGHT_DOWN;

			if (mRabbit.GetPosY() > mScreenHeight)
				mDir = LEFT_UP;

			break;
		}


		case RIGHT_UP:
		{
			mDirX = mDelta * mSpeed;
			mDirY = -mDelta * mSpeed;

			if (mRabbit.GetPosX() > mScreenWidth)
				mDir = LEFT_UP;

			if (mRabbit.GetPosY() < 0)
				mDir = RIGHT_DOWN;

			break;
		}


		case LEFT_UP:
		{
			mDirX = -mDelta * mSpeed;
			mDirY = -mDelta * mSpeed;

			if (mRabbit.GetPosX() < 0)
				mDir = RIGHT_UP;

			if (mRabbit.GetPosY() < 0)
				mDir = LEFT_DOWN;

			break;
		}
	}

	mRabbit.SetPosition (mRabbit.GetPosX() + mDirX,  mRabbit.GetPosY() + mDirY, mZ);
	mRabbit.SetAngleXYZ (0, 0, mRabbit.GetAngleZ() + mSpeedRotation * mDelta);
}
