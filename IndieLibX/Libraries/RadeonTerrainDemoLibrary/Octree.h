

//-----------------------------------------------------------------------------
// File: Octree.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _OCTREE_H
#define _OCTREE_H

#include "DXErrors.h"
#include "TriangleList.h"
#include "RenderQueue.h"
#include "Helper.h"

class COctreeNode
{
    D3DXFROMWINEVECTOR3 m_vCenter;
    D3DXFROMWINEVECTOR3 m_vCorners[8];
    // Node dimentions
    float m_fWidth;
    float m_fHeight;
    float m_fDepth;
    // Number of strips
    __DWORD__ m_dwListCount;
    // Triangle lists
    CTriangleList *m_pTriangleLists;
    // Children of this node
    COctreeNode *m_pKids[8];

    void ReadTree(FILE *fp);

public:
    COctreeNode(FILE *fp);
    ~COctreeNode();
    void Cull(CRenderQueue *pRQ, CLIPVOLUME& cv, D3DXFROMWINEVECTOR3& pos);
};

#endif

