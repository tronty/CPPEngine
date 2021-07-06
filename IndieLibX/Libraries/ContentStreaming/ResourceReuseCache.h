//--------------------------------------------------------------------------------------
// File: ResourceReuseCache.h
//
// Illustrates streaming content 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef RESOURCE_REUSE_CACHE_H
#define RESOURCE_REUSE_CACHE_H
#ifndef _MSC_VER
typedef STX::uint64_t UINT64;//???
#define S_OK 0
inline bool FAILED(int a)
{
	return a!=S_OK;
}
FORMAT FORMAT3FromD3D(UINT a);
#endif
#include <FrameworkHLSLCg3/Platform.h>

#include "dds.h"

//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define E_TRYAGAIN MAKE_HRESULT(1,FACILITY_WIN32,123456)

//--------------------------------------------------------------------------------------
// structures
//--------------------------------------------------------------------------------------
/*
struct LOADER_DEVICE
{
LOADER_DEVICE() {}
};
*/
struct DEVICE_TEXTURE
{
UINT Width;
UINT Height;
UINT MipLevels;
UINT Format;
TextureID	  pTexture9;
UINT64 EstimatedSize;
BOOL bInUse;
UINT RecentUseCounter;
};

struct DEVICE_VERTEX_BUFFER
{
UINT iSizeBytes;
VertexBufferID  pVB9;
BOOL bInUse;
UINT RecentUseCounter;
};

struct DEVICE_INDEX_BUFFER
{
UINT iSizeBytes;
UINT ibFormat;
IndexBufferID	 pIB9;
BOOL bInUse;
UINT RecentUseCounter;
};
//struct RendererHLSLCg;
//--------------------------------------------------------------------------------------
// CResourceReuseCache class
//--------------------------------------------------------------------------------------
class IDataProcessor;
class CResourceReuseCache
{
private:
//LOADER_DEVICE							m_Device;
static VArray<DEVICE_TEXTURE*> 		m_TextureList;
static VArray<DEVICE_VERTEX_BUFFER*>	m_VBList;
static VArray<DEVICE_INDEX_BUFFER*>	m_IBList;
static UINT64									m_MaxManagedMemory;
static UINT64									m_UsedManagedMemory;
static BOOL									m_bSilent;
static BOOL									m_bDontCreateResources;

int FindTexture( TextureID pTex9 );
int EnsureFreeTexture( UINT Width, UINT Height, UINT MipLevels, UINT Format );
UINT64 GetEstimatedSize( UINT Width, UINT Height, UINT MipLevels, UINT Format );

int FindVB( VertexBufferID pVB );
int EnsureFreeVB( const IDataProcessor* p,UINT iSizeBytes );

int FindIB( IndexBufferID pIB );
int EnsureFreeIB( const IDataProcessor* p,UINT iSizeBytes, UINT ibFormat );

void DestroyTexture9( DEVICE_TEXTURE* pTex );
void DestroyTexture10( DEVICE_TEXTURE* pTex );
void DestroyVB9( DEVICE_VERTEX_BUFFER* pVB );
void DestroyVB10( DEVICE_VERTEX_BUFFER* pVB );
void DestroyIB9( DEVICE_INDEX_BUFFER* pVB );
void DestroyIB10( DEVICE_INDEX_BUFFER* pVB );

public:
CResourceReuseCache( IRenderer* r );
~CResourceReuseCache();

IRenderer* m_r;
// memory handling
void SetMaxManagedMemory( UINT64 MaxMemory );
UINT64 GetMaxManagedMemory();
UINT64 GetUsedManagedMemory();
void SetDontCreateResources( BOOL bDontCreateResources );
UINT64 DestroyLRUTexture();
UINT64 DestroyLRUVB();
UINT64 DestroyLRUIB();
void DestroyLRUResources( UINT64 SizeGainNeeded );

// texture functions
TextureID GetFreeTexture9( UINT Width, UINT Height, UINT MipLevels, UINT Format );
void UnuseDeviceTexture9( TextureID pTexture );
int GetNumTextures();
DEVICE_TEXTURE* GetTexture( int i );

// vertex buffer functions
VertexBufferID GetFreeVB9( const IDataProcessor* p,UINT sizeBytes );
void UnuseDeviceVB9( VertexBufferID pVB );
int GetNumVBs();
DEVICE_VERTEX_BUFFER* GetVB( int i );

// index buffer functions
IndexBufferID GetFreeIB9( const IDataProcessor* p,UINT sizeBytes, UINT ibFormat );
void UnuseDeviceIB9( IndexBufferID pVB );
int GetNumIBs();
DEVICE_INDEX_BUFFER* GetIB( int i );

void OnDestroy();

};

D3DFORMAT GetD3D9Format( DDS_PIXELFORMAT ddpf );
void GetSurfaceInfo( UINT width, UINT height, D3DFORMAT fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows );

#endif
