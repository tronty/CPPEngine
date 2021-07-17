/*****************************************************************************************
/* File: CUfo.h
/* Desc: Ufo object
/*****************************************************************************************/

// ----- Includes -----

#include "CUfo.h"

// --------------------------------------------------------------------------------
//								Initialization / Destruction
// --------------------------------------------------------------------------------

/*
==================
Init
==================
*/
CUfo::CUfo ()
{
	// Get IndieLib singleton object
	mI = CIndieLib::Instance();
}


/*
==================
Init
==================
*/
void CUfo::Init (float pX, float pY, IND_Animation *pUfoAnimation)
{
	mI->Entity2dManager->Add (0, &mUfo);			// Entity adding in layer 0 (terrain)
	mUfo.SetAnimation (pUfoAnimation);
	mUfo.SetPosition (pX, pY, 2);
}
