/*****************************************************************************************
 * File: ImageCutter.cpp
 * Desc: This class is used for cutting bitmap areas
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

#include <ImageCutter.h>


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*
*************************
Init
*************************
*/
bool ImageCutter::Init (IND_ImageManager *pImageManager, int pMaxTextureSize)
{
	End ();
	InitVars ();

	mImageManager = pImageManager;
	mMaxTextureSize = pMaxTextureSize;

	mOk = true;
	return mOk;
}


/*
*************************
End
*************************
*/
void ImageCutter::End()
{
	if (mOk)
	{
		FreeVars ();
		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//							        Public methods
// --------------------------------------------------------------------------------

/*
*************************
Get the information necessary in order to cut the image
*************************
*/
void ImageCutter::FillInfoSurface (IND_Image *pImage,
								   INFO_SURFACE *pI,
								   int pBlockSizeX,
								   int pBlockSizeY)
{
	// Width and height of the image
	int mWidth	= pImage->GetWidth ();
	int mHeight = pImage->GetHeight ();

	// Block size is equal to the maximun allowed by the card
	int mBlockSize = mMaxTextureSize;

	// Width and height of the are to be rendered in
	pI->mWidthBlock   = PowerOfTow (mWidth);
	pI->mHeightBlock  = PowerOfTow (mHeight);

	// If width and height are higher than allowed, the block will be smaller
	if (pI->mWidthBlock  > mBlockSize)	pI->mWidthBlock  = mBlockSize;
	if (pI->mHeightBlock > mBlockSize)	pI->mHeightBlock = mBlockSize;

	// If the user has choosen a size block, we change the values
	if (pBlockSizeX != 0)
	{
		pI->mWidthBlock		= pBlockSizeX;
		pI->mHeightBlock	= pBlockSizeY;
	}

	// Number of blocks (width x height) in which we will divide the image into
	pI->mBlocksX = (int) mWidth  / pI->mWidthBlock;
	pI->mBlocksY = (int) mHeight / pI->mHeightBlock;

	// If the remainder is not equal to 0, width and height will not be power of two
	// and so there will be a spare. We add a block more because that.
	if (mWidth  % pI->mWidthBlock ) (pI->mBlocksX)++;
	if (mHeight % pI->mHeightBlock) (pI->mBlocksY)++;

	// Percentage not used
	// a and b are two rectangular areas of not used surface
	// mSpareX, mSpareY => right spare, up spare
	pI->mSpareX = ((pI->mBlocksX) * (pI->mWidthBlock)) - mWidth;
	int a = (pI->mSpareX) * ((pI->mBlocksY) * (pI->mHeightBlock));

	pI->mSpareY = ((pI->mBlocksY) * (pI->mHeightBlock)) - mHeight;
	int b =  mWidth * (pI->mSpareY);

	float mNotUsed = (float) a + b;

	// Surface of the image
	float mTotal = (float) ((pI->mBlocksX) * (pI->mWidthBlock)) * ((pI->mBlocksY) * (pI->mHeightBlock));

	// Percentage not used
	pI->mNotUsedProportion = ((mNotUsed * 100) / mTotal);

	// Number of vertex
	pI->mNumVertices = pI->mBlocksX * pI->mBlocksY * 4;

	// Number of blocks
	pI->mNumBlocks = pI->mBlocksX * pI->mBlocksY;

	// Width / height
	pI->mWidthImage  = pImage->GetWidth ();
	pI->mHeightImage = pImage->GetHeight ();

	// Useful area of the texture
	pI->mWidthSpareImage  = pI->mWidthBlock	 - pI->mSpareX;
	pI->mHeightSpareImage = pI->mHeightBlock - pI->mSpareY;
}


/*
*************************
Cuts a block of an image in memory, returns a pointer to the cut block
*************************
*/
void ImageCutter::CutBlock (unsigned char *pPtrBlock,
							int pWidthImage,
							int pWidthBlock,
							int pHeightBlock,
							int mSpareX,
							int mSpareY,
							int pBpp,
							unsigned char **pNewBlock)
{
	// ----- Allocate memory for the block  -----

	int mSizeBlock = pWidthBlock * pHeightBlock * pBpp;

		//LOG_FNLN;
		//LOG_PRINT("pWidthBlock=%x\n",  pWidthBlock);
		//LOG_PRINT("pHeightBlock=%x\n",  pHeightBlock);
		//LOG_PRINT("pBpp=%x\n",  pBpp);
		//LOG_PRINT("mSizeBlock=%x\n",  mSizeBlock);
	*pNewBlock = new unsigned char [mSizeBlock];
	stx_memset (*pNewBlock, 0, mSizeBlock);

	// ----- Copy the block -----

	// Initial position
	unsigned char *mInitPosition = *pNewBlock;

	// Cut
	for (int i = 0; i < pHeightBlock - mSpareY; i++)
	{
		// We cut one line
		stx_memcpy (*pNewBlock, pPtrBlock, (pWidthBlock * pBpp) - (mSpareX * pBpp));

		// Following line
		pPtrBlock += pWidthImage * pBpp;
		*pNewBlock += pWidthBlock * pBpp;
	}

	// Return the initial position
	*pNewBlock = mInitPosition;
}


// --------------------------------------------------------------------------------
//							        Private methods
// --------------------------------------------------------------------------------

/*
*************************
We search for a number power of two imediatly higer or equal to the received parameter
*************************
*/
int ImageCutter::PowerOfTow (int pNum)
{
	int mPower;
	for (mPower = 1; mPower < pNum; mPower *= 2);
	return mPower;
}

/*
*************************
Init vars
*************************
*/
void ImageCutter::InitVars ()
{

}


/*
*************************
Free memory
*************************
*/
void ImageCutter::FreeVars ()
{
}

