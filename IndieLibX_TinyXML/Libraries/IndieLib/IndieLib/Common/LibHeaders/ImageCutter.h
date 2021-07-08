/*****************************************************************************************
/* File: ImageCutter.h
/* Desc: This class is used for cutting bitmap areas
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

#ifndef _IND_IMAGECUTTER_H_
#define _IND_IMAGECUTTER_H_

// ----- Includes -----

#include "IND_ImageManager.h"


// --------------------------------------------------------------------------------
//									 INFO_SURFACE
// --------------------------------------------------------------------------------

// Information about the surface
struct structInfoSurface
{
	int mWidthBlock;
	int mHeightBlock;
	int mBlocksX;
	int mBlocksY;
	int mSpareX;
	int mSpareY;
	float mNotUsedProportion;
	int mNumVertices;
	int mNumBlocks;
	int mWidthImage;
	int mHeightImage;
	int mWidthSpareImage;
	int mHeightSpareImage;
};
typedef struct structInfoSurface INFO_SURFACE;


// --------------------------------------------------------------------------------
//								 ImageCutter
// --------------------------------------------------------------------------------
			
class ImageCutter
{
public:

	// ----- Init/End -----

	ImageCutter		(): mOk(false)	{ }
	~ImageCutter		()			{ End(); }

	bool	Init    (IND_ImageManager *pImageManager, int pMaxTextureSize);
	void	End     ();
	bool	IsOk	()			 const { return mOk; }

	// ----- Public methods -----

	void FillInfoSurface   (IND_Image *pImage, INFO_SURFACE *pI, int pBlockSizeX, int pBlockSizeY);

	void CutBlock		   (unsigned char *pPtrBlock, 
							int pWidthImage, 
							int pWidthBlock, 
							int pHeightBlock,
							int mSpareX,
							int mSpareY,
							int pBpp,
							unsigned char **pNewBlock);


private:

	// ----- Private -----

	bool mOk;
	int mMaxTextureSize;

	// ----- Objects -----

	IND_ImageManager *mImageManager;

	// ----- Private methods -----

	int		PowerOfTow	(int pNum);

	void	InitVars	();
	void	FreeVars	();
};

#endif // _IND_IMAGECUTTER_H_
