#include <STXCPP/Platform.h>
using namespace STX;
#ifdef BUILDCONTENTSTREAMING
#if 0
//--------------------------------------------------------------------------------------
// File: ContentLoaders.cpp
//
// Illustrates streaming content 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include <FrameworkHLSLCg3/Platform.h>

#include "AsyncLoader.h"
#include "PackedFile.h"

#define ERROR_RESOURCE_VALUE -1
#if 0
//--------------------------------------------------------------------------------------
// This is a private function that either Locks and copies the data.
//--------------------------------------------------------------------------------------
BOOL CTextureProcessor::PopulateTexture()
{
    if( 0 == m_iNumLockedPtrs )
	return FALSE;

    DDS_HEADER* pSurfDesc9 = ( DDS_HEADER* )( ( BYTE* )m_pData + sizeof( DWORD ) );

    UINT Width = pSurfDesc9->dwWidth;
    UINT Height = pSurfDesc9->dwHeight;
    //UINT MipLevels = 1;//D3D10???pSurfDesc9->dwMipMapCount;
    UINT MipLevels = pSurfDesc9->dwMipMapCount;
	if( MipLevels > m_SkipMips )
	MipLevels -= m_SkipMips;
    else
	m_SkipMips = 0;
    if( 0 == MipLevels )
	MipLevels = 1;
    D3DFORMAT Format = GetD3D9Format( pSurfDesc9->ddspf );

    // Skip X number of mip levels
    UINT BytesToSkip = 0;
    for( UINT i = 0; i < m_SkipMips; i++ )
    {
	UINT SurfaceBytes;
	GetSurfaceInfo( Width, Height, Format, &SurfaceBytes, NULL, NULL );

	BytesToSkip += SurfaceBytes;
	Width = Width >> 1;
	Height = Height >> 1;
    }

    void* pTexData = ( BYTE* )m_pData + sizeof( DWORD ) + sizeof( DDS_HEADER ) + BytesToSkip;

    // Lock, fill, unlock
    UINT NumBytes, RowBytes, NumRows;
    BYTE* pSrcBits = ( BYTE* )pTexData;

   

    for( UINT i = 0; i < m_iNumLockedPtrs; i++ )
    {
	GetSurfaceInfo( Width, Height, Format, &NumBytes, &RowBytes, &NumRows );

	 
	{
	    BYTE* pDestBits = ( BYTE* )m_pLockedRectsBits[i];

	    // Copy stride line by line
	    for( UINT h = 0; h < NumRows; h++ )
	    {
		CopyMemory( pDestBits, pSrcBits, RowBytes );
		pDestBits += m_pLockedRectsPitch[i];
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

   

    return TRUE;
}

//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
CTextureProcessor::CTextureProcessor( 
				      TextureID* ppTexture9,
				      CResourceReuseCache* pResourceReuseCache,
				      UINT SkipMips ) : 
							
							m_ppTexture9( ppTexture9 ),
							m_pResourceReuseCache( pResourceReuseCache ),
							m_SkipMips( SkipMips )
{
    //*m_ppTexture9 = -1;//NULL;
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
HRESULT WINAPI CTextureProcessor::LockDeviceObject()
{
    HRESULT hr = S_OK;
    m_iNumLockedPtrs = 0;

    if( !m_pResourceReuseCache )
	return E_FAIL;

    // setup the pointers in the process request
    DDS_HEADER* pSurfDesc9 = ( DDS_HEADER* )( ( BYTE* )m_pData + sizeof( DWORD ) );

    UINT Width = pSurfDesc9->dwWidth;
    UINT Height = pSurfDesc9->dwHeight;
    UINT MipLevels = pSurfDesc9->dwMipMapCount;
    if( MipLevels > m_SkipMips )
	MipLevels -= m_SkipMips;
    else
	m_SkipMips = 0;
    if( 0 == MipLevels )
	MipLevels = 1;
    D3DFORMAT Format = GetD3D9Format( pSurfDesc9->ddspf );

    // Skip X number of mip levels
    for( UINT i = 0; i < m_SkipMips; i++ )
    {
	Width = Width >> 1;
	Height = Height >> 1;
    }

    // Find an appropriate resource

    
    {
	m_pRealTexture9 = m_pResourceReuseCache->GetFreeTexture9( Width, Height, MipLevels, ( UINT )Format );

	if( -1 == m_pRealTexture9 )
	{
	    hr = E_TRYAGAIN;
	}
	else
	{
	    // Lock
		m_iNumLockedPtrs = 1;//???
	    //if???(!D3D10) m_iNumLockedPtrs = MipLevels - m_SkipMips;
	    for( UINT i = 0; i < m_iNumLockedPtrs; i++ )
	    {
		m_pLockedRectsBits[i] = 
			IRenderer::GetRendererInstance()->lockTexture(
			m_pRealTexture9 ,
			i,
			*(m_pLockedRectsPitch+i)
		);
		//m_pLockedRectsPitch[i]=IRenderer::GetRendererInstance()->texturePitch(m_pRealTexture9,i);
		if( 0)//FAILED( hr ) )
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
// On D3D9, this unlocks the resource.
//--------------------------------------------------------------------------------------
HRESULT WINAPI CTextureProcessor::UnLockDeviceObject()
{
    HRESULT hr = S_OK;

    if( 0 == m_iNumLockedPtrs )
	return E_FAIL;

    // Find an appropriate resource
	
    {
	// Unlock
	for( UINT i = 0; i < m_iNumLockedPtrs; i++ )
	{
	    IRenderer::GetRendererInstance()->unlockTexture(m_pRealTexture9,i );
	}

	*m_ppTexture9 = m_pRealTexture9;
    }

    return hr;
}

//--------------------------------------------------------------------------------------
HRESULT WINAPI CTextureProcessor::Destroy()
{
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Any texture processing would go here.
//--------------------------------------------------------------------------------------
HRESULT WINAPI CTextureProcessor::Process( void* pData, SIZE_T cBytes )
{
    HRESULT hr = S_OK;

    if( m_pResourceReuseCache )
    {	
	DWORD dwMagicNumber = *( DWORD* )pData;
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
HRESULT WINAPI CTextureProcessor::CopyToResource()
{
    HRESULT hr = S_OK;

#if 0
    
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
void WINAPI CTextureProcessor::SetResourceError()
{
    
    {
	*m_ppTexture9 = ( TextureID )ERROR_RESOURCE_VALUE;
    }
}



//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
CVertexBufferProcessor::CVertexBufferProcessor( 
						VertexBufferID* ppBuffer9,
						UINT iSizeBytes,
						//DWORD Usage,
						//DWORD FVF,
						//D3DPOOL Pool,
						void* pData
						) : 
											     
											     m_ppBuffer9( ppBuffer9 ),
											     m_iSizeBytes(
											     iSizeBytes ),
											     //m_Usage( Usage ),
											     //m_FVF( FVF ),
											     //m_Pool( Pool ),
											     m_pData( pData ),
											     m_pResourceReuseCache(
												 RendererHLSLCg::g_pResourceReuseCache )
{
    //*m_ppBuffer9 = -1;
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
HRESULT WINAPI CVertexBufferProcessor::LockDeviceObject()
{
    HRESULT hr = S_OK;
    if( !m_pResourceReuseCache )
	return E_FAIL;

    
    {
	m_pRealBuffer9 = m_pResourceReuseCache->GetFreeVB9( m_iSizeBytes );
	if( -1 == m_pRealBuffer9 )
	{
	    hr = E_TRYAGAIN;
	}
	else
	{
	    // Lock
	    m_pLockedData = IRenderer::GetRendererInstance()->lockVertexBuffer(m_pRealBuffer9);
	    if( 0)//FAILED( hr ) )
	    {
		m_pLockedData = NULL;
		return hr;
	    }
	}
    }

    return hr;
}

//--------------------------------------------------------------------------------------
// On D3D9, this unlocks the resource.
//--------------------------------------------------------------------------------------
HRESULT WINAPI CVertexBufferProcessor::UnLockDeviceObject()
{
    HRESULT hr = S_OK;

    
    {
	if( !m_pLockedData )
	    return E_FAIL;

	// Unlock
	IRenderer::GetRendererInstance()->unlockVertexBuffer(m_pRealBuffer9);
	*m_ppBuffer9 = m_pRealBuffer9;
    }

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT WINAPI CVertexBufferProcessor::Destroy()
{
    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT WINAPI CVertexBufferProcessor::Process( void* pData, SIZE_T cBytes )
{
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Copies the data to the locked pointer on D3D9
//--------------------------------------------------------------------------------------
HRESULT WINAPI CVertexBufferProcessor::CopyToResource()
{
    
    {
	if( !m_pLockedData )
	{

	//CopyMemory( m_pLockedData, m_pData, m_iSizeBytes );
	IRenderer::GetRendererInstance()->fillVertexBuffer(
		*this->m_ppBuffer9, (BYTE *)m_pData, m_iSizeBytes);


//	    return E_FAIL;
	}

	}

    return S_OK;
}

//--------------------------------------------------------------------------------------
void WINAPI CVertexBufferProcessor::SetResourceError()
{
    
    {
	*m_ppBuffer9 = ( VertexBufferID )ERROR_RESOURCE_VALUE;
    }
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
CIndexBufferProcessor::CIndexBufferProcessor( 
					      IndexBufferID* ppBuffer9,
					      UINT iSizeBytes,
					      //DWORD Usage,
					      int ibFormat,
					      //D3DPOOL Pool,
					      void* pData
					      ) : 
											   
											   m_ppBuffer9( ppBuffer9 ),
											   m_iSizeBytes( iSizeBytes ),
											   //m_Usage( Usage ),
											   m_ibFormat( ibFormat ),
											   //m_Pool( Pool ),
											   m_pData( pData ),
											   m_pResourceReuseCache(
											   RendererHLSLCg::g_pResourceReuseCache )
{
    //*m_ppBuffer9 = -1;//NULL;
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
HRESULT WINAPI CIndexBufferProcessor::LockDeviceObject()
{
    HRESULT hr = S_OK;
    if( !m_pResourceReuseCache )
	return E_FAIL;

   
    {
	m_pRealBuffer9 = m_pResourceReuseCache->GetFreeIB9( m_iSizeBytes, m_ibFormat );
	if( -1 == m_pRealBuffer9 )
	{
	    hr = E_TRYAGAIN;
	}
	else
	{
	    // Lock
	    m_pLockedData = IRenderer::GetRendererInstance()->lockIndexBuffer(m_pRealBuffer9);
	    if( 0)//FAILED( hr ) )
	    {
		m_pLockedData = NULL;
		return hr;
	    }
	}
    }

    return hr;
}

//--------------------------------------------------------------------------------------
// On D3D9, this unlocks the resource.
//--------------------------------------------------------------------------------------
HRESULT WINAPI CIndexBufferProcessor::UnLockDeviceObject()
{
    HRESULT hr = S_OK;

    
    {
	if( !m_pLockedData )
	    return E_FAIL;

	// Unlock
	IRenderer::GetRendererInstance()->unlockIndexBuffer(m_pRealBuffer9);
	*m_ppBuffer9 = m_pRealBuffer9;
    }

    return hr;
}

//--------------------------------------------------------------------------------------
HRESULT WINAPI CIndexBufferProcessor::Destroy()
{
    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT WINAPI CIndexBufferProcessor::Process( void* pData, SIZE_T cBytes )
{
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Copies the data to the locked pointer on D3D9
//--------------------------------------------------------------------------------------
HRESULT WINAPI CIndexBufferProcessor::CopyToResource()
{
    
	if( !m_pLockedData )
	{

	//CopyMemory( m_pLockedData, m_pData, m_iSizeBytes );
	IRenderer::GetRendererInstance()->fillIndexBuffer(
		*this->m_ppBuffer9, (BYTE *)m_pData, m_iSizeBytes);



//	    return E_FAIL;
	}
    return S_OK;
}

//--------------------------------------------------------------------------------------
void WINAPI CIndexBufferProcessor::SetResourceError()
{
    
    {
	*m_ppBuffer9 = ( IndexBufferID )ERROR_RESOURCE_VALUE;
    }
}
#if 0
//--------------------------------------------------------------------------------------
// SDKMesh
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
CSDKMeshLoader::CSDKMeshLoader( CDXUTSDKMesh* pMesh,  LPCTSTR szFileName,
				bool bCreateAdjacencyIndices, SDKMESH_CALLBACKS9* pLoaderCallbacks ) : m_pMesh(
												       pMesh ),
												       m_Device(
												       pDev9 ),
												       m_bCreateAdjacencyIndices( bCreateAdjacencyIndices ),
												       m_pLoaderCallbacks9( pLoaderCallbacks )
{
    stx_strlcpy( m_szFileName, szFileName,strlen(szFileName)+1 );
}

//--------------------------------------------------------------------------------------
CSDKMeshLoader::~CSDKMeshLoader()
{
    Destroy();
}

//--------------------------------------------------------------------------------------
HRESULT WINAPI CSDKMeshLoader::Decompress( void** ppData, SIZE_T* pcBytes )
{
    *ppData = NULL;
    *pcBytes = 0;
    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT WINAPI CSDKMeshLoader::Destroy()
{
    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT WINAPI CSDKMeshLoader::Load()
{
    HRESULT hr = E_FAIL;

    if( m_pMesh )
    {
	
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
HRESULT WINAPI CSDKMeshProcessor::LockDeviceObject()
{
    return S_OK;
}
HRESULT WINAPI CSDKMeshProcessor::UnLockDeviceObject()
{
    return S_OK;
}
HRESULT WINAPI CSDKMeshProcessor::Destroy()
{
    return S_OK;
}
HRESULT WINAPI CSDKMeshProcessor::Process( void* pData, SIZE_T cBytes )
{
    return S_OK;
}
HRESULT WINAPI CSDKMeshProcessor::CopyToResource()
{
    return S_OK;
}
void	WINAPI CSDKMeshProcessor::SetResourceError()
{
}
#endif
#endif
//--------------------------------------------------------------------------------------
CTextureLoader::CTextureLoader( CHAR* szFileName, CPackedFile* pPackedFile ) : m_pData( NULL ),
										m_cBytes( 0 ),
										m_pPackedFile( pPackedFile )
{
    stx_strlcpy( m_szFileName, szFileName,strlen(szFileName)+1 );
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
HRESULT WINAPI CTextureLoader::Decompress( void** ppData, SIZE_T* pcBytes )
{
    *ppData = ( void* )m_pData;
    *pcBytes = m_cBytes;
    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT WINAPI CTextureLoader::Destroy()
{
    if( !m_pPackedFile->UsingMemoryMappedIO() )
    {
	SAFE_DELETE_ARRAY( m_pData );
    }
    m_cBytes = 0;

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Load the texture from the packed file.  If not-memory mapped, allocate enough memory
// to hold the data.
//--------------------------------------------------------------------------------------
HRESULT WINAPI CTextureLoader::Load()
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
	    return E_OUTOFMEMORY;

	if( !m_pPackedFile->GetPackedFile( m_szFileName, &m_pData, &m_cBytes ) )
	    return E_FAIL;
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
CVertexBufferLoader::CVertexBufferLoader()
{
}
CVertexBufferLoader::~CVertexBufferLoader()
{
    Destroy();
}
HRESULT WINAPI CVertexBufferLoader::Decompress( void** ppData, SIZE_T* pcBytes )
{
    //???*ppData = NULL; *pcBytes = 0; 
	return S_OK;
}
HRESULT WINAPI CVertexBufferLoader::Destroy()
{
    return S_OK;
}
HRESULT WINAPI CVertexBufferLoader::Load()
{
    return S_OK;
}
//--------------------------------------------------------------------------------------
CIndexBufferLoader::CIndexBufferLoader()
{
}
CIndexBufferLoader::~CIndexBufferLoader()
{
    Destroy();
}
HRESULT WINAPI CIndexBufferLoader::Decompress( void** ppData, SIZE_T* pcBytes )
{
    //???*ppData = NULL; *pcBytes = 0; 
	return S_OK;
}
HRESULT WINAPI CIndexBufferLoader::Destroy()
{
    return S_OK;
}
HRESULT WINAPI CIndexBufferLoader::Load()
{
    return S_OK;
}
#endif
#endif

