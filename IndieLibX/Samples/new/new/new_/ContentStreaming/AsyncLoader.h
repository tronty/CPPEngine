/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//--------------------------------------------------------------------------------------
// File: AsyncLoader.h
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef ASYNC_LOADER_H
#define ASYNC_LOADER_H

#include <Framework3/IRenderer.h>
#include "ResourceReuseCache.h"

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
class IDataLoader;
class IDataProcessor;
void WarmIOCache( BYTE* pData, SIZE_T size );

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct RESOURCE_REQUEST
{
    IDataLoader* pDataLoader;
    IDataProcessor* pDataProcessor;
    int* pHR;
    void** ppDeviceObject;
    bool bLock;
    bool bCopy;

    bool bError;
};

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
#define CRITICAL_SECTION int
class CAsyncLoader
{
private:
    BOOL m_bDone;
    BOOL m_bProcessThreadDone;
    BOOL m_bIOThreadDone;
    unsigned int m_NumResourcesToService;
    unsigned int m_NumOustandingResources;
    CGrowableArray <RESOURCE_REQUEST> m_IOQueue;
    CGrowableArray <RESOURCE_REQUEST> m_ProcessQueue;
    CGrowableArray <RESOURCE_REQUEST> m_RenderThreadQueue;
    CRITICAL_SECTION m_csIOQueue;
    CRITICAL_SECTION m_csProcessQueue;
    CRITICAL_SECTION m_csRenderThreadQueue;
    void* m_hIOQueueSemaphore;
    void* m_hProcessQueueSemaphore;
    void* m_hCopyQueueSemaphore;
    void* m_hIOThread;
    unsigned int m_NumProcessingThreads;
    void** m_phProcessThreads;
    unsigned int m_NumIORequests;
    unsigned int m_NumProcessRequests;

private:
    unsigned int                FileIOThreadProc();
    unsigned int                ProcessingThreadProc();
    bool                        InitAsyncLoadingThreadObjects( unsigned int NumProcessingThreads );
    void                        DestroyAsyncLoadingThreadObjects();

public:
    friend unsigned int   _FileIOThreadProc( LPVOID lpParameter );
    friend unsigned int   _ProcessingThreadProc( LPVOID lpParameter );

                                CAsyncLoader( unsigned int NumProcessingThreads );
                                ~CAsyncLoader();

    int                     AddWorkItem( IDataLoader* pDataLoader, IDataProcessor* pDataProcessor,
                                             int* pHResult, void** ppDeviceObject );
    void                        WaitForAllItems();
    void                        ProcessDeviceWorkItems( unsigned int CurrentNumResourcesToService, BOOL bRetryLoads=TRUE );
};

#endif
