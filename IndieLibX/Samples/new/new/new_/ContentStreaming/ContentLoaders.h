/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
//--------------------------------------------------------------------------------------
// File: ContentLoaders.h
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include <Framework3/IRenderer.h>
#include "AsyncLoader.h"
#include "ResourceReuseCache.h"

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
struct SDKMESH_ S10;
struct SDKMESH_ S9;
class MeshRenderer2;
class CPackedFile;

//--------------------------------------------------------------------------------------
// IDataLoader is an interface that the AsyncLoader class uses to load data from disk.
//
// Load is called from the IO thread to load data.
// Decompress is called by one of the processing threads to decompress the data.
// Destroy is called by the graphics thread when it has consumed the data.
//--------------------------------------------------------------------------------------
class IDataLoader
{
public:
    virtual                 ~IDataLoader()
    {
    };
    virtual int   Decompress( void** ppData, SIZE_T* pcBytes ) = 0;
    virtual int   Destroy() = 0;
    virtual int   Load() = 0;
};

//--------------------------------------------------------------------------------------
// IDataProcessor is an interface that the AsyncLoader class uses to process and copy
// data into locked resource pointers.
//
// Process is called by one of the processing threads to process the data before it is
//   consumed.
// LockDeviceObject is called from the Graphics thread to lock the device object (D3D9).
// UnLockDeviceObject is called from the Graphics thread to unlock the device object, or
//   to call updatesubresource for D3D10.
// CopyToResource copies the data from memory to the locked device object (D3D9).
// SetResourceError is called to set the resource pointer to an error code in the event
//   that something went wrong.
// Destroy is called by the graphics thread when it has consumed the data.
//--------------------------------------------------------------------------------------
class IDataProcessor
{
public:
    virtual                 ~IDataProcessor()
    {
    };
    virtual int   LockDeviceObject() = 0;
    virtual int   UnLockDeviceObject() = 0;
    virtual int   Destroy() = 0;
    virtual int   Process( void* pData, SIZE_T cBytes ) = 0;
    virtual int   CopyToResource() = 0;
    virtual void      SetResourceError() = 0;
};

//--------------------------------------------------------------------------------------
// CTextureLoader implementation of IDataLoader
//--------------------------------------------------------------------------------------
class CTextureLoader : public IDataLoader
{
private:
    std::string           m_szFileName[MAX_PATH];
    BYTE* m_pData;
    unsigned int m_cBytes;
    CPackedFile* m_pPackedFile;

public:
                    CTextureLoader( std::string szFileName, CPackedFile* pPackedFile );
                    ~CTextureLoader();

    // overrides
public:
    int   Decompress( void** ppData, SIZE_T* pcBytes );
    int   Destroy();
    int   Load();
};

//--------------------------------------------------------------------------------------
// CTextureProcessor implementation of IDataProcessor
//--------------------------------------------------------------------------------------
#define MAX_MIP_LEVELS 32
class CTextureProcessor : public IDataProcessor
{
private:
    TextureID  m_ppTexture9;
    BYTE* m_pData;
    SIZE_T m_cBytes;
    TextureID m_pRealTexture9;
    BYTE*          m_pLockedPtr[MAX_MIP_LEVELS];
    unsigned int   m_pLockedStride[MAX_MIP_LEVELS];
    unsigned int m_iNumLockedPtrs;
    unsigned int m_SkipMips;

private:
    BOOL                    PopulateTexture();

public:
                            CTextureProcessor( TextureID  ppTexture9,
                                               CResourceReuseCache* pResourceReuseCache, unsigned int SkipMips );
                            ~CTextureProcessor();

    // overrides
public:
    int           LockDeviceObject();
    int           UnLockDeviceObject();
    int           Destroy();
    int           Process( void* pData, SIZE_T cBytes );
    int           CopyToResource();
    void              SetResourceError();
};

//--------------------------------------------------------------------------------------
// CVertexBufferLoader implementation of IDataLoader
//--------------------------------------------------------------------------------------
class CVertexBufferLoader : public IDataLoader
{
private:

public:
                    CVertexBufferLoader();
                    ~CVertexBufferLoader();

    // overrides
public:
    int   Decompress( void** ppData, SIZE_T* pcBytes );
    int   Destroy();
    int   Load();
};

#define D3DPOOL int
//--------------------------------------------------------------------------------------
// CVertexBufferProcessor implementation of IDataProcessor
//--------------------------------------------------------------------------------------
class CVertexBufferProcessor : public IDataProcessor
{
private:
    VertexBufferID  m_ppBuffer9;
    unsigned int m_iSizeBytes;
    __DWORD__ m_Usage;
    __DWORD__ m_FVF;
    D3DPOOL m_Pool;
    void* m_pData;
    CResourceReuseCache* m_pResourceReuseCache;
    VertexBufferID m_pRealBuffer9;
    void* m_pLockedData;

public:
                    CVertexBufferProcessor( VertexBufferID  ppBuffer9,
                                            unsigned int iSizeBytes,
                                            __DWORD__ Usage,
                                            __DWORD__ FVF,
                                            D3DPOOL Pool,
                                            void* pData,
                                            CResourceReuseCache* pResourceReuseCache );
                    ~CVertexBufferProcessor();

    // overrides
public:
    int   LockDeviceObject();
    int   UnLockDeviceObject();
    int   Destroy();
    int   Process( void* pData, SIZE_T cBytes );
    int   CopyToResource();
    void      SetResourceError();
};

//--------------------------------------------------------------------------------------
// CIndexBufferLoader implementation of IDataLoader
//--------------------------------------------------------------------------------------
class CIndexBufferLoader : public IDataLoader
{
private:

public:
                    CIndexBufferLoader();
                    ~CIndexBufferLoader();

    // overrides
public:
    int   Decompress( void** ppData, SIZE_T* pcBytes );
    int   Destroy();
    int   Load();
};

//--------------------------------------------------------------------------------------
// CIndexBufferProcessor implementation of IDataProcessor
//--------------------------------------------------------------------------------------
class CIndexBufferProcessor : public IDataProcessor
{
private:
    IndexBufferID  m_ppBuffer9;
    unsigned int m_iSizeBytes;
    __DWORD__ m_Usage;
    FORMAT m_ibFormat;
    D3DPOOL m_Pool;
    void* m_pData;
    CResourceReuseCache* m_pResourceReuseCache;
    IndexBufferID m_pRealBuffer9;
    void* m_pLockedData;

public:
                    CIndexBufferProcessor( IndexBufferID  ppBuffer9,
                                           unsigned int iSizeBytes,
                                           __DWORD__ Usage,
                                           FORMAT ibFormat,
                                           D3DPOOL Pool,
                                           void* pData,
                                           CResourceReuseCache* pResourceReuseCache );
                    ~CIndexBufferProcessor();

    // overrides
public:
    int   LockDeviceObject();
    int   UnLockDeviceObject();
    int   Destroy();
    int   Process( void* pData, SIZE_T cBytes );
    int   CopyToResource();
    void      SetResourceError();
};

//--------------------------------------------------------------------------------------
// CSDKMeshLoader implementation of IDataLoader
//--------------------------------------------------------------------------------------
class CSDKMeshLoader : public IDataLoader
{
private:
    MeshRenderer2* m_pMesh;
    std::string           m_szFileName[MAX_PATH];
    bool m_bCreateAdjacencyIndices;
    SDKMESH_ S10* m_pLoaderCallbacks10;
    SDKMESH_ S9* m_pLoaderCallbacks9;

public:
                    CSDKMeshLoader( std::string& szFileName,
                                    bool bCreateAdjacencyIndices=false,
                                    SDKMESH_ S9* pLoaderCallbacks=0 );
                    ~CSDKMeshLoader();

    // overrides
public:
    int   Decompress( void** ppData, SIZE_T* pcBytes );
    int   Destroy();
    int   Load();
};

//--------------------------------------------------------------------------------------
// CSDKMeshProcessor implementation of IDataProcessor
//--------------------------------------------------------------------------------------
class CSDKMeshProcessor : public IDataProcessor
{
public:
                    CSDKMeshProcessor();
                    ~CSDKMeshProcessor();

    // overrides
public:
    int   LockDeviceObject();
    int   UnLockDeviceObject();
    int   Destroy();
    int   Process( void* pData, SIZE_T cBytes );
    int   CopyToResource();
    void      SetResourceError();
};

