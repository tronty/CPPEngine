/*****************************************************************************************
/* File: IND_AnimationManager.h
/* Desc: Animation manager
/*****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (javierlopezpro@gmail.com) 

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

#ifndef _IND_ANIMATIONMANAGER_
#define _IND_ANIMATIONMANAGER_

// ----- Includes -----

#include <list>
#include <vector>
#include "IND_SurfaceManager.h"
#include "IND_Timer.h"
#include "CollisionParser.h"
#include "IND_Animation.h"


// --------------------------------------------------------------------------------
//									IND_AnimationManager
// --------------------------------------------------------------------------------

/*!
\defgroup IND_AnimationManager IND_AnimationManager
\ingroup Managers
Manager of ::IND_Animation objects. Click in ::IND_AnimationManager to see all the methods of this class.
*/
/*@{*/

/*!
Manager of ::IND_Animation objects. Used for storing these type of objects.
*/
class IND_ImageManager;
class IND_SurfaceManager;
class DLL_EXP IND_AnimationManager
{
public:

	// ----- Init/End -----

	IND_AnimationManager		(): mOk(false)	{ }
	~IND_AnimationManager		()				{ End(); }

	bool	Init    (IND_ImageManager *pImageManager, IND_SurfaceManager *pSurfaceManager);
	void	End     ();
	bool	IsOk	() { return mOk; }
	
	// ----- Public methods -----

	bool AddToImage			(IND_Animation *pNewAnimation, const char *pAnimation);

	// ----- Not specifying block size -----

	bool AddToSurface		(IND_Animation *pNewAnimation, 
							const char *pAnimation,
							IND_Type pType, 
							IND_Quality	pQuality);

	bool AddToSurface		(IND_Animation *pNewAnimation, 
							const char *pAnimation,
							IND_Type pType, 
							IND_Quality	pQuality,
							unsigned char pR,
							unsigned char pG,
							unsigned char pB);

	// ----- Specifying block size ----- 

	bool AddToSurface		(IND_Animation *pNewAnimation, 
							const char *pAnimation,
							int pBlockSize,
							IND_Type pType, 
							IND_Quality	pQuality);

	bool AddToSurface		(IND_Animation *pNewAnimation, 
							const char *pAnimation,
							int pBlockSize,
							IND_Type pType, 
							IND_Quality	pQuality,
							unsigned char pR,
							unsigned char pG,
							unsigned char pB);

	bool Delete				(IND_Animation *pAn);

					
private:

	// ----- Private -----
	
	bool mOk;

	// ----- Enums -----

	enum 
	{
		FRAMES,
		FRAME_,
		PATH,
		NAME,
		FILE,
		OFFSET_X,
		OFFSET_Y,
		SEQUENCES,
		TIME,
		OBRACKET,
		CBRACKET,
		KW_UNKNOWN
	};

	// ----- Objects -----

	IND_ImageManager *mImageManager;
	IND_SurfaceManager *mSurfaceManager;
	CollisionParser *mCollisionParser;

	// ----- Containers -----

	list <IND_Animation*> *mListAnimations;
	
	// ----- Private methods -----
	
	bool CalculeAxis					(IND_Animation *pAn, 
										int pSequence,
										float pAxisX, 
										float pAxisY, 
										int *pAxisCalX, 
										int *pAxisCalY);

	void		AddToList				(IND_Animation *pNewAnimation);
	void		DelFromList				(IND_Animation *pAn);
	IND_Image	*LoadImage				(const char *pName);
	bool		Delete					(IND_Animation *pAn, bool pType);							

	bool		ParseAnimation			(IND_Animation *pNewAnimation, const char *pAnimationName);
	bool		IsDeclaredFrame			(const char *pFrameName, IND_Animation *pNewAnimation, int *pPos);
	
	void		WriteMessage			();
	void		InitVars				();
	void		FreeVars				();
};
/*@}*/

#endif // _IND_ANIMATIONMANAGER_
