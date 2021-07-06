///--------------------------------------------------------------------------------------
// File: DetailTessellation11.cpp
//
// This sample demonstrates the use of detail tessellation for improving the 
// quality of material surfaces in real-time rendering applications.
//
// Developed by AMD Developer Relations team.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
//include "resource.h"
#include "Grid_Creation11.h"

//--------------------------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------------------------
#define DWORD_POSITIVE_RANDOM(x)    ((__DWORD__)(( ((x)*rand()) / RAND_MAX )))
#define FLOAT_POSITIVE_RANDOM(x)    ( ((x)*rand()) / RAND_MAX )
#define FLOAT_RANDOM(x)             ((((2.0f*rand())/RAND_MAX) - 1.0f)*(x))

//--------------------------------------------------------------------------------------
// Constants
//--------------------------------------------------------------------------------------
const float GRID_SIZE               = 200.0f;
const float MAX_TESSELLATION_FACTOR = 11.0f;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct CONSTANT_BUFFER_STRUCT
{
    D3DXFROMWINEMATRIX    mWorld;                         // World matrix
    D3DXFROMWINEMATRIX    mView;                          // View matrix
    D3DXFROMWINEMATRIX    mProjection;                    // Projection matrix
    D3DXFROMWINEMATRIX    mWorldViewProjection;           // WVP matrix
    D3DXFROMWINEMATRIX    mViewProjection;                // VP matrix
    D3DXFROMWINEMATRIX    mInvView;                       // Inverse of view matrix
    D3DXFROMWINEVECTOR4      vMeshColor;                     // Mesh color
    D3DXFROMWINEVECTOR4      vTessellationFactor;            // Edge, inside and minimum tessellation factor
    D3DXFROMWINEVECTOR4      vDetailTessellationHeightScale; // Height scale for detail tessellation of grid surface
    D3DXFROMWINEVECTOR4      vGridSize;                      // Grid size
    D3DXFROMWINEVECTOR4      vDebugColorMultiply;            // Debug colors
    D3DXFROMWINEVECTOR4      vDebugColorAdd;                 // Debug colors
};                                                      
                                                        
struct MATERIAL_CB_STRUCT
{
    D3DXFROMWINEVECTOR4     g_materialAmbientColor;  // Material's ambient color
    D3DXFROMWINEVECTOR4     g_materialDiffuseColor;  // Material's diffuse color
    D3DXFROMWINEVECTOR4     g_materialSpecularColor; // Material's specular color
    D3DXFROMWINEVECTOR4     g_fSpecularExponent;     // Material's specular exponent

    D3DXFROMWINEVECTOR4     g_LightPosition;         // Light's position in world space
    D3DXFROMWINEVECTOR4     g_LightDiffuse;          // Light's diffuse color
    D3DXFROMWINEVECTOR4     g_LightAmbient;          // Light's ambient color

    D3DXFROMWINEVECTOR4     g_vEye;                  // Camera's location
    D3DXFROMWINEVECTOR4     g_fBaseTextureRepeat;    // The tiling factor for base and normal map textures
    D3DXFROMWINEVECTOR4     g_fPOMHeightMapScale;    // Describes the useful range of values for the height field

    D3DXFROMWINEVECTOR4     g_fShadowSoftening;      // Blurring factor for the soft shadows computation

    int             g_nMinSamples;           // The minimum number of samples for sampling the height field
    int             g_nMaxSamples;           // The maximum number of samples for sampling the height field
    int             uDummy1;
    int             uDummy2;
};

struct DETAIL_TESSELLATION_TEXTURE_STRUCT
{
    char* DiffuseMap;                  // Diffuse texture map
    char* NormalHeightMap;             // Normal and height map (normal in .xyz, height in .w)
    char* DisplayName;                 // Display name of texture
    float  fHeightScale;                // Height scale when rendering
    float  fBaseTextureRepeat;          // Base repeat of texture coordinates (1.0f for no repeat)
    float  fDensityScale;               // Density scale (used for density map generation)
    float  fMeaningfulDifference;       // Meaningful difference (used for density map generation)
};

//--------------------------------------------------------------------------------------
// Enums
//--------------------------------------------------------------------------------------
enum
{
    TESSELLATIONMETHOD_DISABLED,
    TESSELLATIONMETHOD_DISABLED_POM,
    TESSELLATIONMETHOD_DETAIL_TESSELLATION,
} eTessellationMethod;

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
// Shaders
stx_Effect*                 g_pPOMFX = 0;
stx_Effect*                 g_pParticleFX = 0;
stx_Effect*                 g_pDetailTessellationFX = 0;
ShaderID                    g_pBumpMapPS = 0;
ShaderID                   g_pNoTessellationVS = 0;

// Textures
#define NUM_TEXTURES 7
DETAIL_TESSELLATION_TEXTURE_STRUCT DetailTessellationTextures[NUM_TEXTURES + 1] =
{
//    DiffuseMap              NormalHeightMap                 DisplayName,    fHeightScale fBaseTextureRepeat fDensityScale fMeaningfulDifference
    { "Textures/rocks.jpg",    "Textures/rocks_NM_height.dds",  "Rocks",       10.0f,       1.0f,              25.0f,        2.0f/255.0f },
    { "Textures/stones.bmp",   "Textures/stones_NM_height.dds", "Stones",      5.0f,        1.0f,              10.0f,        5.0f/255.0f },
    { "Textures/wall.jpg",     "Textures/wall_NM_height.dds",   "Wall",        8.0f,        1.0f,              7.0f,         7.0f/255.0f },
    { "Textures/wood.jpg",     "Textures/four_NM_height.dds",   "Four shapes", 30.0f,       1.0f,              35.0f,        2.0f/255.0f },
    { "Textures/concrete.bmp", "Textures/bump_NM_height.dds",   "Bump",        10.0f,       4.0f,              50.0f,        2.0f/255.0f },
    { "Textures/concrete.bmp", "Textures/dent_NM_height.dds",   "Dent",        10.0f,       4.0f,              50.0f,        2.0f/255.0f },
    { "Textures/wood.jpg",     "Textures/saint_NM_height.dds",  "Saints" ,     20.0f,       1.0f,              40.0f,        2.0f/255.0f },
    { "",                    "",                            "Custom" ,     5.0f,        1.0f,              10.0f,        2.0f/255.0f },
};
__DWORD__                               g_dwNumTextures = 0;
TextureID            g_pDetailTessellationBaseTextureRV[NUM_TEXTURES+1];
TextureID            g_pDetailTessellationHeightTextureRV[NUM_TEXTURES+1];
TextureID            g_pDetailTessellationDensityTextureRV[NUM_TEXTURES+1];
TextureID            g_pLightTextureRV = 0;
char                               g_pszCustomTextureDiffuseFilename[MAX_PATH] = "";
char                               g_pszCustomTextureNormalHeightFilename[MAX_PATH] = "";
__DWORD__                               g_bRecreateCustomTextureDensityMap  = false;

// Geometry
VertexBufferID                        g_pGridTangentSpaceVB = 0;
VertexBufferID                        g_pGridTangentSpaceIB = 0;
VertexBufferID                        g_pMainCB = 0;
VertexBufferID                        g_pMaterialCB = 0;
VertexFormatID                   g_pTangentSpaceVertexLayout = 0;
VertexFormatID                   g_pPositionOnlyVertexLayout = 0;
VertexBufferID                        g_pGridDensityVB[NUM_TEXTURES+1];
TextureID            g_pGridDensityVBSRV[NUM_TEXTURES+1];
VertexBufferID                        g_pParticleVB = 0;

// States
RasterizerStateID               g_pRasterizerStateSolid = 0;
RasterizerStateID               g_pRasterizerStateWireframe = 0;
SamplerStateID                  g_pSamplerStateLinear = 0;
BlendStateID                    g_pBlendStateNoBlend = 0;
BlendStateID                    g_pBlendStateAdditiveBlending = 0;
DepthStateID             g_pDepthStencilState = 0;

// Camera and light parameters
D3DXFROMWINEVECTOR3                         g_vecEye(76.099495f, 43.191154f, -110.108971f);
D3DXFROMWINEVECTOR3                         g_vecAt (75.590889f, 42.676582f, -109.418678f);
D3DXFROMWINEVECTOR4                         g_LightPosition(100.0f, 30.0f, -50.0f, 1.0f);
D3DXFROMWINEVECTOR3                g_LightControl; 

// Render settings
int                                 g_nRenderHUD = 2;
__DWORD__                               g_dwGridTessellation = 50;
bool                                g_bEnableWireFrame = false;
float                               g_fTessellationFactorEdges = 7.0f;
float                               g_fTessellationFactorInside = 7.0f;
int                                 g_nTessellationMethod = TESSELLATIONMETHOD_DETAIL_TESSELLATION;
int                                 g_nCurrentTexture = 0;
bool                                g_bFrameBasedAnimation = FALSE;
bool                                g_bAnimatedCamera = FALSE;
bool                                g_bDisplacementEnabled = TRUE;
D3DXFROMWINEVECTOR3                         g_vDebugColorMultiply(1.0, 1.0, 1.0);
D3DXFROMWINEVECTOR3                         g_vDebugColorAdd(0.0, 0.0, 0.0);
bool                                g_bDensityBasedDetailTessellation = FALSE;
bool                                g_bDistanceAdaptiveDetailTessellation = FALSE;
bool                                g_bDetailTessellationShadersNeedReloading = FALSE;
bool                                g_bShowFPS = TRUE;
bool                                g_bDrawLightSource = TRUE;

// Frame buffer readback ( 0 means never dump to disk (frame counter starts at 1) )
__DWORD__                               g_dwFrameNumberToDump = 0; 

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN                    1
#define IDC_TOGGLEREF                           3
#define IDC_CHANGEDEVICE                        4
#define IDC_STATIC_TECHNIQUE                   10
#define IDC_RADIOBUTTON_DISABLED               11
#define IDC_RADIOBUTTON_POM                    12
#define IDC_RADIOBUTTON_DETAILTESSELLATION     13
#define IDC_STATIC_TEXTURE                     14
#define IDC_COMBOBOX_TEXTURE                   15
#define IDC_CHECKBOX_WIREFRAME                 16
#define IDC_CHECKBOX_DISPLACEMENT              18
#define IDC_CHECKBOX_DENSITYBASED              19    
#define IDC_CHECKBOX_DISTANCEADAPTIVE          20
#define IDC_STATIC_TESS_FACTOR_EDGES           21
#define IDC_SLIDER_TESS_FACTOR_EDGES           22
#define IDC_STATIC_TESS_FACTOR_INSIDE          23
#define IDC_SLIDER_TESS_FACTOR_INSIDE          24
#define IDC_CHECKBOX_LINK_TESS_FACTORS         25
#define IDC_CHECKBOX_ROTATING_CAMERA           26


struct App
{
	App(){}
	virtual ~App(){}
	D3DXFROMWINEVECTOR3 g_EyePt;
	D3DXFROMWINEMATRIX g_Proj;
	D3DXFROMWINEMATRIX g_View;
//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
	D3DXFROMWINEVECTOR3 vUp(0.0f, 1.0f, 0.0f);
    // Setup the camera's view parameters
    D3DXFROMWINEMatrixLookAtLH(&g_View, &g_vecEye, &g_vecAt, &vUp);

    D3DXFROMWINEVECTOR3 vEyePt, vEyeTo;
    vEyePt.x = 76.099495f;
    vEyePt.y = 43.191154f;
    vEyePt.z = -110.108971f;

    vEyeTo.x = 75.590889f;
    vEyeTo.y = 42.676582f;
    vEyeTo.z = -109.418678f; 

    D3DXFROMWINEMatrixLookAtLH(&g_View, &vEyePt, &vEyeTo, &vUp);
    // Initialize the light control
    D3DXFROMWINEVECTOR3 vLightDirection;
    vLightDirection = -(D3DXFROMWINEVECTOR3)g_LightPosition;
    D3DXFROMWINEVec3Normalize( &vLightDirection, &vLightDirection );
    g_LightControl=vLightDirection;

    // Load default scene
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
int  OnCreateDevice()
{
    int hr;

    //
    // Compile shaders
    //

    // POM shaders
    g_pPOMFX=IRenderer::GetRendererInstance()->addEffectFromFile("/DetailTessellation/POM.fx");

    // Detail tessellation shaders
    char def[256];
    stx_snprintf(def, 256, "#define DENSITY_BASED_TESSELLATION %d\n#define DISTANCE_ADAPTIVE_TESSELLATION %d\n", (g_bDensityBasedDetailTessellation ? 1 : 0), (g_bDistanceAdaptiveDetailTessellation ? 1 : 0)); 
    g_pDetailTessellationFX=IRenderer::GetRendererInstance()->addEffectFromFile("/DetailTessellation/DetailTessellation11.fx", def);

    // Particle rendering shader
    g_pParticleFX=IRenderer::GetRendererInstance()->addEffectFromFile("/DetailTessellation/Particle.fx");

    //
    // Create vertex layouts
    //
    
    // Tangent space vertex input layout
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
		0, TYPE_TANGENT,  FORMAT_FLOAT, 3
	};
	g_pTangentSpaceVertexLayout = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}

    // Position-only vertex input layout
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3
	};
	g_pPositionOnlyVertexLayout = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}

    // Create grid geometry
    FillGrid_Indexed_WithTangentSpace( g_dwGridTessellation, g_dwGridTessellation, GRID_SIZE, GRID_SIZE, 
                                       &g_pGridTangentSpaceVB, &g_pGridTangentSpaceIB );

    // Create particle VB
    g_pParticleVB = IRenderer::GetRendererInstance()->addVertexBuffer(0,0,0); // ???

    //
    // Load textures
    //
    
    // Determine how many textures to load
    g_dwNumTextures = NUM_TEXTURES;

    // Is a custom texture specified?
    if ( ( stx_strlen( DetailTessellationTextures[NUM_TEXTURES].DiffuseMap )!=0 ) && 
         ( stx_strlen( DetailTessellationTextures[NUM_TEXTURES].NormalHeightMap )!=0 ) )
    {
        // Yes, add one to number of textures and update array
        g_dwNumTextures += 1;
    }

    // Loop through all textures and load them
    char str[256];
    for ( int i=0; i<(int)g_dwNumTextures; ++i )
    {
        // Load detail tessellation base texture
        g_pDetailTessellationBaseTextureRV[i] = IRenderer::GetRendererInstance()->addImageLibTexture(DetailTessellationTextures[i].DiffuseMap, false);

        // Load detail tessellation normal+height texture
        g_pDetailTessellationHeightTextureRV[i] = IRenderer::GetRendererInstance()->addImageLibTexture(DetailTessellationTextures[i].NormalHeightMap, false);

        // Compute density map filename
        char pszDensityMapFilename[256];
        char* pExtensionPointer;

        // Copy normal_height filename
        stx_snprintf(pszDensityMapFilename, 256, DetailTessellationTextures[i].NormalHeightMap );
        pExtensionPointer = stx_strrchr(pszDensityMapFilename, '.');
        stx_snprintf(pExtensionPointer, 256, pExtensionPointer-pszDensityMapFilename, "_density.dds"); // ???
        
        {
            // Density file for this texture doesn't exist, create it

            // Get description of source texture
            TextureID  pHeightMap;
            TextureID  pDensityMap;
            g_pDetailTessellationHeightTextureRV[i]->GetResource( (stx_Resource* *)&pHeightMap );
            
            // Create density map from height map
            CreateDensityMapFromHeightMap( pHeightMap, 
                                           DetailTessellationTextures[i].fDensityScale, &pDensityMap, 0, 
                                           DetailTessellationTextures[i].fMeaningfulDifference );

            // Save density map to file
            D3DXFROMWINE11SaveTextureToFile( pDensityMap, D3DXFROMWINE11_IFF_DDS, pszDensityMapFilename );
        }

        // Create density vertex stream for grid

        // Create STAGING versions of VB and IB
        VertexBufferID  pGridVBSTAGING = 0;
        CreateStagingBufferFromBuffer( g_pGridTangentSpaceVB, &pGridVBSTAGING );
        
        VertexBufferID  pGridIBSTAGING = 0;
        CreateStagingBufferFromBuffer( g_pGridTangentSpaceIB, &pGridIBSTAGING );

        TextureID  pDensityMap = 0;
        g_pDetailTessellationDensityTextureRV[i]->GetResource( (stx_Resource* *)&pDensityMap );

        // Map those buffers for reading
        
        
        IRenderer::GetRendererInstance()->Map( pGridVBSTAGING, 0, D3D11_MAP_READ, 0, &VBRead );
        IRenderer::GetRendererInstance()->Map( pGridIBSTAGING, 0, D3D11_MAP_READ, 0, &IBRead );

        // Set up a pointer to texture coordinates
        D3DXFROMWINEVECTOR2* pTexcoord = (D3DXFROMWINEVECTOR2 *)( &( (float *)VBRead.pData )[3] );

        // Create vertex buffer containing edge density data
        CreateEdgeDensityVertexStream( pTexcoord, sizeof(TANGENTSPACEVERTEX), 
                                       IBRead.pData, sizeof(__WORD__), 3*2*g_dwGridTessellation*g_dwGridTessellation, 
                                       pDensityMap, &g_pGridDensityVB[i], DetailTessellationTextures[i].fBaseTextureRepeat );

        pDensityMap->Release();

        IRenderer::GetRendererInstance()->Unmap( pGridIBSTAGING, 0 );
        IRenderer::GetRendererInstance()->Unmap( pGridVBSTAGING, 0 );

        ////////////SAFE_RELEASE( pGridIBSTAGING );
        ////////////SAFE_RELEASE( pGridVBSTAGING );

        // Create SRV for density vertex buffer
    }

    // Load light texture
    g_pLightTextureRV="media/Particle.dds";

    
    //
    // Create state objects
    //

    // Create solid and wireframe rasterizer state objects
    
    ZeroMemory( &RasterDesc, sizeof( 
    RasterDesc.FillMode = D3D11_FILL_SOLID;
    RasterDesc.CullMode = D3D11_CULL_BACK;
    RasterDesc.DepthClipEnable = TRUE;
    ( IRenderer::GetRendererInstance()->CreateRasterizerState( &RasterDesc, &g_pRasterizerStateSolid ) );

    RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    ( IRenderer::GetRendererInstance()->CreateRasterizerState( &RasterDesc, &g_pRasterizerStateWireframe ) );

    // Create sampler state for heightmap and normal map
    
    ZeroMemory( &SSDesc, sizeof( 
    SSDesc.Filter =         D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SSDesc.AddressU =       D3D11_TEXTURE_ADDRESS_WRAP;
    SSDesc.AddressV =       D3D11_TEXTURE_ADDRESS_WRAP;
    SSDesc.AddressW =       D3D11_TEXTURE_ADDRESS_WRAP;
    SSDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SSDesc.MaxAnisotropy =  16;
    SSDesc.MinLOD =         0;
    SSDesc.MaxLOD =         D3D11_FLOAT32_MAX;
    ( IRenderer::GetRendererInstance()->CreateSamplerState( &SSDesc, &g_pSamplerStateLinear) );

    // Create a blend state to disable alpha blending
    
    ZeroMemory(&BlendState, sizeof(
    BlendState.IndependentBlendEnable =                 FALSE;
    BlendState.RenderTarget[0].BlendEnable =            FALSE;
    BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = IRenderer::GetRendererInstance()->CreateBlendState( &BlendState, &g_pBlendStateNoBlend );
    BlendState.RenderTarget[0].BlendEnable =            TRUE;
    BlendState.RenderTarget[0].BlendOp =                D3D11_BLEND_OP_ADD;
    BlendState.RenderTarget[0].SrcBlend =               D3D11_BLEND_ONE;
    BlendState.RenderTarget[0].DestBlend =              D3D11_BLEND_ONE;
    BlendState.RenderTarget[0].RenderTargetWriteMask =  D3D11_COLOR_WRITE_ENABLE_ALL;
    BlendState.RenderTarget[0].BlendOpAlpha =           D3D11_BLEND_OP_ADD;
    BlendState.RenderTarget[0].SrcBlendAlpha =          D3D11_BLEND_ZERO;
    BlendState.RenderTarget[0].DestBlendAlpha =         D3D11_BLEND_ZERO;
    hr = IRenderer::GetRendererInstance()->CreateBlendState(&BlendState, &g_pBlendStateAdditiveBlending);
    
    // Create a depthstencil state
    
    DSDesc.DepthEnable =        TRUE;
    DSDesc.DepthFunc =          D3D11_COMPARISON_LESS_EQUAL;
    DSDesc.DepthWriteMask =     D3D11_DEPTH_WRITE_MASK_ALL;
    DSDesc.StencilEnable =      FALSE;
    hr = IRenderer::GetRendererInstance()->CreateDepthStencilState( &DSDesc, &g_pDepthStencilState );

    return 0;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
int  OnResizedSwapChain()
{
    int hr;

    // Setup the camera's projection parameters
    float fAspectRatio = IRenderer::GetRendererInstance()->GetAspectRatio();
    D3DXFROMWINEMatrixPerspectiveFovLH(&g_Proj, D3DXFROMWINE_PI/4, fAspectRatio, 2.0f, 100000.0f );

    return 0;
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void  OnFrameRender(double fTime, float fElapsedTime)
{
    int                     hr;
    static __DWORD__                s_dwFrameNumber = 1;
    VertexBufferID                pBuffers[2];
    TextureID    pSRV[4];
    SamplerStateID          pSS[1];
    D3DXFROMWINEVECTOR3                 vFrom;

    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if( g_D3DSettingsDlg.IsActive() )
    {
        g_D3DSettingsDlg.OnRender( fElapsedTime );
        return;
    }

    // Check if detail tessellation shaders need reloading
    if ( g_bDetailTessellationShadersNeedReloading )
    {
        // Yes, releasing existing detail tessellation shaders
        ////////////SAFE_RELEASE(g_pBumpMapPS);
        ////////////SAFE_RELEASE(g_pDetailTessellationDS);
        ////////////SAFE_RELEASE(g_pDetailTessellationHS);
        ////////////SAFE_RELEASE(g_pDetailTessellationVS);
        ////////////SAFE_RELEASE(g_pNoTessellationVS);

        // ... and reload them
        D3D10_SHADER_MACRO    pDetailTessellationDefines[] = 
            { "DENSITY_BASED_TESSELLATION", g_bDensityBasedDetailTessellation ? "1" : "0", 
              "DISTANCE_ADAPTIVE_TESSELLATION", g_bDistanceAdaptiveDetailTessellation ? "1" : "0", 
              0, 0 };
        char wcPath[256];
        stx_snprintf( wcPath, 256, "DetailTessellation11.hlsl" );
        CreateVertexShaderFromFile( wcPath, pDetailTessellationDefines, 0, 
                "VS_NoTessellation", "vs_5_0", 0, 0, 0, &g_pNoTessellationVS);
        CreateVertexShaderFromFile( wcPath, pDetailTessellationDefines, 0, 
                "VS","vs_5_0", 0, 0, 0, &g_pDetailTessellationVS);
        CreateHullShaderFromFile(   wcPath, pDetailTessellationDefines, 0, 
                "HS", "hs_5_0", 0, 0, 0, &g_pDetailTessellationHS);
        CreateDomainShaderFromFile( wcPath, pDetailTessellationDefines, 0, 
                "DS", "ds_5_0", 0, 0, 0, &g_pDetailTessellationDS);
        CreatePixelShaderFromFile(  wcPath, pDetailTessellationDefines, 0, 
                "BumpMapPS", "ps_5_0", 0, 0, 0, &g_pBumpMapPS);
        
        g_bDetailTessellationShadersNeedReloading = false;
    }

    // Projection matrix
    D3DXFROMWINEMATRIX mProj = *g_Proj;

    // View matrix
    D3DXFROMWINEMATRIX mView;
    if ( g_bAnimatedCamera )
    {
        float fRPS = 0.1f;
        float fAngleRAD;
        if ( g_bFrameBasedAnimation )
        {
            static float s_fTick = 0.0f;
            s_fTick += 1.0f/60.0f;
            fTime = s_fTick;
        }
        fAngleRAD = (float)( fRPS * fTime * 2.0f * D3DXFROMWINE_PI );
        float fRadius = (3.0f/4.0f) * GRID_SIZE;
        vFrom.x = fRadius * cos( fAngleRAD );
        vFrom.y = 80.0f;
        vFrom.z = fRadius * sin( fAngleRAD );
        const D3DXFROMWINEVECTOR3 vAt( 0, 0, 0 );
        const D3DXFROMWINEVECTOR3 vUp(0, 1, 0);
        D3DXFROMWINEMatrixLookAtLH( &mView, &vFrom, &vAt, &vUp );
    }
    else
    {
        vFrom = g_EyePt;
        mView = g_View;
    }

    // World matrix: identity
    D3DXFROMWINEMATRIX mWorld;
    D3DXFROMWINEMatrixIdentity( &mWorld );

    // Update combined matrices
    D3DXFROMWINEMATRIX mWorldViewProjection = mWorld * mView * mProj;    
    D3DXFROMWINEMATRIX mViewProjection = mView * mProj;    
    D3DXFROMWINEMATRIX mInvView;
    D3DXFROMWINEMatrixInverse( &mInvView, 0, &mView );

    // Transpose matrices before passing to shader stages
    D3DXFROMWINEMatrixTranspose( &mProj, &mProj );
    D3DXFROMWINEMatrixTranspose( &mView, &mView );
    D3DXFROMWINEMatrixTranspose( &mWorld, &mWorld );                    
    D3DXFROMWINEMatrixTranspose( &mWorldViewProjection, &mWorldViewProjection ); 
    D3DXFROMWINEMatrixTranspose( &mViewProjection, &mViewProjection );
    D3DXFROMWINEMatrixTranspose( &mInvView, &mInvView );

    // Update light position from light direction control
    g_LightPosition.x =  -(GRID_SIZE/2.0f) * g_LightControl.GetLightDirection().x;
    g_LightPosition.y =  -(GRID_SIZE/2.0f) * g_LightControl.GetLightDirection().y;
    g_LightPosition.z =  -(GRID_SIZE/2.0f) * g_LightControl.GetLightDirection().z;

    // Update main constant buffer
    D3DXFROMWINEVECTOR4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );
    
    hr = IRenderer::GetRendererInstance()->Map( g_pMainCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource );
    ((CONSTANT_BUFFER_STRUCT *)MappedSubResource.pData)->mWorld =               mWorld;
    ((CONSTANT_BUFFER_STRUCT *)MappedSubResource.pData)->mView =                mView;
    ((CONSTANT_BUFFER_STRUCT *)MappedSubResource.pData)->mProjection =          mProj;
    ((CONSTANT_BUFFER_STRUCT *)MappedSubResource.pData)->mWorldViewProjection = mWorldViewProjection;
    ((CONSTANT_BUFFER_STRUCT *)MappedSubResource.pData)->mViewProjection =      mViewProjection;
    ((CONSTANT_BUFFER_STRUCT *)MappedSubResource.pData)->mInvView =             mInvView;
    ((CONSTANT_BUFFER_STRUCT *)MappedSubResource.pData)->vMeshColor =           vWhite;
    // Min tessellation factor is half the selected edge tessellation factor
    ((CONSTANT_BUFFER_STRUCT *)MappedSubResource.pData)->vTessellationFactor =             
        D3DXFROMWINEVECTOR4( g_fTessellationFactorEdges, g_fTessellationFactorInside, g_fTessellationFactorEdges / 2.0f, 0.0f );  
    ((CONSTANT_BUFFER_STRUCT *)MappedSubResource.pData)->vDetailTessellationHeightScale =  
        g_bDisplacementEnabled ? D3DXFROMWINEVECTOR4( DetailTessellationTextures[g_nCurrentTexture].fHeightScale, 0.0f, 0.0f, 0.0f ) : vWhite;
    ((CONSTANT_BUFFER_STRUCT *)MappedSubResource.pData)->vGridSize =                       
        D3DXFROMWINEVECTOR4( GRID_SIZE, GRID_SIZE, 1.0f / GRID_SIZE, 1.0f / GRID_SIZE );
    ((CONSTANT_BUFFER_STRUCT *)MappedSubResource.pData)->vDebugColorMultiply=   D3DXFROMWINEVECTOR4( g_vDebugColorMultiply, 1.0f );
    ((CONSTANT_BUFFER_STRUCT *)MappedSubResource.pData)->vDebugColorAdd =       D3DXFROMWINEVECTOR4( g_vDebugColorAdd, 0.0f );
    IRenderer::GetRendererInstance()->Unmap( g_pMainCB, 0 );

    // Update material constant buffer
    D3DXFROMWINEVECTOR4 g_colorMtrlAmbient( 0.35f, 0.35f, 0.35f, 0 );
    D3DXFROMWINEVECTOR4 g_colorMtrlDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
    D3DXFROMWINEVECTOR4 g_colorMtrlSpecular( 1.0f, 1.0f, 1.0f, 1.0f );
    float       g_fSpecularExponent( 60.0f );
    hr = IRenderer::GetRendererInstance()->Map( g_pMaterialCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource );
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_materialAmbientColor =    g_colorMtrlAmbient;
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_materialDiffuseColor =    g_colorMtrlDiffuse;
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_materialSpecularColor =   g_colorMtrlSpecular;
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_fSpecularExponent =       D3DXFROMWINEVECTOR4( g_fSpecularExponent, 0.0f, 0.0f, 0.0f );
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_LightPosition =           g_LightPosition;
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_LightDiffuse =            vWhite;
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_LightAmbient =            vWhite;
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_vEye =                    D3DXFROMWINEVECTOR4( vFrom, 0.0f );
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_fBaseTextureRepeat =      
        D3DXFROMWINEVECTOR4( DetailTessellationTextures[g_nCurrentTexture].fBaseTextureRepeat, 0.0f, 0.0f, 0.0f );
    // POM height scale is in texcoord per world space unit thus: (heightscale * basetexturerepeat) / (texture size in world space units)
    float fPomHeightScale = 
        ( DetailTessellationTextures[g_nCurrentTexture].fHeightScale*DetailTessellationTextures[g_nCurrentTexture].fBaseTextureRepeat ) / GRID_SIZE;
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_fPOMHeightMapScale =      D3DXFROMWINEVECTOR4(fPomHeightScale , 0.0f, 0.0f, 0.0f );
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_fShadowSoftening =        D3DXFROMWINEVECTOR4( 0.58f, 0.0f, 0.0f, 0.0f );
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_nMinSamples =             8;
    ((MATERIAL_CB_STRUCT *)MappedSubResource.pData)->g_nMaxSamples =             50;
    IRenderer::GetRendererInstance()->Unmap(g_pMaterialCB, 0);

    // Bind the constant buffers to the device for all stages
    pBuffers[0] = g_pMainCB;
    pBuffers[1] = g_pMaterialCB;
    IRenderer::GetRendererInstance()->VSSetConstantBuffers( 0, 2, pBuffers );
    IRenderer::GetRendererInstance()->GSSetConstantBuffers( 0, 2, pBuffers );
    IRenderer::GetRendererInstance()->HSSetConstantBuffers( 0, 2, pBuffers );
    IRenderer::GetRendererInstance()->DSSetConstantBuffers( 0, 2, pBuffers );
    IRenderer::GetRendererInstance()->PSSetConstantBuffers( 0, 2, pBuffers );

    // Set sampler states
    pSS[0] = g_pSamplerStateLinear;
    IRenderer::GetRendererInstance()->VSSetSamplers(0, 1, pSS);
    IRenderer::GetRendererInstance()->HSSetSamplers(0, 1, pSS);
    IRenderer::GetRendererInstance()->DSSetSamplers(0, 1, pSS);
    IRenderer::GetRendererInstance()->PSSetSamplers(0, 1, pSS);

    // Set states
    IRenderer::GetRendererInstance()->OMSetBlendState( g_pBlendStateNoBlend, 0, 0xffffffff );
    IRenderer::GetRendererInstance()->RSSetState( g_bEnableWireFrame ? g_pRasterizerStateWireframe : g_pRasterizerStateSolid );
    IRenderer::GetRendererInstance()->OMSetDepthStencilState( g_pDepthStencilState, 0 );

    // Clear the render target and depth stencil
    IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.05f, 0.05f, 0.05f, 1.0f));

    // Set vertex buffer
    unsigned int stride = sizeof(TANGENTSPACEVERTEX);
    unsigned int offset = 0;
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1, &g_pGridTangentSpaceVB, &stride, &offset );

    // Set index buffer
    IRenderer::GetRendererInstance()->setIndexBuffer( g_pGridTangentSpaceIB, FORMAT_R16_UINT, 0 );

    // Set input layout
    IRenderer::GetRendererInstance()->setVertexFormat( g_pTangentSpaceVertexLayout );

    // Set shaders and geometries
    switch (g_nTessellationMethod)
    {
        case TESSELLATIONMETHOD_DISABLED:
        {
            // Render grid with simple bump mapping applied            
            IRenderer::GetRendererInstance()->VSSetShader( g_pNoTessellationVS, 0, 0 );
            IRenderer::GetRendererInstance()->HSSetShader( 0, 0, 0);
            IRenderer::GetRendererInstance()->DSSetShader( 0, 0, 0);
            IRenderer::GetRendererInstance()->GSSetShader( 0, 0, 0 );
            IRenderer::GetRendererInstance()->PSSetShader( g_pBumpMapPS, 0, 0 ); 

            // Set primitive topology
            IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLES );

            // Set shader resources
            pSRV[0] = g_pDetailTessellationBaseTextureRV[g_nCurrentTexture];
            pSRV[1] = g_pDetailTessellationHeightTextureRV[g_nCurrentTexture];
            IRenderer::GetRendererInstance()->VSSetShaderResources( 0, 2, pSRV );
            IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 2, pSRV );
        }
        break;

        case TESSELLATIONMETHOD_DISABLED_POM:
        {
            // Render grid with POM applied                
            IRenderer::GetRendererInstance()->VSSetShader( g_pPOMVS, 0, 0 );
            IRenderer::GetRendererInstance()->HSSetShader( 0, 0, 0);
            IRenderer::GetRendererInstance()->DSSetShader( 0, 0, 0);
            IRenderer::GetRendererInstance()->GSSetShader( 0, 0, 0 );
            IRenderer::GetRendererInstance()->PSSetShader( g_pPOMPS, 0, 0 ); 

            // Set primitive topology
            IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLES );

            // Set shader resources
            pSRV[0] = g_pDetailTessellationBaseTextureRV[g_nCurrentTexture];
            pSRV[1] = g_pDetailTessellationHeightTextureRV[g_nCurrentTexture];
            IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 2, pSRV );
        }
        break;

        case TESSELLATIONMETHOD_DETAIL_TESSELLATION:
        {
            // Render grid with detail tessellation
            IRenderer::GetRendererInstance()->VSSetShader( g_pDetailTessellationVS, 0, 0 );
            IRenderer::GetRendererInstance()->HSSetShader( g_pDetailTessellationHS, 0, 0);
            IRenderer::GetRendererInstance()->DSSetShader( g_pDetailTessellationDS, 0, 0);
            IRenderer::GetRendererInstance()->GSSetShader( 0, 0, 0 );
            IRenderer::GetRendererInstance()->PSSetShader( g_pBumpMapPS, 0, 0 ); 

            // Set primitive topology
            IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_3_CONTROL_POINT_PATCHLIST );

            // Set shader resources
            pSRV[0] = g_pDetailTessellationBaseTextureRV[g_nCurrentTexture];
            pSRV[1] = g_pDetailTessellationHeightTextureRV[g_nCurrentTexture];
            pSRV[2] = g_pDetailTessellationDensityTextureRV[g_nCurrentTexture];
            pSRV[3] = g_pGridDensityVBSRV[g_nCurrentTexture];
            IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 3, pSRV );
            IRenderer::GetRendererInstance()->DSSetShaderResources( 0, 3, pSRV );
            IRenderer::GetRendererInstance()->VSSetShaderResources( 0, 4, pSRV );

            pSRV[0] = g_pGridDensityVBSRV[g_nCurrentTexture];
            IRenderer::GetRendererInstance()->HSSetShaderResources( 0, 1, pSRV );
        }
        break;
    }

    // Draw grid
    IRenderer::GetRendererInstance()->DrawIndexed( 3*2*g_dwGridTessellation*g_dwGridTessellation, 0, 0 );

    // Draw light source if requested
    if ( g_bDrawLightSource )
    {
        // Set shaders
        IRenderer::GetRendererInstance()->VSSetShader( g_pParticleVS, 0, 0 );
        IRenderer::GetRendererInstance()->HSSetShader( 0, 0, 0);
        IRenderer::GetRendererInstance()->DSSetShader( 0, 0, 0);
        IRenderer::GetRendererInstance()->GSSetShader( g_pParticleGS, 0, 0 );
        IRenderer::GetRendererInstance()->PSSetShader( g_pParticlePS, 0, 0 ); 

        // Set primitive topology
        IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_POINTS );

        // Set shader resources
        pSRV[0] = g_pLightTextureRV;
        IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 1, pSRV );

        // Store new light position into particle's VB
        
        IRenderer::GetRendererInstance()->Map( g_pParticleVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
        *(D3DXFROMWINEVECTOR3*)MappedSubresource.pData = (D3DXFROMWINEVECTOR3)g_LightPosition;
        IRenderer::GetRendererInstance()->Unmap( g_pParticleVB, 0 );

        // Set vertex buffer
        unsigned int stride = sizeof(D3DXFROMWINEVECTOR3);
        unsigned int offset = 0;
        IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1, &g_pParticleVB, &stride, &offset );

        // Set input layout
        IRenderer::GetRendererInstance()->setVertexFormat( g_pPositionOnlyVertexLayout );

        // Additive blending
        IRenderer::GetRendererInstance()->OMSetBlendState( g_pBlendStateAdditiveBlending, 0, 0xffffffff );

        // Solid rendering (not affected by global wireframe toggle)
        IRenderer::GetRendererInstance()->RSSetState( g_pRasterizerStateSolid );

        // Draw light
        IRenderer::GetRendererInstance()->Draw( 1, 0 );
    }

    // Check if current frame needs to be dumped to disk
    if ( s_dwFrameNumber == g_dwFrameNumberToDump )
    {
        // Retrieve resource for current render target
        stx_Resource* pRTResource;
        DXUTGetD3D11RenderTargetView()->GetResource( &pRTResource );

        // Retrieve a Texture2D interface from resource
        TextureID  RTTexture;
        hr = pRTResource->QueryInterface( __uuidof( TextureID ), ( LPVOID* )&RTTexture );

        // Check if RT is multisampled or not
        SURFACE_DESC TexDesc;
        RTTexture->GetDesc( &TexDesc );
        if ( TexDesc.SampleDesc.Count > 1 )
        {
            // RT is multisampled: need resolving before dumping to disk

            // Create single-sample RT of the same type and dimensions
            DXGI_SAMPLE_DESC SingleSample = { 1, 0 };
            TexDesc.CPUAccessFlags =    D3D11_CPU_ACCESS_READ;
            TexDesc.MipLevels =         1;
            TexDesc.Usage =             USAGE_DEFAULT;
            TexDesc.CPUAccessFlags =    0;
            TexDesc.BindFlags =         0;
            TexDesc.SampleDesc =        SingleSample;

            // Create single-sample texture
            TextureID pSingleSampleTexture=-1;
	{
		//These have to be set to have a render target
		Image3 img;
		img.create(FORMAT_, , , 1, 1);
		pSingleSampleTexture=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear()); 
	}

            // Resolve multisampled render target into single-sample texture
            IRenderer::GetRendererInstance()->ResolveSubresource( pSingleSampleTexture, 0, RTTexture, 0, TexDesc.Format );

            // Save texture to disk
            hr = D3DXFROMWINE11SaveTextureToFile( pSingleSampleTexture, D3DXFROMWINE11_IFF_BMP, "RTOutput.bmp" );

            // Release single-sample version of texture
            ////////////SAFE_RELEASE(pSingleSampleTexture);
            
        }
        else
        {
            // Single sample case
            hr = D3DXFROMWINE11SaveTextureToFile( RTTexture, D3DXFROMWINE11_IFF_BMP, "RTOutput.bmp" );
        }

        // Release texture and resource
        ////////////SAFE_RELEASE(RTTexture);
        ////////////SAFE_RELEASE(pRTResource);
    }

    // Increase frame number
    s_dwFrameNumber++;
}

//--------------------------------------------------------------------------------------
// Create a density texture map from a height map
//--------------------------------------------------------------------------------------
void CreateDensityMapFromHeightMap( TextureID  pHeightMap, 
                                    float fDensityScale, TextureID  ppDensityMap, TextureID  ppDensityMapStaging, 
                                    float fMeaningfulDifference )
{
#define ReadPixelFromMappedSubResource(x, y)       ( *( (RGBQUAD *)((BYTE *)MappedSubResourceRead.pData + (y)*MappedSubResourceRead.RowPitch + (x)*sizeof(__DWORD__)) ) )
#define WritePixelToMappedSubResource(x, y, value) ( ( *( (__DWORD__ *)((BYTE *)MappedSubResourceWrite.pData + (y)*MappedSubResourceWrite.RowPitch + (x)*sizeof(__DWORD__)) ) ) = (__DWORD__)(value) )

    // Get description of source texture
    FORMAT f=IRenderer::GetRendererInstance()->GetTextureFormat(pHeightMap);
    int w=IRenderer::GetRendererInstance()->GetTextureWidth(pHeightMap);
    int h=IRenderer::GetRendererInstance()->GetTextureHeight(pHeightMap);

    // Create density map with the same properties
    Image3 img;
    img.create(f, w, h, 1, 1);
    *ppDensityMap=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());

    // Create STAGING height map texture
    TextureID pHeightMapStaging=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());

    // Create STAGING density map
    TextureID  pDensityMapStaging=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());
#if 0 // ???
    // Copy contents of height map into staging version
    IRenderer::GetRendererInstance()->CopyResource( pHeightMapStaging, pHeightMap );

    // Compute density map and store into staging version
    
    IRenderer::GetRendererInstance()->Map( pHeightMapStaging, 0, D3D11_MAP_READ, 0, &MappedSubResourceRead );

    IRenderer::GetRendererInstance()->Map( pDensityMapStaging, 0, D3D11_MAP_WRITE, 0, &MappedSubResourceWrite );

    // Loop map and compute derivatives
    for ( int j=0; j <= (int)Desc.Height-1; ++j )
    {
        for ( int i=0; i <= (int)Desc.Width-1; ++i )
        {
            // Edges are set to the minimum value
            if ( ( j == 0 ) || 
                 ( i == 0 ) || 
                 ( j == ( (int)Desc.Height-1 ) ) || 
                 ( i == ((int)Desc.Width-1 ) ) )
            {
                WritePixelToMappedSubResource( i, j, (__DWORD__)1 );
                continue;
            }

            // Get current pixel
            RGBQUAD CurrentPixel =     ReadPixelFromMappedSubResource( i, j );

            // Get left pixel
            RGBQUAD LeftPixel =        ReadPixelFromMappedSubResource( i-1, j );

            // Get right pixel
            RGBQUAD RightPixel =       ReadPixelFromMappedSubResource( i+1, j );

            // Get top pixel
            RGBQUAD TopPixel =         ReadPixelFromMappedSubResource( i, j-1 );

            // Get bottom pixel
            RGBQUAD BottomPixel =      ReadPixelFromMappedSubResource( i, j+1 );

            // Get top-right pixel
            RGBQUAD TopRightPixel =    ReadPixelFromMappedSubResource( i+1, j-1 );

            // Get bottom-right pixel
            RGBQUAD BottomRightPixel = ReadPixelFromMappedSubResource( i+1, j+1 );
            
            // Get top-left pixel
            RGBQUAD TopLeftPixel =     ReadPixelFromMappedSubResource( i-1, j-1 );
            
            // Get bottom-left pixel
            RGBQUAD BottomLeft =       ReadPixelFromMappedSubResource( i-1, j+1 );

            float fCurrentPixelHeight =            ( CurrentPixel.rgbReserved     / 255.0f );
            float fCurrentPixelLeftHeight =        ( LeftPixel.rgbReserved        / 255.0f );
            float fCurrentPixelRightHeight =       ( RightPixel.rgbReserved       / 255.0f );
            float fCurrentPixelTopHeight =         ( TopPixel.rgbReserved         / 255.0f );
            float fCurrentPixelBottomHeight =      ( BottomPixel.rgbReserved      / 255.0f );
            float fCurrentPixelTopRightHeight =    ( TopRightPixel.rgbReserved    / 255.0f );
            float fCurrentPixelBottomRightHeight = ( BottomRightPixel.rgbReserved / 255.0f );
            float fCurrentPixelTopLeftHeight =     ( TopLeftPixel.rgbReserved     / 255.0f );
            float fCurrentPixelBottomLeftHeight =  ( BottomLeft.rgbReserved       / 255.0f );

            float fDensity = 0.0f;
            float fHorizontalVariation = fabs( ( fCurrentPixelRightHeight - fCurrentPixelHeight ) - 
                                               ( fCurrentPixelHeight - fCurrentPixelLeftHeight ) );
            float fVerticalVariation = fabs( ( fCurrentPixelBottomHeight - fCurrentPixelHeight ) - 
                                             ( fCurrentPixelHeight - fCurrentPixelTopHeight ) );
            float fFirstDiagonalVariation = fabs( ( fCurrentPixelTopRightHeight - fCurrentPixelHeight ) - 
                                                  ( fCurrentPixelHeight - fCurrentPixelBottomLeftHeight ) );
            float fSecondDiagonalVariation = fabs( ( fCurrentPixelBottomRightHeight - fCurrentPixelHeight ) - 
                                                   ( fCurrentPixelHeight - fCurrentPixelTopLeftHeight ) );
            if ( fHorizontalVariation     >= fMeaningfulDifference) fDensity += 0.293f * fHorizontalVariation;
            if ( fVerticalVariation       >= fMeaningfulDifference) fDensity += 0.293f * fVerticalVariation;
            if ( fFirstDiagonalVariation  >= fMeaningfulDifference) fDensity += 0.207f * fFirstDiagonalVariation;
            if ( fSecondDiagonalVariation >= fMeaningfulDifference) fDensity += 0.207f * fSecondDiagonalVariation;
                
            // Scale density with supplied scale                
            fDensity *= fDensityScale;

            // Clamp density
            fDensity = max( min( fDensity, 1.0f ), 1.0f/255.0f );

            // Write density into density map
            WritePixelToMappedSubResource( i, j, (__DWORD__)( fDensity * 255.0f ) );
        }
    }

    IRenderer::GetRendererInstance()->Unmap( pDensityMapStaging, 0 );

    IRenderer::GetRendererInstance()->Unmap( pHeightMapStaging, 0 );
    ////////////SAFE_RELEASE( pHeightMapStaging );

    // Copy contents of density map into DEFAULT density version
    IRenderer::GetRendererInstance()->CopyResource( *ppDensityMap, pDensityMapStaging );
    
    // If a pointer to a staging density map was provided then return it with staging density map
    if ( ppDensityMapStaging )
    {
        *ppDensityMapStaging = pDensityMapStaging;
    }
    else
    {
        ////////////SAFE_RELEASE( pDensityMapStaging );
    }
#endif
}

#if 0
//--------------------------------------------------------------------------------------
// Sample a 32-bit texture at the specified texture coordinate (point sampling only)
//--------------------------------------------------------------------------------------
__inline RGBQUAD SampleTexture2D_32bit( __DWORD__ *pTexture2D, __DWORD__ dwWidth, __DWORD__ dwHeight, D3DXFROMWINEVECTOR2 fTexcoord, __DWORD__ dwStride )
{
#define FROUND(x)    ( (int)( (x) + 0.5 ) )

    // Convert texture coordinates to texel coordinates using round-to-nearest
    int nU = FROUND( fTexcoord.x * ( dwWidth-1 ) );
    int nV = FROUND( fTexcoord.y * ( dwHeight-1 ) );

    // Clamp texcoord between 0 and [width-1, height-1]
    nU = nU % dwWidth;
    nV = nV % dwHeight;

    // Return value at this texel coordinate
    return *(RGBQUAD *)( ( (BYTE *)pTexture2D) + ( nV*dwStride ) + ( nU*sizeof(__DWORD__) ) );
}


//--------------------------------------------------------------------------------------
// Sample density map along specified edge and return maximum value
//--------------------------------------------------------------------------------------
float GetMaximumDensityAlongEdge( __DWORD__ *pTexture2D, __DWORD__ dwStride, __DWORD__ dwWidth, __DWORD__ dwHeight, D3DXFROMWINEVECTOR2 fTexcoord0, D3DXFROMWINEVECTOR2 fTexcoord1 )
{
#define GETTEXEL(x, y)       ( *(RGBQUAD *)( ( (BYTE *)pTexture2D ) + ( (y)*dwStride ) + ( (x)*sizeof(__DWORD__) ) ) )
#define LERP(x, y, a)        ( (x)*(1.0f-(a)) + (y)*(a) )

    // Convert texture coordinates to texel coordinates using round-to-nearest
    int nU0 = FROUND( fTexcoord0.x * ( dwWidth  - 1 )  );
    int nV0 = FROUND( fTexcoord0.y * ( dwHeight - 1 ) );
    int nU1 = FROUND( fTexcoord1.x * ( dwWidth  - 1 )  );
    int nV1 = FROUND( fTexcoord1.y * ( dwHeight - 1 ) );

    // Wrap texture coordinates
    nU0 = nU0 % dwWidth;
    nV0 = nV0 % dwHeight;
    nU1 = nU1 % dwWidth;
    nV1 = nV1 % dwHeight;

    // Find how many texels on edge
    int nNumTexelsOnEdge = max( abs( nU1 - nU0 ), abs( nV1 - nV0 ) ) + 1;

    float fU, fV;
    float fMaxDensity = 0.0f;
    for ( int i=0; i<nNumTexelsOnEdge; ++i )
    {
        // Calculate current texel coordinates
        fU = LERP( (float)nU0, (float)nU1, ( (float)i ) / ( nNumTexelsOnEdge - 1 ) );
        fV = LERP( (float)nV0, (float)nV1, ( (float)i ) / ( nNumTexelsOnEdge - 1 ) );

        // Round texel texture coordinates to nearest
        int nCurrentU = FROUND( fU );
        int nCurrentV = FROUND( fV );

        // Update max density along edge
        fMaxDensity = max( fMaxDensity, GETTEXEL( nCurrentU, nCurrentV ).rgbBlue / 255.0f );
    }
        
    return fMaxDensity;
}


//--------------------------------------------------------------------------------------
// Calculate the maximum density along a triangle edge and
// store it in the resulting vertex stream.
//--------------------------------------------------------------------------------------
void CreateEdgeDensityVertexStream( D3DXFROMWINEVECTOR2* pTexcoord, 
                                    __DWORD__ dwVertexStride, void *pIndex, __DWORD__ dwIndexSize, __DWORD__ dwNumIndices, 
                                    TextureID  pDensityMap, VertexBufferID  ppEdgeDensityVertexStream,
                                    float fBaseTextureRepeat )
{
    int                hr;
    
    TextureID        pDensityMapReadFrom;
    __DWORD__                  dwNumTriangles = dwNumIndices / 3;

    // Create host memory buffer
    D3DXFROMWINEVECTOR4 *pEdgeDensityBuffer = new D3DXFROMWINEVECTOR4[dwNumTriangles];

    // Retrieve density map description
    SURFACE_DESC Tex2DDesc;
    pDensityMap->GetDesc( &Tex2DDesc );
    pDensityMap->Release();

    // Check if provided texture can be Mapped() for reading directly
    BOOL bCanBeRead = Tex2DDesc.CPUAccessFlags & D3D11_CPU_ACCESS_READ;
    if ( !bCanBeRead )
    {
        // Texture cannot be read directly, need to create STAGING texture and copy contents into it
        Tex2DDesc.CPUAccessFlags =   D3D11_CPU_ACCESS_READ;
        Tex2DDesc.Usage =            D3D11_USAGE_STAGING;
        Tex2DDesc.BindFlags =        0;
	{
		Image3 img;
		img.create(FORMAT_, , , 1, 1);
		pDensityMapReadFrom=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear()); 
	}

        // Copy contents of height map into staging version
        IRenderer::GetRendererInstance()->CopyResource( pDensityMapReadFrom, pDensityMap );
    }
    else
    {
        pDensityMapReadFrom = pDensityMap;
    }

    // Map density map for reading
    
    IRenderer::GetRendererInstance()->Map( pDensityMapReadFrom, 0, D3D11_MAP_READ, 0, &MappedSubResource );

    // Loop through all triangles
    for ( __DWORD__ i=0; i<dwNumTriangles; ++i )
    {
        __DWORD__ wIndex0, wIndex1, wIndex2;

        // Retrieve indices of current triangle
        if ( dwIndexSize == sizeof(__WORD__) )
        {
            wIndex0 = (__DWORD__)( (__WORD__ *)pIndex )[3*i+0];
            wIndex1 = (__DWORD__)( (__WORD__ *)pIndex )[3*i+1];
            wIndex2 = (__DWORD__)( (__WORD__ *)pIndex )[3*i+2];
        }
        else
        {
            wIndex0 = ( (__DWORD__ *)pIndex )[3*i+0];
            wIndex1 = ( (__DWORD__ *)pIndex )[3*i+1];
            wIndex2 = ( (__DWORD__ *)pIndex )[3*i+2];
        }

        // Retrieve texture coordinates of each vertex making up current triangle
        D3DXFROMWINEVECTOR2    vTexcoord0 = *(D3DXFROMWINEVECTOR2 *)( (BYTE *)pTexcoord + wIndex0 * dwVertexStride );
        D3DXFROMWINEVECTOR2    vTexcoord1 = *(D3DXFROMWINEVECTOR2 *)( (BYTE *)pTexcoord + wIndex1 * dwVertexStride );
        D3DXFROMWINEVECTOR2    vTexcoord2 = *(D3DXFROMWINEVECTOR2 *)( (BYTE *)pTexcoord + wIndex2 * dwVertexStride );

        // Adjust texture coordinates with texture repeat
        vTexcoord0 *= fBaseTextureRepeat;
        vTexcoord1 *= fBaseTextureRepeat;
        vTexcoord2 *= fBaseTextureRepeat;

        // Sample density map at vertex location
        float fEdgeDensity0 = GetMaximumDensityAlongEdge( (__DWORD__ *)MappedSubResource.pData, MappedSubResource.RowPitch, 
                                                          Tex2DDesc.Width, Tex2DDesc.Height, vTexcoord0, vTexcoord1 );
        float fEdgeDensity1 = GetMaximumDensityAlongEdge( (__DWORD__ *)MappedSubResource.pData, MappedSubResource.RowPitch, 
                                                          Tex2DDesc.Width, Tex2DDesc.Height, vTexcoord1, vTexcoord2 );
        float fEdgeDensity2 = GetMaximumDensityAlongEdge( (__DWORD__ *)MappedSubResource.pData, MappedSubResource.RowPitch, 
                                                          Tex2DDesc.Width, Tex2DDesc.Height, vTexcoord2, vTexcoord0 );

        // Store edge density in x,y,z and store maximum density in .w
        pEdgeDensityBuffer[i] = D3DXFROMWINEVECTOR4( fEdgeDensity0, fEdgeDensity1, fEdgeDensity2, 
                                             max( max( fEdgeDensity0, fEdgeDensity1 ), fEdgeDensity2 ) );
    }

    // Unmap density map
    IRenderer::GetRendererInstance()->Unmap( pDensityMapReadFrom, 0 );

    // Release staging density map if we had to create it
    if ( !bCanBeRead )
    {
        ////////////SAFE_RELEASE( pDensityMapReadFrom );
    }
    
    // Set source buffer for initialization data
    InitData.pSysMem = (void *)pEdgeDensityBuffer;

    // Create density vertex stream buffer: 1 float per entry
	ppEdgeDensityVertexStream=IRenderer::GetRendererInstance()->addVertexBuffer(dwNumTriangles * sizeof( D3DXFROMWINEVECTOR4 ), STATIC, (const void *)pEdgeDensityBuffer);

    // Free host memory buffer
    delete [] pEdgeDensityBuffer;
}
#endif
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
	IRenderer* r=IRenderer::GetRendererInstance("DetailTesselation");
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


