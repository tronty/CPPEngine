/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
//----------------------------------------------------------------------------------
// File:   PerlinFire.cpp
// Author: Andrew Tatarinov
// Email:  sdkfeedback@nvidia.com
// 
// Copyright (c) 2007 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
// BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
//----------------------------------------------------------------------------------

#define MIN_SAMPLING_RATE   4
#define MAX_SAMPLING_RATE   64
D3DXFROMWINEMATRIX g_EyePt;
D3DXFROMWINEMATRIX g_Proj;
D3DXFROMWINEMATRIX g_View;
D3DXFROMWINEMATRIX g_World;

// Direct3D 10 resources
stx_Effect*  g_pEffect = 0;

// DX10 device and rendering related variables

VertexFormatID g_pGeometryVertexLayout   = -1;

// DX10 meshes

MeshRenderer2 g_OutsideWorldMesh;
MeshRenderer2* g_mMesh = 0;

// Shader variables and matrices

stx_Effect::stx_Variable*  g_pmCubeViewMatrixVariable = 0;
stx_Effect::stx_Variable*  g_pmCubeProjMatrixVariable = 0;
stx_Effect::stx_Variable*  g_pmWorldViewProj = 0;
stx_Effect::stx_Variable*  g_pvEyePos = 0;
stx_Effect::stx_Variable*  g_pvLightPos = 0;
stx_Effect::stx_Variable*  g_pfLightIntensity = 0;
stx_Effect::stx_Variable*  g_pfStepSize = 0;
stx_Effect::stx_Variable*  g_pfTime = 0;
stx_Effect::stx_Variable*  g_pfNoiseScale = 0;
stx_Effect::stx_Variable*  g_pfRoughness = 0;
stx_Effect::stx_Variable*  g_pfFrequencyWeights = 0;
stx_Effect::stx_Variable*  g_pbJitter = 0;
stx_Effect::stx_Variable*  g_piCubeMapFace = 0;

// Textures and related shader resource views

TextureID g_pDepthBuffer = -1;
TextureID g_pFireTexture = -1;
TextureID g_pNoiseTexture = -1;
TextureID g_pJitterTexture = -1;
TextureID g_pPermTexture = -1;

stx_Effect::stx_Variable* g_ptxScreenDepth=0;
stx_Effect::stx_Variable* g_ptxSceneTexture = 0;
stx_Effect::stx_Variable* g_ptxFireTex = 0;
stx_Effect::stx_Variable* g_ptxJitterTex = 0;
stx_Effect::stx_Variable* g_ptxPermTex = 0;

// Textures and views for shadow mapping

TextureID g_pCubeMapDepth = -1;
DepthStencilViewID g_pCubeMapDepthViewArray[6] = { 0, 0, 0, 0, 0, 0 };
TextureID g_pCubeMapTexture = -1;
stx_Effect::stx_Variable* g_pCubeMapTextureVariable = 0;

// Effect techniques
stx_Effect::stx_Technique*  g_pCurrentTechnique = 0;
stx_Effect::stx_Technique*  g_pPerlinFire3D = 0;
stx_Effect::stx_Technique*  g_pPerlinFire4D = 0;
stx_Effect::stx_Technique*  g_pPerlinFire3DFlow = 0;
stx_Effect::stx_Technique*  g_pGeometryTechnique = 0;
stx_Effect::stx_Technique*  g_pGeometryTechniqueAux = 0;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_STATIC                  -1
#define IDC_TOGGLEFULLSCREEN        1
#define IDC_TOGGLEREF               2
#define IDC_CHANGEDEVICE            3
#define IDC_RESET                   4

#define IDC_USEJITTERING            5
#define IDC_FIRETECHNIQUE           6
#define IDC_SAMPLINGRATE_STATIC     7
#define IDC_SAMPLINGRATE            8

#define IDC_NOISESCALE_STATIC       9
#define IDC_NOISESCALE              10
#define IDC_ROUGHNESS_STATIC        11
#define IDC_ROUGHNESS               12
#define IDC_SHAPESIZE_STATIC        13
#define IDC_SHAPESIZE               14

#define IDC_FREQUENCIES_STATIC      15
#define IDC_FREQUENCY1              16
#define IDC_FREQUENCY2              17
#define IDC_FREQUENCY3              18
#define IDC_FREQUENCY4              19
#define IDC_FREQUENCY5              20

// Fire parameters

#define DEFAULT_JITTER true
#define DEFAULT_SAMPLING_RATE 16
#define DEFAULT_SPEED 0.6f
#define DEFAULT_NOISE_SCALE 1.35f
#define DEFAULT_ROUGHNESS 3.20f
#define DEFAULT_SHAPE_SIZE 3.0f
#define DEFAULT_FREQUENCY1 1.0f
#define DEFAULT_FREQUENCY2 0.5f
#define DEFAULT_FREQUENCY3 0.25f
#define DEFAULT_FREQUENCY4 0.125f
#define DEFAULT_FREQUENCY5 0.0625f

bool g_bJitter = DEFAULT_JITTER;
int g_nSamplingRate = DEFAULT_SAMPLING_RATE;
float g_fSpeed = DEFAULT_SPEED;
float g_fNoiseScale = DEFAULT_NOISE_SCALE;
float g_fRoughness = DEFAULT_ROUGHNESS;
float g_fShapeSize = DEFAULT_SHAPE_SIZE;
float g_fFrequencyWeights [5] = { DEFAULT_FREQUENCY1,
                                  DEFAULT_FREQUENCY2, 
                                  DEFAULT_FREQUENCY3, 
                                  DEFAULT_FREQUENCY4, 
                                  DEFAULT_FREQUENCY5 };
int g_CubeMapSize = 800;

struct App
{
	App(){}
	virtual ~App(){}

bool LoadTexture2D(std::string fileName, TextureID*  tex, ShaderResourceViewID* texRV)
{
    *texRV=IRenderer::GetRendererInstance()->addImageLibTexture(fileName.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
	*tex = *texRV;
    return true;
}

// Prepare cube map texture array

int PrepareCubeMap()
{
    // Create cubic depth stencil texture.
    #if 0
    dstex.MipLevels = 1;
    dstex.ArraySize = 6;
    dstex.SampleDesc.Count = 1;
    dstex.SampleDesc.Quality = 0;
    dstex.Format = FORMAT_R24G8_TYPELESS;
    dstex.BindFlags = BIND_DEPTH_STENCIL | BIND_SHADER_RESOURCE;
    #else
    g_pCubeMapDepth=IRenderer::GetRendererInstance()->addTexture(g_CubeMapSize, g_CubeMapSize, 1, FORMAT_RGBA8, IRenderer::GetRendererInstance()->Getlinear /* Clamp */ ());
    #endif
    
    return true;
}

// Set matrices for cube mapping

void InitCubeMatrices( float * cubeCenter )
{
    D3DXFROMWINEVECTOR3 vLookDir;
    D3DXFROMWINEVECTOR3 vUpDir;
    D3DXFROMWINEMATRIX cubeViewMatrices[6];
    D3DXFROMWINEMATRIX cubeProjMatrix;

    vLookDir = D3DXFROMWINEVECTOR3( 1.0f, 0.0f, 0.0f ) + (* (D3DXFROMWINEVECTOR3 *) cubeCenter);
    vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXFROMWINEMatrixLookAtLH( &cubeViewMatrices[0], (D3DXFROMWINEVECTOR3 *) cubeCenter, &vLookDir, &vUpDir );

    vLookDir = D3DXFROMWINEVECTOR3( -1.0f, 0.0f, 0.0f ) + (* (D3DXFROMWINEVECTOR3 *) cubeCenter);
    vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXFROMWINEMatrixLookAtLH( &cubeViewMatrices[1], (D3DXFROMWINEVECTOR3 *) cubeCenter, &vLookDir, &vUpDir );

    vLookDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f,  0.0f ) + (* (D3DXFROMWINEVECTOR3 *) cubeCenter);
    vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, -1.0f );
    D3DXFROMWINEMatrixLookAtLH( &cubeViewMatrices[2], (D3DXFROMWINEVECTOR3 *) cubeCenter, &vLookDir, &vUpDir );

    vLookDir = D3DXFROMWINEVECTOR3( 0.0f, -1.0f, 0.0f ) + (* (D3DXFROMWINEVECTOR3 *) cubeCenter);
    vUpDir = D3DXFROMWINEVECTOR3( 0.0f,  0.0f, 1.0f );
    D3DXFROMWINEMatrixLookAtLH( &cubeViewMatrices[3], (D3DXFROMWINEVECTOR3 *) cubeCenter, &vLookDir, &vUpDir );

    vLookDir = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 1.0f ) + (* (D3DXFROMWINEVECTOR3 *) cubeCenter);
    vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXFROMWINEMatrixLookAtLH( &cubeViewMatrices[4], (D3DXFROMWINEVECTOR3 *) cubeCenter, &vLookDir, &vUpDir );

    vLookDir = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, -1.0f ) + (* (D3DXFROMWINEVECTOR3 *) cubeCenter);
    vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f,  0.0f );
    D3DXFROMWINEMatrixLookAtLH( &cubeViewMatrices[5], (D3DXFROMWINEVECTOR3 *) cubeCenter, &vLookDir, &vUpDir );

    D3DXFROMWINEMatrixPerspectiveFovLH( &cubeProjMatrix, (float)D3DXFROMWINE_PI * 0.5f, 1.0f, 0.2f, 200.0f );

    g_pmCubeViewMatrixVariable->SetMatrixArray( (float *)cubeViewMatrices, 0, 6 );
    g_pmCubeProjMatrixVariable->SetMatrix( cubeProjMatrix );
}

//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
int  OnCreateDevice()
{
    int hr;

    // Load effect from file
    g_pEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/PerlinFire/PerlinFire.fx");

    // Obtain techniques

    g_pPerlinFire3D = g_pEffect->GetTechniqueByName( "PerlinFire3D" );
    g_pPerlinFire3DFlow = g_pEffect->GetTechniqueByName( "PerlinFire3DFlow" );
    g_pPerlinFire4D = g_pEffect->GetTechniqueByName( "PerlinFire4D" );
    g_pGeometryTechnique = g_pEffect->GetTechniqueByName( "RenderGeometry" );
    g_pGeometryTechniqueAux = g_pEffect->GetTechniqueByName( "RenderGeometryAux" );
    g_pCurrentTechnique = g_pPerlinFire4D;

    // Obtain texture variables

    g_ptxScreenDepth = g_pEffect->GetVariableByName( "ScreenDepth" );
    g_ptxSceneTexture = g_pEffect->GetVariableByName( "SceneTexture" );
    g_ptxFireTex = g_pEffect->GetVariableByName( "FireShape" );
    g_ptxJitterTex = g_pEffect->GetVariableByName( "JitterTexture" );
    g_ptxPermTex = g_pEffect->GetVariableByName( "PermTexture" );
    g_pCubeMapTextureVariable = g_pEffect->GetVariableByName( "ShadowMap" );
    
    // Obtain miscellaneous variables

    g_pmCubeViewMatrixVariable = g_pEffect->GetVariableByName( "CubeViewMatrices" );
    g_pmCubeProjMatrixVariable = g_pEffect->GetVariableByName( "CubeProjectionMatrix" );
    g_pmWorldViewProj = g_pEffect->GetVariableByName( "WorldViewProj" );
    g_pvEyePos = g_pEffect->GetVariableByName( "EyePos" );
    g_pvLightPos = g_pEffect->GetVariableByName( "LightPos" );
    g_pfLightIntensity = g_pEffect->GetVariableByName( "LightIntensity" );
    g_pfStepSize = g_pEffect->GetVariableByName( "StepSize" );
    g_pfTime = g_pEffect->GetVariableByName( "Time" );
    g_pfNoiseScale = g_pEffect->GetVariableByName( "NoiseScale" );
    g_pfRoughness = g_pEffect->GetVariableByName( "Roughness" );
    g_pfFrequencyWeights = g_pEffect->GetVariableByName( "FrequencyWeights" );
    g_pbJitter = g_pEffect->GetVariableByName( "bJitter" );
    g_piCubeMapFace = g_pEffect->GetVariableByName( "CubeMapFace" );

    // Set input layouts
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	g_pGeometryVertexLayout = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}

#if 0
    g_OutsideWorldMesh=FW3::GetMesh(g_OutsideWorldMesh, "/Orcs/Bonfire_wOrcs.x", eSimpleShader, false);
#elif 1
    g_OutsideWorldMesh.Load("/Orcs/Bonfire_wOrcs.x", eSimpleShader, false);
#endif
    //g_mMesh->CreateTexturedBox(1.0f, 1.0f, 1.0f, eShaderNone);
    //g_mMesh->CreateBox(1.0f, 1.0f, 1.0f, eShaderNone);
    g_mMesh->CreateSphere(0.1f, eShaderNone);

    // Load textures
    //g_pFireTexture=IRenderer::GetRendererInstance()->addImageLibTexture("Firetex.dds, false, IRenderer::GetRendererInstance()->Getlinear());
    //g_pFireTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/flame.png", false, IRenderer::GetRendererInstance()->Getlinear());
    g_pFireTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/Smoke/fireBase.png", false, IRenderer::GetRendererInstance()->Getlinear());


    // Create noise texture
    // Fill the texture with random numbers from 0 to 256
    stx_srand( timeGetTime() );

    BYTE data[256 * 256];
    for (int i = 0; i < 256 * 256; i++)
    {
        data[i] = (unsigned char) (stx_rand () % 256);
    }

    g_pNoiseTexture=IRenderer::GetRendererInstance()->addTexture(256, 256, 1, FORMAT_R8, IRenderer::GetRendererInstance()->Getlinear());

    // Create the shader resource view for jittering
    g_pJitterTexture=IRenderer::GetRendererInstance()->addTexture(256, 256, 1, FORMAT_R8, IRenderer::GetRendererInstance()->Getlinear/*Aniso*/());

    // Create the shader resource view for permutation
    g_pPermTexture=IRenderer::GetRendererInstance()->addTexture(256, 256, 1, FORMAT_R8, IRenderer::GetRendererInstance()->Getlinear());

    // Setup camera
    D3DXFROMWINEVECTOR3 vEye(0.0f, 15.0f, -30.0f);
    D3DXFROMWINEVECTOR3 vAt(0, 7.0f, 0);
    D3DXFROMWINEVECTOR3 vUp(0, 0.0f, 0);
    D3DXFROMWINEMatrixLookAtLH(&g_View, &vEye, &vAt, &vUp);
    //g_Camera.SetModelCenter( D3DXFROMWINEVECTOR3(0, 7.0f, 0 ) );
    
    // Setup cubemap
    
    PrepareCubeMap(  );
    
    g_pCubeMapTextureVariable->SetResource( g_pCubeMapTexture );
    
    return 0;
}

//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
int  OnResizedSwapChain()
{
    int hr;

    // Setup the camera's projection parameters
    float aspectRatio=IRenderer::GetRendererInstance()->GetAspectRatio();
    D3DXFROMWINEMatrixPerspectiveFovLH(&g_Proj, D3DXFROMWINE_PI / 3, aspectRatio, 1.0f, 1000.0f);

    Image3 img;
    img.create(FORMAT_R32f, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight(), 1, 1);
    g_pDepthBuffer=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());

    g_ptxScreenDepth->SetResource( g_pDepthBuffer );

    return 0;
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void  OnFrameRender( double fTime, float fElapsedTime )
{
    // Clear render target and the depth stencil
    IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
    
    // Set matrices
    D3DXFROMWINEMATRIX mWorld = g_World;
    D3DXFROMWINEMATRIX mView = g_View;
    D3DXFROMWINEMATRIX mProj = g_Proj;

    D3DXFROMWINEMATRIX mViewProj = mView * mProj;
    D3DXFROMWINEMATRIX mWorldView = mWorld * mView;
    D3DXFROMWINEMATRIX mWorldViewProj = mWorldView * mProj;

    g_pmWorldViewProj -> SetMatrix ((float *) & mWorldViewProj );
    
    float rnd = (float)stx_rand() / RAND_MAX * 0.5f + 0.5f;
    float lightPos[] = { 0.25f * (rnd - 0.5f), 5.7f, 1.0f * (rnd - 0.5f), 1.0f };
    
    InitCubeMatrices( lightPos );
    
    g_pvLightPos->SetFloatVector( lightPos );

    // First, render an additional z-buffer for reads in the shader
    // Unfortunately DX10 doesn't allow to set a currently bound depth buffer
    // as a shader resource even if z-writes are explicitly disabled
    IRenderer::GetRendererInstance()->Clear(false, true, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
    IRenderer::GetRendererInstance()->changeRenderTarget(g_pDepthBuffer);

    g_pGeometryTechnique->GetPassByIndex(0)->Apply(0);
    //IRenderer::GetRendererInstance()->setVertexFormat( g_pGeometryVertexLayout );
    g_OutsideWorldMesh.Render(  );
    
    // Second, render the whole scene to a shadowmap
    // Set a new viewport for rendering to cube map

    D3DFROMWINEVIEWPORT9 SMVP, prevSMVP;
    unsigned int prevViewpotrNum = 1;
    
    SMVP.Height		= g_CubeMapSize;
    SMVP.Width		= g_CubeMapSize;
    SMVP.MinZ	= 0;
    SMVP.MaxZ	= 1;
    SMVP.X	= 0;
    SMVP.Y	= 0;

    prevSMVP=IRenderer::GetRendererInstance()->GetViewport();
    IRenderer::GetRendererInstance()->setViewport( &SMVP );
    
    //IRenderer::GetRendererInstance()->setVertexFormat( g_pGeometryVertexLayout );
    for( int i = 0; i < 6; i ++ )
    {
        g_piCubeMapFace->SetInt(i);

        IRenderer::GetRendererInstance()->changeRenderTarget(g_pCubeMapDepthViewArray[i]);
        float f=128.0f/256.0f;
        IRenderer::GetRendererInstance()->Clear(false, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
        
        g_pGeometryTechniqueAux->GetPassByIndex(0)->Apply(0);
        g_OutsideWorldMesh.Render(  );
    }

    // Switch back to main RTs

    IRenderer::GetRendererInstance()->setViewport( &prevSMVP );
    IRenderer::GetRendererInstance()->changeToMainFramebuffer();

    g_pfNoiseScale->SetFloat( g_fNoiseScale );
    g_pfRoughness->SetFloat( g_fRoughness );
    g_pfFrequencyWeights->SetFloatArray( g_fFrequencyWeights, 0, 5 );

    g_pfTime->SetFloat( (float)fTime * g_fSpeed );
    g_pfStepSize->SetFloat( (float)1.0f / g_nSamplingRate );
    g_pbJitter->SetBool( g_bJitter );
    
    g_pGeometryTechnique->GetPassByIndex(1)->Apply(0);
    //IRenderer::GetRendererInstance()->setVertexFormat( g_pGeometryVertexLayout );
    g_OutsideWorldMesh.Render(  );
    
    // Render fire volume

    D3DXFROMWINEMATRIX mTranslate, mScale, mWorldViewInv;
    D3DXFROMWINEMatrixTranslation( &mTranslate, 0, 0.5f, 0 );
    D3DXFROMWINEMatrixScaling( &mScale, 4.0f * g_fShapeSize, 8.0f * g_fShapeSize, 4.0f * g_fShapeSize);

    mWorldView = mTranslate * mScale * mWorld * mView;
    mWorldViewProj = mWorldView * mProj;

    D3DXFROMWINEMatrixInverse( &mWorldViewInv, 0, &mWorldView );

    D3DXFROMWINEVECTOR4 vEye;
    D3DXFROMWINEVECTOR4 v(0, 0, 0, 1);
    D3DXFROMWINEVec4Transform( &vEye, &v, &mWorldViewInv );

    g_pmWorldViewProj->SetMatrix ( (float *) & mWorldViewProj );
    g_pvEyePos->SetFloatVector ( (float *) &vEye );
    g_pfLightIntensity->SetFloat( rnd );

    g_pCurrentTechnique->GetPassByIndex(0)->Apply(0);
    g_mMesh->DrawSubset( 0 );
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
	IRenderer* r=IRenderer::GetRendererInstance("PerlinFire");
	//if(std::string(r->getString())!="D3D11") stx_exit(11);
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

