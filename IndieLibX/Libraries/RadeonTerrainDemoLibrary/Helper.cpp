

//-----------------------------------------------------------------------------
// File: Helper.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "DXErrors.h"
#include "Helper.h"

// D3DXFROMWINEMatrixLookAtLH is broken, so use our own routine
void ComputeViewMatrix(D3DXFROMWINEMATRIX *matView, D3DXFROMWINEVECTOR3 *vEye, D3DXFROMWINEVECTOR3 *vDir, D3DXFROMWINEVECTOR3 *vUp)
{
    *vUp = *vUp - (D3DXFROMWINEVec3Dot(vDir, vUp) * *vDir);
    D3DXFROMWINEVec3Normalize(vUp, vUp);
    D3DXFROMWINEVECTOR3 vCross;
    D3DXFROMWINEVec3Cross(&vCross, vUp, vDir);

    D3DXFROMWINEMatrixIdentity(matView);
    matView->m[0][0] = vCross.x;  matView->m[0][1] = vUp->x;  matView->m[0][2] = vDir->x;
    matView->m[1][0] = vCross.y;  matView->m[1][1] = vUp->y;  matView->m[1][2] = vDir->y;
    matView->m[2][0] = vCross.z;  matView->m[2][1] = vUp->z;  matView->m[2][2] = vDir->z;

    matView->m[3][0] = -D3DXFROMWINEVec3Dot(vEye, &vCross);
    matView->m[3][1] = -D3DXFROMWINEVec3Dot(vEye, vUp);
    matView->m[3][2] = -D3DXFROMWINEVec3Dot(vEye, vDir);
}

