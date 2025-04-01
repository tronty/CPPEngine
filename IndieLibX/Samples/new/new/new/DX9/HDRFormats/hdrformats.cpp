/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// File: HDRFormats.cpp
//
// Desc: This sample shows how to do a single pass motion blur effect using 
//       floating point textures and multiple render targets.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define NUM_TONEMAP_TEXTURES  5       // Number of stages in the 3x3 down-scaling 
// of average luminance textures
#define NUM_BLOOM_TEXTURES    2
#define RGB16_MAX             100

enum ENCODING_MODE
{
    FP16,
    FP32,
    RGB16,
    RGBE8,
    NUM_ENCODING_MODES
};
enum RENDER_MODE
{
    DECODED,
    RGB_ENCODED,
    ALPHA_ENCODED,
    NUM_RENDER_MODES
};

struct TECH_HANDLES
{
    stx_Effect::stx_Technique* Scene;
    stx_Effect::stx_Technique* DownScale2x2_Lum;
    stx_Effect::stx_Technique* DownScale3x3;
    stx_Effect::stx_Technique* DownScale3x3_BrightPass;
    stx_Effect::stx_Technique* FinalPass;
};

struct SCREEN_VERTEX
{
    D3DXFROMWINEVECTOR4 pos;
    D3DXFROMWINEVECTOR2 tex;

    static const __DWORD__ FVF;
};
const __DWORD__                 SCREEN_VERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;


//-----------------------------------------------------------------------------
// Globals variables and definitions
//-----------------------------------------------------------------------------
stx_CModelViewerCamera          g_Camera;
stx_Effect*                g_pEffect;
stx_SkyBox g_aSkybox[NUM_ENCODING_MODES];
SurfaceID           g_pMSRT = 0;       // Multi-Sample float render target
SurfaceID           g_pMSDS = 0;       // Depth Stencil surface for the float RT
TextureID          g_pTexRender;         // Render target texture
TextureID          g_pTexBrightPass;     // Bright pass filter
MeshRenderer2                  g_pMesh;
TextureID g_apTexToneMap[NUM_TONEMAP_TEXTURES]; // Tone mapping calculation textures
TextureID g_apTexBloom[NUM_BLOOM_TEXTURES];     // Blooming effect intermediate texture
bool                        g_bBloom;             // Bloom effect on/off
ENCODING_MODE               g_eEncodingMode;
RENDER_MODE                 g_eRenderMode;
stx_Effect::stx_Technique* g_aTechHandles[NUM_ENCODING_MODES];
stx_Effect::stx_Technique* g_pCurTechnique;
bool                        g_bShowHelp;
bool                        g_bShowText;
double g_aPowsOfTwo[257]; // Lookup table for log calculations
bool                        g_bSupportsR16F = false;
bool                        g_bSupportsR32F = false;
bool                        g_bSupportsD16 = false;
bool                        g_bSupportsD32 = false;
bool                        g_bSupportsD24X8 = false;
bool                        g_bUseMultiSample = false; // True when using multisampling on a supported back buffer
MULTISAMPLE_TYPE         g_MaxMultiSampleType = MULTISAMPLE_NONE;  // Non-Zero when g_bUseMultiSample is true
__DWORD__                       g_dwMultiSampleQuality = 0; // Used when we have multisampling on a backbuffer
D3DXFROMWINEMATRIX g_View;
D3DXFROMWINEMATRIX g_Proj;
D3DXFROMWINEMATRIX g_World;
D3DXFROMWINEVECTOR3 g_EyePt;

struct App
{
	App(){}
	virtual ~App(){}
//-----------------------------------------------------------------------------
// Name: AppInit()
// Desc: Sets attributes for the app.
//-----------------------------------------------------------------------------
void AppInit()
{
    g_pEffect = 0;
    g_bShowHelp = false;
    g_bShowText = true;

    g_pTexRender = 0;

    g_bBloom = TRUE;
    g_eEncodingMode = RGBE8;
    g_eRenderMode = DECODED;

    g_pCurTechnique = g_aTechHandles[ g_eEncodingMode ];

    for( int i = 0; i <= 256; i++ )
    {
        g_aPowsOfTwo[i] = powf( 2.0f, ( float )( i - 128 ) );
    }

    ZeroMemory( g_apTexToneMap, sizeof( g_apTexToneMap ) );
    ZeroMemory( g_apTexBloom, sizeof( g_apTexBloom ) );
    ZeroMemory( g_aTechHandles, sizeof( g_aTechHandles ) );

    const char* RENDER_MODE_NAMES[] =
    {
        "Decoded scene",
        "RGB channels",
        "Alpha channel",
    };
}
//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// created, which will happen during application initialization and windowed/full screen 
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these 
// resources need to be reloaded whenever the device is destroyed. Resources created  
// here should be released in the OnDestroyDevice callback. 
//--------------------------------------------------------------------------------------
int  OnCreateDevice()
{
    g_pEffect=IRenderer::GetRendererInstance()->addEffectFromFile("HDRFormats.fx");

    RetrieveTechHandles();

    bool bCreatedTexture = false;
    TextureID  pCubeTexture = 0;
    TextureID  pEncodedTexture = 0;

    char strPath[MAX_PATH];
    stx_snprintf(strPath, MAX_PATH, "Light Probes/uffizi_cross.dds");

    g_bSupportsR16F = true;

    g_bSupportsR32F = true;

    bool bSupports128FCube = true;
    // FP16
    if( g_bSupportsR16F && bSupports128FCube )
    {
        // Device supports floating-point textures.
	pCubeTexture=IRenderer::GetRendererInstance()->addImageLibTexture(strPath, false, IRenderer::GetRendererInstance()->GetbilinearClamp()); // FORMAT_RGBA16f
        ( g_aSkybox[FP16].OnCreateDevice( 50, pCubeTexture, "skybox.fx" ) );
    }
    // FP32
    if( g_bSupportsR32F && bSupports128FCube )
    {
        // Device supports floating-point textures.
	pCubeTexture=IRenderer::GetRendererInstance()->addImageLibTexture(strPath, false, IRenderer::GetRendererInstance()->GetbilinearClamp()); // , FORMAT_RGBA16F
        g_aSkybox[FP32].OnCreateDevice( 50, pCubeTexture, "skybox.fx");
    }
    if( ( !g_bSupportsR32F && !g_bSupportsR16F ) || !bSupports128FCube )
    {
        // Device doesn't support floating-point textures. Use the scratch pool to load it temporarily
        // in order to create encoded textures from it.
        bCreatedTexture = true;
	pCubeTexture=IRenderer::GetRendererInstance()->addImageLibTexture(strPath, false, IRenderer::GetRendererInstance()->GetbilinearClamp()); // , FORMAT_RGBA16F
    }
    // RGB16
    ( CreateEncodedTexture( pCubeTexture, &pEncodedTexture, RGB16 ) );
    ( g_aSkybox[RGB16].OnCreateDevice( 50, pEncodedTexture, "skybox.fx" ) );
    // RGBE8
    ( CreateEncodedTexture( pCubeTexture, &pEncodedTexture, RGBE8 ) );
    ( g_aSkybox[RGBE8].OnCreateDevice( 50, pEncodedTexture, "skybox.fx" ) );

    g_pMesh.Load("misc/teapot.x", eSimpleShader, true, true);

    D3DXFROMWINEVECTOR3 vecEye( 0.0f, 0.0f, -5.0f );
    D3DXFROMWINEVECTOR3 vecAt ( 0.0f, 0.0f, 0.0f );
    D3DXFROMWINEVECTOR3 vecUp ( 0.0f, 1.0f, 0.0f );
    D3DXFROMWINEMatrixLookAtLH(&g_View, &vecEye, &vecAt, &vecUp );

    return 0;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// reset, which will happen after a lost device scenario. This is the best location to 
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever 
// the device is lost. Resources created here should be released in the OnLostDevice 
// callback. 
//--------------------------------------------------------------------------------------
int  OnResetDevice()
{
    

    FORMAT fmt = FORMAT_NONE;
    switch( g_eEncodingMode )
    {
        case FP16:
            fmt = FORMAT_RGBA16f; break;
        case FP32:
            fmt = FORMAT_RGBA16f; break;
        case RGBE8:
            fmt = FORMAT_RGBA8; break;
        case RGB16:
            fmt = FORMAT_RGBA16; break;
    }

    g_pTexRender = IRenderer::GetRendererInstance()->addRenderTarget(STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(), fmt, IRenderer::GetRendererInstance()->GetnearestClamp());

    g_pTexBrightPass = IRenderer::GetRendererInstance()->addRenderTarget(STX_Service::GetWindowInstance()->GetWidth() / 8, STX_Service::GetWindowInstance()->GetHeight() / 8, FORMAT_RGBA8, IRenderer::GetRendererInstance()->GetnearestClamp());

    // Determine whether we can and should support a multisampling on the HDR render target
    g_bUseMultiSample = false;

    g_bSupportsD16 = true;
    g_bSupportsD32 = false;
    g_bSupportsD24X8 = false;

    FORMAT dfmt = FORMAT_NONE;
    if( g_bSupportsD16 )
        dfmt = FORMAT_D16;

    if( dfmt != FORMAT_NONE )
    {
        g_MaxMultiSampleType = MULTISAMPLE_NONE;
        for( MULTISAMPLE_TYPE imst = MULTISAMPLE_2_SAMPLES; imst <= MULTISAMPLE_16_SAMPLES;
             imst = ( MULTISAMPLE_TYPE )( imst + 1 ) )
        {
            __DWORD__ msQuality = 0;
            if(1)
            {
                g_bUseMultiSample = true;
                g_MaxMultiSampleType = imst;
                if( msQuality > 0 )
                    g_dwMultiSampleQuality = msQuality - 1;
                else
                    g_dwMultiSampleQuality = msQuality;
            }
        }

        // Create the Multi-Sample floating point render target
        if( g_bUseMultiSample )
        {
            IRenderer::GetRendererInstance()->CreateRenderTarget( STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(), fmt);
            {
                IRenderer::GetRendererInstance()->CreateDepthStencilSurface( STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(), dfmt);
                {
                    g_bUseMultiSample = false;
                    //////////////SAFE_RELEASE( g_pMSRT );
                }
            }
        }
    }

    // For each scale stage, create a texture to hold the intermediate results
    // of the luminance calculation
    int nSampleLen = 1;
    unsigned int i = 0;
    for( i = 0; i < NUM_TONEMAP_TEXTURES; i++ )
    {
        fmt = FORMAT_NONE;
        switch( g_eEncodingMode )
        {
            case FP16:
                fmt = FORMAT_R16f; break;
            case FP32:
                fmt = FORMAT_R32f; break;
            case RGBE8:
                fmt = FORMAT_RGBA8; break;
            case RGB16:
                fmt = FORMAT_RGBA16; break;
        }

        g_apTexToneMap[i]=IRenderer::GetRendererInstance()->addRenderTarget( nSampleLen, nSampleLen, fmt, IRenderer::GetRendererInstance()->GetnearestClamp());

        nSampleLen *= 3;
    }

    // Create the temporary blooming effect textures
    for( i = 0; i < NUM_BLOOM_TEXTURES; i++ )
    {
        g_apTexBloom[i]=IRenderer::GetRendererInstance()->addRenderTarget( STX_Service::GetWindowInstance()->GetWidth() / 8, STX_Service::GetWindowInstance()->GetHeight() / 8, FORMAT_RGBA8, IRenderer::GetRendererInstance()->GetnearestClamp());
    }

    // Setup the camera's projection parameters
    float fAspectRatio = IRenderer::GetRendererInstance()->GetAspectRatio();
    D3DXFROMWINEMatrixPerspectiveFovLH(&g_Proj, D3DXFROMWINE_PI / 4, fAspectRatio, 0.1f, 5000.0f );

    return 0;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void  OnFrameMove( double fTime, float fElapsedTime )
{
    // Update the camera's postion based on user input 
    g_pEffect->SetValue( "g_vEyePt", g_EyePt, sizeof( D3DXFROMWINEVECTOR3 ) );
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, DXUT will call 
// ::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void  OnFrameRender(  double fTime, float fElapsedTime )
{
    // Setup the HDR render target
    if( g_bUseMultiSample )
    {
	IRenderer::GetRendererInstance()->changeRenderTarget( g_pMSRT );
	IRenderer::GetRendererInstance()->SetDepthStencilSurface( g_pMSDS );
    }
    else
    {
	IRenderer::GetRendererInstance()->changeRenderTarget( g_pTexRender );
    }

    // Clear the render target
	IRenderer::GetRendererInstance()->Clear(true, true, stx_GetRGBA(0x000000FF));

    // Update matrices
    D3DXFROMWINEMATRIX mWorld;
    D3DXFROMWINEMATRIX mWorldView;
    D3DXFROMWINEMATRIX mWorldViewProj;

    D3DXFROMWINEMatrixMultiply( &mWorldViewProj, &g_View, &g_Proj );
    g_pEffect->SetMatrix( "g_mWorldViewProj", &mWorldViewProj );

    // For the first pass we'll draw the screen to the full screen render target
    // and to update the velocity render target with the velocity of each pixel
    IRenderer::GetRendererInstance()->BeginScene();
    {
        // Draw the skybox
        g_aSkybox[g_eEncodingMode].render( &mWorldViewProj );

        RenderModel();

        // If using floating point multi sampling, stretchrect to the rendertarget
        if( g_bUseMultiSample )
        {
	// ??? IRenderer::GetRendererInstance()->StretchRect( g_pMSRT, 0, g_pTexRender, 0, D3DTEXF_NONE );
	IRenderer::GetRendererInstance()->changeRenderTarget( g_pTexRender );
	IRenderer::GetRendererInstance()->SetDepthStencilSurface( pSurfDS );
	IRenderer::GetRendererInstance()->Clear(false, true, stx_GetRGBA(0x00000000));
        }

        MeasureLuminance();
        BrightPassFilter();

        if( g_bBloom )
            RenderBloom();

        //---------------------------------------------------------------------
        // Final pass
	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
	IRenderer::GetRendererInstance()->setTexture( 0, g_pTexRender );
	IRenderer::GetRendererInstance()->setTexture( 1, g_apTexToneMap[0] );
	IRenderer::GetRendererInstance()->setTexture( 2, g_bBloom ? g_apTexBloom[0] : 0 );
	//IRenderer::GetRendererInstance()->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	//IRenderer::GetRendererInstance()->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

        switch( g_eRenderMode )
        {
            case DECODED:
                g_pEffect->SetTechnique( g_pCurTechnique->FinalPass->GetName() ); break;
            case RGB_ENCODED:
                g_pEffect->SetTechnique( "FinalPassEncoded_RGB" ); break;
            case ALPHA_ENCODED:
                g_pEffect->SetTechnique( "FinalPassEncoded_A" ); break;
        }

        unsigned int uiPass, uiNumPasses;
        g_pEffect->Begin( &uiNumPasses, 0 );

        for( uiPass = 0; uiPass < uiNumPasses; uiPass++ )
        {
            g_pEffect->BeginPass( uiPass );

            DrawFullScreenQuad();

            g_pEffect->EndPass();
        }

        g_pEffect->End();

	IRenderer::GetRendererInstance()->EndScene();
    }

    //////////////SAFE_RELEASE( pSurfRenderTarget );
    //////////////SAFE_RELEASE( pSurfBackBuffer );
    //////////////SAFE_RELEASE( pSurfDS );
}

//--------------------------------------------------------------------------------------
inline float GaussianDistribution( float x, float y, float rho )
{
    float g = 1.0f / sqrtf( 2.0f * D3DXFROMWINE_PI * rho * rho );
    g *= expf( -( x * x + y * y ) / ( 2 * rho * rho ) );

    return g;
}


//--------------------------------------------------------------------------------------
inline int log2_ceiling( float val )
{
    int iMax = 256;
    int iMin = 0;

    while( iMax - iMin > 1 )
    {
        int iMiddle = ( iMax + iMin ) / 2;

        if( val > g_aPowsOfTwo[iMiddle] )
            iMin = iMiddle;
        else
            iMax = iMiddle;
    }

    return iMax - 128;
}


//--------------------------------------------------------------------------------------
inline VOID EncodeRGBE8( half* pSrc, BYTE** ppDest )
{
    FLOAT r, g, b;

    r = ( FLOAT )*( pSrc + 0 );
    g = ( FLOAT )*( pSrc + 1 );
    b = ( FLOAT )*( pSrc + 2 );

    // Determine the largest color component
    float maxComponent = std::max( std::max( r, g ), b );

    // Round to the nearest integer exponent
    int nExp = log2_ceiling( maxComponent );

    // Divide the components by the shared exponent
    FLOAT fDivisor = ( FLOAT )g_aPowsOfTwo[ nExp + 128 ];

    r /= fDivisor;
    g /= fDivisor;
    b /= fDivisor;

    // Constrain the color components
    r = std::max( float(0), std::min( float(1), r ) );
    g = std::max( float(0), std::min( float(1), g ) );
    b = std::max( float(0), std::min( float(1), b ) );

    // Store the shared exponent in the alpha channel
    D3DXFROMWINEVECTOR4* pDestColor = ( D3DXFROMWINEVECTOR4* )*ppDest;
    (*(pDestColor+0)).x = ( r * 255 );
    (*(pDestColor+1)).y = ( g * 255 );
    (*(pDestColor+2)).z = ( b * 255 );
    (*(pDestColor+3)).w = nExp + 128;
    *ppDest += sizeof( D3DXFROMWINEVECTOR4 );
}


//--------------------------------------------------------------------------------------
inline VOID EncodeRGB16( half* pSrc, BYTE** ppDest )
{
    FLOAT r, g, b;

    r = ( FLOAT )*( pSrc + 0 );
    g = ( FLOAT )*( pSrc + 1 );
    b = ( FLOAT )*( pSrc + 2 );

    // Divide the components by the multiplier
    r /= RGB16_MAX;
    g /= RGB16_MAX;
    b /= RGB16_MAX;

    // Constrain the color components
    r = std::max( float(0), std::min( float(1), r ) );
    g = std::max( float(0), std::min( float(1), g ) );
    b = std::max( float(0), std::min( float(1), b ) );

    // Store
    USHORT* pDestColor = ( USHORT* )*ppDest;
    *pDestColor++ = ( USHORT )( r * 65535 );
    *pDestColor++ = ( USHORT )( g * 65535 );
    *pDestColor++ = ( USHORT )( b * 65535 );

    *ppDest += sizeof( uint64 );
}


//-----------------------------------------------------------------------------
// Name: RetrieveTechHandles()
// Desc: 
//-----------------------------------------------------------------------------
int RetrieveTechHandles()
{
    char* modes[] = { "FP16", "FP16", "RGB16", "RGBE8" };
    char* techniques[] = { "Scene", "DownScale2x2_Lum", "DownScale3x3", "DownScale3x3_BrightPass", "FinalPass" };
    __DWORD__ dwNumTechniques = sizeof( TECH_HANDLES ) / sizeof( stx_Effect::stx_Technique* );

    char strBuffer[MAX_PATH] = {0};

    stx_Effect::stx_Technique** pHandle = ( stx_Effect::stx_Technique** )g_aTechHandles;

    for( unsigned int m = 0; m < ( unsigned int )NUM_ENCODING_MODES; m++ )
    {
        for( unsigned int t = 0; t < dwNumTechniques; t++ )
        {
            stx_snprintf( strBuffer, MAX_PATH - 1, "%s_%s", techniques[t], modes[m] );

            *pHandle++ = g_pEffect->GetTechniqueByName( strBuffer );
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Name: MeasureLuminance()
// Desc: Measure the average log luminance in the scene.
//-----------------------------------------------------------------------------
int MeasureLuminance()
{
    
    unsigned int uiNumPasses, uiPass;
    D3DXFROMWINEVECTOR2 avSampleOffsets[16];

    TextureID pTexSrc = 0;
    TextureID pTexDest = 0;
    SurfaceID pSurfDest = 0;

    //-------------------------------------------------------------------------
    // Initial sampling pass to convert the image to the log of the grayscale
    //-------------------------------------------------------------------------
    pTexSrc = g_pTexRender;
    pTexDest = g_apTexToneMap[NUM_TONEMAP_TEXTURES - 1];

    D3DSURFACE_DESC descSrc;
    pTexSrc->GetLevelDesc( 0, &descSrc );

    D3DSURFACE_DESC descDest;
    pTexDest->GetLevelDesc( 0, &descDest );

    GetSampleOffsets_DownScale2x2_Lum( descSrc.Width, descSrc.Height, descDest.Width, descDest.Height,
                                       avSampleOffsets );

    g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );
    g_pEffect->SetTechnique( g_pCurTechnique->DownScale2x2_Lum->GetName() );

    pTexDest->GetSurfaceLevel( 0, &pSurfDest );
	IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfDest );
	IRenderer::GetRendererInstance()->setTexture( 0, pTexSrc );
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

    g_pEffect->Begin( &uiNumPasses, 0 );

    for( uiPass = 0; uiPass < uiNumPasses; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad();

        g_pEffect->EndPass();
    }

    g_pEffect->End();
	IRenderer::GetRendererInstance()->setTexture( 0, 0 );

    //////////////SAFE_RELEASE( pSurfDest );

    //-------------------------------------------------------------------------
    // Iterate through the remaining tone map textures
    //------------------------------------------------------------------------- 
    for( int i = NUM_TONEMAP_TEXTURES - 1; i > 0; i-- )
    {
        // Cycle the textures
        pTexSrc = g_apTexToneMap[i];
        pTexDest = g_apTexToneMap[i - 1];

        pTexDest->GetSurfaceLevel( 0, &pSurfDest );
        
            return 0;

        D3DSURFACE_DESC desc;
        pTexSrc->GetLevelDesc( 0, &desc );
        GetSampleOffsets_DownScale3x3( desc.Width, desc.Height, avSampleOffsets );

        g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );
        g_pEffect->SetTechnique( g_pCurTechnique->DownScale3x3->GetName() );
	IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfDest );
	IRenderer::GetRendererInstance()->setTexture( 0, pTexSrc );
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

        g_pEffect->Begin( &uiNumPasses, 0 );

        for( uiPass = 0; uiPass < uiNumPasses; uiPass++ )
        {
            g_pEffect->BeginPass( uiPass );

            // Draw a fullscreen quad to sample the RT
            DrawFullScreenQuad();

            g_pEffect->EndPass();
        }

        g_pEffect->End();
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_DownScale3x3
// Desc: Get the texture coordinate offsets to be used inside the DownScale3x3
//       pixel shader.
//-----------------------------------------------------------------------------
int GetSampleOffsets_DownScale3x3( __DWORD__ dwWidth, __DWORD__ dwHeight, D3DXFROMWINEVECTOR2 avSampleOffsets[] )
{
    if( 0 == avSampleOffsets )
        return E_INVALIDARG;

    float tU = 1.0f / dwWidth;
    float tV = 1.0f / dwHeight;

    // Sample from the 9 surrounding points. 
    int index = 0;
    for( int y = -1; y <= 1; y++ )
    {
        for( int x = -1; x <= 1; x++ )
        {
            avSampleOffsets[ index ].x = x * tU;
            avSampleOffsets[ index ].y = y * tV;

            index++;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_DownScale2x2_Lum
// Desc: Get the texture coordinate offsets to be used inside the DownScale2x2_Lum
//       pixel shader.
//-----------------------------------------------------------------------------
int GetSampleOffsets_DownScale2x2_Lum( __DWORD__ dwSrcWidth, __DWORD__ dwSrcHeight, __DWORD__ dwDestWidth, __DWORD__ dwDestHeight,
                                           D3DXFROMWINEVECTOR2 avSampleOffsets[] )
{
    if( 0 == avSampleOffsets )
        return E_INVALIDARG;

    float tU = 1.0f / dwSrcWidth;
    float tV = 1.0f / dwSrcHeight;

    float deltaU = ( float )dwSrcWidth / dwDestWidth / 2.0f;
    float deltaV = ( float )dwSrcHeight / dwDestHeight / 2.0f;

    // Sample from 4 surrounding points. 
    int index = 0;
    for( int y = 0; y < 2; y++ )
    {
        for( int x = 0; x <= 2; x++ )
        {
            avSampleOffsets[ index ].x = ( x - 0.5f ) * deltaU * tU;
            avSampleOffsets[ index ].y = ( y - 0.5f ) * deltaV * tV;

            index++;
        }
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_Bloom()
// Desc: 
//-----------------------------------------------------------------------------
int GetSampleOffsets_Bloom( __DWORD__ dwD3DTexSize,
                                float afTexCoordOffset[15],
                                D3DXFROMWINEVECTOR4* avColorWeight,
                                float fDeviation, float fMultiplier )
{
    int i = 0;
    float tu = 1.0f / ( float )dwD3DTexSize;

    // Fill the center texel
    float weight = 1.0f * GaussianDistribution( 0, 0, fDeviation );
    avColorWeight[0] = D3DXFROMWINEVECTOR4( weight, weight, weight, 1.0f );

    afTexCoordOffset[0] = 0.0f;

    // Fill the right side
    for( i = 1; i < 8; i++ )
    {
        weight = fMultiplier * GaussianDistribution( ( float )i, 0, fDeviation );
        afTexCoordOffset[i] = i * tu;

        avColorWeight[i] = D3DXFROMWINEVECTOR4( weight, weight, weight, 1.0f );
    }

    // Copy to the left side
    for( i = 8; i < 15; i++ )
    {
        avColorWeight[i] = avColorWeight[i - 7];
        afTexCoordOffset[i] = -afTexCoordOffset[i - 7];
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: RenderModel()
// Desc: Render the model
//-----------------------------------------------------------------------------
int RenderModel()
{
    

    // Set the transforms
    D3DXFROMWINEMATRIX mWorldViewProj;
    D3DXFROMWINEMatrixMultiply( &mWorldViewProj, &g_World, &g_View );
    D3DXFROMWINEMatrixMultiply( &mWorldViewProj, &mWorldViewProj, &g_Proj );
    g_pEffect->SetMatrix( "g_mWorld", &g_World );
    g_pEffect->SetMatrix( "g_mWorldViewProj", &mWorldViewProj );

    // Draw the mesh
    g_pEffect->SetTechnique( g_pCurTechnique->Scene->GetName() );
    g_pEffect->SetTexture( "g_tCube", g_aSkybox[g_eEncodingMode].GetEnvironmentMap() );

    unsigned int uiPass, uiNumPasses;
    g_pEffect->Begin( &uiNumPasses, 0 );
    
        return 0;

    for( uiPass = 0; uiPass < uiNumPasses; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        g_pMesh.render( &mWorldViewProj );

        g_pEffect->EndPass();
    }

    g_pEffect->End();

    return 0;
}



//-----------------------------------------------------------------------------
// Name: BrightPassFilter
// Desc: Prepare for the bloom pass by removing dark information from the scene
//-----------------------------------------------------------------------------
int BrightPassFilter()
{
    D3DXFROMWINEVECTOR2 avSampleOffsets[16];
    GetSampleOffsets_DownScale3x3( STX_Service::GetWindowInstance()->GetWidth() / 2, STX_Service::GetWindowInstance()->GetHeight() / 2, avSampleOffsets );

    g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );

    SurfaceID pSurfBrightPass = 0;
    g_pTexBrightPass->GetSurfaceLevel( 0, &pSurfBrightPass );
    
        return 0;

    g_pEffect->SetTechnique( g_pCurTechnique->DownScale3x3_BrightPass->GetName() );
    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfBrightPass );
    IRenderer::GetRendererInstance()->setTexture( 0, g_pTexRender );
    IRenderer::GetRendererInstance()->setTexture( 1, g_apTexToneMap[0] );

    //IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

    unsigned int uiPass, uiNumPasses;
    g_pEffect->Begin( &uiNumPasses, 0 );
    
        return 0;

    for( uiPass = 0; uiPass < uiNumPasses; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad();

        g_pEffect->EndPass();
    }

    g_pEffect->End();
    IRenderer::GetRendererInstance()->setTexture( 0, 0 );
    IRenderer::GetRendererInstance()->setTexture( 1, 0 );

    //////////////SAFE_RELEASE( pSurfBrightPass );

    return 0;
}


//-----------------------------------------------------------------------------
// Name: RenderBloom()
// Desc: Render the blooming effect
//-----------------------------------------------------------------------------
int RenderBloom()
{
    
    unsigned int uiPassCount, uiPass;
    int i = 0;

    D3DXFROMWINEVECTOR2 avSampleOffsets[16];
    float afSampleOffsets[16];
    D3DXFROMWINEVECTOR4 avSampleWeights[16];

    SurfaceID pSurfDest = 0;
    g_apTexBloom[1]->GetSurfaceLevel( 0, &pSurfDest );
    
        return 0;

    D3DSURFACE_DESC desc;
    g_pTexBrightPass->GetLevelDesc( 0, &desc );
    
        return 0;

    GetSampleOffsets_Bloom( desc.Width, afSampleOffsets, avSampleWeights, 3.0f, 1.25f );
    for( i = 0; i < 16; i++ )
    {
        avSampleOffsets[i] = D3DXFROMWINEVECTOR2( afSampleOffsets[i], 0.0f );
    }

    g_pEffect->SetTechnique( "Bloom" );
    g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );
    g_pEffect->SetValue( "g_avSampleWeights", avSampleWeights, sizeof( avSampleWeights ) );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfDest );
    IRenderer::GetRendererInstance()->setTexture( 0, g_pTexBrightPass );
    //IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    //IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );


    g_pEffect->Begin( &uiPassCount, 0 );
    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad();

        g_pEffect->EndPass();
    }
    g_pEffect->End();
    IRenderer::GetRendererInstance()->setTexture( 0, 0 );

    //////////////SAFE_RELEASE( pSurfDest );
    g_apTexBloom[0]->GetSurfaceLevel( 0, &pSurfDest );

    GetSampleOffsets_Bloom( desc.Height, afSampleOffsets, avSampleWeights, 3.0f, 1.25f );
    for( i = 0; i < 16; i++ )
    {
        avSampleOffsets[i] = D3DXFROMWINEVECTOR2( 0.0f, afSampleOffsets[i] );
    }

    g_pEffect->SetTechnique( "Bloom" );
    g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );
    g_pEffect->SetValue( "g_avSampleWeights", avSampleWeights, sizeof( avSampleWeights ) );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfDest );
    IRenderer::GetRendererInstance()->setTexture( 0, g_apTexBloom[1] );
    //IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    //IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );


    g_pEffect->Begin( &uiPassCount, 0 );

    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad();

        g_pEffect->EndPass();
    }

    g_pEffect->End();
    IRenderer::GetRendererInstance()->setTexture( 0, 0 );

    //////////////SAFE_RELEASE( pSurfDest );

    return 0;
}


//-----------------------------------------------------------------------------
// Name: DrawFullScreenQuad
// Desc: Draw a properly aligned quad covering the entire render target
//-----------------------------------------------------------------------------
void DrawFullScreenQuad( float fLeftU=0.0f, float fTopV=0.0f, float fRightU=1.0f, float fBottomV=1.0f )
{
    D3DSURFACE_DESC dtdsdRT;
    SurfaceID pSurfRT;

    // Acquire render target width and height
    IRenderer::GetRendererInstance()->GetRenderTarget( 0, &pSurfRT );
    pSurfRT->GetDesc( &dtdsdRT );
    pSurfRT->Release();

    // Ensure that we're directly mapping texels to pixels by offset by 0.5
    // For more info see the doc page titled "Directly Mapping Texels to Pixels"
    FLOAT fWidth5 = ( FLOAT )dtdsdRT.Width - 0.5f;
    FLOAT fHeight5 = ( FLOAT )dtdsdRT.Height - 0.5f;

    // Draw the quad
    SCREEN_VERTEX svQuad[4];

    svQuad[0].pos = D3DXFROMWINEVECTOR4( -0.5f, -0.5f, 0.5f, 1.0f );
    svQuad[0].tex = D3DXFROMWINEVECTOR2( fLeftU, fTopV );

    svQuad[1].pos = D3DXFROMWINEVECTOR4( fWidth5, -0.5f, 0.5f, 1.0f );
    svQuad[1].tex = D3DXFROMWINEVECTOR2( fRightU, fTopV );

    svQuad[2].pos = D3DXFROMWINEVECTOR4( -0.5f, fHeight5, 0.5f, 1.0f );
    svQuad[2].tex = D3DXFROMWINEVECTOR2( fLeftU, fBottomV );

    svQuad[3].pos = D3DXFROMWINEVECTOR4( fWidth5, fHeight5, 0.5f, 1.0f );
    svQuad[3].tex = D3DXFROMWINEVECTOR2( fRightU, fBottomV );

    //IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ZENABLE, FALSE );
    IRenderer::GetRendererInstance()->SetFVF( SCREEN_VERTEX::FVF );
    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, svQuad, sizeof( SCREEN_VERTEX ) );
    //IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ZENABLE, TRUE );
}


//-----------------------------------------------------------------------------
// Name: CreateEncodedTexture
// Desc: Create a copy of the input floating-point texture with RGBE8 or RGB16 
//       encoding
//-----------------------------------------------------------------------------
int CreateEncodedTexture( TextureID  pTexSrc, TextureID   ppTexDest,
                              ENCODING_MODE eTarget )
{
    

    D3DSURFACE_DESC desc;
    ( pTexSrc->GetLevelDesc( 0, &desc ) );

    // Create a texture with equal dimensions to store the encoded texture
    FORMAT fmt = FORMAT_NONE;
    switch( eTarget )
    {
        case RGBE8:
            fmt = FORMAT_RGBA8; break;
        case RGB16:
            fmt = FORMAT_RGBA16; break;
    }

    ( IRenderer::GetRendererInstance()->CreateCubeTexture( desc.Width, 1, 0,
                                               fmt, D3DPOOL_MANAGED,
                                               ppTexDest, 0 ) );

    for( unsigned int iFace = 0; iFace < 6; iFace++ )
    {
        // Lock the source texture for reading
        D3DLOCKED_RECT rcSrc;
        ( pTexSrc->LockRect( ( D3DCUBEMAP_FACES )iFace, 0, &rcSrc, 0, D3DLOCK_READONLY ) );

        // Lock the destination texture for writing
        D3DLOCKED_RECT rcDest;
        ( ( *ppTexDest )->LockRect( ( D3DCUBEMAP_FACES )iFace, 0, &rcDest, 0, 0 ) );

        BYTE* pSrcBytes = ( BYTE* )rcSrc.pBits;
        BYTE* pDestBytes = ( BYTE* )rcDest.pBits;


        for( unsigned int y = 0; y < desc.Height; y++ )
        {
            half* pSrc = ( half* )pSrcBytes;
            BYTE* pDest = pDestBytes;

            for( unsigned int x = 0; x < desc.Width; x++ )
            {
                switch( eTarget )
                {
                    case RGBE8:
                        EncodeRGBE8( pSrc, &pDest ); break;
                    case RGB16:
                        EncodeRGB16( pSrc, &pDest ); break;
                    default:
                        return 1;
                }

                pSrc += 4;
            }

            pSrcBytes += rcSrc.Pitch;
            pDestBytes += rcDest.Pitch;
        }

        // Release the locks
        ( *ppTexDest )->UnlockRect( ( D3DCUBEMAP_FACES )iFace, 0 );
        pTexSrc->UnlockRect( ( D3DCUBEMAP_FACES )iFace, 0 );
    }

    return 0;
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
	IRenderer* r=IRenderer::GetRendererInstance("hdrformats");
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

