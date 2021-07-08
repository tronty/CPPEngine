#ifdef BUILDCONTENTSTREAMING
//--------------------------------------------------------------------------------------
// File: ContentStreaming10.cpp
//
// Illustrates streaming content using Direct3D 10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>
#include "resource.h"
#include "AsyncLoader.h"
//include <FrameworkHLSLCg3/ContentLoaders.h>
#include "ResourceReuseCache.h"
#include "PackedFile.h"
#include "Terrain.h"

//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define DEG2RAD(p) ( PI*(p/180.0f) )
#ifndef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
#endif


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
//CFirstPersonCamera		    g_Camera;		    // A first person camera
	mat4 p,mv,mvp;


// Direct3D 9 resources

extern	ShaderID	g_hRenderTileDiff;
extern	   ShaderID	g_hRenderTileBump;
extern	   ShaderID	g_hRenderTileWire;

//extern D3DXHANDLE		    g_htxDiffuse;
//extern D3DXHANDLE		    g_htxNormal;

CAsyncLoader*		    g_pAsyncLoader=NULL;
CResourceReuseCache*	    g_pResourceReuseCache=NULL;
CPackedFile			    g_PackFile;



float				    g_fFOV = 70.0f;
float				    g_fAspectRatio = 0.0f;
float				    g_fInitialLoadTime = 0.0f;
UINT				    g_NumModelsInUse = 0;
float				    g_fLoadingRadius;
float				    g_fVisibleRadius;
float				    g_fViewHeight = 7.5f;
float				    g_fRotationFrequency = 20.0f;
float				    g_fRotateInLeadTime = 2.0f;
int				    g_NumResourceToLoadPerFrame = 1;
int				    g_UploadToVRamEveryNthFrame = 3;
UINT				    g_SkipMips = 0;
UINT				    g_NumProcessingThreads = 1;
UINT				    g_MaxOutstandingResources = 1500;
UINT64				    g_AvailableVideoMem = 0;
bool				    g_bUseWDDMPaging = false;
bool				    g_bStartupResourcesLoaded = false;
bool				    g_bDrawUI = true;
bool				    g_bWireframe = false;

VArray <LEVEL_ITEM*>	    g_LevelItemArray;
VArray <LEVEL_ITEM*>	    g_VisibleItemArray;
VArray <LEVEL_ITEM*>	    g_LoadedItemArray;
CTerrain			    g_Terrain;

enum LOAD_TYPE
{
    LOAD_TYPE_MULTITHREAD = 0,
    LOAD_TYPE_SINGLETHREAD,
};
LOAD_TYPE			    g_LoadType = LOAD_TYPE_MULTITHREAD;

enum APP_STATE
{
    APP_STATE_STARTUP = 0,
    APP_STATE_RENDER_SCENE
};
APP_STATE			    g_AppState = APP_STATE_STARTUP; // Current state of app

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
// General HUD
#define IDC_TOGGLEFULLSCREEN		1
#define IDC_TOGGLEREF				2
#define IDC_CHANGEDEVICE			3
// StartupUI
#define IDC_LOAD_TYPE_GROUP			4
#define IDC_ONDEMANDSINGLETHREAD	5
#define IDC_ONDEMANDMULTITHREAD 	8
#define IDC_RUN 					9
#define IDC_DELETE_PACK_FILE		10
// SampleUI
#define IDC_VIEWHEIGHT_STATIC		20
#define IDC_VIEWHEIGHT				21
#define IDC_VISIBLERADIUS_STATIC	22
#define IDC_VISIBLERADIUS			23
#define IDC_NUMPERFRAMERES_STATIC	24
#define IDC_NUMPERFRAMERES			25
#define IDC_UPLOADTOVRAMFREQ_STATIC 26
#define IDC_UPLOADTOVRAMFREQ		27
#define IDC_WIREFRAME				28
#define IDC_STARTOVER				29

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( );// const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnResetDevice( );// const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void	CALLBACK OnFrameMove(  double fTime, float fElapsedTime);//, void* pUserContext );
void	CALLBACK OnFrameRender(  double fTime, float fElapsedTime);//, void* pUserContext );

void	CALLBACK OnLostDevice( void* pUserContext );
void	CALLBACK OnDestroyDevice( void* pUserContext );

extern void CALLBACK CreateTextureFromFile9_Serial(  CHAR* szFileName,
						    TextureID* ppTexture, void* pContext );
extern void CALLBACK CreateVertexBuffer9_Serial(  VertexBufferID* ppBuffer,
						 UINT iSizeBytes, DWORD Usage, DWORD FVF, D3DPOOL Pool, void* pData,
						 void* pContext );
extern void CALLBACK CreateIndexBuffer9_Serial(  IndexBufferID* ppBuffer,
						UINT iSizeBytes, DWORD Usage, D3DFORMAT ibFormat, D3DPOOL Pool,
						void* pData, void* pContext );
extern void CALLBACK CreateTextureFromFile9_Async(  CHAR* szFileName,
						   TextureID* ppTexture, void* pContext );
extern void CALLBACK CreateVertexBuffer9_Async(  VertexBufferID* ppBuffer,
						UINT iSizeBytes, DWORD Usage, DWORD FVF, D3DPOOL Pool, void* pData,
						void* pContext );
extern void CALLBACK CreateIndexBuffer9_Async(	IndexBufferID* ppBuffer,
					       UINT iSizeBytes, DWORD Usage, D3DFORMAT ibFormat, D3DPOOL Pool,
					       void* pData, void* pContext );


void InitApp();
void LoadStartupResources(   double fTime );
UINT EnsureResourcesLoaded(   float visradius, float loadradius );
UINT EnsureUnusedResourcesUnloaded(   double fTime );
void CheckForLoadDone(	 );
void SetTextureLOD(   );
void RenderText();
void DestroyAllMeshes(	);

	int init(unsigned int width, unsigned int height)
	{
		LOG_FNLN;
		g_fAspectRatio=g_pApp->width*1.0f/g_pApp->height;
		InitApp();
		OnCreateDevice(  );
		//g_AppState=APP_STATE_RENDER_SCENE;//???
//		  OnResetDevice(  );



	    g_LoadType = LOAD_TYPE_SINGLETHREAD;
	    //g_LoadType = LOAD_TYPE_MULTITHREAD;
	    g_bUseWDDMPaging = false;
		LoadStartupResources( timeGetTime()/1000.0f );


//	    g_AppState = APP_STATE_RENDER_SCENE;
		OnResetDevice(	);
	    g_AppState = APP_STATE_RENDER_SCENE;
LOG_FNLN;

	return 0;

	}
	int render()
	{
		LOG_FNLN;
		float fElapsedTime=timeGetTime()/1000.0f;
		float fTime=fElapsedTime;
		OnFrameMove(  fTime, fElapsedTime );
		OnFrameRender(	fTime, fElapsedTime);
	return 0;
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance();
	r->Init(eGLGLSL_1_1, 640, 512, "ContentStreaming10", false, 0, 0);		
	IInput*    i=STX_Service::GetInputInstance(eSDL_Input);
	init(0,0);
	while (!i->OnKeyPress (KEY_ESCAPE) || i->Quit())
	{
		i->Update();	
		render();
	}
	return 0;
}

#if 0
//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif





	return 0;
}
#endif
//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
void InitApp()
{



}

//--------------------------------------------------------------------------------------
// Load a mesh using one of three techniques
//--------------------------------------------------------------------------------------
void SmartLoadMesh(   LEVEL_ITEM* pItem )
{

    {
	if( LOAD_TYPE_SINGLETHREAD == g_LoadType )
	{
	    BYTE* pData;
	    UINT DataBytes;

	    if( !g_PackFile.GetPackedFile( pItem->szVBName, &pData, &DataBytes ) )
		return;
	    CreateVertexBuffer9_Serial(  &pItem->VB.pVB9, DataBytes, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED,
					pData, NULL );
	    if( !g_PackFile.GetPackedFile( pItem->szIBName, &pData, &DataBytes ) )
		return;
	    CreateIndexBuffer9_Serial(	&pItem->IB.pIB9, DataBytes, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
				       D3DPOOL_MANAGED, pData, NULL );
	    CreateTextureFromFile9_Serial(  pItem->szDiffuseName, &pItem->Diffuse.pTexture9, NULL );
	    CreateTextureFromFile9_Serial(  pItem->szNormalName, &pItem->Normal.pTexture9, NULL );
	}
	else if( LOAD_TYPE_MULTITHREAD == g_LoadType )
	{
	    BYTE* pData;
	    UINT DataBytes;

	    if( !g_PackFile.GetPackedFile( pItem->szVBName, &pData, &DataBytes ) )
		return;
	    CreateVertexBuffer9_Async(	&pItem->VB.pVB9, DataBytes, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED,
				       pData, ( void* )g_pAsyncLoader );
	    if( !g_PackFile.GetPackedFile( pItem->szIBName, &pData, &DataBytes ) )
		return;
	    CreateIndexBuffer9_Async(  &pItem->IB.pIB9, DataBytes, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
				      D3DPOOL_MANAGED, pData, ( void* )g_pAsyncLoader );
	    CreateTextureFromFile9_Async(  pItem->szDiffuseName, &pItem->Diffuse.pTexture9,
					  ( void* )g_pAsyncLoader );
	    CreateTextureFromFile9_Async(  pItem->szNormalName, &pItem->Normal.pTexture9,
					  ( void* )g_pAsyncLoader );
	}
    }

}

//--------------------------------------------------------------------------------------
float RPercent()
{
    float ret = ( float )( ( rand() % 20000 ) - 10000 );
    return ret / 10000.0f;
}

//-------------------------------------------------------------------------------------
#include <ShlObj.h>
const CHAR g_strFile[MAX_PATH] = "\\ContentPackedFile.packedfile";
const UINT64			    g_PackedFileSize = 3408789504u;
void GetPackedFilePath( CHAR* strPath, UINT cchPath )
{
    CHAR strFolder[MAX_PATH] = "\\ContentStreaming";
#if 1
    //SHGetFolderPath( NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, strPath );
	strcat_s( strPath, cchPath, "C:" );
	strcat_s( strPath, cchPath, strFolder );
//    strcat_s( strPath, cchPath, g_strFile );
#else
    if( FAILED( FindSDKMediaFileCch( strPath, cchPath, "ContentPackedFile.packedfile" ) ) )
{
     SHGetFolderPath( NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, strPath );
    strcat( strPath, strFolder );
//  wcscat_s( strPath, cchPath, "D:\\ContentStreaming111\\ContentPackedFile.packedfile");
}
  #endif
}

bool CreatePackedFile(CPackedFile* p,char *,unsigned int,unsigned int,float,float);

//--------------------------------------------------------------------------------------
// Load the resources necessary at the beginning of a level
//--------------------------------------------------------------------------------------
void LoadStartupResources(   double fTime )
{
    if( g_bStartupResourcesLoaded )
	return;

    CHAR strPath[MAX_PATH] = {0};
    CHAR strDirectory[MAX_PATH] = {0};
    GetPackedFilePath( strDirectory, MAX_PATH );

    if( 0xFFFFFFFF == GetFileAttributes( strDirectory ) )
    {
		LOG_PRINT("strDirectory=%s\n",strDirectory);
	// create the directory
	if( !CreateDirectory( strDirectory, NULL ) )
	{
	    MessageBox( NULL, "There was an error creating the pack file.  ContentStreaming will now exit.", "Error",
			MB_OK );
	    PostQuitMessage( 0 );
	    return;
	}
    }

    // Find the pack file
    UINT SqrtNumTiles = 20;
    UINT SidesPerTile = 50;
    float fWorldScale = 6667.0f;
    float fHeightScale = 300.0f;
    strcpy( strPath, strDirectory );
    strcat_s( strPath, MAX_PATH, g_strFile );

LOG_PRINT("\n\n\n");
	LOG_PRINT( strPath);
    LOG_PRINT("\n\n\n");

    bool bCreatePackedFile = false;
    if( 0xFFFFFFFF == GetFileAttributes( strPath ) )
    {
	bCreatePackedFile = true;
    }
    else
    {
	HANDLE hFile = CreateFile( strPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
				   FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( INVALID_HANDLE_VALUE != hFile )
	{
	    // Check against file size in case the process was killed trying to create the file
	    LARGE_INTEGER FileSize;
	    GetFileSizeEx( hFile, &FileSize );
	    UINT64 Size = FileSize.QuadPart;
	    CloseHandle( hFile );

	    if( Size != g_PackedFileSize )
		bCreatePackedFile = true;
	}
	else
	{
	    bCreatePackedFile = true;
	}
    }

    if( bCreatePackedFile )
    {
	// Check for necessary disk space
	ULARGE_INTEGER FreeBytesAvailable;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;
	if( !GetDiskFreeSpaceEx( strDirectory, &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes ) ||
	    FreeBytesAvailable.QuadPart < g_PackedFileSize )
	{
	    CHAR strSizeMessage[1024];
	    sprintf( strSizeMessage,
			     "There is not enough free disk space to create the file %s.  ContentStreaming will now exit.", strPath );
	    MessageBox( NULL, strSizeMessage, "Error", MB_OK );
	    PostQuitMessage( 0 );
	    return;
	}

	if( IDNO == MessageBox( NULL,
				"This is the first time ContentStreaming has been run.	The sample will need to create a 3.3 gigabyte pack file in order to demonstrate loading assets from a packed file format.  Do you wish to continue?", "Warning", MB_YESNO ) )
	{
	    PostQuitMessage( 0 );
	    return;
	}

		LOG_PRINT("strPath=%s\n",strPath);

	if( !CreatePackedFile(&g_PackFile,   strPath, SqrtNumTiles, SidesPerTile, fWorldScale,
					  fHeightScale ) )
	{

	    MessageBox( NULL, "There was an error creating the pack file.  ContentStreaming will now exit.", "Error",
			MB_OK );
	    PostQuitMessage( 0 );
	    return;
	}
    }

    // Create a pseudo terrain
    CHAR str[MAX_PATH];
    if( FAILED( FindSDKMediaFileCch( str, MAX_PATH, "ContentStreaming\\terrain1.bmp" ) ) )
	return;
    if( FAILED( g_Terrain.LoadTerrain( str, SqrtNumTiles, SidesPerTile, fWorldScale, fHeightScale, false ) ) )
	return;

    bool b64Bit = false;
    if( !g_PackFile.LoadPackedFile( strPath, b64Bit, &g_LevelItemArray ) )
    {
	MessageBox( NULL, "There was an error loading the pack file.  ContentStreaming will now exit.", "Error",
		    MB_OK );
	PostQuitMessage( 0 );
	return;
    }

#if defined(_AMD64_)
    // We don't have a VS space limit on X64
    UINT maxChunks = (UINT)g_PackFile.GetNumChunks();
    g_PackFile.SetMaxChunksMapped( maxChunks );
    for(UINT i=0; i<maxChunks; i++)
	g_PackFile.EnsureChunkMapped( i );
#else
    // This ensure that the loading radius can cover at most 9 chunks
    UINT maxChunks = g_PackFile.GetMaxChunksInVA();
    g_PackFile.SetMaxChunksMapped( maxChunks );
#endif
    g_fLoadingRadius = g_PackFile.GetLoadingRadius();
    g_fVisibleRadius = g_fLoadingRadius;
//    g_Camera.SetProjParams( DEG2RAD(g_fFOV), g_fAspectRatio, 0.5f, g_fVisibleRadius );
	g_pApp->m_pRenderer->buildPerspectiveMatrix(g_fFOV, g_fAspectRatio, 0.5f, g_fVisibleRadius);

    // Determine our available texture memory and try to skip mip levels to fit into it
    g_pResourceReuseCache->SetMaxManagedMemory( 
		
g_pApp->m_pRenderer->GetAvailableTextureMem()		
		);//???g_AvailableVideoMem );
    g_SkipMips = 0;
    UINT64 FullUsage = g_PackFile.GetVideoMemoryUsageAtFullMips();
    while( FullUsage > g_AvailableVideoMem )
    {
	FullUsage = FullUsage >> 2;
	g_SkipMips ++;
    }

    sprintf( str, "Visible Radius: %0.2f", g_fVisibleRadius );

	g_pResourceReuseCache->OnDestroy();

    // Tell the resource cache to create all resources
    g_pResourceReuseCache->SetDontCreateResources( FALSE );

    // Use the same random seed each time for consistent repros
    srand( 100 );

    g_bStartupResourcesLoaded = true;
}

//--------------------------------------------------------------------------------------
// GetCameraCullPlanes
//--------------------------------------------------------------------------------------
void GetCameraCullPlanes( float3* p1Normal, float3* p2Normal, float* p1D, float* p2D )
{
    float3 vEye;
    float3 vDir;
    vEye = g_pApp->m_pRenderer->GetEyePt();
    vDir = g_pApp->m_pRenderer->GetLookAtPt() - vEye;
    vec3Normalize( &vDir, &vDir );

    // setup clip planes
    float3 vLeftNormal;
    float3 vRightNormal;
    mat4 mRotLeft;
    mat4 mRotRight;
    float fAngle = PI / 2.0f + ( DEG2RAD(g_fFOV) / 2.0f ) * 1.3333f;
#if 0
    D3DXMatrixRotationY( &mRotLeft, -fAngle );
    D3DXMatrixRotationY( &mRotRight, fAngle );
#else
	makeRotateMatrix(-fAngle,
		      0, 1, 0,
		      mRotLeft);
	makeRotateMatrix(fAngle,
		      0, 1, 0,
		      mRotRight);
#endif


	vec3TransformNormal( &vLeftNormal, &vDir, &mRotLeft );
    vec3TransformNormal( &vRightNormal, &vDir, &mRotRight );
    *p1D = vec3Dot( &vLeftNormal, &vEye );
    *p2D = vec3Dot( &vRightNormal, &vEye );
    *p1Normal = vLeftNormal;
    *p2Normal = vRightNormal;
}

//--------------------------------------------------------------------------------------
// Calculate our visible and potentially visible items
//--------------------------------------------------------------------------------------
void CalculateVisibleItems( float3 vEye, float fVisRadius, float fLoadRadius )
{
    g_VisibleItemArray.RemoveAll();//???.Reset();
    g_LoadedItemArray.RemoveAll();//???Reset();

    // setup cull planes
    float3 vLeftNormal;
    float3 vRightNormal;
    float leftD;
    float rightD;
    GetCameraCullPlanes( &vLeftNormal, &vRightNormal, &leftD, &rightD );
    float fTileSize = g_PackFile.GetTileSideSize();

    for( int i = 0; i < g_LevelItemArray.GetSize(); i++ )
    {
	LEVEL_ITEM* pItem = g_LevelItemArray.GetAt( i );

	// Default is not in loading radius
	pItem->bInLoadRadius = false;

	float3 vDelta = vEye - pItem->vCenter;
	float len2 = vec3Length( &vDelta )*vec3Length( &vDelta );
	if( len2 < fVisRadius * fVisRadius )
	{
	    pItem->bInFrustum = false;

	    if( len2 < fTileSize * fTileSize ||
		( vec3Dot( &pItem->vCenter, &vLeftNormal ) < leftD + fTileSize &&
		  vec3Dot( &pItem->vCenter, &vRightNormal ) < rightD + fTileSize )
		)
	    {
		pItem->bInFrustum = true;
	    }

	    g_VisibleItemArray.Add( pItem );
	}
	if( len2 < fLoadRadius * fLoadRadius )
	{
	    pItem->bInLoadRadius = true;
	    g_LoadedItemArray.Add( pItem );
	}
    }
}

//--------------------------------------------------------------------------------------
// Ensure resources within a certain radius are loaded
//--------------------------------------------------------------------------------------
UINT EnsureResourcesLoaded(   float fVisRadius, float fLoadRadius )
{
    UINT NumToLoad = 0;
    for( int i = 0; i < g_LoadedItemArray.GetSize(); i++ )
    {
	LEVEL_ITEM* pItem = g_LoadedItemArray.GetAt( i );

	if( !pItem->bLoaded && !pItem->bLoading )
	{
	    pItem->bLoading = true;
	    NumToLoad ++;
	    SmartLoadMesh(  pItem );
	}
    }

    return NumToLoad;
}

//--------------------------------------------------------------------------------------
void FreeUpMeshResources( LEVEL_ITEM* pItem )
{
    {
	g_pResourceReuseCache->UnuseDeviceTexture9( pItem->Diffuse.pTexture9 );
	g_pResourceReuseCache->UnuseDeviceTexture9( pItem->Normal.pTexture9 );
	g_pResourceReuseCache->UnuseDeviceVB9( pItem->VB.pVB9 );
	g_pResourceReuseCache->UnuseDeviceIB9( pItem->IB.pIB9 );
    }

}

//--------------------------------------------------------------------------------------
// Ensure resources that are unused are unloaded
//--------------------------------------------------------------------------------------
UINT EnsureUnusedResourcesUnloaded(   double fTime )
{
    UINT NumToUnload = 0;

    for( int i = 0; i < g_LevelItemArray.GetSize(); i++ )
    {
	LEVEL_ITEM* pItem = g_LevelItemArray.GetAt( i );

	if( pItem->bLoaded && !pItem->bInLoadRadius )
	{
	    // Unload the mesh textures from the texture cache
	    FreeUpMeshResources( pItem);
	    pItem->bLoading = false;
	    pItem->bLoaded = false;
	    pItem->bHasBeenRenderedDiffuse = false;
	    pItem->bHasBeenRenderedNormal = false;
	}
    }

    return NumToUnload;
}


//--------------------------------------------------------------------------------------
// If an item is done loading, label it as loaded
//--------------------------------------------------------------------------------------
void CheckForLoadDone(	 )
{

    {
	for( int i = 0; i < g_LevelItemArray.GetSize(); i++ )
	{
	    LEVEL_ITEM* pItem = g_LevelItemArray.GetAt( i );

	    if( pItem->bLoading )
	    {
		if( pItem->VB.pVB9 &&
		    pItem->IB.pIB9 )
		{
		    pItem->bLoading = false;
		    pItem->bLoaded = true;

		    pItem->CurrentCountdownDiff = 5;
		    pItem->CurrentCountdownNorm = 10;
		}
	    }
	}
    }

}

//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
//    CHAR str[MAX_PATH];

    if( g_pResourceReuseCache )
    {
	int NumTextures = g_pResourceReuseCache->GetNumTextures();
	UINT NumUsed = 0;
	UINT64 EstimatedManagedMemory = g_pResourceReuseCache->GetUsedManagedMemory();

	for( int i = 0; i < NumTextures; i++ )
	{
	    DEVICE_TEXTURE* tex = g_pResourceReuseCache->GetTexture( i );
	    if( tex->bInUse )
		NumUsed ++;
	}




	int NumVBs = g_pResourceReuseCache->GetNumVBs();
	int NumIBs = g_pResourceReuseCache->GetNumIBs();
	UINT NumUsedVBs = 0;
	UINT NumUsedIBs = 0;

	for( int i = 0; i < NumVBs; i++ )
	{
	    DEVICE_VERTEX_BUFFER* vb = g_pResourceReuseCache->GetVB( i );
	    if( vb->bInUse )
		NumUsedVBs ++;
	}
	for( int i = 0; i < NumIBs; i++ )
	{
	    DEVICE_INDEX_BUFFER* ib = g_pResourceReuseCache->GetIB( i );
	    if( ib->bInUse )
		NumUsedIBs ++;
	}


    }

}



//--------------------------------------------------------------------------------------
// Render the scene using the programmable pipeline
//--------------------------------------------------------------------------------------
static int			    iFrameNum = 0;
//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime)//, void* pUserContext )
{
    if( APP_STATE_RENDER_SCENE == g_AppState )
    {
	// Update the camera's position based on user input
//	g_Camera.FrameMove( fElapsedTime );

	// Keep us close to the terrain
	float3 vEye = g_pApp->m_pRenderer->GetEyePt();
	float3 vAt = g_pApp->m_pRenderer->GetLookAtPt();
	float3 vDir = vAt - vEye;
	float fHeight = g_Terrain.GetHeightOnMap( &vEye );
	vEye.y = fHeight + g_fViewHeight;
	vAt = vEye + vDir;
//	g_Camera.SetViewParams( &vEye, &vAt );

	g_pApp->m_pRenderer->buildLookAtMatrix(vEye);//, vAt,float3(0,1,0),mv);
	mvp=g_pApp->m_pRenderer->p()*g_pApp->m_pRenderer->mv();



	// Find visible sets
	CalculateVisibleItems( vEye, g_fVisibleRadius, g_fLoadingRadius );

	// Ensure resources within a certian radius are loaded
	EnsureResourcesLoaded(	g_fVisibleRadius, g_fLoadingRadius );

	// Never unload when using WDDM paging
	if( !g_bUseWDDMPaging )
	    EnsureUnusedResourcesUnloaded(  fTime );

	CheckForLoadDone( );
    }
}



//--------------------------------------------------------------------------------------
void DestroyAllMeshes(	)
{
    // Wait for everything to load
    if( LOAD_TYPE_MULTITHREAD == g_LoadType )
	g_pAsyncLoader->WaitForAllItems();

    g_pResourceReuseCache->OnDestroy();

    // Destroy the level-item array
    for( int i = 0; i < g_LevelItemArray.GetSize(); i++ )
    {
	LEVEL_ITEM* pItem = g_LevelItemArray.GetAt( i );
	SAFE_DELETE( pItem );
    }
    g_LevelItemArray.RemoveAll();
    g_VisibleItemArray.RemoveAll();
    g_LoadedItemArray.RemoveAll();
}
#endif

