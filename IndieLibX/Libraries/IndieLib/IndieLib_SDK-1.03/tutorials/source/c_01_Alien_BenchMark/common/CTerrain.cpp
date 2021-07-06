/*****************************************************************************************
/* File: CTerrain.h
/* Desc: Tiled terrain
/*****************************************************************************************/

// ----- Includes -----

#include "CTerrain.h"

// --------------------------------------------------------------------------------
//								Initialization / Destruction
// --------------------------------------------------------------------------------

/*
==================
Init
==================
*/
CTerrain::CTerrain	(int pNumBlocksX, int pNumBlocksY, int pNumHoles, IND_Surface *pTerrainTile, IND_Surface *pHole1, IND_Surface *pHole2)
{
	// Get IndieLib singleton object
	mI = CIndieLib::Instance();

	mNumBlocksX = pNumBlocksX;
	mNumBlocksY = pNumBlocksY;

	// ----- Tiled terrain: Our terrain has pNumBlocksX x pNumBlocksY tiles of 512x512 pixels  each one ----

	// We create a tiled surface using ToogleWrap and SetRegion methods
	mI->Entity2dManager->Add (0, &mTerrain);	
	mTerrain.SetSurface(pTerrainTile);
	mTerrain.ToggleWrap (1);
	mTerrain.SetRegion (0, 0, pNumBlocksX *  pTerrainTile->GetWidth(), pNumBlocksY *  pTerrainTile->GetHeight());

	// ----- Holes: Some craters of two types with random position and sizes ----

	// Memory allocation in a bidimensional array for the tiles
	mHoles = new IND_Entity2d [pNumHoles];

	for (int i = 0; i < pNumHoles; i++)
	{
		// Entity adding in layer 0 (terrain)
		mI->Entity2dManager->Add (0, &mHoles[i]);			

		// 50% chance of creating a hole type 1, 50% type 2
		if (mI->Math->Rand (0, 1))
		{
			mHoles[i].SetSurface (pHole1);	
		}
		else
		{
			mHoles[i].SetSurface (pHole2);

		}

		// Setting the collision areas
		mHoles[i].SetBoundingCircle ("area_crater", 250, 250, 270);
		mHoles[i].SetBoundingCircle ("area_crater_ufo", 250, 250, 200);

		// Random position for the hole
		mHoles[i].SetPosition	((float) mI->Math->Rand (0, pNumBlocksX * pTerrainTile->GetWidth()),
								(float) mI->Math->Rand (0, pNumBlocksY * pTerrainTile->GetHeight()),
								1);

		// Ramdom size (50% - 100%) for the hole
		int mScale = mI->Math->Rand (50, 100);
		mHoles[i].SetScale ((float) mScale / 100.0f, (float) mScale / 100.0f);

		// Check if the hole collides with another hole
		for (int j = 0; j < i; j++)
		{
			// If the new hole collides with a previos hole, delete it
			if (mI->Entity2dManager->IsCollision (&mHoles[i], "area_crater", &mHoles[j], "area_crater"))
			{
				mI->Entity2dManager->Delete (&mHoles[i]);
				break;
			}
		}
	}
}


/*
==================
End
==================
*/
CTerrain::~CTerrain()
{
	delete[] (mHoles);
}
