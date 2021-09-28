//--------------------------------------------------------------------------------------
// File: SDKMisc.h
//
// Various helper functionality that is shared between SDK samples
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef SDKMISC_H
#define SDKMISC_H


//-----------------------------------------------------------------------------
// Resource cache for textures, fonts, meshs, and effects.  
// Use DXUTGetGlobalResourceCache() to access the global cache
//-----------------------------------------------------------------------------

enum DXUTCACHE_SOURCELOCATION
{
    DXUTCACHE_LOCATION_FILE,
    DXUTCACHE_LOCATION_RESOURCE
};

struct DXUTCache_Texture
{
    DXUTCACHE_SOURCELOCATION Location;
    char*   wszSource[MAX_PATH];
    
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT MipLevels;
    UINT MiscFlags;
        DWORD Usage9;
        FORMAT Format9;
        D3DPOOL Pool9;
        UINT BindFlags;
    TextureID pTexture9;

            DXUTCache_Texture()
            {
                pTexture9 = NULL;
                pSRV10 = NULL;
            }
};

struct DXUTCache_Effect
{
    DXUTCACHE_SOURCELOCATION Location;
    char* wszSource[MAX_PATH];
    
    DWORD dwFlags;
    ShaderID pEffect;
};

//--------------------------------------------------------------------------------------
// Tries to finds a media file by searching in common locations
//--------------------------------------------------------------------------------------
HRESULT DXUTFindDXSDKMediaFileCch( char** strDestPath,
                                   int cchDest, 
                                   const char* strFilename );
HRESULT DXUTSetMediaSearchPath( const char* strPath );
const char* DXUTGetMediaSearchPath();


//--------------------------------------------------------------------------------------
// Returns a view matrix for rendering to a face of a cubemap.
//--------------------------------------------------------------------------------------
D3DXFROMWINEMATRIX DXUTGetCubeMapViewMatrix( DWORD dwFace );

#endif

