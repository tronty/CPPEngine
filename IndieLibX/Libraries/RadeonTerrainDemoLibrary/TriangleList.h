//-----------------------------------------------------------------------------
// File: TriangleList.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _TRIANGLELIST_H
#define _TRIANGLELIST_H

#include "DXErrors.h"
#include "LitVertex.h"
#include "VertexStore.h"

class CTriangleList
{
    void DeleteArrays();

public:
    __DWORD__ m_dwMaterialID;
    TextureID m_pTexID;
    IndexBufferID m_pIndicesIB;
    __WORD__ *m_pIndices;
    __DWORD__ m_dwIndNum;
    __DWORD__ m_dwVBID;

    CTriangleList();
    CTriangleList(FILE *fp);
    ~CTriangleList();
    int Load(FILE *fp);
    int Render( CVertexStore *pVS, int tex);
};

#endif
