/*****************************************************************************************
/* File: CTerrain.h
/* Desc: Tiled terrain
/*****************************************************************************************/

#ifndef _CTERRAIN_
#define _CTERRAIN_

// ------ Includes -----

#include <CIndieLib.h>


// --------------------------------------------------------------------------------
//									 CTerrain
// --------------------------------------------------------------------------------

class CTerrain
{
public:
	CTerrain			()
	{
		mI=0;
		mNumBlocksX=0;
		mNumBlocksY=0;
	}
	CTerrain			(int pNumBlocksX, int pNumBlocksY, int pNumHoles, 
						IND_Surface *pTerrainTile, IND_Surface *pHole1, IND_Surface *pHole2);

	~CTerrain			();

	IND_Entity2d *mHoles;				// Craters
private:

	// ----- Private ------

	CIndieLib *mI;						// IndieLib object
	IND_Entity2d mTerrain;				// Wrapped texture


	int mNumBlocksX, mNumBlocksY;		// Number of horizontal and vertical tiles
};


#endif // _CTERRAIN_
