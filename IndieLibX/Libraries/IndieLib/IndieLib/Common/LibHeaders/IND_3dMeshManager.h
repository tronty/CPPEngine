#if defined( BUILD_MESHRENDERER) 
/*****************************************************************************************
/* File: IND_3dMeshManager.h
/* Desc: Manager de mallas 3d
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

#ifndef _IND_3DMESHMANAGER_
#define _IND_3DMESHMANAGER_

// ----- Includes -----

#include <list>
//#include "XMeshStructures.h"
//#include "IND_Render.h"
//#include "IND_3dMesh.h"


// --------------------------------------------------------------------------------
//									IND_3dMeshManager
// --------------------------------------------------------------------------------

/*!
\defgroup IND_3dMeshManager IND_3dMeshManager
\ingroup Managers
Manager of IND_3dMesh objects. Click in ::IND_3dMeshManager to see all the methods of this class.
*/
/*@{*/

/*!
This class stores 3d meshes (IND_3dMesh) that can be inserted into a IND_Entity3d and rendered to the screen using
IND_Entity3dManager::RenderEntities3d().
*/
class IND_3dMesh;
class IND_Render;
class DLL_EXP IND_3dMeshManager
{
public:
	
	// ----- Init/End -----

	IND_3dMeshManager		(): mOk(false)	{ }
	~IND_3dMeshManager		()				{ End(); }

	bool	Init    (IND_Render *pRender);
	void	End     ();
	bool	IsOk	()			 const { return mOk; }
					
	// ----- Public methods -----

	bool Add			(IND_3dMesh *pNew3dMesh, const char *pName);
	bool Add			(IND_3dMesh *pNew3dMesh, const char *pName, const char *pPath);
	bool Delete			(IND_3dMesh *pMe);
	
private:

	// ----- Private Methods -----

	bool mOk;

	// ----- Objects -----

	IND_Render *mRender;
	
	// ----- Containers -----

	list <IND_3dMesh*> *mList3dMesh;

	// ----- Private methods -----

	//void SetupBoneMatrices	(D3DXFROMWINEFRAME_EXTENDED *pFrame, LPD3DXFROMWINEMATRIX pParentMatrix, IND_3dMesh *pNew3dMesh);

	void AddToList			(IND_3dMesh *pNew3dMesh);
	void DelFromList		(IND_3dMesh *pMe);
	void WriteMessage		();
	void InitVars			();
	void FreeVars			();
};
/*@}*/
	
#endif // _IND_3DMESHMANAGER_
#endif

