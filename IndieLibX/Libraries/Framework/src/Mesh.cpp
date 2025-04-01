/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\Mesh.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <Util/Mesh.h>

Mesh::Mesh(){
	materialIndex = -1;	
	baseVertex =  numVertices = 0;
	baseTriangle = numTriangles = 0;
	pPrimitive = NULL;
	pBone = NULL;
}

Mesh::~Mesh(){
	for (uint i = 0; i<numPrimitives; i++)
		delete [] pPrimitive[i].pIndices;

	delete [] pPrimitive;
	delete [] pBone;
}
