/*****************************************************************************************
 * File: IndieLib.cpp
 * Desc: IndieLib initialization / destruction
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
#define __ImageLib__ 1
#include <CIndieLib.h>



// ----- Librerias -----

// ImageLib lib (for loading images and applying filters)

// SDL lib (for input, timers and window creation)

// ----- Header -----

#include <IndieLib.h>
/*
	structMatrix::structMatrix ()
	{
//		_11 = _12 = _13 = _14 = _21 = _22 = _23 = _24 = _31 = _32 = _33 = _34 = _41 = _42 = _43 = _44 = 0
	stx_memset(m,0,sizeof(float)*16);
	}
*/

// --------------------------------------------------------------------------------
//							 Initialization /Destruction
// --------------------------------------------------------------------------------

/*!
\arg \b pMode				::IND_InitializationMode. See ::IND_InitializationMode.

\b Operation:

This function initializes IndieLib.
*/
void IndieLib::Init (IND_InitializationMode pMode)
{
	// GetDebugAPI() initilization
	if (pMode)	GetDebugAPI()->Init ();

//ililuInit();//true)
	// SDL initialization
	//_putenv("SDL_VIDEODRIVER=windib")
	//SDL_Init (SDL_INIT_TIMER)
}


/*!
\b Operation:

This function finalizes IndieLib and frees all the memory of the managers. So, all the resources like textures,
3d meshes, etc. will be freed.
*/
extern DebugApi *Debug;
void IndieLib::End ()
{
	GetDebugAPI()->End();
	Dispose (Debug);

	STX_Service::CloseAll();
}

