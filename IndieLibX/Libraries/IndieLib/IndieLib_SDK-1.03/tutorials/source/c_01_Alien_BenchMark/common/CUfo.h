/*****************************************************************************************
/* File: CUfo.h
/* Desc: Ufo object
/*****************************************************************************************/

#ifndef _CUFO_
#define _CUFO_

// ------ Includes -----

#include <CIndieLib.h>

// --------------------------------------------------------------------------------
//									 CUfo
// --------------------------------------------------------------------------------

class CUfo
{
public:

	CUfo		();

	// ----- Public methods ------

	void Init		(float pX, float pY, IND_Animation *pUfoAnimation);

private:

	// ----- Private ------

	CIndieLib *mI;						// IndieLib object
	IND_Entity2d mUfo;
};


#endif // _CUFO_
