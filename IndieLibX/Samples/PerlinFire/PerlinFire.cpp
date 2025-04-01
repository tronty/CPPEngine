/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//----------------------------------------------------------------------------------
// File:   PerlinFire.cpp
// Author: Andrew Tatarinov
// Email:  sdkfeedback@nvidia.com
// 
// Copyright (c) 2007 NVIDIA_Corporation. All rights reserved.
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

#include <Framework3/IRenderer.h>

#include <GUI/GUIUtils.h>
#include <FW3.h>


#define MIN_SAMPLING_RATE   4
#define MAX_SAMPLING_RATE   64

//D3DXFROMWINEMATRIX g_Camera;
D3DXFROMWINEMATRIX mWorld;
D3DXFROMWINEMATRIX mView;
D3DXFROMWINEMATRIX mProj;

// Direct3D 10 resources
//ShaderID g_pEffect = -1;
//ID3D10StateBlock * g_pCurrentState = 0;  // stateblock for tracking state

// DX10 device and rendering related variables

VertexFormatID g_pGeometryVertexLayout   = -1;

// DX10 meshes

bool bOutsideWorldMesh=false;
MeshRenderer2 g_OutsideWorldMesh;
MeshRenderer2 g_pMesh;

// Shader variables and matrices

D3DXFROMWINEMATRIX g_pmCubeViewMatrixVariable[6];
D3DXFROMWINEMATRIX g_pmCubeProjMatrixVariable;
D3DXFROMWINEMATRIX g_pmWorldView;
D3DXFROMWINEMATRIX g_pmWorldViewProj;
D3DXFROMWINEMATRIX g_pmWorldViewInv;
D3DXFROMWINEVECTOR4 g_pvEyePos;
D3DXFROMWINEVECTOR4 g_pvLightPos;
float g_pfLightIntensity = 0;
float g_pfStepSize = 0;
float g_pfTime = 0;
float g_pfNoiseScale = 0;
float g_pfRoughness = 0;
float g_pfFrequencyWeights = 0;
float g_pbJitter = 0;
float g_piCubeMapFace = 0;

D3DXFROMWINEVECTOR3 g_pflightPos;
float rnd;

// Textures and related shader resource views

TextureID g_pDepthBuffer = -1;
//TextureID * g_pDepthBufferSRV = 0;
//TextureID * g_pDepthBufferDSV = 0;

TextureID g_pFireTexture = -1;

TextureID g_pNoiseTexture = -1;
TextureID g_pJitterTexture = -1;
TextureID g_pPermTexture = -1;

TextureID g_pScreenDepth = -1;
//TextureID g_pSceneTexture = -1;

// Textures and views for shadow mapping

TextureID g_pCubeMapDepth = -1;
//TextureID * g_pCubeMapDepthViewArray[6] = { 0, 0, 0, 0, 0, 0 };
//TextureID * g_pCubeMapTextureRV = 0;
TextureID g_pCubeMapTexture = -1;

// Effect techniques
ShaderID g_pCurrentTechnique = -1;
ShaderID g_pPerlinFire3D = -1;
ShaderID g_pPerlinFire4D = -1;
ShaderID g_pPerlinFire3DFlow = -1;
ShaderID g_pGeometryTechnique = -1;
ShaderID g_pGeometryTechniqueAux = -1;

VertexFormatID g_pCurrentLayout = -1;
VertexFormatID g_pPerlinFire3DLayout = -1;
VertexFormatID g_pPerlinFire4DLayout = -1;
VertexFormatID g_pPerlinFire3DFlowLayout = -1;
VertexFormatID g_pGeometryTechniqueLayout = -1;
VertexFormatID g_pGeometryTechniqueAuxLayout = -1;

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

const char* name[] =
{
"4Dsimplexnoise",
"3Dflownoise",
"3Dsimplexnoise",
0
};

class PerlinFire : public STXGUI {
public:
	PerlinFire()
	{
	}
	virtual ~PerlinFire()
	{
	}
virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
    GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
    if(radioButton->isPressed() && radioButton->isChecked())
    {
		if(callbackString == "4Dsimplexnoise")
      		{
			g_pCurrentTechnique = g_pPerlinFire4D;
			g_pCurrentLayout = g_pPerlinFire4DLayout;
		}
		else if(callbackString == "3Dflownoise")
      		{
			g_pCurrentTechnique = g_pPerlinFire3DFlow;
			g_pCurrentLayout = g_pPerlinFire3DFlowLayout;
		}
		else
		{
			g_pCurrentTechnique = g_pPerlinFire3D;
			g_pCurrentLayout = g_pPerlinFire3DLayout;
		}
    }
  }
}
// Prepare cube map texture array

int PrepareCubeMap()
{
    // Create cubic depth stencil texture.
    /*
    SURFACE_DESC dstex;

    dstex.Width = g_CubeMapSize;
    dstex.Height = g_CubeMapSize;
    dstex.MipLevels = 1;
    dstex.ArraySize = 6;
    dstex.SampleDesc.Count = 1;
    dstex.SampleDesc.Quality = 0;
    dstex.Format = DXGI_FORMAT_R24G8_TYPELESS;
    dstex.Usage = D3D10_USAGE_DEFAULT;
    dstex.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
    dstex.CPUAccessFlags = 0;
    dstex.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

    if( FAILED( IRenderer::GetRendererInstance()->CreateTexture2D( &dstex, 0, &g_pCubeMapDepth ) ) )
    {
        DXUTTRACE( "Failed to create depth stencil texture\n" );
        return false;
    }

    // Create the depth stencil view for the entire cube

    D3D10_DEPTH_STENCIL_VIEW_DESC DescDS;

    for( int i = 0; i < 6; i ++ )
    {
        DescDS.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        DescDS.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DARRAY;
        DescDS.Texture2DArray.FirstArraySlice = i;
        DescDS.Texture2DArray.ArraySize = (unsigned) 1;
        DescDS.Texture2DArray.MipSlice = 0;

        if( FAILED( IRenderer::GetRendererInstance()->CreateDepthStencilView( g_pCubeMapDepth, &DescDS, &(g_pCubeMapDepthViewArray[i]) ) ) )
        {
            DXUTTRACE( "Failed to create depth stencil view for a depth cube map\n" );
            return false;
        }
    }

    // Create the shader resource view for the shadow map

    D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;

    ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );

    SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
    SRVDesc.Texture2DArray.MipLevels = 1;
    SRVDesc.Texture2DArray.MostDetailedMip = 0;
    SRVDesc.Texture2DArray.FirstArraySlice = 0;
    SRVDesc.Texture2DArray.ArraySize = 6;

    if( FAILED( IRenderer::GetRendererInstance()->CreateShaderResourceView( g_pCubeMapDepth, &SRVDesc, &g_pCubeMapTextureRV ) ) )
    {
        DXUTTRACE( "Failed to create shader resource view for a depth stencil\n" );
        return false;
    }
    */
    g_pCubeMapDepth=IRenderer::GetRendererInstance()->addTexture(g_CubeMapSize, g_CubeMapSize, 1, FORMAT_RGBA8, IRenderer::GetRendererInstance()->Getlinear /* Clamp */ ());
    return true;
}

// Set matrices for cube mapping

void InitCubeMatrices( D3DXFROMWINEVECTOR3 cubeCenter )
{
    D3DXFROMWINEVECTOR3 vLookDir;
    D3DXFROMWINEVECTOR3 vUpDir;
    D3DXFROMWINEMATRIX cubeViewMatrices[6];
    D3DXFROMWINEMATRIX cubeProjMatrix;

    vLookDir = D3DXFROMWINEVECTOR3( 1.0f, 0.0f, 0.0f ) + cubeCenter;
    vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXFROMWINEMatrixLookAtLH( &cubeViewMatrices[0], &cubeCenter, &vLookDir, &vUpDir );

    vLookDir = D3DXFROMWINEVECTOR3( -1.0f, 0.0f, 0.0f ) + cubeCenter;
    vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXFROMWINEMatrixLookAtLH( &cubeViewMatrices[1], &cubeCenter, &vLookDir, &vUpDir );

    vLookDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f,  0.0f ) + cubeCenter;
    vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, -1.0f );
    D3DXFROMWINEMatrixLookAtLH( &cubeViewMatrices[2], &cubeCenter, &vLookDir, &vUpDir );

    vLookDir = D3DXFROMWINEVECTOR3( 0.0f, -1.0f, 0.0f ) + cubeCenter;
    vUpDir = D3DXFROMWINEVECTOR3( 0.0f,  0.0f, 1.0f );
    D3DXFROMWINEMatrixLookAtLH( &cubeViewMatrices[3], &cubeCenter, &vLookDir, &vUpDir );

    vLookDir = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 1.0f ) + cubeCenter;
    vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXFROMWINEMatrixLookAtLH( &cubeViewMatrices[4], &cubeCenter, &vLookDir, &vUpDir );

    vLookDir = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, -1.0f ) + cubeCenter;
    vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f,  0.0f );
    D3DXFROMWINEMatrixLookAtLH( &cubeViewMatrices[5], &cubeCenter, &vLookDir, &vUpDir );

    D3DXFROMWINEMatrixPerspectiveFovLH( &cubeProjMatrix, (float)D3DXFROMWINE_PI * 0.5f, 1.0f, 0.2f, 200.0f );

}

// Initialize the app
void InitApp()
{
    int hr;

    // Setup camera
    D3DXFROMWINEVECTOR3 vEye(0.0f, 15.0f, -30.0f);
    D3DXFROMWINEVECTOR3 vAt(0, 7.0f, 0);
    D3DXFROMWINEVECTOR3 vUp(0, 1.0f, 0);
#if 0
    LuaScript script;
		script.Load("/Orcs/Bonfire_wOrcs.lua");
    vEye.x = script.get<float>("PerlinFire.Eye.x");
    vEye.y = script.get<float>("PerlinFire.Eye.y");
    vEye.z = script.get<float>("PerlinFire.Eye.z");
    vAt.x = script.get<float>("PerlinFire.At.x");
    vAt.y = script.get<float>("PerlinFire.At.y");
    vAt.z = script.get<float>("PerlinFire.At.z");
    vUp.x = script.get<float>("PerlinFire.Up.x");
    vUp.y = script.get<float>("PerlinFire.Up.y");
    vUp.z = script.get<float>("PerlinFire.Up.z");
#elif 0
	TiXmlDocument* document;
	TiXmlHandle* docHandle;
	TiXmlHandle* BodyHandle;
	std::string fn=stx_convertpath("/Orcs/Bonfire_wOrcs.xml");
	//LOG_PRINT("fn.c_str()=%s\n", fn.c_str());

	TiXmlDocument document(fn.c_str());
	document.LoadFile();
	TiXmlHandle docHandle( &document );
	TiXmlHandle BodyHandle = docHandle.FirstChild("Body");
	TiXmlElement* BodyElement = BodyHandle.Element();
	if(BodyElement) {

	TiXmlElement* CameraElement = BodyHandle.FirstChild("Camera").Element();
   	AngleX=CameraElement->Attribute("aX");
    vEye.x=CameraElement->Attribute("Eyex");
    vEye.y=CameraElement->Attribute("Eyey");
    vEye.z=CameraElement->Attribute("Eyez");
    vAt.x=CameraElement->Attribute("Atx");
    vAt.y=CameraElement->Attribute("Aty");
    vAt.z=CameraElement->Attribute("Atz");
    vUp.x=CameraElement->Attribute("Upx");
    vUp.y=CameraElement->Attribute("Upy");
    vUp.z=CameraElement->Attribute("Upz");}
#endif
    D3DXFROMWINEMatrixLookAtLH(&mView, &vEye, &vAt, &vUp);
    //g_Camera.SetModelCenter( D3DXFROMWINEVECTOR3(0, 7.0f, 0 ) );

    // Setup the camera's projection parameters
    float aspectRatio = (IRenderer::GetRendererInstance()->GetViewportWidth()*1.0f)/IRenderer::GetRendererInstance()->GetViewportHeight();
    D3DXFROMWINEMatrixPerspectiveFovLH(&mProj, D3DXFROMWINE_PI / 3, aspectRatio, 1.0f, 1000.0f);

            g_bJitter = DEFAULT_JITTER;
            g_fSpeed = DEFAULT_SPEED;
            g_fNoiseScale = DEFAULT_NOISE_SCALE;
            g_fRoughness = DEFAULT_ROUGHNESS;
            g_fShapeSize = DEFAULT_SHAPE_SIZE;
            g_fFrequencyWeights [0] = DEFAULT_FREQUENCY1;
            g_fFrequencyWeights [1] = DEFAULT_FREQUENCY2;
            g_fFrequencyWeights [2] = DEFAULT_FREQUENCY3;
            g_fFrequencyWeights [3] = DEFAULT_FREQUENCY4;
            g_fFrequencyWeights [4] = DEFAULT_FREQUENCY5;

    // Obtain techniques
    //LOG_FNLN;
    g_pPerlinFire3D = IRenderer::GetRendererInstance()->addShaderFromFile("/Orcs/PerlinFire.fx", "PerlinFireVS", "PerlinFire3DPS" );
    //LOG_FNLN;
    g_pPerlinFire3DFlow = IRenderer::GetRendererInstance()->addShaderFromFile("/Orcs/PerlinFire.fx", "PerlinFireVS", "PerlinFire3DFlowPS" );
    //LOG_FNLN;
    g_pPerlinFire4D = IRenderer::GetRendererInstance()->addShaderFromFile("/Orcs/PerlinFire.fx", "PerlinFireVS", "PerlinFire4DPS" );
    // g_pGeometryTechnique = IRenderer::GetRendererInstance()->addShaderFromFile("/Orcs/PerlinFire.fx", "SimpleVS", "SimplePS" );
    // g_pGeometryTechniqueAux = IRenderer::GetRendererInstance()->addShaderFromFile("/Orcs/PerlinFire.fx", "ShadowVS", "" );
	//LOG_FNLN;
    
	FormatDesc format1[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_BINORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TANGENT,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	g_pPerlinFire3DLayout = IRenderer::GetRendererInstance()->addVertexFormat(format1, elementsOf(format1), g_pPerlinFire3D);
	g_pPerlinFire3DFlowLayout = IRenderer::GetRendererInstance()->addVertexFormat(format1, elementsOf(format1), g_pPerlinFire3DFlow);
	g_pPerlinFire4DLayout = IRenderer::GetRendererInstance()->addVertexFormat(format1, elementsOf(format1), g_pPerlinFire4D);
	#if 1
	//LOG_FNLN;
	//LOG_PRINT("g_pPerlinFire3D:%x\n", g_pPerlinFire3D);
	//LOG_PRINT("g_pPerlinFire3DLayout:%x\n", g_pPerlinFire3DLayout);
	//LOG_PRINT("g_pPerlinFire3DFlow:%x\n", g_pPerlinFire3DFlow);
	//LOG_PRINT("g_pPerlinFire3DFlowLayout:%x\n", g_pPerlinFire3DFlowLayout);
	//LOG_PRINT("g_pPerlinFire4D:%x\n", g_pPerlinFire4D);
	//LOG_PRINT("g_pPerlinFire4DLayout:%x\n", g_pPerlinFire4DLayout);
	#endif

/*

    // Populate combo-box
    CDXUTComboBox * pComboBox = g_SampleUI.GetComboBox( IDC_FIRETECHNIQUE );
    pComboBox->AddItem( "4D simplex noise", g_pPerlinFire4D );
    pComboBox->AddItem( "3D flow noise", g_pPerlinFire3DFlow );
    pComboBox->AddItem( "3D simplex noise", g_pPerlinFire3D );
*/
    g_pCurrentTechnique = g_pPerlinFire3D;
	g_pCurrentLayout = g_pPerlinFire3DLayout;
/*
    // Obtain texture variables

    g_pScreenDepth = g_pEffect->GetVariableByName( "ScreenDepth" )->AsShaderResource();
    g_pSceneTexture = g_pEffect->GetVariableByName( "SceneTexture" )->AsShaderResource();
    g_pFireTex = g_pEffect->GetVariableByName( "FireShape" )->AsShaderResource();
    g_pJitterTex = g_pEffect->GetVariableByName( "JitterTexture" )->AsShaderResource();
    g_pPermTex = g_pEffect->GetVariableByName( "PermTexture" )->AsShaderResource();
    g_pCubeMapTextureVariable = g_pEffect->GetVariableByName( "ShadowMap" )->AsShaderResource();
    
    // Obtain miscellaneous variables

    g_pmCubeViewMatrixVariable = g_pEffect->GetVariableByName( "CubeViewMatrices" )->AsMatrix();
    g_pmCubeProjMatrixVariable = g_pEffect->GetVariableByName( "CubeProjectionMatrix" )->AsMatrix();
    g_pmWorldViewProj = g_pEffect->GetVariableByName( "WorldViewProj" )->AsMatrix();
    g_pvEyePos = g_pEffect->GetVariableByName( "EyePos" )->AsVector();
    g_pvLightPos = g_pEffect->GetVariableByName( "LightPos" )->AsVector();
    g_pfLightIntensity = g_pEffect->GetVariableByName( "LightIntensity" )->AsScalar();
    g_pfStepSize = g_pEffect->GetVariableByName( "StepSize" )->AsScalar();
    g_pfTime = g_pEffect->GetVariableByName( "Time" )->AsScalar();
    g_pfNoiseScale = g_pEffect->GetVariableByName( "NoiseScale" )->AsScalar();
    g_pfRoughness = g_pEffect->GetVariableByName( "Roughness" )->AsScalar();
    g_pfFrequencyWeights = g_pEffect->GetVariableByName( "FrequencyWeights" )->AsScalar();
    g_pbJitter = g_pEffect->GetVariableByName( "bJitter" )->AsScalar();
    g_piCubeMapFace = g_pEffect->GetVariableByName( "CubeMapFace" )->AsScalar();
*/

	Image3 img;
	img.create(FORMAT_R32f, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight(), 1, 1);
	g_pScreenDepth=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());

    // Set input layouts

	if(g_pGeometryTechnique>-1)
	{
	FormatDesc format2[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	g_pGeometryVertexLayout = IRenderer::GetRendererInstance()->addVertexFormat(format1, elementsOf(format2), g_pGeometryTechnique);
	}
#if 0
    g_OutsideWorldMesh=FW3::GetMesh(g_OutsideWorldMesh, "/Orcs/Bonfire_wOrcs.x", eSimpleShader, false);
#elif 1
    g_OutsideWorldMesh.Load("/Orcs/Bonfire_wOrcs.x", eSimpleShader, false);
#endif

    D3DXFROMWINEVECTOR3 _min;
    D3DXFROMWINEVECTOR3 _max;
    g_OutsideWorldMesh.GetBounds(_min, _max);
    //LOG_FNLN;
    //LOG_PRINT("_min.x=%f\n", _min.x);
    //LOG_PRINT("_min.y=%f\n", _min.y);
    //LOG_PRINT("_min.z=%f\n", _min.z);
    //LOG_PRINT("_max.x=%f\n", _max.x);
    //LOG_PRINT("_max.y=%f\n", _max.y);
    //LOG_PRINT("_max.z=%f\n", _max.z);
	

    //g_pMesh.CreateTexturedBox(1.0f, 1.0f, 1.0f, eShaderNone);
    //g_pMesh.CreateBox(1.0f, 1.0f, 1.0f, eShaderNone);
    g_pMesh.CreateSphere(0.1f, eShaderNone);

    // Load textures
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

	{
	   g_pNoiseTexture=IRenderer::GetRendererInstance()->addTexture(256, 256, 1, FORMAT_R8, IRenderer::GetRendererInstance()->Getlinear());
	}

    // Create the shader resource view for jittering
	{
	    g_pJitterTexture=IRenderer::GetRendererInstance()->addTexture(256, 256, 1, FORMAT_R8, IRenderer::GetRendererInstance()->Getlinear/*Aniso*/());
	}

    // Create the shader resource view for permutation
	{
	   g_pPermTexture=IRenderer::GetRendererInstance()->addTexture(256, 256, 1, FORMAT_R8, IRenderer::GetRendererInstance()->Getlinear());
	}

    D3DXFROMWINEMatrixIdentity( &mWorld );
    // Setup cubemap
    
    PrepareCubeMap(  );
    
    //g_pCubeMapTextureVariable->SetResource( g_pCubeMapTextureRV );
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------

virtual void render()
{
    IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
    IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);

    // Set matrices

    D3DXFROMWINEMATRIX mView, mProj, mWorld, R, S, T;
#if 1
    float aspectRatio = 1.0f*IRenderer::GetRendererInstance()->GetViewportWidth()/IRenderer::GetRendererInstance()->GetViewportHeight();
    //g_Camera.SetProjParams(D3DXFROMWINE_PI / 3, aspectRatio, 1.0f, 1000.0f);
	
	//D3DXFROMWINEMatrixPerspectiveLH(&mProj, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight(), 1.0f, 1000.0f);
	D3DXFROMWINEMatrixPerspectiveFovLH(&mProj, D3DXFROMWINE_PI / 3, aspectRatio, 1.0f, 1000.0f);

    // Setup camera
    D3DXFROMWINEVECTOR3 vEye3(0.0f, 0.0f, -7.0f);
    D3DXFROMWINEVECTOR3 vAt(0.0f, 0.0f, 0.0f);
	D3DXFROMWINEVECTOR3 vup(0.0f, 1.0f, 0.0f);
	D3DXFROMWINEMatrixLookAtLH(&mView, &vEye3, &vAt, &vup);

		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX+18.37f),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
		float s=1.0f;
		D3DXFROMWINEMatrixScaling( &S, s, s, s);
		D3DXFROMWINEMatrixTranslation( &T, 0.0f, 0.0f, -7.0f);  
    mWorld=R*S*T;

    // Set matrices
    D3DXFROMWINEMATRIX mViewProj = mView * mProj;
    D3DXFROMWINEMATRIX mWorldView = mWorld * mView;
    D3DXFROMWINEMATRIX mWorldViewProj = mWorldView * mProj;
#else
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
		float s=1.0f;
		D3DXFROMWINEMatrixScaling( &S, s, s, s); 
		D3DXFROMWINEMatrixTranslation( &T, 0.0f, 0.0f, -7.0f); 
    D3DXFROMWINEMATRIX mWorldViewProj= R*S*T;
#endif
    g_OutsideWorldMesh.render(&mWorldViewProj);
 
    float rnd = (float)stx_rand() / RAND_MAX * 0.5f + 0.5f;
    g_pflightPos=D3DXFROMWINEVECTOR3(0.25f * (rnd - 0.5f), 5.7f, 1.0f * (rnd - 0.5f));
    
    InitCubeMatrices( g_pflightPos );


    D3DXFROMWINEMATRIX mTranslate, mScale, mWorldViewInv;
    D3DXFROMWINEMatrixTranslation( &mTranslate, 0.0f, 0.5f, 0.0f );
    D3DXFROMWINEMatrixScaling( &mScale, 4.0f * g_fShapeSize, 8.0f * g_fShapeSize, 4.0f * g_fShapeSize);

	D3DXFROMWINEMatrixIdentity(&mWorld);

    mWorldView = mTranslate * mScale * mWorld * mView;
    mWorldViewProj = mWorldView * mProj;

    D3DXFROMWINEMatrixInverse( &mWorldViewInv, 0, &mWorldView );

    D3DXFROMWINEVECTOR4 vEye;
    D3DXFROMWINEVECTOR4 v4(0.0f, 0.0f, 0.0f, 1.0f);
    D3DXFROMWINEVec4Transform( &vEye, &v4, &mWorldViewInv );
	//LOG_FNLN;
	//LOG_PRINT("x:g_pCurrentTechnique=%x\n", g_pCurrentTechnique);
	//LOG_PRINT("x:g_pCurrentLayout=%x\n", g_pCurrentLayout);
	D3DXFROMWINEMatrixIdentity(&mWorldViewProj);
    //g_pMesh.BeginDraw(mWorldViewProj, -1, g_pCurrentTechnique, g_pCurrentLayout);

	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
    g_pMesh.BeginDraw(&I, -1, g_pCurrentTechnique, g_pCurrentLayout);
	//IRenderer::GetRendererInstance()->setVertexFormat(g_pCurrentLayout);

    D3DXFROMWINEMatrixInverse( &g_pmWorldViewInv, 0, &g_pmWorldView );

    D3DXFROMWINEVECTOR4 v(0, 0, 0, 1);
    D3DXFROMWINEVec4Transform( &g_pvEyePos, &v, &g_pmWorldViewInv );

    // Obtain texture variables

    	IRenderer::GetRendererInstance()->setTexture("ScreenDepth", g_pScreenDepth);
    	//IRenderer::GetRendererInstance()->setTexture("SceneTexture", g_pSceneTexture);
    	IRenderer::GetRendererInstance()->setTexture("FireShape", g_pFireTexture);
    	IRenderer::GetRendererInstance()->setTexture("JitterTexture", g_pJitterTexture);
    	IRenderer::GetRendererInstance()->setTexture("PermTexture", g_pPermTexture);
    	//IRenderer::GetRendererInstance()->setTexture("ShadowMap", g_pCubeMapTexture);

	//IRenderer::GetRendererInstance()->setTexture("FireTex", g_pFireTexture);
    
    // Obtain miscellaneous variables
    	IRenderer::GetRendererInstance()->setShaderConstant4f("LightPos", g_pvLightPos);
    	IRenderer::GetRendererInstance()->setShaderConstant1i("StepSize", g_pfStepSize);
    	IRenderer::GetRendererInstance()->setShaderConstant1i("Time", g_pfTime);
    	IRenderer::GetRendererInstance()->setShaderConstant1i("NoiseScale", g_pfNoiseScale);
    	IRenderer::GetRendererInstance()->setShaderConstant1i("Roughness", g_pfRoughness);
    	IRenderer::GetRendererInstance()->setShaderConstant1i("FrequencyWeights", g_pfFrequencyWeights);
    	IRenderer::GetRendererInstance()->setShaderConstant1i("bJitter", g_pbJitter);
    	IRenderer::GetRendererInstance()->setShaderConstant1i("CubeMapFace", g_piCubeMapFace);

	IRenderer::GetRendererInstance()->setShaderConstantArray4x4f("CubeViewMatrices", g_pmCubeViewMatrixVariable, 6);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("CubeProjectionMatrix", g_pmCubeProjMatrixVariable);

    IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProj", mWorldViewProj );
    IRenderer::GetRendererInstance()->setShaderConstant4f("EyePos", vEye );
    IRenderer::GetRendererInstance()->setShaderConstant1f("LightIntensity", rnd );

	g_pMesh.EndDraw();
	STXGUI::update();

    IRenderer::GetRendererInstance()->EndScene();
    IRenderer::GetRendererInstance()->Present();
}

virtual int init(const char* aTitle)
{
	InitApp();	
	STXGUI::init("/Orcs/GUILayout.xml");
	return 0;
}
};
int ApplicationLogic()
{
#if 0//def LINUX
	IRenderer* r=IRenderer::GetRendererInstance("PerlinFire", eGLGLSL_1_1, eDebugInit);
#else
	IRenderer* r=IRenderer::GetRendererInstance("PerlinFire");
#endif	
	IInput*    i=STX_Service::GetInputInstance();
	{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

		const char* txt = "Initializing. Please wait a moment.";
		IRenderer::GetRendererInstance()->drawText(txt, 250, 250, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
	}
	PerlinFire app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}


