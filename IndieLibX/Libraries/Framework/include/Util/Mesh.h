/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
	//-----------------------------------------------------------------------------
// File: Framework\Util\Mesh.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _MESH_H_
#define _MESH_H_

#include <Framework3/IRenderer.h>
#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>
#include "Transform.h"

enum {
	PL_TRIANGLE_LIST = 0,
	PL_TRIANGLE_STRIP,
	PL_TRIANGLE_FAN,
};

struct Primitive {
	int	Type;
	__DWORD__ numIndices;
	__DWORD__ *pIndices;
};

class Mesh : public Transform {
protected:
	int materialIndex;

	__DWORD__ baseVertex, numVertices;
	__DWORD__ baseTriangle, numTriangles;

	__DWORD__ numBones;
	__DWORD__ *pBone;

	__DWORD__ numPrimitives;
	Primitive* pPrimitive;
	
public:
	Mesh();
	~Mesh();

	inline const int getMaterialIndex() {return materialIndex; }; 
	inline const __DWORD__ getNumPrimitives() {return numPrimitives;};
	inline const Primitive& getPrimitive(int i) {return pPrimitive[i];};
	inline const __DWORD__ getNumVertices() {return numVertices;};
	inline const __DWORD__ getBaseVertex() {return baseVertex;};
	inline const __DWORD__ getNumTriangles() {return numTriangles;};
	inline const __DWORD__ getBaseTriangle() {return baseTriangle;};
friend class ATIModel;
};

#endif // _MESH_H_
