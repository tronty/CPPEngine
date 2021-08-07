

//-----------------------------------------------------------------------------
// File: Helper.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _HELPER_H
#define _HELPER_H

#include "DXErrors.h"

#define PI 3.1415926535897932384626433832795f

#if defined(SAFE_DELETE)
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#undef SAFE_RELEASE
#endif
#define SAFE_DELETE(p)
#define SAFE_DELETE_ARRAY(p)
#define SAFE_RELEASE(p)


typedef struct _CLIPVOLUME
{
    D3DXFROMWINEPLANE pLeft, pRight;
    D3DXFROMWINEPLANE pTop, pBottom;
    D3DXFROMWINEPLANE pNear, pFar;
} CLIPVOLUME;

void ComputeViewMatrix(D3DXFROMWINEMATRIX *matView, D3DXFROMWINEVECTOR3 *vEye, D3DXFROMWINEVECTOR3 *vDir, D3DXFROMWINEVECTOR3 *vUp);

#endif

