/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//--------------------------------------------------------------------------------------
// File: DX11TessellationTest.h
//
// Copyright (c) AMD Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#ifndef DX11TESSELLATIONTEST_H
#define DX11TESSELLATIONTEST_H

struct SIMPLEVERTEX 
{
        FLOAT x, y, z;
        FLOAT u, v;
};

struct EXTENDEDVERTEX 
{
        FLOAT x, y, z;
        FLOAT nx, ny, nz;
        FLOAT u, v;
};

struct TANGENTSPACEVERTEX 
{
        FLOAT x, y, z;
        FLOAT u, v;
        FLOAT nx, ny, nz;
        FLOAT bx, by, bz;
        FLOAT tx, ty, tz;
};


#endif
