//-----------------------------------------------------------------------------
// File: TManager.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _TMANAGER_H
#define _TMANAGER_H

#include "DXErrors.h"
#include "Texture.h"

#define MAX_TEXTURES 50

class CTextureManager
{
    CTexture *m_Textures[MAX_TEXTURES];

public:
    CTextureManager();
    ~CTextureManager();
    CTexture *GetTexture(__DWORD__ n);
    __DWORD__ AddTexture(CTexture *texture);
    int DeleteTexture(__DWORD__ n);
    int RestoreAll();
    void InvalidateAll();
};

#endif
