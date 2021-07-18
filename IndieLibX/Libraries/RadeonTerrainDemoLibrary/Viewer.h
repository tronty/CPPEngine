//-----------------------------------------------------------------------------
// File: Viewer.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _VIEWER_H
#define _VIEWER_H

#include "DXErrors.h"
#include "Helper.h"

// Frustum parameters
const float NEAR_CLIP = 150.0f;
const float FAR_CLIP = 250000.0f;
const float FOV = PI / 4.0f;

class CViewer
{
public:
    // Viewer position and orientation
    D3DXFROMWINEVECTOR3 m_vUp;
    D3DXFROMWINEVECTOR3 m_vDir;
    D3DXFROMWINEVECTOR3 m_vPos;
    D3DXFROMWINEVECTOR3 m_vCross;

    D3DXFROMWINEMATRIX m_matView;
    D3DXFROMWINEMATRIX m_matProj;
    D3DXFROMWINEMATRIX m_matRot;
    float m_fAspect;

    void CalculateTransform();

public:
    CViewer();
    void SetAspect(float aspect);
    void UpdateViewer(
        D3DXFROMWINEVECTOR3 &pos, D3DXFROMWINEQUATERNION &quat);
    D3DXFROMWINEVECTOR3& GetPos();

    void ComputeClipVolume(CLIPVOLUME& cv);
};

#endif
