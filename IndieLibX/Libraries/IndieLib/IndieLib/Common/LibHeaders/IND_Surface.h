/*****************************************************************************************
/* File: IND_Surface.h
/* Desc: Surface object
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

#ifndef _IND_SURFACE_
#define _IND_SURFACE_

// ----- Includes -----
/*
#include <d3d9.h> 
#include <d3dx9.h>
*/
#include "IND_Math.h"


// --------------------------------------------------------------------------------
//									 IND_Surface
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Surface IND_Surface
\ingroup Objects
IND_Surface class managed by IND_SurfaceManager for displaying surfaces and animations into the screen. Click in IND_Surface to see all the methods of this class.
*/
/*@{*/

/*!
\b IND_Surface is an surface object from the class ::IND_SurfaceManager. Read the explanation in ::IND_SurfaceManager for more details.
*/
class DLL_EXP IND_Surface
{
public:

	// ----- Public Methods ------

	bool		SetGrid				(int pNumBlocksX, int pNumBlocksY);
	bool		SetVertexPos		(int pVertexX, int pVertexY, int pX, int pY);
	bool		SetVertexPos		(float pVertexX, float pVertexY, float pX, float pY){return SetVertexPos(int(pVertexX), int(pVertexY), int(pX), int(pY));}
	int			GetVertexPosX		(int pVertexX, int pVertexY); 
	int			GetVertexPosY		(int pVertexX, int pVertexY); 

	// ----- Public gets ------

	//! This function returns the type of the surface. See ::IND_Type.
	IND_Type	GetTypeInt		()		{ return mSurface.mAttributes.mType;						}
	//! This function returns the quality of the surface. See ::IND_Quality.
	IND_Quality	GetQualityInt	()		{ return mSurface.mAttributes.mQuality;						}
	//! This function returns the number of textures the surface is using.
	int			GetNumTextures	()		{ return mSurface.mAttributes.mNumTextures;					}
	//! This function returns the width of the surface.
	int			GetWidth		()		{ return mSurface.mAttributes.mWidth;						}
	//! This function returns the height of the surface.
	int			GetHeight		()		{ return mSurface.mAttributes.mHeight;						}
	//! This function returns the number of blocks (width) in which the surface is divided.
	int			GetBlocksX		()		{ return mSurface.mAttributes.mBlocksX;						}
	//! This function returns the number of blocks (height) in which the surface is divided.
	int			GetBlocksY		()		{ return mSurface.mAttributes.mBlocksY;						}		
	//! This function returns the number of blocks in which the surface is divided.
	int			GetNumBlocks	()		{ return mSurface.mAttributes.mNumBlocks;					}
	//! This function returns the width of each block in which the surface is divided.
	int			GetWidthBlock	()		{ return mSurface.mAttributes.mWidthBlock;					}
	//! This function returns the height of each block in which the surface is divided.
	int			GetHeightBlock	()		{ return mSurface.mAttributes.mHeightBlock;					}
	//! This function returns 1 if the surface is loaded.
	bool		IsHaveSurface	()		{ return mSurface.mAttributes.mIsHaveSurface;				}
	//! This function returns 1 if the surface has a grid assigned.
	bool		IsHaveGrid		()		{ return mSurface.mAttributes.mIsHaveGrid;					}	
	//! This function returns the type of surface in a string of chars. 
	const char		*GetTypeChar	()		{ return TypeToChar (mSurface.mAttributes.mType);			}
	//! This function returns the quality of the surface in a string of chars. See ::IND_Quality.
	const char		*GetQualityChar	()		{ return QualityToChar (mSurface.mAttributes.mQuality);		}

	int			GetSpareX		()		{ return mSurface.mAttributes.mSpareX;						}
	int			GetSpareY		()		{ return mSurface.mAttributes.mSpareY;						}

private:

	// ----- Structures ------

	// TextureOTL
	struct structTextures
	{	
		TextureID mTexture;			// TextureOTL
	};
	typedef struct structTextures TEXTURES;

	// ----- Vertex array -----


	// Attributes
	struct structAttributes
	{	
		structAttributes()
		{
			mType = IND_OPAQUE;
			mQuality=IND_32;
			mNumTextures=0;
			mWidth=0;
			mHeight=0;
			mBlocksX=0;
			mBlocksY=0;
			mSpareX=0;
			mSpareY=0;
			mNumBlocks=0;
			mWidthBlock=0;
			mHeightBlock=0;
			mIsHaveSurface=false;
			mIsHaveGrid=false;
		}
		IND_Type	mType;					// Surface type
		IND_Quality 	mQuality;				// Color quality
		int		mNumTextures;				// Number of textures
		int		mWidth;					// Image width
		int		mHeight;				// Image height
		int		mBlocksX;				// Number of blocks width
		int		mBlocksY;				// Number of blocks height
		int		mSpareX;				// Right spare area
		int		mSpareY;				// Upper spare area
		int		mNumBlocks;				// Total number of blocks
		int		mWidthBlock;				// Block width
		int		mHeightBlock;				// Block height
		bool		mIsHaveSurface;				// Surface loaded or not

		bool		mIsHaveGrid;				
	};
	typedef struct structAttributes ATTRIBUTES;

	// TYPE
	struct structSurface
	{
		structSurface()
		{
			mVertexArray=0;
			mTexturesArray=0;
			mAttributes=structAttributes();
		}
		CUSTOMVERTEX2D *mVertexArray;			// Vertex array (store the blocks (quads) of the IND_Surface
		TEXTURES *mTexturesArray;				// TextureOTL array (one texture per block)
		ATTRIBUTES mAttributes;					// Attributes
	};
	typedef struct structSurface SURFACE;

	SURFACE mSurface;

	// ----- Private sets ------

	void SetTypeInt							(int pType)						{ mSurface.mAttributes.mType = pType;				}
	void SetQualityInt						(int pQuality)					{ mSurface.mAttributes.mQuality = pQuality;			}
	void SetWidth							(int pWidth)					{ mSurface.mAttributes.mWidth = pWidth;				}
	void SetHeight							(int pHeight)					{ mSurface.mAttributes.mHeight = pHeight;			}
	void SetBlocksX							(int pBlocksX)					{ mSurface.mAttributes.mBlocksX = pBlocksX;			}
	void SetBlocksY							(int pBlocksY)					{ mSurface.mAttributes.mBlocksY = pBlocksY;			}		
	void SetNumBlocks						(int pNumBlocks)				{ mSurface.mAttributes.mNumBlocks = pNumBlocks;		}
	void SetWidthBlock						(int pWidthBlock)				{ mSurface.mAttributes.mWidthBlock = pWidthBlock;	}
	void SetHeightBlock						(int pHeightBlock)				{ mSurface.mAttributes.mHeightBlock = pHeightBlock;	}

	void SetVertexArray						(CUSTOMVERTEX2D *pVertexArray)	{ mSurface.mVertexArray = pVertexArray;						}
	void SetTextArray						(TEXTURES *pTextArray)			{ mSurface.mTexturesArray = pTextArray;				}
			
	// ----- Private gets ------	

	CUSTOMVERTEX2D			*GetVertexArray	()								{ return mSurface.mVertexArray;					}
	TEXTURES				*GetTextArray	()								{ return mSurface.mTexturesArray;					}
	
	// ----- Private methods ------	
	
	const char					*TypeToChar			(IND_Type pType);
	const char					*QualityToChar		(IND_Quality pQuality);
	void					PushVertex			(CUSTOMVERTEX2D *pVertices, 
												int pPosVert,
												int pX, 
												int pY, 
												int pZ,
												float pU,
												float pV);

	void					Push4Vertices		(CUSTOMVERTEX2D *pVertices, 
												int pPosVert,
												int pPosX, 
												int pPosY, 
												int pPosZ, 
												int pWidthBlock, 
												int pHeightBlock,
												int pWidth,
												int pHeight);

	// ----- Friends -----

	friend class IND_SurfaceManager;
	friend class IND_Render;
};
/*@}*/

#endif // _IND_SURFACE_
