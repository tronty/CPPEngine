/*****************************************************************************************
 * File: IND_Surface.cpp
 * Desc: Surface object
 *****************************************************************************************/
/*
IndieLib 2d library Copyright (C) 2005 Javier Lpez Lpez (info@pixelartgames.com)

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

#include <CIndieLib.h>

#include <IND_Surface.h>

/* --------------------------------------------------------------------------------
//					 IND_Surface object implementation concept
// --------------------------------------------------------------------------------

// To be able to render image of any size (Direct3d /Opengl only supports textures power
// of two) IndieLib divides the image in blocks (textures). The size of that blocks depends
// on the size of the image and the maximum texture size allowed by the 3d card.
// So, from now, we will call the images "surfaces", that are in fact a
// group of blocks. (Note: that these "surfaces" are not the same thing as Direct3d surfaces)
//
// IndieLib implements this surfaces in the IND_Surface class. An example of an IND_Surface object will be:
// --------------------------------------------------------------------------------------------------------
// The numbers indicate the order in which the blocks will be rendered
// To avoiding showing the spare areas, that quads of the border are adjusted with proper uv coordinates
//		 ___________
//		|___|___|_ 9|
//		|_7_|_8_|_|_|
//		| 4 | 5 |6| |
//		|___|___|_|_|
//		| 1 | 2 |3| |
//		|___|___|_|_|
//
// On the screen, after adjusting the quad and uv coordiantes the surface will be rendered as follows,
// it will be rendered only the image, not the spare areas
//
//		 _________
//		|_7_|_8_|_|9
//		| 4 | 5 |6|
//		|___|___|_|
//		| 1 | 2 |3|
//		|___|___|_|
*/

// --------------------------------------------------------------------------------
//										Public Methods
// --------------------------------------------------------------------------------

/*
\b Parameters:

\arg \b pNumBlocksX, pNumBlocksY    Number of horizontal and vertical blocks

\b Operation:

This method sets a grid to the ::IND_Surface object. A grid is just a mesh which vertices
can be moved in order to deform the graphical object. You can set grids of different levels
of tesselation.

Using grids you can apply lot of different morphing effects to your sprites or animations
(waves, bubble animation, etc). It is also possible to change the position of all the vertices
so it would be possible to create for example a "snake" sprite that could simulate the crawling
when moving.

There is a restriction: the amount of horizontal and vertical blocks should be power of two.

Example:
- SetGrid (4, 4) => Correct!
- SetGrid (16, 2) => Correct!
- SetGrid (1, 8) => Correct!
- SetGrid (1, 3) => Incorrect!
*/
bool IND_Surface::SetGrid (int pNumBlocksX, int pNumBlocksY)
{
	// At least one block
	if (pNumBlocksX < 1 || pNumBlocksY < 1) return 0;

	// Only power of two values allowed
	IND_Math mMath;
	if (!mMath.IsPowerOfTwo (pNumBlocksX) || !mMath.IsPowerOfTwo (pNumBlocksY)) return 0;

	// Only 1-texture-IND_Surfaces allowed
	if (GetNumTextures() > 1) return 0;

	// Reset attributes
	mSurface.mAttributes.mIsHaveGrid		= 1;
	mSurface.mAttributes.mBlocksX			= pNumBlocksX;
	mSurface.mAttributes.mBlocksY			= pNumBlocksY;
	mSurface.mAttributes.mWidthBlock		= (mSurface.mAttributes.mWidth / mSurface.mAttributes.mBlocksX);
	mSurface.mAttributes.mHeightBlock		= (mSurface.mAttributes.mHeight / mSurface.mAttributes.mBlocksY);
	mSurface.mAttributes.mNumBlocks			= mSurface.mAttributes.mBlocksX * mSurface.mAttributes.mBlocksY;

	// Reset the vertex array
	DisposeArray (mSurface.mVertexArray);
	mSurface.mVertexArray = new CUSTOMVERTEX2D [mSurface.mAttributes.mBlocksX * mSurface.mAttributes.mBlocksY * 4];

	// Current position of the vertex
	int mPosX = 0;
	int mPosY = mSurface.mAttributes.mHeight;
	int mPosZ = 0;

	// Position in which we are storing a vertex
	int mPosVer = 0;

	// Position in which we are storing a texture
	int mCont = 0;

	// Create the new vertex array
	// We iterate the blocks starting from the lower row
	// We MUST draw the blocks in this order, because the image starts drawing from the lower-left corner
	for (int i = GetBlocksY(); i > 0; i--)
	{
		for (int j = 1; j < GetBlocksX() + 1; j++)
		{
			// ----- Block creation (using the position, uv coordiantes and texture) -----

			// We push into the buffer the 4 vertices of the block

			// Normal block
			if (i != 1 && j !=  GetBlocksX())
			{
				Push4Vertices  (&mSurface.mVertexArray[0],						// Pointer to the buffer
								mPosVer,									// Position in wich we are storing a vertex
								mPosX,										// x
								mPosY,										// y
								mPosZ,										// z
								mSurface.mAttributes.mWidthBlock,			// Block width
								mSurface.mAttributes.mHeightBlock,			// Block height
								mSurface.mAttributes.mWidth,				// U mapping coordinate
								mSurface.mAttributes.mHeight);				// V mapping coordinate
			}

			// The ones of the right column
			if (i != 1 && j ==  GetBlocksX())
			{
				Push4Vertices  (&mSurface.mVertexArray[0],						// Pointer to the buffer
								mPosVer,									// Position in wich we are storing a vertex
								mPosX,										// x
								mPosY,										// y
								mPosZ,										// z
								mSurface.mAttributes.mWidthBlock,			// Block width
								mSurface.mAttributes.mHeightBlock,			// Block height
								mSurface.mAttributes.mWidth,				// U mapping coordinate
								mSurface.mAttributes.mHeight);				// V mapping coordinate
			}

			// The ones of the upper row
			if (i == 1 && j != GetBlocksX())
			{
				Push4Vertices  (&mSurface.mVertexArray[0],						// Pointer to the buffer
								mPosVer,									// Position in wich we are storing a vertex
								mPosX,										// x
								mPosY,										// y
								mPosZ,										// z
								mSurface.mAttributes.mWidthBlock,			// Block width
								mSurface.mAttributes.mHeightBlock,			// Block height
								mSurface.mAttributes.mWidth,				// U mapping coordinate
								mSurface.mAttributes.mHeight);				// V mapping coordinate
			}

			// The one of the upper-right corner
			if (i == 1 && j ==  GetBlocksX())
			{
				Push4Vertices  (&mSurface.mVertexArray[0],						// Pointer to the buffer
								mPosVer,									// Position in wich we are storing a vertex
								mPosX,										// x
								mPosY,										// y
								mPosZ,										// z
								mSurface.mAttributes.mWidthBlock,			// Block width
								mSurface.mAttributes.mHeightBlock,			// Block height
								mSurface.mAttributes.mWidth,				// U mapping coordinate
								mSurface.mAttributes.mHeight);				// V mapping coordinate
			}

			// ----- Advance -----

			// Increase the vertex's position by 4
			mPosVer += 4;

			// ----- Column change -----

			// We point to the next block
			mPosX += mSurface.mAttributes.mWidthBlock;
		}

		// ----- Row change -----

		// We point to the next block
		mPosX = 0;
		mPosY -= mSurface.mAttributes.mHeightBlock;
	}

	return 1;
}


/*
\b Parameters:

\arg \b pVertexX, pVertexY		The vertex we want to move
\arg \b pX, pY					The new position for the vertex

\b Operation:

This method changes the position of one of the vertices of the grid. The vertices starts
from 0 to n, from left to right and from up to down. So: (0,0) is the upper-left vertex
of the grid and (n, n) is the lower-right vertex of the grid.

Remember that there is always one vertex more (horizontal or vertical) that number of blocks.
For example, a grid of 2x2 blocks would have 3x3 vertices.
*/
bool IND_Surface::SetVertexPos (int pVertexX, int pVertexY, int pX, int pY)
{
	if (!IsHaveGrid	()) return 0;

	// ----- Corners (we must move 1 vertex) -----

	if (pVertexX == 0 && pVertexY == 0)
	{
		mSurface.mVertexArray [((GetNumBlocks() - GetBlocksX()) * 4) + 2].mX = (float) pX;
		mSurface.mVertexArray [((GetNumBlocks() - GetBlocksX()) * 4) + 2].mY = (float) pY;
		return 1;
	}

	if (pVertexX == GetBlocksX() && pVertexY == GetBlocksY())
	{
		mSurface.mVertexArray [((GetBlocksX() - 1) * 4) + 1].mX = (float) pX;
		mSurface.mVertexArray [((GetBlocksX() - 1) * 4) + 1].mY = (float) pY;
		return 1;
	}

	if (pVertexX == GetBlocksX() && pVertexY == 0)
	{
		mSurface.mVertexArray [(GetNumBlocks() - 1) * 4].mX = (float) pX;
		mSurface.mVertexArray [(GetNumBlocks() - 1) * 4].mY = (float) pY;
		return 1;
	}

	if (pVertexX == 0 && pVertexY == GetBlocksY())
	{
		mSurface.mVertexArray [3].mX = (float) pX;
		mSurface.mVertexArray [3].mY = (float) pY;
		return 1;
	}

	// ----- Borders (we must move 2 vertices) -----

	if (pVertexX == 0)
	{
		mSurface.mVertexArray [(abs (GetBlocksY() - pVertexY - 1) * GetBlocksX() * 4) + 2].mX = (float) pX;
		mSurface.mVertexArray [(abs (GetBlocksY() - pVertexY - 1) * GetBlocksX() * 4) + 2].mY = (float) pY;
		mSurface.mVertexArray [(abs (GetBlocksY() - pVertexY) * GetBlocksX() * 4) + 3].mX = (float) pX;
		mSurface.mVertexArray [(abs (GetBlocksY() - pVertexY) * GetBlocksX() * 4) + 3].mY = (float) pY;
		return 1;
	}

	if (pVertexX == GetBlocksX())
	{
		mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY - 1) * GetBlocksX()) + GetBlocksX() - 1) * 4)].mX = (float) pX;
		mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY - 1) * GetBlocksX()) + GetBlocksX() - 1) * 4)].mY = (float) pY;
		mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY) * GetBlocksX()) + GetBlocksX() - 1) * 4) + 1].mX = (float) pX;
		mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY) * GetBlocksX()) + GetBlocksX() - 1) * 4) + 1].mY = (float) pY;
		return 1;
	}

	if (pVertexY == GetBlocksY())
	{
		mSurface.mVertexArray [((pVertexX - 1) * 4) + 1].mX = (float) pX;
		mSurface.mVertexArray [((pVertexX - 1)* 4) + 1].mY = (float) pY;
		mSurface.mVertexArray [((pVertexX) * 4) + 3].mX = (float) pX;
		mSurface.mVertexArray [((pVertexX) * 4) + 3].mY = (float) pY;
		return 1;
	}

	if (pVertexY == 0)
	{
		mSurface.mVertexArray [((GetNumBlocks() - GetBlocksX() + pVertexX - 1) * 4)].mX = (float) pX;
		mSurface.mVertexArray [((GetNumBlocks() - GetBlocksX() + pVertexX - 1) * 4)].mY = (float) pY;
		mSurface.mVertexArray [((GetNumBlocks() - GetBlocksX() + pVertexX) * 4) + 2].mX = (float) pX;
		mSurface.mVertexArray [((GetNumBlocks() - GetBlocksX() + pVertexX) * 4) + 2].mY = (float) pY;
		return 1;
	}

	// ----- Inside (we must move 4 vertices) -----

	if (pVertexX > 0 && pVertexX < GetBlocksX() && pVertexY > 0 && pVertexY < GetBlocksY())
	{
		mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY - 1) * GetBlocksX()) + pVertexX - 1) * 4)].mX = (float) pX;
		mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY - 1) * GetBlocksX()) + pVertexX - 1) * 4)].mY = (float) pY;
		mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY - 1) * GetBlocksX()) + pVertexX) * 4) + 2].mX = (float) pX;
		mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY - 1) * GetBlocksX()) + pVertexX) * 4) + 2].mY = (float) pY;
		mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY) * GetBlocksX()) + pVertexX - 1) * 4) + 1].mX = (float) pX;
		mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY) * GetBlocksX()) + pVertexX - 1) * 4) + 1].mY = (float) pY;
		mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY) * GetBlocksX()) + pVertexX) * 4) + 3].mX = (float) pX;
		mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY) * GetBlocksX()) + pVertexX) * 4) + 3].mY = (float) pY;
		return 1;
	}

	return 0;
}

/*
\b Parameters:

\arg \b pVertexX, pVertexY		One vertex of the grid

\b Operation:

This method returns the horizontal position of the vertex passed as parameter.

Remember that there is always one vertex more (horizontal or vertical) than number of blocks.
For example, a grid of 2x2 blocks would have 3x3 vertices.
*/
int IND_Surface::GetVertexPosX	(int pVertexX, int pVertexY)
{
	if (!IsHaveGrid	()) return 0;

	// ----- Corners -----

	if (pVertexX == 0 && pVertexY == 0)
		return (int) mSurface.mVertexArray [((GetNumBlocks() - GetBlocksX()) * 4) + 2].mX;

	if (pVertexX == GetBlocksX() && pVertexY == GetBlocksY())
		return (int) mSurface.mVertexArray [((GetBlocksX() - 1) * 4) + 1].mX;

	if (pVertexX == GetBlocksX() && pVertexY == 0)
		return (int) mSurface.mVertexArray [(GetNumBlocks() - 1) * 4].mX;

	if (pVertexX == 0 && pVertexY == GetBlocksY())
		return (int) mSurface.mVertexArray [3].mX;

	// ----- Borders -----

	if (pVertexX == 0)
		return (int) mSurface.mVertexArray [(abs (GetBlocksY() - pVertexY - 1) * GetBlocksX() * 4) + 2].mX;

	if (pVertexX == GetBlocksX())
		return (int) mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY) * GetBlocksX()) + GetBlocksX() - 1) * 4) + 1].mX;

	if (pVertexY == GetBlocksY())
		return (int) mSurface.mVertexArray [((pVertexX) * 4) + 3].mX;

	if (pVertexY == 0)
		return (int) mSurface.mVertexArray [((GetNumBlocks() - GetBlocksX() + pVertexX - 1) * 4)].mX;

	// ----- Inside (we must move 4 vertices) -----

	if (pVertexX > 0 && pVertexX < GetBlocksX() && pVertexY > 0 && pVertexY < GetBlocksY())
		return (int) mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY - 1) * GetBlocksX()) + pVertexX - 1) * 4)].mX;

	return 0;
}


/*
\b Parameters:

\arg \b pVertexX, pVertexY		One vertex of the grid

\b Operation:

This method returns the vertical position of the vertex passed as parameter.

Remember that there is always one vertex more (horizontal or vertical) than number of blocks.
For example, a grid of 2x2 blocks would have 3x3 vertices.
*/
int IND_Surface::GetVertexPosY	(int pVertexX, int pVertexY)
{
	if (!IsHaveGrid	()) return 0;

	// ----- Corners -----

	if (pVertexX == 0 && pVertexY == 0)
		return (int) mSurface.mVertexArray [((GetNumBlocks() - GetBlocksX()) * 4) + 2].mY;

	if (pVertexX == GetBlocksX() && pVertexY == GetBlocksY())
		return (int) mSurface.mVertexArray [((GetBlocksX() - 1) * 4) + 1].mY;

	if (pVertexX == GetBlocksX() && pVertexY == 0)
		return (int) mSurface.mVertexArray [(GetNumBlocks() - 1) * 4].mY;

	if (pVertexX == 0 && pVertexY == GetBlocksY())
		return (int) mSurface.mVertexArray [3].mY;

	// ----- Borders -----

	if (pVertexX == 0)
		return (int) mSurface.mVertexArray [(abs (GetBlocksY() - pVertexY - 1) * GetBlocksX() * 4) + 2].mY;

	if (pVertexX == GetBlocksX())
		return (int) mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY) * GetBlocksX()) + GetBlocksX() - 1) * 4) + 1].mY;

	if (pVertexY == GetBlocksY())
		return (int) mSurface.mVertexArray [((pVertexX) * 4) + 3].mY;

	if (pVertexY == 0)
		return (int) mSurface.mVertexArray [((GetNumBlocks() - GetBlocksX() + pVertexX - 1) * 4)].mY;

	// ----- Inside (we must move 4 vertices) -----

	if (pVertexX > 0 && pVertexX < GetBlocksX() && pVertexY > 0 && pVertexY < GetBlocksY())
		return (int) mSurface.mVertexArray [(((abs (GetBlocksY() - pVertexY - 1) * GetBlocksX()) + pVertexX - 1) * 4)].mY;

	return 0;
}




// --------------------------------------------------------------------------------
//										Private methods
// --------------------------------------------------------------------------------

/*
*************************
Push a vertex into the buffer
*************************
*/
void IND_Surface::PushVertex		(CUSTOMVERTEX2D *pVertices,
									int pPosVert,

									int pVx,

									int pVy,
									int pVz,
									float pU,
									float pV)
{
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
void IND_Surface::Push4Vertices (CUSTOMVERTEX2D *pVertices,
								int pPosVert,
								int pX,
								int pY,
								int pZ,
								int pWidthBlock,
								int pHeightBlock,
								int pWidth,
								int pHeight)
{
	// Push the 4 vertex of the quad
	// The pushing order is important
#if 1
	// Upper-right
	PushVertex (pVertices,
				pPosVert,
				pX + pWidthBlock,
				pY - pHeightBlock,
				pZ,
				(float) (pX + pWidthBlock) / pWidth,
				1.0f - ( (float) (pY - pHeightBlock) / pHeight) );

	// Lower-right
	PushVertex (pVertices,
				pPosVert+1,
				pX + pWidthBlock,
				pY,
				pZ,
				(float) (pX + pWidthBlock) / pWidth,
				1.0f - ( (float) pY / pHeight) );

	// Upper-left
	PushVertex (pVertices,
				pPosVert+2,
				pX,
				pY - pHeightBlock,
				pZ,
				(float) pX / pWidth,
				1.0f - ( (float) (pY - pHeightBlock) / pHeight) );

	// Lower-left
	PushVertex (pVertices,
				pPosVert+3,
				pX,
				pY,
				pZ,
				(float) pX / pWidth,
				1.0f - ( (float) pY / pHeight) );
#else
	// Lower-left
	PushVertex (pVertices,
				pPosVert+3,
				pX,
				pY,
				pZ,
				(float) pX / pWidth,
				1.0f - ( (float) pY / pHeight) );
	// Lower-right
	PushVertex (pVertices,
				pPosVert+1,
				pX + pWidthBlock,
				pY,
				pZ,
				(float) (pX + pWidthBlock) / pWidth,
				1.0f - ( (float) pY / pHeight) );

	// Upper-left
	PushVertex (pVertices,
				pPosVert+2,
				pX,
				pY - pHeightBlock,
				pZ,
				(float) pX / pWidth,
				1.0f - ( (float) (pY - pHeightBlock) / pHeight) );

	// Upper-right
	PushVertex (pVertices,
				pPosVert,
				pX + pWidthBlock,
				pY - pHeightBlock,
				pZ,
				(float) (pX + pWidthBlock) / pWidth,
				1.0f - ( (float) (pY - pHeightBlock) / pHeight) );

#endif
}


/*
*************************
Format to string
*************************
*/
const char *IND_Surface::TypeToChar (IND_Type pType)
{
	switch (pType)
	{
		case IND_OPAQUE:	return "IND_OPAQUE";
		case IND_ALPHA:		return "IND_ALPHA";

		default:			return "TYPE_NOT_IDENTIFIED";
	}
}


/*
*************************
Quality to string
*************************
*/
const char *IND_Surface::QualityToChar (IND_Quality pQuality)
{
	switch (pQuality)
	{
		case IND_GREY_8:	return "IND_GREY_8";
		case IND_GREY_16:	return "IND_GREY_16";
//???		case IND_16:		return "IND_16";
		case IND_32:		return "IND_32";

		default:			return "QUALITY_NOT_IDENTIFIED";
	}
}

