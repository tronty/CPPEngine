/*****************************************************************************************
/* File: IND_Light.h
/* Desc: Light object
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

#ifndef _IND_LIGHT_
#define _IND_LIGHT_

// ----- Includes -----

#include "IND_Render.h"


// --------------------------------------------------------------------------------
//									 IND_Light
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Light IND_Light
\ingroup Objects
IND_Light class managed by IND_LightManager for managing lights, click in IND_Light to see all the methods of this class.
*/
/*@{*/

/*!
\b IND_Image is an image object from the class ::IND_ImageManager. Read the explanation in ::IND_ImageManager for having more details.
*/
class DLL_EXP IND_Light
{
public:

	// ----- Public sets------

	void SetColor			(float pR, float pG, float pB, float pA);
	void SetPosition		(float pX, float pY, float pZ);
	void SetDirection		(float pD1, float pD2, float pD3);
	void SetAttenuation		(float pAttenuation);
	void SetRange			(float pRange);
	void SetTheta			(float pTheta);
	void SetPhi				(float pPhi);
	void SetFalloff			(float pFalloff);

	// ----- Public gets ------

	//! Returns true if the light is enabled, false otherwise.
	bool IsEnable				()			{ return mLight.mAttributes.mIsEnabled;						}
	//! Returns red color component of the light.
	float GetColorR				()			{ return mLight.mAttributes.mR;								}
	//! Returns green color component of the light.	
	float GetColorG				()			{ return mLight.mAttributes.mG;								}
	//! Returns blue color component	of the light.
	float GetColorB				()			{ return mLight.mAttributes.mB;								}
	//! Returns alpha  component	of the light.
	float GetColorA				()			{ return mLight.mAttributes.mA;								}
	//! Returns x position of the light.
	float GetPosX				()			{ return mLight.mAttributes.mX;								}
	//! Returns y position of the light.	
	float GetPosY				()			{ return mLight.mAttributes.mY;								}
	//! Returns z position of the light.	
	float GetPosZ				()			{ return mLight.mAttributes.mZ;								}
	//! Returns the first component of the vector which defines the light direction.
	float GetDirP1				()			{ return mLight.mAttributes.mD1;							}
	//! Returns the second component of the vector which defines the light direction.	
	float GetDirP2				()			{ return mLight.mAttributes.mD2;							}
	//! Returns the third component of the vector which defines the light direction.	
	float GetDirP3				()			{ return mLight.mAttributes.mD3;							}
	//! Returns the attenuation factor of the light.	
	float GetAttenuation		()			{ return mLight.mAttributes.mAttenuation;					}
	//! Returns the range factor of the light.		
	float GetRange				()			{ return mLight.mAttributes.mRange;							}
	//! Returns the range theta of the light.		
	float GetTheta				()			{ return mLight.mAttributes.mTheta;							}
	//! Returns the phi factor of the light.		
	float GetPhi				()			{ return mLight.mAttributes.mPhi;							}
	//! Returns the falloff factor of the light.		
	float GetFalloff			()			{ return mLight.mAttributes.mFalloff;						}

private:

	// ----- Structures ------

	// Attributes
	struct structAttributes
	{
		float mX, mY, mZ;				// Position
		float mR, mG, mB, mA;			// Color
		float mLastR, mLastG, mLastB;	// Previous color of the ambient light (for being able to disabe/enable;
		float mD1, mD2, mD3;			// Direction
		float mAttenuation;				// Attenuation (poing lights)
		float mRange;					// Range (point lights)
		float mTheta;					// Theta (spot lights)	
		float mPhi;						// Phi (spot lights)
		float mFalloff;					// Falloff (spot lights)
		bool mIsEnabled;				// Light ON / OFF

		structAttributes ()
		{
			mX = mY = mZ = 0;
			mR = mG = mB = mA = 1.0f;
			mLastR = mLastG = mLastB = 0.0f;
			mD1 = mD2 = mD3 = mAttenuation = mRange = mTheta = mPhi = mFalloff = 0;
			mIsEnabled = 0;
		}
	};
	typedef struct structAttributes ATTRIBUTES;

	// TYPE
	struct structLight
	{		
		bool			mSetLightFlag;				// Flag for knowing if we have to set the light because their attributes has been modified
		int				mLightIndex;				// Index that we have assigned to the light
		IND_LightType	mLightType;					// Light type
		D3DLIGHT9		mD3DLight;					// Direct3D light
		ATTRIBUTES		mAttributes;				// Light attributes

		structLight()
		{
			ZeroMemory (&mD3DLight, sizeof (D3DLIGHT9));
		}
	};
	typedef struct structLight LIGHT;

	LIGHT mLight;

	// ----- Friends -----
	
	friend class IND_LightManager;
	
};
/*@}*/

#endif // _IND_LIGHT_
