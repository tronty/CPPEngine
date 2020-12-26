/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Math\Frustum.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

/** \file
    View frustum definition
*/

#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>

#define FRUSTUM_LEFT   0
#define FRUSTUM_RIGHT  1
#define FRUSTUM_TOP    2
#define FRUSTUM_BOTTOM 3
#define FRUSTUM_FAR    4
#define FRUSTUM_NEAR   5

/** Standard mathematical plane definition */
struct Plane {
	Plane(){};
	Plane(float x, float y, float z, float w){
        normal = D3DXFROMWINEVECTOR3(x, y, z);
		float invLen = 1.0f / D3DXFROMWINEVec3Length(&normal);
		normal *= invLen;
		offset = w * invLen;
	}
    D3DXFROMWINEVECTOR3 normal;
	float offset;
};

/** 6-planes view frustum class */
class Frustum {
public:
	void loadFrustum(const D3DXFROMWINEMATRIX &mvp);
	const Plane &getPlane(const int index) const { return planes[index]; }

	bool isPointInFrustum(const D3DXFROMWINEVECTOR3 &pos) const;
	bool isSphereInFrustum(const D3DXFROMWINEVECTOR3 &pos, const float radius) const;
protected:
	Plane planes[6];
};
#endif // _FRUSTUM_H_

