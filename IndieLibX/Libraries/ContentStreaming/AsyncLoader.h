//--------------------------------------------------------------------------------------
// File: AsyncLoader.h
//
// Illustrates streaming content 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef ASYNC_LOADER_H
#define ASYNC_LOADER_H

#include <FrameworkHLSLCg3/RendererHLSLCg.h>
//include "SDKMesh.h"
#include "ResourceReuseCache.h"

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
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
	static const int mo=-1;
	int& iid;
	void SetID(int& aid){iid=aid;}
	IDataProcessor(int& aid):iid(aid)//???*&mo)
    {
    };
    virtual                 ~IDataProcessor()
    {
    };
    virtual HRESULT WINAPI  LockDeviceObject() = 0;
    virtual HRESULT WINAPI  UnLockDeviceObject() = 0;
    virtual HRESULT WINAPI  Destroy() = 0;
    virtual HRESULT WINAPI  Process( void* pData, SIZE_T cBytes ) = 0;
    virtual HRESULT WINAPI  CopyToResource() = 0;
    virtual void WINAPI     SetResourceError() = 0;
BOOL operator = (CONST IDataProcessor& a) const
{
	return iid=a.iid;//???
}
};

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct RESOURCE_REQUEST
{
    IDataLoader* pDataLoader;
    IDataProcessor* pDataProcessor;
    HRESULT* pHR;
    void** ppDeviceObject;
    bool bLock;
    bool bCopy;

    bool bError;
};
typedef int LoaderID;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
class CAsyncLoader
{
private:
    BOOL m_bDone;
    BOOL m_bProcessThreadDone;
    BOOL m_bIOThreadDone;
    UINT m_NumResourcesToService;
    UINT m_NumOustandingResources;
    VArray <RESOURCE_REQUEST> m_IOQueue;
    VArray <RESOURCE_REQUEST> m_ProcessQueue;
    VArray <RESOURCE_REQUEST> m_RenderThreadQueue;
    CRITICAL_SECTION m_csIOQueue;
    CRITICAL_SECTION m_csProcessQueue;
    CRITICAL_SECTION m_csRenderThreadQueue;
    HANDLE m_hIOQueueSemaphore;
    HANDLE m_hProcessQueueSemaphore;
    HANDLE m_hCopyQueueSemaphore;
    HANDLE m_hIOThread;
    UINT m_NumProcessingThreads;
    HANDLE* m_phProcessThreads;
    UINT m_NumIORequests;
    UINT m_NumProcessRequests;

private:
    unsigned int                FileIOThreadProc();
    unsigned int                ProcessingThreadProc();
    bool                        InitAsyncLoadingThreadObjects( UINT NumProcessingThreads );
    void                        DestroyAsyncLoadingThreadObjects();

public:
    friend unsigned int WINAPI  _FileIOThreadProc( LPVOID lpParameter );
    friend unsigned int WINAPI  _ProcessingThreadProc( LPVOID lpParameter );

                                CAsyncLoader( UINT NumProcessingThreads );
                                ~CAsyncLoader();

    HRESULT                     AddWorkItem( /*IDataLoader* */LoaderID pDataLoader, IDataProcessor* pDataProcessor,
                                             HRESULT* pHResult, void** ppDeviceObject );
    void                        WaitForAllItems();
    void                        ProcessDeviceWorkItems( UINT CurrentNumResourcesToService, BOOL bRetryLoads=TRUE );
};

#endif
