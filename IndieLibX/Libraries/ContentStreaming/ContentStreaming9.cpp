#include <FrameworkHLSLCg3/Platform.h>
using namespace STX;
#ifdef BUILDCONTENTSTREAMING
//--------------------------------------------------------------------------------------
// File: ContentStreaming9.cpp
//
// Illustrates streaming content using Direct3D 9
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <FrameworkHLSLCg3/RendererHLSLCg.h>
#include "resource.h"
#include "AsyncLoader.h"
//include <FrameworkHLSLCg3/ContentLoaders.h>
#include "ResourceReuseCache.h"
#include "PackedFile.h"
#include "Terrain.h"
#include <oleauto.h>
#include <wbemidl.h>

//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define DEG2RAD(p) ( PI*(p/180.0f) )

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
//extern CFirstPersonCamera	    g_Camera;		    // A first person camera

extern CAsyncLoader*		    g_pAsyncLoader;
extern CResourceReuseCache*	    g_pResourceReuseCache;
extern UINT			    g_NumProcessingThreads;
extern UINT			    g_MaxOutstandingResources;
extern CPackedFile		    g_PackFile;

// Direct3D 9 resources
VertexFormatID	    g_pDeclTileWire = NULL;
VertexFormatID	    g_pDeclTileBump = NULL;
VertexFormatID	    g_pDeclTileDiff = NULL;

// Effect variable handles
	const char*	g_hmWorld = "g_mWorld" ;
    const char*	g_hmWorldViewProjection = "g_mWorldViewProj" ;
    const char*	g_hEyePt = "g_vEyePt" ;
    const char*	g_htxDiffuse = "g_txDiffuse" ;
    const char*	g_htxNormal = "g_txNormal" ;
/*
    const char*	g_hRenderTileDiff = "RenderTileDiff" ;
    const char*	g_hRenderTileBump = "RenderTileBump" ;
    const char*	g_hRenderTileWire = "RenderTileWire" ;
*/
	ShaderID	g_hRenderTileDiff;
    ShaderID	g_hRenderTileBump;
    ShaderID	g_hRenderTileWire;

	extern 	mat4 mv;
	extern 	mat4 p;
	extern 	mat4 mvp;

extern VArray <LEVEL_ITEM*> g_VisibleItemArray;
extern CTerrain 		    g_Terrain;

extern float			    g_fFOV;
extern float			    g_fAspectRatio;
extern float			    g_fInitialLoadTime;
extern float			    g_LoadingRadius;
extern int			    g_NumResourceToLoadPerFrame;
extern int			    g_UploadToVRamEveryNthFrame;
extern UINT			    g_SkipMips;
extern UINT			    g_NumMeshPlacements;
extern UINT			    g_NumLevelPieces;
extern UINT			    g_NumLevelItems;
extern bool			    g_bStartupResourcesLoaded;
extern bool			    g_bDrawUI;
extern bool			    g_bWireframe;
extern UINT64			    g_AvailableVideoMem;

enum LOAD_TYPE
{
    LOAD_TYPE_MULTITHREAD = 0,
    LOAD_TYPE_SINGLETHREAD,
};
extern LOAD_TYPE		    g_LoadType;

enum APP_STATE
{
    APP_STATE_STARTUP = 0,
    APP_STATE_RENDER_SCENE
};
extern APP_STATE		    g_AppState;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice(  );
HRESULT CALLBACK OnResetDevice(  );
void CALLBACK OnFrameRender(  double fTime, float fElapsedTime  );
void CALLBACK OnLostDevice(  );
void CALLBACK OnDestroyDevice(  );

void CALLBACK CreateTextureFromFile9_Serial(  CHAR* szFileName, TextureID* ppTexture,
					     void* pContext );
void CALLBACK CreateVertexBuffer9_Serial(  VertexBufferID* ppBuffer, UINT iSizeBytes,
					  DWORD Usage, DWORD FVF, D3DPOOL Pool, void* pData, void* pContext );
void CALLBACK CreateIndexBuffer9_Serial(  IndexBufferID* ppBuffer, UINT iSizeBytes,
					 DWORD Usage, D3DFORMAT ibFormat, D3DPOOL Pool, void* pData, void* pContext );
void CALLBACK CreateTextureFromFile9_Async(  CHAR* szFileName, TextureID* ppTexture,
					    void* pContext );
void CALLBACK CreateVertexBuffer9_Async(  VertexBufferID* ppBuffer, UINT iSizeBytes,
					 DWORD Usage, DWORD FVF, D3DPOOL Pool, void* pData, void* pContext );
void CALLBACK CreateIndexBuffer9_Async(  IndexBufferID* ppBuffer, UINT iSizeBytes,
					DWORD Usage, D3DFORMAT ibFormat, D3DPOOL Pool, void* pData, void* pContext );

extern void LoadStartupResources( double fTime );
extern void RenderText();
extern void DestroyAllMeshes( );




//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice(  
				      )
{
//    HRESULT hr;


    // Read the D3DX effect file
    CHAR str[MAX_PATH];
    ( FindSDKMediaFileCch( str, MAX_PATH, "ContentStreaming.shd" ) );
    //( D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, D3DXFX_NOT_CLONEABLE,// | D3DXFX_LARGEADDRESSAWARE,
	//				NULL, &g_pEffect9, NULL ) );
	OutputDebugString("\n");
	OutputDebugString(str);
	OutputDebugString("\n");
	//strcpy(str,"c:\\DX10\\DX10\\FrameworkHLSLCg3\\Samples\\Uusi kansio\\ContentStreaming9\\ContentStreaming.shd");
	strcpy(str,"ContentStreaming.shd");
	g_hRenderTileDiff = g_pApp->m_pRenderer->addShaderFromFile(str, "VSBasic", "PSTerrain",ASSEMBLY);
    g_hRenderTileBump = g_pApp->m_pRenderer->addShaderFromFile(str, "VSBasic", "PSTerrainBump",ASSEMBLY);
    g_hRenderTileWire = g_pApp->m_pRenderer->addShaderFromFile(str, "VSBasic", "PSTerrain",ASSEMBLY);

    // Create a decl for the object data.
    FormatDesc quadFmt[] = {
	{ 0, TYPE_VERTEX, FORMAT_FLOAT, 3},
	{0, TYPE_NORMAL, FORMAT_FLOAT, 3},
	{0, TYPE_TEXCOORD, FORMAT_FLOAT, 2},
    };

	// 0, TYPE_VERTEX, FORMAT_FLOAT, 2
	g_pDeclTileWire = g_pApp->m_pRenderer->addVertexFormat(quadFmt, elementsOf(quadFmt),g_hRenderTileWire);
	g_pDeclTileBump = g_pApp->m_pRenderer->addVertexFormat(quadFmt, elementsOf(quadFmt),g_hRenderTileBump);
	g_pDeclTileDiff = g_pApp->m_pRenderer->addVertexFormat(quadFmt, elementsOf(quadFmt),g_hRenderTileDiff);


    // Setup the camera's view parameters
    float3 vecEye( 0.0f, 2.0f, 0.0f );
    float3 vecAt ( 0.0f, 2.0f, 1.0f );
//    g_pApp->m_pRenderer->SetViewParams( &vecEye, &vecAt );
	g_pApp->m_pRenderer->buildLookAtMatrix(vecEye);//, vecAt,float3(0,1,0),mv);
	mvp=g_pApp->m_pRenderer->p()*g_pApp->m_pRenderer->mv();

    // Create the async loader
    g_pAsyncLoader=new CAsyncLoader( g_NumProcessingThreads );
    if( !g_pAsyncLoader )
	return E_OUTOFMEMORY;

    // Create the texture reuse cache
    g_pResourceReuseCache=new CResourceReuseCache(  );
    if( !g_pResourceReuseCache )
	return E_OUTOFMEMORY;

    return S_OK;
}


typedef BOOL ( WINAPI* PfnCoSetProxyBlanket )(
IUnknown* pProxy,
DWORD dwAuthnSvc,
DWORD dwAuthzSvc,
OLECHAR* pServerPrincName,
DWORD dwAuthnLevel,
DWORD dwImpLevel,
RPC_AUTH_IDENTITY_HANDLE pAuthInfo,
DWORD dwCapabilities );

UINT32 GetAdapterRAMFromWMI( IDirect3DDevice9* pd3dDevice )
{
    HRESULT hr;
    IWbemLocator* pIWbemLocator = NULL;
    IWbemServices* pIWbemServices = NULL;
    BSTR pNamespace = NULL;

    UINT32 dwAdapterRAM = 0;
    bool bFoundViaWMI = false;

    CoInitialize( 0 );

    hr = CoCreateInstance( CLSID_WbemLocator,
			   NULL,
			   CLSCTX_INPROC_SERVER,
			   IID_IWbemLocator,
			   ( LPVOID* )&pIWbemLocator );

    if( SUCCEEDED( hr ) && pIWbemLocator )
    {
	// Using the locator, connect to WMI in the given namespace.
	pNamespace = SysAllocString( L"\\\\.\\root\\cimv2" );

	hr = pIWbemLocator->ConnectServer( pNamespace, NULL, NULL, 0L,
					   0L, NULL, NULL, &pIWbemServices );
	if( SUCCEEDED( hr ) && pIWbemServices != NULL )
	{
	    HINSTANCE hinstOle32 = NULL;

	    hinstOle32 = LoadLibrary( "ole32.dll" );
	    if( hinstOle32 )
	    {
		PfnCoSetProxyBlanket pfnCoSetProxyBlanket = NULL;

		pfnCoSetProxyBlanket = ( PfnCoSetProxyBlanket )GetProcAddress( hinstOle32, "CoSetProxyBlanket" );
		if( pfnCoSetProxyBlanket != NULL )
		{
		    // Switch security level to IMPERSONATE.
		    pfnCoSetProxyBlanket( pIWbemServices,		// proxy
					  RPC_C_AUTHN_WINNT,		// authentication service
					  RPC_C_AUTHZ_NONE,		// authorization service
					  NULL, 			// server principle name
					  RPC_C_AUTHN_LEVEL_CALL,	// authentication level
					  RPC_C_IMP_LEVEL_IMPERSONATE,	// impersonation level
					  NULL, 			// identity of the client
					  EOAC_NONE );			// capability flags
		}

		FreeLibrary( hinstOle32 );
	    }

	    IEnumWbemClassObject* pEnumProcessorDevs = NULL;
	    BSTR pClassName = NULL;

	    pClassName = SysAllocString( L"Win32_VideoController" );

	    hr = pIWbemServices->CreateInstanceEnum( pClassName,
						     0,
						     NULL, &pEnumProcessorDevs );

	    if( SUCCEEDED( hr ) && pEnumProcessorDevs )
	    {
		IWbemClassObject* pProcessorDev = NULL;
		DWORD uReturned = 0;
		BSTR pPropName = NULL;

		// Get the first one in the list
		pEnumProcessorDevs->Reset();
		hr = pEnumProcessorDevs->Next( 5000,		 // timeout in 5 seconds
					       1,		 // return just one storage device
					       &pProcessorDev,	 // pointer to storage device
					       &uReturned );	 // number obtained: one or zero

		if( SUCCEEDED( hr ) && uReturned != 0 && pProcessorDev != NULL )
		{
		    pPropName = SysAllocString( L"AdapterRAM" );

		    VARIANT var;
		    hr = pProcessorDev->Get( pPropName, 0L, &var, NULL, NULL );
		    if( SUCCEEDED( hr ) )
		    {
			dwAdapterRAM = var.lVal;
			bFoundViaWMI = true;
		    }

		    if( pPropName )
			SysFreeString( pPropName );
		}

		SAFE_RELEASE( pProcessorDev );
	    }

	    if( pClassName )
		SysFreeString( pClassName );
	    SAFE_RELEASE( pEnumProcessorDevs );
	}

	if( pNamespace )
	    SysFreeString( pNamespace );
	SAFE_RELEASE( pIWbemServices );
    }

    SAFE_RELEASE( pIWbemLocator );

    if( bFoundViaWMI )
	return dwAdapterRAM;

    return g_pApp->m_pRenderer->GetAvailableTextureMem();
}




//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT)
// or that are tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice(
				    )
{
//    HRESULT hr;


    // Setup the camera's projection parameters
    g_fAspectRatio = g_pApp->width / ( FLOAT )g_pApp->height;
    //g_Camera.SetProjParams( DEG2RAD(g_fFOV), g_fAspectRatio, 0.1f, 1500.0f );
	g_pApp->m_pRenderer->buildPerspectiveMatrix(g_fFOV, g_fAspectRatio, 0.1f, 1500.0f);
	/*
	g_pApp->m_pRenderer->buildLookAtMatrix(0,0,-1.5,0,0,1,0,1,0,mv);
	multMatrix(mvp,
                p,mv);
*/

    // Get availble texture memory
	
    g_AvailableVideoMem = 		536870912;
	if((g_pApp->m_renderer==D3D9)
		||(g_pApp->m_renderer==D3D10)
		||(g_pApp->m_renderer==D3D11))
		g_AvailableVideoMem =g_pApp->m_pRenderer->GetAvailableTextureMem( );

    // Load resources if they haven't been already (for a device reset)
    if( APP_STATE_RENDER_SCENE == g_AppState )
	{
	LoadStartupResources(   timeGetTime()/1000.0f );//???
	}
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Render the scene using the programmable pipeline
//--------------------------------------------------------------------------------------
static int			    iFrameNum = 0;
void RenderScene(  double fTime, float fElapsedTime  )
{
//    HRESULT hr;
    mat4 mWorld;
    mat4 mView;
    mat4 mProj;
    mat4 mWorldViewProjection;

    // Get the projection & view matrix from the camera class
    mProj = g_pApp->m_pRenderer->GetProjMatrix();
    mView = g_pApp->m_pRenderer->GetViewMatrix();

    // Set the eye vector
    float3 vEyePt = g_pApp->m_pRenderer->GetEyePt();
    float4 vEyePt4;
    vEyePt4.x = vEyePt.x;
    vEyePt4.y = vEyePt.y;
    vEyePt4.z = vEyePt.z;
    ( g_pApp->m_pRenderer->setShaderConstant4f( g_hEyePt, vEyePt4 ) );

    int NewTextureUploadsToVidMem = 0;
    if( iFrameNum % g_UploadToVRamEveryNthFrame > 0 )
	NewTextureUploadsToVidMem = g_NumResourceToLoadPerFrame;
    iFrameNum ++;

    // Render the level
    for( int i = 0; i < g_VisibleItemArray.GetSize(); i++ )
    {
	LEVEL_ITEM* pItem = g_VisibleItemArray.GetAt( i );

	mWorld =identity4();
	mWorldViewProjection = mWorld * mView * mProj;

	( g_pApp->m_pRenderer->setShaderConstant4x4f( g_hmWorldViewProjection, mWorldViewProjection ) );
	( g_pApp->m_pRenderer->setShaderConstant4x4f( g_hmWorld, mWorld ) );

	if( pItem->bLoaded )
	{
	    g_pApp->m_pRenderer->setVertexBuffer( 0, pItem->VB.pVB9);
	    g_pApp->m_pRenderer->setIndexBuffer( pItem->IB.pIB9 );

	    bool bDiff = pItem->Diffuse.pTexture9 ? true : false;
	    if( bDiff && pItem->CurrentCountdownDiff > 0 )
	    {
		bDiff = false;
		pItem->CurrentCountdownDiff --;
	    }
	    bool bNorm = pItem->Normal.pTexture9 ? true : false;
	    if( bNorm && pItem->CurrentCountdownNorm > 0 )
	    {
		bNorm = false;
		pItem->CurrentCountdownNorm --;
	    }

	    bool bCanRenderDiff = bDiff;
	    bool bCanRenderNorm = bDiff && bNorm && pItem->bHasBeenRenderedDiffuse;
	    if( bDiff && !pItem->bHasBeenRenderedDiffuse )
	    {
		if( NewTextureUploadsToVidMem >= g_NumResourceToLoadPerFrame )
		    bCanRenderDiff = false;
		else
		    NewTextureUploadsToVidMem ++;
	    }
	    if( bCanRenderDiff && bNorm && !pItem->bHasBeenRenderedNormal )
	    {
		if( NewTextureUploadsToVidMem >= g_NumResourceToLoadPerFrame )
		    bCanRenderNorm = false;
		else
		    NewTextureUploadsToVidMem ++;
	    }

	    if( !bCanRenderDiff && !bCanRenderNorm )
		continue;

	    // Render the scene with this technique
	    if( g_bWireframe )
	    {
		g_pApp->m_pRenderer->setShader( g_hRenderTileWire );
		g_pApp->m_pRenderer->setVertexFormat( g_pDeclTileWire );
		g_pApp->m_pRenderer->setTexture( g_htxDiffuse, pItem->Diffuse.pTexture9 );
	    }
	    else if( bCanRenderNorm )
	    {
		pItem->bHasBeenRenderedNormal = true;
		g_pApp->m_pRenderer->setShader( g_hRenderTileBump );
		g_pApp->m_pRenderer->setVertexFormat( g_pDeclTileBump );
		g_pApp->m_pRenderer->setTexture( g_htxDiffuse, pItem->Diffuse.pTexture9 );
		g_pApp->m_pRenderer->setTexture( g_htxNormal, pItem->Normal.pTexture9 );
	    }
	    else if( bCanRenderDiff )
	    {
		pItem->bHasBeenRenderedDiffuse = true;
		g_pApp->m_pRenderer->setShader( g_hRenderTileDiff );
		g_pApp->m_pRenderer->setVertexFormat( g_pDeclTileDiff );
		g_pApp->m_pRenderer->setTexture( g_htxDiffuse, pItem->Diffuse.pTexture9 );
	    }


		g_pApp->m_pRenderer->DrawIndexedPrimitive( PRIM_TRIANGLE_STRIP, 0, 0, g_Terrain.GetNumTileVertices(), 0,
						  g_Terrain.GetNumIndices() - 2 );

	    }
	}


    g_pApp->m_pRenderer->setTexture( g_htxDiffuse, NULL );
    g_pApp->m_pRenderer->setTexture( g_htxNormal, NULL );
	
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender(double fTime, float fElapsedTime)//,  )
{
    HRESULT hr = S_OK;

    // Clear the render target and the zbuffer
    ( g_pApp->m_pRenderer->Clear( true,true,float4( 0, 160, 160, 250 )) );



    // Render the scene
    g_pApp->m_pRenderer->BeginScene() ;
    {
	if( APP_STATE_RENDER_SCENE == g_AppState )
	    RenderScene(fTime, fElapsedTime);



	if( g_bDrawUI )
	{
	    RenderText();
	}


	( g_pApp->m_pRenderer->EndScene() );
	g_pApp->m_pRenderer->Present();
    }

    // Load in up to g_NumResourceToLoadPerFrame resources at the end of every frame
    if( LOAD_TYPE_MULTITHREAD == g_LoadType && APP_STATE_RENDER_SCENE == g_AppState )
    {
	UINT NumResToProcess = g_NumResourceToLoadPerFrame;
	g_pAsyncLoader->ProcessDeviceWorkItems( NumResToProcess );
    }
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnResetDevice callback
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice(  )
{
    DestroyAllMeshes(  );

    g_bStartupResourcesLoaded = false;
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnCreateDevice callback
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice(  )
{

    SAFE_DELETE( g_pAsyncLoader );

    g_pResourceReuseCache->OnDestroy();
    SAFE_DELETE( g_pResourceReuseCache );
}

//--------------------------------------------------------------------------------------
// Serial create texture
//--------------------------------------------------------------------------------------
void	CALLBACK CreateTextureFromFile9_Serial(  CHAR* szFileName,
						TextureID* ppTexture, void* pContext )
{
    LoaderID pLoader=g_pApp->m_pRenderer->addTextureLoader( szFileName, &g_PackFile );
    ProcessorID pProcessor=g_pApp->m_pRenderer->addTextureProcessor( *ppTexture, g_pResourceReuseCache, g_SkipMips );

    void* pLocalData;
    SIZE_T Bytes;
    if( FAILED( g_pApp->m_pRenderer->LoadTexture(pLoader) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->DecompressTexture( pLoader,&pLocalData, &Bytes ) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->ProcessTexture( pLoader,pLocalData, Bytes ) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->LockTextureDeviceObject(pProcessor) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->CopyToResourceTexture(pProcessor) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->UnLockTextureDeviceObject(pProcessor) ) )
	goto Error;

    goto Finish;

Error:
    g_pApp->m_pRenderer->SetProcessorResourceError(pProcessor);
Finish:
    g_pApp->m_pRenderer->DestroyProcessor(pProcessor);
    g_pApp->m_pRenderer->DestroyLoader(pLoader);
}

//--------------------------------------------------------------------------------------
// Serial create VB
//--------------------------------------------------------------------------------------
void	CALLBACK CreateVertexBuffer9_Serial(  VertexBufferID* ppBuffer,
					     UINT iSizeBytes, DWORD Usage, DWORD FVF, D3DPOOL Pool, void* pData,
					     void* pContext )
{
    LoaderID pLoader=g_pApp->m_pRenderer->addVertexBufferLoader();
    ProcessorID pProcessor=g_pApp->m_pRenderer->addVertexBufferProcessor( *ppBuffer, iSizeBytes,  FVF,
								     pData, g_pResourceReuseCache );

    void* pLocalData;
    SIZE_T Bytes;
    if( FAILED( g_pApp->m_pRenderer->LoadVertexBuffer(pLoader) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->DecompressVertexBuffer(pLoader, &pLocalData, &Bytes ) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->ProcessVertexBuffer( pProcessor,pLocalData, Bytes ) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->LockVertexBufferDeviceObject(pProcessor) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->CopyToResourceVertexBuffer(pProcessor) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->UnLockVertexBufferDeviceObject(pProcessor) ) )
	goto Error;

    goto Finish;

Error:
    g_pApp->m_pRenderer->SetProcessorResourceError(pProcessor);
Finish:
    g_pApp->m_pRenderer->DestroyProcessor(pProcessor);
    g_pApp->m_pRenderer->DestroyLoader(pLoader);
}

//--------------------------------------------------------------------------------------
// Serial create IB
//--------------------------------------------------------------------------------------
void	CALLBACK CreateIndexBuffer9_Serial(  IndexBufferID* ppBuffer, UINT iSizeBytes,
					    DWORD Usage, D3DFORMAT ibFormat, D3DPOOL Pool, void* pData,
					    void* pContext )
{
    LoaderID pLoader=g_pApp->m_pRenderer->addIndexBufferLoader();
    ProcessorID pProcessor=g_pApp->m_pRenderer->addIndexBufferProcessor( *ppBuffer, iSizeBytes,  //ibFormat,
								   pData, g_pResourceReuseCache );

    void* pLocalData;
    SIZE_T Bytes;
    if( FAILED( g_pApp->m_pRenderer->LoadIndexBuffer(pLoader) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->DecompressIndexBuffer(pLoader, &pLocalData, &Bytes ) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->ProcessIndexBuffer(pProcessor, pLocalData, Bytes ) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->LockIndexBufferDeviceObject(pProcessor) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->CopyToResourceIndexBuffer(pProcessor) ) )
	goto Error;
    if( FAILED( g_pApp->m_pRenderer->UnLockIndexBufferDeviceObject(pProcessor) ) )
	goto Error;

    goto Finish;

Error:
    g_pApp->m_pRenderer->SetProcessorResourceError(pProcessor);
Finish:
    g_pApp->m_pRenderer->DestroyProcessor(pProcessor);
    g_pApp->m_pRenderer->DestroyLoader(pLoader);
}

//--------------------------------------------------------------------------------------
// Async create texture
//--------------------------------------------------------------------------------------
void	CALLBACK CreateTextureFromFile9_Async(  CHAR* szFileName,
					       TextureID* ppTexture, void* pContext )
{
    CAsyncLoader* pAsyncLoader = ( CAsyncLoader* )pContext;
    if( pAsyncLoader )
    {
	LoaderID pLoader=g_pApp->m_pRenderer->addTextureLoader( szFileName, &g_PackFile );
	ProcessorID pProcessor=g_pApp->m_pRenderer->addTextureProcessor( *ppTexture, g_pResourceReuseCache, g_SkipMips );

	g_pApp->m_pRenderer->AddWorkItem( pLoader,pAsyncLoader, pProcessor, NULL, ( void** )ppTexture );
    }
}

//--------------------------------------------------------------------------------------
// Async create VB
//--------------------------------------------------------------------------------------
void	CALLBACK CreateVertexBuffer9_Async(  VertexBufferID* ppBuffer, UINT iSizeBytes,
					    DWORD Usage, DWORD FVF, D3DPOOL Pool, void* pData, void* pContext )
{
    CAsyncLoader* pAsyncLoader = ( CAsyncLoader* )pContext;
    if( pAsyncLoader )
    {
	LoaderID pLoader=g_pApp->m_pRenderer->addVertexBufferLoader();
	ProcessorID pProcessor=g_pApp->m_pRenderer->addVertexBufferProcessor( *ppBuffer, iSizeBytes,  FVF,
									 pData, g_pResourceReuseCache );

	g_pApp->m_pRenderer->AddWorkItem( pLoader,pAsyncLoader, pProcessor, NULL, ( void** )ppBuffer );
    }
}

//--------------------------------------------------------------------------------------
// Async create IB
//--------------------------------------------------------------------------------------
void	CALLBACK CreateIndexBuffer9_Async(  IndexBufferID* ppBuffer, UINT iSizeBytes,
					   DWORD Usage, D3DFORMAT ibFormat, D3DPOOL Pool, void* pData, void* pContext )
{
    CAsyncLoader* pAsyncLoader = ( CAsyncLoader* )pContext;
    if( pAsyncLoader )
    {
	LoaderID pLoader=g_pApp->m_pRenderer->addIndexBufferLoader();
	ProcessorID pProcessor=g_pApp->m_pRenderer->addIndexBufferProcessor( *ppBuffer, iSizeBytes, /*ibFormat,*/
								        pData, g_pResourceReuseCache );

	g_pApp->m_pRenderer->AddWorkItem( pLoader,pAsyncLoader, pProcessor, NULL, ( void** )ppBuffer );
    }
}
bool CreatePackedFile(  CPackedFile* pF, CHAR* szFileName,
                                    UINT SqrtNumTiles, UINT SidesPerTile, float fWorldScale, float fHeightScale )
{
    bool bRet = false;
    HANDLE hFile;

    pF->m_FileHeader.NumFiles = 4 * SqrtNumTiles * SqrtNumTiles;

    VArray <FILE_INDEX*> TempFileIndices;
    VArray <CHUNK_HEADER*> TempHeaderList;
    VArray <STRING> FullFilePath;

    STRING strDiffuseTexture;
    STRING strNormalTexture;
    if( FAILED( FindSDKMediaFileCch( strDiffuseTexture.str, MAX_PATH,
                                           "ContentStreaming\\2kPanels_Diff.dds" ) ) )
        return false;
    if( FAILED( FindSDKMediaFileCch( strNormalTexture.str, MAX_PATH, "ContentStreaming\\2kPanels_Norm.dds" ) ) )
        return false;

    UINT64 SizeDiffuse = GetSize( strDiffuseTexture.str );
    UINT64 SizeNormal = GetSize( strNormalTexture.str );

    STRING strTerrainHeight;
    if( FAILED( FindSDKMediaFileCch( strTerrainHeight.str, MAX_PATH, "ContentStreaming\\terrain1.bmp" ) ) )
        return false;
    CTerrain Terrain;
    HRESULT hr = Terrain.LoadTerrain( strTerrainHeight.str, SqrtNumTiles, SidesPerTile, fWorldScale, fHeightScale,
                                      true );
    if( FAILED( hr ) )
        return false;

    TERRAIN_TILE* pTile = Terrain.GetTile( 0 );
    UINT64 SizeTerrainVB = pTile->NumVertices * sizeof( TERRAIN_VERTEX );
    UINT64 SizeTerrainIB = Terrain.GetNumIndices() * sizeof( SHORT );

    float fTileWidth = pTile->BBox.max.x - pTile->BBox.min.x;
    float fChunkSpan = sqrtf( ( float )pF->m_MaxChunksMapped ) - 1;
    UINT64 TotalTerrainTileSize = SizeTerrainVB + SizeTerrainIB + SizeDiffuse + SizeNormal;
    UINT64 LastChunkMegs = 0;
    UINT64 ChunkMegs = TotalTerrainTileSize;
    UINT iSqrt = 1;
    UINT64 SizeTry = 512;
    UINT64 VideoMemoryUsage = 0;
    UINT64 PrevVideoMemoryUsage = 0;
    UINT64 VidMemLimit = 512 * 1024 * 1024;
    UINT PrevMaxLoadedTiles = 0;
    UINT MaxLoadedTiles = 0;
    float fLoadingRadius = 0;
    float fPrevLoadingRadius = 0;
    while( VideoMemoryUsage < VidMemLimit )
    {
        LastChunkMegs = ChunkMegs;
        ChunkMegs = TotalTerrainTileSize;

        fPrevLoadingRadius = fLoadingRadius;
        fLoadingRadius = iSqrt * fTileWidth * ( fChunkSpan / 2.0f );
        float fLoadingArea = PI * fLoadingRadius * fLoadingRadius;
        float fTileArea = fTileWidth * fTileWidth;
        PrevMaxLoadedTiles = MaxLoadedTiles;
        MaxLoadedTiles = ( UINT )floorf( fLoadingArea / fTileArea );
        PrevVideoMemoryUsage = VideoMemoryUsage;
        VideoMemoryUsage = ( UINT64 )( MaxLoadedTiles * TotalTerrainTileSize );

        iSqrt ++;
        SizeTry += 32;
    }
    iSqrt --;
    pF->m_MaxChunksMapped = PrevMaxLoadedTiles + 20;
    ChunkMegs = LastChunkMegs;
    VideoMemoryUsage = PrevVideoMemoryUsage;
    fLoadingRadius = fPrevLoadingRadius;

    // Create Chunks
    UINT ChunkSide = SqrtNumTiles;
    int NumChunks = ChunkSide * ChunkSide;

    for( int i = 0; i < NumChunks; i++ )
    {
        CHUNK_HEADER* pChunkHeader = new CHUNK_HEADER;
        TempHeaderList.Add( pChunkHeader );
    }

    // Create indices
    UINT iTile = 0;
    for( UINT y = 0; y < SqrtNumTiles; y++ )
    {
        for( UINT x = 0; x < SqrtNumTiles; x++ )
        {
            UINT ChunkX = x;
            UINT ChunkY = y;
            UINT ChunkIndex = ChunkY * ChunkSide + ChunkX;

            // Tile
            TERRAIN_TILE* pTile = Terrain.GetTile( iTile );
            float3 vCenter = ( pTile->BBox.min + pTile->BBox.max ) / 2.0f;

            // TerrainVB
            FILE_INDEX* pFileIndex = new FILE_INDEX;
            sprintf_s( pFileIndex->szFileName, MAX_PATH, "terrainVB%d_%d", x, y );
            pFileIndex->FileSize = SizeTerrainVB;
            pFileIndex->ChunkIndex = ChunkIndex;
            pFileIndex->OffsetIntoChunk = 0; // unknown
            pFileIndex->vCenter = vCenter;
            TempFileIndices.Add( pFileIndex );

            STRING strTemp;
            strcpy( strTemp.str,  "VB" );
            FullFilePath.Add( strTemp );

            // TerrainIB
            pFileIndex = new FILE_INDEX;
            sprintf_s( pFileIndex->szFileName, MAX_PATH, "terrainIB%d_%d", x, y );
            pFileIndex->FileSize = SizeTerrainIB;
            pFileIndex->ChunkIndex = ChunkIndex;
            pFileIndex->OffsetIntoChunk = 0; // unknown
            pFileIndex->vCenter = vCenter;
            TempFileIndices.Add( pFileIndex );

            strcpy( strTemp.str,  "IB" );
            FullFilePath.Add( strTemp );

            // TerrainDiffuse
            pFileIndex = new FILE_INDEX;
            sprintf_s( pFileIndex->szFileName, MAX_PATH, "terrainDiff%d_%d", x, y );
            pFileIndex->FileSize = SizeDiffuse;
            pFileIndex->ChunkIndex = ChunkIndex;
            pFileIndex->OffsetIntoChunk = 0; // unknown
            pFileIndex->vCenter = vCenter;
            TempFileIndices.Add( pFileIndex );

            FullFilePath.Add( strDiffuseTexture );

            // TerrainDiffuse
            pFileIndex = new FILE_INDEX;
            sprintf_s( pFileIndex->szFileName, MAX_PATH, "terrainNorm%d_%d", x, y );
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
    UINT64 Granularity = SystemInfo.dwAllocationGranularity; // Allocation granularity (always 64k)

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

    UINT64 IndexSize = sizeof( PACKED_FILE_HEADER ) + sizeof( CHUNK_HEADER ) * TempHeaderList.GetSize() + sizeof
        ( FILE_INDEX ) * TempFileIndices.GetSize();
    UINT64 ChunkOffset = AlignToGranularity( IndexSize, Granularity );

    // Align chunks to the proper granularities
    for( int c = 0; c < NumChunks; c++ )
    {
        CHUNK_HEADER* pChunkHeader = TempHeaderList.GetAt( c );
        pChunkHeader->ChunkOffset = ChunkOffset;

        ChunkOffset += AlignToGranularity( pChunkHeader->ChunkSize, Granularity );
    }

    // Fill in the header data
    pF->m_FileHeader.FileSize = ChunkOffset;
    pF->m_FileHeader.NumChunks = TempHeaderList.GetSize();
    pF->m_FileHeader.NumFiles = TempFileIndices.GetSize();
    pF->m_FileHeader.Granularity = Granularity;
    pF->m_FileHeader.MaxChunksInVA = pF->m_MaxChunksMapped;

   pF-> m_FileHeader.TileBytesSize = TotalTerrainTileSize;
    pF->m_FileHeader.TileSideSize = pTile->BBox.max.x - pTile->BBox.min.x;
    pF->m_FileHeader.LoadingRadius = fLoadingRadius;
    pF->m_FileHeader.VideoMemoryUsageAtFullMips = VideoMemoryUsage;

    // Open the file
    hFile = CreateFile( szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN,
                        NULL );
    if( INVALID_HANDLE_VALUE == hFile )
        return bRet;

    // write the header
    DWORD dwWritten;
    DWORD dwRead;
    if( !WriteFile( hFile, &pF->m_FileHeader, sizeof( PACKED_FILE_HEADER ), &dwWritten, NULL ) )
        goto Error;

    // write out chunk headers
    for( int i = 0; i < TempHeaderList.GetSize(); i++ )
    {
        CHUNK_HEADER* pChunkHeader = TempHeaderList.GetAt( i );
        if( !WriteFile( hFile, pChunkHeader, sizeof( CHUNK_HEADER ), &dwWritten, NULL ) )
            goto Error;
    }

    // write the index
    for( int i = 0; i < TempFileIndices.GetSize(); i++ )
    {
        FILE_INDEX* pIndex = TempFileIndices.GetAt( i );
        if( !WriteFile( hFile, pIndex, sizeof( FILE_INDEX ), &dwWritten, NULL ) )
            goto Error;
    }

    // Fill in up to the granularity
    UINT64 CurrentFileSize = IndexSize;
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
                BYTE* pTempData = NULL;

                if( 0 == strcmp( FullFilePath.GetAt( i ).str, "VB" ) )
                {
                    pTempData = ( BYTE* )Terrain.GetTile( i / 4 )->pRawVertices;
                }
                else if( 0 == strcmp( FullFilePath.GetAt( i ).str, "IB" ) )
                {
                    pTempData = ( BYTE* )Terrain.GetIndices();
                }
                else
                {
                    HANDLE hIndexFile = CreateFile( FullFilePath.GetAt( i ).str, FILE_READ_DATA, FILE_SHARE_READ, NULL,
                                                    OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
                    if( INVALID_HANDLE_VALUE == hIndexFile )
                        goto Error;

                    pTempData = new BYTE[ ( SIZE_T )pIndex->FileSize ];
                    if( !pTempData )
                    {
                        CloseHandle( hIndexFile );
                        goto Error;
                    }

                    bDel = true;

                    if( !ReadFile( hIndexFile, pTempData, ( DWORD )pIndex->FileSize, &dwRead, NULL ) )
                    {
                        CloseHandle( hIndexFile );
                        SAFE_DELETE_ARRAY( pTempData );
                        goto Error;
                    }

                    CloseHandle( hIndexFile );
                }

                if( !WriteFile( hFile, pTempData, ( DWORD )pIndex->FileSize, &dwWritten, NULL ) )
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
#endif

