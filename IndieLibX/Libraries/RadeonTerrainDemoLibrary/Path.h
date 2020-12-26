

//-----------------------------------------------------------------------------
// File: Path.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _PATH_H
#define _PATH_H

#include "DXErrors.h"

typedef struct _PATHPOINT
{
    float fTime;
    D3DXFROMWINEVECTOR3 vPosition;
    D3DXFROMWINEVECTOR3 vPositionD2;
    D3DXFROMWINEQUATERNION qOrientation;
    D3DXFROMWINEQUATERNION qOrientationD2;
} PATHPOINT;

class CPath
{
    PATHPOINT *m_pPoints;
    __DWORD__ m_dwCount;
    float m_fPathTime;

    void CalcOrientationD2(D3DXFROMWINEQUATERNION &qOrientationD2, D3DXFROMWINEQUATERNION &q0,
        D3DXFROMWINEQUATERNION &q1, D3DXFROMWINEQUATERNION &q2);
    void InitInterpolation();
    void InterpolatePostiontion();
    void InterpolateOrientation();

public:
    CPath();
    ~CPath();

    int Load(char *strPathName);
    void Play(float fTime, D3DXFROMWINEVECTOR3 &vPosition, D3DXFROMWINEQUATERNION &qOrientation);
    float GetPathTime();
};

#endif

