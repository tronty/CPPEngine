

//-----------------------------------------------------------------------------
// File: Model.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _MODEL_H
#define _MODEL_H

#include "DXErrors.h"

class CModel
{
public:
    CModel() {};
    virtual int Load() { return 1; };
    virtual int Init() { return 1; };
    virtual void Destroy() {};
    virtual int Render() { return 1; };
    virtual __DWORD__ CountPolys() { return 0; };
};

#endif

