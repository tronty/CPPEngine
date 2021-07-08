/*****************************************************************************************
/* File: IND_SurfaceManager.h
/* Desc: Manager de superficies
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


#ifndef _IND_SURFACEMANAGER_
#define _IND_SURFACEMANAGER_

// ----- Includes -----

#include "IND_ImageManager.h"
#include "ImageCutter.h"
#include <list>
#include "IND_Surface.h"
#include "IND_Render.h"


// --------------------------------------------------------------------------------
//							     IND_SurfaceManager
// --------------------------------------------------------------------------------

/*!
\defgroup IND_SurfaceManager IND_SurfaceManager
\ingroup Managers
Manager of IND_Surface objects. Click in ::IND_SurfaceManager to see all the methods of this class.
*/
/*@{*/

/*!
This class stores 2d surfaces (IND_Surface) that can be inserted into a IND_Entity2d and rendered to 
the screen using IND_Entity2dManager::RenderEntities2d().

IND_Surface is the most used object
in the library due to its versatility. Suppose that we use \b IndieLib to develop a game.
The IND_Surface object will be the sprite of the main character or enemies, the background scroll, 
the tiles of the map, the markups of life, etc.

All graphic entities in \b IndieLib that are drawn in the screen as a surface, including 
text and animations, are internally managed by IND_Surface class.

One of the main characteristics of a IND_Surface is that it can have any desired size
(it doesn't matter the maximum texture of the graphic card and they don't need to be power of two). 
And you can use a big IND_Surface as s a  \b scroll, without worrying about the areas that are out of 
the screen (because blocks outside the viewport will be automatically discarded). Furthermore, 
to make scroll creation easier, <b>the size of blocks</b>, in which the surface is
divided when it is created from a ::IND_Image object or directly from a graphic file <b>can be 
specified</b>.

There are several types of surfaces (see ::IND_Type), each type is used for a different purpose:
- IND_ALPHA: Per pixel transparency using alpha channel
- IND_OPAQUE: Opaque

To <b>save memory</b> or to get <b>black and white images</b>, it is also possible to specify
differents surfaces qualities (see ::IND_Quality).

<BR>

\image html surfa2.jpg All the graphics entities in IndieLib are internally represented in surfaces
*/
class IND_Render;
class DLL_EXP IND_SurfaceManager
{
public:

	// ----- Init/End -----

	IND_SurfaceManager		(): mOk(false)	{ }
	~IND_SurfaceManager		()				{ End(); }

	bool	Init    (IND_ImageManager *pImageManager, IND_Render *pRender);
	void	End     ();
	bool	IsOk	() { return mOk; }
	
	// ----- Public methods -----

	// ----- Not specifying block size -----

	bool	Add						(IND_Surface	*pNewSurface,
									const char			*pName,
									IND_Type		pType,
									IND_Quality		pQuality);

	bool	 Add					(IND_Surface	*pNewSurface, 
									IND_Image		*pImage, 
									IND_Type		pType,
									IND_Quality		pQuality);

	bool	 Add					(IND_Surface	*pNewSurface, 
									const char			*pName, 
									IND_Type		pType,
									IND_Quality		pQuality,
									unsigned char pR,
									unsigned char pG,
									unsigned char pB);



	// ----- Specifying block size ----- 

	
	bool Add			(IND_Surface	*pNewSurface,
						const char			*pName,
						int				pBlockSize,
						IND_Type		pType,
						IND_Quality		pQuality);

	bool Add			(IND_Surface	*pNewSurface,
						IND_Image		*pImage,
						int				pBlockSize,
						IND_Type		pType,
						IND_Quality		pQuality);

	bool Add			(IND_Surface	*pNewSurface, 
						const char			*pName,
						int				pBlockSize,
						IND_Type		pType,
						IND_Quality		pQuality,
						unsigned char pR,
						unsigned char pG,
						unsigned char pB);

	bool Clone			(IND_Surface *pNewSurface, IND_Surface *pSurfaceToClone);

	bool Delete			(IND_Surface *pSu);

private:

	// ----- Private -----
	
	bool mOk;

	// ----- Objects -----

	ImageCutter *mCutter;
	IND_ImageManager *mImageManager;
	IND_Render *mRender;

	// ----- Containers -----

	list <IND_Surface*> *mListSurfaces;
	
	// ----- Private methods -----

	bool	AddMain						(IND_Surface	*pNewSurface, 
										IND_Image		*pImage, 
										int				pBlockSizeX,
										int				pBlockSizeY,
										IND_Type		pType, 
										IND_Quality		pQuality);

	bool	CalculeAxis					(IND_Surface *pSu, 
										float pAxisX, 
										float pAxisY, 
										int *pAxisCalX, 
										int *pAxisCalY);
	
	void				AddToList		(IND_Surface *pNewImage);
	void				DelFromList		(IND_Surface *pSu);

	FORMAT			GetSourceFormat	(IND_Image *pImage);
	FORMAT			GetDestFormat	(IND_Surface *pNewSurface, IND_Type pType, IND_Quality pQuality);
	
	void				PushVertex		(CUSTOMVERTEX2D *pVertices, 
										int pPosVert,
										int pVx, 
										int pVy, 
										int pVz,
										float pU,
										float pV);

	void				Push4Vertices	(CUSTOMVERTEX2D *pVertices, 
										int pPosVert,
										int pPosX, 
										int pPosY, 
										int pPosZ, 
										int pWidthBlock, 
										int pHeightBlock,
										float pU,
										float pV);

	TextureID	CreateTexture	(unsigned char *pImage,
										int pBlockWidth,
										int pBlockHeight,
										int pSrcBpp,
										FORMAT pSrcFormat,
										FORMAT pDstFormat);

	void				WriteMessage	();
	void				InitVars		();
	void				FreeVars		();
};
/*@}*/
	
#endif // _IND_SURFACEMANAGER_
