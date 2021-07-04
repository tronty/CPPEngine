/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//--------------------------------------------------------------------------------------
// File: Grid_Creation11.h
//
// Header file for Grid_Creation11.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#ifndef GRID_CREATION11_H
#define GRID_CREATION11_H

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

void FillGrid_NonIndexed( __DWORD__ dwWidth, __DWORD__ dwLength, 
                         float fGridSizeX, float fGridSizeZ, 
                         __DWORD__ uDummyStartVertices, __DWORD__ uDummyEndVertices,
                         VertexBufferID  lplpVB);

void FillGrid_Indexed( __DWORD__ dwWidth, __DWORD__ dwLength, 
                      float fGridSizeX, float fGridSizeZ, 
                      VertexBufferID  lplpVB, VertexBufferID  lplpIB);

void FillGrid_WithNormals_Indexed( __DWORD__ dwWidth, __DWORD__ dwLength, 
                                  float fGridSizeX, float fGridSizeZ, 
                                  VertexBufferID  lplpVB, VertexBufferID  lplpIB);

void FillGrid_Quads_Indexed( __DWORD__ dwWidth, __DWORD__ dwLength, 
                            float fGridSizeX, float fGridSizeZ, 
                            VertexBufferID  lplpVB, VertexBufferID  lplpIB);

void FillGrid_Quads_NonIndexed( __DWORD__ dwWidth, __DWORD__ dwLength, 
                               float fGridSizeX, float fGridSizeZ, 
                               VertexBufferID  lplpVB);

void FillGrid_Indexed_WithTangentSpace( __DWORD__ dwWidth, __DWORD__ dwLength, 
                                       float fGridSizeX, float fGridSizeZ, 
                                       VertexBufferID  lplpVB, VertexBufferID  lplpIB);

#endif
