/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//--------------------------------------------------------------------------------------
// File: PackedFile.h
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef PACKD_FILE_H
#define PACKD_FILE_H

#include "ResourceReuseCache.h"

//--------------------------------------------------------------------------------------
// Packed file structures
//--------------------------------------------------------------------------------------
struct PACKED_FILE_HEADER
{
    uint64 FileSize;
    uint64 NumFiles;
    uint64 NumChunks;
    uint64 Granularity;
    unsigned int MaxChunksInVA;

    uint64 TileBytesSize;
    float TileSideSize;
    float LoadingRadius;
    uint64 VideoMemoryUsageAtFullMips;
};

struct CHUNK_HEADER
{
    uint64 ChunkOffset;
    uint64 ChunkSize;
};

struct FILE_INDEX
{
    std::string szFileName;
    uint64 FileSize;
    uint64 ChunkIndex;
    uint64 OffsetIntoChunk;
    D3DXFROMWINEVECTOR3 vCenter;
};

struct LEVEL_ITEM
{
    D3DXFROMWINEVECTOR3 vCenter;
    DEVICE_VERTEX_BUFFER VB;
    DEVICE_INDEX_BUFFER IB;
    DEVICE_TEXTURE Diffuse;
    DEVICE_TEXTURE Normal;
    std::string   szVBName;
    std::string   szIBName;
    std::string   szDiffuseName;
    std::string   szNormalName;
    bool bLoaded;
    bool bLoading;
    bool bInLoadRadius;
    bool bInFrustum;
    int CurrentCountdownDiff;
    int CurrentCountdownNorm;
    bool bHasBeenRenderedDiffuse;
    bool bHasBeenRenderedNormal;
};

struct MAPPED_CHUNK
{
    void* pMappingPointer;
    unsigned int UseCounter;
    bool bInUse;
};

struct BOX_VERTEX
{
    D3DXFROMWINEVECTOR3 pos;
    D3DXFROMWINEVECTOR3 norm;
    D3DXFROMWINEVECTOR3 tan;
    D3DXFROMWINEVECTOR2 tex;
};

//--------------------------------------------------------------------------------------
// CPackedFile class
//--------------------------------------------------------------------------------------
class CPackedFile
{
private:
    PACKED_FILE_HEADER m_FileHeader;
    FILE_INDEX* m_pFileIndices;
    CHUNK_HEADER* m_pChunks;
    MAPPED_CHUNK* m_pMappedChunks;

    void* m_hFile;
    void* m_hFileMapping;
    unsigned int m_ChunksMapped;
    unsigned int m_MaxChunksMapped;
    unsigned int m_CurrentUseCounter;

public:
            CPackedFile();
            ~CPackedFile();

    bool    CreatePackedFile( std::string& szFileName, unsigned int SqrtNumTiles,
                              unsigned int SidesPerTile, float fWorldScale, float fHeightScale );
    bool    LoadPackedFile( std::string& szFileName, bool b64Bit, CGrowableArray <LEVEL_ITEM*>* pLevelItemArray );
    void    UnloadPackedFile();
    void    EnsureChunkMapped( uint64 iChunk );
    bool    GetPackedFileInfo( std::string& szFile, unsigned int* pDataBytes );
    bool    GetPackedFile( std::string& szFile, BYTE** ppData, unsigned int* pDataBytes );
    bool    UsingMemoryMappedIO();

    void    SetMaxChunksMapped( unsigned int maxmapped );
    uint64  GetTileBytesSize();
    float   GetTileSideSize();
    float   GetLoadingRadius();
    unsigned int    GetMaxChunksMapped();
    unsigned int    GetMaxChunksInVA();
    uint64  GetNumChunks();
    uint64  GetVideoMemoryUsageAtFullMips();
};

#endif
