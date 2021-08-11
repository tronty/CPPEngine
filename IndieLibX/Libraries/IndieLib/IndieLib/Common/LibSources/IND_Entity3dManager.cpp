#if defined( BUILD_MESHRENDERER) 
#include <CIndieLib.h>
/*****************************************************************************************
 * File: IND_Entity3dManager.cpp
 * Desc: Manager de entidades. Una entidad contiene el un objeto gráfico y gestiona
 * sus atributos.
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

#include <IND_Entity3dManager.h>


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pRender					Pointer to IND_Render

\b Operation:

This function returns 1(true) if the manager is initialized satisfactorily.
It must be called before using any method.
*/
bool IND_Entity3dManager::Init (IND_Render *pRender)
{
	End ();
	InitVars ();

	GetDebugAPI()->Header ("Initializing Entity3dManager", 5);
	GetDebugAPI()->Header ("Preparing Entity3dManager", 1);

	mOk = true;

	// Checking IND_Render
	if (pRender->IsOk())
	{
		GetDebugAPI()->Header ("Checking IND_Render", 1);
		mRender = pRender;
	}
	else
	{
		GetDebugAPI()->Header ("IND_Render is not correctly itialized", 2);
		mOk = false;
		return mOk;
	}

	GetDebugAPI()->Header ("Entity3dManager OK", 6);

	return mOk;
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_Entity3dManager::End()
{
	if (mOk)
	{
		// Si este objeto ya había cargado una entidad 3d, lo liberamos
		GetDebugAPI()->Header ("Finalizing Entity3dManager", 5);
		GetDebugAPI()->Header ("Freeing 3d entities" , 5);
		FreeVars ();
		GetDebugAPI()->Header ("3d entities freed", 6);
		GetDebugAPI()->Header ("Entity3dManager finalized", 6);

		mOk = false;
	}
}

// --------------------------------------------------------------------------------
//									 Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pNewEntity3d				Pointer to an entity object.

\b Operation:

This function returns 1 (true) if the entity object passed as parameter exists
and is added to the manager successfully.
*/
bool IND_Entity3dManager::Add (IND_Entity3d *pNewEntity3d)
{
	GetDebugAPI()->Header ("Adding 3d entity", 5);
	GetDebugAPI()->Header ("Name:", 3);
	//GetDebugAPI()->DataInt (int(pNewEntity3d), 1);

	if (!mOk)
	{
		WriteMessage ();
		return 0;
	}

	// ----- Puts the entinty into the manager -----

	AddToList (pNewEntity3d);

	// ----- GetDebugAPI() -----

	GetDebugAPI()->Header("3d entity added", 6);

	return 1;
}


/*!
\b Parameters:

\arg \b pNewEntity3d				Pointer to an entity object.

\b Operation:

This function returns 1 (true) if the entity object passed as parameter exists
and is deleted from the manager successfully.
*/
bool IND_Entity3dManager::Delete (IND_Entity3d *pEn)
{
	GetDebugAPI()->Header ("Freeing 3d entity", 5);

	if (!mOk || !pEn)
	{
		WriteMessage ();
		return 0;
	}

	// Search object
	bool mIs = 0;
	vector <IND_Entity3d*>::iterator mListIter;
	for (mListIter  = mListEntities3d->begin();
		 mListIter  != mListEntities3d->end();
		 mListIter++)
	{
		if ((*mListIter) == pEn)
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

	GetDebugAPI()->Header ("Name:", 3);
	//GetDebugAPI()->DataInt (int(pEn), 1);

	// Quit from list
	mListEntities3d->erase (mListIter);

	GetDebugAPI()->Header ("Ok", 6);

	return 1;
}


/*!
\b Operation:

This function renders (blits on the screen) all the 3d entities of the manager.
*/
#include <IND_Viewport.h>
extern IND_Viewport g_Viewport3d;
extern IND_Camera3d g_Camera3d;
void IND_Entity3dManager::RenderEntities3d ()
{
	if (!mOk || mListEntities3d->empty())
	{
		return;
	}

	// Iterates the list
	vector <IND_Entity3d*>::iterator mIter;
	for (mIter  = mListEntities3d->begin();
		 mIter != mListEntities3d->end();
		 mIter++)
	{
		// Only render it if "show" flag is true
		if ((*mIter)->mShow)
		{
			// If it has a 3d Mesh
			if ((*mIter)->mMe)
			{
#if 0
				mRender->SetTransform3d	((*mIter)->mX,
										(*mIter)->mY,
										(*mIter)->mZ,
										(*mIter)->mAngleX,
										(*mIter)->mAngleY,
										(*mIter)->mAngleZ,
										(*mIter)->mScaleX,
										(*mIter)->mScaleY,
										(*mIter)->mScaleZ,
										&(*mIter)->mMat);

#else
				(*mIter)->mMe->SetTransform3d	((*mIter)->mX,
										(*mIter)->mY,
										(*mIter)->mZ,
										(*mIter)->mAngleX,
										(*mIter)->mAngleY,
										(*mIter)->mAngleZ,
										(*mIter)->mScaleX,
										(*mIter)->mScaleY,
										(*mIter)->mScaleZ);
#endif
				// ----- Color attributes -----

				mRender->SetRainbow3d	((*mIter)->mCull,
										(*mIter)->mFlipNormals,
										(*mIter)->mFilter,
										(*mIter)->mR,
										(*mIter)->mG,
										(*mIter)->mB,
										(*mIter)->mA,
										(*mIter)->mFadeR,
										(*mIter)->mFadeG,
										(*mIter)->mFadeB,
										(*mIter)->mFadeA,
										(*mIter)->mSo,
										(*mIter)->mDs);

				// ----- Blit Mesh -----

				if ((*mIter)->mChangeSequenceFlag)
				{
					(*mIter)->mChangeSequenceFlag = 0;
					mRender->Set3dMeshSequence ((*mIter)->mMe, (*mIter)->mSequence);
				}

				mRender->Blit3dMesh ((*mIter)->mMe);
			}
		}
	}
}


// --------------------------------------------------------------------------------
//									 Private methods
// --------------------------------------------------------------------------------

/*
*************************
Inserts object into the manager
*************************
*/
void IND_Entity3dManager::AddToList (IND_Entity3d *pNewEntity3d)
{
	mListEntities3d->push_back (pNewEntity3d);
}


/*
*************************
Initialization error message
*************************
*/
void IND_Entity3dManager::WriteMessage ()
{
	GetDebugAPI()->Header ("This operation can not be done", 3);
	GetDebugAPI()->DataChar ("", 1);
	GetDebugAPI()->Header ("Invalid Id or IND_Entity3dManager not correctly initialized", 2);
}


/*
*************************
Init manager vars
*************************
*/
void IND_Entity3dManager::InitVars ()
{
	mListEntities3d = new vector <IND_Entity3d*>;
}


/*
*************************
Free manager memory
*************************
*/
void IND_Entity3dManager::FreeVars ()
{
	// Deletes all the manager entities
	vector <IND_Entity3d*>::iterator mListIter;

	for (mListIter  = mListEntities3d->begin();
		 mListIter != mListEntities3d->end();


		 mListIter++)
	{
		GetDebugAPI()->Header	("Freeing 3d entity:", 3);
		//GetDebugAPI()->DataInt (int((*mListIter)), 1);
	}

	// Clear list
	mListEntities3d->clear ();

	// Free list
	Dispose (mListEntities3d);
}








#endif
