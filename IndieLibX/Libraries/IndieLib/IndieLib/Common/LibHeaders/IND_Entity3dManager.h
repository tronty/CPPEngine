#if defined( BUILD_MESHRENDERER) 
/*****************************************************************************************
/* File: IND_Entity3dManager.h
/* Desc: Manager de entidades 3d. Una entidad contiene el una malla 3d y gestiona
/* sus atributos.
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


#ifndef _IND_ENTITY3DMANAGER_
#define _IND_ENTITY3DMANAGER_

// ----- Includes -----

#include "IND_3dMeshManager.h"
#include "IND_Render.h"
#include "IND_Entity3d.h"


// --------------------------------------------------------------------------------
//									IND_Entity3dManager
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Entity3dManager IND_Entity3dManager
\ingroup EntityManagers
Manager of ::IND_Entity3d objects. Click in ::IND_Entity3dManager to see all the methods of this class.
*/
/*@{*/ 

/*!
Manager of ::IND_Entity3d objects. Used for storing these type of objects. 

Other uses of this class:
- Function for drawing all the entities stored
*/
class DLL_EXP IND_Entity3dManager
{
public:
	
	// ----- Init/End -----

	IND_Entity3dManager			(): mOk(false)	{ }
	~IND_Entity3dManager		()				{ End(); }

	bool	Init    (IND_Render *pRender);
	void	End     ();
	bool	IsOk	()			 const { return mOk; }
					
	// ----- Métodos -----
	
	bool		Add					(IND_Entity3d *pNewEntity3d);
	bool		Delete				(IND_Entity3d *pEn);

	void		RenderEntities3d	();

private:

	// ----- Métodos privados -----

	bool mOk;

	IND_Render *mRender;

	// ----- Contenedores -----

	// Lista de imágenes
	vector <IND_Entity3d*> *mListEntities3d;

	// ----- Operaciones -----

	void AddToList			(IND_Entity3d *pNewEntity3d);

	void WriteMessage		();
	void InitVars			();
	void FreeVars			();
};
/*@}*/
	
#endif // _IND_ENTITY3DMANAGER_
#endif

