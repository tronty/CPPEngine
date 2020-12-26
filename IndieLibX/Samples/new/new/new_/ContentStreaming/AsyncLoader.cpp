/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//--------------------------------------------------------------------------------------
// File: AsyncLoader.cpp
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "SDKMisc.h"
#include "AsyncLoader.h"
#include "ContentLoaders.h"
#include <process.h>

//--------------------------------------------------------------------------------------
// External flags
//--------------------------------------------------------------------------------------
extern bool g_bUseMemoryMappedIO;
extern bool g_bWarmIOCache;

//--------------------------------------------------------------------------------------
unsigned int  _FileIOThreadProc( LPVOID lpParameter )
{
    return ( ( CAsyncLoader* )lpParameter )->FileIOThreadProc();
}

//--------------------------------------------------------------------------------------
unsigned int  _ProcessingThreadProc( LPVOID lpParameter )
{
    return ( ( CAsyncLoader* )lpParameter )->ProcessingThreadProc();
}

//--------------------------------------------------------------------------------------
// WarmIOCache tells the virtual memory subsystem to prefetch pages for this chunk
// of memory.  By touching 1 byte in every 4k of data, we can ensure that each page
// we need is loaded into memory.
//--------------------------------------------------------------------------------------
void WarmIOCache( BYTE* pData, SIZE_T size )
{
    // read one byte in every 4k page in order to force all of the pages to load
    SIZE_T start = 0;
    BYTE byteTemp = 0;

    while( start < size )
    {
        byteTemp = pData[ start ];
        start += 4096;
    }
}

//--------------------------------------------------------------------------------------
CAsyncLoader::CAsyncLoader( unsigned int NumProcessingThreads ) : m_bDone( false ),
                                                          m_bProcessThreadDone( false ),
                                                          m_bIOThreadDone( false ),
                                                          m_NumResourcesToService( 0 ),
                                                          m_NumOustandingResources( 0 ),
                                                          m_hIOQueueSemaphore( 0 ),
                                                          m_hProcessQueueSemaphore( 0 ),
                                                          m_hIOThread( 0 ),
                                                          m_NumProcessingThreads( 0 ),
                                                          m_phProcessThreads( 0 )
{
    InitAsyncLoadingThreadObjects( NumProcessingThreads );
    m_NumIORequests = 0;
    m_NumProcessRequests = 0;
}

//--------------------------------------------------------------------------------------
CAsyncLoader::~CAsyncLoader()
{
    DestroyAsyncLoadingThreadObjects();
}

//--------------------------------------------------------------------------------------
// Add a work item to the queue of work items
//--------------------------------------------------------------------------------------
int CAsyncLoader::AddWorkItem( IDataLoader* pDataLoader, IDataProcessor* pDataProcessor, int* pHResult,
                                   void** ppDeviceObject )
{
    if( !pDataLoader || !pDataProcessor )
        return E_INVALIDARG;

    RESOURCE_REQUEST ResourceRequest;
    ResourceRequest.pDataLoader = pDataLoader;
    ResourceRequest.pDataProcessor = pDataProcessor;
    ResourceRequest.pHR = pHResult;
    ResourceRequest.ppDeviceObject = ppDeviceObject;
    ResourceRequest.bCopy = false;
    ResourceRequest.bLock = false;
    ResourceRequest.bError = false;
    if( ppDeviceObject )
        *ppDeviceObject = 0;

    // Add the request to the read queue
    EnterCriticalSection( &m_csIOQueue );
    m_IOQueue.Add( ResourceRequest );
    LeaveCriticalSection( &m_csIOQueue );

    // TODO: critsec around this?
    m_NumOustandingResources ++;

    // Signal that we have something to read
    ReleaseSemaphore( m_hIOQueueSemaphore, 1, 0 );

    return 0;
}

//--------------------------------------------------------------------------------------
// Wait for all work in the queues to finish
//--------------------------------------------------------------------------------------
void CAsyncLoader::WaitForAllItems()
{
    ProcessDeviceWorkItems( UINT_MAX, FALSE );

    for(; ; )
    {
        // Only exit when all resources are loaded
        if( 0 == m_NumOustandingResources )
            return;

        // Service Queues
        ProcessDeviceWorkItems( UINT_MAX, FALSE );
        Sleep( 100 );
    }
}

//--------------------------------------------------------------------------------------
// FileIOThreadProc
//
// This is the one IO threadproc.  This function is responsible for processing read
// requests made by the application.  There should only be one IO thread per device.  
// This ensures that the disk is only trying to read one part of the disk at a time.
//
// This thread performs double-duty as the copy thread as well.  It manages the copying
// of resource data from temporary system memory buffer (or memory mapped pointer) into
// the locked data of the resource.
//--------------------------------------------------------------------------------------
unsigned int CAsyncLoader::FileIOThreadProc()
{
    char szMessage[MAX_PATH];
    int hr = 0;
    m_bIOThreadDone = false;

    RESOURCE_REQUEST ResourceRequest = {0};

    while( !m_bDone )
    {
        // Wait for a read or create request
        WaitForSingleObject( m_hIOQueueSemaphore, INFINITE );
        if( m_bDone )
            break;

        m_NumIORequests ++;

        // Pop a request off of the IOQueue
        EnterCriticalSection( &m_csIOQueue );
        ResourceRequest = m_IOQueue.GetAt( 0 );
        m_IOQueue.Remove( 0 );
        LeaveCriticalSection( &m_csIOQueue );

        // Handle a read request
        if( !ResourceRequest.bCopy )
        {
            if( !ResourceRequest.bError )
            {
                // Load the data
                hr = ResourceRequest.pDataLoader->Load();

                if( FAILED( hr ) )
                {
                    swprintf_s( szMessage, MAX_PATH, "FileIO Error: hr = %x\n", hr );
                    OutputDebugString( szMessage );

                    ResourceRequest.bError = true;
                    if( ResourceRequest.pHR )
                        *ResourceRequest.pHR = hr;
                }
            }

            // Add it to the ProcessQueue
            EnterCriticalSection( &m_csProcessQueue );
            m_ProcessQueue.Add( ResourceRequest );
            LeaveCriticalSection( &m_csProcessQueue );

            // Let the process thread know it's got work to do
            ReleaseSemaphore( m_hProcessQueueSemaphore, 1, 0 );
        }

            // Handle a copy request
        else
        {
            if( !ResourceRequest.bError )
            {
                // Create the data
                hr = ResourceRequest.pDataProcessor->CopyToResource();

                if( FAILED( hr ) )
                {
                    OutputDebugString( "Failed to Copy Data to Device Object\n" );

                    ResourceRequest.bError = true;
                    if( ResourceRequest.pHR )
                        *ResourceRequest.pHR = hr;
                }
            }
            else
            {
                ResourceRequest.pDataProcessor->SetResourceError();
            }

            // send an unlock request
            ResourceRequest.bLock = false;
            EnterCriticalSection( &m_csRenderThreadQueue );
            m_RenderThreadQueue.Add( ResourceRequest );
            LeaveCriticalSection( &m_csRenderThreadQueue );
        }
    }
    m_bIOThreadDone = true;
    return 0;
}

//--------------------------------------------------------------------------------------
// ProcessingThreadProc
// 
// This is the threadproc for the processing thread.  There are multiple processing
// threads.  The job of the processing thread is to uncompress, unpack, or otherwise
// manipulate the data loaded by the loading thread in order to get it ready for the
// ProcessDeviceWorkItems function in the graphics thread to lock or unlock the resource.
//--------------------------------------------------------------------------------------
unsigned int CAsyncLoader::ProcessingThreadProc()
{
    char szMessage[MAX_PATH];

    int hr = 0;
    m_bProcessThreadDone = false;
    while( !m_bDone )
    {
        // Acquire ProcessQueueSemaphore
        WaitForSingleObject( m_hProcessQueueSemaphore, INFINITE );
        if( m_bDone )
            break;

        m_NumProcessRequests ++;

        // Pop a request off of the ProcessQueue
        EnterCriticalSection( &m_csProcessQueue );
        RESOURCE_REQUEST ResourceRequest = m_ProcessQueue.GetAt( 0 );
        m_ProcessQueue.Remove( 0 );
        LeaveCriticalSection( &m_csProcessQueue );

        // Decompress the data
        if( !ResourceRequest.bError )
        {
            void* pData = 0;
            SIZE_T cDataSize = 0;
            hr = ResourceRequest.pDataLoader->Decompress( &pData, &cDataSize );
            if( ( hr ) )
            {
                // Process the data
                hr = ResourceRequest.pDataProcessor->Process( pData, cDataSize );
            }
        }

        if( FAILED( hr ) )
        {
            swprintf_s( szMessage, MAX_PATH, "Processing Thread Error: hr = %x\n", hr );
            OutputDebugString( szMessage );

            ResourceRequest.bError = true;
            if( ResourceRequest.pHR )
                *ResourceRequest.pHR = hr;
        }

        // Add it to the RenderThreadQueue
        ResourceRequest.bLock = true;
        EnterCriticalSection( &m_csRenderThreadQueue );
        m_RenderThreadQueue.Add( ResourceRequest );
        LeaveCriticalSection( &m_csRenderThreadQueue );
    }
    m_bProcessThreadDone = true;
    return 0;
}

//--------------------------------------------------------------------------------------
// Create 1 IO thread and multiple processing threads to handle all of our background
// data loading.
//--------------------------------------------------------------------------------------
bool CAsyncLoader::InitAsyncLoadingThreadObjects( unsigned int NumProcessingThreads )
{
    LONG MaxSemaphoreCount = LONG_MAX;

    // Create 2 semaphores
    m_hIOQueueSemaphore = CreateSemaphore( 0, 0, MaxSemaphoreCount, 0 );
    m_hProcessQueueSemaphore = CreateSemaphore( 0, 0, MaxSemaphoreCount, 0 );

    // Create the queue critical sections
    InitializeCriticalSection( &m_csIOQueue );
    InitializeCriticalSection( &m_csProcessQueue );
    InitializeCriticalSection( &m_csRenderThreadQueue );

    // Create the Processing threads
    m_NumProcessingThreads = NumProcessingThreads;
    m_phProcessThreads = new void*[ m_NumProcessingThreads ];
    if( !m_phProcessThreads )
        return false;
    for( unsigned int i = 0; i < m_NumProcessingThreads; i++ )
    {
        m_phProcessThreads[i] = ( void* )_beginthreadex( 0, 0, _ProcessingThreadProc, ( LPVOID )this,
                                                          CREATE_SUSPENDED, 0 );
        // we would set thread affinity here if we wanted to lock this thread to a processor
        ResumeThread( m_phProcessThreads[i] );
    }

    // Create the IO thread
    m_hIOThread = ( void* )_beginthreadex( 0, 0, _FileIOThreadProc, ( LPVOID )this, CREATE_SUSPENDED, 0 );
    // we would set thread affinity here if we wanted to lock this thread to a processor
    ResumeThread( m_hIOThread );

    return true;
}

//--------------------------------------------------------------------------------------
// Destroy
//--------------------------------------------------------------------------------------
void CAsyncLoader::DestroyAsyncLoadingThreadObjects()
{
    m_bDone = true;

    ReleaseSemaphore( m_hIOQueueSemaphore, 1, 0 );
    ReleaseSemaphore( m_hProcessQueueSemaphore, 1, 0 );

    while( !m_bIOThreadDone || !m_bProcessThreadDone )
    {
        Sleep( 100 );
    }

    CloseHandle( m_hIOQueueSemaphore );
    CloseHandle( m_hProcessQueueSemaphore );

    DeleteCriticalSection( &m_csIOQueue );
    DeleteCriticalSection( &m_csProcessQueue );
    DeleteCriticalSection( &m_csRenderThreadQueue );

    for( unsigned int i = 0; i < m_NumProcessingThreads; i++ )
        CloseHandle( m_phProcessThreads[i] );

    CloseHandle( m_hIOThread );
}

//--------------------------------------------------------------------------------------
// ProcessDeviceWorkItems is called by the graphics thread.  Depending on the request
// it either Locks or Unlocks a resource (or calls UpdateSubresource for D3D10).  One of
// of the arguments is the number of resources to service.  This ensure that no matter
// how many items are in the queue, the graphics thread doesn't stall trying to process
// all of them.
//--------------------------------------------------------------------------------------
void CAsyncLoader::ProcessDeviceWorkItems( unsigned int CurrentNumResourcesToService, BOOL bRetryLoads )
{
    int hr = 0;

    EnterCriticalSection( &m_csRenderThreadQueue );
    unsigned int numJobs = m_RenderThreadQueue.GetSize();
    LeaveCriticalSection( &m_csRenderThreadQueue );

    for( unsigned int i = 0; i < numJobs && i < CurrentNumResourcesToService; i++ )
    {
        EnterCriticalSection( &m_csRenderThreadQueue );
        RESOURCE_REQUEST ResourceRequest = m_RenderThreadQueue.GetAt( 0 );
        m_RenderThreadQueue.Remove( 0 );
        LeaveCriticalSection( &m_csRenderThreadQueue );

        if( ResourceRequest.bLock )
        {
            if( !ResourceRequest.bError )
            {
                hr = ResourceRequest.pDataProcessor->LockDeviceObject();
                if( E_TRYAGAIN == hr && bRetryLoads )
                {
                    // add it back to the list
                    EnterCriticalSection( &m_csRenderThreadQueue );
                    m_RenderThreadQueue.Add( ResourceRequest );
                    LeaveCriticalSection( &m_csRenderThreadQueue );

                    // move on to the next guy
                    continue;
                }
                else if( FAILED( hr ) )
                {
                    ResourceRequest.bError = true;
                    if( ResourceRequest.pHR )
                        *ResourceRequest.pHR = hr;
                }
            }

            ResourceRequest.bCopy = true;
            EnterCriticalSection( &m_csIOQueue );
            m_IOQueue.Add( ResourceRequest );
            LeaveCriticalSection( &m_csIOQueue );

            // Signal that we have something to copy
            ReleaseSemaphore( m_hIOQueueSemaphore, 1, 0 );
        }
        else
        {
            if( !ResourceRequest.bError )
            {
                int hr = ResourceRequest.pDataProcessor->UnLockDeviceObject();
                if( ResourceRequest.pHR )
                    *ResourceRequest.pHR = hr;
            }

            SAFE_DELETE( ResourceRequest.pDataLoader );
            SAFE_DELETE( ResourceRequest.pDataProcessor );

            // Decrement num oustanding resources
            m_NumOustandingResources --;
        }
    }
}
