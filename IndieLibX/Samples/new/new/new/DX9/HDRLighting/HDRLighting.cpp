/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// File: HDRLighting.cpp
//
// Desc: This sample demonstrates some high dynamic range lighting effects 
//       using floating point textures.
//
// The algorithms described in this sample are based very closely on the 
// lighting effects implemented in Masaki Kawase's Rthdribl sample and the tone 
// mapping process described in the whitepaper "Tone Reproduction for Digital 
// Images"
//
// Real-Time High Dynamic Range Image-Based Lighting (Rthdribl)
// Masaki Kawase
// http://www.daionet.gr.jp/~masa/rthdribl/ 
//
// "Photographic Tone Reproduction for Digital Images"
// Erik Reinhard, Mike Stark, Peter Shirley and Jim Ferwerda
// http://www.cs.utah.edu/~reinhard/cdrom/ 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "glaredefd3d.h"

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 

//-----------------------------------------------------------------------------
// Constants and custom types
//-----------------------------------------------------------------------------
#define MAX_SAMPLES           16      // Maximum number of texture grabs

#define NUM_LIGHTS            2       // Number of lights in the scene

#define EMISSIVE_COEFFICIENT  39.78f  // Emissive color multiplier for each lumen
// of light intensity                                    
#define NUM_TONEMAP_TEXTURES  4       // Number of stages in the 4x4 down-scaling 
// of average luminance textures
#define NUM_STAR_TEXTURES     12      // Number of textures used for the star
// post-processing effect
#define NUM_BLOOM_TEXTURES    3       // Number of textures used for the bloom
// post-processing effect


// Texture coordinate rectangle
struct CoordRect
{
    float fLeftU, fTopV;
    float fRightU, fBottomV;
};


// World vertex format
struct WorldVertex
{
    D3DXFROMWINEVECTOR3 p; // position
    D3DXFROMWINEVECTOR3 n; // normal
    D3DXFROMWINEVECTOR2 t; // texture coordinate

    static const __DWORD__ FVF;
};
const __DWORD__                 WorldVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


// Screen quad vertex format
struct ScreenVertex
{
    D3DXFROMWINEVECTOR4 p; // position
    D3DXFROMWINEVECTOR2 t; // texture coordinate

    static const __DWORD__ FVF;
};
const __DWORD__                 ScreenVertex::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;



//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
stx_Effect*                g_pEffect = 0;       // D3DXFROMWINE effect interface
CFirstPersonCamera          g_Camera;               // A model viewing camera
FORMAT                   g_LuminanceFormat;      // Format to use for luminance map
bool                        g_bShowHelp = true;     // If true, it renders the UI control text

SurfaceID           g_pFloatMSRT = 0;           // Multi-Sample float render target
SurfaceID           g_pFloatMSDS = 0;           // Depth Stencil surface for the float RT
TextureID           g_pTexScene = 0;            // HDR render target containing the scene
TextureID           g_pTexSceneScaled = 0;      // Scaled copy of the HDR scene
TextureID           g_pTexBrightPass = 0;       // Bright-pass filtered copy of the scene
TextureID           g_pTexAdaptedLuminanceCur = 0;  // The luminance that the user is currenly adapted to
TextureID           g_pTexAdaptedLuminanceLast = 0; // The luminance that the user is currenly adapted to
TextureID           g_pTexStarSource = 0;       // Star effect source texture
TextureID           g_pTexBloomSource = 0;      // Bloom effect source texture

TextureID           g_pTexWall = 0;     // Stone texture for the room walls
TextureID           g_pTexFloor = 0;    // Concrete texture for the room floor
TextureID           g_pTexCeiling = 0;  // Plaster texture for the room ceiling
TextureID           g_pTexPainting = 0; // Texture for the paintings on the wall


TextureID g_apTexBloom[NUM_BLOOM_TEXTURES] = {0};     // Blooming effect working textures
TextureID g_apTexStar[NUM_STAR_TEXTURES] = {0};       // Star effect working textures
TextureID g_apTexToneMap[NUM_TONEMAP_TEXTURES] = {0}; // Log average luminance samples 
// from the HDR render target

MeshRenderer2                  g_pWorldMesh = 0;       // Mesh to contain world objects
MeshRenderer2                  g_pmeshSphere = 0;      // Representation of point light

CGlareDef                   g_GlareDef;         // Glare defintion
EGLARELIBTYPE               g_eGlareType;       // Enumerated glare type

D3DXFROMWINEVECTOR4 g_avLightPosition[NUM_LIGHTS];    // Light positions in world space
D3DXFROMWINEVECTOR4 g_avLightIntensity[NUM_LIGHTS];   // Light floating point intensities
int g_nLightLogIntensity[NUM_LIGHTS]; // Light intensities on a log scale
int g_nLightMantissa[NUM_LIGHTS];     // Mantissa of the light intensity

__DWORD__                       g_dwCropWidth;    // Width of the cropped scene texture
__DWORD__                       g_dwCropHeight;   // Height of the cropped scene texture

float                       g_fKeyValue;              // Middle gray key value for tone mapping

bool                        g_bToneMap;               // True when scene is to be tone mapped            
bool                        g_bDetailedStats;         // True when state variables should be rendered
bool                        g_bDrawHelp;              // True when help instructions are to be drawn
bool                        g_bBlueShift;             // True when blue shift is to be factored in
bool                        g_bAdaptationInvalid;     // True when adaptation level needs refreshing
bool                        g_bUseMultiSampleFloat16 = false; // True when using multisampling on a floating point back buffer
D3DMULTISAMPLE_TYPE         g_MaxMultiSampleType = D3DMULTISAMPLE_NONE;  // Non-Zero when g_bUseMultiSampleFloat16 is true
__DWORD__                       g_dwMultiSampleQuality = 0; // Non-Zero when we have multisampling on a float backbuffer
bool                        g_bSupportsD16 = false;
bool                        g_bSupportsD32 = false;
bool                        g_bSupportsD24X8 = false;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_STATIC              -1
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_GLARETYPE           5
#define IDC_LIGHT0_LABEL        6
#define IDC_LIGHT1_LABEL        7
#define IDC_LIGHT0              8
#define IDC_LIGHT1              9
#define IDC_KEYVALUE            10
#define IDC_KEYVALUE_LABEL      11
#define IDC_TONEMAP             12
#define IDC_BLUESHIFT           13
#define IDC_RESET               14

struct App
{
	App(){}
	virtual ~App(){}
//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    // Set light positions in world space
    g_avLightPosition[0] = D3DXFROMWINEVECTOR4( 4.0f, 2.0f, 18.0f, 1.0f );
    g_avLightPosition[1] = D3DXFROMWINEVECTOR4( 11.0f, 2.0f, 18.0f, 1.0f );

    ResetOptions();
}


//-----------------------------------------------------------------------------
// Name: ResetOptions()
// Desc: Reset all user-controlled options to default values
//-----------------------------------------------------------------------------
void ResetOptions()
{
    g_bDrawHelp = TRUE;
    g_bDetailedStats = TRUE;
}


//-----------------------------------------------------------------------------
// Name: AdjustLight
// Desc: Increment or decrement the light at the given index
//-----------------------------------------------------------------------------
int AdjustLight( unsigned int iLight, bool bIncrement )
{
    if( iLight >= NUM_LIGHTS )
        return E_INVALIDARG;

    if( bIncrement && g_nLightLogIntensity[iLight] < 7 )
    {
        g_nLightMantissa[iLight]++;
        if( g_nLightMantissa[iLight] > 9 )
        {
            g_nLightMantissa[iLight] = 1;
            g_nLightLogIntensity[iLight]++;
        }
    }

    if( !bIncrement && g_nLightLogIntensity[iLight] > -4 )
    {
        g_nLightMantissa[iLight]--;
        if( g_nLightMantissa[iLight] < 1 )
        {
            g_nLightMantissa[iLight] = 9;
            g_nLightLogIntensity[iLight]--;
        }
    }

    RefreshLights();
    return 0;
}


//-----------------------------------------------------------------------------
// Name: RefreshLights
// Desc: Set the light intensities to match the current log luminance
//-----------------------------------------------------------------------------
int RefreshLights()
{
    char strBuffer[256] = {0};
    for( int i = 0; i < NUM_LIGHTS; i++ )
    {
        g_avLightIntensity[i].x = g_nLightMantissa[i] * ( float )pow( 10.0f, g_nLightLogIntensity[i] );
        g_avLightIntensity[i].y = g_nLightMantissa[i] * ( float )pow( 10.0f, g_nLightLogIntensity[i] );
        g_avLightIntensity[i].z = g_nLightMantissa[i] * ( float )pow( 10.0f, g_nLightLogIntensity[i] );
        g_avLightIntensity[i].w = 1.0f;

        stx_snprintf( strBuffer, 255, "Light %d: %d.0e%d", i, g_nLightMantissa[i], g_nLightLogIntensity[i] );
    }
    return 0;
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
    int hr;
#if 0
        g_LuminanceFormat = FORMAT_R32f;
#else
        g_LuminanceFormat = FORMAT_R16f;
#endif

    // Determine whether we can support multisampling on a A16B16G16R16F render target
    g_bUseMultiSampleFloat16 = false;
    g_MaxMultiSampleType = D3DMULTISAMPLE_NONE;
    {
        __DWORD__ msQuality = 0;
        {
            g_bUseMultiSampleFloat16 = true;
            // ??? g_MaxMultiSampleType = imst;
            if( msQuality > 0 )
                g_dwMultiSampleQuality = msQuality - 1;
            else
                g_dwMultiSampleQuality = msQuality;
        }
    }

    g_bSupportsD16 = true;
    g_bSupportsD32 = true;
    g_bSupportsD24X8 = true; 

    // If this fails, there should be debug output as to 
    // they the .fx file failed to compile
	g_pEffect="HDRLighting.fx");

    // Initialize the camera
    D3DXFROMWINEVECTOR3 vFromPt( 7.5f, 1.8f, 2 );
    D3DXFROMWINEVECTOR3 vLookatPt( 7.5f, 1.5f, 10.0f );
    D3DXFROMWINEMatrixLookAtLH(&g_View, &vFromPt, &vLookatPt );

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
    int hr;
    int i = 0; // loop variable

    // Create the Multi-Sample floating point render target
    FORMAT dfmt = FORMAT_NONE;
    if( g_bSupportsD16 )
        dfmt = FORMAT_D16;
    else if( g_bSupportsD32 )
        dfmt = D3DFMT_D32;
    else if( g_bSupportsD24X8 )
        dfmt = FORMAT_D24S8;
    else
        g_bUseMultiSampleFloat16 = false;

    if( g_bUseMultiSampleFloat16 )
    {
        IRenderer::GetRendererInstance()->CreateRenderTarget( STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(),
                                               FORMAT_RGBA16f,
                                               g_MaxMultiSampleType, g_dwMultiSampleQuality,
                                               FALSE, &g_pFloatMSRT, 0 );
        if( ( hr ) )
            g_bUseMultiSampleFloat16 = false;
        else
        {
            IRenderer::GetRendererInstance()->CreateDepthStencilSurface( STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(),
                                                          dfmt,
                                                          g_MaxMultiSampleType, g_dwMultiSampleQuality,
                                                          TRUE, &g_pFloatMSDS, 0 );
            if( ( hr ) )
            {
                g_bUseMultiSampleFloat16 = false;
                //////////////SAFE_RELEASE( g_pFloatMSRT );
            }
        }
    }

    // Crop the scene texture so width and height are evenly divisible by 8.
    // This cropped version of the scene will be used for post processing effects,
    // and keeping everything evenly divisible allows precise control over
    // sampling points within the shaders.
    g_dwCropWidth = STX_Service::GetWindowInstance()->GetWidth() - STX_Service::GetWindowInstance()->GetWidth() % 8;
    g_dwCropHeight = STX_Service::GetWindowInstance()->GetHeight() - STX_Service::GetWindowInstance()->GetHeight() % 8;

    // Create the HDR scene texture
    IRenderer::GetRendererInstance()->addTexture( STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(),
                                      1, D3DUSAGE_RENDERTARGET, FORMAT_RGBA16f,
                                      D3DPOOL_DEFAULT, &g_pTexScene, 0 );


    // Scaled version of the HDR scene texture
    IRenderer::GetRendererInstance()->addTexture( g_dwCropWidth / 4, g_dwCropHeight / 4,
                                      1, D3DUSAGE_RENDERTARGET,
                                      FORMAT_RGBA16f, D3DPOOL_DEFAULT,
                                      &g_pTexSceneScaled, 0 );


    // Create the bright-pass filter texture. 
    // Texture has a black border of single texel thickness to fake border 
    // addressing using clamp addressing
    IRenderer::GetRendererInstance()->addTexture( g_dwCropWidth / 4 + 2, g_dwCropHeight / 4 + 2,
                                      1, D3DUSAGE_RENDERTARGET,
                                      FORMAT_RGBA8, D3DPOOL_DEFAULT,
                                      &g_pTexBrightPass, 0 );



    // Create a texture to be used as the source for the star effect
    // Texture has a black border of single texel thickness to fake border 
    // addressing using clamp addressing
    IRenderer::GetRendererInstance()->addTexture( g_dwCropWidth / 4 + 2, g_dwCropHeight / 4 + 2,
                                      1, D3DUSAGE_RENDERTARGET,
                                      FORMAT_RGBA8, D3DPOOL_DEFAULT,
                                      &g_pTexStarSource, 0 );



    // Create a texture to be used as the source for the bloom effect
    // Texture has a black border of single texel thickness to fake border 
    // addressing using clamp addressing
    IRenderer::GetRendererInstance()->addTexture( g_dwCropWidth / 8 + 2, g_dwCropHeight / 8 + 2,
                                      1, D3DUSAGE_RENDERTARGET,
                                      FORMAT_RGBA8, D3DPOOL_DEFAULT,
                                      &g_pTexBloomSource, 0 );




    // Create a 2 textures to hold the luminance that the user is currently adapted
    // to. This allows for a simple simulation of light adaptation.
    IRenderer::GetRendererInstance()->addTexture( 1, 1, 1, D3DUSAGE_RENDERTARGET,
                                      g_LuminanceFormat, D3DPOOL_DEFAULT,
                                      &g_pTexAdaptedLuminanceCur, 0 );
    IRenderer::GetRendererInstance()->addTexture( 1, 1, 1, D3DUSAGE_RENDERTARGET,
                                      g_LuminanceFormat, D3DPOOL_DEFAULT,
                                      &g_pTexAdaptedLuminanceLast, 0 );


    // For each scale stage, create a texture to hold the intermediate results
    // of the luminance calculation
    for( i = 0; i < NUM_TONEMAP_TEXTURES; i++ )
    {
        int iSampleLen = 1 << ( 2 * i );

        IRenderer::GetRendererInstance()->addTexture( iSampleLen, iSampleLen, 1, D3DUSAGE_RENDERTARGET,
                                          g_LuminanceFormat, D3DPOOL_DEFAULT,
                                          &g_apTexToneMap[i], 0 );
    }


    // Create the temporary blooming effect textures
    // Texture has a black border of single texel thickness to fake border 
    // addressing using clamp addressing
    for( i = 1; i < NUM_BLOOM_TEXTURES; i++ )
    {
        IRenderer::GetRendererInstance()->addTexture( g_dwCropWidth / 8 + 2, g_dwCropHeight / 8 + 2,
                                          1, D3DUSAGE_RENDERTARGET, FORMAT_RGBA8,
                                          D3DPOOL_DEFAULT, &g_apTexBloom[i], 0 );
    }

    // Create the final blooming effect texture
    IRenderer::GetRendererInstance()->addTexture( g_dwCropWidth / 8, g_dwCropHeight / 8,
                                      1, D3DUSAGE_RENDERTARGET, FORMAT_RGBA8,
                                      D3DPOOL_DEFAULT, &g_apTexBloom[0], 0 );


    // Create the star effect textures
    for( i = 0; i < NUM_STAR_TEXTURES; i++ )
    {
        IRenderer::GetRendererInstance()->addTexture( g_dwCropWidth / 4, g_dwCropHeight / 4,
                                          1, D3DUSAGE_RENDERTARGET,
                                          FORMAT_RGBA16f, D3DPOOL_DEFAULT,
                                          &g_apTexStar[i], 0 );
    }

    // Create a texture to paint the walls
    	g_pTexWall );
	g_pTexFloor );
	g_pTexCeiling );
	g_pTexPainting );


    // Textures with borders must be cleared since scissor rect testing will
    // be used to avoid rendering on top of the border
    ClearTexture( g_pTexAdaptedLuminanceCur );
    ClearTexture( g_pTexAdaptedLuminanceLast );
    ClearTexture( g_pTexBloomSource );
    ClearTexture( g_pTexBrightPass );
    ClearTexture( g_pTexStarSource );

    for( i = 0; i < NUM_BLOOM_TEXTURES; i++ )
    {
        ClearTexture( g_apTexBloom[i] );
    }


    // Build the world object
    BuildWorldMesh();


    // Create sphere mesh to represent the light
    LoadMesh$1, &g_pmeshSphere );

    // Setup the camera's projection parameters
    float fAspectRatio = ( ( FLOAT )g_dwCropWidth ) / g_dwCropHeight;
    D3DXFROMWINEMatrixPerspectiveFovLH(&g_Proj, D3DXFROMWINE_PI / 4, fAspectRatio, 0.2f, 30.0f );
    D3DXFROMWINEMATRIX mProjection = *g_Proj;

    // Set effect file variables
    g_pEffect->SetMatrix( "g_mProjection", &mProjection );
    g_pEffect->SetFloat( "g_fBloomScale", 1.0f );
    g_pEffect->SetFloat( "g_fStarScale", 0.5f );

    return 0;
}


//-----------------------------------------------------------------------------
// Name: ClearTexture()
// Desc: Helper function for RestoreDeviceObjects to clear a texture surface
//-----------------------------------------------------------------------------
int ClearTexture( TextureID pTexture )
{
    int 0;
    SurfaceID pSurface = 0;

    pTexture->GetSurfaceLevel( 0, &pSurface );
    if( ( hr ) )
        IRenderer::GetRendererInstance()->ColorFill( pSurface, 0, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );

    //////////////SAFE_RELEASE( pSurface );
    return hr;
}


//-----------------------------------------------------------------------------
// Name: BuildWorldMesh()
// Desc: Creates the wall, floor, ceiling, columns, and painting mesh
//-----------------------------------------------------------------------------
int BuildWorldMesh()
{
    int hr;
    unsigned int i; // Loop variable

    const FLOAT fWidth = 15.0f;
    const FLOAT fDepth = 20.0f;
    const FLOAT fHeight = 3.0f;

    // Create the room  
    MeshRenderer2 pWorldMeshTemp = 0;
    D3DXFROMWINECreateMeshFVF( 48, 96, 0, WorldVertex::FVF, &pWorldMeshTemp );
    if( ( hr ) )
        goto LCleanReturn;

    WorldVertex* pVertex;
    pWorldMeshTemp->LockVertexBuffer( 0, ( PVOID* )&pVertex );
    if( ( hr ) )
        goto LCleanReturn;

    WorldVertex* pV;
    pV = pVertex;

    // Front wall
    SetTextureCoords( pV, 7.0f, 2.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 0.0f, fHeight, fDepth );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( fWidth, fHeight, fDepth );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( fWidth, 0.0f, fDepth );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, fDepth );

    // Right wall
    SetTextureCoords( pV, 10.5f, 2.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( fWidth, fHeight, fDepth );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( fWidth, fHeight, 0.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( fWidth, 0.0f, 0.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( fWidth, 0.0f, fDepth );

    // Back wall
    SetTextureCoords( pV, 7.0f, 2.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( fWidth, fHeight, 0.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 0.0f, fHeight, 0.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( fWidth, 0.0f, 0.0f );

    // Left wall
    SetTextureCoords( pV, 10.5f, 2.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 0.0f, fHeight, 0.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 0.0f, fHeight, fDepth );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, fDepth );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );

    BuildColumn( pV, 4.0f, fHeight, 7.0f, 0.75f );
    BuildColumn( pV, 4.0f, fHeight, 13.0f, 0.75f );
    BuildColumn( pV, 11.0f, fHeight, 7.0f, 0.75f );
    BuildColumn( pV, 11.0f, fHeight, 13.0f, 0.75f );

    // Floor
    SetTextureCoords( pV, 7.0f, 7.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, fDepth );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( fWidth, 0.0f, fDepth );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( fWidth, 0.0f, 0.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );

    // Ceiling
    SetTextureCoords( pV, 7.0f, 2.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 0.0f, fHeight, 0.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( fWidth, fHeight, 0.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( fWidth, fHeight, fDepth );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 0.0f, fHeight, fDepth );

    // Painting 1
    SetTextureCoords( pV, 1.0f, 1.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 2.0f, fHeight - 0.5f, fDepth - 0.01f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 6.0f, fHeight - 0.5f, fDepth - 0.01f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 6.0f, fHeight - 2.5f, fDepth - 0.01f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 2.0f, fHeight - 2.5f, fDepth - 0.01f );

    // Painting 2
    SetTextureCoords( pV, 1.0f, 1.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 9.0f, fHeight - 0.5f, fDepth - 0.01f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 13.0f, fHeight - 0.5f, fDepth - 0.01f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 13.0f, fHeight - 2.5f, fDepth - 0.01f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( 9.0f, fHeight - 2.5f, fDepth - 0.01f );

    pWorldMeshTemp->UnlockVertexBuffer();

    // Retrieve the indices
    __WORD__* pIndex;
    pWorldMeshTemp->LockIndexBuffer( 0, ( PVOID* )&pIndex );
    if( ( hr ) )
        goto LCleanReturn;

    for( i = 0; i < pWorldMeshTemp->GetNumFaces() / 2; i++ )
    {
        *pIndex++ = (__WORD__)( ( i * 4 ) + 0 );
        *pIndex++ = (__WORD__)( ( i * 4 ) + 1 );
        *pIndex++ = (__WORD__)( ( i * 4 ) + 2 );
        *pIndex++ = (__WORD__)( ( i * 4 ) + 0 );
        *pIndex++ = (__WORD__)( ( i * 4 ) + 2 );
        *pIndex++ = (__WORD__)( ( i * 4 ) + 3 );
    }

    pWorldMeshTemp->UnlockIndexBuffer();

    // Set attribute groups to draw floor, ceiling, walls, and paintings
    // separately, with different shader constants. These group numbers
    // will be used during the calls to DrawSubset().
    __DWORD__* pAttribs;
    pWorldMeshTemp->LockAttributeBuffer( 0, &pAttribs );
    if( ( hr ) )
        goto LCleanReturn;

    for( i = 0; i < 40; i++ )
        *pAttribs++ = 0;

    for( i = 0; i < 2; i++ )
        *pAttribs++ = 1;

    for( i = 0; i < 2; i++ )
        *pAttribs++ = 2;

    for( i = 0; i < 4; i++ )
        *pAttribs++ = 3;

    pWorldMeshTemp->UnlockAttributeBuffer();
    D3DXFROMWINEComputeNormals( pWorldMeshTemp, 0 );

    // Optimize the mesh
    pWorldMeshTemp->CloneMeshFVF( D3DXFROMWINEMESH_VB_WRITEONLY | D3DXFROMWINEMESH_IB_WRITEONLY,
                                       WorldVertex::FVF, &g_pWorldMesh );
    if( ( hr ) )
        goto LCleanReturn;

    0;

LCleanReturn:
    //////////////SAFE_RELEASE( pWorldMeshTemp );
    return hr;
}

//-----------------------------------------------------------------------------
// Name: BuildColumn()
// Desc: Helper function for BuildWorldMesh to add column quads to the scene 
//-----------------------------------------------------------------------------
int BuildColumn( WorldVertex*& pV, float x, float y, float z, float width )
{
    float w = width / 2;

    SetTextureCoords( pV, 1.0f, 2.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x - w, y, z - w );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x + w, y, z - w );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x + w, 0.0f, z - w );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x - w, 0.0f, z - w );

    SetTextureCoords( pV, 1.0f, 2.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x + w, y, z - w );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x + w, y, z + w );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x + w, 0.0f, z + w );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x + w, 0.0f, z - w );

    SetTextureCoords( pV, 1.0f, 2.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x + w, y, z + w );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x - w, y, z + w );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x - w, 0.0f, z + w );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x + w, 0.0f, z + w );

    SetTextureCoords( pV, 1.0f, 2.0f );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x - w, y, z + w );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x - w, y, z - w );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x - w, 0.0f, z - w );
    ( pV++ )->p = D3DXFROMWINEVECTOR3( x - w, 0.0f, z + w );

    return 0;
}


//-----------------------------------------------------------------------------
// Name: SetTextureCoords()
// Desc: Helper function for BuildWorldMesh to set texture coordinates
//       for vertices
//-----------------------------------------------------------------------------
void SetTextureCoords( WorldVertex* pVertex, float u, float v )
{
    ( pVertex++ )->t = D3DXFROMWINEVECTOR2( 0.0f, 0.0f );
    ( pVertex++ )->t = D3DXFROMWINEVECTOR2( u, 0.0f );
    ( pVertex++ )->t = D3DXFROMWINEVECTOR2( u, v );
    ( pVertex++ )->t = D3DXFROMWINEVECTOR2( 0.0f, v );
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void  OnFrameMove( double fTime, float fElapsedTime )
{
    // Update the camera's position based on user input 
    g_Camera.FrameMove( fElapsedTime );

    // Set the flag to refresh the user's simulated adaption level.
    // Frame move is not called when the scene is paused or single-stepped. 
    // If the scene is paused, the user's adaptation level needs to remain
    // unchanged.
    g_bAdaptationInvalid = true;

    // Calculate the position of the lights in view space
    D3DXFROMWINEVECTOR4 avLightViewPosition[NUM_LIGHTS];
    for( int iLight = 0; iLight < NUM_LIGHTS; iLight++ )
    {
        D3DXFROMWINEMATRIX mView = *g_View;
        D3DXFROMWINEVec4Transform( &avLightViewPosition[iLight], &g_avLightPosition[iLight], &mView );
    }

    // Set frame shader constants
    g_pEffect->SetBool( "g_bEnableToneMap", g_bToneMap );
    g_pEffect->SetBool( "g_bEnableBlueShift", g_bBlueShift );
    g_pEffect->SetValue( "g_avLightPositionView", avLightViewPosition, sizeof( D3DXFROMWINEVECTOR4 ) * NUM_LIGHTS );
    g_pEffect->SetValue( "g_avLightIntensity", g_avLightIntensity, sizeof( D3DXFROMWINEVECTOR4 ) * NUM_LIGHTS );

}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, DXUT will call 
// ::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void  OnFrameRender(  double fTime, float fElapsedTime )
{
    int hr;

    SurfaceID pSurfLDR; // Low dynamic range surface for final output
    SurfaceID pSurfDS;  // Low dynamic range depth stencil surface
    SurfaceID pSurfHDR; // High dynamic range surface to store 
    // intermediate floating point color values

    // Store the old render target
    ( IRenderer::GetRendererInstance()->GetRenderTarget( 0, &pSurfLDR ) );
    ( IRenderer::GetRendererInstance()->GetDepthStencilSurface( &pSurfDS ) );

    // Setup HDR render target
    ( g_pTexScene->GetSurfaceLevel( 0, &pSurfHDR ) );
    if( g_bUseMultiSampleFloat16 )
    {
        ( IRenderer::GetRendererInstance()->changeRenderTarget( 0, g_pFloatMSRT ) );
        ( IRenderer::GetRendererInstance()->SetDepthStencilSurface( g_pFloatMSDS ) );
    }
    else
    {
        ( IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfHDR ) );
    }

    // Clear the viewport
    ( IRenderer::GetRendererInstance()->Clear( 0L, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA( 0, 0, 0, 0 ), 1.0f, 0L ) );

    // Render the scene
    if( ( IRenderer::GetRendererInstance()->BeginScene() ) )
    {
        // Render the HDR Scene
        {
            RenderScene();
        }

        // If using floating point multi sampling, stretchrect to the rendertarget
        if( g_bUseMultiSampleFloat16 )
        {
            ( IRenderer::GetRendererInstance()->StretchRect( g_pFloatMSRT, 0, pSurfHDR, 0, D3DTEXF_NONE ) );
            ( IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfHDR ) );
            ( IRenderer::GetRendererInstance()->SetDepthStencilSurface( pSurfDS ) );
        }

        // Create a scaled copy of the scene
        Scene_To_SceneScaled();

        // Setup tone mapping technique
        if( g_bToneMap )
            MeasureLuminance();

        // If FrameMove has been called, the user's adaptation level has also changed
        // and should be updated
        if( g_bAdaptationInvalid )
        {
            // Clear the update flag
            g_bAdaptationInvalid = false;

            // Calculate the current luminance adaptation level
            CalculateAdaptation();
        }

        // Now that luminance information has been gathered, the scene can be bright-pass filtered
        // to remove everything except bright lights and reflections.
        SceneScaled_To_BrightPass();

        // Blur the bright-pass filtered image to create the source texture for the star effect
        BrightPass_To_StarSource();

        // Scale-down the source texture for the star effect to create the source texture
        // for the bloom effect
        StarSource_To_BloomSource();

        // Render post-process lighting effects
        {
            RenderBloom();
        }
        {
            RenderStar();
        }

        // Draw the high dynamic range scene texture to the low dynamic range
        // back buffer. As part of this final pass, the scene will be tone-mapped
        // using the user's current adapted luminance, blue shift will occur
        // if the scene is determined to be very dark, and the post-process lighting
        // effect textures will be added to the scene.
        unsigned int uiPassCount, uiPass;

        ( g_pEffect->SetTechnique( "FinalScenePass" ) );
        ( g_pEffect->SetFloat( "g_fMiddleGray", g_fKeyValue ) );

        ( IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfLDR ) );
        ( IRenderer::GetRendererInstance()->setTexture( 0, g_pTexScene ) );
        ( IRenderer::GetRendererInstance()->setTexture( 1, g_apTexBloom[0] ) );
        ( IRenderer::GetRendererInstance()->setTexture( 2, g_apTexStar[0] ) );
        ( IRenderer::GetRendererInstance()->setTexture( 3, g_pTexAdaptedLuminanceCur ) );
        ( IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT ) );
        ( IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT ) );
        ( IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ) );
        ( IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ) );
        ( IRenderer::GetRendererInstance()->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ) );
        ( IRenderer::GetRendererInstance()->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ) );
        ( IRenderer::GetRendererInstance()->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_POINT ) );
        ( IRenderer::GetRendererInstance()->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_POINT ) );


        ( g_pEffect->Begin( &uiPassCount, 0 ) );
        {

            for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
            {
                ( g_pEffect->BeginPass( uiPass ) );

                DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );

                ( g_pEffect->EndPass() );
            }
        }
        ( g_pEffect->End() );

        ( IRenderer::GetRendererInstance()->setTexture( 1, 0 ) );
        ( IRenderer::GetRendererInstance()->setTexture( 2, 0 ) );
        ( IRenderer::GetRendererInstance()->setTexture( 3, 0 ) );

        ( IRenderer::GetRendererInstance()->EndScene() );
    }

    // Release surfaces
    //////////////SAFE_RELEASE( pSurfHDR );
    //////////////SAFE_RELEASE( pSurfLDR );
    //////////////SAFE_RELEASE( pSurfDS );
}


//-----------------------------------------------------------------------------
// Name: RenderScene()
// Desc: Render the world objects and lights
//-----------------------------------------------------------------------------
int RenderScene()
{
    int 0;

    unsigned int uiPassCount, uiPass;
    D3DXFROMWINEMATRIX mWorld;
    D3DXFROMWINEMATRIX mTrans;
    D3DXFROMWINEMATRIX mRotate;
    D3DXFROMWINEMATRIX mObjectToView;

    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

    D3DXFROMWINEMATRIX mView = *g_View;

    g_pEffect->SetTechnique( "RenderScene" );
    g_pEffect->SetMatrix( "g_mObjectToView", &mView );

    g_pEffect->Begin( &uiPassCount, 0 );
    if( ( hr ) )
        return hr;

    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Turn off emissive lighting
        D3DXFROMWINEVECTOR4 vNull( 0.0f, 0.0f, 0.0f, 0.0f );
        g_pEffect->SetVector( "g_vEmissive", &vNull );

        // Enable texture
        g_pEffect->SetBool( "g_bEnableTexture", true );
        IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );


        // Render walls and columns
        g_pEffect->SetFloat( "g_fPhongExponent", 5.0f );
        g_pEffect->SetFloat( "g_fPhongCoefficient", 1.0f );
        g_pEffect->SetFloat( "g_fDiffuseCoefficient", 0.5f );
        IRenderer::GetRendererInstance()->setTexture( 0, g_pTexWall );
        g_pEffect->CommitChanges();
        g_pWorldMesh->DrawSubset( 0 );

        // Render floor
        g_pEffect->SetFloat( "g_fPhongExponent", 50.0f );
        g_pEffect->SetFloat( "g_fPhongCoefficient", 3.0f );
        g_pEffect->SetFloat( "g_fDiffuseCoefficient", 1.0f );
        IRenderer::GetRendererInstance()->setTexture( 0, g_pTexFloor );
        g_pEffect->CommitChanges();
        g_pWorldMesh->DrawSubset( 1 );

        // Render ceiling
        g_pEffect->SetFloat( "g_fPhongExponent", 5.0f );
        g_pEffect->SetFloat( "g_fPhongCoefficient", 0.3f );
        g_pEffect->SetFloat( "g_fDiffuseCoefficient", 0.3f );
        IRenderer::GetRendererInstance()->setTexture( 0, g_pTexCeiling );
        g_pEffect->CommitChanges();
        g_pWorldMesh->DrawSubset( 2 );

        // Render paintings
        g_pEffect->SetFloat( "g_fPhongExponent", 5.0f );
        g_pEffect->SetFloat( "g_fPhongCoefficient", 0.3f );
        g_pEffect->SetFloat( "g_fDiffuseCoefficient", 1.0f );
        IRenderer::GetRendererInstance()->setTexture( 0, g_pTexPainting );
        g_pEffect->CommitChanges();
        g_pWorldMesh->DrawSubset( 3 );

        // Draw the light spheres.
        g_pEffect->SetFloat( "g_fPhongExponent", 5.0f );
        g_pEffect->SetFloat( "g_fPhongCoefficient", 1.0f );
        g_pEffect->SetFloat( "g_fDiffuseCoefficient", 1.0f );
        g_pEffect->SetBool( "g_bEnableTexture", false );

        for( int iLight = 0; iLight < NUM_LIGHTS; iLight++ )
        {
            // Just position the point light -- no need to orient it
            D3DXFROMWINEMATRIX mScale;
            D3DXFROMWINEMatrixScaling( &mScale, 0.05f, 0.05f, 0.05f );

            mView = *g_View;
            D3DXFROMWINEMatrixTranslation( &mWorld, g_avLightPosition[iLight].x, g_avLightPosition[iLight].y,
                                   g_avLightPosition[iLight].z );
            mWorld = mScale * mWorld;
            mObjectToView = mWorld * mView;
            g_pEffect->SetMatrix( "g_mObjectToView", &mObjectToView );

            // A light which illuminates objects at 80 lum/sr should be drawn
            // at 3183 lumens/meter^2/steradian, which equates to a multiplier
            // of 39.78 per lumen.
            D3DXFROMWINEVECTOR4 vEmissive = EMISSIVE_COEFFICIENT * g_avLightIntensity[iLight];
            g_pEffect->SetVector( "g_vEmissive", &vEmissive );

            g_pEffect->CommitChanges();
            g_pmeshSphere->DrawSubset( 0 );
        }
        g_pEffect->EndPass();
    }

    g_pEffect->End();

    return 0;
}




//-----------------------------------------------------------------------------
// Name: MeasureLuminance()
// Desc: Measure the average log luminance in the scene.
//-----------------------------------------------------------------------------
int MeasureLuminance()
{
    int 0;
    unsigned int uiPassCount, uiPass;
    int i, x, y, index;
    D3DXFROMWINEVECTOR2 avSampleOffsets[MAX_SAMPLES];


    __DWORD__ dwCurTexture = NUM_TONEMAP_TEXTURES - 1;

    // Sample log average luminance
    SurfaceID apSurfToneMap[NUM_TONEMAP_TEXTURES] = {0};

    // Retrieve the tonemap surfaces
    for( i = 0; i < NUM_TONEMAP_TEXTURES; i++ )
    {
        g_apTexToneMap[i]->GetSurfaceLevel( 0, &apSurfToneMap[i] );
        if( ( hr ) )
            goto LCleanReturn;
    }

    D3DSURFACE_DESC desc;
    g_apTexToneMap[dwCurTexture]->GetLevelDesc( 0, &desc );


    // Initialize the sample offsets for the initial luminance pass.
    float tU, tV;
    tU = 1.0f / ( 3.0f * desc.Width );
    tV = 1.0f / ( 3.0f * desc.Height );

    index = 0;
    for( x = -1; x <= 1; x++ )
    {
        for( y = -1; y <= 1; y++ )
        {
            avSampleOffsets[index].x = x * tU;
            avSampleOffsets[index].y = y * tV;

            index++;
        }
    }


    // After this pass, the g_apTexToneMap[NUM_TONEMAP_TEXTURES-1] texture will contain
    // a scaled, grayscale copy of the HDR scene. Individual texels contain the log 
    // of average luminance values for points sampled on the HDR texture.
    g_pEffect->SetTechnique( "SampleAvgLum" );
    g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, apSurfToneMap[dwCurTexture] );
    IRenderer::GetRendererInstance()->setTexture( 0, g_pTexSceneScaled );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );


    g_pEffect->Begin( &uiPassCount, 0 );
    if( ( hr ) )
        goto LCleanReturn;

    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );

        g_pEffect->EndPass();
    }

    g_pEffect->End();
    dwCurTexture--;

    // Initialize the sample offsets for the iterative luminance passes
    while( dwCurTexture > 0 )
    {
        g_apTexToneMap[dwCurTexture + 1]->GetLevelDesc( 0, &desc );
        GetSampleOffsets_DownScale4x4( desc.Width, desc.Height, avSampleOffsets );


        // Each of these passes continue to scale down the log of average
        // luminance texture created above, storing intermediate results in 
        // g_apTexToneMap[1] through g_apTexToneMap[NUM_TONEMAP_TEXTURES-1].
        g_pEffect->SetTechnique( "ResampleAvgLum" );
        g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );

        IRenderer::GetRendererInstance()->changeRenderTarget( 0, apSurfToneMap[dwCurTexture] );
        IRenderer::GetRendererInstance()->setTexture( 0, g_apTexToneMap[dwCurTexture + 1] );
        IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
        IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );


        g_pEffect->Begin( &uiPassCount, 0 );
        if( ( hr ) )
            goto LCleanReturn;

        for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
        {
            g_pEffect->BeginPass( uiPass );

            // Draw a fullscreen quad to sample the RT
            DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );

            g_pEffect->EndPass();
        }

        g_pEffect->End();
        dwCurTexture--;
    }

    // Downsample to 1x1
    g_apTexToneMap[1]->GetLevelDesc( 0, &desc );
    GetSampleOffsets_DownScale4x4( desc.Width, desc.Height, avSampleOffsets );


    // Perform the final pass of the average luminance calculation. This pass
    // scales the 4x4 log of average luminance texture from above and performs
    // an exp() operation to return a single texel cooresponding to the average
    // luminance of the scene in g_apTexToneMap[0].
    g_pEffect->SetTechnique( "ResampleAvgLumExp" );
    g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, apSurfToneMap[0] );
    IRenderer::GetRendererInstance()->setTexture( 0, g_apTexToneMap[1] );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );


    g_pEffect->Begin( &uiPassCount, 0 );
    if( ( hr ) )
        goto LCleanReturn;

    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );

        g_pEffect->EndPass();
    }

    g_pEffect->End();


    0;
LCleanReturn:
    for( i = 0; i < NUM_TONEMAP_TEXTURES; i++ )
    {
        //////////////SAFE_RELEASE( apSurfToneMap[i] );
    }

    return hr;
}




//-----------------------------------------------------------------------------
// Name: CalculateAdaptation()
// Desc: Increment the user's adapted luminance
//-----------------------------------------------------------------------------
int CalculateAdaptation()
{
    int 0;
    unsigned int uiPass, uiPassCount;

    // Swap current & last luminance
    TextureID pTexSwap = g_pTexAdaptedLuminanceLast;
    g_pTexAdaptedLuminanceLast = g_pTexAdaptedLuminanceCur;
    g_pTexAdaptedLuminanceCur = pTexSwap;

    SurfaceID pSurfAdaptedLum = 0;
    ( g_pTexAdaptedLuminanceCur->GetSurfaceLevel( 0, &pSurfAdaptedLum ) );

    // This simulates the light adaptation that occurs when moving from a 
    // dark area to a bright area, or vice versa. The g_pTexAdaptedLuminance
    // texture stores a single texel cooresponding to the user's adapted 
    // level.
    g_pEffect->SetTechnique( "CalculateAdaptedLum" );
    g_pEffect->SetFloat( "g_fElapsedTime", DXUTGetElapsedTime() );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfAdaptedLum );
    IRenderer::GetRendererInstance()->setTexture( 0, g_pTexAdaptedLuminanceLast );
    IRenderer::GetRendererInstance()->setTexture( 1, g_apTexToneMap[0] );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );


    ( g_pEffect->Begin( &uiPassCount, 0 ) );

    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );

        g_pEffect->EndPass();
    }

    g_pEffect->End();


    //////////////SAFE_RELEASE( pSurfAdaptedLum );
    return 0;
}




//-----------------------------------------------------------------------------
// Name: RenderStar()
// Desc: Render the blooming effect
//-----------------------------------------------------------------------------
int RenderStar()
{
    int 0;
    unsigned int uiPassCount, uiPass;
    int i, d, p, s; // Loop variables

    SurfaceID pSurfStar = 0;
    g_apTexStar[0]->GetSurfaceLevel( 0, &pSurfStar );
    if( ( hr ) )
        return hr;

    // Clear the star texture
    IRenderer::GetRendererInstance()->ColorFill( pSurfStar, 0, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
    //////////////SAFE_RELEASE( pSurfStar );

    // Avoid rendering the star if it's not being used in the current glare
    if( g_GlareDef.m_fGlareLuminance <= 0.0f ||
        g_GlareDef.m_fStarLuminance <= 0.0f )
    {
        return 0;
    }

    // Initialize the constants used during the effect
    const CStarDef& starDef = g_GlareDef.m_starDef;
    const float fTanFoV = atanf( D3DXFROMWINE_PI / 8 );
    const D3DXFROMWINEVECTOR4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );
    static const int s_maxPasses = 3;
    static const int nSamples = 8;
    static D3DXFROMWINEVECTOR4 s_aaColor[s_maxPasses][8];
    static const D3DXFROMWINEVECTOR4 s_colorWhite( 0.63f, 0.63f, 0.63f, 0.0f );

    D3DXFROMWINEVECTOR4 avSampleWeights[MAX_SAMPLES];
    D3DXFROMWINEVECTOR2 avSampleOffsets[MAX_SAMPLES];

    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    SurfaceID pSurfSource = 0;
    SurfaceID pSurfDest = 0;

    // Set aside all the star texture surfaces as a convenience
    SurfaceID apSurfStar[NUM_STAR_TEXTURES] = {0};
    for( i = 0; i < NUM_STAR_TEXTURES; i++ )
    {
        g_apTexStar[i]->GetSurfaceLevel( 0, &apSurfStar[i] );
        if( ( hr ) )
            goto LCleanReturn;
    }

    // Get the source texture dimensions
    g_pTexStarSource->GetSurfaceLevel( 0, &pSurfSource );
    if( ( hr ) )
        goto LCleanReturn;

    D3DSURFACE_DESC desc;
    pSurfSource->GetDesc( &desc );
    if( ( hr ) )
        goto LCleanReturn;

    //////////////SAFE_RELEASE( pSurfSource );

    float srcW;
    srcW = ( FLOAT )desc.Width;
    float srcH;
    srcH = ( FLOAT )desc.Height;



    for( p = 0; p < s_maxPasses; p ++ )
    {
        float ratio;
        ratio = ( float )( p + 1 ) / ( float )s_maxPasses;

        for( s = 0; s < nSamples; s ++ )
        {
            D3DXFROMWINEVECTOR4 chromaticAberrColor;
            D3DXFROMWINEColorLerp( &chromaticAberrColor,
                           &( CStarDef::GetChromaticAberrationColor( s ) ),
                           &s_colorWhite,
                           ratio );

            D3DXFROMWINEColorLerp( ( D3DXFROMWINEVECTOR4* )&( s_aaColor[p][s] ),
                           &s_colorWhite, &chromaticAberrColor,
                           g_GlareDef.m_fChromaticAberration );
        }
    }

    float radOffset;
    radOffset = g_GlareDef.m_fStarInclination + starDef.m_fInclination;


    TextureID pTexSource;


    // Direction loop
    for( d = 0; d < starDef.m_nStarLines; d ++ )
    {
        CONST STARLINE& starLine = starDef.m_pStarLine[d];

        pTexSource = g_pTexStarSource;

        float rad;
        rad = radOffset + starLine.fInclination;
        float sn, cs;
        sn = sinf( rad ), cs = cosf( rad );
        D3DXFROMWINEVECTOR2 vtStepUV;
        vtStepUV.x = sn / srcW * starLine.fSampleLength;
        vtStepUV.y = cs / srcH * starLine.fSampleLength;

        float attnPowScale;
        attnPowScale = ( fTanFoV + 0.1f ) * 1.0f *
            ( 160.0f + 120.0f ) / ( srcW + srcH ) * 1.2f;

        // 1 direction expansion loop
        IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

        int iWorkTexture;
        iWorkTexture = 1;
        for( p = 0; p < starLine.nPasses; p ++ )
        {

            if( p == starLine.nPasses - 1 )
            {
                // Last pass move to other work buffer
                pSurfDest = apSurfStar[d + 4];
            }
            else
            {
                pSurfDest = apSurfStar[iWorkTexture];
            }

            // Sampling configration for each stage
            for( i = 0; i < nSamples; i ++ )
            {
                float lum;
                lum = powf( starLine.fAttenuation, attnPowScale * i );

                avSampleWeights[i] = s_aaColor[starLine.nPasses - 1 - p][i] *
                    lum * ( p + 1.0f ) * 0.5f;


                // Offset of sampling coordinate
                avSampleOffsets[i].x = vtStepUV.x * i;
                avSampleOffsets[i].y = vtStepUV.y * i;
                if( fabs( avSampleOffsets[i].x ) >= 0.9f ||
                    fabs( avSampleOffsets[i].y ) >= 0.9f )
                {
                    avSampleOffsets[i].x = 0.0f;
                    avSampleOffsets[i].y = 0.0f;
                    avSampleWeights[i] *= 0.0f;
                }

            }


            g_pEffect->SetTechnique( "Star" );
            g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );
            g_pEffect->SetVectorArray( "g_avSampleWeights", avSampleWeights, nSamples );

            IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfDest );
            IRenderer::GetRendererInstance()->setTexture( 0, pTexSource );
            IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
            IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );


            g_pEffect->Begin( &uiPassCount, 0 );
            if( ( hr ) )
                return hr;

            for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
            {
                g_pEffect->BeginPass( uiPass );

                // Draw a fullscreen quad to sample the RT
                DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );

                g_pEffect->EndPass();
            }

            g_pEffect->End();

            // Setup next expansion
            vtStepUV *= nSamples;
            attnPowScale *= nSamples;

            // Set the work drawn just before to next texture source.
            pTexSource = g_apTexStar[iWorkTexture];

            iWorkTexture += 1;
            if( iWorkTexture > 2 )
            {
                iWorkTexture = 1;
            }

        }
    }


    pSurfDest = apSurfStar[0];


    for( i = 0; i < starDef.m_nStarLines; i++ )
    {
        IRenderer::GetRendererInstance()->setTexture( i, g_apTexStar[i + 4] );
        IRenderer::GetRendererInstance()->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        IRenderer::GetRendererInstance()->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

        avSampleWeights[i] = vWhite * 1.0f / ( FLOAT )starDef.m_nStarLines;
    }

    char strTechnique[256];
    stx_snprintf( strTechnique, 256, "MergeTextures_%d", starDef.m_nStarLines );

    g_pEffect->SetTechnique( strTechnique );

    g_pEffect->SetVectorArray( "g_avSampleWeights", avSampleWeights, starDef.m_nStarLines );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfDest );

    g_pEffect->Begin( &uiPassCount, 0 );
    if( ( hr ) )
        goto LCleanReturn;

    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );

        g_pEffect->EndPass();
    }

    g_pEffect->End();

    for( i = 0; i < starDef.m_nStarLines; i++ )
        IRenderer::GetRendererInstance()->setTexture( i, 0 );


    0;
LCleanReturn:
    for( i = 0; i < NUM_STAR_TEXTURES; i++ )
    {
        //////////////SAFE_RELEASE( apSurfStar[i] );
    }

    return hr;
}




//-----------------------------------------------------------------------------
// Name: RenderBloom()
// Desc: Render the blooming effect
//-----------------------------------------------------------------------------
int RenderBloom()
{
    int 0;
    unsigned int uiPassCount, uiPass;
    int i = 0;


    D3DXFROMWINEVECTOR2 avSampleOffsets[MAX_SAMPLES];
    FLOAT afSampleOffsets[MAX_SAMPLES];
    D3DXFROMWINEVECTOR4 avSampleWeights[MAX_SAMPLES];

    SurfaceID pSurfScaledHDR;
    g_pTexSceneScaled->GetSurfaceLevel( 0, &pSurfScaledHDR );

    SurfaceID pSurfBloom;
    g_apTexBloom[0]->GetSurfaceLevel( 0, &pSurfBloom );

    SurfaceID pSurfHDR;
    g_pTexScene->GetSurfaceLevel( 0, &pSurfHDR );

    SurfaceID pSurfTempBloom;
    g_apTexBloom[1]->GetSurfaceLevel( 0, &pSurfTempBloom );

    SurfaceID pSurfBloomSource;
    g_apTexBloom[2]->GetSurfaceLevel( 0, &pSurfBloomSource );

    // Clear the bloom texture
    IRenderer::GetRendererInstance()->ColorFill( pSurfBloom, 0, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );

    if( g_GlareDef.m_fGlareLuminance <= 0.0f ||
        g_GlareDef.m_fBloomLuminance <= 0.0f )
    {
        0;
        goto LCleanReturn;
    }

    RECT rectSrc;
    GetTextureRect( g_pTexBloomSource, &rectSrc );
    InflateRect( &rectSrc, -1, -1 );

    RECT rectDest;
    GetTextureRect( g_apTexBloom[2], &rectDest );
    InflateRect( &rectDest, -1, -1 );

    CoordRect coords;
    GetTextureCoords( g_pTexBloomSource, &rectSrc, g_apTexBloom[2], &rectDest, &coords );

    D3DSURFACE_DESC desc;
    g_pTexBloomSource->GetLevelDesc( 0, &desc );
    if( ( hr ) )
        return hr;


    g_pEffect->SetTechnique( "GaussBlur5x5" );

    GetSampleOffsets_GaussBlur5x5( desc.Width, desc.Height, avSampleOffsets, avSampleWeights, 1.0f );

    g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );
    g_pEffect->SetValue( "g_avSampleWeights", avSampleWeights, sizeof( avSampleWeights ) );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfBloomSource );
    IRenderer::GetRendererInstance()->setTexture( 0, g_pTexBloomSource );
    IRenderer::GetRendererInstance()->SetScissorRect( &rectDest );
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );


    g_pEffect->Begin( &uiPassCount, 0 );
    if( ( hr ) )
        goto LCleanReturn;

    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad( coords );

        g_pEffect->EndPass();
    }
    g_pEffect->End();
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

    g_apTexBloom[2]->GetLevelDesc( 0, &desc );
    if( ( hr ) )
        return hr;

    GetSampleOffsets_Bloom( desc.Width, afSampleOffsets, avSampleWeights, 3.0f, 2.0f );
    for( i = 0; i < MAX_SAMPLES; i++ )
    {
        avSampleOffsets[i] = D3DXFROMWINEVECTOR2( afSampleOffsets[i], 0.0f );
    }


    g_pEffect->SetTechnique( "Bloom" );
    g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );
    g_pEffect->SetValue( "g_avSampleWeights", avSampleWeights, sizeof( avSampleWeights ) );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfTempBloom );
    IRenderer::GetRendererInstance()->setTexture( 0, g_apTexBloom[2] );
    IRenderer::GetRendererInstance()->SetScissorRect( &rectDest );
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );


    g_pEffect->Begin( &uiPassCount, 0 );
    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad( coords );

        g_pEffect->EndPass();
    }
    g_pEffect->End();
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );


    g_apTexBloom[1]->GetLevelDesc( 0, &desc );
    if( ( hr ) )
        return hr;

    GetSampleOffsets_Bloom( desc.Height, afSampleOffsets, avSampleWeights, 3.0f, 2.0f );
    for( i = 0; i < MAX_SAMPLES; i++ )
    {
        avSampleOffsets[i] = D3DXFROMWINEVECTOR2( 0.0f, afSampleOffsets[i] );
    }


    GetTextureRect( g_apTexBloom[1], &rectSrc );
    InflateRect( &rectSrc, -1, -1 );

    GetTextureCoords( g_apTexBloom[1], &rectSrc, g_apTexBloom[0], 0, &coords );


    g_pEffect->SetTechnique( "Bloom" );
    g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );
    g_pEffect->SetValue( "g_avSampleWeights", avSampleWeights, sizeof( avSampleWeights ) );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfBloom );
    IRenderer::GetRendererInstance()->setTexture( 0, g_apTexBloom[1] );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );


    g_pEffect->Begin( &uiPassCount, 0 );
    if( ( hr ) )
        goto LCleanReturn;

    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad( coords );

        g_pEffect->EndPass();
    }

    g_pEffect->End();


    0;

LCleanReturn:
    //////////////SAFE_RELEASE( pSurfBloomSource );
    //////////////SAFE_RELEASE( pSurfTempBloom );
    //////////////SAFE_RELEASE( pSurfBloom );
    //////////////SAFE_RELEASE( pSurfHDR );
    //////////////SAFE_RELEASE( pSurfScaledHDR );

    return hr;
}



//-----------------------------------------------------------------------------
// Name: DrawFullScreenQuad
// Desc: Draw a properly aligned quad covering the entire render target
//-----------------------------------------------------------------------------
void DrawFullScreenQuad( float fLeftU, float fTopV, float fRightU, float fBottomV )
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
    ScreenVertex svQuad[4];

    svQuad[0].p = D3DXFROMWINEVECTOR4( -0.5f, -0.5f, 0.5f, 1.0f );
    svQuad[0].t = D3DXFROMWINEVECTOR2( fLeftU, fTopV );

    svQuad[1].p = D3DXFROMWINEVECTOR4( fWidth5, -0.5f, 0.5f, 1.0f );
    svQuad[1].t = D3DXFROMWINEVECTOR2( fRightU, fTopV );

    svQuad[2].p = D3DXFROMWINEVECTOR4( -0.5f, fHeight5, 0.5f, 1.0f );
    svQuad[2].t = D3DXFROMWINEVECTOR2( fLeftU, fBottomV );

    svQuad[3].p = D3DXFROMWINEVECTOR4( fWidth5, fHeight5, 0.5f, 1.0f );
    svQuad[3].t = D3DXFROMWINEVECTOR2( fRightU, fBottomV );

    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ZENABLE, FALSE );
    IRenderer::GetRendererInstance()->SetFVF( ScreenVertex::FVF );
    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, svQuad, sizeof( ScreenVertex ) );
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ZENABLE, TRUE );
}


//-----------------------------------------------------------------------------
// Name: GetTextureRect()
// Desc: Get the dimensions of the texture
//-----------------------------------------------------------------------------
int GetTextureRect( TextureID pTexture, RECT* pRect )
{
    int 0;

    if( pTexture == 0 || pRect == 0 )
        return E_INVALIDARG;

    D3DSURFACE_DESC desc;
    pTexture->GetLevelDesc( 0, &desc );
    if( ( hr ) )
        return hr;

    pRect->left = 0;
    pRect->top = 0;
    pRect->right = desc.Width;
    pRect->bottom = desc.Height;

    return 0;
}




//-----------------------------------------------------------------------------
// Name: GetTextureCoords()
// Desc: Get the texture coordinates to use when rendering into the destination
//       texture, given the source and destination rectangles
//-----------------------------------------------------------------------------
int GetTextureCoords( TextureID pTexSrc, RECT* pRectSrc,
                          TextureID pTexDest, RECT* pRectDest, CoordRect* pCoords )
{
    int 0;
    D3DSURFACE_DESC desc;
    float tU, tV;

    // Validate arguments
    if( pTexSrc == 0 || pTexDest == 0 || pCoords == 0 )
        return E_INVALIDARG;

    // Start with a default mapping of the complete source surface to complete 
    // destination surface
    pCoords->fLeftU = 0.0f;
    pCoords->fTopV = 0.0f;
    pCoords->fRightU = 1.0f;
    pCoords->fBottomV = 1.0f;

    // If not using the complete source surface, adjust the coordinates
    if( pRectSrc != 0 )
    {
        // Get destination texture description
        pTexSrc->GetLevelDesc( 0, &desc );
        if( ( hr ) )
            return hr;

        // These delta values are the distance between source texel centers in 
        // texture address space
        tU = 1.0f / desc.Width;
        tV = 1.0f / desc.Height;

        pCoords->fLeftU += pRectSrc->left * tU;
        pCoords->fTopV += pRectSrc->top * tV;
        pCoords->fRightU -= ( desc.Width - pRectSrc->right ) * tU;
        pCoords->fBottomV -= ( desc.Height - pRectSrc->bottom ) * tV;
    }

    // If not drawing to the complete destination surface, adjust the coordinates
    if( pRectDest != 0 )
    {
        // Get source texture description
        pTexDest->GetLevelDesc( 0, &desc );
        if( ( hr ) )
            return hr;

        // These delta values are the distance between source texel centers in 
        // texture address space
        tU = 1.0f / desc.Width;
        tV = 1.0f / desc.Height;

        pCoords->fLeftU -= pRectDest->left * tU;
        pCoords->fTopV -= pRectDest->top * tV;
        pCoords->fRightU += ( desc.Width - pRectDest->right ) * tU;
        pCoords->fBottomV += ( desc.Height - pRectDest->bottom ) * tV;
    }

    return 0;
}




//-----------------------------------------------------------------------------
// Name: Scene_To_SceneScaled()
// Desc: Scale down g_pTexScene by 1/4 x 1/4 and place the result in 
//       g_pTexSceneScaled
//-----------------------------------------------------------------------------
int Scene_To_SceneScaled()
{
    int 0;
    D3DXFROMWINEVECTOR2 avSampleOffsets[MAX_SAMPLES];


    // Get the new render target surface
    SurfaceID pSurfScaledScene = 0;
    g_pTexSceneScaled->GetSurfaceLevel( 0, &pSurfScaledScene );
    if( ( hr ) )
        goto LCleanReturn;

    const D3DSURFACE_DESC* pBackBufferDesc = DXUTGetD3D9BackBufferSurfaceDesc();

    // Create a 1/4 x 1/4 scale copy of the HDR texture. Since bloom textures
    // are 1/8 x 1/8 scale, border texels of the HDR texture will be discarded 
    // to keep the dimensions evenly divisible by 8; this allows for precise 
    // control over sampling inside pixel shaders.
    g_pEffect->SetTechnique( "DownScale4x4" );

    // Place the rectangle in the center of the back buffer surface
    RECT rectSrc;
    rectSrc.left = ( STX_Service::GetWindowInstance()->GetWidth() - g_dwCropWidth ) / 2;
    rectSrc.top = ( STX_Service::GetWindowInstance()->GetHeight() - g_dwCropHeight ) / 2;
    rectSrc.right = rectSrc.left + g_dwCropWidth;
    rectSrc.bottom = rectSrc.top + g_dwCropHeight;

    // Get the texture coordinates for the render target
    CoordRect coords;
    GetTextureCoords( g_pTexScene, &rectSrc, g_pTexSceneScaled, 0, &coords );

    // Get the sample offsets used within the pixel shader
    GetSampleOffsets_DownScale4x4( STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(), avSampleOffsets );
    g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfScaledScene );
    IRenderer::GetRendererInstance()->setTexture( 0, g_pTexScene );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

    unsigned int uiPassCount, uiPass;
    g_pEffect->Begin( &uiPassCount, 0 );
    if( ( hr ) )
        goto LCleanReturn;

    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad
        DrawFullScreenQuad( coords );

        g_pEffect->EndPass();
    }

    g_pEffect->End();


    0;
LCleanReturn:
    //////////////SAFE_RELEASE( pSurfScaledScene );
    return hr;
}




//-----------------------------------------------------------------------------
// Name: SceneScaled_To_BrightPass
// Desc: Run the bright-pass filter on g_pTexSceneScaled and place the result
//       in g_pTexBrightPass
//-----------------------------------------------------------------------------
int SceneScaled_To_BrightPass()
{
    int 0;

    D3DXFROMWINEVECTOR2 avSampleOffsets[MAX_SAMPLES];
    D3DXFROMWINEVECTOR4 avSampleWeights[MAX_SAMPLES];


    // Get the new render target surface
    SurfaceID pSurfBrightPass;
    g_pTexBrightPass->GetSurfaceLevel( 0, &pSurfBrightPass );
    if( ( hr ) )
        goto LCleanReturn;


    D3DSURFACE_DESC desc;
    g_pTexSceneScaled->GetLevelDesc( 0, &desc );

    // Get the rectangle describing the sampled portion of the source texture.
    // Decrease the rectangle to adjust for the single pixel black border.
    RECT rectSrc;
    GetTextureRect( g_pTexSceneScaled, &rectSrc );
    InflateRect( &rectSrc, -1, -1 );

    // Get the destination rectangle.
    // Decrease the rectangle to adjust for the single pixel black border.
    RECT rectDest;
    GetTextureRect( g_pTexBrightPass, &rectDest );
    InflateRect( &rectDest, -1, -1 );

    // Get the correct texture coordinates to apply to the rendered quad in order 
    // to sample from the source rectangle and render into the destination rectangle
    CoordRect coords;
    GetTextureCoords( g_pTexSceneScaled, &rectSrc, g_pTexBrightPass, &rectDest, &coords );

    // The bright-pass filter removes everything from the scene except lights and
    // bright reflections
    g_pEffect->SetTechnique( "BrightPassFilter" );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfBrightPass );
    IRenderer::GetRendererInstance()->setTexture( 0, g_pTexSceneScaled );
    IRenderer::GetRendererInstance()->setTexture( 1, g_pTexAdaptedLuminanceCur );
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
    IRenderer::GetRendererInstance()->SetScissorRect( &rectDest );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );

    unsigned int uiPass, uiPassCount;
    g_pEffect->Begin( &uiPassCount, 0 );
    if( ( hr ) )
        goto LCleanReturn;

    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad( coords );

        g_pEffect->EndPass();
    }

    g_pEffect->End();
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

    0;
LCleanReturn:
    //////////////SAFE_RELEASE( pSurfBrightPass );
    return hr;
}




//-----------------------------------------------------------------------------
// Name: BrightPass_To_StarSource
// Desc: Perform a 5x5 gaussian blur on g_pTexBrightPass and place the result
//       in g_pTexStarSource. The bright-pass filtered image is blurred before
//       being used for star operations to avoid aliasing artifacts.
//-----------------------------------------------------------------------------
int BrightPass_To_StarSource()
{
    int 0;

    D3DXFROMWINEVECTOR2 avSampleOffsets[MAX_SAMPLES];
    D3DXFROMWINEVECTOR4 avSampleWeights[MAX_SAMPLES];

    // Get the new render target surface
    SurfaceID pSurfStarSource;
    g_pTexStarSource->GetSurfaceLevel( 0, &pSurfStarSource );
    if( ( hr ) )
        goto LCleanReturn;


    // Get the destination rectangle.
    // Decrease the rectangle to adjust for the single pixel black border.
    RECT rectDest;
    GetTextureRect( g_pTexStarSource, &rectDest );
    InflateRect( &rectDest, -1, -1 );

    // Get the correct texture coordinates to apply to the rendered quad in order 
    // to sample from the source rectangle and render into the destination rectangle
    CoordRect coords;
    GetTextureCoords( g_pTexBrightPass, 0, g_pTexStarSource, &rectDest, &coords );

    // Get the sample offsets used within the pixel shader
    D3DSURFACE_DESC desc;
    g_pTexBrightPass->GetLevelDesc( 0, &desc );
    if( ( hr ) )
        return hr;


    GetSampleOffsets_GaussBlur5x5( desc.Width, desc.Height, avSampleOffsets, avSampleWeights );
    g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );
    g_pEffect->SetValue( "g_avSampleWeights", avSampleWeights, sizeof( avSampleWeights ) );

    // The gaussian blur smooths out rough edges to avoid aliasing effects
    // when the star effect is run
    g_pEffect->SetTechnique( "GaussBlur5x5" );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfStarSource );
    IRenderer::GetRendererInstance()->setTexture( 0, g_pTexBrightPass );
    IRenderer::GetRendererInstance()->SetScissorRect( &rectDest );
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

    unsigned int uiPassCount, uiPass;
    g_pEffect->Begin( &uiPassCount, 0 );
    if( ( hr ) )
        goto LCleanReturn;

    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad
        DrawFullScreenQuad( coords );

        g_pEffect->EndPass();
    }

    g_pEffect->End();
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );


    0;
LCleanReturn:
    //////////////SAFE_RELEASE( pSurfStarSource );
    return hr;
}




//-----------------------------------------------------------------------------
// Name: StarSource_To_BloomSource
// Desc: Scale down g_pTexStarSource by 1/2 x 1/2 and place the result in 
//       g_pTexBloomSource
//-----------------------------------------------------------------------------
int StarSource_To_BloomSource()
{
    int 0;

    D3DXFROMWINEVECTOR2 avSampleOffsets[MAX_SAMPLES];

    // Get the new render target surface
    SurfaceID pSurfBloomSource;
    g_pTexBloomSource->GetSurfaceLevel( 0, &pSurfBloomSource );
    if( ( hr ) )
        goto LCleanReturn;


    // Get the rectangle describing the sampled portion of the source texture.
    // Decrease the rectangle to adjust for the single pixel black border.
    RECT rectSrc;
    GetTextureRect( g_pTexStarSource, &rectSrc );
    InflateRect( &rectSrc, -1, -1 );

    // Get the destination rectangle.
    // Decrease the rectangle to adjust for the single pixel black border.
    RECT rectDest;
    GetTextureRect( g_pTexBloomSource, &rectDest );
    InflateRect( &rectDest, -1, -1 );

    // Get the correct texture coordinates to apply to the rendered quad in order 
    // to sample from the source rectangle and render into the destination rectangle
    CoordRect coords;
    GetTextureCoords( g_pTexStarSource, &rectSrc, g_pTexBloomSource, &rectDest, &coords );

    // Get the sample offsets used within the pixel shader
    D3DSURFACE_DESC desc;
    g_pTexBrightPass->GetLevelDesc( 0, &desc );
    if( ( hr ) )
        return hr;

    GetSampleOffsets_DownScale2x2( desc.Width, desc.Height, avSampleOffsets );
    g_pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );

    // Create an exact 1/2 x 1/2 copy of the source texture
    g_pEffect->SetTechnique( "DownScale2x2" );

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurfBloomSource );
    IRenderer::GetRendererInstance()->setTexture( 0, g_pTexStarSource );
    IRenderer::GetRendererInstance()->SetScissorRect( &rectDest );
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

    unsigned int uiPassCount, uiPass;
    g_pEffect->Begin( &uiPassCount, 0 );
    if( ( hr ) )
        goto LCleanReturn;

    for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
    {
        g_pEffect->BeginPass( uiPass );

        // Draw a fullscreen quad
        DrawFullScreenQuad( coords );

        g_pEffect->EndPass();
    }

    g_pEffect->End();
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );



    0;
LCleanReturn:
    //////////////SAFE_RELEASE( pSurfBloomSource );
    return hr;
}




//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_DownScale4x4
// Desc: Get the texture coordinate offsets to be used inside the DownScale4x4
//       pixel shader.
//-----------------------------------------------------------------------------
int GetSampleOffsets_DownScale4x4( __DWORD__ dwWidth, __DWORD__ dwHeight, D3DXFROMWINEVECTOR2 avSampleOffsets[] )
{
    if( 0 == avSampleOffsets )
        return E_INVALIDARG;

    float tU = 1.0f / dwWidth;
    float tV = 1.0f / dwHeight;

    // Sample from the 16 surrounding points. Since the center point will be in
    // the exact center of 16 texels, a 0.5f offset is needed to specify a texel
    // center.
    int index = 0;
    for( int y = 0; y < 4; y++ )
    {
        for( int x = 0; x < 4; x++ )
        {
            avSampleOffsets[ index ].x = ( x - 1.5f ) * tU;
            avSampleOffsets[ index ].y = ( y - 1.5f ) * tV;

            index++;
        }
    }

    return 0;
}




//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_DownScale2x2
// Desc: Get the texture coordinate offsets to be used inside the DownScale2x2
//       pixel shader.
//-----------------------------------------------------------------------------
int GetSampleOffsets_DownScale2x2( __DWORD__ dwWidth, __DWORD__ dwHeight, D3DXFROMWINEVECTOR2 avSampleOffsets[] )
{
    if( 0 == avSampleOffsets )
        return E_INVALIDARG;

    float tU = 1.0f / dwWidth;
    float tV = 1.0f / dwHeight;

    // Sample from the 4 surrounding points. Since the center point will be in
    // the exact center of 4 texels, a 0.5f offset is needed to specify a texel
    // center.
    int index = 0;
    for( int y = 0; y < 2; y++ )
    {
        for( int x = 0; x < 2; x++ )
        {
            avSampleOffsets[ index ].x = ( x - 0.5f ) * tU;
            avSampleOffsets[ index ].y = ( y - 0.5f ) * tV;

            index++;
        }
    }

    return 0;
}




//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_GaussBlur5x5
// Desc: Get the texture coordinate offsets to be used inside the GaussBlur5x5
//       pixel shader.
//-----------------------------------------------------------------------------
int GetSampleOffsets_GaussBlur5x5( __DWORD__ dwD3DTexWidth,
                                       __DWORD__ dwD3DTexHeight,
                                       D3DXFROMWINEVECTOR2* avTexCoordOffset,
                                       D3DXFROMWINEVECTOR4* avSampleWeight,
                                       FLOAT fMultiplier )
{
    float tu = 1.0f / ( float )dwD3DTexWidth;
    float tv = 1.0f / ( float )dwD3DTexHeight;

    D3DXFROMWINEVECTOR4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );

    float totalWeight = 0.0f;
    int index = 0;
    for( int x = -2; x <= 2; x++ )
    {
        for( int y = -2; y <= 2; y++ )
        {
            // Exclude pixels with a block distance greater than 2. This will
            // create a kernel which approximates a 5x5 kernel using only 13
            // sample points instead of 25; this is necessary since 2.0 shaders
            // only support 16 texture grabs.
            if( abs( x ) + abs( y ) > 2 )
                continue;

            // Get the unscaled Gaussian intensity for this offset
            avTexCoordOffset[index] = D3DXFROMWINEVECTOR2( x * tu, y * tv );
            avSampleWeight[index] = vWhite * GaussianDistribution( ( float )x, ( float )y, 1.0f );
            totalWeight += avSampleWeight[index].x;

            index++;
        }
    }

    // Divide the current weight by the total weight of all the samples; Gaussian
    // blur kernels add to 1.0f to ensure that the intensity of the image isn't
    // changed when the blur occurs. An optional multiplier variable is used to
    // add or remove image intensity during the blur.
    for( int i = 0; i < index; i++ )
    {
        avSampleWeight[i] /= totalWeight;
        avSampleWeight[i] *= fMultiplier;
    }

    return 0;
}




//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_Bloom
// Desc: Get the texture coordinate offsets to be used inside the Bloom
//       pixel shader.
//-----------------------------------------------------------------------------
int GetSampleOffsets_Bloom( __DWORD__ dwD3DTexSize,
                                float afTexCoordOffset[15],
                                D3DXFROMWINEVECTOR4* avColorWeight,
                                float fDeviation,
                                float fMultiplier )
{
    int i = 0;
    float tu = 1.0f / ( float )dwD3DTexSize;

    // Fill the center texel
    float weight = fMultiplier * GaussianDistribution( 0, 0, fDeviation );
    avColorWeight[0] = D3DXFROMWINEVECTOR4( weight, weight, weight, 1.0f );

    afTexCoordOffset[0] = 0.0f;

    // Fill the first half
    for( i = 1; i < 8; i++ )
    {
        // Get the Gaussian intensity for this offset
        weight = fMultiplier * GaussianDistribution( ( float )i, 0, fDeviation );
        afTexCoordOffset[i] = i * tu;

        avColorWeight[i] = D3DXFROMWINEVECTOR4( weight, weight, weight, 1.0f );
    }

    // Mirror to the second half
    for( i = 8; i < 15; i++ )
    {
        avColorWeight[i] = avColorWeight[i - 7];
        afTexCoordOffset[i] = -afTexCoordOffset[i - 7];
    }

    return 0;
}




//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_Bloom
// Desc: Get the texture coordinate offsets to be used inside the Bloom
//       pixel shader.
//-----------------------------------------------------------------------------
int GetSampleOffsets_Star( __DWORD__ dwD3DTexSize,
                               float afTexCoordOffset[15],
                               D3DXFROMWINEVECTOR4* avColorWeight,
                               float fDeviation )
{
    int i = 0;
    float tu = 1.0f / ( float )dwD3DTexSize;

    // Fill the center texel
    float weight = 1.0f * GaussianDistribution( 0, 0, fDeviation );
    avColorWeight[0] = D3DXFROMWINEVECTOR4( weight, weight, weight, 1.0f );

    afTexCoordOffset[0] = 0.0f;

    // Fill the first half
    for( i = 1; i < 8; i++ )
    {
        // Get the Gaussian intensity for this offset
        weight = 1.0f * GaussianDistribution( ( float )i, 0, fDeviation );
        afTexCoordOffset[i] = i * tu;

        avColorWeight[i] = D3DXFROMWINEVECTOR4( weight, weight, weight, 1.0f );
    }

    // Mirror to the second half
    for( i = 8; i < 15; i++ )
    {
        avColorWeight[i] = avColorWeight[i - 7];
        afTexCoordOffset[i] = -afTexCoordOffset[i - 7];
    }

    return 0;
}




//-----------------------------------------------------------------------------
// Name: GaussianDistribution
// Desc: Helper function for GetSampleOffsets function to compute the 
//       2 parameter Gaussian distrubution using the given standard deviation
//       rho
//-----------------------------------------------------------------------------
float GaussianDistribution( float x, float y, float rho )
{
    float g = 1.0f / sqrtf( 2.0f * D3DXFROMWINE_PI * rho * rho );
    g *= expf( -( x * x + y * y ) / ( 2 * rho * rho ) );

    return g;
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
	IRenderer* r=IRenderer::GetRendererInstance("HDRLightning");
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

