/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//--------------------------------------------------------------------------------------
// File: PackedFile.cpp
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "PackedFile.h"
#include "SDKMisc.h"
#include "Terrain.h"

//--------------------------------------------------------------------------------------
CPackedFile::CPackedFile() : m_pChunks( 0 ),
                             m_pMappedChunks( 0 ),
                             m_pFileIndices( 0 ),
                             m_hFileMapping( 0 ),
                             m_hFile( 0 ),
                             m_MaxChunksMapped( 78 )
{
    ZeroMemory( &m_FileHeader, sizeof( PACKED_FILE_HEADER ) );
}

//--------------------------------------------------------------------------------------
CPackedFile::~CPackedFile()
{
    UnloadPackedFile();
}

//--------------------------------------------------------------------------------------
uint64 GetSize( char* szFile )
{
    uint64 Size = 0;

    void* hFile = CreateFileA( szFile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
                                0 );
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
uint64 GetSize( char* szFile )
{
    uint64 Size = 0;

    void* hFile = CreateFile( szFile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
                               0 );
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
// Align the input offset to the specified granularity (see CreatePackedFile below)
//--------------------------------------------------------------------------------------
uint64 AlignToGranularity( uint64 Offset, uint64 Granularity )
{
    uint64 floor = Offset / Granularity;
    return ( floor + 1 ) * Granularity;
}

//--------------------------------------------------------------------------------------
// Write bytes into the file until the granularity is reached (see CreatePackedFile below)
//--------------------------------------------------------------------------------------
uint64 FillToGranularity( void* hFile, uint64 CurrentOffset, uint64 Granularity )
{
    uint64 NewOffset = AlignToGranularity( CurrentOffset, Granularity );
    uint64 NumBytes = NewOffset - CurrentOffset;

    __DWORD__ dwWritten;
    BYTE Zero = 0;
    for( uint64 i = 0; i < NumBytes; i++ )
    {
        if( !WriteFile( hFile, &Zero, sizeof( BYTE ), &dwWritten, 0 ) )
            return 0;
    }

    return NewOffset;
}

//--------------------------------------------------------------------------------------
// Creates a packed file.  The file is a flat uncompressed file containing all resources
// needed for the sample.  The file consists of chunks of data.  Each chunk represents
// a mappable window that can be accessed by MapViewOfFile.  Since MapViewOfFile can
// only map a view onto a file in 64k granularities, each chunk must start on a 64k
// boundary.  The packed file also creates an index.  This index is loaded into memory
// at startup and is not memory mapped.  The index is used to find the locations of 
// resource files within the packed file.
//--------------------------------------------------------------------------------------
struct STRING
{
    char str[MAX_PATH];
};
bool CPackedFile::CreatePackedFile( IDirect3DDevice9* pDev9, char* szFileName,
                                    unsigned int SqrtNumTiles, unsigned int SidesPerTile, float fWorldScale, float fHeightScale )
{
    bool bRet = false;
    void* hFile;

    m_FileHeader.NumFiles = 4 * SqrtNumTiles * SqrtNumTiles;

    CGrowableArray <FILE_INDEX*> TempFileIndices;
    CGrowableArray <CHUNK_HEADER*> TempHeaderList;
    CGrowableArray <STRING> FullFilePath;

    STRING strDiffuseTexture;
    STRING strNormalTexture;
	strDiffuseTexture.str="contentstreaming\\2kPanels_Diff.dds";
	strNormalTexture.str="contentstreaming\\2kPanels_Norm.dds";

    uint64 SizeDiffuse = GetSize( strDiffuseTexture.str );
    uint64 SizeNormal = GetSize( strNormalTexture.str );

    CTerrain Terrain;
    int hr = Terrain.LoadTerrain( "contentstreaming\\terrain1.bmp", SqrtNumTiles, SidesPerTile, fWorldScale, fHeightScale,
                                      true );
    if( FAILED( hr ) )
        return false;

    TERRAIN_TILE* pTile = Terrain.GetTile( 0 );
    uint64 SizeTerrainVB = pTile->NumVertices * sizeof( TERRAIN_VERTEX );
    uint64 SizeTerrainIB = Terrain.GetNumIndices() * sizeof( __WORD__ );

    float fTileWidth = pTile->BBox.max.x - pTile->BBox.min.x;
    float fChunkSpan = sqrtf( ( float )m_MaxChunksMapped ) - 1;
    uint64 TotalTerrainTileSize = SizeTerrainVB + SizeTerrainIB + SizeDiffuse + SizeNormal;
    uint64 LastChunkMegs = 0;
    uint64 ChunkMegs = TotalTerrainTileSize;
    unsigned int iSqrt = 1;
    uint64 SizeTry = 512;
    uint64 VideoMemoryUsage = 0;
    uint64 PrevVideoMemoryUsage = 0;
    uint64 VidMemLimit = 512 * 1024 * 1024;
    unsigned int PrevMaxLoadedTiles = 0;
    unsigned int MaxLoadedTiles = 0;
    float fLoadingRadius = 0;
    float fPrevLoadingRadius = 0;
    while( VideoMemoryUsage < VidMemLimit )
    {
        LastChunkMegs = ChunkMegs;
        ChunkMegs = TotalTerrainTileSize;

        fPrevLoadingRadius = fLoadingRadius;
        fLoadingRadius = iSqrt * fTileWidth * ( fChunkSpan / 2.0f );
        float fLoadingArea = D3DXFROMWINE_PI * fLoadingRadius * fLoadingRadius;
        float fTileArea = fTileWidth * fTileWidth;
        PrevMaxLoadedTiles = MaxLoadedTiles;
        MaxLoadedTiles = ( unsigned int )floorf( fLoadingArea / fTileArea );
        PrevVideoMemoryUsage = VideoMemoryUsage;
        VideoMemoryUsage = ( uint64 )( MaxLoadedTiles * TotalTerrainTileSize );

        iSqrt ++;
        SizeTry += 32;
    }
    iSqrt --;
    m_MaxChunksMapped = PrevMaxLoadedTiles + 20;
    ChunkMegs = LastChunkMegs;
    VideoMemoryUsage = PrevVideoMemoryUsage;
    fLoadingRadius = fPrevLoadingRadius;

    // Create Chunks
    unsigned int ChunkSide = SqrtNumTiles;
    int NumChunks = ChunkSide * ChunkSide;

    for( int i = 0; i < NumChunks; i++ )
    {	
        CHUNK_HEADER* pChunkHeader = new CHUNK_HEADER;
        TempHeaderList.Add( pChunkHeader );
    }

    // Create indices
    unsigned int iTile = 0;
    for( unsigned int y = 0; y < SqrtNumTiles; y++ )
    {
        for( unsigned int x = 0; x < SqrtNumTiles; x++ )
        {
            unsigned int ChunkX = x;
            unsigned int ChunkY = y;
            unsigned int ChunkIndex = ChunkY * ChunkSide + ChunkX;

            // Tile
            TERRAIN_TILE* pTile = Terrain.GetTile( iTile );
            D3DXFROMWINEVECTOR3 vCenter = ( pTile->BBox.min + pTile->BBox.max ) / 2.0f;

            // TerrainVB
            FILE_INDEX* pFileIndex = new FILE_INDEX;
            swprintf_s( pFileIndex->szFileName, MAX_PATH, "terrainVB%d_%d", x, y );
            pFileIndex->FileSize = SizeTerrainVB;
            pFileIndex->ChunkIndex = ChunkIndex;
            pFileIndex->OffsetIntoChunk = 0; // unknown
            pFileIndex->vCenter = vCenter;
            TempFileIndices.Add( pFileIndex );

            STRING strTemp;
            wcscpy_s( strTemp.str, MAX_PATH, "VB" );
            FullFilePath.Add( strTemp );

            // TerrainIB
            pFileIndex = new FILE_INDEX;
            swprintf_s( pFileIndex->szFileName, MAX_PATH, "terrainIB%d_%d", x, y );
            pFileIndex->FileSize = SizeTerrainIB;
            pFileIndex->ChunkIndex = ChunkIndex;
            pFileIndex->OffsetIntoChunk = 0; // unknown
            pFileIndex->vCenter = vCenter;
            TempFileIndices.Add( pFileIndex );

            wcscpy_s( strTemp.str, MAX_PATH, "IB" );
            FullFilePath.Add( strTemp );

            // TerrainDiffuse
            pFileIndex = new FILE_INDEX;
            swprintf_s( pFileIndex->szFileName, MAX_PATH, "terrainDiff%d_%d", x, y );
            pFileIndex->FileSize = SizeDiffuse;
            pFileIndex->ChunkIndex = ChunkIndex;
            pFileIndex->OffsetIntoChunk = 0; // unknown
            pFileIndex->vCenter = vCenter;
            TempFileIndices.Add( pFileIndex );

            FullFilePath.Add( strDiffuseTexture );

            // TerrainDiffuse
            pFileIndex = new FILE_INDEX;
            swprintf_s( pFileIndex->szFileName, MAX_PATH, "terrainNorm%d_%d", x, y );
            pFileIndex->FileSize = SizeNormal;
            pFileIndex->ChunkIndex = ChunkIndex;
            pFileIndex->OffsetIntoChunk = 0; // unknown
            pFileIndex->vCenter = vCenter;
            TempFileIndices.Add( pFileIndex );

            FullFilePath.Add( strNormalTexture );

            iTile++;
        }
    }

    // Get granularity
    SYSTEM_INFO SystemInfo;
    GetSystemInfo( &SystemInfo );
    uint64 Granularity = SystemInfo.dwAllocationGranularity; // Allocation granularity (always 64k)

    // Calculate offsets into chunks
    for( int c = 0; c < NumChunks; c++ )
    {
        CHUNK_HEADER* pChunkHeader = TempHeaderList.GetAt( c );
        pChunkHeader->ChunkSize = 0;

        for( int i = 0; i < TempFileIndices.GetSize(); i++ )
        {
            FILE_INDEX* pIndex = TempFileIndices.GetAt( i );

            if( pIndex->ChunkIndex == c )
            {
                pIndex->OffsetIntoChunk = pChunkHeader->ChunkSize;
                pChunkHeader->ChunkSize += pIndex->FileSize;
            }
        }
    }

    uint64 IndexSize = sizeof( PACKED_FILE_HEADER ) + sizeof( CHUNK_HEADER ) * TempHeaderList.GetSize() + sizeof
        ( FILE_INDEX ) * TempFileIndices.GetSize();
    uint64 ChunkOffset = AlignToGranularity( IndexSize, Granularity );

    // Align chunks to the proper granularities
    for( int c = 0; c < NumChunks; c++ )
    {
        CHUNK_HEADER* pChunkHeader = TempHeaderList.GetAt( c );
        pChunkHeader->ChunkOffset = ChunkOffset;

        ChunkOffset += AlignToGranularity( pChunkHeader->ChunkSize, Granularity );
    }

    // Fill in the header data
    m_FileHeader.FileSize = ChunkOffset;
    m_FileHeader.NumChunks = TempHeaderList.GetSize();
    m_FileHeader.NumFiles = TempFileIndices.GetSize();
    m_FileHeader.Granularity = Granularity;
    m_FileHeader.MaxChunksInVA = m_MaxChunksMapped;

    m_FileHeader.TileBytesSize = TotalTerrainTileSize;
    m_FileHeader.TileSideSize = pTile->BBox.max.x - pTile->BBox.min.x;
    m_FileHeader.LoadingRadius = fLoadingRadius;
    m_FileHeader.VideoMemoryUsageAtFullMips = VideoMemoryUsage;

    // Open the file
    hFile = CreateFile( szFileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN,
                        0 );
    if( INVALID_HANDLE_VALUE == hFile )
        return bRet;

    // write the header
    __DWORD__ dwWritten;
    __DWORD__ dwRead;
    if( !WriteFile( hFile, &m_FileHeader, sizeof( PACKED_FILE_HEADER ), &dwWritten, 0 ) )
        goto Error;

    // write out chunk headers
    for( int i = 0; i < TempHeaderList.GetSize(); i++ )
    {
        CHUNK_HEADER* pChunkHeader = TempHeaderList.GetAt( i );
        if( !WriteFile( hFile, pChunkHeader, sizeof( CHUNK_HEADER ), &dwWritten, 0 ) )
            goto Error;
    }

    // write the index
    for( int i = 0; i < TempFileIndices.GetSize(); i++ )
    {
        FILE_INDEX* pIndex = TempFileIndices.GetAt( i );
        if( !WriteFile( hFile, pIndex, sizeof( FILE_INDEX ), &dwWritten, 0 ) )
            goto Error;
    }

    // Fill in up to the granularity
    uint64 CurrentFileSize = IndexSize;
    CurrentFileSize = FillToGranularity( hFile, CurrentFileSize, Granularity );
    if( 0 == CurrentFileSize )
        goto Error;

    // Write out the files
    for( int c = 0; c < TempHeaderList.GetSize(); c++ )
    {
        for( int i = 0; i < TempFileIndices.GetSize(); i++ )
        {
            FILE_INDEX* pIndex = TempFileIndices.GetAt( i );

            if( pIndex->ChunkIndex == c )
            {
                // Write out the indexed file
                bool bDel = false;
                BYTE* pTempData = 0;

                if( 0 == wcscmp( FullFilePath.GetAt( i ).str, "VB" ) )
                {
                    pTempData = ( BYTE* )Terrain.GetTile( i / 4 )->pRawVertices;
                }
                else if( 0 == wcscmp( FullFilePath.GetAt( i ).str, "IB" ) )
                {
                    pTempData = ( BYTE* )Terrain.GetIndices();
                }
                else
                {
                    void* hIndexFile = CreateFile( FullFilePath.GetAt( i ).str, FILE_READ_DATA, FILE_SHARE_READ, 0,
                                                    OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
                    if( INVALID_HANDLE_VALUE == hIndexFile )
                        goto Error;

                    pTempData = new BYTE[ ( SIZE_T )pIndex->FileSize ];
                    if( !pTempData )
                    {
                        CloseHandle( hIndexFile );
                        goto Error;
                    }

                    bDel = true;

                    if( !ReadFile( hIndexFile, pTempData, ( __DWORD__ )pIndex->FileSize, &dwRead, 0 ) )
                    {
                        CloseHandle( hIndexFile );
                        SAFE_DELETE_ARRAY( pTempData );
                        goto Error;
                    }

                    CloseHandle( hIndexFile );
                }

                if( !WriteFile( hFile, pTempData, ( __DWORD__ )pIndex->FileSize, &dwWritten, 0 ) )
                {
                    if( bDel )
                        SAFE_DELETE_ARRAY( pTempData );
                    goto Error;
                }

                if( bDel )
                    SAFE_DELETE_ARRAY( pTempData );

                CurrentFileSize += pIndex->FileSize;
            }
        }

        // Fill in up to the granularity
        CurrentFileSize = FillToGranularity( hFile, CurrentFileSize, Granularity );
        if( 0 == CurrentFileSize )
            goto Error;
    }

    bRet = true;
Error:

    for( int i = 0; i < TempFileIndices.GetSize(); i++ )
    {
        FILE_INDEX* pIndex = TempFileIndices.GetAt( i );
        SAFE_DELETE( pIndex );
    }

    for( int i = 0; i < TempHeaderList.GetSize(); i++ )
    {
        CHUNK_HEADER* pChunkHeader = TempHeaderList.GetAt( i );
        SAFE_DELETE( pChunkHeader );
    }

    FlushFileBuffers( hFile );
    CloseHandle( hFile );
    return bRet;
}

//--------------------------------------------------------------------------------------
// Loads the index of a packed file and optionally creates mapped pointers using
// MapViewOfFile for each of the different chunks in the file.  The chunks must be
// aligned to the proper granularity (64k) or MapViewOfFile will fail.
//--------------------------------------------------------------------------------------
bool CPackedFile::LoadPackedFile( char* szFileName, bool b64Bit, CGrowableArray <LEVEL_ITEM*>* pLevelItemArray )
{
    bool bRet = false;

    // Open the file
    m_hFile = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
                          0 );
    if( INVALID_HANDLE_VALUE == m_hFile )
        return bRet;

    // read the header
    __DWORD__ dwRead;
    if( !ReadFile( m_hFile, &m_FileHeader, sizeof( PACKED_FILE_HEADER ), &dwRead, 0 ) )
        goto Error;

    // Make sure the granularity is the same
    SYSTEM_INFO SystemInfo;
    GetSystemInfo( &SystemInfo );
    if( m_FileHeader.Granularity != SystemInfo.dwAllocationGranularity )
        goto Error;

    m_ChunksMapped = 0;

    // Create the chunk and index data
    m_pChunks = new CHUNK_HEADER[ ( SIZE_T )m_FileHeader.NumChunks ];
    if( !m_pChunks )
        goto Error;
    m_pFileIndices = new FILE_INDEX[ ( SIZE_T )m_FileHeader.NumFiles ];
    if( !m_pFileIndices )
        goto Error;

    // Load the chunk and index data
    if( !ReadFile( m_hFile, m_pChunks, sizeof( CHUNK_HEADER ) * ( __DWORD__ )m_FileHeader.NumChunks, &dwRead, 0 ) )
        goto Error;
    if( !ReadFile( m_hFile, m_pFileIndices, sizeof( FILE_INDEX ) * ( ( __DWORD__ )m_FileHeader.NumFiles ), &dwRead,
                   0 ) )
        goto Error;

    // Load the level item array
    for( unsigned int i = 0; i < m_FileHeader.NumFiles; i += 4 )
    {
        LEVEL_ITEM* pLevelItem = new LEVEL_ITEM;
        ZeroMemory( pLevelItem, sizeof( LEVEL_ITEM ) );
        pLevelItem->vCenter = m_pFileIndices[i].vCenter;
        wcscpy_s( pLevelItem->szVBName, MAX_PATH, m_pFileIndices[i].szFileName );
        wcscpy_s( pLevelItem->szIBName, MAX_PATH, m_pFileIndices[i + 1].szFileName );
        wcscpy_s( pLevelItem->szDiffuseName, MAX_PATH, m_pFileIndices[i + 2].szFileName );
        wcscpy_s( pLevelItem->szNormalName, MAX_PATH, m_pFileIndices[i + 3].szFileName );
        pLevelItem->bLoaded = false;
        pLevelItem->bLoading = false;
        pLevelItem->bInLoadRadius = false;
        pLevelItem->bInFrustum = false;
        pLevelItem->CurrentCountdownDiff = 0;
        pLevelItem->CurrentCountdownNorm = 0;
        pLevelItem->bHasBeenRenderedDiffuse = false;
        pLevelItem->bHasBeenRenderedNormal = false;

        pLevelItemArray->Add( pLevelItem );
    }

    if( !b64Bit )
    {
        m_pMappedChunks = new MAPPED_CHUNK[ ( SIZE_T )m_FileHeader.NumChunks ];
        if( !m_pMappedChunks )
            goto Error;

        // Memory map the rest
        m_hFileMapping = CreateFileMapping( m_hFile, 0, PAGE_READONLY, 0, 0, 0 );
        if( INVALID_HANDLE_VALUE == m_hFile )
            goto Error;

        for( uint64 i = 0; i < m_FileHeader.NumChunks; i++ )
        {
            m_pMappedChunks[i].bInUse = FALSE;
            m_pMappedChunks[i].pMappingPointer = 0;
            m_pMappedChunks[i].UseCounter = 0;
        }
    }
    else
    {
        // TODO: Map the entire file on 64bits
    }

    bRet = true;
Error:
    return bRet;
}

//--------------------------------------------------------------------------------------
void CPackedFile::UnloadPackedFile()
{
    if( m_pMappedChunks )
    {
        for( unsigned int i = 0; i < m_FileHeader.NumChunks; i++ )
        {
            if( m_pMappedChunks[i].bInUse )
            {
                UnmapViewOfFile( m_pMappedChunks[i].pMappingPointer );
            }
        }
    }

    SAFE_DELETE_ARRAY( m_pMappedChunks );


    if( m_hFileMapping )
        CloseHandle( m_hFileMapping );
    m_hFileMapping = 0;

    if( m_hFile )
        CloseHandle( m_hFile );
    m_hFile = 0;

    SAFE_DELETE_ARRAY( m_pChunks );
    SAFE_DELETE_ARRAY( m_pFileIndices );
}


//--------------------------------------------------------------------------------------
void CPackedFile::EnsureChunkMapped( uint64 iChunk )
{
    if( !m_pMappedChunks[iChunk].bInUse )
    {
        if( m_ChunksMapped == m_MaxChunksMapped )
        {
            // We need to free a chunk
            unsigned int lruValue = m_CurrentUseCounter;
            uint64 lruChunk = ( unsigned int )-1;
            for( uint64 i = 0; i < m_FileHeader.NumChunks; i++ )
            {
                if( m_pMappedChunks[i].bInUse )
                {
                    if( lruChunk == ( unsigned int )-1 || m_pMappedChunks[i].UseCounter < lruValue )
                    {
                        lruValue = m_pMappedChunks[i].UseCounter;
                        lruChunk = i;
                    }
                }
            }

            UnmapViewOfFile( m_pMappedChunks[lruChunk].pMappingPointer );
            m_pMappedChunks[lruChunk].pMappingPointer = 0;
            m_pMappedChunks[lruChunk].bInUse = FALSE;
            m_ChunksMapped --;

            OutputDebugString( "Unmapped File Chunk\n" );
        }

        // Map this chunk
        __DWORD__ dwOffsetHigh = ( __DWORD__ )( ( m_pChunks[iChunk].ChunkOffset & 0xFFFFFFFF00000000 ) >> 32 );
        __DWORD__ dwOffsetLow = ( __DWORD__ )( ( m_pChunks[iChunk].ChunkOffset & 0x00000000FFFFFFFF ) );
        m_pMappedChunks[iChunk].bInUse = TRUE;
        m_pMappedChunks[iChunk].pMappingPointer = MapViewOfFile( m_hFileMapping, FILE_MAP_READ, dwOffsetHigh,
                                                                 dwOffsetLow, ( __DWORD__ )m_pChunks[iChunk].ChunkSize );
        if( !m_pMappedChunks[iChunk].pMappingPointer )
        {
            OutputDebugString( "File Chunk not Mapped!\n" );
        }
        m_ChunksMapped ++;
    }

    // Set our use counter for the LRU check
    m_pMappedChunks[iChunk].UseCounter = m_CurrentUseCounter;
    m_CurrentUseCounter ++;
}

//--------------------------------------------------------------------------------------
bool CPackedFile::GetPackedFileInfo( char* szFile, unsigned int* pDataBytes )
{
    char str[MAX_PATH];
    MultiByteToWideChar( CP_ACP, 0, szFile, -1, str, MAX_PATH );

    return GetPackedFileInfo( str, pDataBytes );
}

//--------------------------------------------------------------------------------------
// Finds information about a resource using the index
//--------------------------------------------------------------------------------------
bool CPackedFile::GetPackedFileInfo( char* szFile, unsigned int* pDataBytes )
{
    // Look the file up in the index
    int iFoundIndex = -1;
    for( unsigned int i = 0; i < m_FileHeader.NumFiles; i++ )
    {
        if( 0 == wcscmp( szFile, m_pFileIndices[i].szFileName ) )
        {
            iFoundIndex = i;
            break;
        }
    }

    if( -1 == iFoundIndex )
        return false;

    *pDataBytes = ( unsigned int )m_pFileIndices[iFoundIndex].FileSize;

    return true;
}

//--------------------------------------------------------------------------------------
bool CPackedFile::GetPackedFile( char* szFile, BYTE** ppData, unsigned int* pDataBytes )
{
    char str[MAX_PATH];
    MultiByteToWideChar( CP_ACP, 0, szFile, -1, str, MAX_PATH );

    return GetPackedFile( str, ppData, pDataBytes );
}

//--------------------------------------------------------------------------------------
// Finds the location of a resource in a packed file and returns its contents in 
// *ppData.
//--------------------------------------------------------------------------------------
bool CPackedFile::GetPackedFile( char* szFile, BYTE** ppData, unsigned int* pDataBytes )
{
    // Look the file up in the index
    int iFoundIndex = -1;
    for( unsigned int i = 0; i < m_FileHeader.NumFiles; i++ )
    {
        if( 0 == wcscmp( szFile, m_pFileIndices[i].szFileName ) )
        {
            iFoundIndex = i;
            break;
        }
    }

    if( -1 == iFoundIndex )
        return false;

    *pDataBytes = ( unsigned int )m_pFileIndices[iFoundIndex].FileSize;

    // Memory mapped io
    EnsureChunkMapped( m_pFileIndices[iFoundIndex].ChunkIndex );
    *ppData = ( BYTE* )m_pMappedChunks[ m_pFileIndices[iFoundIndex].ChunkIndex ].pMappingPointer +
        m_pFileIndices[iFoundIndex].OffsetIntoChunk;

    return true;
}

//--------------------------------------------------------------------------------------
bool CPackedFile::UsingMemoryMappedIO()
{
    return ( 0 != m_pMappedChunks );
}

//--------------------------------------------------------------------------------------
void CPackedFile::SetMaxChunksMapped( unsigned int maxmapped )
{
    m_MaxChunksMapped = maxmapped;
}

//--------------------------------------------------------------------------------------
uint64 CPackedFile::GetTileBytesSize()
{
    return m_FileHeader.TileBytesSize;
}

//--------------------------------------------------------------------------------------
float CPackedFile::GetTileSideSize()
{
    return m_FileHeader.TileSideSize;
}

//--------------------------------------------------------------------------------------
float CPackedFile::GetLoadingRadius()
{
    return m_FileHeader.LoadingRadius;
}

//--------------------------------------------------------------------------------------
unsigned int CPackedFile::GetMaxChunksMapped()
{
    return m_MaxChunksMapped;
}

//--------------------------------------------------------------------------------------
unsigned int CPackedFile::GetMaxChunksInVA()
{
    return m_FileHeader.MaxChunksInVA;
}

//--------------------------------------------------------------------------------------
uint64 CPackedFile::GetNumChunks()
{
    return m_FileHeader.NumChunks;
}

//--------------------------------------------------------------------------------------
uint64 CPackedFile::GetVideoMemoryUsageAtFullMips()
{
    return m_FileHeader.VideoMemoryUsageAtFullMips;
}
