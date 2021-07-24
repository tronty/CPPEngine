/*****************************************************************************************
/* File: IND_Entity2dManager.h
/* Desc: Manager de entidades. Una entidad contiene el un objeto gráfico y gestiona
/* sus atributos. Además, facilitan el uso de animaciones.
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


#ifndef _IND_ENTITY2DMANAGER_
#define _IND_ENTITY2DMANAGER_

// ----- Includes -----

#ifndef SWIG
#include <algorithm>
#endif
#include "IND_SurfaceManager.h"
#include "IND_AnimationManager.h"
#include "IND_Render.h"
#include "CollisionParser.h"
#include "IND_Entity2d.h"

// ----- Defines -----

#define NUM_LAYERS 64

// --------------------------------------------------------------------------------
//									IND_Entity2dManager
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Entity2dManager IND_Entity2dManager
\ingroup EntityManagers
Manager of ::IND_Entity2d objects. Click in ::IND_Entity2dManager to see all the methods of this class.
*/
/*@{*/ 

/*!
Manager of ::IND_Entity2d objects. Used for storing these type of objects. 

Other uses of this class:
- Check collisions between the entities stored
- Function for drawing all the entities stored
*/
class DLL_EXP IND_Entity2dManager
{
public:
	
	// ----- Init/End -----

	IND_Entity2dManager			(): mOk(false)	{ }
	~IND_Entity2dManager		()				{ End(); }

	bool	Init    (IND_Render *pRender);
	void	End     ();
	bool	IsOk	()			 const { return mOk; }
					
	// ----- Methods -----
	
	bool			Add						(IND_Entity2d *pNewEntity2d);
	bool			Add						(int pLayer, IND_Entity2d *pNewEntity2d);
	bool			Delete					(IND_Entity2d *pEn);

	INLINE void		RenderEntities2d		();
	INLINE void		RenderEntities2d		(int pLayer);
	INLINE void		RenderCollisionAreas	(unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA);
	INLINE void		RenderCollisionAreas	(int pLayer, unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA);
	INLINE void		RenderGridAreas			(unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA);
	INLINE void		RenderGridAreas			(int pLayer, unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA);

	INLINE bool		IsCollision				(IND_Entity2d *pEn1, const char *pId1, IND_Entity2d *pEn2, const char *pId2);
private:

	// ----- Private -----

	bool mOk;

	IND_Render *mRender;

	// ----- Containers -----

	vector <IND_Entity2d*> *mListEntities2d  [NUM_LAYERS];

	// ----- Private methods -----

	bool IsCollision		(list <BOUNDING_COLLISION*> *pBoundingList1, list <BOUNDING_COLLISION*> *pBoundingList2,
							const char *pId1, const char *pId2,
							IND_Matrix pMat1, IND_Matrix pMat2,
							float pScale1, float pScale2);

	bool IsNullMatrix		(IND_Matrix pMat);

	void AddToList			(int pLayer, IND_Entity2d *pNewEntity2d);

	void WriteMessage		();
	void InitVars			();
	void FreeVars			();
	
};
/*@}*/
#ifdef BUILD_REPLAY
#include <IND_Viewport.h>
#if 1
struct ReplayFrame
	{
		ReplayFrame(){}
		~ReplayFrame(){}
		ReplayFrame(const ReplayFrame& rhs)
		{
			mViewport2d=rhs.mViewport2d;
			mCamera2d=rhs.mCamera2d;
			for(unsigned int i=0;i<NUM_LAYERS;i++)
			mEntities2d[i]=rhs.mEntities2d[i];
		}
		IND_Viewport mViewport2d;
		IND_Camera2d mCamera2d;
		std::vector < IND_Entity2d* > mEntities2d [NUM_LAYERS];
	};
#endif
#endif
#endif // _IND_ENTITY2DMANAGER_
