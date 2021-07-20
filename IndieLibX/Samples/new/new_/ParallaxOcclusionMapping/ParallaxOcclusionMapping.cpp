//=================================================================================================
// File: ParallaxOcclusionMapping.cpp
//
// Author: Natalya Tatarchuk
//         ATI Research, Inc.
//         3D Application Research Group  
//
// Implementation of the algorithm as described in "Dynamic Parallax Occlusion Mapping with 
// Approximate Soft Shadows" paper, by N. Tatarchuk, ATI Research, to appear in the proceedings 
// of ACM Symposium on Interactive 3D Graphics and Games, 2006.
//                 
// Copyright (c) ATI Research, Inc. All rights reserved.
//=================================================================================================
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
//include "resource.h"

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 
D3DXFROMWINEVECTOR3 g_LightControl;
D3DXFROMWINEMATRIX g_View;
D3DXFROMWINEMATRIX g_Proj;
D3DXFROMWINEMATRIX g_World;
D3DXFROMWINEVECTOR3 g_EyePt;

//--------------------------------------------------------------------------------------
// Global variables and parameters
//--------------------------------------------------------------------------------------
const int                   s_iMAX_STRING_SIZE = 100;
const int                   s_iDECL_SIZE = 56;
const int                   s_iNUM_TEXTURES = 7;  // Number of loaded texturs in the program

//--------------------------------------------------------------------------------------
// Texture pair ids for use for this sample
//--------------------------------------------------------------------------------------
enum POM_TEXTURE_IDS
{
    POM_STONES = 0,
    POM_ROCKS,
    POM_WALL,
    POM_FOUR_BUMPS,
    POM_BUMPS,
    POM_DENTS,
    POM_SAINT
};

//--------------------------------------------------------------------------------------
// Id numbers for different rendering techniques used
//--------------------------------------------------------------------------------------
enum TECHNIQUE_IDS
{
    POM,           // Parallax occlusion mapping
    BUMPMAP,       // Bump mapping
    PM             // Parallax mapping with offset limiting
};

int                         g_nCurrentTextureID = POM_STONES;
int                         g_nCurrentTechniqueID = POM;

//--------------------------------------------------------------------------------------
// Texture pair names for use for this sample
//--------------------------------------------------------------------------------------
char*                      g_strBaseTextureNames[] =
{
    "/new/ParallaxOcclusionMapping/stones.bmp",
    "/new/ParallaxOcclusionMapping/rocks.jpg",
    "/new/ParallaxOcclusionMapping/wall.jpg",
    "/new/ParallaxOcclusionMapping/wood.jpg",
    "/new/ParallaxOcclusionMapping/concrete.bmp",
    "/new/ParallaxOcclusionMapping/concrete.bmp",
    "/new/ParallaxOcclusionMapping/wood.jpg"
};

char*                      g_strNMHTextureNames[] =
{
    "/new/ParallaxOcclusionMapping/stones_NM_height.tga",
    "/new/ParallaxOcclusionMapping/rocks_NM_height.tga",
    "/new/ParallaxOcclusionMapping/wall_NM_height.tga",
    "/new/ParallaxOcclusionMapping/four_NM_height.tga",
    "/new/ParallaxOcclusionMapping/bump_NM_height.tga",
    "/new/ParallaxOcclusionMapping/dent_NM_height.tga",
    "/new/ParallaxOcclusionMapping/saint_NM_height.tga"
};


//--------------------------------------------------------------------------------------
bool                        g_bShowHelp = false;       // If true, it renders the UI control text
stx_CModelViewerCamera          g_Camera;                  // A model viewing camera
stx_Effect*                g_pEffect = 0;        // D3DXFROMWINE effect interface

MeshRenderer2                  g_pMesh;            // Mesh object

std::vector<TextureID>         g_pBaseTextures;    // VArray of base map texture surfaces
std::vector<TextureID>         g_pNMHTextures;    // VArray of normal / height map texture surfaces
//       a height map in the alpha channel

D3DXFROMWINEMATRIX               g_mWorldFix;
float                       g_fLightScale;             // Light brightness scale parameter


//--------------------------------------------------------------------------------------
// UI scalar parameters
//--------------------------------------------------------------------------------------
const float                 s_fLightScaleUIScale = 10.0f;
const int                   s_iLightScaleSliderMin = 0;
const int                   s_iLightScaleSliderMax = 20;

int                         g_iHeightScaleSliderMin = 0;
int                         g_iHeightScaleSliderMax = 100;
float                       g_fHeightScaleUIScale = 100.0f;

const int                   s_iSliderMin = 8;
const int                   s_iSliderMax = 130;

//--------------------------------------------------------------------------------------
// Material properties parameters:
//--------------------------------------------------------------------------------------
D3DXFROMWINEVECTOR4                   g_colorMtrlDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
D3DXFROMWINEVECTOR4                   g_colorMtrlAmbient( 0.35f, 0.35f, 0.35f, 0 );
D3DXFROMWINEVECTOR4                   g_colorMtrlSpecular( 1.0f, 1.0f, 1.0f, 1.0f );

float                       g_fSpecularExponent( 60.0f );        // Material's specular exponent
float                       g_fBaseTextureRepeat( 1.0f );        // The tiling factor for base and normal map textures

bool                        g_bVisualizeLOD( false );            // Toggles visualization of level of detail colors
bool                        g_bVisualizeMipLevel( false );       // Toggles visualization of mip level
bool                        g_bDisplayShadows( true );           // Toggles display of self-occlusion based shadows
bool                        g_bAddSpecular( true );              // Toggles rendering with specular or without
bool                        g_bRenderPOM( true );                // Toggles whether using normal mapping or parallax occlusion mapping

int                         g_nLODThreshold( 3 );                // The mip level id for transitioning between the full computation
// for parallax occlusion mapping and the bump mapping computation
int                         g_nMinSamples( 8 );                  // The minimum number of samples for sampling the height field profile
int                         g_nMaxSamples( 50 );                 // The maximum number of samples for sampling the height field profile
float                       g_fShadowSoftening( 0.58f );         // Blurring factor for the soft shadows computation
float                       g_fHeightScale;                    // This parameter controls the height map range for displacement mapping

//--------------------------------------------------------------------------------------
// Mesh file names for use
//--------------------------------------------------------------------------------------
char*                      g_strMeshFileName0 = "/new/ParallaxOcclusionMapping/Disc.x";


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN           1
#define IDC_TOGGLEREF                  3
#define IDC_CHANGEDEVICE               4
#define IDC_HEIGHT_SCALE_SLIDER        5
#define IDC_HEIGHT_SCALE_STATIC        6
#define IDC_TOGGLE_SHADOWS             7
#define IDC_SELECT_TEXTURES_COMBOBOX   8
#define IDC_TOGGLE_SPECULAR            9
#define IDC_SPECULAR_EXPONENT_SLIDER   10
#define IDC_SPECULAR_EXPONENT_STATIC   11
#define IDC_MIN_NUM_SAMPLES_SLIDER     12
#define IDC_MIN_NUM_SAMPLES_STATIC     13
#define IDC_MAX_NUM_SAMPLES_SLIDER     14
#define IDC_MAX_NUM_SAMPLES_STATIC     15
#define IDC_TECHNIQUE_COMBO_BOX        16
#define IDC_RELOAD_BUTTON              20

struct App
{
	App(){}
	virtual ~App(){}
//--------------------------------------------------------------------------------------
// Select a pair of base and normal map / height map textures to use and
// setup related height map range parameters, given a texture index
// Note: all texture surfaces in g_pBaseTextures and g_pNMHTextures MUST
// be created prior to calling this function.
//--------------------------------------------------------------------------------------
void SetPOMTextures( int iTextureIndex )
{
    g_nCurrentTextureID = iTextureIndex;

    // Bind the new active textures to the correct texture slots in the shaders:
    if( g_pEffect )
    {
        int hr;

        ( g_pEffect->SetTexture( "g_baseTexture", g_pBaseTextures[g_nCurrentTextureID] ) );
        ( g_pEffect->SetTexture( "g_nmhTexture", g_pNMHTextures[g_nCurrentTextureID] ) );

    }

    // Setup height map range slider parameters (need to be setup per-texture, as very height-map specific:
    switch( iTextureIndex )
    {
        case POM_STONES:
        {
            // Stones texture pair:
            g_iHeightScaleSliderMin = 0;
            g_iHeightScaleSliderMax = 10;
            g_fHeightScaleUIScale = 100.0f;
            g_fHeightScale = 0.02f;

            g_fSpecularExponent = 60.0f;
            g_fBaseTextureRepeat = 1.0f;

            g_nMinSamples = 8;
            g_nMaxSamples = 50;
            break;
        }

        case POM_ROCKS:
        {
            g_iHeightScaleSliderMin = 0;
            g_iHeightScaleSliderMax = 10;
            g_fHeightScaleUIScale = 100.0f;
            g_fHeightScale = 0.1f;

            g_fSpecularExponent = 100.0f;
            g_fBaseTextureRepeat = 1.0f;

            g_nMinSamples = 8;
            g_nMaxSamples = 100;
            break;
        }

        case POM_WALL:
        {
            g_iHeightScaleSliderMin = 0;
            g_iHeightScaleSliderMax = 10;
            g_fHeightScaleUIScale = 100.0f;
            g_fHeightScale = 0.06f;

            g_fSpecularExponent = 60.0f;
            g_fBaseTextureRepeat = 1.0f;

            g_nMinSamples = 8;
            g_nMaxSamples = 50;
            break;
        }

        case POM_FOUR_BUMPS:
        {
            g_iHeightScaleSliderMin = 0;
            g_iHeightScaleSliderMax = 10;
            g_fHeightScaleUIScale = 10.0f;
            g_fHeightScale = 0.2f;

            g_fSpecularExponent = 100.0f;
            g_fBaseTextureRepeat = 1.0f;
            g_nMinSamples = 12;
            g_nMaxSamples = 100;
            break;
        }

        case POM_BUMPS:
        {
            g_iHeightScaleSliderMin = 0;
            g_iHeightScaleSliderMax = 10;
            g_fHeightScaleUIScale = 10.0f;
            g_fHeightScale = 0.2f;

            g_fSpecularExponent = 100.0f;
            g_fBaseTextureRepeat = 4.0f;
            g_nMinSamples = 12;
            g_nMaxSamples = 100;
            break;
        }

        case POM_DENTS:
        {
            g_iHeightScaleSliderMin = 0;
            g_iHeightScaleSliderMax = 10;
            g_fHeightScaleUIScale = 10.0f;
            g_fHeightScale = 0.2f;

            g_fSpecularExponent = 100.0f;
            g_fBaseTextureRepeat = 4.0f;
            g_nMinSamples = 12;
            g_nMaxSamples = 100;
            break;
        }

        case POM_SAINT:
        {
            g_iHeightScaleSliderMin = 0;
            g_iHeightScaleSliderMax = 10;
            g_fHeightScaleUIScale = 100.0f;
            g_fHeightScale = 0.08f;

            g_fSpecularExponent = 100.0f;
            g_fBaseTextureRepeat = 1.0f;
            g_nMinSamples = 12;
            g_nMaxSamples = 100;
            break;
        }
    }
}


//--------------------------------------------------------------------------------------
// Initialize the app: initialize the light controls and the GUI 
// elements for this application.
//--------------------------------------------------------------------------------------
void InitApp()
{
    // UI elements parameters:
    const int ciPadding = 24;        // vertical padding between controls
    const int ciLeft = 5;         // Left align border (x-coordinate) of the controls
    const int ciWidth = 125;       // widget width
    const int ciHeight = 22;        // widget height

    // Initialize the light control:
    g_LightControl=D3DXFROMWINEVECTOR3( sinf( D3DXFROMWINE_PI * 2 - D3DXFROMWINE_PI / 6 ),
                                                   0,
                                                   -cosf( D3DXFROMWINE_PI * 2 - D3DXFROMWINE_PI / 6 ) );
    g_fLightScale = 1.0f;

    // Initialize POM textures and height map parameters:
    SetPOMTextures( POM_STONES );

    char sz[s_iMAX_STRING_SIZE];

    // Slider parameters:
    const int ciSliderLeft = 15;
    const int ciSliderWidth = 100;
}

//--------------------------------------------------------------------------------------
// Loads the effect file from disk.
//--------------------------------------------------------------------------------------
int LoadEffectFile()
{
    g_pEffect = IRenderer::GetRendererInstance()->addEffectFromFile("/new/ParallaxOcclusionMapping/ParallaxOcclusionMapping.fx");
    return 0;
}

//--------------------------------------------------------------------------------------
int  OnCreateDevice(   )
{
    int hr;

    // Load the mesh 
    g_pMesh.Load(g_strMeshFileName0, eSimpleShader, false, false);

    D3DXFROMWINEVECTOR3* pData;
    D3DXFROMWINEVECTOR3 vCenter;
    FLOAT fObjectRadius;
    vCenter=g_pMesh.GetMeshBBoxCenter(0);
    g_pMesh.GetBoundingSphereRadius(fObjectRadius);

    // Align the starting frame of the mesh to be slightly toward the viewer yet to 
    // see the grazing angles:                                                      
    D3DXFROMWINEMatrixTranslation( &g_mWorldFix, -vCenter.x, -vCenter.y, -vCenter.z );
    D3DXFROMWINEMATRIX mRotation;
    D3DXFROMWINEMatrixRotationY( &mRotation, -D3DXFROMWINE_PI / 3.0f );
    g_mWorldFix *= mRotation;
    D3DXFROMWINEMatrixRotationX( &mRotation, D3DXFROMWINE_PI / 3.0f );
    g_mWorldFix *= mRotation;

    ( LoadEffectFile() );

    // Load all textures used by the program from disk 
    char str[MAX_PATH];

    g_pBaseTextures.resize(s_iNUM_TEXTURES);
    g_pNMHTextures.resize(s_iNUM_TEXTURES);

    for( int iTextureIndex = POM_STONES; iTextureIndex < s_iNUM_TEXTURES; iTextureIndex++ )
    {
        // Load the pair of textures (base and combined normal map / height map texture) into 
        // memory for each type of POM texture pairs

        // Load the base mesh:
	g_pBaseTextures[iTextureIndex]=IRenderer::GetRendererInstance()->addImageLibTexture(g_strBaseTextureNames[iTextureIndex], false, IRenderer::GetRendererInstance()->Getlinear());

        // Load the normal map / height map texture
	g_pNMHTextures[iTextureIndex]=IRenderer::GetRendererInstance()->addImageLibTexture(g_strNMHTextureNames[iTextureIndex], false, IRenderer::GetRendererInstance()->Getlinear());
    }

    SetPOMTextures( g_nCurrentTextureID );

    // Setup the camera's view parameters 
    D3DXFROMWINEVECTOR3 vecEye( 0.0f, 0.0f, -15.0f );
    D3DXFROMWINEVECTOR3 vecAt ( 0.0f, 0.0f, -0.0f );
    D3DXFROMWINEVECTOR3 vecUp ( 0.0f, 1.0f, 0.0f );
    D3DXFROMWINEMatrixLookAtLH(&g_View, &vecEye, &vecAt, &vecUp );

    return 0;
}

//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
int  OnResetDevice()
{
    int hr;

    // Setup the camera's projection parameters
    float fAspectRatio = IRenderer::GetRendererInstance()->GetAspectRatio();

    D3DXFROMWINEMatrixPerspectiveFovLH(&g_Proj, D3DXFROMWINE_PI / 4, fAspectRatio, 0.1f, 5000.0f );

    const int ciHUDLeftBorderOffset = 170;
    const int ciHUDVerticalSize = 170;
    const int ciUILeftBorderOffset = 0;
    const int ciUITopBorderOffset = 40;
    const int ciUIVerticalSize = 600;
    const int ciUIHorizontalSize = 300;

    return 0;

}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void  OnFrameRender(  double fTime,
                             float fElapsedTime )
{
    int hr;
    D3DXFROMWINEMATRIX mWorldViewProjection;
    D3DXFROMWINEVECTOR3 vLightDir;
    D3DXFROMWINEVECTOR4 vLightDiffuse;
    unsigned int iPass, cPasses;

    D3DXFROMWINEMATRIX mWorld;
    D3DXFROMWINEMATRIX mView;
    D3DXFROMWINEMATRIX mProjection;

    // Clear the render target and the zbuffer 
    IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (0.0f, 0.25f, 0.25f, 0.55f));

    // Render the scene
    IRenderer::GetRendererInstance()->BeginScene();

    if( g_pEffect )
    {
        // Get the projection & view matrix from the camera class
        mWorld = g_mWorldFix * ( g_World );
        mProjection = ( g_Proj );
        mView = ( g_View );

        mWorldViewProjection = mWorld * mView * mProjection;

        // Get camera position:
        D3DXFROMWINEVECTOR4 vEye;
        D3DXFROMWINEVECTOR3 vTemp = ( g_EyePt );
        vEye.x = vTemp.x;
        vEye.y = vTemp.y;
        vEye.z = vTemp.z;
        vEye.w = 1.0;

        // Render the light arrow so the user can visually see the light dir
        D3DXFROMWINEVECTOR4 arrowColor = D3DXFROMWINEVECTOR4( 1, 1, 0, 1 );
        // ??? ( g_LightControl.OnRender9( arrowColor, &mView, &mProjection, g_EyePt ) );
        vLightDir = g_LightControl;
        vLightDiffuse = g_fLightScale * D3DXFROMWINEVECTOR4( 1, 1, 1, 1 );

        ( g_pEffect->SetValue( "g_LightDir", &vLightDir, sizeof( D3DXFROMWINEVECTOR3 ) ) );
        ( g_pEffect->SetValue( "g_LightDiffuse", &vLightDiffuse, sizeof( D3DXFROMWINEVECTOR4 ) ) );

        // Update the effect's variables.  Instead of using strings, it would 
        // be more efficient to cache a handle to the parameter by calling 
        // ID3DXEffect::GetVariableByName
        ( g_pEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProjection ) );
        ( g_pEffect->SetMatrix( "g_mWorld", &mWorld ) );
        ( g_pEffect->SetMatrix( "g_mView", &mView ) );
        ( g_pEffect->SetVector( "g_vEye", &vEye ) );
        ( g_pEffect->SetValue( "g_fHeightMapScale", &g_fHeightScale, sizeof( float ) ) );

        D3DXFROMWINEVECTOR4 vWhite = D3DXFROMWINEVECTOR4( 1, 1, 1, 1 );
        ( g_pEffect->SetValue( "g_materialDiffuseColor", &vWhite, sizeof( D3DXFROMWINEVECTOR4 ) ) );

        ( g_pEffect->SetValue( "g_materialAmbientColor", &g_colorMtrlAmbient, sizeof( D3DXFROMWINEVECTOR4 ) ) );
        ( g_pEffect->SetValue( "g_materialDiffuseColor", &g_colorMtrlDiffuse, sizeof( D3DXFROMWINEVECTOR4 ) ) );
        ( g_pEffect->SetValue( "g_materialSpecularColor", &g_colorMtrlSpecular, sizeof( D3DXFROMWINEVECTOR4 ) ) );


        ( g_pEffect->SetValue( "g_fSpecularExponent", &g_fSpecularExponent, sizeof( float ) ) );
        ( g_pEffect->SetValue( "g_fBaseTextureRepeat", &g_fBaseTextureRepeat, sizeof( float ) ) );
        ( g_pEffect->SetValue( "g_nLODThreshold", &g_nLODThreshold, sizeof( int ) ) );
        ( g_pEffect->SetValue( "g_nMinSamples", &g_nMinSamples, sizeof( int ) ) );
        ( g_pEffect->SetValue( "g_nMaxSamples", &g_nMaxSamples, sizeof( int ) ) );
        ( g_pEffect->SetValue( "g_fShadowSoftening", &g_fShadowSoftening, sizeof( float ) ) );

        ( g_pEffect->SetBool( "g_bVisualizeLOD", ( BOOL )g_bVisualizeLOD ) );
        ( g_pEffect->SetBool( "g_bVisualizeMipLevel", ( BOOL )g_bVisualizeMipLevel ) );
        ( g_pEffect->SetBool( "g_bDisplayShadows", ( BOOL )g_bDisplayShadows ) );
        ( g_pEffect->SetBool( "g_bAddSpecular", ( BOOL )g_bAddSpecular ) );

        // Render the scene:
        switch( g_nCurrentTechniqueID )
        {
            case POM:
                ( g_pEffect->SetTechnique( "RenderSceneWithPOM" ) );
                break;

            case BUMPMAP:
                ( g_pEffect->SetTechnique( "RenderSceneWithBumpMap" ) );
                break;

            case PM:
                ( g_pEffect->SetTechnique( "RenderSceneWithPM" ) );
                break;
        }

        #if 1
        ( g_pEffect->Begin( &cPasses, 0 ) );
        for( iPass = 0; iPass < cPasses; iPass++ )
        {
            ( g_pEffect->BeginPass( iPass ) );
            ( g_pMesh.render(&mWorldViewProjection) );
            ( g_pEffect->EndPass() );
        }
        ( g_pEffect->End() );
        #else
        g_pMesh.Render(pTechnique, &mWorldViewProjection);
        #endif
    }

    ( IRenderer::GetRendererInstance()->EndScene() );
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
	IRenderer* r=IRenderer::GetRendererInstance("ParallaxOcclusionMapping");
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

