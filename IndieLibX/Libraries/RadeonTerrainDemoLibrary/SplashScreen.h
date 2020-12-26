

//-----------------------------------------------------------------------------
// File: SplashScreen.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _SPLASHSCREEN_H
#define _SPLASHSCREEN_H

#include "DXErrors.h"
#include "Demo.h"

class CSplashScreen : public CDemo
{
private:
    bool m_bFinished;
    D3DVERTEX2 m_vMesh[4];
    VertexBufferID m_vMeshVB;
    __DWORD__ m_dwTexIndx; // -1 if not loaded
	TextureID m_pTextureID; // -1 if not loaded

    void DeleteTexture();

public:
    CSplashScreen();
    ~CSplashScreen();
    int Load();
    int Init();
    void Destroy();
    int Render(float fTime );
};

#endif

