//--------------------------------------------------------------------------------------
// File: PackedFile.h
//
// Illustrates streaming content 
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
    UINT64 FileSize;
    UINT64 NumFiles;
    UINT64 NumChunks;
    UINT64 Granularity;
    UINT MaxChunksInVA;

    UINT64 TileBytesSize;
    float TileSideSize;
    float LoadingRadius;
    UINT64 VideoMemoryUsageAtFullMips;
};

struct CHUNK_HEADER
{
    UINT64 ChunkOffset;
    UINT64 ChunkSize;
};

struct FILE_INDEX
{
    CHAR szFileName[MAX_PATH];
    UINT64 FileSize;
    UINT64 ChunkIndex;
    UINT64 OffsetIntoChunk;
    float3 vCenter;
};

struct LEVEL_ITEM
{
    float3 vCenter;
    DEVICE_VERTEX_BUFFER VB;
    DEVICE_INDEX_BUFFER IB;
    DEVICE_TEXTURE Diffuse;
    DEVICE_TEXTURE Normal;
    CHAR   szVBName[MAX_PATH];
    CHAR   szIBName[MAX_PATH];
    CHAR   szDiffuseName[MAX_PATH];
    CHAR   szNormalName[MAX_PATH];
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
    UINT UseCounter;
    bool bInUse;
};

struct BOX_VERTEX
{
    float3 pos;
    float3 norm;
    float3 tan;
    float2 tex;
};
struct STRING
{
    CHAR str[MAX_PATH];
};

//--------------------------------------------------------------------------------------
// Align the input offset to the specified granularity (see CreatePackedFile below)
//--------------------------------------------------------------------------------------
inline UINT64 AlignToGranularity( UINT64 Offset, UINT64 Granularity )
{
    UINT64 floor = Offset / Granularity;
    return ( floor + 1 ) * Granularity;
}
//--------------------------------------------------------------------------------------
// Write bytes into the file until the granularity is reached (see CreatePackedFile below)
//--------------------------------------------------------------------------------------
inline UINT64 FillToGranularity( HANDLE hFile, UINT64 CurrentOffset, UINT64 Granularity )
{
    UINT64 NewOffset = AlignToGranularity( CurrentOffset, Granularity );
    UINT64 NumBytes = NewOffset - CurrentOffset;

    DWORD dwWritten;
    BYTE Zero = 0;
    for( UINT64 i = 0; i < NumBytes; i++ )
    {
        if( !WriteFile( hFile, &Zero, sizeof( BYTE ), &dwWritten, NULL ) )
            return 0;
    }

    return NewOffset;
}
//--------------------------------------------------------------------------------------
inline UINT64 GetSize( char* szFile )
{
    UINT64 Size = 0;

    HANDLE hFile = CreateFileA( szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
                                NULL );
    if( INVALID_HANDLE_VALUE != hFile )
    {
        LARGE_INTEGER FileSize;
        GetFileSizeEx( hFile, &FileSize );
        Size = FileSize.QuadPart;
        CloseHandle( hFile );
    }

    return Size;
}

//--------------------------------------------------------------------------------------
// CPackedFile class
//--------------------------------------------------------------------------------------
class CPackedFile
{
public:
    PACKED_FILE_HEADER m_FileHeader;
    FILE_INDEX* m_pFileIndices;
    CHUNK_HEADER* m_pChunks;
    MAPPED_CHUNK* m_pMappedChunks;

    HANDLE m_hFile;
    HANDLE m_hFileMapping;
    UINT m_ChunksMapped;
    UINT m_MaxChunksMapped;
    UINT m_CurrentUseCounter;

public:
            CPackedFile();
            ~CPackedFile();

    bool    CreatePackedFile(  CHAR* szFileName, UINT SqrtNumTiles,
                              UINT SidesPerTile, float fWorldScale, float fHeightScale );
    bool    LoadPackedFile( CHAR* szFileName, bool b64Bit, VArray <LEVEL_ITEM*>* pLevelItemArray );
    void    UnloadPackedFile();
    void    EnsureChunkMapped( UINT64 iChunk );
    bool    GetPackedFileInfo( char* szFile, UINT* pDataBytes );

//	bool    GetPackedFileInfo( CHAR* szFile, UINT* pDataBytes );
    bool    GetPackedFile( char* szFile, BYTE** ppData, UINT* pDataBytes );
//    bool    GetPackedFile( CHAR* szFile, BYTE** ppData, UINT* pDataBytes );
    bool    UsingMemoryMappedIO();

    void    SetMaxChunksMapped( UINT maxmapped );
    UINT64  GetTileBytesSize();
    float   GetTileSideSize();
    float   GetLoadingRadius();
    UINT    GetMaxChunksMapped();
    UINT    GetMaxChunksInVA();
    UINT64  GetNumChunks();
    UINT64  GetVideoMemoryUsageAtFullMips();
};

#endif
