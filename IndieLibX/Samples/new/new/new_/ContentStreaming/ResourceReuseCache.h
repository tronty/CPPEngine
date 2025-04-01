/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
//--------------------------------------------------------------------------------------
// File: ResourceReuseCache.h
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef RESOURCE_REUSE_CACHE_H
#define RESOURCE_REUSE_CACHE_H

#include <Framework3/IRenderer.h>
#define CGrowableArray std::vector

//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define E_TRYAGAIN MAKE_HRESULT(1,FACILITY_WIN32,123456)

enum LOADER_DEVICE_TYPE
{
    LDT_D3D9 = 0x1
};

//--------------------------------------------------------------------------------------
// structures
//--------------------------------------------------------------------------------------
struct LOADER_DEVICE
{
LOADER_DEVICE_TYPE Type;
LOADER_DEVICE() { Type = LDT_D3D9; }
};

struct DEVICE_TEXTURE
{
unsigned int Width;
unsigned int Height;
unsigned int MipLevels;
unsigned int Format;
union
{
TextureID        pTexture9;
uint64                    Align64bit;

};

uint64 EstimatedSize;
BOOL bInUse;
unsigned int RecentUseCounter;
};

struct DEVICE_VERTEX_BUFFER
{
unsigned int iSizeBytes;

union
{
VertexBufferID  pVB9;
uint64                   Align64bit;
};

BOOL bInUse;
unsigned int RecentUseCounter;
};

struct DEVICE_INDEX_BUFFER
{
unsigned int iSizeBytes;
unsigned int ibFormat;

union
{
IndexBufferID   pIB9;
uint64                   Align64bit;
};

BOOL bInUse;
unsigned int RecentUseCounter;
};

//--------------------------------------------------------------------------------------
// CResourceReuseCache class
//--------------------------------------------------------------------------------------
class CResourceReuseCache
{
private:
LOADER_DEVICE							m_Device;
CGrowableArray<DEVICE_TEXTURE*>			m_TextureList;
CGrowableArray<DEVICE_VERTEX_BUFFER*>	m_VBList;
CGrowableArray<DEVICE_INDEX_BUFFER*>	m_IBList;
uint64									m_MaxManagedMemory;
uint64									m_UsedManagedMemory;
BOOL									m_bSilent;
BOOL									m_bDontCreateResources;

int FindTexture( TextureID pTex9 );
int EnsureFreeTexture( unsigned int Width, unsigned int Height, unsigned int MipLevels, unsigned int Format );
uint64 GetEstimatedSize( unsigned int Width, unsigned int Height, unsigned int MipLevels, unsigned int Format );

int FindVB( VertexBufferID pVB );
int EnsureFreeVB( unsigned int iSizeBytes );

int FindIB( IndexBufferID pIB );
int EnsureFreeIB( unsigned int iSizeBytes, unsigned int ibFormat );

void DestroyTexture9( DEVICE_TEXTURE* pTex );
void DestroyVB9( DEVICE_VERTEX_BUFFER* pVB );
void DestroyIB9( DEVICE_INDEX_BUFFER* pVB );

public:
CResourceReuseCache();
~CResourceReuseCache();

// memory handling
void SetMaxManagedMemory( uint64 MaxMemory );
uint64 GetMaxManagedMemory();
uint64 GetUsedManagedMemory();
void SetDontCreateResources( BOOL bDontCreateResources );
uint64 DestroyLRUTexture();
uint64 DestroyLRUVB();
uint64 DestroyLRUIB();
void DestroyLRUResources( uint64 SizeGainNeeded );

// texture functions
TextureID GetFreeTexture9( unsigned int Width, unsigned int Height, unsigned int MipLevels, unsigned int Format );
void UnuseDeviceTexture9( TextureID pTexture );
int GetNumTextures();
DEVICE_TEXTURE* GetTexture( int i );

// vertex buffer functions
VertexBufferID GetFreeVB9( unsigned int sizeBytes );
void UnuseDeviceVB9( VertexBufferID pVB );
int GetNumVBs();
DEVICE_VERTEX_BUFFER* GetVB( int i );

// index buffer functions
IndexBufferID GetFreeIB9( unsigned int sizeBytes, unsigned int ibFormat );
void UnuseDeviceIB9( IndexBufferID pVB );
int GetNumIBs();
DEVICE_INDEX_BUFFER* GetIB( int i );

void OnDestroy();

};

#define DDS_PIXELFORMAT FORMAT
FORMAT GetD3D9Format( DDS_PIXELFORMAT ddpf );
void GetSurfaceInfo( unsigned int width, unsigned int height, FORMAT fmt, unsigned int* pNumBytes, unsigned int* pRowBytes, unsigned int* pNumRows );

#endif
