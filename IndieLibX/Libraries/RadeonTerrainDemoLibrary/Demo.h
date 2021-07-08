

//-----------------------------------------------------------------------------
// File: Demo.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _DEMO_H
#define _DEMO_H

#include "DXErrors.h"

class CDemo
{
public:
    bool m_bLoaded;
    bool m_bInitialized;
    bool m_bWireframe;

    CDemo() { m_bWireframe = false; };
    virtual int Load() { return 1; };
    virtual int Init() { return 1; };
    virtual void Destroy() {};
    virtual int Render(float fTime ) { return 1; };
    void ToggleWireframe() { m_bWireframe = !m_bWireframe; };
};

#endif

