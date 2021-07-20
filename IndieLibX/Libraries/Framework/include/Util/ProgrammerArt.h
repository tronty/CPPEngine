/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
	//-----------------------------------------------------------------------------
// File: ProgrammerArt.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#include "Model.h"

#ifndef _PROGRAMMERART_
#define _PROGRAMMERART_

#define TEXCOORDS 0x1
#define NORMALS   0x2

Model *createCube(const float sizeX, const float sizeY, const float sizeZ, const uint flags = 0);
Model *createSphere(const int subDivLevel, const float radius = 1);

#define CENTER_SAMPLE  0x1
#define TWO_COMPONENTS 0x2
#define XY_PLANE       0x0 /* Default */
#define XZ_PLANE       0x4
#define YZ_PLANE       0x8

typedef float (*ZFunc)(float x, float y);
Model *createDisc(const int nSlices, const int nRings = 1, const float radius = 1, uint flags = 0, ZFunc zFunc = NULL);

inline int getRectangleIndexCount(const int width, const int height){
	return (height - 1) * (2 * width + 1);
}

template <typename INT_TYPE>
void fillRectangleIB(INT_TYPE *dest, const int width, const int height);

#endif // _PROGRAMMERART_
