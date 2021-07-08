//-----------------------------------------------------------------------------
// File: Texture.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "DXErrors.h"
//include <tchar.h>

#define TEXTURE_PATH "/RadeonTerrainDemo/"

class CTexture
{
    bool m_bHasAlpha;
    bool m_bMipMap;
    TextureID m_pTextureID;
    char m_strFileName[MAX_PATH];

public:
    CTexture();
    ~CTexture();
    TextureID Create(char *strTexName, bool bMipMap, bool bAlpha);
    int Restore();
    void Invalidate();
    TextureID GetSurface();
};

#endif
