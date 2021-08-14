#include <CIndieLib.h>
/*****************************************************************************************
 * File: IND_Math.cpp
 * Desc: ¿Math methods?
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (javierlopezpro@gmail.com)

This library is free software you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free Software
Foundation either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this library if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA 
*/
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	
*/

// ----- Includes -----

#include <CIndieLib.h>


#include <IND_Math.h>


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*
*************************
Init
*************************
*/
void IND_Math::Reset ()
{
	stx_srand (m_seed);
}
bool IND_Math::Init ()
{
	End ();
	FreeVars ();

	m_seed=timeGetTime();
	// Random Seed
	stx_srand (
		m_seed
);

	mOk = true;

	return mOk;
}


/*
*************************
End
*************************
*/
void IND_Math::End()
{
	if (mOk)
	{

		FreeVars ();

		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//										 Public methods
// --------------------------------------------------------------------------------

/*
*************************
Sets the seed for random numbers
*************************
*/
/*
void IND_Math::RandSeed	(int pNum)
{
	stx_srand (pNum);
}
*/

/*
*************************
Returns a random number between two boundaries
*************************
*/
INLINE 
int IND_Math::Rand	(int pIni, int pEnd)
{
	if (pIni > pEnd)
	{
		int pTemp = pEnd;
		pEnd = pIni;
		pIni = pTemp;
	}

	return stx_rand () % (pEnd - pIni + 1) + pIni;

	//int mResu = pIni + (stx_rand () % ((pEnd - pIni) + 1));
	//return mResu;
}


/*
*************************
Returns 1 if the number is power of two, 0 otherwise
*************************
*/
bool IND_Math::IsPowerOfTwo (int pN)
{
	while (pN > 1L)
	{
		if (pN % 2L) break;
		pN >>= 1L;
	}

	return (pN == 1L);
}


// --------------------------------------------------------------------------------
//									 Private methods
// --------------------------------------------------------------------------------

/*
*************************
Init vars
*************************
*/
void IND_Math::InitVars ()
{

}


/*
*************************
Free memory
*************************
*/
void IND_Math::FreeVars ()
{

}

