/*****************************************************************************************
 * File: IND_Light.cpp
 * Desc: Light object
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


#include <IND_Light.h>


// --------------------------------------------------------------------------------
//										Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pR                             Float R (Red)
\arg \b pG                             Float G (Green)
\arg \b pB                             Float B (Blue)

\b Operation:

Sets the light color. The color is specified using three floats (red, green and blue values). Usually you will use the
range between 0.0f and 1.0f. The higer the value the nearer to the color. For example, red color will be:
(1.0f, 0.0f, 0.0f).

Using values higher that 1.0f will increase the light intensity.

By Default: (1.0f, 1.0f, 1.0f)
*/
void IND_Light::SetColor (float pR, float pG, float pB, float pA)
{
	mLight.mAttributes.mR = pR;
	mLight.mAttributes.mG = pG;
	mLight.mAttributes.mB = pB;
	mLight.mAttributes.mA = pA;

	mLight.mD3DLight.Diffuse.r = pR;
	mLight.mD3DLight.Diffuse.g = pG;
	mLight.mD3DLight.Diffuse.b = pB;
	mLight.mD3DLight.Diffuse.a = pA;

	mLight.mSetLightFlag = 1;
}


/*!
\b Parameters:

\arg \b pX, \b pY, \b pZ				Position in the space

\b Operation:

Sets the light position.

By default: (0, 0, 0)
*/
void IND_Light::SetPosition	 (float pX, float pY, float pZ)
{
	mLight.mAttributes.mX = pX;
	mLight.mAttributes.mY = pY;
	mLight.mAttributes.mZ = pZ;

	mLight.mD3DLight.Position.x = pX;
	mLight.mD3DLight.Position.y = pY;
	mLight.mD3DLight.Position.z = pZ;

	mLight.mSetLightFlag = 1;
}


/*!
\b Parameters:

\arg \b pD1, \b pD2, \b pD3				The three coordinates define a vector with the direction of the light

\b Operation:

Sets the light direction. This value is used for ::IND_DIRECTIONAL_LIGHT and ::IND_SPOT_LIGHT lights.

For example, for a Sun light, you can choose a direction: (0.0f, -0.3f, 0.5) with a ::IND_DIRECTIONAL_LIGHT.

By default: (0, 0, 0)
*/
void IND_Light::SetDirection (float pD1, float pD2, float pD3)
{
	mLight.mAttributes.mD1 = pD1;
	mLight.mAttributes.mD2 = pD2;
	mLight.mAttributes.mD3 = pD3;

	// Create a direction for our light, it must be normalized
	D3DXFROMWINEVECTOR3 mVecDir;
	mVecDir = D3DXFROMWINEVECTOR3 (pD1, pD2, pD3);
	D3DXFROMWINEVec3Normalize ((D3DXFROMWINEVECTOR3*) &mLight.mD3DLight.Direction, &mVecDir);

	mLight.mSetLightFlag = 1;
}


/*!
\b Parameters:

\arg \b pAttenuation				Attenuation factor

\b Operation:

Sets the light attenuation. This value is used for ::IND_POINT_LIGHT and ::IND_SPOT_LIGHT lights.

By default: (0)
*/
void IND_Light::SetAttenuation	(float pAttenuation)
{
	mLight.mAttributes.mAttenuation = pAttenuation;

	mLight.mD3DLight.Attenuation0 = pAttenuation;

	mLight.mSetLightFlag = 1;
}


/*!
\b Parameters:

\arg \b pRange				Range factor

\b Operation:

Sets the light range. This value is used for ::IND_DIRECTIONAL_LIGHT, ::IND_POINT_LIGHT and ::IND_SPOT_LIGHT lights.

By default: (0)
*/
void IND_Light::SetRange (float pRange)
{
	mLight.mAttributes.mRange = pRange;

	mLight.mD3DLight.Range = pRange;

	mLight.mSetLightFlag = 1;
}


/*!
\b Parameters:

\arg \b pTheta				Theta factor

\b Operation:

Sets the light theta factor. This value is used for ::IND_SPOT_LIGHT lights.

By default: (0)
*/
void IND_Light::SetTheta (float pTheta)
{
	mLight.mAttributes.mTheta = pTheta;

	mLight.mD3DLight.Theta = pTheta;

	mLight.mSetLightFlag = 1;
}


/*!
\b Parameters:

\arg \b pPhi				Phi factor

\b Operation:

Sets the light phi factor. This value is used for ::IND_SPOT_LIGHT lights.

By default: (0)
*/
void IND_Light::SetPhi (float pPhi)
{
	mLight.mAttributes.mPhi = pPhi;

	mLight.mD3DLight.Phi = pPhi;

	mLight.mSetLightFlag = 1;
}


/*!
\b Parameters:

\arg \b pPhi				Phi factor

\b Operation:

Sets the light falloff factor. This value is used for ::IND_SPOT_LIGHT lights.

By default: (0)
*/
void IND_Light::SetFalloff	(float pFalloff)
{
	mLight.mAttributes.mFalloff = pFalloff;

	mLight.mD3DLight.Falloff = pFalloff;

	mLight.mSetLightFlag = 1;
}

