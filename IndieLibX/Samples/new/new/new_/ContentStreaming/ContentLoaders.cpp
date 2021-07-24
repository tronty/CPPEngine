/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//--------------------------------------------------------------------------------------
// File: ContentLoaders.cpp
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "DXUT.h"
#include "ContentLoaders.h"
#include "AsyncLoader.h"
#include "PackedFile.h"

//--------------------------------------------------------------------------------------
CTextureLoader::CTextureLoader( char* szFileName, CPackedFile* pPackedFile ) : m_pData( 0 ),
                                                                                m_cBytes( 0 ),
                                                                                m_pPackedFile( pPackedFile )
{
    wcscpy_s( m_szFileName, MAX_PATH, szFileName );
}

//--------------------------------------------------------------------------------------
CTextureLoader::~CTextureLoader()
{
    Destroy();
}

//--------------------------------------------------------------------------------------
// The SDK uses only DXTn (BCn) textures with a few small non-compressed texture.  However,
// for a game that uses compressed textures or textures in a zip file, this is the place
// to decompress them.
//--------------------------------------------------------------------------------------
int  CTextureLoader::Decompress( void** ppData, SIZE_T* pcBytes )
{
    *ppData = ( void* )m_pData;
    *pcBytes = m_cBytes;
    return 0;
}

//--------------------------------------------------------------------------------------
int  CTextureLoader::Destroy()
{
    if( !m_pPackedFile->UsingMemoryMappedIO() )
    {
        SAFE_DELETE_ARRAY( m_pData );
    }
    m_cBytes = 0;

    return 0;
}

//--------------------------------------------------------------------------------------
// Load the texture from the packed file.  If not-memory mapped, allocate enough memory
// to hold the data.
//--------------------------------------------------------------------------------------
int  CTextureLoader::Load()
{
    if( m_pPackedFile->UsingMemoryMappedIO() )
    {
        if( !m_pPackedFile->GetPackedFile( m_szFileName, &m_pData, &m_cBytes ) )
            return E_FAIL;
    }
    else
    {
        if( !m_pPackedFile->GetPackedFileInfo( m_szFileName, &m_cBytes ) )
            return E_FAIL;

        // create enough space for the file data
        m_pData = new BYTE[ m_cBytes ];
        if( !m_pData )
            return 1;

        if( !m_pPackedFile->GetPackedFile( m_szFileName, &m_pData, &m_cBytes ) )
            return E_FAIL;
    }

    return 0;
}

//--------------------------------------------------------------------------------------
// This is a private function that either Locks and copies the data (D3D9) or calls
// UpdateSubresource (D3D10).
//--------------------------------------------------------------------------------------
BOOL CTextureProcessor::PopulateTexture()
{
    if( 0 == m_iNumLockedPtrs )
        return FALSE;

    DDS_HEADER* pSurfDesc9 = ( DDS_HEADER* )( ( BYTE* )m_pData + sizeof( __DWORD__ ) );

    unsigned int Width = pSurfDesc9->dwWidth;
    unsigned int Height = pSurfDesc9->dwHeight;
    unsigned int MipLevels = pSurfDesc9->dwMipMapCount;
    if( MipLevels > m_SkipMips )
        MipLevels -= m_SkipMips;
    else
        m_SkipMips = 0;
    if( 0 == MipLevels )
        MipLevels = 1;
    FORMAT Format = GetD3D9Format( pSurfDesc9->ddspf );

    // Skip X number of mip levels
    unsigned int BytesToSkip = 0;
    for( unsigned int i = 0; i < m_SkipMips; i++ )
    {
        unsigned int SurfaceBytes;
        GetSurfaceInfo( Width, Height, Format, &SurfaceBytes, 0, 0 );

        BytesToSkip += SurfaceBytes;
        Width = Width >> 1;
        Height = Height >> 1;
    }

    void* pTexData = ( BYTE* )m_pData + sizeof( __DWORD__ ) + sizeof( DDS_HEADER ) + BytesToSkip;

    // Lock, fill, unlock
    unsigned int NumBytes, RowBytes, NumRows;
    BYTE* pSrcBits = ( BYTE* )pTexData;

    stx_Resource pTexture10 = 0;
    if( LDT_D3D10 == m_Device.Type )
    {
        m_pRealRV10->GetResource( &pTexture10 );
    }

    for( unsigned int i = 0; i < m_iNumLockedPtrs; i++ )
    {
        GetSurfaceInfo( Width, Height, Format, &NumBytes, &RowBytes, &NumRows );

        if( LDT_D3D10 == m_Device.Type )
        {
#if defined(USE_D3D10_STAGING_RESOURCES)
            BYTE* pDestBits = ( BYTE* )m_pLockedRects10[i].pData;

            // Copy stride line by line
            for( unsigned int h = 0; h < NumRows; h++ )
            {
                CopyMemory( pDestBits, pSrcBits, RowBytes );
                pDestBits += m_pLockedRects10[i].RowPitch;
                pSrcBits += RowBytes;
            }
#endif

#if !defined(USE_D3D10_STAGING_RESOURCES)
            // Use UpdateSubresource in d3d10
            m_Device.pDev10->UpdateSubresource( pTexture10, D3D10CalcSubresource( i, 0, MipLevels ), 0, pSrcBits, RowBytes, 0 );
            pSrcBits += NumBytes;
#endif
        }
        else if( LDT_D3D9 == m_Device.Type )
        {
            BYTE* pDestBits = ( BYTE* )m_pLockedRects[i].pBits;

            // Copy stride line by line
            for( unsigned int h = 0; h < NumRows; h++ )
            {
                CopyMemory( pDestBits, pSrcBits, RowBytes );
                pDestBits += m_pLockedRects[i].Pitch;
                pSrcBits += RowBytes;
            }
        }

        Width = Width >> 1;
        Height = Height >> 1;
        if( Width == 0 )
            Width = 1;
        if( Height == 0 )
            Height = 1;
    }

    if( LDT_D3D10 == m_Device.Type )
    {
        ////////////SAFE_RELEASE( pTexture10 );
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------
CTextureProcessor::CTextureProcessor( 
                                      TextureID   ppRV10,
                                      CResourceReuseCache* pResourceReuseCache,
                                      unsigned int SkipMips ) : m_Device( pDevice ),
                                                        m_ppRV10( ppRV10 ),
                                                        m_ppTexture9( 0 ),
                                                        m_pResourceReuseCache( pResourceReuseCache ),
                                                        m_SkipMips( SkipMips )
{
    *m_ppRV10 = 0;
}

//--------------------------------------------------------------------------------------
CTextureProcessor::CTextureProcessor( IDirect3DDevice9* pDevice,
                                      TextureID  ppTexture9,
                                      CResourceReuseCache* pResourceReuseCache,
                                      unsigned int SkipMips ) : m_Device( pDevice ),
                                                        m_ppRV10( 0 ),
                                                        m_ppTexture9( ppTexture9 ),
                                                        m_pResourceReuseCache( pResourceReuseCache ),
                                                        m_SkipMips( SkipMips )
{
    *m_ppTexture9 = 0;
}

//--------------------------------------------------------------------------------------
CTextureProcessor::~CTextureProcessor()
{
    Destroy();
}

//--------------------------------------------------------------------------------------
// LockDeviceObject is called by the graphics thread to find an appropriate resource from
// the resource reuse cache.  If no resource is found, the return code tells the calling
// thread to try again later.  For D3D9, this function also locks all mip-levels.
//--------------------------------------------------------------------------------------
int  CTextureProcessor::LockDeviceObject()
{
    int hr = 0;
    m_iNumLockedPtrs = 0;

    if( !m_pResourceReuseCache )
        return E_FAIL;

    // setup the pointers in the process request
    DDS_HEADER* pSurfDesc9 = ( DDS_HEADER* )( ( BYTE* )m_pData + sizeof( __DWORD__ ) );

    unsigned int Width = pSurfDesc9->dwWidth;
    unsigned int Height = pSurfDesc9->dwHeight;
    unsigned int MipLevels = pSurfDesc9->dwMipMapCount;
    if( MipLevels > m_SkipMips )
        MipLevels -= m_SkipMips;
    else
        m_SkipMips = 0;
    if( 0 == MipLevels )
        MipLevels = 1;
    FORMAT Format = GetD3D9Format( pSurfDesc9->ddspf );

    // Skip X number of mip levels
    for( unsigned int i = 0; i < m_SkipMips; i++ )
    {
        Width = Width >> 1;
        Height = Height >> 1;
    }

    // Find an appropriate resource
    if( LDT_D3D10 == m_Device.Type )
    {
        m_pRealRV10 = m_pResourceReuseCache->GetFreeTexture10( Width, Height,
                                                               MipLevels, ( unsigned int )Format, &m_pStaging10 );

        if( 0 == m_pRealRV10 )
        {
            hr = E_TRYAGAIN;
        }
        else
        {
#if defined(USE_D3D10_STAGING_RESOURCES)
            // Lock
            m_iNumLockedPtrs = MipLevels - m_SkipMips;
            for( unsigned int i = 0; i < m_iNumLockedPtrs; i++ )
            {
                hr = m_pStaging10->Map( i, D3D10_MAP_WRITE, 0, &m_pLockedRects10[i] );
                if( FAILED( hr ) )
                {
                    m_iNumLockedPtrs = 0;
                    *m_ppRV10 = ( TextureID  )ERROR_RESOURCE_VALUE;
                    return hr;
                }
            }
#endif

#if !defined(USE_D3D10_STAGING_RESOURCES)
            m_iNumLockedPtrs = MipLevels;
            hr = 0;
#endif
        }
    }
    else if( LDT_D3D9 == m_Device.Type )
    {
        m_pRealTexture9 = m_pResourceReuseCache->GetFreeTexture9( Width, Height, MipLevels, ( unsigned int )Format );

        if( 0 == m_pRealTexture9 )
        {
            hr = E_TRYAGAIN;
        }
        else
        {
            // Lock
            m_iNumLockedPtrs = MipLevels - m_SkipMips;
            for( unsigned int i = 0; i < m_iNumLockedPtrs; i++ )
            {
                hr = m_pRealTexture9->LockRect( i, &m_pLockedRects[i], 0, 0 );
                if( FAILED( hr ) )
                {
                    m_iNumLockedPtrs = 0;
                    *m_ppTexture9 = ( TextureID )ERROR_RESOURCE_VALUE;
                    return hr;
                }
            }
        }
    }

    return hr;
}

//--------------------------------------------------------------------------------------
// On D3D9, this unlocks the resource.  On D3D10, this actually populates the resource.
//--------------------------------------------------------------------------------------
int  CTextureProcessor::UnLockDeviceObject()
{
    int hr = 0;

    if( 0 == m_iNumLockedPtrs )
        return E_FAIL;

    // Find an appropriate resource
    if( LDT_D3D10 == m_Device.Type )
    {
#if defined(USE_D3D10_STAGING_RESOURCES)
        // Unlock
        for( unsigned int i = 0; i < m_iNumLockedPtrs; i++ )
        {
            m_pStaging10->Unmap( i );
        }
        stx_Resource pDest;
        m_pRealRV10->GetResource( &pDest );
        m_Device.pDev10->CopyResource( pDest, m_pStaging10 );
        ////////////SAFE_RELEASE( pDest );
#endif

#if !defined(USE_D3D10_STAGING_RESOURCES)
        if( !PopulateTexture() )
            hr = E_FAIL;
#endif
        *m_ppRV10 = m_pRealRV10;
    }
    else if( LDT_D3D9 == m_Device.Type )
    {
        // Unlock
        for( unsigned int i = 0; i < m_iNumLockedPtrs; i++ )
        {
            hr = m_pRealTexture9->UnlockRect( i );
            if( FAILED( hr ) )
            {
                return hr;
            }
        }

        *m_ppTexture9 = m_pRealTexture9;
    }

    return hr;
}

//--------------------------------------------------------------------------------------
int  CTextureProcessor::Destroy()
{
    return 0;
}

//--------------------------------------------------------------------------------------
// Any texture processing would go here.
//--------------------------------------------------------------------------------------
int  CTextureProcessor::Process( void* pData, SIZE_T cBytes )
{
    int hr = 0;

    if( m_pResourceReuseCache )
    {	
        __DWORD__ dwMagicNumber = *( __DWORD__* )pData;
        if( dwMagicNumber != 0x20534444 )
            hr = E_FAIL;
    }

    m_pData = ( BYTE* )pData;
    m_cBytes = cBytes;

    return hr;
}

//--------------------------------------------------------------------------------------
// Copies the data to the locked pointer on D3D9
//--------------------------------------------------------------------------------------
int  CTextureProcessor::CopyToResource()
{
    int hr = 0;

#if 0
    if( LDT_D3D9 == m_Device.Type )
#endif
    {
        if( !PopulateTexture() )
        {
            hr = E_FAIL;
        }
    }

    return hr;
}

//--------------------------------------------------------------------------------------
void  CTextureProcessor::SetResourceError()
{
    if( LDT_D3D10 == m_Device.Type )
    {
        *m_ppRV10 = ( TextureID  )ERROR_RESOURCE_VALUE;
    }
    else if( LDT_D3D9 == m_Device.Type )
    {
        *m_ppTexture9 = ( TextureID )ERROR_RESOURCE_VALUE;
    }
}

//--------------------------------------------------------------------------------------
CVertexBufferLoader::CVertexBufferLoader()
{
}
CVertexBufferLoader::~CVertexBufferLoader()
{
    Destroy();
}
int  CVertexBufferLoader::Decompress( void** ppData, SIZE_T* pcBytes )
{
    *ppData = 0; *pcBytes = 0; return 0;
}
int  CVertexBufferLoader::Destroy()
{
    return 0;
}
int  CVertexBufferLoader::Load()
{
    return 0;
}

//--------------------------------------------------------------------------------------
CVertexBufferProcessor::CVertexBufferProcessor( 
                                                exBufferID   ppBuffer10,
                                                D3D10_BUFFER_DESC* pBufferDesc,
                                                void* pData,
                                                CResourceReuseCache* pResourceReuseCache ) : m_Device( pDevice ),
                                                                                             m_ppBuffer10(
                                                                                             ppBuffer10 ),
                                                                                             m_ppBuffer9( 0 ),
                                                                                             m_iSizeBytes( 0 ),
                                                                                             m_Usage( 0 ),
                                                                                             m_FVF( 0 ),
                                                                                             m_Pool( D3DPOOL_DEFAULT ),
                                                                                             m_pData( pData ),
                                                                                             m_pResourceReuseCache(
                                                                                             pResourceReuseCache )
{
    *m_ppBuffer10 = 0;
    CopyMemory( &m_BufferDesc, pBufferDesc, sizeof( 
}

//--------------------------------------------------------------------------------------
CVertexBufferProcessor::CVertexBufferProcessor( IDirect3DDevice9* pDevice,
                                                VertexBufferID  ppBuffer9,
                                                unsigned int iSizeBytes,
                                                __DWORD__ Usage,
                                                __DWORD__ FVF,
                                                D3DPOOL Pool,
                                                void* pData,
                                                CResourceReuseCache* pResourceReuseCache ) : m_Device( pDevice ),
                                                                                             m_ppBuffer10( 0 ),
                                                                                             m_ppBuffer9( ppBuffer9 ),
                                                                                             m_iSizeBytes(
                                                                                             iSizeBytes ),
                                                                                             m_Usage( Usage ),
                                                                                             m_FVF( FVF ),
                                                                                             m_Pool( Pool ),
                                                                                             m_pData( pData ),
                                                                                             m_pResourceReuseCache(
                                                                                             pResourceReuseCache )
{
    *m_ppBuffer9 = 0;
}

//--------------------------------------------------------------------------------------
CVertexBufferProcessor::~CVertexBufferProcessor()
{
    Destroy();
}

//--------------------------------------------------------------------------------------
// LockDeviceObject is called by the graphics thread to find an appropriate resource from
// the resource reuse cache.  If no resource is found, the return code tells the calling
// thread to try again later.  For D3D9, this function also locks the resource.
//--------------------------------------------------------------------------------------
int  CVertexBufferProcessor::LockDeviceObject()
{
    int hr = 0;
    if( !m_pResourceReuseCache )
        return E_FAIL;

    if( LDT_D3D10 == m_Device.Type )
    {
        m_pRealBuffer10 = m_pResourceReuseCache->GetFreeVB10( m_BufferDesc.ByteWidth );
        if( 0 == m_pRealBuffer10 )
        {
            hr = E_TRYAGAIN;
        }
        else
        {
            hr = 0;
        }
    }
    else if( LDT_D3D9 == m_Device.Type )
    {
        m_pRealBuffer9 = m_pResourceReuseCache->GetFreeVB9( m_iSizeBytes );
        if( 0 == m_pRealBuffer9 )
        {
            hr = E_TRYAGAIN;
        }
        else
        {
            // Lock
            hr = m_pRealBuffer9->Lock( 0, 0, ( void** )&m_pLockedData, 0 );
            if( FAILED( hr ) )
            {
                m_pLockedData = 0;
                return hr;
            }
        }
    }

    return hr;
}

//--------------------------------------------------------------------------------------
// On D3D9, this unlocks the resource.  On D3D10, this actually populates the resource.
//--------------------------------------------------------------------------------------
int  CVertexBufferProcessor::UnLockDeviceObject()
{
    int hr = 0;

    if( LDT_D3D10 == m_Device.Type )
    {
        m_Device.pDev10->UpdateSubresource( m_pRealBuffer10, 0, 0, m_pData, 0, 0 );
        *m_ppBuffer10 = m_pRealBuffer10;
    }
    else if( LDT_D3D9 == m_Device.Type )
    {
        if( !m_pLockedData )
            return E_FAIL;

        // Unlock
        hr = m_pRealBuffer9->Unlock();
        *m_ppBuffer9 = m_pRealBuffer9;
    }

    return hr;
}

//--------------------------------------------------------------------------------------
int  CVertexBufferProcessor::Destroy()
{
    return 0;
}

//--------------------------------------------------------------------------------------
int  CVertexBufferProcessor::Process( void* pData, SIZE_T cBytes )
{
    return 0;
}

//--------------------------------------------------------------------------------------
// Copies the data to the locked pointer on D3D9
//--------------------------------------------------------------------------------------
int  CVertexBufferProcessor::CopyToResource()
{
    if( LDT_D3D9 == m_Device.Type )
    {
        if( !m_pLockedData )
        {
            return E_FAIL;
        }

        CopyMemory( m_pLockedData, m_pData, m_iSizeBytes );
    }

    return 0;
}

//--------------------------------------------------------------------------------------
void  CVertexBufferProcessor::SetResourceError()
{
    if( LDT_D3D10 == m_Device.Type )
    {
        *m_ppBuffer10 = ( exBufferID  )ERROR_RESOURCE_VALUE;
    }
    else if( LDT_D3D9 == m_Device.Type )
    {
        *m_ppBuffer9 = ( VertexBufferID )ERROR_RESOURCE_VALUE;
    }
}

//--------------------------------------------------------------------------------------
CIndexBufferLoader::CIndexBufferLoader()
{
}
CIndexBufferLoader::~CIndexBufferLoader()
{
    Destroy();
}
int  CIndexBufferLoader::Decompress( void** ppData, SIZE_T* pcBytes )
{
    *ppData = 0; *pcBytes = 0; return 0;
}
int  CIndexBufferLoader::Destroy()
{
    return 0;
}
int  CIndexBufferLoader::Load()
{
    return 0;
}

//--------------------------------------------------------------------------------------
CIndexBufferProcessor::CIndexBufferProcessor( 
                                              exBufferID   ppBuffer10,
                                              D3D10_BUFFER_DESC* pBufferDesc,
                                              void* pData,
                                              CResourceReuseCache* pResourceReuseCache ) : m_Device( pDevice ),
                                                                                           m_ppBuffer10( ppBuffer10 ),
                                                                                           m_ppBuffer9( 0 ),
                                                                                           m_iSizeBytes( 0 ),
                                                                                           m_Usage( 0 ),
                                                                                           m_ibFormat(
                                                                                           D3DFMT_INDEX32 ),
                                                                                           m_Pool( D3DPOOL_DEFAULT ),
                                                                                           m_pData( pData ),
                                                                                           m_pResourceReuseCache(
                                                                                           pResourceReuseCache )
{
    *m_ppBuffer10 = 0;
    CopyMemory( &m_BufferDesc, pBufferDesc, sizeof( 
}

//--------------------------------------------------------------------------------------
CIndexBufferProcessor::CIndexBufferProcessor( IDirect3DDevice9* pDevice,
                                              IndexBufferID  ppBuffer9,
                                              unsigned int iSizeBytes,
                                              __DWORD__ Usage,
                                              FORMAT ibFormat,
                                              D3DPOOL Pool,
                                              void* pData,
                                              CResourceReuseCache* pResourceReuseCache ) : m_Device( pDevice ),
                                                                                           m_ppBuffer10( 0 ),
                                                                                           m_ppBuffer9( ppBuffer9 ),
                                                                                           m_iSizeBytes( iSizeBytes ),
                                                                                           m_Usage( Usage ),
                                                                                           m_ibFormat( ibFormat ),
                                                                                           m_Pool( Pool ),
                                                                                           m_pData( pData ),
                                                                                           m_pResourceReuseCache(
                                                                                           pResourceReuseCache )
{
    *m_ppBuffer9 = 0;
}

//--------------------------------------------------------------------------------------
CIndexBufferProcessor::~CIndexBufferProcessor()
{
    Destroy();
}

//--------------------------------------------------------------------------------------
// LockDeviceObject is called by the graphics thread to find an appropriate resource from
// the resource reuse cache.  If no resource is found, the return code tells the calling
// thread to try again later.  For D3D9, this function also locks the resource.
//--------------------------------------------------------------------------------------
int  CIndexBufferProcessor::LockDeviceObject()
{
    int hr = 0;
    if( !m_pResourceReuseCache )
        return E_FAIL;

    if( LDT_D3D10 == m_Device.Type )
    {
        m_pRealBuffer10 = m_pResourceReuseCache->GetFreeIB10( m_BufferDesc.ByteWidth, 0 );
        if( 0 == m_pRealBuffer10 )
        {
            hr = E_TRYAGAIN;
        }
        else
        {
            hr = 0;
        }
    }
    else if( LDT_D3D9 == m_Device.Type )
    {
        m_pRealBuffer9 = m_pResourceReuseCache->GetFreeIB9( m_iSizeBytes, m_ibFormat );
        if( 0 == m_pRealBuffer9 )
        {
            hr = E_TRYAGAIN;
        }
        else
        {
            // Lock
            hr = m_pRealBuffer9->Lock( 0, 0, ( void** )&m_pLockedData, 0 );
            if( FAILED( hr ) )
            {
                m_pLockedData = 0;
                return hr;
            }
        }
    }

    return hr;
}

//--------------------------------------------------------------------------------------
// On D3D9, this unlocks the resource.  On D3D10, this actually populates the resource.
//--------------------------------------------------------------------------------------
int  CIndexBufferProcessor::UnLockDeviceObject()
{
    int hr = 0;

    if( LDT_D3D10 == m_Device.Type )
    {
        m_Device.pDev10->UpdateSubresource( m_pRealBuffer10, 0, 0, m_pData, 0, 0 );
        *m_ppBuffer10 = m_pRealBuffer10;
    }
    else if( LDT_D3D9 == m_Device.Type )
    {
        if( !m_pLockedData )
            return E_FAIL;

        // Unlock
        hr = m_pRealBuffer9->Unlock();
        *m_ppBuffer9 = m_pRealBuffer9;
    }

    return hr;
}

//--------------------------------------------------------------------------------------
int  CIndexBufferProcessor::Destroy()
{
    return 0;
}

//--------------------------------------------------------------------------------------
int  CIndexBufferProcessor::Process( void* pData, SIZE_T cBytes )
{
    return 0;
}

//--------------------------------------------------------------------------------------
// Copies the data to the locked pointer on D3D9
//--------------------------------------------------------------------------------------
int  CIndexBufferProcessor::CopyToResource()
{
    if( LDT_D3D10 == m_Device.Type )
    {

    }
    else if( LDT_D3D9 == m_Device.Type )
    {
        if( !m_pLockedData )
        {
            return E_FAIL;
        }

        CopyMemory( m_pLockedData, m_pData, m_iSizeBytes );
    }

    return 0;
}

//--------------------------------------------------------------------------------------
void  CIndexBufferProcessor::SetResourceError()
{
    if( LDT_D3D10 == m_Device.Type )
    {
        *m_ppBuffer10 = ( exBufferID  )ERROR_RESOURCE_VALUE;
    }
    else if( LDT_D3D9 == m_Device.Type )
    {
        *m_ppBuffer9 = ( IndexBufferID )ERROR_RESOURCE_VALUE;
    }
}

//--------------------------------------------------------------------------------------
// SDKMesh
//--------------------------------------------------------------------------------------
CSDKMeshLoader::CSDKMeshLoader( MeshRenderer2* pMesh, std::string szFileName,
                                bool bCreateAdjacencyIndices, SDKMESH_ S10* pLoaderCallbacks ) : m_pMesh(
                                                                                                        pMesh ),
                                                                                                        m_Device(
                                                                                                        pDev10 ),
                                                                                                        m_bCreateAdjacencyIndices( bCreateAdjacencyIndices ),
                                                                                                        m_pLoaderCallbacks10( pLoaderCallbacks ),
                                                                                                        m_pLoaderCallbacks9( 0 )
{
    wcscpy_s( m_szFileName, MAX_PATH, szFileName );
}

//--------------------------------------------------------------------------------------
CSDKMeshLoader::CSDKMeshLoader( MeshRenderer2* pMesh, IDirect3DDevice9* pDev9, std::string szFileName,
                                bool bCreateAdjacencyIndices, SDKMESH_ S9* pLoaderCallbacks ) : m_pMesh(
                                                                                                       pMesh ),
                                                                                                       m_Device(
                                                                                                       pDev9 ),
                                                                                                       m_bCreateAdjacencyIndices( bCreateAdjacencyIndices ),
                                                                                                       m_pLoaderCallbacks10( 0 ),
                                                                                                       m_pLoaderCallbacks9( pLoaderCallbacks )
{
    wcscpy_s( m_szFileName, MAX_PATH, szFileName );
}

//--------------------------------------------------------------------------------------
CSDKMeshLoader::~CSDKMeshLoader()
{
    Destroy();
}

//--------------------------------------------------------------------------------------
int  CSDKMeshLoader::Decompress( void** ppData, SIZE_T* pcBytes )
{
    *ppData = 0;
    *pcBytes = 0;
    return 0;
}

//--------------------------------------------------------------------------------------
int  CSDKMeshLoader::Destroy()
{
    return 0;
}

//--------------------------------------------------------------------------------------
int  CSDKMeshLoader::Load()
{
    int hr = E_FAIL;

    if( m_pMesh )
    {
        if( LDT_D3D10 == m_Device.Type )
        {
            hr = m_pMesh->Create( m_Device.pDev10, m_szFileName, m_bCreateAdjacencyIndices, m_pLoaderCallbacks10 );
        }
        else if( LDT_D3D9 == m_Device.Type )
        {
            hr = m_pMesh->Create( m_Device.pDev9, m_szFileName, m_bCreateAdjacencyIndices, m_pLoaderCallbacks9 );
        }
    }

    return hr;
}

//--------------------------------------------------------------------------------------
CSDKMeshProcessor::CSDKMeshProcessor()
{
}
CSDKMeshProcessor::~CSDKMeshProcessor()
{
    Destroy();
}
int  CSDKMeshProcessor::LockDeviceObject()
{
    return 0;
}
int  CSDKMeshProcessor::UnLockDeviceObject()
{
    return 0;
}
int  CSDKMeshProcessor::Destroy()
{
    return 0;
}
int  CSDKMeshProcessor::Process( void* pData, SIZE_T cBytes )
{
    return 0;
}
int  CSDKMeshProcessor::CopyToResource()
{
    return 0;
}
void     CSDKMeshProcessor::SetResourceError()
{
}
