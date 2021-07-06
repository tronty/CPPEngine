

//-----------------------------------------------------------------------------
// File: Material.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "DXErrors.h"
#include "Texture.h"
#include "TManager.h"

class CMaterial
{

public:
    D3DFROMWINEMATERIAL9 m_Mtrl;
    TextureID m_pTextureID; // -1 if not loaded
    int m_dwTexIndx; // -1 if not loaded
    CMaterial();
    ~CMaterial();
    int LoadMaterial(FILE *fp);
    TextureID LoadMaterial(D3DFROMWINEMATERIAL9 &mtrl, char *tname, bool mipmap, bool alpha);
    TextureID SetMaterial();
    void DeleteTexture();
};

#endif

