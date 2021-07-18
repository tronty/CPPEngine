/*****************************************************************************************
 * File: IND_LightManager.cpp
 * Desc: Light manager
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

#include <IND_LightManager.h>


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b parameters:

\arg \b pRender					Pointer to IND_Render correctly initialized

\b Operation:

This function returns 1 (true) if the adminstrator is successfully initialized.
Must be called before using any method.
*/
bool IND_LightManager::Init (IND_Render *pRender)
{
	End ();
	InitVars ();

	GetDebugAPI()->Header ("Initializing LightManager", 5);
	GetDebugAPI()->Header ("Preparing LightManager", 1);

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

	mOk = true;

	GetDebugAPI()->Header ("LightManager OK", 6);

	return mOk;
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_LightManager::End()
{
	if (mOk)
	{
		GetDebugAPI()->Header ("Finalizing LightManager", 5);
		GetDebugAPI()->Header ("Freeing lights" , 5);
		FreeVars ();
		GetDebugAPI()->Header ("Lights freed", 6);
		GetDebugAPI()->Header ("LightManager finalized", 6);

		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//									  Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pNewLight				Pointer to a new IND_Light object
\arg \b pLightType				Type of the light. See ::IND_LightType.

\b Operation:

This function returns 1 (true) if the light object passed as a parameter exists and is added successfully to the manager.
See ::IND_LightType to see the different type of lights you can use.

Using this method, IndieLib will try to enable the light automatically. You can only have 8 ligths enabled at the same time
(not including ambient light), but you can add to the manager as many
as you want.

If you add a light to the manager and there are already 8 lights enabled, it will be automatically disabled.
You can disable other lights in the scene and then you will be able to enable this light.

Use the ::IND_Light methods in order to change the attributes of the light.
*/
bool IND_LightManager::Add (IND_Light *pNewLight, IND_LightType pLightType)
{
	GetDebugAPI()->Header ("Adding Light", 5);
	GetDebugAPI()->Header ("Light name:", 3);
	//GetDebugAPI()->DataInt (int(&pNewLight->mLight), 1);

	if (!mOk)
	{
		WriteMessage ();
		return 0;
	}

	// ----- Light creation -----

	switch (pLightType)
	{
		case IND_AMBIENT_LIGHT:
		{
			pNewLight->mLight.mLightType = IND_AMBIENT_LIGHT;
			pNewLight->mLight.mAttributes.mIsEnabled = 1;
			//mRender->GetDevice()->SetRenderState (D3DRS_AMBIENT, D3DCOLOR_XRGB (255, 255, 255));
			break;
		}

		case IND_DIRECTIONAL_LIGHT:
		{
			CreateLight (pNewLight, D3DLIGHT_DIRECTIONAL);

			break;
		}

		case IND_POINT_LIGHT:
		{
			CreateLight (pNewLight, D3DLIGHT_POINT);

			break;
		}

		case IND_SPOT_LIGHT:
		{
			CreateLight (pNewLight, D3DLIGHT_SPOT);

			break;
		}
	}


	// ----- Puts the object into the manager -----

	AddToList (pNewLight);

	GetDebugAPI()->Header("Light added", 6);

	return 1;
}


/*!
\b Parameters:

\arg \b pLight					Pointer to a IND_Light object

\b Operation:

This function returns 1(true) if the light object passed as parameter exists and it is
deleted from the manager successfully.
*/
bool IND_LightManager::Delete(IND_Light *pLight)
{
	GetDebugAPI()->Header ("Freeing light", 5);

	if (!mOk || !pLight)
	{
		WriteMessage ();
		return 0;
	}

	// Search object
	bool mIs = 0;
	list <IND_Light*>::iterator mListIter;
	for (mListIter  = mListLights->begin();
		 mListIter != mListLights->end();
		 mListIter++)
	{
		if ((*mListIter) == pLight)
		{
			mIs = 1;
			break;
		}
	}

	// Not found
	if (!mIs)
	{
		WriteMessage ();
		return 0;
	}

	// ----- Free object -----

	GetDebugAPI()->Header ("Light name:", 3);
	//GetDebugAPI()->DataInt (int(&pLight->mLight), 1);

	// Quit from list
	DelFromList (pLight);

	GetDebugAPI()->Header ("Ok", 6);

	return 1;
}


/*!
\b Parameters:

\b Operation:

This method must be called always before the IND_Entity3dManager::RenderEntities3d() method. It will
update all the lights attributes.
*/
void IND_LightManager::Update()
{
	list <IND_Light*>::iterator mListIter;
	for (mListIter  = mListLights->begin();
		 mListIter  != mListLights->end();
		 mListIter++)
	{
		// Ambient light
		if ((*mListIter)->mLight.mLightType == IND_AMBIENT_LIGHT)
		{
//???			mRender->GetDevice()->SetRenderState (D3DRS_AMBIENT, D3DCOLOR_XRGB	(	(unsigned char) (*mListIter)->mLight.mAttributes.mR * 255,
//???																					(unsigned char) (*mListIter)->mLight.mAttributes.mG * 255,
//???																					(unsigned char) (*mListIter)->mLight.mAttributes.mB * 255) )
		}
		else
		// Update lights that has been modified
		if ((*mListIter)->mLight.mSetLightFlag)
		{
			// Rest of lights
			(*mListIter)->mLight.mSetLightFlag = 0;
//???			mRender->GetDevice()->SetLight ((*mListIter)->mLight.mLightIndex, &(*mListIter)->mLight.mD3DLight)
		}
	}

}


/*!
\b Parameters:

\arg \b pLight					Pointer to a IND_Light object

\b Operation:

This function returns 1 (true) if the light object passed as a parameter exists and the light is enabled.

You can only have 8 ligths enabled at the same time (not including ambient light). This method will return 0 if there are
already 8 lights enabled.

Use the IND_LightManager::GetNumLightsEnabled() method in order to know how many lights are enabled.
*/
bool IND_LightManager::Enable (IND_Light *pLight)
{
#if 0
	if (!mOk)
	{
		WriteMessage ()
		return 0;
	}

	if (!pLight) return 0;

	// Check that the light is disabled
	if (!pLight->IsEnable())
	{
		// Ambient light
		if (pLight->mLight.mLightType == IND_AMBIENT_LIGHT)
		{
			pLight->mLight.mAttributes.mR = pLight->mLight.mAttributes.mLastR
			pLight->mLight.mAttributes.mG = pLight->mLight.mAttributes.mLastG
			pLight->mLight.mAttributes.mB = pLight->mLight.mAttributes.mLastB

			pLight->mLight.mAttributes.mIsEnabled = 1
		}
		// Rest of lights
		// Check how many lights are enabled already and that the light is not already enabled
		else
		if (GetNumLightsEnabled() < MAX_NUM_LIGHTS && !pLight->IsEnable())
		{
			mRender->GetDevice()->LightEnable (pLight->mLight.mLightIndex, 1)	// Enable light
			mNumLightsEnabled++
			pLight->mLight.mAttributes.mIsEnabled = 1
			return 1;
		}
	}
#endif
	return 0;
}


/*!
\b Parameters:

\arg \b pLight					Pointer to a IND_Light object

\b Operation:

This function returns 1 (true) if the light object passed as a parameter exists and the light is disabled.

Use the IND_LightManager::GetNumLightsEnabled() method in order to know how many lights are enabled.
*/

bool IND_LightManager::Disable (IND_Light *pLight)
{
#if 0
	if (!mOk)
	{
		WriteMessage ()
		return 0;
	}

	if (!pLight) return 0;

	// Check that the light is enable
	if (pLight->IsEnable())
	{
		// Ambient light
		if (pLight->mLight.mLightType == IND_AMBIENT_LIGHT)
		{
			pLight->mLight.mAttributes.mLastR = pLight->mLight.mAttributes.mR
			pLight->mLight.mAttributes.mLastG = pLight->mLight.mAttributes.mG
			pLight->mLight.mAttributes.mLastB = pLight->mLight.mAttributes.mB

			pLight->SetColor (0, 0, 0, 0)

			pLight->mLight.mAttributes.mIsEnabled = 0
		}
		// Rest of lights
		else
		{
			mRender->GetDevice()->LightEnable (pLight->mLight.mLightIndex, 0)	// Disable light
			mNumLightsEnabled--
			pLight->mLight.mAttributes.mIsEnabled = 0
		}

		return 1;
	}
#endif
	return 0;
}


// --------------------------------------------------------------------------------
//										Private methods
// --------------------------------------------------------------------------------

/*
*************************
Initializes a light
*************************
*/
void IND_LightManager::CreateLight (IND_Light *pNewLight, D3DLIGHTTYPE pLightType)
{
#if 0
	// Set light
	pNewLight->mLight.mD3DLight.Type = pLightType
	pNewLight->mLight.mLightIndex = mLightIndex
	mRender->GetDevice()->SetLight (mLightIndex, &pNewLight->mLight.mD3DLight)

	// Enable light if possible
	if (GetNumLightsEnabled() < MAX_NUM_LIGHTS)
	{
		mRender->GetDevice()->LightEnable (mLightIndex, 1)
		pNewLight->mLight.mAttributes.mIsEnabled = 1
		mNumLightsEnabled++
	}
	// Too much lights, create the light but disable it
	else
	{
		pNewLight->mLight.mAttributes.mIsEnabled = 0
	}

	// Increase light index
	mLightIndex++
#endif
}


/*
*************************
Inserts object into the manager
*************************
*/
void IND_LightManager::AddToList (IND_Light *pLi)
{
	mListLights->push_back (pLi);
}


/*
*************************
Deletes object from the manager
*************************
*/
void IND_LightManager::DelFromList	(IND_Light *pLi)
{
	mListLights->remove (pLi);
	pLi = 0;
}


/*
*************************
Initialization error message
*************************
*/
void IND_LightManager::WriteMessage ()
{
	GetDebugAPI()->Header ("This operation can not be done", 3);
	GetDebugAPI()->DataChar ("", 1);
	GetDebugAPI()->Header ("Invalid Id or IND_LightManager not correctly initialized", 2);
}


/*
*************************
Init manager vars
*************************
*/
void IND_LightManager::InitVars ()
{
	mListLights = new list <IND_Light*>;

	mNumLightsEnabled = 0;
	mLightIndex = 0;
}


/*
*************************
Free manager memory
*************************
*/
void IND_LightManager::FreeVars ()
{
	// Release all of the manager images
	list <IND_Light*>::iterator mLightListIter;
	for (mLightListIter  = mListLights->begin();
		 mLightListIter != mListLights->end();
		 mLightListIter++)
	{
		GetDebugAPI()->Header	("Freeing light:", 3);
		//GetDebugAPI()->DataInt (int(&(*mLightListIter)->mLight), 1);
	}

	// Clear list
	mListLights->clear ();

	// Free list
	Dispose (mListLights);
}

