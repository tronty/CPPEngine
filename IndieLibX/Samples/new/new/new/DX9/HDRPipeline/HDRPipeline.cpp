/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//======================================================================
//
//      HIGH DYNAMIC RANGE RENDERING DEMONSTRATION
//      Written by Jack Hoxley, November 2005
//
//======================================================================

#include "HDRScene.h"
#include "Luminance.h"
#include "PostProcess.h"
#include "Enumeration.h"

//--------------------------------------------------------------------------------------
// Data Structure Definitions
//--------------------------------------------------------------------------------------
struct TLVertex
{
    D3DXFROMWINEVECTOR4 p;
    D3DXFROMWINEVECTOR2 t;
};

static const __DWORD__          FVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_TEX1;



//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
TextureID          g_pArrowTex = 0;     // Used to indicate flow between cells
stx_CModelViewerCamera          g_Camera;                               // A model viewing camera
ShaderID      g_pFinalPassPS = 0;     // The pixel shader that maps HDR->LDR
LPD3DXCONSTANTTABLE         g_pFinalPassPSConsts = 0;     // The interface for setting param/const data for the above PS
__DWORD__                       g_dwLastFPSCheck = 0;        // The time index of the last frame rate check
__DWORD__                       g_dwFrameCount = 0;        // How many frames have elapsed since the last check
__DWORD__                       g_dwFrameRate = 0;        // How many frames rendered during the PREVIOUS interval
float                       g_fExposure = 0.50f;    // The exposure bias fed into the FinalPass.psh shader (OnFrameRender )



//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3
#define IDC_THRESHOLDSLIDER     4
#define IDC_THRESHOLDLABEL      5
#define IDC_EXPOSURESLIDER      6
#define IDC_EXPOSURELABEL       7
#define IDC_MULTIPLIERSLIDER    8
#define IDC_MULTIPLIERLABEL     9
#define IDC_MEANSLIDER          10
#define IDC_MEANLABEL           11
#define IDC_STDDEVSLIDER        12
#define IDC_STDDEVLABEL         13



//--------------------------------------------------------------------------------------
// HIGH DYNAMIC RANGE VARIABLES
//--------------------------------------------------------------------------------------
FORMAT                   g_FloatRenderTargetFmt = FORMAT_NONE;   // 128 or 64 bit rendering...
FORMAT                   g_DepthFormat = FORMAT_NONE;   // How many bits of depth buffer are we using?
__DWORD__                       g_dwApproxMemory = 0;                // How many bytes of VRAM are we using up?
TextureID          g_pFinalTexture = 0;

//--------------------------------------------------------------------------------------
// Debug output helper
//--------------------------------------------------------------------------------------
char g_szStaticOutput[ MAX_PATH ];
char* DebugHelper( char* szFormat, unsigned int cSizeBytes )
{
    swprintf_s( g_szStaticOutput, MAX_PATH, szFormat, cSizeBytes );
    return g_szStaticOutput;
}

struct App
{
	App(){}
	virtual ~App(){}

//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
int  OnCreateDevice()
{
    int 0;

    // Determine any necessary enumerations
    ( HDREnumeration::FindBestHDRFormat( &g_FloatRenderTargetFmt ) );

    g_pArrowTex="misc/Arrows.bmp";

    // Setup the camera's view parameters
    D3DXFROMWINEVECTOR3 vecEye( 0.0f, 0.0f, -5.0f );
    D3DXFROMWINEVECTOR3 vecAt ( 0.0f, 0.0f, -0.0f );
    D3DXFROMWINEMatrixLookAtLH(&g_View, &vecEye, &vecAt );


    return 0;

}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
int  OnResetDevice(  const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    int 0;
    LPD3DXBUFFER pCode;

    //IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    //IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

    //IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    //IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

    //IRenderer::GetRendererInstance()->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    //IRenderer::GetRendererInstance()->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

    // Setup the camera's projection parameters
    float fAspectRatio = IRenderer::GetRendererInstance()->GetAspectRatio();
    D3DXFROMWINEMatrixPerspectiveFovLH(&g_Proj, D3DXFROMWINE_PI / 4, fAspectRatio, 0.1f, 1000.0f );

    g_Config.SetLocation( STX_Service::GetWindowInstance()->GetWidth() - 170, STX_Service::GetWindowInstance()->GetHeight() - 300 );
    g_Config.SetSize( 170, 210 );

    // Recreate the floating point resources
    ( IRenderer::GetRendererInstance()->addTexture( STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(),
                                         1, D3DUSAGE_RENDERTARGET, pBackBufferSurfaceDesc->Format,
                                         D3DPOOL_DEFAULT, &g_pFinalTexture, 0 ) );

    //Attempt to recalulate some memory statistics:
    int iMultiplier = 0;
    switch( pBackBufferSurfaceDesc->Format )
    {

            //32bit modes:
        case FORMAT_R32F:
        case FORMAT_RGBA8:
            iMultiplier = 4;
            break;

            //24bit modes:
        case FORMAT_RGB8:
            iMultiplier = 3;
            break;

            //16bit modes:
        case FORMAT_RG16:
            iMultiplier = 2;
            break;

        default:
            iMultiplier = 1;
    }

    //the *3 constant due to having double buffering AND the composition render target..
    g_dwApproxMemory = ( STX_Service::GetWindowInstance()->GetWidth() * STX_Service::GetWindowInstance()->GetHeight() * 3 * iMultiplier );

    switch( g_DepthFormat )
    {

            //16 bit formats
        case FORMAT_D16:
            g_dwApproxMemory += ( STX_Service::GetWindowInstance()->GetWidth() * STX_Service::GetWindowInstance()->GetHeight() * 2 );
            break;

            //32bit formats:
        case D3DFMT_D32:
        case FORMAT_D32F:
        case FORMAT_D24S8:
            g_dwApproxMemory += ( STX_Service::GetWindowInstance()->GetWidth() * STX_Service::GetWindowInstance()->GetHeight() * 4 );
            break;

        default:
            break;
    }

#ifdef _DEBUG
    {
        LOG_PRINT( DebugHelper( "HDRPipeline.cpp : Basic Swap Chain/Depth Buffer Occupy %d bytes.\n", g_dwApproxMemory ) );
    }
    #endif

    g_pFinalPassPS="Shader Code/FinalPass.psh";

    // Hand over execution to the 'HDRScene' module so it can perform it's
    // resource creation:
    ( HDRScene::CreateResources( pBackBufferSurfaceDesc ) );
    g_dwApproxMemory += HDRScene::CalculateResourceUsage();
#ifdef _DEBUG
    {
        LOG_PRINT( DebugHelper( "HDRPipeline.cpp : HDR Scene Resources Occupy %d bytes.\n", HDRScene::CalculateResourceUsage( ) ) );
    }
    #endif

    // Hand over execution to the 'Luminance' module so it can perform it's
    // resource creation:
    ( Luminance::CreateResources( pBackBufferSurfaceDesc ) );
    g_dwApproxMemory += Luminance::ComputeResourceUsage();
#ifdef _DEBUG
    {
        LOG_PRINT( DebugHelper( "HDRPipeline.cpp : Luminance Resources Occupy %d bytes.\n", Luminance::ComputeResourceUsage( ) ) );
    }
    #endif

    // Hand over execution to the 'PostProcess' module so it can perform it's
    // resource creation:
    ( PostProcess::CreateResources( pBackBufferSurfaceDesc ) );
    g_dwApproxMemory += PostProcess::CalculateResourceUsage();
#ifdef _DEBUG
    {
        LOG_PRINT( DebugHelper( "HDRPipeline.cpp : Post Processing Resources Occupy %d bytes.\n", PostProcess::CalculateResourceUsage( ) ) );

        LOG_PRINT( DebugHelper( "HDRPipeline.cpp : Total Graphics Resources Occupy %d bytes.\n", g_dwApproxMemory ) );
    }
    #endif

    return 0;

}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void  OnFrameMove( double fTime, float fElapsedTime )
{

    // Update the camera's position based on user input 
    g_Camera.FrameMove( fElapsedTime );

    // Compute the frame rate based on a 1/4 second update cycle
    if( ( GetTickCount() - g_dwLastFPSCheck ) >= 250 )
    {
        g_dwFrameRate = g_dwFrameCount * 4;
        g_dwFrameCount = 0;
        g_dwLastFPSCheck = GetTickCount();
    }

    ++g_dwFrameCount;

    // The original HDR scene needs to update some of it's internal structures, so
    // pass the message along...
    HDRScene::UpdateScene( DXUTGetD3D9Device(), static_cast< float >( fTime ), &g_Camera );

}


//--------------------------------------------------------------------------------------
//  OnFrameRender()
//  ---------------
//  NOTES: This function makes particular use of the 'D3DPERF_BeginEvent()' and
//         'D3DPERF_EndEvent()' functions. See the documentation for more details,
//         but these are essentially used to provide better output from PIX. If you
//         perform a full call-stream capture on this sample, PIX will group together
//         related calls, making it much easier to interpret the results.
//--------------------------------------------------------------------------------------
void  OnFrameRender(  double fTime, float fElapsedTime )
{

    // 'hr' is used by the '()' and '()' macros
    int hr;

    //Cache some results for later use
    SurfaceID pLDRSurface = 0;

    //Configure the render targets
    ( IRenderer::GetRendererInstance()->GetRenderTarget( 0, &pLDRSurface ) );        //This is the output surface - a standard 32bit device

    // Clear the render target and the zbuffer 
    ( IRenderer::GetRendererInstance()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0xFF, 0xFF, 0xFF, 0xFF ), 1.0f,
                          0 ) );

    // Render the scene
    if( ( IRenderer::GetRendererInstance()->BeginScene() ) )
    {

        // RENDER THE COMPLETE SCENE
        //--------------------------
        // The first part of each frame is to actually render the "core"
        // resources - those that would be required for an HDR-based pipeline.

        // HDRScene creates an unprocessed, raw, image to work with.
        HDRScene::RenderScene(  );

        // Luminance attempts to calculate what sort of tone/mapping should
        // be done as part of the post-processing step.
        Luminance::MeasureLuminance(  );

        // The post-processing adds the blur to the bright (over-exposed) areas
        // of the image.
        PostProcess::PerformPostProcessing(  );

        // When all the individual components have been created we have the final
        // composition. The output of this is the image that will be displayed
        // on screen.
        {
            SurfaceID pFinalSurf = 0;
            g_pFinalTexture->GetSurfaceLevel( 0, &pFinalSurf );
            IRenderer::GetRendererInstance()->changeRenderTarget( 0, pFinalSurf );

            TextureID pHDRTex = 0;
            HDRScene::GetOutputTexture( &pHDRTex );

            TextureID pLumTex = 0;
            Luminance::GetLuminanceTexture( &pLumTex );

            TextureID pBloomTex = 0;
            PostProcess::GetTexture( &pBloomTex );

            IRenderer::GetRendererInstance()->setTexture( 0, pHDRTex );
            IRenderer::GetRendererInstance()->setTexture( 1, pLumTex );
            IRenderer::GetRendererInstance()->setTexture( 2, pBloomTex );

            IRenderer::GetRendererInstance()->SetPixelShader( g_pFinalPassPS );

            D3DSURFACE_DESC d;
            pBloomTex->GetLevelDesc( 0, &d );
            g_pFinalPassPSConsts->SetFloat( "g_rcp_bloom_tex_w", 1.0f / static_cast< float >( d.Width ) );
            g_pFinalPassPSConsts->SetFloat( "g_rcp_bloom_tex_h", 1.0f / static_cast< float >( d.Height ) );
            g_pFinalPassPSConsts->SetFloat( "fExposure", g_fExposure );
            g_pFinalPassPSConsts->SetFloat( "fGaussianScalar", PostProcess::GetGaussianMultiplier() );

            DrawHDRTextureToScreen();

            IRenderer::GetRendererInstance()->SetPixelShader( 0 );

            IRenderer::GetRendererInstance()->setTexture( 2, 0 );
            IRenderer::GetRendererInstance()->setTexture( 1, 0 );
            IRenderer::GetRendererInstance()->setTexture( 0, 0 );

            IRenderer::GetRendererInstance()->changeRenderTarget( 0, pLDRSurface );

            //////////////SAFE_RELEASE( pBloomTex );
            //////////////SAFE_RELEASE( pLumTex );
            //////////////SAFE_RELEASE( pHDRTex );
            //////////////SAFE_RELEASE( pFinalSurf );
        }

        // We've finished all of the rendering, so make sure that D3D
        // is aware of this...
        ( IRenderer::GetRendererInstance()->EndScene() );
    }

    //Remove any memory involved in the render target switching
    //////////////SAFE_RELEASE( pLDRSurface );

}

//--------------------------------------------------------------------------------------
//  The last stage of the HDR pipeline requires us to take the image that was rendered
//  to an HDR-format texture and map it onto a LDR render target that can be displayed
//  on screen. This is done by rendering a screen-space quad and setting a pixel shader
//  up to map HDR->LDR.
//--------------------------------------------------------------------------------------
void DrawHDRTextureToScreen()
{

    // Find out what dimensions the screen is
     IRenderer::GetRendererInstance() = DXUTGetD3D9Device();
    assert( IRenderer::GetRendererInstance() != 0 );
    D3DSURFACE_DESC desc;
    SurfaceID pSurfRT;

    IRenderer::GetRendererInstance()->GetRenderTarget( 0, &pSurfRT );
    pSurfRT->GetDesc( &desc );
    pSurfRT->Release();

    // To correctly map from texels->pixels we offset the coordinates
    // by -0.5f:
    float fWidth = static_cast< float >( desc.Width ) - 0.5f;
    float fHeight = static_cast< float >( desc.Height ) - 0.5f;

    // Now we can actually assemble the screen-space geometry
    TLVertex v[4];

    v[0].p = D3DXFROMWINEVECTOR4( -0.5f, -0.5f, 0.0f, 1.0f );
    v[0].t = D3DXFROMWINEVECTOR2( 0.0f, 0.0f );

    v[1].p = D3DXFROMWINEVECTOR4( fWidth, -0.5f, 0.0f, 1.0f );
    v[1].t = D3DXFROMWINEVECTOR2( 1.0f, 0.0f );

    v[2].p = D3DXFROMWINEVECTOR4( -0.5f, fHeight, 0.0f, 1.0f );
    v[2].t = D3DXFROMWINEVECTOR2( 0.0f, 1.0f );

    v[3].p = D3DXFROMWINEVECTOR4( fWidth, fHeight, 0.0f, 1.0f );
    v[3].t = D3DXFROMWINEVECTOR2( 1.0f, 1.0f );

    // Configure the device and render..
    IRenderer::GetRendererInstance()->SetVertexShader( 0 );
    IRenderer::GetRendererInstance()->SetFVF( FVF_TLVERTEX );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( TLVertex ) );

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
	IRenderer* r=IRenderer::GetRendererInstance("HDRPipeline");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
		IRenderer::GetRendererInstance()->Present( );
	}
	return 0;
}


