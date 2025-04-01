//--------------------------------------------------------------------------------------
// File: ContentStreaming9.cpp
//
// Illustrates streaming content using Direct3D 9
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
#include "AsyncLoader.h"
#include "ContentLoaders.h"
#include "ResourceReuseCache.h"
#include "PackedFile.h"
#include "Terrain.h"

//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define DEG2RAD(p) ( D3DXFROMWINE_PI*(p/180.0f) )

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
extern D3DXFROMWINEMATRIX           g_Proj;               // A first person camera
extern D3DXFROMWINEMATRIX           g_View;               // A first person camera
extern CAsyncLoader*                g_pAsyncLoader;
extern CResourceReuseCache*         g_pResourceReuseCache;
extern unsigned int                         g_NumProcessingThreads;
extern unsigned int                         g_MaxOutstandingResources;
extern CResourceReuseCache*         g_pResourceReuseCache;
extern CPackedFile                  g_PackFile;

// Direct3D 9 resources
stx_Effect*                        g_pEffect9 = 0;
VertexFormatID        g_pDeclTile = 0;

// Effect variable handles
stx_Effect::stx_Variable*                          g_hTimeShift = 0;
stx_Effect::stx_Variable*                          g_hmWorld = 0;
stx_Effect::stx_Variable*                          g_hmWorldViewProjection = 0;
stx_Effect::stx_Variable*                          g_hEyePt = 0;
stx_Effect::stx_Variable*                          g_htxDiffuse = 0;
stx_Effect::stx_Variable*                          g_htxNormal = 0;
stx_Effect::stx_Technique*                         g_hRenderTileDiff = 0;
stx_Effect::stx_Technique*                         g_hRenderTileBump = 0;
stx_Effect::stx_Technique*                         g_hRenderTileWire = 0;

extern CGrowableArray <LEVEL_ITEM*> g_VisibleItemArray;
extern CTerrain                     g_Terrain;

extern float                        g_fFOV;
extern float                        g_fAspectRatio;
extern float                        g_fInitialLoadTime;
extern float                        g_LoadingRadius;
extern int                          g_NumResourceToLoadPerFrame;
extern int                          g_UploadToVRamEveryNthFrame;
extern unsigned int                         g_SkipMips;
extern unsigned int                         g_NumMeshPlacements;
extern unsigned int                         g_NumLevelPieces;
extern unsigned int                         g_NumLevelItems;
extern bool                         g_bStartupResourcesLoaded;
extern bool                         g_bDrawUI;
extern bool                         g_bWireframe;
extern uint64                       g_AvailableVideoMem;

enum LOAD_TYPE
{
    LOAD_TYPE_MULTITHREAD = 0,
    LOAD_TYPE_SINGLETHREAD,
};
extern LOAD_TYPE                    g_LoadType;

enum APP_STATE
{
    APP_STATE_STARTUP = 0,
    APP_STATE_RENDER_SCENE
};
extern APP_STATE                    g_AppState;

struct App
{
	App(){iFrameNum = 0;}
	virtual ~App(){}
	D3DXFROMWINEVECTOR3 g_EyePt;
//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
int  OnCreateDevice()
{
    int hr;

    // Read the D3DXFROMWINE effect file
    g_pEffect9=IRenderer::GetRendererInstance()->addEffectFromFile("/ContentStreaming/ContentStreaming.fx");

    // Get the effect variable handles
    g_hmWorld = g_pEffect9->GetVariableByName("g_mWorld" );
    g_hmWorldViewProjection = g_pEffect9->GetVariableByName("g_mWorldViewProj" );
    g_hEyePt = g_pEffect9->GetVariableByName("g_vEyePt" );
    g_htxDiffuse = g_pEffect9->GetVariableByName("g_txDiffuse" );
    g_htxNormal = g_pEffect9->GetVariableByName("g_txNormal" );

    // Create a decl for the object data.
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	g_pDeclTile = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);

    // Setup the camera's view parameters
    D3DXFROMWINEVECTOR3 vecEye( 0.0f, 2.0f, 0.0f );
    D3DXFROMWINEVECTOR3 vecAt ( 0.0f, 2.0f, 1.0f );
    D3DXFROMWINEVECTOR3 vecUp( 0.0f, 1.0f, 0.0f );
    D3DXFROMWINEMatrixLookAtLH(&g_View, &vecEye, &vecAt, &vecUp );
	g_EyePt=vecEye;

    // Create the async loader
    g_pAsyncLoader = new CAsyncLoader( g_NumProcessingThreads );
    if( !g_pAsyncLoader )
        return 1;

    // Create the texture reuse cache
    g_pResourceReuseCache = new CResourceReuseCache();
    if( !g_pResourceReuseCache )
        return 1;

    return 0;
}

UINT32 GetAdapterRAMFromWMI()
{
    return IRenderer::GetRendererInstance()->GetAvailableTextureMem();
}

//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
int  OnResetDevice()
{
    int hr;

    // Setup the camera's projection parameters
    float fAspect = float(IRenderer::GetRendererInstance()->GetViewportWidth())/IRenderer::GetRendererInstance()->GetViewportHeight();
    D3DXFROMWINEMatrixPerspectiveFovLH( &g_Proj, DEG2RAD(g_fFOV), g_fAspectRatio, 0.1f, 1500.0f );

    // Load resources if they haven't been already (for a device reset)
    // ??? if( APP_STATE_RENDER_SCENE == g_AppState ) LoadStartupResources( 0, timeGetTime() );

    return 0;
}

//--------------------------------------------------------------------------------------
// Render the scene using the programmable pipeline
//--------------------------------------------------------------------------------------
int                          iFrameNum;
void RenderScene( double fTime, float fElapsedTime )
{
    int hr;
    D3DXFROMWINEMATRIX mWorld;
    D3DXFROMWINEMATRIX mView;
    D3DXFROMWINEMATRIX mProj;
    D3DXFROMWINEMATRIX mWorldViewProjection;

    // Get the projection & view matrix from the camera class
    mProj = g_Proj;
    mView = g_View;

    // Set the eye vector
    D3DXFROMWINEVECTOR3 vEyePt = g_EyePt;
    D3DXFROMWINEVECTOR4 vEyePt4;
    vEyePt4.x = vEyePt.x;
    vEyePt4.y = vEyePt.y;
    vEyePt4.z = vEyePt.z;
	g_hEyePt->SetVector( &vEyePt4 );

    int NewTextureUploadsToVidMem = 0;
    if( iFrameNum % g_UploadToVRamEveryNthFrame > 0 )
        NewTextureUploadsToVidMem = g_NumResourceToLoadPerFrame;
    iFrameNum ++;

    // Render the level
    IRenderer::GetRendererInstance()->SetVertexDeclaration( g_pDeclTile );
    for( int i = 0; i < g_VisibleItemArray.size(); i++ )
    {
        LEVEL_ITEM* pItem = g_VisibleItemArray.at( i );

        D3DXFROMWINEMatrixIdentity( &mWorld );
        mWorldViewProjection = mWorld * mView * mProj;

        g_hmWorldViewProjection->SetMatrix( &mWorldViewProjection );
 	g_hmWorld->SetMatrix( &mWorld );

        if( pItem->bLoaded )
        {
            IRenderer::GetRendererInstance()->SetStreamSource( 0, &pItem->VB.pVB9, 0, sizeof( TERRAIN_VERTEX ) );
            IRenderer::GetRendererInstance()->SetIndices( &pItem->IB.pIB9 );

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
                g_pEffect9->SetTechnique( "RenderTileWire" );
                g_htxDiffuse->SetTexture( pItem->Diffuse.pTexture9 );
            }
            else if( bCanRenderNorm )
            {
                pItem->bHasBeenRenderedNormal = true;
                g_pEffect9->SetTechnique( "RenderTileBump" );
                g_htxDiffuse->SetTexture( pItem->Diffuse.pTexture9 );
                g_htxNormal->SetTexture( pItem->Normal.pTexture9 );
            }
            else if( bCanRenderDiff )
            {
                pItem->bHasBeenRenderedDiffuse = true;
                g_pEffect9->SetTechnique( "RenderTileDiff" );
                g_htxDiffuse->SetTexture( pItem->Diffuse.pTexture9 );
            }

            // Apply the technique contained in the effect 
            unsigned int cPasses = 0;
            ( g_pEffect9->Begin( &cPasses, 0 ) );

            for( unsigned int iPass = 0; iPass < cPasses; iPass++ )
            {
                ( g_pEffect9->BeginPass( iPass ) );

                IRenderer::GetRendererInstance()->DrawIndexedPrimitive( PRIM_TRIANGLE_STRIP, 0, 0, g_Terrain.GetNumTileVertices(), 0,
                                                  g_Terrain.GetNumIndices() - 2 );

                ( g_pEffect9->EndPass() );
            }
            ( g_pEffect9->End() );
        }
        else
        {
            // TODO: draw default object here
        }
    }
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void  OnFrameRender( double fTime, float fElapsedTime )
{
    int hr = 0;

    // Clear the render target and the zbuffer 
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0, 160, 160, 250)/256.0f);

    // Render the scene
    IRenderer::GetRendererInstance()->BeginScene();
    {
        if( APP_STATE_RENDER_SCENE == g_AppState )
            RenderScene( fTime, fElapsedTime );

        ( IRenderer::GetRendererInstance()->EndScene() );
    }

    // Load in up to g_NumResourceToLoadPerFrame resources at the end of every frame
    if( LOAD_TYPE_MULTITHREAD == g_LoadType && APP_STATE_RENDER_SCENE == g_AppState )
    {
        unsigned int NumResToProcess = g_NumResourceToLoadPerFrame;
        g_pAsyncLoader->ProcessDeviceWorkItems( NumResToProcess );
    }
}

//--------------------------------------------------------------------------------------
// Serial create texture
//--------------------------------------------------------------------------------------
void	 CreateTextureFromFile9_Serial( char* szFileName,
                                                TextureID  ppTexture, void* pContext )
{
    CTextureLoader* pLoader = new CTextureLoader( szFileName, &g_PackFile );
    CTextureProcessor* pProcessor = new CTextureProcessor( ppTexture, g_pResourceReuseCache, g_SkipMips );

    void* pLocalData;
    SIZE_T Bytes;
    if( FAILED( pLoader->Load() ) )
        goto Error;
    if( FAILED( pLoader->Decompress( &pLocalData, &Bytes ) ) )
        goto Error;
    if( FAILED( pProcessor->Process( pLocalData, Bytes ) ) )
        goto Error;
    if( FAILED( pProcessor->LockDeviceObject() ) )
        goto Error;
    if( FAILED( pProcessor->CopyToResource() ) )
        goto Error;
    if( FAILED( pProcessor->UnLockDeviceObject() ) )
        goto Error;

    goto Finish;

Error:
    pProcessor->SetResourceError();
Finish:
    pProcessor->Destroy();
    pLoader->Destroy();
    SAFE_DELETE( pLoader );
    SAFE_DELETE( pProcessor );
}

//--------------------------------------------------------------------------------------
// Serial create VB
//--------------------------------------------------------------------------------------
void	 CreateVertexBuffer9_Serial( VertexBufferID  ppBuffer,
                                             unsigned int iSizeBytes, __DWORD__ Usage, __DWORD__ FVF, D3DPOOL Pool, void* pData,
                                             void* pContext )
{
    CVertexBufferLoader* pLoader = new CVertexBufferLoader();
    CVertexBufferProcessor* pProcessor = new CVertexBufferProcessor( ppBuffer, iSizeBytes, Usage, FVF, Pool,
                                                                     pData, g_pResourceReuseCache );

    void* pLocalData;
    SIZE_T Bytes;
    if( FAILED( pLoader->Load() ) )
        goto Error;
    if( FAILED( pLoader->Decompress( &pLocalData, &Bytes ) ) )
        goto Error;
    if( FAILED( pProcessor->Process( pLocalData, Bytes ) ) )
        goto Error;
    if( FAILED( pProcessor->LockDeviceObject() ) )
        goto Error;
    if( FAILED( pProcessor->CopyToResource() ) )
        goto Error;
    if( FAILED( pProcessor->UnLockDeviceObject() ) )
        goto Error;

    goto Finish;

Error:
    pProcessor->SetResourceError();
Finish:
    pProcessor->Destroy();
    pLoader->Destroy();
    SAFE_DELETE( pLoader );
    SAFE_DELETE( pProcessor );
}

//--------------------------------------------------------------------------------------
// Serial create IB
//--------------------------------------------------------------------------------------
void	 CreateIndexBuffer9_Serial( IndexBufferID  ppBuffer, unsigned int iSizeBytes,
                                            __DWORD__ Usage, FORMAT ibFormat, D3DPOOL Pool, void* pData,
                                            void* pContext )
{
    CIndexBufferLoader* pLoader = new CIndexBufferLoader();
    CIndexBufferProcessor* pProcessor = new CIndexBufferProcessor( ppBuffer, iSizeBytes, Usage, ibFormat, Pool,
                                                                   pData, g_pResourceReuseCache );

    void* pLocalData;
    SIZE_T Bytes;
    if( FAILED( pLoader->Load() ) )
        goto Error;
    if( FAILED( pLoader->Decompress( &pLocalData, &Bytes ) ) )
        goto Error;
    if( FAILED( pProcessor->Process( pLocalData, Bytes ) ) )
        goto Error;
    if( FAILED( pProcessor->LockDeviceObject() ) )
        goto Error;
    if( FAILED( pProcessor->CopyToResource() ) )
        goto Error;
    if( FAILED( pProcessor->UnLockDeviceObject() ) )
        goto Error;

    goto Finish;

Error:
    pProcessor->SetResourceError();
Finish:
    pProcessor->Destroy();
    pLoader->Destroy();
    SAFE_DELETE( pLoader );
    SAFE_DELETE( pProcessor );
}

//--------------------------------------------------------------------------------------
// Async create texture
//--------------------------------------------------------------------------------------
void	 CreateTextureFromFile9_Async( char* szFileName,
                                               TextureID  ppTexture, void* pContext )
{
    CAsyncLoader* pAsyncLoader = ( CAsyncLoader* )pContext;
    if( pAsyncLoader )
    {
        CTextureLoader* pLoader = new CTextureLoader( szFileName, &g_PackFile );
        CTextureProcessor* pProcessor = new CTextureProcessor( ppTexture, g_pResourceReuseCache, g_SkipMips );

        pAsyncLoader->AddWorkItem( pLoader, pProcessor, 0, ( void** )ppTexture );
    }
}

//--------------------------------------------------------------------------------------
// Async create VB
//--------------------------------------------------------------------------------------
void	 CreateVertexBuffer9_Async( VertexBufferID  ppBuffer, unsigned int iSizeBytes,
                                            __DWORD__ Usage, __DWORD__ FVF, D3DPOOL Pool, void* pData, void* pContext )
{
    CAsyncLoader* pAsyncLoader = ( CAsyncLoader* )pContext;
    if( pAsyncLoader )
    {
        CVertexBufferLoader* pLoader = new CVertexBufferLoader();
        CVertexBufferProcessor* pProcessor = new CVertexBufferProcessor( ppBuffer, iSizeBytes, Usage, FVF, Pool,
                                                                         pData, g_pResourceReuseCache );

        pAsyncLoader->AddWorkItem( pLoader, pProcessor, 0, ( void** )ppBuffer );
    }
}

//--------------------------------------------------------------------------------------
// Async create IB
//--------------------------------------------------------------------------------------
void	 CreateIndexBuffer9_Async( IndexBufferID  ppBuffer, unsigned int iSizeBytes,
                                           __DWORD__ Usage, FORMAT ibFormat, D3DPOOL Pool, void* pData, void* pContext )
{
    CAsyncLoader* pAsyncLoader = ( CAsyncLoader* )pContext;
    if( pAsyncLoader )
    {
        CIndexBufferLoader* pLoader = new CIndexBufferLoader();
        CIndexBufferProcessor* pProcessor = new CIndexBufferProcessor( ppBuffer, iSizeBytes, Usage, ibFormat,
                                                                       Pool, pData, g_pResourceReuseCache );

        pAsyncLoader->AddWorkItem( pLoader, pProcessor, 0, ( void** )ppBuffer );
    }
}
};
App app;
int init(const char* aTitle)
{
    	app.OnCreateDevice();	
	return 0;
}

void render()
{
   double fTime=0;
   float fElapsedTime=0.0f;
   app.OnFrameRender(fTime, fElapsedTime);
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("ContentStreaming");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		IRenderer::GetRendererInstance()->BeginScene();
		render();
		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present( );
	}
	return 0;
}


