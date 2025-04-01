/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Math\Frustum.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <Math/Frustum.h>

void Frustum::loadFrustum(const D3DXFROMWINEMATRIX &mvp){
	planes[FRUSTUM_LEFT  ] = Plane(mvp.m2[12] - mvp.m2[0], mvp.m2[13] - mvp.m2[1], mvp.m2[14] - mvp.m2[2],  mvp.m2[15] - mvp.m2[3]);
	planes[FRUSTUM_RIGHT ] = Plane(mvp.m2[12] + mvp.m2[0], mvp.m2[13] + mvp.m2[1], mvp.m2[14] + mvp.m2[2],  mvp.m2[15] + mvp.m2[3]);

	planes[FRUSTUM_TOP   ] = Plane(mvp.m2[12] - mvp.m2[4], mvp.m2[13] - mvp.m2[5], mvp.m2[14] - mvp.m2[6],  mvp.m2[15] - mvp.m2[7]);
	planes[FRUSTUM_BOTTOM] = Plane(mvp.m2[12] + mvp.m2[4], mvp.m2[13] + mvp.m2[5], mvp.m2[14] + mvp.m2[6],  mvp.m2[15] + mvp.m2[7]);

	planes[FRUSTUM_FAR   ] = Plane(mvp.m2[12] - mvp.m2[8], mvp.m2[13] - mvp.m2[9], mvp.m2[14] - mvp.m2[10], mvp.m2[15] - mvp.m2[11]);
	planes[FRUSTUM_NEAR  ] = Plane(mvp.m2[12] + mvp.m2[8], mvp.m2[13] + mvp.m2[9], mvp.m2[14] + mvp.m2[10], mvp.m2[15] + mvp.m2[11]);
}

bool Frustum::isPointInFrustum(const D3DXFROMWINEVECTOR3 &pos) const {
    for (unsigned int i = 0; i < 6; i++){
		if (D3DXFROMWINEVec3Dot(&pos, &planes[i].normal) + planes[i].offset <= 0) return false;
    }
    return true;
}

bool Frustum::isSphereInFrustum(const D3DXFROMWINEVECTOR3 &pos, const float radius) const {
    for (unsigned int i = 0; i < 6; i++){
		if (D3DXFROMWINEVec3Dot(&pos, &planes[i].normal) + planes[i].offset <= -radius) return false;
    }
    return true;
}
