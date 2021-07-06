/*****************************************************************************************
 * File: IND_SurfaceManager.cpp
 * Desc: Manager de superficies
 *****************************************************************************************/

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

// ----- Includes -----
#define __ImageLib__ 1
#include <CIndieLib.h>
#include <IND_Render.h>
#include <IND_SurfaceManager.h>

// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b parameters:

\arg \b pImageManager			Pointer to IND_ImageManager correctly initialized
\arg \b pRender					Pointer to IND_Render correctly initialized

\b Operation:

This function returns 1 (true) when the manager is correctly initialized.
It should be called before using any method.
*/
bool IND_SurfaceManager::Init (IND_ImageManager *pImageManager, IND_Render *pRender)
{
	End ();
	InitVars ();

	GetDebugAPI()->Header ("Initializing SurfaceManager", 5);

	// Checking IND_Render
	if (pRender->IsOk())
	{
		GetDebugAPI()->Header ("Checking IND_Render", 1);
		mRender = pRender;
	}
	else
	{
		GetDebugAPI()->Header ("IND_Render is not correctly initialized", 2);
		mOk = false;
		return mOk;
	}

	
	// Checking IND_ImageManager
	if (pImageManager->IsOk())
	{
		GetDebugAPI()->Header ("Checking IND_ImageManager", 1);
		mImageManager = pImageManager;

		
		//LOG_PRINT("pImageManager=%x\n",pImageManager);
		//LOG_PRINT("mImageManager=%x\n",mImageManager);

		// Image cutter
		mCutter = new ImageCutter;
		mCutter->Init (mImageManager, mRender->GetMaxTextureSize());
		//LOG_PRINT("mCutter=%x\n",mCutter);
		//LOG_PRINT("mImageManager=%x\n",mImageManager);
		//LOG_PRINT("mRender=%x\n",mRender);
		//LOG_PRINT("mRender->GetMaxTextureSize()=%x\n",mRender->GetMaxTextureSize());

		mOk = true;

		GetDebugAPI()->Header ("SurfaceManager OK", 6);
		//stx_exit(0);
	}
	else
	{
		GetDebugAPI()->Header ("ImageManager is not correctly initialized", 2);
		mOk = false;
	}

	return mOk;
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_SurfaceManager::End()
{
	if (mOk)
	{
		GetDebugAPI()->Header ("Finalizing SurfaceManager", 5);
		GetDebugAPI()->Header ("Freeing surfaces" , 5);
		FreeVars ();
		GetDebugAPI()->Header ("Surfaces freed", 6);
		GetDebugAPI()->Header ("IND_SurfaceManager finalized", 6);

		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//									 Public methods
// --------------------------------------------------------------------------------

/*!
\b parameters:

\arg \b pNewSurface				Pointer to a new surface object
\arg \b (char*)pName.c_str()					Name of the file that contains the image
\arg \b pType					Type of surface (see ::IND_Type)
\arg \b pQuality                Quality of the surface (see ::IND_Quality)

\b Operation:

This function returns 1 (true) if the parameter surface object exists and it is added
by loading the image directly from the file.

Graphic formats supported (Thanks to http://openil.sourceforge.net):
bmp, png, tga, jpg and pcx.
*/
bool IND_SurfaceManager::Add	(IND_Surface	*pNewSurface,
                                const char			*apName,
								IND_Type		pType,
								IND_Quality		pQuality)
{

	// Loads the image

	std::string pName =(apName);
	/*
	//LOG_PRINT("apName = %s\n", apName);
	//LOG_PRINT("(char*)pName.c_str()=%s\n",(char*)pName.c_str());
*/

	IND_Image *mNewImage = new IND_Image;
/*
    	//LOG_PRINT("mImageManager = %x\n", mImageManager);
	//LOG_PRINT("mImageManager = %x\n", mImageManager);
	//LOG_PRINT("mNewImage = %x\n", mNewImage);
	//LOG_PRINT("(char*)pName.c_str()=%s\n",(char*)pName.c_str());
*/

	if (!
		//mImageManager
		mImageManager
		->Add (mNewImage, pName.c_str()))
		return 0;

	// Surface creation
	if (!AddMain (pNewSurface, mNewImage, 0, 0, pType, pQuality))
	{

		//LOG_PRINT("Surface creation fail\n");
		#ifndef OMITDELETE		
		mImageManager->Delete (mNewImage);
		#endif

		return 0;
	}
	//LOG_PRINT("Surface creation success\n");

	// Free image
	#ifndef OMITDELETE
	mImageManager->Delete (mNewImage);
	#endif

	return 1;
}


/*!
\b parameters:

\arg \b pNewSurface				Pointer to a new surface object
\arg \b pImage					Pointer to a ::IND_Image object
\arg \b pType					Type of surface (see ::IND_Type)
\arg \b pQuality                Quality of surface (see ::IND_Quality)

\b Operation:

This function returns 1 (true) if the parameter surface object exists and it is added
to the manager, loading the image directly from a ::IND_Image object.
*/
bool IND_SurfaceManager::Add	(IND_Surface	*pNewSurface,
								IND_Image		*pImage,
								IND_Type		pType,
								IND_Quality		pQuality)
{
;
	// Surface creation
	if (!AddMain (pNewSurface, pImage, 0, 0, pType, pQuality))
		return 0;
;
	return 1;
}


/*!
\b parameters:

\arg \b pNewSurface				Pointer to a new surface object
\arg \b (char*)pName.c_str()					Name of the file that contains the image
\arg \b pType					Type of surface (see ::IND_Type)
\arg \b pQuality                Quality of the surface (see ::IND_Quality)
\arg <b>pR, pG, pB</b>			Color from which the colorkey will be applied, this areas will become transparent.


\b Operation:

This function returns 1 (true) if the parameter surface object exists and it is added
satisfactory loading the image directly from the file and all the colors equal to RGB=(pR, pG, pB), will
become transparent.

Graphic formats supported (Thanks to http://openil.sourceforge.net):
bmp, png, tga, jpg and pcx.
*/
bool IND_SurfaceManager::Add	(IND_Surface	*pNewSurface,
                                const char			*apName,
								IND_Type		pType,
								IND_Quality		pQuality,
								unsigned char			pR,
								unsigned char			pG,
								unsigned char			pB)
{
	std::string pName =(apName);
	//LOG_PRINT("(char*)pName.c_str()=%s\n",(char*)pName.c_str());

	// Image loading
	IND_Image *mNewImage = new IND_Image;

	if (!mImageManager->Add (mNewImage, pName.c_str()))
		return 0;
;
	// Color key
	mNewImage->SetAlpha (pR, pG, pB);
;
	// Surface creation
	if (!AddMain (pNewSurface, mNewImage, 0, 0, pType, pQuality))
	{
;
		#ifndef OMITDELETE
		mImageManager->Delete (mNewImage);
		#endif
		return 0;
	}
;
	// Free image
	#ifndef OMITDELETE
	mImageManager->Delete (mNewImage);
	#endif

	return 1;
}


/*!
\b parameters:

\arg \b pNewSurface				Pointer to a new surface object
\arg \b (char*)pName.c_str()					Name of the file that contains the image
\arg \b pBlockSize				Width of the blocks
\arg \b pType					Type of surface (see ::IND_Type)
\arg \b pQuality                Quality of surface (see ::IND_Quality)

\b Operation:

This function returns 1 (true) if the parameter surface object exists and it is added satisfactory
to the manager loading the image directly from a file. This method is useful for creating scrolls because we
can specify the width of the block. Blocks outside the viewport will be automatically discarded.

Graphic formats supported (Thanks to http://openil.sourceforge.net):
bmp, png, tga, jpg and pcx.
*/
bool IND_SurfaceManager::Add	(IND_Surface	*pNewSurface,
                                const char			*apName,
								int				pBlockSize,
								IND_Type		pType,
								IND_Quality		pQuality)
{
;
	std::string pName =(apName);
	//LOG_PRINT("(char*)pName.c_str()=%s\n",(char*)pName.c_str());
;
	IND_Image *mNewImage = new IND_Image;
;
	if (!mImageManager->Add (mNewImage, pName.c_str()))
		return 0;
;
	if (!AddMain (pNewSurface, mNewImage, pBlockSize, pBlockSize, pType, pQuality))
		return 0;
;
	#ifndef OMITDELETE
	mImageManager->Delete (mNewImage);
	#endif
;
	return 1;
}


/*!
\b parameters:

\arg \b pNewSurface				Pointer to a new surface object
\arg \b pImage					Pointer to a  ::IND_Image object
\arg \b pBlockSize				Width of the blocks.
\arg \b pType					Type of surface (see ::IND_Type)
\arg \b pQuality                Quality of surface (see ::IND_Quality)

\b Operation:



This function returns 1 (true) if the parameter surface object exists and it is added
to the manager loading the image directly from a file. This method is useful for creating scrolls because we
can specify the width of the block. Blocks outside the viewport will be automatically discarded.
*/
bool IND_SurfaceManager::Add	(IND_Surface	*pNewSurface,
								IND_Image		*pImage,
								int				pBlockSize,
								IND_Type		pType,
								IND_Quality		pQuality)
{
;
	if (!AddMain (pNewSurface, pImage, pBlockSize, pBlockSize, pType, pQuality))
		return 0;
;
	return 1;
}


/*!
\b parameters:

\arg \b pNewSurface				Pointer to a new surface object
\arg \b (char*)pName.c_str()					Name of the file that contains the image
\arg \b pBlockSize				Width of the blocks
\arg \b pType					Type of surface (see ::IND_Type)
\arg \b pQuality                Quality of surface (see ::IND_Quality)
\arg <b>pR, pG, pB</b>			Color from which the colorkey will be applied, this areas will become transparent.

\b Operation:

This function returns 1 (true) if the parameter surface object exists and it is added satisfactory
to the manager loading the image directly from a file. This method is useful for creating scrolls because we
can specify the width of the block. Blocks outside the viewport will be automatically discarded.

Graphic formats supported (Thanks to http://openil.sourceforge.net):
bmp, png, tga, jpg and pcx.
*/
bool IND_SurfaceManager::Add	(IND_Surface	*pNewSurface,
                                const char			*apName,
								int				pBlockSize,
								IND_Type		pType,
								IND_Quality		pQuality,
								unsigned char pR,
								unsigned char pG,
								unsigned char pB)
{
	// Image loading
;
	std::string pName =(apName);
	//LOG_PRINT("(char*)pName.c_str()=%s\n",(char*)pName.c_str());
;
	IND_Image *mNewImage = new IND_Image;
;
	if (!mImageManager->Add (mNewImage, pName.c_str()))
		return 0;
;
	// Color key
	mNewImage->SetAlpha (pR, pG, pB);
;
	// Surface creation
	if (!AddMain (pNewSurface, mNewImage, pBlockSize, pBlockSize, pType, pQuality))
	{
;
		#ifndef OMITDELETE
		mImageManager->Delete (mNewImage);
		#endif
;
		return 0;
	}
;
	// Free image
	#ifndef OMITDELETE
	mImageManager->Delete (mNewImage);
	#endif
;
	return 1;
}

/*!
\b parameters:

\arg \b pNewSurface				Pointer to a new surface object
\arg \b pSurfaceToClone			Surface we want to clone

\b Operation:

This function returns 1 (true) if the parameter surface object exists and a new surface is created
cloned from a previous existing one. The new surface will share the texture data from the "father"
but will have it's own grid data. Show, if you want to have several ::IND_Surface objects
with different grid assigned (see IND_Surface::SetGrid()) this is the way to go.
*/
bool IND_SurfaceManager::Clone (IND_Surface *pNewSurface, IND_Surface *pSurfaceToClone)
{
	// Copy attributes
	pNewSurface->mSurface.mAttributes.mType = pSurfaceToClone->mSurface.mAttributes.mType;
	pNewSurface->mSurface.mAttributes.mQuality = pSurfaceToClone->mSurface.mAttributes.mQuality;
	pNewSurface->mSurface.mAttributes.mWidth = pSurfaceToClone->mSurface.mAttributes.mWidth;
	pNewSurface->mSurface.mAttributes.mHeight = pSurfaceToClone->mSurface.mAttributes.mHeight;
	pNewSurface->mSurface.mAttributes.mBlocksX = pSurfaceToClone->mSurface.mAttributes.mBlocksX;
	pNewSurface->mSurface.mAttributes.mBlocksY = pSurfaceToClone->mSurface.mAttributes.mBlocksY;
	pNewSurface->mSurface.mAttributes.mSpareX = pSurfaceToClone->mSurface.mAttributes.mSpareX;
	pNewSurface->mSurface.mAttributes.mSpareY = pSurfaceToClone->mSurface.mAttributes.mSpareY;
	pNewSurface->mSurface.mAttributes.mNumBlocks = pSurfaceToClone->mSurface.mAttributes.mNumBlocks;
	pNewSurface->mSurface.mAttributes.mWidthBlock = pSurfaceToClone->mSurface.mAttributes.mWidthBlock;
	pNewSurface->mSurface.mAttributes.mHeightBlock = pSurfaceToClone->mSurface.mAttributes.mHeightBlock;
	pNewSurface->mSurface.mAttributes.mIsHaveSurface = pSurfaceToClone->mSurface.mAttributes.mIsHaveSurface;
	pNewSurface->mSurface.mAttributes.mIsHaveGrid = pSurfaceToClone->mSurface.mAttributes.mIsHaveGrid;

	// Reference to texture
	pNewSurface->mSurface.mTexturesArray =  pSurfaceToClone->mSurface.mTexturesArray;

	// Copy vertex data
	int mNumVertices = pSurfaceToClone->GetBlocksX() * pSurfaceToClone->GetBlocksY() * 4;
	pNewSurface->mSurface.mVertexArray = new CUSTOMVERTEX2D [mNumVertices];

	for (int i = 0; i < mNumVertices; i++)
		pNewSurface->mSurface.mVertexArray [i] = pSurfaceToClone->mSurface.mVertexArray[i];

	return 1;
}


/*!
\b parameters:

\arg \b pSu				Pointer to a new surface object

\b Operation:

This function returns 1 (true) if the parameter surface object exists and it is satisfactory deleted from the manager.
*/
bool IND_SurfaceManager::Delete (IND_Surface *pSu)
{
	GetDebugAPI()->Header ("Freeing surface", 5);

	if (!mOk || !pSu)
	{
		WriteMessage ();
		return 0;
	}

	// Search object
	bool mIs = 0;
	list <IND_Surface*>::iterator mSurfaceListIter;
	for (mSurfaceListIter  = mListSurfaces->begin();
		 mSurfaceListIter != mListSurfaces->end();
		 mSurfaceListIter++)
	{
		if ((*mSurfaceListIter) == pSu)
		{
			mIs = 1;
			break;
		}
	}

	if (!mIs)
	{
		WriteMessage ();
		return 0;
	}

	// ----- Free object -----

	GetDebugAPI()->Header	("Name:", 3);
	//GetDebugAPI()->DataInt (int(&pSu->mSurface), 1);

	// Quit from list
	DelFromList (pSu);

	pSu->mSurface.mAttributes.mIsHaveSurface = 0;

	// Free textures
	//for (int i = 0; i <  pSu->GetNumTextures(); i++)	
	//	pSu->mSurface.mTexturesArray [i].mTexture->Release ();

	// Free vertex buffer
	DisposeArray (pSu->mSurface.mVertexArray);

	GetDebugAPI()->Header ("Ok", 6);
	
	return 1;
}

// --------------------------------------------------------------------------------
//										Private methods
// --------------------------------------------------------------------------------

/*
*************************
Add principal (All other Add use this)
Add main (All public Add use this)
*************************
*/
bool IND_SurfaceManager::AddMain	(IND_Surface	*pNewSurface,
									IND_Image		*pImage,
									int				pBlockSizeX,
									int				pBlockSizeY,
									IND_Type		pType,
									IND_Quality		pQuality)
{
//LOG_FNLN;
//LOG_PRINT("pType=%x\n",pType);
//LOG_PRINT("pQuality=%x\n",pQuality);

/*
//LOG_PRINT("pNewSurface=%x\n",pNewSurface);
//LOG_PRINT("pImage=%x\n",pImage);


//LOG_PRINT("pBlockSizeX=%x\n",pBlockSizeX);
//LOG_PRINT("pBlockSizeY=%x\n",pBlockSizeY);
//LOG_PRINT("pType=%x\n",pType);
//LOG_PRINT("pQuality=%x\n",pQuality);

//LOG_PRINT("mImageManager=%x\n",mImageManager);
//LOG_PRINT("mRender=%x\n",mRender);

if(mImageManager->IsOk())
//LOG_PRINT("mImageManager is Ok\n");
else
//LOG_PRINT("mImageManager is not Ok\n");
if(mRender->IsOk())
//LOG_PRINT("mRender is Ok\n");
else
//LOG_PRINT("mRender is not Ok\n");
*/

	GetDebugAPI()->Header ("Creating surface", 5);

	if (!mOk || !pNewSurface || !pImage)
	{

		WriteMessage ();

		return 0;
	}

	GetDebugAPI()->Header ("From image:", 3);

	GetDebugAPI()->DataChar (pImage->GetName (), 1);

	GetDebugAPI()->Header ("File name:", 3);

	//GetDebugAPI()->DataInt (int(&pNewSurface->mSurface), 1);

//LOG_PRINT("Name=%s\n",pImage->GetName ());


	// Fill attributes
	pNewSurface->mSurface.mAttributes.mType		= pType;

	pNewSurface->mSurface.mAttributes.mQuality  = pQuality;


	// ----- Check IND_Type and IND_Quality of the image and choose a D3D source and destination format -----

	// ----- Source format (Image)------

	int mSrcBpp = pImage->GetBpp ();// / 8;
	
		//LOG_FNLN;
		//LOG_PRINT("mSrcBpp=%x\n",  mSrcBpp);
		//stx_exit(0);
									// Source bpp
	FORMAT mSrcFormat = GetSourceFormat (pImage);						// D3D Format

	// ----- Destination format (TextureOTL) ------

	FORMAT mDstFormat = GetDestFormat (pNewSurface, pType, pQuality);	// D3D Format

	// ----- Cutting blocks -----

	// ----- Obtaining info in order to store the image -----

/*
//LOG_PRINT("mSrcFormat=%x\n",mSrcFormat);
//LOG_PRINT("mDstFormat=%x\n",mDstFormat);
//LOG_PRINT("this=%x\n",this);
//LOG_PRINT("pImage=%x\n",pImage);
*/
if(mCutter==0)
{
		// Image cutter
		mCutter = new ImageCutter;
		mCutter->Init (mImageManager, mRender->GetMaxTextureSize());
/*
		//LOG_PRINT("mCutter=%x\n",mCutter);
		//LOG_PRINT("mImageManager=%x\n",mImageManager);
		//LOG_PRINT("mRender=%x\n",mRender);
		//LOG_PRINT("mRender->GetMaxTextureSize()=%x\n",mRender->GetMaxTextureSize());
*/
}

	INFO_SURFACE mI0;

//LOG_PRINT("mCutter=%x\n",mCutter);
//LOG_PRINT("&mI0=%x\n",&mI0);

	mCutter->FillInfoSurface   (pImage, &mI0, pBlockSizeX, pBlockSizeY);
//LOG_PRINT("pBlockSizeX=%x\n",pBlockSizeX);
//LOG_PRINT("pBlockSizeY=%x\n",pBlockSizeY);


	// Fill attributes
	pNewSurface->mSurface.mAttributes.mBlocksX			= mI0.mBlocksX;
	pNewSurface->mSurface.mAttributes.mBlocksY			= mI0.mBlocksY;
	pNewSurface->mSurface.mAttributes.mSpareX			= mI0.mSpareX;
	pNewSurface->mSurface.mAttributes.mSpareY			= mI0.mSpareY;
	pNewSurface->mSurface.mAttributes.mNumBlocks		= mI0.mBlocksX * mI0.mBlocksY;
	pNewSurface->mSurface.mAttributes.mNumTextures		= mI0.mBlocksX * mI0.mBlocksY;
	pNewSurface->mSurface.mAttributes.mIsHaveGrid		= 0;
	pNewSurface->mSurface.mAttributes.mWidthBlock		= mI0.mWidthBlock;
	pNewSurface->mSurface.mAttributes.mHeightBlock		= mI0.mHeightBlock;
	pNewSurface->mSurface.mAttributes.mWidth			= mI0.mWidthImage;
	pNewSurface->mSurface.mAttributes.mHeight			= mI0.mHeightImage;
	pNewSurface->mSurface.mAttributes.mIsHaveSurface	= 1;

	// Allocate space for the vertex buffer
	// This buffer will be used for drawing the IND_Surface using DrawPrimitiveUp
	pNewSurface->mSurface.mVertexArray = new CUSTOMVERTEX2D [mI0.mNumVertices];

	// Each block, needs a texture. We use an array of textures in order to store them. 
	pNewSurface->mSurface.mTexturesArray = new IND_Surface::TEXTURES [mI0.mBlocksX * mI0.mBlocksY];

	// Current position of the vertex
	int mPosX = 0;
	int mPosY = mI0.mHeightImage;
	int mPosZ = 0;

	// Position in wich we are storing a vertex
	int mPosVer = 0;

	// Position in wich we are storing a texture
	int mCont = 0;

	// Image pointer
	
	unsigned char *mPtrBlock = pImage->GetPointer ();
	
//LOG_PRINT("mPtrBlock=%d\n",mPtrBlock);

	// Vars
	int mActualWidthBlockX;

	int mActualHeightBlockY;

	float mActualU;
	float mActualV;
	int mActualSpareX;
	int mActualSpareY;

	// ----- Cutting blocks -----

	// We iterate the blocks starting from the lower row
	// We MUST draw the blocks in this order, because the image starts drawing from the lower-left corner
	for (int i = mI0.mBlocksY; i > 0; i--)
	{
		
		for (int j = 1; j < mI0.mBlocksX + 1; j++)
		{
			
			// ----- Vertices position of the block -----

			// There are 4 types of blocks: the ones of the right column, the ones of the upper row,
			// the one of the upper-right corner and the rest of blocks.
			// Depending on the block, we store the vertices one way or another.

			// Normal block
			if (i != 1 && j !=  mI0.mBlocksX)
			{
				mActualWidthBlockX	= mI0.mWidthBlock;
				mActualHeightBlockY	= mI0.mHeightBlock;
				mActualU			= 1.0f;
				mActualV			= 1.0f;
				mActualSpareX		= 0;
				mActualSpareY		= 0;
			}

			// The ones of the right column
			if (i != 1 && j ==  mI0.mBlocksX)
			{
				mActualWidthBlockX	= mI0.mWidthSpareImage;
				mActualHeightBlockY	= mI0.mHeightBlock;
				mActualU			= (float) mI0.mWidthSpareImage / mI0.mWidthBlock;
				mActualV			= 1.0f;
				mActualSpareX		= mI0.mSpareX;
				mActualSpareY		= 0;
			}

			// The ones of the upper row
			if (i == 1 && j !=  mI0.mBlocksX)
			{
				mActualWidthBlockX	= mI0.mWidthBlock;
				mActualHeightBlockY	= mI0.mHeightSpareImage;
				mActualU			= 1.0f;
				mActualV			= (float) mI0.mHeightSpareImage / mI0.mHeightBlock;
				mActualSpareX		= 0;
				mActualSpareY		= mI0.mSpareY;
			}

			// The one of the upper-right corner
			if (i == 1 && j ==  mI0.mBlocksX)
			{
				mActualWidthBlockX	= mI0.mWidthSpareImage;
				mActualHeightBlockY	= mI0.mHeightSpareImage;
				mActualU			= (float) mI0.mWidthSpareImage / mI0.mWidthBlock;
				mActualV			= (float) mI0.mHeightSpareImage / mI0.mHeightBlock;
				mActualSpareX		= mI0.mSpareX;
				mActualSpareY		= mI0.mSpareY;
			}

			// ----- Block creation (using the position, uv coordiantes and texture) -----

			// We push into the buffer the 4 vertices of the block
			Push4Vertices  (pNewSurface->mSurface.mVertexArray,			// Pointer to the buffer
							mPosVer,									// Position in wich we are storing a vertex 
							mPosX,										// x
							mPosY,										// y
							mPosZ,										// z
							mActualWidthBlockX,							// Block width
							mActualHeightBlockY,						// Block height
							mActualU,									// U mapping coordinate
							mActualV);								// V mapping coordinate

			// Cuts a block from the image (bitmap)
			unsigned char *mTempBlock = 0;
			mCutter->CutBlock  (mPtrBlock,
								mI0.mWidthImage,
								mI0.mWidthBlock,
								mI0.mHeightBlock,
								mActualSpareX,
								mActualSpareY,
								mSrcBpp,
								&mTempBlock);

			// We create a texture using the cutted bitmap block
			pNewSurface->mSurface.mTexturesArray [mCont].mTexture = CreateTexture (	mTempBlock,
																					mI0.mWidthBlock,
																					mI0.mHeightBlock,
																					mSrcBpp,
																					mSrcFormat,
																					mDstFormat);

			// Free the bitmap cutted block
			DisposeArray (mTempBlock);

			// ----- Advance -----

			// Increase in 4 vertices the position (we have already stored a quad)
			mPosVer += 4;

			// Increase the texture counter (we have alread stored one texture)
			mCont++;

			// ----- Column change -----

			// We point to the next block (memory and screen)
			mPosX += mI0.mWidthBlock;
			mPtrBlock += mI0.mWidthBlock * mSrcBpp;
		}

		// ----- Row change -----

		// We point to the next block  (memory and screen)
		mPosX = 0;
		mPtrBlock -= mI0.mSpareX * mSrcBpp;

		// If this block is in the last row, we take in count the spare areas.
		if (i == 1)
		{
			mPosY -= mI0.mSpareY;
			mPtrBlock += (mI0.mWidthImage * mSrcBpp) * (mI0.mSpareY - 1);
		}
		else
		{
			mPosY -= mI0.mHeightBlock;
			mPtrBlock += (mI0.mWidthImage * mSrcBpp) * (mI0.mHeightBlock - 1);
		}
	}

	// ----- Puts the objet into the manager  -----

	AddToList (pNewSurface);

	// ----- GetDebugAPI() -----


	GetDebugAPI()->Header ("Type:", 3);

	GetDebugAPI()->DataChar (pNewSurface->GetTypeChar (), 1);

	GetDebugAPI()->Header ("Quality:", 3);
	GetDebugAPI()->DataChar (pNewSurface->GetQualityChar (), 1);

	GetDebugAPI()->Header ("Image size:", 3);
	//GetDebugAPI()->DataInt (mI0.mWidthImage, 0);
	GetDebugAPI()->DataChar ("x", 0);
	//GetDebugAPI()->DataInt (mI0.mHeightImage, 1);

	GetDebugAPI()->Header ("Block size:", 3);
	//GetDebugAPI()->DataInt (mI0.mWidthBlock, 0);
	GetDebugAPI()->DataChar ("x", 0);
	//GetDebugAPI()->DataInt (mI0.mHeightBlock, 1);

	GetDebugAPI()->Header ("Number of blocks:", 3);
	//GetDebugAPI()->DataInt (mI0.mBlocksX, 0);
	GetDebugAPI()->DataChar ("x", 0);
	//GetDebugAPI()->DataInt (mI0.mBlocksY, 1);

	GetDebugAPI()->Header ("Spare (Right | Down):", 3);
	//GetDebugAPI()->DataInt (mI0.mSpareX, 0);
	GetDebugAPI()->DataChar ("x", 0);
	//GetDebugAPI()->DataInt (mI0.mSpareY, 1);

	GetDebugAPI()->Header ("Not used percentage:", 3);
	GetDebugAPI()->DataFloat (mI0.mNotUsedProportion, 0);
	GetDebugAPI()->DataChar ("%", 1);

	GetDebugAPI()->Header("Surface created", 6);

	return 1;
}


/*
*************************
This function returns 1 (true) if the parameter surface object exists and it returns in
 \b pAxisCalX and \b pAxisCalY the values necessary to specify the hotspot when we
 want to call the function IND_Render::SetTransform() (before drawing the surface).

This function is used to change the point of reference of the image. The point of reference
 or hotspot is an imaginary point from which the translation, rotation and range transformations
 are applied. This way we can originate the rotation of asurface over its center, over
 one of its vertexes or over any intermediate location.

To seat the hotspot we \b usepAxisX and \b pAxisY that have a range from 0 to 1, and refer
respectively to the horizontal position and vertical position of the surface. So, (0.5f, 0.5f)
is the center of the image and  (1.0f, 0.0f) is the top right corner, etc.

The results values (pAxisCalX and pAxisCalY) should be stored and used later as parameters
to the function IND_Render::SetTransform(), in such way, when the image is drawn
the hotspot specified os used.

When we use two null values for the function IND_Render::SetTransform() the default values
would be used, the default hotspot is the top left corner of the object.

This function can be very difficult to use with each surface separately. It is optimal
to apply it over a list of surfaces that the user can manage. An other option of using this
function is using  ::IND_Entity2dManager to manage all the objects of  IndieLib and its
attributes efficiently.
*************************
*/
bool IND_SurfaceManager::CalculeAxis	(IND_Surface *pSu,
										float pAxisX,
										float pAxisY,
										int *pAxisCalX,
										int *pAxisCalY)
{
	if (!mOk || !pSu)
	{
		return 0;
	}

	if (pAxisX < 0.0f || pAxisX > 1.0f)
		pAxisX = 0.0f;

	if (pAxisY < 0.0f || pAxisY > 1.0f)
		pAxisY = 0.0f;


	*pAxisCalX = (int) (pAxisX * pSu->mSurface.mAttributes.mWidth	* -1);
	*pAxisCalY = (int) (pAxisY * pSu->mSurface.mAttributes.mHeight	* -1);

	return 1;
}


/*
*************************
Receiving a IND_Image returns a D3D format for the source (the image source for creating the texture)
*************************
*/
FORMAT IND_SurfaceManager::GetSourceFormat (IND_Image *pImage)
{
	switch (pImage->GetFormatInt ())
	{
		case IND_COLOUR_INDEX:
		{
			return FORMAT_I8;
		}

		case IND_RGB:

		{
			pImage->Convert (IND_BGR);
			return FORMAT_RGB8;
		}

		case IND_RGBA:
		{
			pImage->Convert (IND_BGRA);
			return FORMAT_RGBA8;
		}

		case IND_BGR:

		{
			return FORMAT_RGB8;
		}

		case IND_BGRA:
		{
			return FORMAT_RGBA8;
		}

		// By default, BGR
		default:
		{
			pImage->Convert (IND_BGR);
			GetDebugAPI()->Header ("IND_Type unknown, using IND_BGR instead" , 4);
			return FORMAT_RGB8;
		}
	}
}


/*
*************************
Receiving a IND_Type and IND_Quality values returns a D3D format for the destination (the texture we are going to create)
*************************
*/
FORMAT IND_SurfaceManager::GetDestFormat (IND_Surface *pNewSurface, IND_Type pType, IND_Quality pQuality)
{
	switch (pQuality)
	{
		case IND_GREY_8:
		{
			// Without alpha channel
			pNewSurface->mSurface.mAttributes.mType = IND_OPAQUE;
			return FORMAT_I8;
		}

		case IND_GREY_16:
		{
			// Without alpha channel
			pNewSurface->mSurface.mAttributes.mType = IND_OPAQUE;
			return FORMAT_I16;
		}
/*???
		case IND_16:
		{
			if (pType == IND_ALPHA)
				return FORMAT_A4R4G4B4;
			else
				return FORMAT_R5G6B5;
		}
*/
		case IND_32:
		{
			if (pType == IND_ALPHA)
				return FORMAT_RGBA8;
			else
				return FORMAT_RGBA8;//FORMAT_RGB8;
		}

		// By default, IND_32 quality
		default:
		{
			GetDebugAPI()->Header ("IND_Quality desconocida, usando IND_32 por defecto" , 4);

			if (pType == IND_ALPHA)
				return FORMAT_RGBA8;
			else
				return FORMAT_RGBA8;//FORMAT_RGB8;
		}
	}
}


/*
*************************
Push a vertex into the buffer
*************************
*/
void IND_SurfaceManager::PushVertex		(CUSTOMVERTEX2D *pVertices, 
										int pPosVert,
										int pVx, 
										int pVy, 
										int pVz,
										float pU,
										float pV)
{
/*
if(

	pPosVert>=n

) return;*/
	pVertices [pPosVert].mX = (float) pVx;
	pVertices [pPosVert].mY = (float) pVy;
	pVertices [pPosVert].mZ = (float) pVz;
	pVertices [pPosVert].mU = pU;
	pVertices [pPosVert].mV = pV;





	//pVertices [pPosVert].mColor = D3DCOLOR_COLORVALUE (1.0f, 1.0f, 1.0f, 0.1f);
}


/*
*************************
Push 4 vertices into the buffer creating a quad
*************************
*/
void IND_SurfaceManager::Push4Vertices (CUSTOMVERTEX2D *pVertices, 
										   int pPosVert,
										   int pPosX, 
										   int pPosY, 
										   int pPosZ, 
										   int pWidthBlock, 
										   int pHeightBlock,
										   float pU,
										   float pV)
{
	// Push the 4 vertex of the quad
	// The pushing order is important

	// Upper-right
	PushVertex (pVertices,//n,
				pPosVert,
				pPosX + pWidthBlock,
				pPosY - pHeightBlock,
				pPosZ,
				pU,
				pV);

	// Lower-right
	PushVertex (pVertices,//n,
				pPosVert+1,
				pPosX + pWidthBlock,
				pPosY,
				pPosZ,
				pU,
				0.0f);

	// Upper-left
	PushVertex (pVertices,//n,
				pPosVert+2,
				pPosX,
				pPosY - pHeightBlock,
				pPosZ,
				0.0f,
				pV);

	// Lower-left
	PushVertex (pVertices,//n,
				pPosVert+3,
				pPosX,
				pPosY,
				pPosZ,
				0.0f,
				0.0f);
}


/*
*************************
Creates a texture
*************************
*/
TextureID IND_SurfaceManager::CreateTexture	(unsigned char *pImage,
														int pBlockWidth,
														int pBlockHeight,
														int pSrcBpp,
														FORMAT pSrcFormat,
														FORMAT pDstFormat)
{
/*
	mHr = D3DXCreateTexture	(mRender->GetDevice(),
							pBlockWidth, 
							pBlockHeight, 
							1,
							0,
							pDstFormat,
							D3DPOOL_MANAGED,
							&mNewTexture);
*/

	TextureID id=-1;

	if(!pImage)
	{
		//LOG_FNLN;
		//LOG_PRINT("pImage=%x\n", pImage);
		//LOG_PRINT("Leaving %s\n",__FUNCTION__);
		return id;
	}	
	FORMAT formats=pSrcFormat;
	FORMAT formatd=pDstFormat;
	//LOG_FNLN;
	//LOG_PRINT("formats=%x\n", formats);
	//LOG_PRINT("formatd=%x\n", formatd);

	Image3 imgs;
	BYTE* ds=imgs.create(formatd,
				pBlockWidth,
				pBlockHeight,1,1);

	imgs.setPixels(pImage);//???
	if(imgs.getFormat()!=FORMAT_RGBA8)
		imgs.convert(FORMAT_RGBA8);
	//LOG_FNLN;

	id=IRenderer::GetRendererInstance()->addTexture(
		&imgs,
		false,
		IRenderer::GetRendererInstance()->Getlinear()
		,STATIC);
	return id;
}

/*
*************************
Inserts object into the manager
*************************
*/
void IND_SurfaceManager::AddToList (IND_Surface *pNewImage)
{
	mListSurfaces->push_back (pNewImage);
}


/*
*************************
Deletes object from the manager
*************************
*/
void IND_SurfaceManager::DelFromList (IND_Surface *pSu)
{
	mListSurfaces->remove (pSu);
	pSu = 0;
}


/*
*************************
Writes a message in the log that the object was not initialized
*************************
*/
void IND_SurfaceManager::WriteMessage ()
{
	GetDebugAPI()->Header ("This operation can not be done", 3);
	GetDebugAPI()->DataChar ("", 1);
	GetDebugAPI()->Header ("Invalid Id or SurfaceManager not correctly initialized", 2);
}


/*
*************************
Init manager vars
*************************
*/
void IND_SurfaceManager::InitVars ()
{
	mListSurfaces = new list <IND_Surface*>;
}


/*
*************************
Free manager memory
*************************
*/
void IND_SurfaceManager::FreeVars ()
{
	// Free cutter object
	Dispose (mCutter);

	// Deletes all the manager entities
	list <IND_Surface*>::iterator mSurfaceListIter;
	for (mSurfaceListIter  = mListSurfaces->begin();
		 mSurfaceListIter != mListSurfaces->end();
		 mSurfaceListIter++)
	{
		GetDebugAPI()->Header	("Freeing surface:", 3);
		//GetDebugAPI()->DataInt (int(&(*mSurfaceListIter)->mSurface), 1);
#ifndef OMITDELETE
		// Free textures
		for (int i = 0; i < (*mSurfaceListIter)->GetNumTextures(); i++)	
			IRenderer::GetRendererInstance()->deleteTexture((*mSurfaceListIter)->mSurface.mTexturesArray [i].mTexture);
#endif
		// Free vertex buffer
		DisposeArray ((*mSurfaceListIter)->mSurface.mVertexArray);
	}

	// Clear list
	mListSurfaces->clear ();

	// Free list
	Dispose (mListSurfaces);
}

