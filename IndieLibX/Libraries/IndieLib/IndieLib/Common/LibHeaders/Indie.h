/*****************************************************************************************
/* File: IndieLib.h
/* Desc: Includes
/*****************************************************************************************/

#ifndef _INDIELIB_
#define _INDIELIB_

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (javierlopezpro@gmail.com) 

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

// Namespace
//include <windows.h>

#include <Framework3/IRenderer.h>
#include <fstream>
using namespace std;

int Getbbb();
int Getmmm();
D3DXFROMWINEMATRIX GetMVP();

// Defines and types
#include "Defines.h"

#include "DebugApi.h"
DebugApi * GetDebugAPI();

// Init/End
#include "IndieLib.h"

// Display
#include "IND_Window.h"

// Render
#include "IND_Render.h"

// Timer
#include "IND_Timer.h"

// Math
#include "IND_Math.h"

// Entities
#include "IND_Entity2dManager.h"
#include "IND_Entity3dManager.h"

// Input
#include "IND_Input.h"

// 2d Objects
#include "IND_ImageManager.h"
#include "IND_SurfaceManager.h"
#include "IND_AnimationManager.h"
#include "IND_FontManager.h"

// 3d Objects
#include "IND_3dMeshManager.h"

// Cameras
#include "IND_Camera2d.h"
#include "IND_Camera3d.h"

// Common
#include "IND_LightManager.h"

#endif // _INDIELIB_


