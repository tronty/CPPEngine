

//-----------------------------------------------------------------------------
// File: Terrain.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _TERRAIN_H
#define _TERRAIN_H

#include "DXErrors.h"
#include "Model.h"
#include "TriangleList.h"
#include "RenderQueue.h"
#include "Octree.h"
#include "VertexStore.h"

class CTerrain : public CModel
{
    bool m_bLoaded;
    // Terrain octree
    COctreeNode* m_pOT;
    // Render queue
    CRenderQueue* m_pRQ;
    // Vertex storage
    CVertexStore* m_pVS;

public:
    bool m_bDetailTexture;

    CTerrain();
    ~CTerrain();
    int Load();
    int Init();
    void Destroy();
    int Render();
    __DWORD__ CountPolys();
};

#endif

