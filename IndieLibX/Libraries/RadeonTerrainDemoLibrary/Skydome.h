

//-----------------------------------------------------------------------------
// File: Skydome.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _SKYDOME_H
#define _SKYDOME_H

#include "DXErrors.h"
#include "Model.h"
#include "Material.h"
#include "LitVertex.h"

class CSkyDome : public CModel
{
    bool m_bLoaded;
    __DWORD__ m_mSkyInd;
    __DWORD__ m_mGroundInd;
    TextureID m_mSkyID, m_mGroundID;
    // Sky mesh
    LPLITVERTEX2 m_pSkyVert;
    __DWORD__ m_dwSkyNumVert;
    VertexBufferID m_pSkyVB;
    // Ground mesh
    LPLITVERTEX2 m_pGroundVert;
    __DWORD__ m_dwGroundNumVert;
    VertexBufferID m_pGroundVB;
	ShaderID dirtshd;
	VertexFormatID dirtvf;

public:
    CSkyDome();
    ~CSkyDome();
    int Load();
    int Init();
    void Destroy();
    int Render();
	int Render_(TextureID dirtTexture=-1, float height=0.0f);
    __DWORD__ CountPolys();
};

#endif

