#if defined( BUILD_MESHRENDERER) 
#include <CIndieLib.h>
/*****************************************************************************************
 * File: IND_Entity3d.cpp
 * Desc: Entity 3d object
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (info@pixelartgames.com)

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


#include <IND_Entity3d.h>


// --------------------------------------------------------------------------------
//									 Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg <b> pNewMesh </b>                      Pointer to a mesh

\b Operation:

This function assigns a 3d mesh IND_3dMesh to the 3d entity
*/
void IND_Entity3d::Set3dMesh (IND_3dMesh *pNewMesh)
{
	InitAttrib ();

	mMe = pNewMesh;
}


/*!
\b Parameters:

\arg \b pShow             True = show the entity / False = hide entity

\b Operation:

This function showes / hides the entity. It dosen't delete it.

Default: 0.
*/
void IND_Entity3d::SetShow (bool pShow)
{
	mShow = pShow;
}


/*!
\b Parameters:

\arg \b pS                  Number of sequence to draw

\b Operation:

This function sets the animation sequence that will be rendered. When you use this method, the transition between the
current animation and the following will not be abrubt. IndieLib will fade out the effect of the first animation
and fade in the second. Use ::SetTransitionSpeed() in order set the speed of that transition.

Default: 0 (first sequence).
*/
bool IND_Entity3d::SetSequence	(int pS)
{
	if (!mMe) return 0;

	if (pS < 0 || pS > mMe->GetNumSequences()) return 0;

	mSequence = pS;
	mChangeSequenceFlag = 1;

	return 1;
}


/*!
\b Parameters:

\arg \b pSpeed                    Value higher than 0 that sets the animation speed

\b Operation:

This function sets the animation speed when changing from one sequence to another. The default value is 1.0f.
To speed up the animation choose values between 1.0f and 0.0f. The fastest animation possible is 0.0f. In order to
slow down the animation speed, choose values higher than 1.0f, the higher the value, the slower the animation.

Default: 1.0f
*/
void IND_Entity3d::SetAnimationSpeed	(float pSpeed)
{
	if (!mMe) return;

	mMe->SetAnimationSpeed (pSpeed);
}


/*!
\b Parameters:

\arg \b pSpeed                    Value higher than 0 that sets the animation speed

\b Operation:

When IndieLib is rendering an animation sequence of a 3d Mesh, if you change the animation sequence (using
IND_Entity3d::SetSequence()) the change will not be abrupt. IndieLib will fade out the effect of the first animation
and fade in the second.

This function sets the animation speed of this transition animation. The default value is 0.25f. To speed up the
transition choose values between 0.25f and 0.0f. The faster transition possible is 0.0f. In order to slow down the
animation speed, choose values higher than 0.25f, the higher the value, the slower the animation.

Default: 0.25f
*/
void IND_Entity3d::SetTransitionSpeed (float pSpeed)
{
	if (!mMe) return;

	mMe->SetTransitionSpeed (pSpeed);
}


/*!
\b Parameters:

\arg <b> pX, pY, pZ </b>                      Tranlation int the x,y and z axis.

\b Operation:

This function sets the position where the entity will be drawn.

Default (0, 0, 0).
*/
void IND_Entity3d::SetPosition (float pX, float pY, float pZ)
{
	mX = pX; mY = pY; mZ = pZ;
}


/*!
\b Parameters:

\arg \b pAnX						Angle of rotation in the x axis expressed in degrees.
\arg \b pAnY                        Angle of rotation in the y axis expressed in degrees.
\arg \b pAnZ                        Angle of rotation in the z axis expressed in degrees.

\b Operation:

This function sets the rotation of the entity in each of the coordinate axis.

Default (0, 0, 0).
*/
void IND_Entity3d::SetAngleXYZ (float pAnX, float pAnY, float pAnZ)
{
	mAngleX = pAnX; mAngleY = pAnY; mAngleZ = pAnZ;
}


/*!
\b Parameters:

\arg <b> pSx, pSy</b>                  Level of scaling in the x and y axis. (1,1) to keep the normal size

\b Operation:

This function sets the scaling of the entity in the x, y and z axis.

By default (1, 1, 1).
*/
void IND_Entity3d::SetScale	(float pSx, float pSy, float pSz)
{
	mScaleX = pSx; mScaleY = pSy;	mScaleZ = pSz;
}


/*!
\b Parameters:

\arg \b pCull            Back faces culling  activated / deactivated

\b Operation:

This function activates or deactivates the backface culling. (1 / 0) = (activated / deactivated). This parameter is interesting to deactivate when we rotate the graphical object in the x or y axis.

Activated by default (1).
*/
void IND_Entity3d::SetBackCull (bool pCull)
{
	mCull = pCull;
}


/*!
\b Parameters:

\arg \b    pF                   Type of filter ::IND_Filter. There are two types of filters that can be applied to the graphical object when it undergoes a rotation or scaling:
								- ::IND_FILTER_POINT (Nearest point filter). Less quality, but bigger performance. It is possible to use this without quality loss in graphical objects that don't rotate or are affected by scaling.
								- ::IND_FILTER_LINEAR (Bilinear filter). More quality, but less performance. Used in graphical objects that be rotated or affected by scaling.

\b Operation:

This function sets the filter that will affect the graphical object when drawing.

Default: ::IND_FILTER_LINEAR
*/
void IND_Entity3d::SetFilter (IND_Filter pF)
{
	mFilter = pF;
}


/*!
\b Parameters:

\arg \b    pFlipNormals				Flip normals 1 / 0 => on / off

\b Operation:

This function toggles the back face culling of the model. You can use this method if you see
if your model is being displayed incorrectly.

Default: 0
*/
void IND_Entity3d::FlipNormals (bool pFlipNormals)
{
	mFlipNormals = pFlipNormals;
}


/*!
\b Parameters:

\arg <b> pR, pG, pB</b>       Bytes R, G, B

\b Operation:

This function sets the bytes pR, pG, pB for "tinting" the 3d Mesh to a certain color. For example,
if we use RGB = (255, 0, 0), the mesh will be tinted  red. Note: It is not the same as tinting.
The tint produces a colored transparent effect (similar to looking through celophane),
whereas the fade completely changes the image to the color selected.

It is not possible to use IND_Entity3d::SetTint() and IND_Entity3d::SetFade() at the same time,
the fade will prevail upon the tinting.

Default RGBA = (0, 0, 0) (Without tint)
*/
void IND_Entity3d::SetTint (unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA)
{
	mR = pR; mG = pG; mB = pB; mA = pA;
}


/*!
\b Parameters:

\arg <b>pA</b>       Byte A

\b Operation:

This function sets the level of transparency of the entity.

The A unsigned char indicates the level of transparency. If a value 0 is used, the following graphical
object will be completely transparent, as opposed to the value 255 that will be drawn completely
opaque. It is possible to use all the intermediate values for different levels of transparency.

Default A = (255) (Without transparency)
*/
void IND_Entity3d::SetTransparency (unsigned char pA)
{

	mA = pA;
}


/*!
\b Parameters:

\arg <b> pR, pG, pB, pA </b>       Bytes R, G, B, A

\b Operation:

This function sets the bytes FadeR, FadeG, FadeB for making a fade of the 3d mesh to a certain
color. When we speak of fade we refer to the progressive change of the mesh to certain color.
For example, if we use RGB = (255, 0, 0), the mesh will be faded red. Note: It is not
the same as tinting. Tint produces an colored transparent effect(like looking through cellophane,
whereas the fade changes the mesh to the color selected completely.

The unsigned char A indicates the level of fade. If the value 0 is used, the following graphical object
will be filled solid with the selected color, as opposed to the value 255 that would cause
the object to be drawn completely normally. It is possible to use all the intermediate values for different
levels of fade.

It is not possible to use IND_Entity3d::SetTint() and IND_Entity3d::SetFade() at the same time,
the fade will override the tinting.

Default RGBA = (0, 0, 0, 255) (Without fade)
*/
void IND_Entity3d::SetFade (unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA)
{
	mFadeR = pR; mFadeG = pG; mFadeB = pB; mFadeA = pA;
}


/*!
\b Parameters:

\arg \b		pSo                    Indicates the blending source, see ( ::IND_BlendingType).

\b Operation:

This function sets the blending source.

Default: 0. (Blending source is not in use).
*/
void IND_Entity3d::SetBlendSource (IND_BlendingType pSo)
{
	mSo = pSo;
}


/*!
\b Parameters:

\arg \b		pDs                            Indicates blending destiny, see ( ::IND_BlendingType).

\b Operation:

This function sets the blending destiny.

Default 0. (Blending destiny is not in use).
*/
void IND_Entity3d::SetBlendDest (IND_BlendingType pDs)
{
	mDs = pDs;
}


// --------------------------------------------------------------------------------
//									 Private methods
// --------------------------------------------------------------------------------

/*
*************************
Attributes initialization
*************************
*/
void IND_Entity3d::InitAttrib ()
{
	// Assigned object
	mMe = 0;

	// Showing
	mShow = 1;

	// Space transformation attributes
	mX = 0;
	mY = 0;
	mZ = 0;
	mAngleX = 0;
	mAngleY = 0;
	mAngleZ = 0;
	mScaleX = 1;
	mScaleY = 1;
	mScaleZ = 1;
	mFilter = IND_FILTER_LINEAR;

	// Color, transperency, fading and back face culling attributes
	mCull = 1;
	mFlipNormals = 0;
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

	// Animations attributes
	mChangeSequenceFlag	= 0;
	mSequence = 0;
}

#endif
