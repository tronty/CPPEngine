#ifdef _WIN32
#include <SGX/SGXCPP.h>

#ifdef BUILDCONTENTSTREAMING
//--------------------------------------------------------------------------------------
// File: ResourceReuseCache.cpp
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <SGX/SGXCPP.h>

#include "ResourceReuseCache.h"


//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define ISBITMASK( r,g,b,a ) ( ddpf.dwRBitMask == r && ddpf.dwGBitMask == g && ddpf.dwBBitMask == b && ddpf.dwABitMask == a )

//--------------------------------------------------------------------------------------
// Texture functions
//--------------------------------------------------------------------------------------

VArray<DEVICE_TEXTURE*> 		CResourceReuseCache::m_TextureList;
VArray<DEVICE_VERTEX_BUFFER*>	CResourceReuseCache::m_VBList;
VArray<DEVICE_INDEX_BUFFER*>	CResourceReuseCache::m_IBList;
UINT64									CResourceReuseCache::m_MaxManagedMemory=1024 * 1024 * 32;
UINT64									CResourceReuseCache::m_UsedManagedMemory=0;
BOOL									CResourceReuseCache::m_bSilent=FALSE;
BOOL									CResourceReuseCache::m_bDontCreateResources;
//RendererHLSLCg* CResourceReuseCache::m_r=0;
//--------------------------------------------------------------------------------------
int CResourceReuseCache::FindTexture( TextureID pTex9 )
{
    for( int i = 0; i < m_TextureList.GetSize(); i++ )
    {
        DEVICE_TEXTURE* texTest = m_TextureList.GetAt( i );
        if( texTest->pTexture9 == pTex9 )
            return i;
    }

    return -1;
}

//--------------------------------------------------------------------------------------
#if 0
DXGI_FORMAT ConvertToDXGI_FORMAT( D3DFORMAT d3dformat )
{
    switch( d3dformat )
    {
        case D3DFMT_A32B32G32R32F:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;

        case D3DFMT_A16B16G16R16:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case D3DFMT_A16B16G16R16F:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case D3DFMT_G32R32F:
            return DXGI_FORMAT_R32G32_FLOAT;

        case D3DFMT_R8G8B8:
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;

        case D3DFMT_G16R16:
        case D3DFMT_V16U16:
            return DXGI_FORMAT_R16G16_UNORM;

        case D3DFMT_G16R16F:
            return DXGI_FORMAT_R16G16_FLOAT;
        case D3DFMT_R32F:
            return DXGI_FORMAT_R32_FLOAT;

        case D3DFMT_R16F:
            return DXGI_FORMAT_R16_FLOAT;

        case D3DFMT_A8:
            return DXGI_FORMAT_A8_UNORM;
        case D3DFMT_P8:
        case D3DFMT_L8:
            return DXGI_FORMAT_R8_UNORM;

        case D3DFMT_DXT1:
            return DXGI_FORMAT_BC1_UNORM;
        case D3DFMT_DXT2:
            return DXGI_FORMAT_BC1_UNORM_SRGB;
        case D3DFMT_DXT3:
            return DXGI_FORMAT_BC2_UNORM;
        case D3DFMT_DXT4:
            return DXGI_FORMAT_BC2_UNORM_SRGB;
        case D3DFMT_DXT5:
            return DXGI_FORMAT_BC3_UNORM;

        default:
            assert( FALSE ); // unhandled format
            return DXGI_FORMAT_UNKNOWN;
    }
}


//--------------------------------------------------------------------------------------
D3DFORMAT ConvertToD3DFORMAT( DXGI_FORMAT dxgiformat )
{
    switch( dxgiformat )
    {

        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return D3DFMT_A32B32G32R32F;

        case DXGI_FORMAT_R16G16B16A16_UNORM:
            return D3DFMT_A16B16G16R16;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            return D3DFMT_A16B16G16R16F;
        case DXGI_FORMAT_R32G32_FLOAT:
            return D3DFMT_G32R32F;

        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return D3DFMT_A8R8G8B8;

        case DXGI_FORMAT_R16G16_UNORM:
            return D3DFMT_G16R16;

        case DXGI_FORMAT_R16G16_FLOAT:
            return D3DFMT_G16R16F;
        case DXGI_FORMAT_R32_FLOAT:
            return D3DFMT_R32F;

        case DXGI_FORMAT_R16_FLOAT:
            return D3DFMT_R16F;

        case DXGI_FORMAT_A8_UNORM:
            return D3DFMT_A8;
        case DXGI_FORMAT_R8_UNORM:
            return D3DFMT_L8;

        case DXGI_FORMAT_BC1_UNORM:
            return D3DFMT_DXT1;
        case DXGI_FORMAT_BC2_UNORM:
            return D3DFMT_DXT3;
        case DXGI_FORMAT_BC3_UNORM:
            return D3DFMT_DXT5;

        default:
            assert( FALSE ); // unhandled format
            return D3DFMT_UNKNOWN;
    }
}

#endif
//--------------------------------------------------------------------------------------
int CResourceReuseCache::EnsureFreeTexture( UINT Width, UINT Height, UINT MipLevels, UINT Format )
{
    // see if we have a free one available
    for( int i = 0; i < m_TextureList.GetSize(); i++ )
    {
        DEVICE_TEXTURE* texTest = m_TextureList.GetAt( i );
        if( !texTest->bInUse )
        {
            if( texTest->Width == Width &&
                texTest->Height == Height &&
                texTest->MipLevels == MipLevels &&
                texTest->Format == Format )
            {
                // Found one that matches all criteria
                texTest->bInUse = TRUE;
                return i;
            }
        }
    }

    // haven't found a free one
    // try to create a new one
    UINT64 newSize = GetEstimatedSize( Width, Height, MipLevels, Format );
    UINT64 sizeNeeded = m_UsedManagedMemory + newSize;
    if( sizeNeeded > m_MaxManagedMemory )
        DestroyLRUResources( newSize );

    if( !m_bDontCreateResources )
    {
        HRESULT hr = S_OK;
        DEVICE_TEXTURE* tex = new DEVICE_TEXTURE;
        tex->Width = Width;
        tex->Height = Height;
        tex->MipLevels = MipLevels;
        tex->Format = Format;
        tex->EstimatedSize = newSize;
        tex->pTexture9 = -1;
        tex->RecentUseCounter = 0;

        if( !m_bSilent )
            LOG_PRINT( "RESOURCE WARNING: Device needs to create new Texture\n" );


        {
#if 0
			hr = m_Device.pDev9->CreateTexture( Width,
                                                Height,
                                                MipLevels,
                                                0,
                                                ( D3DFORMAT )Format,
                                                D3DPOOL_MANAGED,
                                                &tex->pTexture9,
                                                NULL );
#else
TextureID id=-1;

			id=m_r->addTexture(Width,
                                                Height,
                                                MipLevels>0,

                                                FORMAT3FromD3D (Format),

m_r->GettrilinearAniso(),
0.0f,
0//DYNAMIC

												);
tex->pTexture9=id;
LOG_PRINT("Added tex->pTexture9=%d\n",tex->pTexture9);

#if 0
/*((RendererHLSLD3D9*)*/m_r->textureProcessors[/*((RendererHLSLD3D9*)*/m_r->textureProcessors.size()-1]. m_ppTexture9
=(IDirect3DTexture9 **)
&/*((RendererHLSLD3D9*)*/m_r->textures[tex->pTexture9].texture;
#endif
#endif
		}

        if( FAILED( hr ) )
        {
            SAFE_DELETE( tex );
            if( !m_bSilent )
                LOG_PRINT( "RESOURCE ERROR: Cannot Load Texture!\n" );
            return -1;
        }

        m_UsedManagedMemory += tex->EstimatedSize;
        tex->bInUse = TRUE;

        int index = m_TextureList.GetSize();
        m_TextureList.Add( tex );



        return index;
    }

    return -1;
}

//--------------------------------------------------------------------------------------
// Return the BPP for a particular format
//--------------------------------------------------------------------------------------
UINT BitsPerPixel( D3DFORMAT fmt )
{
    switch( fmt )
    {
        case D3DFMT_A32B32G32R32F:
            return 128;

        case D3DFMT_A16B16G16R16:
        case D3DFMT_Q16W16V16U16:
        case D3DFMT_A16B16G16R16F:
        case D3DFMT_G32R32F:
            return 64;

        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
        case D3DFMT_A2B10G10R10:
        case D3DFMT_A8B8G8R8:
        case D3DFMT_X8B8G8R8:
        case D3DFMT_G16R16:
        case D3DFMT_A2R10G10B10:
        case D3DFMT_Q8W8V8U8:
        case D3DFMT_V16U16:
        case D3DFMT_X8L8V8U8:
        case D3DFMT_A2W10V10U10:
        case D3DFMT_D32:
        case D3DFMT_D24S8:
        case D3DFMT_D24X8:
        case D3DFMT_D24X4S4:
        case D3DFMT_D32F_LOCKABLE:
        case D3DFMT_D24FS8:
        case D3DFMT_INDEX32:
        case D3DFMT_G16R16F:
        case D3DFMT_R32F:
            return 32;

        case D3DFMT_R8G8B8:
            return 24;

        case D3DFMT_A4R4G4B4:
        case D3DFMT_X4R4G4B4:
        case D3DFMT_R5G6B5:
        case D3DFMT_L16:
        case D3DFMT_A8L8:
        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5:
        case D3DFMT_A8R3G3B2:
        case D3DFMT_V8U8:
        case D3DFMT_CxV8U8:
        case D3DFMT_L6V5U5:
        case D3DFMT_G8R8_G8B8:
        case D3DFMT_R8G8_B8G8:
        case D3DFMT_D16_LOCKABLE:
        case D3DFMT_D15S1:
        case D3DFMT_D16:
        case D3DFMT_INDEX16:
        case D3DFMT_R16F:
        case D3DFMT_YUY2:
            return 16;

        case D3DFMT_R3G3B2:
        case D3DFMT_A8:
        case D3DFMT_A8P8:
        case D3DFMT_P8:
        case D3DFMT_L8:
        case D3DFMT_A4L4:
            return 8;

        case D3DFMT_DXT1:
            return 4;
        case D3DFMT_DXT2:
        case D3DFMT_DXT3:
        case D3DFMT_DXT4:
        case D3DFMT_DXT5:
            return  8;

        default:
            assert( FALSE ); // unhandled format
            return 0;
    }
}

//--------------------------------------------------------------------------------------
// Get surface information for a particular format
//--------------------------------------------------------------------------------------
void GetSurfaceInfo( UINT width, UINT height, D3DFORMAT fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows )
{
    UINT numBytes = 0;
    UINT rowBytes = 0;
    UINT numRows = 0;

    // From the DXSDK docs:
    //
    //     When computing DXTn compressed sizes for non-square textures, the
    //     following formula should be used at each mipmap level:
    //
    //         max(1, width ÷ 4) x max(1, height ÷ 4) x 8(DXT1) or 16(DXT2-5)
    //
    //     The pitch for DXTn formats is different from what was returned in
    //     Microsoft DirectX 7.0. It now refers the pitch of a row of blocks.
    //     For example, if you have a width of 16, then you will have a pitch
    //     of four blocks (4*8 for DXT1, 4*16 for DXT2-5.)"

    if( fmt == D3DFMT_DXT1 || fmt == D3DFMT_DXT2 || fmt == D3DFMT_DXT3 || fmt == D3DFMT_DXT4 || fmt == D3DFMT_DXT5 )
    {
        // Note: we support width and/or height being 0 in order to compute
        // offsets in functions like CBufferLockEntry::CopyBLEToPerfectSizedBuffer().
        int numBlocksWide = 0;
        if( width > 0 )
            numBlocksWide = std::max<unsigned int>( 1, width / 4 );
        int numBlocksHigh = 0;
        if( height > 0 )
            numBlocksHigh = std::max<unsigned int>( 1, height / 4 );
        //int numBlocks = numBlocksWide * numBlocksHigh;
        int numBytesPerBlock = ( fmt == D3DFMT_DXT1 ? 8 : 16 );
        rowBytes = numBlocksWide * numBytesPerBlock;
        numRows = numBlocksHigh;
    }
    else
    {
        UINT bpp = BitsPerPixel( fmt );
        rowBytes = ( width * bpp + 7 ) / 8; // round up to nearest byte
        numRows = height;
    }
    numBytes = rowBytes * numRows;
    if( pNumBytes != NULL )
        *pNumBytes = numBytes;
    if( pRowBytes != NULL )
        *pRowBytes = rowBytes;
    if( pNumRows != NULL )
        *pNumRows = numRows;
}

//--------------------------------------------------------------------------------------
D3DFORMAT GetD3D9Format( DDS_PIXELFORMAT ddpf )
{
    // See DDSTextureLoader for a more complete example of this...

    if( ddpf.dwFlags & DDS_RGB )	//rgb codes
        // Only do the more common formats
    {
        if( 32 == ddpf.dwRGBBitCount )
        {
            if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0xff000000) )
                return D3DFMT_A8R8G8B8;
            if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) )
                return D3DFMT_X8R8G8B8;
            if( ISBITMASK(0x000000ff,0x00ff0000,0x0000ff00,0xff000000) )
                return D3DFMT_A8B8G8R8;
            if( ISBITMASK(0x000000ff,0x00ff0000,0x0000ff00,0x00000000) )
                return D3DFMT_X8B8G8R8;
            if( ISBITMASK(0xffffffff,0x00000000,0x00000000,0x00000000) )
                return D3DFMT_R32F;
        }

        if( 24 == ddpf.dwRGBBitCount )
        {
            if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) )
                return D3DFMT_R8G8B8;
        }

        if( 16 == ddpf.dwRGBBitCount )
        {
            if( ISBITMASK(0x0000F800,0x000007E0,0x0000001F,0x00000000) )
                return D3DFMT_R5G6B5;
        }
    }
    else if( ddpf.dwFlags & DDS_LUMINANCE )
    {
        if( 8 == ddpf.dwRGBBitCount )
        {
            return D3DFMT_L8;
        }
    }
    else if( ddpf.dwFlags & DDS_ALPHA )
    {
        if( 8 == ddpf.dwRGBBitCount )
        {
            return D3DFMT_A8;
        }
    }
    else if( ddpf.dwFlags & DDS_FOURCC )  //fourcc codes (dxtn)
    {
        if( MAKEFOURCC( 'D', 'X', 'T', '1' ) == ddpf.dwFourCC )
            return D3DFMT_DXT1;
        if( MAKEFOURCC( 'D', 'X', 'T', '2' ) == ddpf.dwFourCC )
            return D3DFMT_DXT2;
        if( MAKEFOURCC( 'D', 'X', 'T', '3' ) == ddpf.dwFourCC )
            return D3DFMT_DXT3;
        if( MAKEFOURCC( 'D', 'X', 'T', '4' ) == ddpf.dwFourCC )
            return D3DFMT_DXT4;
        if( MAKEFOURCC( 'D', 'X', 'T', '5' ) == ddpf.dwFourCC )
            return D3DFMT_DXT5;
    }

    return D3DFMT_UNKNOWN;
}

//--------------------------------------------------------------------------------------
UINT64 CResourceReuseCache::GetEstimatedSize( UINT Width, UINT Height, UINT MipLevels, UINT Format )
{
    UINT64 SizeTotal = 0;
    UINT Size = 0;

    while( Width > 0 && Height > 0 && MipLevels > 0 )
    {
        GetSurfaceInfo( Width, Height, ( D3DFORMAT )Format, &Size, NULL, NULL );

        SizeTotal += Size;
        Width = Width >> 1;
        Height = Height >> 1;
        MipLevels --;
    }

    return SizeTotal;
}

//--------------------------------------------------------------------------------------
// Vertex Buffer functions
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
int CResourceReuseCache::FindVB( VertexBufferID pVB )
{
    for( int i = 0; i < m_VBList.GetSize(); i++ )
    {
        DEVICE_VERTEX_BUFFER* vb = m_VBList.GetAt( i );
        if( vb->pVB9 == pVB )
            return i;
    }

    return -1;
}

//--------------------------------------------------------------------------------------
int CResourceReuseCache::EnsureFreeVB( const IDataProcessor* p, UINT iSizeBytes )
{
    // Find the closest match
    int closestindex = -1;
    UINT smallestSize = UINT_MAX;
    for( int i = 0; i < m_VBList.GetSize(); i++ )
    {
        DEVICE_VERTEX_BUFFER* vb = m_VBList.GetAt( i );
        if( !vb->bInUse )
        {
            if( vb->iSizeBytes == iSizeBytes )
            {
                smallestSize = vb->iSizeBytes;
                closestindex = i;
                break;
            }
        }
    }

    // if we found a closest match, return it
    if( -1 != closestindex )
    {
        DEVICE_VERTEX_BUFFER* vb = m_VBList.GetAt( closestindex );
        vb->bInUse = TRUE;
        return closestindex;
    }

    // haven't found a free one
    // try to create a new one
    UINT64 newSize = iSizeBytes;
    UINT64 sizeNeeded = m_UsedManagedMemory + newSize;
    if( sizeNeeded > m_MaxManagedMemory )//536/432/DX9
        DestroyLRUResources( newSize );//3436/432/DX10

    if( !m_bDontCreateResources )
    {
        HRESULT hr = S_OK;
        DEVICE_VERTEX_BUFFER* vb = new DEVICE_VERTEX_BUFFER;
        vb->iSizeBytes = iSizeBytes;
        vb->pVB9 = -1;
        vb->RecentUseCounter = 0;

        if( !m_bSilent )
            LOG_PRINT( "RESOURCE WARNING: Device needs to create new Vertex Buffer\n" );


        {
#if 0
            hr = m_Device.pDev9->CreateVertexBuffer( iSizeBytes,
                                                     D3DUSAGE_WRITEONLY,
                                                     0,
                                                     D3DPOOL_MANAGED,
													 &vb->pVB9,
                                                     NULL );
#else

VertexBufferID id=-1;
	LOG_PRINT("m_r=%d\n",m_r);
			id=m_r->addVertexBuffer(iSizeBytes,
							  //DEFAULT,//
							  DYNAMIC ,
							  0, -1);//,p);
vb->pVB9=id;
LOG_PRINT("Added vb->pVB9=%d\n",vb->pVB9);

#if 0
/*((RendererHLSLD3D9*)*/m_r->vertexBufferProcessors[
	/*((RendererHLSLD3D9*)*/m_r->vertexBufferProcessors.size()-1
].m_ppBuffer9=&/*((RendererHLSLD3D9*)*/m_r->vertexBuffers[vb->pVB9].vertexBuffer;
#endif
#endif
        }

        if( FAILED( hr ) )
        {
            SAFE_DELETE( vb );
            if( !m_bSilent )
                LOG_PRINT( "RESOURCE ERROR: Cannot Load Vertex Buffer!\n" );
            return -1;
        }

        m_UsedManagedMemory += vb->iSizeBytes;
        vb->bInUse = TRUE;
        int index = m_VBList.GetSize();
        m_VBList.Add( vb );
        return index;
    }

    return -1;
}

//--------------------------------------------------------------------------------------
// Index Buffer
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
int CResourceReuseCache::FindIB( IndexBufferID pIB )
{
    for( int i = 0; i < m_IBList.GetSize(); i++ )
    {
        DEVICE_INDEX_BUFFER* IB = m_IBList.GetAt( i );
        if( IB->pIB9 == pIB )
            return i;
    }

    return -1;
}

//--------------------------------------------------------------------------------------
int CResourceReuseCache::EnsureFreeIB(const  IDataProcessor* p,UINT iSizeBytes, UINT ibFormat )
{
    // Find the closest match
    int closestindex = -1;
    UINT smallestSize = UINT_MAX;
    for( int i = 0; i < m_IBList.GetSize(); i++ )
    {
        DEVICE_INDEX_BUFFER* IB = m_IBList.GetAt( i );
        if( !IB->bInUse )
        {
            if( 0 == ibFormat )
                ibFormat = IB->ibFormat;
            if( IB->iSizeBytes == iSizeBytes && IB->ibFormat == ibFormat )
            {
                smallestSize = IB->iSizeBytes;
                closestindex = i;
                break;
            }
        }
    }

    // if we found a closest match, return it
    if( -1 != closestindex )
    {
        DEVICE_INDEX_BUFFER* IB = m_IBList.GetAt( closestindex );
        IB->bInUse = TRUE;
        return closestindex;
    }

    // We haven't found a free one, so create a new one
    UINT64 newSize = iSizeBytes;
    UINT64 sizeNeeded = m_UsedManagedMemory + newSize;
    if( sizeNeeded > m_MaxManagedMemory )
        DestroyLRUResources( newSize );

    if( !m_bDontCreateResources )
    {
        HRESULT hr = S_OK;
        DEVICE_INDEX_BUFFER* IB = new DEVICE_INDEX_BUFFER;
        IB->iSizeBytes = iSizeBytes;
        IB->ibFormat = ibFormat;
        IB->pIB9 = -1;
        IB->RecentUseCounter = 0;

        if( !m_bSilent )
            LOG_PRINT( "RESOURCE WARNING: Device needs to create new Index Buffer\n" );


        {
#if 0
			hr = m_Device.pDev9->CreateIndexBuffer( iSizeBytes,
                                                    D3DUSAGE_WRITEONLY,
                                                    ( D3DFORMAT )ibFormat,
                                                    D3DPOOL_MANAGED,
                                                    &IB->pIB9,
                                                    NULL );
#else
int nBytes=iSizeBytes/sizeof(WORD);

IndexBufferID id=-1;

			id=m_r->addIndexBuffer(nBytes,sizeof(WORD),
							 //DEFAULT,//
							 DYNAMIC ,
							 0, -1);//,p);//D3DFMT_INDEX16
IB->pIB9=id;
LOG_PRINT("Added IB->pIB9=%d\n",IB->pIB9);
#if 0
/*((RendererHLSLD3D9*)*/m_r->indexBufferProcessors[
	/*((RendererHLSLD3D9*)*/m_r->indexBufferProcessors.size()-1
].m_ppBuffer9
=&/*((RendererHLSLD3D9*)*/m_r->indexBuffers[IB->pIB9].indexBuffer;
#endif
#endif
		}

        if( FAILED( hr ) )
        {
            SAFE_DELETE( IB );
            if( !m_bSilent )
                LOG_PRINT( "RESOURCE ERROR: Cannot Load Index Buffer!\n" );
            return -1;
        }

        m_UsedManagedMemory += IB->iSizeBytes;
        IB->bInUse = TRUE;
        int index = m_IBList.GetSize();
        m_IBList.Add( IB );
        return index;
    }

    return -1;
}

//--------------------------------------------------------------------------------------
void CResourceReuseCache::DestroyTexture9( DEVICE_TEXTURE* pTex )
{
    UINT iCount = m_r->deleteTexture(pTex->pTexture9);
    while( iCount )
        iCount = m_r->deleteTexture(pTex->pTexture9);
    SAFE_DELETE( pTex );
}

//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
void CResourceReuseCache::DestroyVB9( DEVICE_VERTEX_BUFFER* pVB )
{
    UINT iCount = m_r->deleteVertexBuffer(pVB->pVB9);
    while( iCount )
        iCount = m_r->deleteVertexBuffer(pVB->pVB9);
    SAFE_DELETE( pVB );
}

//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
void CResourceReuseCache::DestroyIB9( DEVICE_INDEX_BUFFER* pIB )
{
    UINT iCount = m_r->deleteIndexBuffer(pIB->pIB9);
    while( iCount )
        iCount = m_r->deleteIndexBuffer(pIB->pIB9);
    SAFE_DELETE( pIB );
}

//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// publics
//--------------------------------------------------------------------------------------


CResourceReuseCache::CResourceReuseCache(IRenderer* r) /*: m_r(r),m_MaxManagedMemory( 1024 * 1024 * 32 ),
                                                                     m_UsedManagedMemory( 0 ),

                                                                     m_bSilent( FALSE )*/
{
	m_r=r;
}

//--------------------------------------------------------------------------------------
CResourceReuseCache::~CResourceReuseCache()
{
    OnDestroy();
}

//--------------------------------------------------------------------------------------
void CResourceReuseCache::SetMaxManagedMemory( UINT64 MaxMemory )
{
    m_MaxManagedMemory = 536870912;//512*1024000;//???MaxMemory;
    //m_MaxManagedMemory = 32*1024000;//???MaxMemory;
}

//--------------------------------------------------------------------------------------
UINT64 CResourceReuseCache::GetMaxManagedMemory()
{
    return m_MaxManagedMemory;
}

//--------------------------------------------------------------------------------------
UINT64 CResourceReuseCache::GetUsedManagedMemory()
{
    return m_UsedManagedMemory;
}

//--------------------------------------------------------------------------------------
void CResourceReuseCache::SetDontCreateResources( BOOL bDontCreateResources )
{
    m_bDontCreateResources = bDontCreateResources;
}

//--------------------------------------------------------------------------------------
UINT64 CResourceReuseCache::DestroyLRUTexture()
{
	if(!m_TextureList.size()) return 0;
    DEVICE_TEXTURE* pLRURes = m_TextureList.GetAt( 0 );
    UINT64 SizeGain = 0;

    int iIndex = 0;
    int NumResources = m_TextureList.GetSize();
    for( int i = 0; i < NumResources; i++ )
    {
        DEVICE_TEXTURE* pRes = m_TextureList.GetAt( i );
        if( pRes->RecentUseCounter < pLRURes->RecentUseCounter )
        {
            pLRURes = pRes;
            iIndex = i;
        }
    }

    m_UsedManagedMemory -= pLRURes->EstimatedSize;
    SizeGain = pLRURes->EstimatedSize;


        DestroyTexture9( pLRURes );

    return SizeGain;

}

//--------------------------------------------------------------------------------------
UINT64 CResourceReuseCache::DestroyLRUVB()
{
    if(!m_VBList.size()) return 0;
    DEVICE_VERTEX_BUFFER* pLRURes = m_VBList.GetAt( 0 );
    UINT64 SizeGain = 0;

    int iIndex = 0;
    int NumResources = m_VBList.GetSize();
    for( int i = 0; i < NumResources; i++ )
    {
        DEVICE_VERTEX_BUFFER* pRes = m_VBList.GetAt( i );
        if( pRes->RecentUseCounter < pLRURes->RecentUseCounter )
        {
            pLRURes = pRes;
            iIndex = i;
        }
    }

    m_UsedManagedMemory -= pLRURes->iSizeBytes;
    SizeGain = pLRURes->iSizeBytes;


        DestroyVB9( pLRURes );

    return SizeGain;
}

//--------------------------------------------------------------------------------------
UINT64 CResourceReuseCache::DestroyLRUIB()
{
    if(!m_IBList.size()) return 0;
    DEVICE_INDEX_BUFFER* pLRURes = m_IBList.GetAt( 0 );
    UINT64 SizeGain = 0;

    int iIndex = 0;
    int NumResources = m_IBList.GetSize();
    for( int i = 0; i < NumResources; i++ )
    {
        DEVICE_INDEX_BUFFER* pRes = m_IBList.GetAt( i );
        if( pRes->RecentUseCounter < pLRURes->RecentUseCounter )
        {
            pLRURes = pRes;
            iIndex = i;
        }
    }

    m_UsedManagedMemory -= pLRURes->iSizeBytes;
    SizeGain = pLRURes->iSizeBytes;


        DestroyIB9( pLRURes );

    return SizeGain;
}

//--------------------------------------------------------------------------------------
void CResourceReuseCache::DestroyLRUResources( UINT64 SizeGainNeeded )
{
	int n=m_TextureList.size()+m_VBList.size()+m_IBList.size();
    UINT64 ReleasedSize = 0;
    while( ReleasedSize < SizeGainNeeded&&n )
    {
		n=m_TextureList.size()+m_VBList.size()+m_IBList.size();
        ReleasedSize += DestroyLRUTexture();
        if( ReleasedSize > SizeGainNeeded )
            return;

        ReleasedSize += DestroyLRUVB();
        if( ReleasedSize > SizeGainNeeded )
            return;

        ReleasedSize += DestroyLRUIB();
        if( ReleasedSize > SizeGainNeeded )
            return;
    }
}

//--------------------------------------------------------------------------------------
// Texture functions
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
TextureID CResourceReuseCache::GetFreeTexture9( UINT Width, UINT Height, UINT MipLevels, UINT Format )
{
    int iTex = EnsureFreeTexture( Width, Height, MipLevels, Format );
    if( -1 == iTex )
        return -1;
    else
    {
        return m_TextureList.GetAt( iTex )->pTexture9;
    }
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void CResourceReuseCache::UnuseDeviceTexture9( TextureID pTexture )
{
    int index = FindTexture( pTexture );
    if( index >= 0 )
    {
        DEVICE_TEXTURE* tex = m_TextureList.GetAt( index );
        tex->bInUse = FALSE;
    }
}

//--------------------------------------------------------------------------------------
int CResourceReuseCache::GetNumTextures()
{
    return m_TextureList.GetSize();
}

//--------------------------------------------------------------------------------------
DEVICE_TEXTURE* CResourceReuseCache::GetTexture( int i )
{
    return m_TextureList.GetAt( i );
}

//--------------------------------------------------------------------------------------
// Vertex Buffer functions
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
VertexBufferID CResourceReuseCache::GetFreeVB9( const IDataProcessor* p, UINT sizeBytes )
{
    int iVB = EnsureFreeVB( p,sizeBytes );
    if( -1 == iVB )
        return -1;
    else
        return m_VBList.GetAt( iVB )->pVB9;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void CResourceReuseCache::UnuseDeviceVB9( VertexBufferID pVB )
{
    int index = FindVB( pVB );
    if( index >= 0 )
    {
        DEVICE_VERTEX_BUFFER* vb = m_VBList.GetAt( index );
        vb->bInUse = FALSE;
    }
}

//--------------------------------------------------------------------------------------
int CResourceReuseCache::GetNumVBs()
{
    return m_VBList.GetSize();
}

//--------------------------------------------------------------------------------------
DEVICE_VERTEX_BUFFER* CResourceReuseCache::GetVB( int i )
{
    return m_VBList.GetAt( i );
}

//--------------------------------------------------------------------------------------
// Index Buffer functions
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
IndexBufferID CResourceReuseCache::GetFreeIB9( const IDataProcessor* p,UINT sizeBytes, UINT ibFormat )
{
    int iIB = EnsureFreeIB( p,sizeBytes, ibFormat );
    if( -1 == iIB )
        return -1;
    else
        return m_IBList.GetAt( iIB )->pIB9;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void CResourceReuseCache::UnuseDeviceIB9( IndexBufferID pIB )
{
    int index = FindIB( pIB );
    if( index >= 0 )
    {
        DEVICE_INDEX_BUFFER* IB = m_IBList.GetAt( index );
        IB->bInUse = FALSE;
    }
}

//--------------------------------------------------------------------------------------
int CResourceReuseCache::GetNumIBs()
{
    return m_IBList.GetSize();
}

//--------------------------------------------------------------------------------------
DEVICE_INDEX_BUFFER* CResourceReuseCache::GetIB( int i )
{
    return m_IBList.GetAt( i );
}

//--------------------------------------------------------------------------------------
void CResourceReuseCache::OnDestroy()
{
    int TexListSize = m_TextureList.GetSize();
    int VBListSize = m_VBList.GetSize();
    int IBListSize = m_IBList.GetSize();


    {
        for( int i = 0; i < TexListSize; i++ )
        {
            DEVICE_TEXTURE* tex = m_TextureList.GetAt( i );
            DestroyTexture9( tex );
        }
        for( int i = 0; i < VBListSize; i++ )
        {
            DEVICE_VERTEX_BUFFER* vb = m_VBList.GetAt( i );
            DestroyVB9( vb );
        }
        for( int i = 0; i < IBListSize; i++ )
        {
            DEVICE_INDEX_BUFFER* IB = m_IBList.GetAt( i );
            DestroyIB9( IB );
        }
    }

    m_TextureList.RemoveAll();
    m_VBList.RemoveAll();
    m_IBList.RemoveAll();

    m_UsedManagedMemory = 0;
}
/*
D3D10: ERROR: ID3D10Texture2D::Map: Map cannot be called with MAP_WRITE access,
because the Resource was created as D3D10_USAGE_DYNAMIC.
D3D10_USAGE_DYNAMIC Resources
must use either MAP_WRITE_DISCARD or MAP_WRITE_NO_OVERWRITE with Map.
[ RESOURCE_MANIPULATION ERROR #307: TEXTURE2D_MAP_INVALIDMAPTYPE ]

D3D10: ERROR: ID3D10Texture2D::Map: Subresource requested
(11) is greater than or equal to the number of existing subresources
(1)! [ RESOURCE_MANIPULATION ERROR #308: TEXTURE2D_MAP_INVALIDSUBRESOURCE ]

*/
/*
D3D10: ERROR: ID3D10Device::UpdateSubresource: Can only invoke UpdateSubresource when the destination Resource was created with D3D10_USAGE_DEFAULT and is not a multisampled Resource. The destination Resource has (1) samples. [ RESOURCE_MANIPULATION ERROR #289: UPDATESUBRESOURCE_INVALIDDESTINATIONSTATE ]
*/
#endif
#endif

