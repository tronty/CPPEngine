/*****************************************************************************************
/* File: IND_LightManager.h
/* Desc: Light manager
/*****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (info@pixelartgames.com) 

This library is free software; you can redistribute it and/or modify it under the 
terms of the GNU Lesser General Public License as published by the Free Software 
Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with 
this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
Suite 330, Boston, MA 02111-1307 USA 
*/
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	 
*/


#ifndef _IND_LIGHTMANAGER_
#define _IND_LIGHTMANAGER_

// ----- Includes -----

#include <list>
#include "IND_Light.h"

// ----- Defines -----

#define MAX_NUM_LIGHTS 8


// --------------------------------------------------------------------------------
//									IND_LightManager
// --------------------------------------------------------------------------------

/*!
\defgroup IND_LightManager IND_LightManager
\ingroup Managers
Manager of IND_Light objects. Click in ::IND_LightManager to see all the methods of this class.
*/
/*@{*/

/*!
This class manages the lights used in 3d scenes. These light don't affect IND_Surface objects
or other 2d graphical objects.

See all the methods of ::IND_Light in order to check which attributes you can change in each light.
The maximun number of allowed enabled lights at the same time is 8, but can can create as many as you
want and disable / enable them.

There are several types of lights, see ::IND_LightType for more details.
*/
class DLL_EXP IND_LightManager
{
public:
	
	// ----- Init/End -----

	IND_LightManager		(): mOk(false)	{ }
	~IND_LightManager		()									{ End(); }

	bool	Init    (IND_Render *pRender);
	void	End     ();
	bool	IsOk	()			 const { return mOk; }
					
	// ----- Public methods -----

	bool Add				(IND_Light *pNewLight, IND_LightType pLightType);
	bool Delete				(IND_Light *pLight);
	bool Enable				(IND_Light *pLight);
	bool Disable			(IND_Light *pLight);
	void Update				();

	// ----- Public gets -----

	int GetNumLightsEnabled ()	{ return mNumLightsEnabled; }

private:

	// ----- Private -----

	bool mOk;
	int mNumLightsEnabled;		// Number of lights enabled
	int mLightIndex;			// Index that will be increased each time we use SetLight() method

	// ----- Objects -----

	IND_Render *mRender;

	// ----- Container -----

	list <IND_Light*> *mListLights;

	// ----- Private methods -----

	void CreateLight		(IND_Light *pNewLight, D3DLIGHTTYPE pLightType);

	void AddToList			(IND_Light *pLi);
	void DelFromList		(IND_Light *pLi);
	void WriteMessage		();
	void InitVars			();
	void FreeVars			();
};
/*@}*/
	
#endif // _IND_LIGHTMANAGER_
