//--------------------------------------------------------------------------------------
// File: ContentLoaders.h
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//include <FrameworkHLSLCg3/RendererHLSLCg.h>
#include "AsyncLoader.h"
#include "ResourceReuseCache.h"

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
struct SDKMESH_CALLBACKS10;
struct SDKMESH_CALLBACKS9;
class CDXUTSDKMesh;
class CPackedFile;

//--------------------------------------------------------------------------------------
// IDataLoader is an interface that the AsyncLoader class uses to load data from disk.
//
// Load is called from the IO thread to load data.
// Decompress is called by one of the processing threads to decompress the data.
// Destroy is called by the graphics thread when it has consumed the data.
//--------------------------------------------------------------------------------------
#if 0
class IDataLoader
{
public:
    virtual                 ~IDataLoader()
    {
    };
    virtual HRESULT WINAPI  Decompress( void** ppData, SIZE_T* pcBytes ) = 0;
    virtual HRESULT WINAPI  Destroy() = 0;
    virtual HRESULT WINAPI  Load() = 0;
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
    virtual HRESULT WINAPI  LockDeviceObject() = 0;
    virtual HRESULT WINAPI  UnLockDeviceObject() = 0;
    virtual HRESULT WINAPI  Destroy() = 0;
    virtual HRESULT WINAPI  Process( void* pData, SIZE_T cBytes ) = 0;
    virtual HRESULT WINAPI  CopyToResource() = 0;
    virtual void WINAPI     SetResourceError() = 0;
};

#endif/*
class CTextureProsessor : public IDataProsessor
{
public:
    HRESULT WINAPI  LockDeviceObject();
    HRESULT WINAPI  UnLockDeviceObject();
    HRESULT WINAPI  Destroy();
    HRESULT WINAPI  Process( void* pData, SIZE_T cBytes );
    HRESULT WINAPI  CopyToResource();
    void WINAPI     SetResourceError();
};*/
//--------------------------------------------------------------------------------------
// CTextureLoader implementation of IDataLoader
//--------------------------------------------------------------------------------------
class CTextureLoader : public IDataLoader
{
private:
    CHAR           m_szFileName[MAX_PATH];
    BYTE* m_pData;
    UINT m_cBytes;
    CPackedFile* m_pPackedFile;

public:
                    CTextureLoader( CHAR* szFileName, CPackedFile* pPackedFile );
                    ~CTextureLoader();

    // overrides
public:
    HRESULT WINAPI  Decompress( void** ppData, SIZE_T* pcBytes );
    HRESULT WINAPI  Destroy();
    HRESULT WINAPI  Load();
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
    HRESULT WINAPI  Decompress( void** ppData, SIZE_T* pcBytes );
    HRESULT WINAPI  Destroy();
    HRESULT WINAPI  Load();
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
    HRESULT WINAPI  Decompress( void** ppData, SIZE_T* pcBytes );
    HRESULT WINAPI  Destroy();
    HRESULT WINAPI  Load();
};

//--------------------------------------------------------------------------------------
// CSDKMeshLoader implementation of IDataLoader
//--------------------------------------------------------------------------------------
#if 0
class CSDKMeshLoader : public IDataLoader
{
private:
    LOADER_DEVICE m_Device;
    CDXUTSDKMesh* m_pMesh;
    CHAR           m_szFileName[MAX_PATH];
    bool m_bCreateAdjacencyIndices;
    SDKMESH_CALLBACKS10* m_pLoaderCallbacks10;
    SDKMESH_CALLBACKS9* m_pLoaderCallbacks9;

public:
                    CSDKMeshLoader( CDXUTSDKMesh* pMesh,
                                    ID3D10Device* pDev10,
                                    LPCTSTR szFileName,
                                    bool bCreateAdjacencyIndices=false,
                                    SDKMESH_CALLBACKS10* pLoaderCallbacks=NULL );
                    CSDKMeshLoader( CDXUTSDKMesh* pMesh,
                                    IDirect3DDevice9* pDev9,
                                    LPCTSTR szFileName,
                                    bool bCreateAdjacencyIndices=false,
                                    SDKMESH_CALLBACKS9* pLoaderCallbacks=NULL );
                    ~CSDKMeshLoader();

    // overrides
public:
    HRESULT WINAPI  Decompress( void** ppData, SIZE_T* pcBytes );
    HRESULT WINAPI  Destroy();
    HRESULT WINAPI  Load();
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
    HRESULT WINAPI  LockDeviceObject();
    HRESULT WINAPI  UnLockDeviceObject();
    HRESULT WINAPI  Destroy();
    HRESULT WINAPI  Process( void* pData, SIZE_T cBytes );
    HRESULT WINAPI  CopyToResource();
    void WINAPI     SetResourceError();
};

//--------------------------------------------------------------------------------------
// CTextureProcessor implementation of IDataProcessor
//--------------------------------------------------------------------------------------
#define MAX_MIP_LEVELS 32
class CTextureProcessor : public IDataProcessor
{
private:
#if 0
//    LOADER_DEVICE m_Device;
    ID3D10ShaderResourceView** m_ppRV10;
    ID3D10ShaderResourceView* m_pRealRV10;
    ID3D10Texture2D* m_pStaging10;
    D3D10_MAPPED_TEXTURE2D  m_pLockedRects10[MAX_MIP_LEVELS];
#endif
    //D3DLOCKED_RECT          m_pLockedRects[MAX_MIP_LEVELS];

	BYTE *  m_pLockedRectsBits[MAX_MIP_LEVELS];
	int  m_pLockedRectsPitch[MAX_MIP_LEVELS];

	TextureID* m_ppTexture9;
    BYTE* m_pData;
    SIZE_T m_cBytes;
    CResourceReuseCache* m_pResourceReuseCache;
     TextureID m_pRealTexture9;
    UINT m_iNumLockedPtrs;
    UINT m_SkipMips;

private:
    BOOL                    PopulateTexture();

public:
CTextureProcessor(
				      TextureID* ppTexture9,
				      CResourceReuseCache* pResourceReuseCache,
				      UINT SkipMips );
					  ~CTextureProcessor();

    // overrides
public:
    HRESULT WINAPI          LockDeviceObject();
    HRESULT WINAPI          UnLockDeviceObject();
    HRESULT WINAPI          Destroy();
    HRESULT WINAPI          Process( void* pData, SIZE_T cBytes );
    HRESULT WINAPI          CopyToResource();
    void WINAPI             SetResourceError();
};


//--------------------------------------------------------------------------------------
// CVertexBufferProcessor implementation of IDataProcessor
//--------------------------------------------------------------------------------------
class CVertexBufferProcessor : public IDataProcessor
{
private:
#if 0
 //   LOADER_DEVICE m_Device;
    ID3D10Buffer** m_ppBuffer10;
    D3D10_BUFFER_DESC m_BufferDesc;
    ID3D10Buffer* m_pRealBuffer10;
    DWORD m_Usage;
    DWORD m_FVF;
    D3DPOOL m_Pool;
#endif
    VertexBufferID* m_ppBuffer9;
    UINT m_iSizeBytes;
    void* m_pData;
    CResourceReuseCache* m_pResourceReuseCache;
    VertexBufferID m_pRealBuffer9;
    void* m_pLockedData;

public:
                    CVertexBufferProcessor(
						VertexBufferID* ppBuffer9,
						UINT iSizeBytes,
						//DWORD Usage,

						//DWORD FVF,
						//D3DPOOL Pool,
						void* pData
						);
					~CVertexBufferProcessor();

    // overrides
public:
    HRESULT WINAPI  LockDeviceObject();
    HRESULT WINAPI  UnLockDeviceObject();
    HRESULT WINAPI  Destroy();
    HRESULT WINAPI  Process( void* pData, SIZE_T cBytes );
    HRESULT WINAPI  CopyToResource();
    void WINAPI     SetResourceError();
};

//--------------------------------------------------------------------------------------
// CIndexBufferProcessor implementation of IDataProcessor
//--------------------------------------------------------------------------------------
class CIndexBufferProcessor : public IDataProcessor
{
private:
#if 0
	//   LOADER_DEVICE m_Device;
    ID3D10Buffer** m_ppBuffer10;
    D3D10_BUFFER_DESC m_BufferDesc;
    D3DPOOL m_Pool;
    ID3D10Buffer* m_pRealBuffer10;
    DWORD m_Usage;
#endif
    int m_ibFormat;

	IndexBufferID* m_ppBuffer9;
    UINT m_iSizeBytes;
    void* m_pData;
    CResourceReuseCache* m_pResourceReuseCache;
    IndexBufferID m_pRealBuffer9;
    void* m_pLockedData;

public:
CIndexBufferProcessor(
					      IndexBufferID* ppBuffer9,
					      UINT iSizeBytes,
					      //DWORD Usage,
					      int ibFormat,
					      //D3DPOOL Pool,
					      void* pData
					      );
			~CIndexBufferProcessor();

    // overrides
public:
    HRESULT WINAPI  LockDeviceObject();
    HRESULT WINAPI  UnLockDeviceObject();
    HRESULT WINAPI  Destroy();
    HRESULT WINAPI  Process( void* pData, SIZE_T cBytes );
    HRESULT WINAPI  CopyToResource();
    void WINAPI     SetResourceError();
};


#endif