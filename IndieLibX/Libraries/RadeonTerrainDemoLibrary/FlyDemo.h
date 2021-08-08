

//-----------------------------------------------------------------------------
// File: FlyDemo.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _FLYDEMO_H
#define _FLYDEMO_H

#include "DXErrors.h"
#include "Demo.h"
#include "Skydome.h"
#include "Terrain.h"
#include "Path.h"

class CFlyDemo : public CDemo
{
    CSkyDome *m_pSky;
    CTerrain *m_pTerrain;
    CPath *m_pPath;

public:
    CFlyDemo();
    ~CFlyDemo();
    int Load();
    int Init();
    void Destroy();
    int Render(float fTime, bool bTerrain, bool bSky, bool bWireframe );
    __DWORD__ CountPolys(bool bTerrain, bool bSky);
    float GetFlightTime();
};

#endif

