#include <CIndieLib.h>
#define __MESHRENDERER__ 1
#if defined( BUILD_MESHRENDERER) 
/*****************************************************************************************
 * File: IND_3dMeshManager.cpp
 * Desc: Manager de mallas 3d
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
  Copyright (c) 2015 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	
*/

// ----- Includes -----


//include "XMeshHierarchy.h"
#include <IND_Render.h>
#include <IND_3dMeshManager.h>


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pImageManager			Pointer to the manager IND_ImageManager correctly initialized
\arg \b pRender					Pointer to the manager IND_Render correctly initialized

\b Operation:

This function returns 1 (true) if the adminstrator is successfully initialized.
Must be called before using any method.
*/

//class Assimp::Importer IND_3dMeshManager::importer

bool IND_3dMeshManager::Init (IND_Render *pRender)
{
	End ();
	InitVars ();

	// Checking IND_render
	if (pRender->IsOk())
	{
		GetDebugAPI()->Header ("Checking IND_Render", 1);
		mRender = pRender;
		mOk = true;
	}
	else
	{
		GetDebugAPI()->Header ("IND_Render is not correctly initialized", 2);
		mOk = false;
		return mOk;
	}

	GetDebugAPI()->Header ("Initializing IND_3dMeshManager", 5);
	GetDebugAPI()->Header ("IND_3dMeshManager OK", 6);

	return mOk;
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_3dMeshManager::End()
{
	if (mOk)
	{
		// Free the subject if it has already loaded a mesh
		GetDebugAPI()->Header ("Finalizing IND_3dMeshManager", 5);
		GetDebugAPI()->Header ("Freeing 3d Meshes" , 5);
		FreeVars ();
		GetDebugAPI()->Header ("3d meshes freed", 6);
		GetDebugAPI()->Header ("IND_3dMeshManager finalized", 6);

		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//									  Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pNew3dMesh				Pointer to a new IND_3dMesh object
\arg \b pName					Name of the 3d mesh
\arg \b pPath					Path where the 3d mesh is stored

\b Operation:

This function adds a 3d Mesh from a .x file. You can use ::IND_Entity3d in order to display the 3d mesh on the screen and to change its
attributes.
*/
//class Assimp::Importer IND_3dMesh::importer
bool IND_3dMeshManager::Add (IND_3dMesh *pNew3dMesh, const char *filename)
{
	std::string basepath;
	std::string modelname;
    int strLastSlash = int(stx_strrchr( filename, stx_getOsSeparator() )-filename);
	if(strLastSlash>0)
	{
	basepath=filename;//+'\0';
//	basepath[strLastSlash+1]='\0';
	basepath=basepath.substr(0,strLastSlash);
	modelname.append(filename+strLastSlash+1);
	}
	else
	{
	basepath="";
	modelname=filename;
	}
	LOG_PRINT("basepath=%s\n",basepath.c_str());
	LOG_PRINT("modelname=%s\n",modelname.c_str());
	stx_exit(0);
    return Add (pNew3dMesh, filename,const_cast<const char*>(basepath.c_str()));
}
const char* pathIL();
bool IND_3dMeshManager::Add (IND_3dMesh *pNew3dMesh, const char *apName, const char *apPath)
{
	std::string pName=(apName);

	std::string pPath=(apPath);

	GetDebugAPI()->Header ("Loading 3d Mesh", 5);

	GetDebugAPI()->Header ("File name:", 3);

	GetDebugAPI()->DataChar (pName.c_str(), 1);

	LOG_PRINT("IND_3dMeshManager::Add:%s\n", apName);

	if (!mOk)
	{
		WriteMessage ();
		return 0;
	}

delete pNew3dMesh->m3dMesh;

pNew3dMesh->m3dMesh=new MeshRenderer2();//pPath.c_str());

LOG_PRINT("IND_3dMeshManager::Add:pName=%s\n",pName.c_str());
//pNew3dMesh->m3dMesh->LoadAsset(pName.c_str());
pNew3dMesh->m3dMesh->Load(pName.c_str(), eSimpleShader, true, true);

	// ----- Puts the object into the manager -----


	if(pNew3dMesh->m3dMesh->m_pAnimator )
		pNew3dMesh->m3dMesh->SetPlay(true);

	AddToList (pNew3dMesh);

	GetDebugAPI()->Header("3d Mesh loaded", 6);

	return 1;
}


/*!
\b Parameters:

\arg \b pMe                 Pointer to an 3d Mesh object.

\b Operation:

This function returns 1 (true) if the 3d Mesh object passed as parameter exists and is
deleted from the manager successfully.
*/
bool IND_3dMeshManager::Delete (IND_3dMesh *pMe)
{
	GetDebugAPI()->Header ("Freeing 3d Mesh", 5);

	if (!mOk || !pMe)
	{
		WriteMessage ();
		return 0;
	}

	// Search object
	bool mIs = 0;
	list <IND_3dMesh*>::iterator mListIter;
	for (mListIter  = mList3dMesh->begin();
		 mListIter != mList3dMesh->end();
		 mListIter++)
	{
		if ((*mListIter) == pMe)
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

	GetDebugAPI()->Header	("Freeing 3d mesh:", 3);
	GetDebugAPI()->DataChar (pMe->m3dMesh->GetFileName(),1);//GetMeshName (), 1)

	// Quit from list
	DelFromList (pMe);

	delete pMe;


	// Free the pointer
	Dispose (pMe);

	GetDebugAPI()->Header ("Ok", 6);

	return 1;
}


// --------------------------------------------------------------------------------
//									  Private methods
// --------------------------------------------------------------------------------

/*
*************************
We need to go through the hierarchy and set the combined matrices calls recursively
pFrame - current frame
pParentMatrix - the parent frame matrix
*************************
*/
#if 0
void IND_3dMeshManager::SetupBoneMatrices (D3DXFROMWINEFRAME_EXTENDED *pFrame, LPD3DXFROMWINEMATRIX pParentMatrix, IND_3dMesh *pNew3dMesh)
{
	DBG_HALT
#if 0
	// Cast to our extended structure first
	D3DXFROMWINEMESHCONTAINER_EXTENDED* pMesh = (D3DXFROMWINEMESHCONTAINER_EXTENDED*) pFrame->pMeshContainer

	// If this frame has a mesh
	if (pMesh)
	{
		// We need to remember which is the first mesh in the hierarchy for later when we
		// update (FrameMove)
		if(!pNew3dMesh->m3dMesh.m_firstMesh)
			pNew3dMesh->m3dMesh.m_firstMesh = pMesh

		// if there is skin info, then setup the bone matrices
		if(pMesh->pSkinInfo && pMesh->MeshData.pMesh)
		{
			// Create a copy of the mesh to skin into later
			D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE]
			if (FAILED(pMesh->MeshData.pMesh->GetDeclaration(Declaration)))
				return

//???			pMesh->MeshData.pMesh->CloneMesh(D3DXFROMWINEMESH_MANAGED,
//???				Declaration, mRender->GetDevice(),
//???				&pMesh->exSkinMesh)

			// Max bones is calculated for later use (to know how big to make the bone matrices array)
			pNew3dMesh->m3dMesh.m_maxBones = max (pNew3dMesh->m3dMesh.m_maxBones, (int)pMesh->pSkinInfo->GetNumBones())

			// For each bone work out its matrix
			for (unsigned int i = 0 i < pMesh->pSkinInfo->GetNumBones() i++)
			{
				// Find the frame containing the bone
				D3DXFROMWINEFRAME_EXTENDED* pTempFrame = (D3DXFROMWINEFRAME_EXTENDED*)D3DXFROMWINEFrameFind (pNew3dMesh->m3dMesh.m_frameRoot,
						pMesh->pSkinInfo->GetBoneName(i))

				// set the bone part - point it at the transformation matrix
				pMesh->exFrameCombinedMatrixPointer[i] = &pTempFrame->exCombinedTransformationMatrix
			}

		}
	}

	// Pass on to sibblings
	if(pFrame->pFrameSibling)
		SetupBoneMatrices((D3DXFROMWINEFRAME_EXTENDED*)pFrame->pFrameSibling, pParentMatrix, pNew3dMesh)



	// Pass on to children
	if(pFrame->pFrameFirstChild)
		SetupBoneMatrices((D3DXFROMWINEFRAME_EXTENDED*)pFrame->pFrameFirstChild, &pFrame->exCombinedTransformationMatrix, pNew3dMesh)
#else//???
#endif
}
#endif

/*
*************************
Inserts object into the manager
*************************
*/
void IND_3dMeshManager::AddToList (IND_3dMesh *pNew3dMesh)
{
	mList3dMesh->push_back (pNew3dMesh);
}


/*
*************************
Deletes object from the manager
*************************
*/
void IND_3dMeshManager::DelFromList (IND_3dMesh *pMe)
{
	mList3dMesh->remove (pMe);
	pMe = 0;
}


/*
*************************
Initialization error message
*************************
*/
void IND_3dMeshManager::WriteMessage ()
{
	GetDebugAPI()->Header ("This operation can not be done", 3);
	GetDebugAPI()->DataChar ("", 1);
	GetDebugAPI()->Header ("Invalid Id or IND_3dMeshManager not correctly initialized", 2);
}


/*
*************************
Init manager vars
*************************
*/
void IND_3dMeshManager::InitVars ()
{
	mList3dMesh = new list <IND_3dMesh*>;
}


/*
*************************
Free memory
*************************
*/
void IND_3dMeshManager::FreeVars ()
{
	// Deletes all the manager entities
	list <IND_3dMesh*>::iterator mListIter;

	for (mListIter  = mList3dMesh->begin();
			mListIter != mList3dMesh->end();
			mListIter++)
	{
		GetDebugAPI()->Header	("Freeing 3d mesh:", 3);
		GetDebugAPI()->DataChar ((*mListIter)->m3dMesh->GetFileName(),1);//GetMeshName (), 1)

	}

	// Clear list
	mList3dMesh->clear ();

	// Free list
	Dispose (mList3dMesh);
}
#endif
