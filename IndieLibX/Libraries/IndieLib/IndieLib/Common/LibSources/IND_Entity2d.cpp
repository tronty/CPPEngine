/*****************************************************************************************
 * File: IND_Entity2d.cpp
 * Desc: Entity 2d object
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

#include <CIndieLib.h>

#include <IND_Entity2d.h>

IND_Entity2d::IND_Entity2d()
{
	InitAttrib ();
}
// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

IND_Entity2d::~IND_Entity2d()
{
	//STX_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	//STX_PRINT("mListBoundingCollision=%x\n",mListBoundingCollision);
	Dispose (mListBoundingCollision);// ??? Heap corruption revelation
	//STX_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
#if 0
../Common/LibSources/IND_Entity2d.cpp:~IND_Entity2d:51
*** glibc detected *** ./IndieLib-TankEnemiesParallax_2008_d.bin: munmap_chunk(): invalid pointer: 0x09665c80 ***
======= Backtrace: =========
/lib/tls/i686/cmov/libc.so.6(+0x6b161)[0x2fb161]
/lib/tls/i686/cmov/libc.so.6(+0x6c3de)[0x2fc3de]
/usr/lib/libIL.so.1(+0x13ba1)[0xd7fba1]
/usr/lib/libIL.so.1(ifree+0x22)[0xd7fb02]
/usr/lib/libIL.so.1(ilCloseImage+0x2d)[0xd80b2d]
/usr/lib/libIL.so.1(ilCloseImage+0x6f)[0xd80b6f]
/usr/lib/libIL.so.1(ilCloseImage+0x6f)[0xd80b6f]
/usr/lib/libIL.so.1(ilCloseImage+0x6f)[0xd80b6f]
/usr/lib/libIL.so.1(ilShutDown+0x67)[0xd80f07]
/lib/tls/i686/cmov/libc.so.6(+0x2f1bf)[0x2bf1bf]
/lib/tls/i686/cmov/libc.so.6(+0x2f22f)[0x2bf22f]
/lib/tls/i686/cmov/libc.so.6(__libc_start_main+0xee)[0x2a6bde]
./IndieLib-TankEnemiesParallax_2008_d.bin[0x8157961]
======= Memory map: ========
#endif
}



// --------------------------------------------------------------------------------
//									 Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pNewSurface			Pointer to an  IND_Surface object

\b Operation:

This function inserts a surface in the entity. If that entity has another graphical object allocated it will be replaced (but not freed) by de present surface. In the next call to IND_Entity2dManager::RenderEntities2d() the surface will be drawn.

In addition, all the attributes of the entity will be restarted.
*/
void IND_Entity2d::SetSurface (IND_Surface *pNewSurface)
{
	InitAttrib ();

	mSu = pNewSurface;
	mType = mSu->GetTypeInt();
}


/*!
\b Parameters:

\arg \b pNewAnimation        Pointer to a  IND_Animation object

\b Operation:

This function inserts an animation in the entity. If that entity has another graphical object allocated it will be replaced (but not freed)
by the present animation. In the next call to IND_Entity2dManager::RenderEntities2d() this animation will be drawn.

In addition, all the attributes of the entity will be restarted.
*/
void IND_Entity2d::SetAnimation	(IND_Animation *pNewAnimation)
{

	InitAttrib ();

	mAn = pNewAnimation;

	if (mAn->GetSurface(0))
{

		mType = mAn->GetSurface(0)->GetTypeInt();

}

}


/*!
\b Parameters:

\arg \b pPri2d               Primitive type. See ::IND_Primitive2d.

\b Operation:

This function inserts a primitive in the entity. If that entity has another graphical object allocated,
it will be replaced (but not freed) by the present primitive. In the next call to IND_Entity2dManager::RenderEntities2d()
this primitive will be drawn.

In addition, all the parameters of the entity will be restarted.
*/
void IND_Entity2d::SetPrimitive2d	(IND_Primitive2d pPri2d)
{
	InitAttrib ();

	mPri2d = pPri2d;
}


/*!
\b Parameters:

\arg \b pFont                      Pointer to a IND_Font object.

\b Operation:

This function inserts a font into the entity. If that entity has another graphical object allocated, it will be replaced (but not freed)
by the present font. In the next call to IND_Entity2dManager::RenderEntities2d() the font will be drawn.

In addition, all the parameters of the entity will be restarted.

Important: Fonts can't be sized and can't be rotated.
*/
void IND_Entity2d::SetFont	(IND_Font *pFont)
{
	InitAttrib ();

	mType = IND_ALPHA;
	mFont = pFont;
}


/*!
\b Parameters:

\arg \b pShow             True = show the entity / False = hide entity

\b Operation:

This function showes / hides the entity. It dosen't delete it.

Default: 0.
*/
void IND_Entity2d::SetShow (bool pShow)
{
	mShow = pShow;
}


/*!
\b Parameters:

\arg \b pS             Number of sequence to draw

\b Operation:

This function sets the number of sequence that will be drawn.

Default: 0.
*/
void IND_Entity2d::SetSequence (int pSequence)
{
	mSequence = pSequence;
}


/*!
\b Parameters:

\arg \b pNumReplays                    Number of repetitions of the animation

\b Operation:

This function sets the number of times that the animation will be drawn on the screen.

When the parameter takes values less or equals 0 the animation repeats forever.
When the parameter takes values greater or equals 1, this number shows the number of times that the animation will be repeated.

Default: -1 (repeat forever),
*/
void IND_Entity2d::SetNumReplays (int pNumReplays)
{
	if (pNumReplays <= 0)
	{
		mNumReplays = -1;
		return;
	}

	if (mFirstTime && pNumReplays != -1)
	{
		pNumReplays--;
		mFirstTime = 0;
	}

	mNumReplays = pNumReplays;
}


/*!
\b Parameters:

\arg <b>pX1, pY1, pX2, pY2</b>            Beginning and end points of the line

\b Operation:

This function sets the coordinates of the ::IND_LINE primitive that will be drawn on screen.

Default: (0, 0, 0, 0).
*/
void IND_Entity2d::SetLine (int pX1, int pY1, int pX2, int pY2)
{
	mX1 = pX1;
	mY1 = pY1;
	mX2 = pX2;
	mY2 = pY2;
}


/*!
\b Parameters:

\arg <b>pX1, pY1, pX2, pY2</b>            Coordinates of the top left vertex and lower right vertex of the rectangle.

\b Operation:

This function sets the coordinates of the ::IND_RECTANGLE primitive that will be drawn on screen.

Default: (0, 0, 0, 0).
*/
void IND_Entity2d::SetRectangle	(int pX1, int pY1, int pX2, int pY2)
{
	mX1 = pX1;
	mY1 = pY1;
	mX2 = pX2;
	mY2 = pY2;
}


/*!
\b Parameters:

\arg \b pRadius                   Length of the radius of the primitive ::IND_REGULAR_POLY

\b Operation:

This function sets the length of the radius of the primitive ::IND_REGULAR_POLY that will be drawn on screen.

Default: 0.
*/
void IND_Entity2d::SetRadius (int pRadius)
{
	mRadius = pRadius;
}


/*!
\b Parameters:

\arg \b pNumSides              Number of sides of the regular polygon ::IND_REGULAR_POLY

\b Operation:

This function sets the number of sides of the regular polygon ::IND_REGULAR_POLY.

Default: 0.
*/
void IND_Entity2d::SetNumSides (int pNumSides)
{
	mNumSides = pNumSides;
}


/*!
\b Parameters:

\arg \b pPolyAngle              Angle of the regular polygon ::IND_REGULAR_POLY

\b Operation:

This function sets the angle of the regular polygon ::IND_REGULAR_POLY. With incremental values it is possible to rotate the polygon.

Default: 0.
*/
void IND_Entity2d::SetPolyAngle	(float pPolyAngle)
{
	mPolyAngle = pPolyAngle;
}


/*!
\b Parameters:

\arg \b pPolyPoints              Pointer to an array of points ::IND_Point. Example: ::IND_Point mPoly3 [] = {{60, 10}, {20, 15}, {50, 90}, {170, 190}} =>  Indicates three points (each one with its x and y coordinates).

\b Operation:

This function sets the array of points that will be used to draw the primitive ::IND_POLY2D.
Every point consists of two coordinates (x, y). This function must be used together with SetNumLines()
that is used to indicate the number of edges to draw between the specified points.

Default: 0.
*/
void IND_Entity2d::SetPolyPoints (IND_Point *pPolyPoints)
{
	mPolyPoints = pPolyPoints;
}


/*!
\b Parameters:

\arg \b pNumLines               Number of edges to draw

\b Operation:

This function sets the number of edges to draw by the primitive ::IND_POLY2D. This function must used together with SetPolyPoints() that is used to indicate the array of points.

Default: 0.
*/
void IND_Entity2d::SetNumLines	(int pNumLines)
{
	mNumLines = pNumLines;
}


/*!
\b Parameters:

\arg \b pAlign                       Text alignment. See ::IND_Align.



\b Operation:

This function sets the alignment of the text.

Default: ::IND_RIGHT.
*/
void IND_Entity2d::SetAlign	(IND_Align pAlign)
{
	mAlign = pAlign;
}


/*!
\b Parameters:

\arg \b pCharSpacing                     Width of the additional space between letters

\b Operation:

This function sets an additional space between characters when using a IND_Font in a 2d entity.

Debault: 0.
*/
void IND_Entity2d::SetCharSpacing (int pCharSpacing)
{
	mCharSpacing = pCharSpacing;
}


/*!
\b Parameters:

\arg \b pLineSpacing                     Height of the line spacing between lines

\b Operation:

This function sets the line spacing between lines when using a IND_Font in a 2d entity.

Debault: 0.
*/
void IND_Entity2d::SetLineSpacing (int pLineSpacing)
{
	mLineSpacing = pLineSpacing;
}


/*!
\b Parameters:

\arg \b pText                         Text to draw in the screen.

\b Operation:

This function sets the text that is going to be shown on screen.

Default: "" (Without text).
*/
void IND_Entity2d::SetText (const char *pText)
{
	mText	= (char*)pText;
}


/*!
\b Parameters:

\arg <b> pX, pY</b>                      Translation in the x and y axis. The position (0,0) corresponds to the top left corner of the ViewPort
\arg \b pZ                               Depth of the entity.

\b Operation:

This function sets the position where the entity will be drawn. The parameter Z controls the depth. This way it is possible to choose that certain entities are drawn over others. An entity with Z = 1 will be drawn over other one with Z = 0.

Default: (0, 0, 0).
*/
void IND_Entity2d::SetPosition (float pX, float pY, int pZ)
{
	// If updated
	if (pX != mX || pY != mY)
	{
		mX = pX;
		mY = pY;
		mUpdateTransFlag = 1;
	}

	mZ = pZ;
}


/*!
\b Parameters:

\arg \b pAnX						 Angle of rotation in the x axis expressed in degrees.
\arg \b pAnY                         Angle of rotation in the y axis expressed in degrees.
\arg \b pAnZ                         Angle of rotation in the z axis expressed in degrees.

\b Operation:

This function sets the rotation of the entity in each of its axes.

Default: (0, 0, 0).
*/
void IND_Entity2d::SetAngleXYZ	(float pAnX, float pAnY, float pAnZ)
{
	// If updated
	if (pAnX != mAngleX || pAnY != mAngleY || pAnZ != mAngleZ)
	{
		mAngleX = pAnX;
		mAngleY = pAnY;
		mAngleZ = pAnZ;
		mUpdateTransFlag = 1;
	}
}


/*!
\b Parameters:

\arg <b> pSx, pSy</b>                  Level of scaling in the x and y axis. (1,1) to keep the normal size

\b Operation:

This function sets the scaling of the entity in the x and y axis. With this method it is possible to get bigger or
smaller entities.

Default: (1.0f, 1.0f).
*/
void IND_Entity2d::SetScale	(float pSx, float pSy)
{
	// If updated
	if (pSx != mScaleX || pSy != mScaleY)
	{
		mScaleX = pSx;
		mScaleY = pSy;
		mUpdateTransFlag = 1;
	}
}


/*!
\b Parameters:

\arg \b pCull            Discard of posterior faces activated / deactivated

\b Operation:

This function activates or deactivates the backface culling. (true / false) = (activated / deactivated).

This feature can be deactivated when we are going to rotate the graphical object in the x or y coordinates.
This way it is possible to draw for example the leaf of a tree that falls down rotating and shows both faces.
In cases that you don't want to draw back faces deactivate this for faster render times.

Activated by default (true).
*/
void IND_Entity2d::SetBackCull (bool pCull)
{
	// If updated
	if (pCull != mCull)
	{
		mCull = pCull;
		mUpdateTransFlag = 1;
	}
}


/*!
\b Parameters:

\arg \b pMx								Mirror (flip) in X. (1 / 0) = (activated / deactivated).

\b Operation:

This function activates or deactivates the horizontal flip.

Deactivated by default (0).
*/
void IND_Entity2d::SetMirrorX (bool pMx)
{
	// If updated
	if (pMx != mMirrorX)
	{
		mMirrorX = pMx;
		mUpdateTransFlag = 1;
	}
}


/*!
\b Parameters:

\arg \b  pMy							Mirror (flip) in Y. (1 / 0) = (activated / deactivated).

\b Operation:

This function activates or deactivates the vertical flip.

Deactivated by default (0).
*/
void IND_Entity2d::SetMirrorY (bool pMy)
{
	// If updated
	if (pMy != mMirrorY)
	{
		mMirrorY = pMy;
		mUpdateTransFlag = 1;
	}
}


/*!
\b Parameters:

\arg \b    pF                   Type of filter ::IND_Filter. There are two types of filters that
								can be applied to the graphical object when it suffers a rotation
								or scaling:
								- ::IND_FILTER_POINT (Nearest point filter). Less quality, but
								bigger performance. It is possible to use this one without
								quality loss in graphical objects that don't rotate or are
								affected by scaling.
								- ::IND_FILTER_LINEAR (Bilinear filter). More quality, but less
								performance. Used in graphical objects that be rotated or
								affected by scaling.

\b Operation:

This function sets the filter that will affect the graphical object when drawing.

Default: ::IND_FILTER_LINEAR
*/
void IND_Entity2d::SetFilter (IND_Filter pF)
{
	// If updated
	if (pF != mFilter)
	{
		mFilter = pF;
		mUpdateTransFlag = 1;
	}
}


/*!
\b Parameters:

\arg 	<b>pX, pY</b>                       Values that indicates the position of the hotspot. The top left corner of the entity corresponds to (0, 0).

\b Operation:

This function sets the hotspot of the graphical object (only works if the entity has an ::IND_Surface or
::IND_Animation assigned). When we talk about hotspot we are refering to
an imaginary point from which the transformations are applied (trasnlation, rotation and scaling).
So, we can choose that certain surface rotates on his center, on one of his vertex, or on any another
intermediate position.

To establish the hotspot we use pX and pY in a range from 0 to 1 (a point inside the surface area)
or higher (a point outside the surface area). Refering respectively to the horizontal and vertical
position of the surface. Thus (0.5f, 0.5f)  would be the center of the image, (1.0f, 0.0f) would be
the top right corner, etc.

You can also set a hotspot out of the image, for example using (2.0f, 4.0f) values. This way you can make the entity
rotate around a point.

Default value (0, 0) = Upper left corner.
*/
bool IND_Entity2d::SetHotSpot	(float pX, float pY)
{
	//if (pX < 0 || pX > 1) pX = 0;
	//if (pY < 0 || pY > 1) pY = 0;

	// If updated
	if (pX != mHotSpotX || pY != mHotSpotY)
	{
		mUpdateTransFlag = 1;

		if (mSu)
		{
			mHotSpotX = pX;
			mHotSpotY = pY;
			mAxisCalX = (int) (pX * mSu->GetWidth	() * -1);
			mAxisCalY = (int) (pY * mSu->GetHeight	() * -1);
		}
		else
		if (mAn)
		{
			mHotSpotX = pX;
			mHotSpotY = pY;
			mAxisCalX = (int) (pX * mAn->GetHighWidth	(mSequence) * -1);
			mAxisCalY = (int) (pY * mAn->GetHighHeight	(mSequence) * -1);
		}
		else
			return 0;
	}

	return 1;
}


/*!
\b Parameters:

\arg <b>pOffX, pOffY</b>		Position of the upper left corner of the region.
\arg \b pRegionWidth			Width of the region.
\arg \b pRegionHeight			Height of the region.

\b Operation:

This function has two use uses:

\b 1) First, it can be used to indicate the rectangular region which is going to be drawn from
the surface or animation assigned to the entity. So, instead of drawing all the surface, it can
be chosen which rectangular area is going to be rendered in the screen. This method doesn't affect
primitives or fonts. If the requested region stays partly or totally out of the surface,
the region will not be applied.

\b 2) Second, if IND_Entity2d::ToggleWrap() has been used on the entity, the image will be
repeated (tiling) in the region specified.

The 1st method is useful in the example of the classic loading bar, which shows itself
from left side to right progressively changing the width of the region.

The 2nd method is useful for multiple things. For example a background made by repeating a tile.

Special remark: this function only works with ::IND_Surface objects that only have ONE texture
assigned (you can check this using::IND_Surface::GetNumTextures() method). So, it will work only
with images that are power of two and lower than the maximum texture size allowed by your card
(you can check this parameter using ::IND_Render::GetMaxTextureSize()). The method will return 0
otherwise.

Default: (0, 0, 0, 0) (Indicates that there is no region, the whole surface will be drawn)
*/
bool IND_Entity2d::SetRegion (int pOffX, int pOffY, int pRegionWidth, int pRegionHeight)
{
	if (!mSu && !mAn) return 0;

	if (pRegionWidth	<= 0) return 0;
	if (pRegionHeight	<= 0) return 0;

	mOffX			= pOffX;
	mOffY			= pOffY;
	mRegionWidth	= pRegionWidth;
	mRegionHeight	= pRegionHeight;

	return 1;
}


/*!
\b Parameters:

\arg \b	pWrap			(Activates / Deactivates) = (true, false) the repetition of the image in
							the x, y axis.

\b Operation:

This function causes the repetition of the image in the x and y axis in the specified region
by IND_Entity2d::SetRegion().
*/
bool IND_Entity2d::ToggleWrap	(bool pWrap)
{
	if (!mSu && !mAn) return 0;

	mWrap = pWrap;

	return 1;
}


/*!
\b Parameters:

\arg \b	pUDisplacement	Horizontal displacement of the image (U coordinate)
\arg \b	pVDisplacement	Vertical displacement of the image (V coordinate)

\b Operation:

This function sets the horizontal and vertical displacement of the an image that is being tiled
to the screen using the IND_Entity2d::SetRegion() and IND_Entity2d::ToggleWrap() methods.

Using this method you can, for example, create and effect of an animated tiled background.
*/
void IND_Entity2d::SetWrapDisplacement (float pUDisplace, float pVDisplace)
{
	mUDisplace = pUDisplace;
	mVDisplace = pVDisplace;
}


/*!
\b Parameters:

\arg <b> pR, pG, pB</b>       Bytes R, G, B

\b Operation:

This function sets the bytes pR, pG, pB for "tinting" the image to a certain color. For example,
if we use RGB = (255, 0, 0), the image will be tinted red. Note: It is not the same as
fade. The tint produces a colored transparent effect (similar to looking through celophane),
whereas the fade completely changes the image to the color selected.

It is not possible to use IND_Entity2d::SetTint() and IND_Entity2d::SetFade() at the same time,
the fade will override the tinting.

Default RGBA = (0, 0, 0) (Without tint)

*/
void IND_Entity2d::SetTint (unsigned char pR, unsigned char pG, unsigned char pB)
{
	mR = pR; mG = pG; mB = pB;
}


/*!
\b Parameters:

\arg <b>pA</b>       Byte A

\b Operation:

This function sets the level of transparency of the entity.

The A unsigned char indicates the level of transparency. If a value of 0 is used, the following graphical object
will be completely transparent, as opposed to the value 255 that will cause the object to be drawn completely opaque.
It is possible to use all the intermediate values for different levels of transparency.

Default A = (255) (Without transparency)

*/
void IND_Entity2d::SetTransparency (unsigned char pA)
{
	mA = pA;
}


/*!
\b Parameters:

\arg <b> pR, pG, pB, pA </b>       Bytes R, G, B, A

\b Operation:

This function sets the bytes FadeR, FadeG, FadeB used for fading the image to a certain color.
For example, if we use RGB = (255, 0, 0), the image will progressively become red.  Note: It is not
the same as the tint. The tint produces a colored transparent effect (like looking through celophane),
whereas the fade changes the image to the selected color completely.

The unsigned char A indicates the level of fade. If a value 0 is used, the following graphical object
will be filled solid with the selected color, as opposed to the value 255 that would cause
the object to be drawn completely normally. It is possible to use all the intermediate values for different
levels of fade.

It is not possible to use IND_Entity2d::SetTint() and IND_Entity2d::SetFade() at the same time,
the fade will override upon the tinting.

Default RGBA = (0, 0, 0, 255) (Without fade)
*/
void IND_Entity2d::SetFade (unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA)
{
	mFadeR = pR;
	mFadeG = pG;
	mFadeB = pB;
	mFadeA = pA;
}


/*!
\b Parameters:

\arg \b		pSo						Indicates the blending source, see (::IND_BlendingType).

\b Operation:

This function sets the blending source.

Default: 0. (Blending source is not in use).
*/
void IND_Entity2d::SetBlendSource (IND_BlendingType pSo)
{
	mSo = pSo;
}


/*!
\b Parameters:

\arg \b		pDs						Indicates blending destiny, see (::IND_BlendingType).

\b Operation:

This function sets the blending destiny.

Default 0. (Blending destiny is not in use).

*/
void IND_Entity2d::SetBlendDest	(IND_BlendingType pDs)
{
	mDs = pDs;
}


/*!
\b Parameters:

\arg \b pFile			Xml file defining the bounding areas

\b Operation:

This function return 1 (true) if an xml file that has the definitions of several
bounding areas for checking collisions is parsed correctly. This
method will return 0 (false) if the entity dosen't have a surface assigned
(check ::SetSurface(). For animations in which you want that each frame has different
collision areas don't use this method, define the .xml script directly in the </frame> section.

You can define in the xml file all the triangular, rectangular and circular areas that you want.
It is possible to "group" the bounding areas of the 2d entity using the "id" parameter. For
example you can add 3 triangles and 2 circles using the same id="head". An having at the same time
4 rectangles with the id="foot". Later, you can use this id parameter in the IND_Entity2dManager::IsCollision()
method for checking collision only in that areas.

Use ::ShowCollisionAreas() method in order to show or hide the collision areas of this entity.
In order to render all the collision areas you should use the
IND_Entity2dManager::RenderCollisionAreas() method.

Default: no bounding areas assigned.
*/
bool IND_Entity2d::SetBoundingAreas (const char *apFile)
{
	std::string pFile=(apFile);
	GetDebugAPI()->Header ("Parsing collision file", 5);
	GetDebugAPI()->Header ("File name:", 3);
	GetDebugAPI()->DataChar (pFile.c_str(), 1);

	// Only for surfaces
	if (!mSu)
	{
		GetDebugAPI()->Header ("The entity hasn't got a surface assigned, for animations you must define the collision scripts in the .xml file", 2);
		return 0;
	}

	// ----- Parsing collision file -----

	if (!mCollisionParser->ParseCollision (mListBoundingCollision, pFile.c_str()))
	{
		GetDebugAPI()->Header ("Fatal error, cannot load the collision xml file", 2);
		return 0;
	}

	// ----- GetDebugAPI() -----

	GetDebugAPI()->Header("Collision file loaded", 6);

	return 1;
}


/*!
\b Parameters:

\arg  \b pId				Id of the collision group, use this Id for group several bounding areas
							together. Later you can use IND_Entity2dManager::IsCollision() method passing
							this parameter to check a collision only in that areas. For
							example a head and ears of one character sprite can be 3 circular
							bounding areas grouped together using the same id="head": one for the head
							and two for the ears.
\arg \b  pAx \b pAy			Vertex A of the triangle
\arg \b  pBx \b pBy			Vertex B of the triangle
\arg \b  pCx \b pCy			Vertex C of the triangle

\b Operation:

This function return 1 (true) if a bounding triangle for checking collisions is defined. This
method will return 0 (false) if the entity dosen't have a surface assigned (check ::SetSurface().

You can assign all the triangle areas (and also circular and rectangular areas, using
::SetBoundingCircle() and ::SetBoundingRectangle()) that you want.
It is possible to "group" the bounding areas of the 2d entity using the "id" parameter. For
example you can add 3 triangles and 2 circles using the same id="head". An having at the same time
4 rectangles with the id="foot". Later, you can use this id parameter in the IND_Entity2dManager::IsCollision()
method for checking collision only in that areas.

You can also use the ::SetBoundingAreas() method in order to load all the bounding areas directly
from an xml file.

Use ::ShowCollisionAreas() method in order to show or hide the collision areas of this entity.
In order to render all the collision areas you should use the
IND_Entity2dManager::RenderCollisionAreas() method.

Default: no bounding triangle assigned.
*/
bool IND_Entity2d::SetBoundingTriangle (const char *pId,
										int pAx, int pAy,
										int pBx, int pBy,
										int pCx, int pCy)
{
	// Only for surfaces
	if (!mSu) return 0;

	mCollisionParser->SetBoundingTriangle (mListBoundingCollision, pId, pAx, pAy, pBx, pBy, pCx, pCy);

	return 1;
}


/*!
\b Parameters:

\arg  \b pId				Id of the collision group, use this Id for group several bounding areas
							together. Later you can use IND_Entity2dManager::IsCollision() method passing
							this parameter to check a collision only in that areas. For
							example a head and ears of one character sprite can be 3 circular
							bounding areas grouped together using the same id="head": one for the head
							and two for the ears.
\arg \b  pOffsetX			Horizontal displacement
\arg \b  pOffsetY			Vertical displacement
\arg \b  pRadius			Radius of the circle area

\b Operation:

This function return 1 (true) if a bounding circle for checking collisions is defined. This
method will return 0 (false) if the entity dosen't have a surface assigned
(check ::SetSurface().

You can assign all the circular areas (and also triangle and rectangular areas, using
::SetBoundingTriangle() and ::SetBoundingRectangle()) that you want.
It is possible to "group" the bounding areas of the 2d entity using the "id" parameter. For
example you can add 3 triangles and 2 circles using the same id="head". An having at the same time
4 rectangles with the id="foot". Later, you can use this id parameter in the IND_Entity2dManager::IsCollision()
method for checking collision only in that areas.

You can also use the ::SetBoundingAreas() method in order to load all the bounding areas directly
from an xml file.

Use ::ShowCollisionAreas() method in order to show or hide the collision areas of this entity.
In order to render all the collision areas you should use the
IND_Entity2dManager::RenderCollisionAreas() method.

Default: no bounding circle assigned.
*/
bool IND_Entity2d::SetBoundingCircle (const char *pId, int pOffsetX, int pOffsetY, int pRadius)
{
	// Only for surfaces
	if (!mSu) return 0;
	if (pRadius <= 0) return 0;

	mCollisionParser->SetBoundingCircle (mListBoundingCollision, pId, pOffsetX, pOffsetY, pRadius);

	return 1;
}


/*!
\b Parameters:

\arg \b pId					Id of the collision group, use this Id for group several bounding areas
							together. Later you can use IND_Entity2dManager::IsCollision() method passing
							this parameter to check a collision only in that areas. For
							example a head and ears of one character sprite can be 3 circular
							bounding areas grouped together using the same id="head": one for the head
							and two for the ears.
\arg \b  pOffsetX			Horizontal displacement
\arg \b  pOffsetY			Vertical displacement
\arg \b  pWidth				Width of the rectangle
\arg \b  pHeight			Height of the rectangle

\b Operation:

This function return 1 (true) if a bounding rectangle for checking collisions is defined. This
method will return 0 (false) if the entity dosen't have a surface assigned (check ::SetSurface().

You can assign all the rectangular areas (and also triangle and circular areas, using
::SetBoundingTriangle() and ::SetBoundingCircle()) that you want.
It is possible to "group" the bounding areas of the 2d entity using the "id" parameter. For
example you can add 3 triangles and 2 circles using the same id="head". An having at the same time
4 rectangles with the id="foot". Later, you can use this id parameter in the IND_Entity2dManager::IsCollision()
method for checking collision only in that areas.

You can also use the ::SetBoundingAreas() method in order to load all the bounding areas directly
from an xml file.

Use ::ShowCollisionAreas() method in order to show or hide the collision areas of this entity.
In order to render all the collision areas you should use the
IND_Entity2dManager::RenderCollisionAreas() method.

Default: no bounding rectangle assigned.
*/
bool IND_Entity2d::SetBoundingRectangle(const char *pId, int pOffsetX, int pOffsetY, int pWidth, int pHeight)
{
	// Only for surfaces
	if (!mSu) return 0;
	if (pWidth <= 0 || pHeight <= 0) return 0;

	mCollisionParser->SetBoundingRectangle (mListBoundingCollision, pId, pOffsetX, pOffsetY, pWidth, pHeight);

	return 1;
}


/*!
\b Parameters:

\arg \b  pId				Id of the collision group

\b Operation:

This function return 1 (true) if a bounding rectangle for checking collisions is deleted. This
method will return 0 (false) if the entity dosen't have a surface assigned (check ::SetSurface().

Use the pId parameter in order to define the group you want to delete. User "*" for delete all the groups.

Default: no bounding rectangle assigned.
*/
bool IND_Entity2d::DeleteBoundingAreas	(const char *pId)
{
	// Only for surfaces
	if (!mSu) return 0;

	mCollisionParser->DeleteBoundingAreas (mListBoundingCollision, pId);

	return 1;
}


/*!
\b Parameters:

\arg \b  pShowCollisionAreas                 (true / false) = (activate / deactivate)

\b Operation:

This function turn on / off the visualization of collision areas. This can be useful
for checking that your collisions are accuarte.

In order to render all the collision areas you should use the Entity2dManager::RenderCollisionAreas()
method.

Default: true.
*/
void IND_Entity2d::ShowCollisionAreas (bool pShowCollisionAreas)
{
	mShowCollisionAreas = pShowCollisionAreas;
}


/*!
\b Parameters:

\arg \b  pShowGridAreas                 (true / false) = (activate / deactivate)

\b Operation:

This function turn on / off the visualization of grid areas.

In order to render all the collision areas you should use the Entity2dManager::RenderGridAreas()
method.

Default: true.
*/
void IND_Entity2d::ShowGridAreas (bool pShowGridAreas)
{
	mShowGridAreas = pShowGridAreas;
}


// --------------------------------------------------------------------------------
//									 Private methods
// --------------------------------------------------------------------------------

/*
*************************
Attributes initialization
*************************
*/
void IND_Entity2d::InitAttrib ()
{
	// Assigned object
	mSu = 0;
	mAn = 0;
	mPri2d = 0;
	mFont = 0;

	// Showing
	mShow = 1;

	// Space transformation attributes
	mUpdateTransFlag = 1;
	mX = 0;
	mY = 0;
	mZ = 0;
	mAngleX = 0;
	mAngleY = 0;
	mAngleZ = 0;
	mScaleX = 1;
	mScaleY = 1;
	mCull = 1;
	mHotSpotX = 0;
	mHotSpotY = 0;
	mAxisCalX = 0;
	mAxisCalY = 0;
	mMirrorX = 0;
	mMirrorY = 0;
	mFilter = IND_FILTER_LINEAR;

	// Color, transperency and fading attributes
	mType = IND_OPAQUE;
	mR = 255;
	mG = 255;
	mB = 255;
	mA = 255;
	mFadeR = 0;
	mFadeG = 0;
	mFadeB = 0;
	mFadeA = 255;
	mSo = 0;
	mDs = 0;

	// Region
	mOffX = 0;
	mOffY = 0;
	mRegionWidth = 0;
	mRegionHeight = 0;

	// Tiling
	mWrap = 0;
	mUDisplace = 0.0f;
	mVDisplace = 0.0f;

	// Animation attributes
	mSequence = 0;
	mNumReplays	= -1;
	mFirstTime = 1;

	// 2d primitive attributes
    //mX1, mY1, mX2,
    mY2	= 0;
	mRadius = 0;
	mPolyPoints = 0;
	mNumLines = 0;
	mNumSides = 0;
	mPolyAngle = 0;

	// Fonts attributes
	mAlign = IND_RIGHT;
	mCharSpacing = 0;
	mLineSpacing = 20;
	mText = (char*)"";

	// Collision attributes
	mShowCollisionAreas = 1;
	mCollisionParser = CollisionParser::Instance();
	mListBoundingCollision = new list <BOUNDING_COLLISION*>;

	// Show grid areas
	mShowGridAreas = 1;
}


/*
*************************
Checks if the matrix has all its member equal to zero
*************************
*/
bool IND_Entity2d::IsNullMatrix ()
{
	if (!mMat.m[0][0] && !mMat.m[0][1]  && !mMat.m[0][2] && !mMat.m[0][3] &&
		!mMat.m[1][0] && !mMat.m[1][1]  && !mMat.m[1][2] && !mMat.m[1][3] &&
		!mMat.m[2][0] && !mMat.m[2][1]  && !mMat.m[2][2] && !mMat.m[2][3] &&
		!mMat.m[3][0] && !mMat.m[3][1]  && !mMat.m[3][2] && !mMat.m[3][3])
		return 1;

	return 0;
}

