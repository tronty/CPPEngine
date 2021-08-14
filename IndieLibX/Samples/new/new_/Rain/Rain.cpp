//----------------------------------------------------------------------------------
// File:   Rain.cpp
// Author: Sarah Tariq
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

#define _USE_MATH_DEFINES

/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>


using namespace std;

#define DEG2RAD( a ) ( a * D3DXFROMWINE_PI / 180.f )


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
D3DXFROMWINEMATRIX                  g_Proj;
D3DXFROMWINEMATRIX                  g_View;
D3DXFROMWINEMATRIX                  g_World;
D3DXFROMWINEVECTOR3 g_EyePt;
stx_Effect*               g_pEffect = 0;       // D3DXFROMWINE effect interface
D3DXFROMWINEVECTOR3       g_lightDirectionWidget;
float       		  g_lightDirectionWidgetRadius;
bool                        g_bShowHelp = false;    // show help menu
D3DFROMWINEVIEWPORT9              g_SceneVP;

//techniques
stx_Effect::stx_Technique*  g_pTechniqueAdvanceRain            = 0;
stx_Effect::stx_Technique*  g_pTechniqueDrawRain               = 0;
stx_Effect::stx_Technique*  g_pTechniqueDrawRainCheap          = 0;
stx_Effect::stx_Technique*  g_pTechniqueRenderScene            = 0;
stx_Effect::stx_Technique*  g_pTechniqueRenderSky              = 0;
stx_Effect::stx_Technique*  g_pTechniqueRenderArrow            = 0;


//vertex buffers and layout
VertexFormatID       g_pVertexLayoutRainVertex          = 0; 
VertexFormatID       g_pVertexLayoutArrow               = 0; // Vertex_ Layout for arrow
VertexFormatID       g_pVertexLayoutScene               = 0;
VertexFormatID       g_pVertexLayoutSky                 = 0;
VertexBufferID            g_pParticleStart                   = 0;
VertexBufferID            g_pParticleStreamTo                = 0;
VertexBufferID            g_pParticleDrawFrom                = 0;
MeshRenderer2             g_Mesh;
MeshRenderer2             g_MeshArrow;
VertexBufferID            g_VertexBufferSky;


//shader variables
stx_Effect::stx_Variable* g_pWorldShaderVariable                = 0;
stx_Effect::stx_Variable* g_pWorldViewProjShaderVariable        = 0;
stx_Effect::stx_Variable* g_pInvViewShaderVariable              = 0;
stx_Effect::stx_Variable* g_pWorldViewShaderVariable            = 0; 
stx_Effect::stx_Variable* g_pProjectionShaderVariable           = 0;
stx_Effect::stx_Variable* g_pInverseProjectionShaderVariable    = 0;
stx_Effect::stx_Variable* g_pEyePosShaderVariable               = 0;
stx_Effect::stx_Variable* g_pLightPosShaderVariable             = 0;
stx_Effect::stx_Variable* g_pTotalVelShaderVariable             = 0;
stx_Effect::stx_Variable* g_pDiffuseShaderVariable              = 0;
stx_Effect::stx_Variable* g_LightPosWithViewTransformationShaderVariable = 0;
stx_Effect::stx_Variable* g_LightPosWithViewTransformation2ShaderVariable = 0;
stx_Effect::stx_Variable* g_VecPointLightEyeShaderVariable      = 0;
stx_Effect::stx_Variable* g_VecPointLightEye2ShaderVariable      = 0;
stx_Effect::stx_Variable* g_pRenderBGShaderVariable             = 0;
stx_Effect::stx_Variable* g_pMoveParticlesShaderVariable        = 0;
stx_Effect::stx_Variable* g_pDirLightIntensityShaderVariable    = 0;
stx_Effect::stx_Variable* g_pPointLightIntensityShaderVariable  = 0;
stx_Effect::stx_Variable* g_pResponseDirLightShaderVariable     = 0;
stx_Effect::stx_Variable* g_pResponsePointLightShaderVariable   = 0;
stx_Effect::stx_Variable* g_pSpriteSizeShaderVariable           = 0;
stx_Effect::stx_Variable* g_pFogThicknessShaderVariable         = 0;
stx_Effect::stx_Variable* g_ScreenWidthShaderVariable           = 0;
stx_Effect::stx_Variable* g_ScreenHeightShaderVariable          = 0;
stx_Effect::stx_Variable* g_ScreenWidthMultiplierShaderVariable = 0;
stx_Effect::stx_Variable* g_ScreenHeightMultiplierShaderVariable= 0;
stx_Effect::stx_Variable* g_TSVPointLightShaderVariable         = 0;
stx_Effect::stx_Variable* g_DSVPointLightShaderVariable         = 0;
stx_Effect::stx_Variable* g_DSVPointLight2ShaderVariable        = 0;
stx_Effect::stx_Variable* g_DSVPointLight3ShaderVariable        = 0;
stx_Effect::stx_Variable* g_deShaderVariable                    = 0;
stx_Effect::stx_Variable* g_UseSpotLightShaderVariable          = 0;
stx_Effect::stx_Variable* g_CosSpotLightShaderVariable          = 0;
stx_Effect::stx_Variable* g_KdShaderVariable                    = 0;
stx_Effect::stx_Variable* g_KsPointShaderVariable               = 0;
stx_Effect::stx_Variable* g_KsDirShaderVariable                 = 0;
stx_Effect::stx_Variable* g_SpecPowerShaderVariable             = 0;
stx_Effect::stx_Variable* g_FPSShaderVariable                   = 0;
stx_Effect::stx_Variable* g_timeCycleShaderVariable             = 0;
stx_Effect::stx_Variable* g_rainSplashesXDisplaceShaderVariable = 0;
stx_Effect::stx_Variable* g_rainSplashesYDisplaceShaderVariable = 0;

//presets for the variables
const D3DXFROMWINEVECTOR3 g_constFogVector1 = D3DXFROMWINEVECTOR3(0.03f,0.03f,0.03f);
const float       g_constDirLightIntensity1 = 0.18f;
const float       g_constResponseDirLight1 = 1.0f;
const float       g_constPointLightIntensity1 = 0.5f;
const float       g_constCosSpotLight1 = 0.3f;
const float       g_constResponsePointLight1 = 2.0f;
const float       g_constDrawFraction1 = 0.7f;
const float       g_constWindAmount1 = 1.0;
const D3DXFROMWINEVECTOR3 g_constVecEye1 = D3DXFROMWINEVECTOR3( 15.5f, 5.0f, 0.0f );
const D3DXFROMWINEVECTOR3 g_constAtVec1  = D3DXFROMWINEVECTOR3( 0.0f, 3.0f, 0.0f );
const D3DXFROMWINEVECTOR3 g_directionalLightVector1 = D3DXFROMWINEVECTOR3(0.551748, 0.731354, 0.400869);

const D3DXFROMWINEVECTOR3 g_constFogVector2 = D3DXFROMWINEVECTOR3(0.02f,0.02f,0.02f);
const float       g_constDirLightIntensity2 = 0.50f;
const float       g_constResponseDirLight2 = 0.68f;
const float       g_constPointLightIntensity2 = 0.4f;
const float       g_constCosSpotLight2 = 0.54f;
const float       g_constResponsePointLight2 = 2.0f;
const float       g_constDrawFraction2 = 0.6f;
const float       g_constWindAmount2 = 0.48;
const D3DXFROMWINEVECTOR3 g_constVecEye2 = D3DXFROMWINEVECTOR3( -4.0f, 18.0f, -5.0f );
const D3DXFROMWINEVECTOR3 g_constAtVec2  = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );
const D3DXFROMWINEVECTOR3 g_directionalLightVector2 = D3DXFROMWINEVECTOR3(0.470105, 0.766044, 0.43838);

//global variables

float g_ScreenWidth = 1264.0f;
float g_ScreenHeight = 958.0f;
float g_SpriteSize = 0.8;
float g_dirLightRadius = 1000;
bool g_bRenderBg = true;
bool g_bMoveParticles = true; 
bool g_bDrawParticles = true;
int g_numRainVertices = 150000;
float g_dirLightIntensity = 0.27f;
float g_PointLightIntensity = 0.58;
float g_znear = 1.0f;
float g_zfar  = 30000.0f;
float g_fov =  0.3*D3DXFROMWINE_PI;
float g_cosSpotLight = 0.54;
bool g_bUseSpotLight = true;
float g_responseDirLight = 0.9f;
float g_responsePointLight = 2.0f;
float g_heightMin = 0.0f;
float g_heightRange = 40.0f;
float g_radiusMin = 0.0f;
float g_radiusRange = 45.0f; 
bool g_bUseCheapShader = false;
bool firstFrame;
int frameCount = 0;
float g_WindAmount;
float g_DrawFraction = 1.0;
D3DXFROMWINEVECTOR3 g_fogVector;
D3DXFROMWINEVECTOR3 g_vecEye;
D3DXFROMWINEVECTOR3 g_vecAt;
D3DXFROMWINEVECTOR3 g_lightPos;
D3DXFROMWINEVECTOR3 g_eyeInObjectSpace;
D3DXFROMWINEVECTOR3 g_TotalVel = D3DXFROMWINEVECTOR3(-0.05,-0.5,0);
D3DXFROMWINEVECTOR3 g_PointLightPos = D3DXFROMWINEVECTOR3(  3.7,5.8,3.15);
D3DXFROMWINEVECTOR3 g_PointLightPos2 = D3DXFROMWINEVECTOR3(-3.7,5.8,3.15);
D3DXFROMWINEMATRIX g_WorldMatrix;


//structures
struct RainVertex
{
    D3DXFROMWINEVECTOR3 pos;
    D3DXFROMWINEVECTOR3 seed;
    D3DXFROMWINEVECTOR3 speed;
    float random;
    unsigned char  Type;
};

struct SimpleVertex
{
    D3DXFROMWINEVECTOR3 Pos;  // Position
    D3DXFROMWINEVECTOR2 Tex; // Texture Coordinate
};

struct WindValue
{
   D3DXFROMWINEVECTOR3 windAmount;
   int time;
   WindValue(D3DXFROMWINEVECTOR3 wA, int t){windAmount = wA; time=t;};
};
vector<WindValue> WindAnimation;
int totalAnimationValues = 0;

struct App
{
    App(){}
    virtual ~App(){}
//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
int OnCreateDevice()
{
    int hr = 0;
    #if 0
	g_pEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/new/Rain/Rain.fx");
    #else
    g_pEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/SimpleTexture/SimpleTexture.fx");
    #endif

    // Obtain the techniques
    g_pTechniqueAdvanceRain   = g_pEffect->GetTechniqueByName("AdvanceParticles");
    g_pTechniqueDrawRain      = g_pEffect->GetTechniqueByName("RenderParticles");
    g_pTechniqueDrawRainCheap = g_pEffect->GetTechniqueByName("RenderParticlesCheap");
    g_pTechniqueRenderScene   = g_pEffect->GetTechniqueByName("RenderScene");
    g_pTechniqueRenderSky     = g_pEffect->GetTechniqueByName("RenderSky");
    g_pTechniqueRenderArrow  = g_pEffect->GetTechniqueByName("RenderArrow");

    // Obtain the variables
    g_pWorldShaderVariable = g_pEffect->GetVariableByName( "g_mWorld" );
    g_pWorldViewProjShaderVariable = g_pEffect->GetVariableByName( "g_mWorldViewProj" );
    g_pInvViewShaderVariable = g_pEffect->GetVariableByName( "g_mInvView" );
    g_pWorldViewShaderVariable = g_pEffect->GetVariableByName( "g_mWorldView" );
    g_pProjectionShaderVariable = g_pEffect->GetVariableByName( "g_mProjection" );
    g_pInverseProjectionShaderVariable = g_pEffect->GetVariableByName( "g_mInverseProjection" );
   
    g_pLightPosShaderVariable = g_pEffect->GetVariableByName( "g_lightPos" );  
    g_pEyePosShaderVariable = g_pEffect->GetVariableByName( "g_eyePos" );
    g_pTotalVelShaderVariable = g_pEffect->GetVariableByName( "g_TotalVel" );    
    g_pDiffuseShaderVariable = g_pEffect->GetVariableByName( "g_DiffuseColor" );

    g_pRenderBGShaderVariable = g_pEffect->GetVariableByName( "renderBg");
    g_pMoveParticlesShaderVariable = g_pEffect->GetVariableByName( "moveParticles");
    g_pFogThicknessShaderVariable = g_pEffect->GetVariableByName( "g_beta");
    g_pDirLightIntensityShaderVariable = g_pEffect->GetVariableByName( "dirLightIntensity");
    g_pPointLightIntensityShaderVariable = g_pEffect->GetVariableByName( "g_PointLightIntensity");
    g_pResponseDirLightShaderVariable = g_pEffect->GetVariableByName( "g_ResponseDirLight");
    g_pResponsePointLightShaderVariable = g_pEffect->GetVariableByName( "g_ResponsePointLight");
    g_pSpriteSizeShaderVariable = g_pEffect->GetVariableByName( "g_SpriteSize");
    g_ScreenWidthShaderVariable = g_pEffect->GetVariableByName( "g_ScreenWidth");
    g_ScreenHeightShaderVariable = g_pEffect->GetVariableByName( "g_ScreenHeight");
    g_ScreenWidthMultiplierShaderVariable = g_pEffect->GetVariableByName( "g_ScreenWidthMultiplier");
    g_ScreenHeightMultiplierShaderVariable = g_pEffect->GetVariableByName( "g_ScreenHeightMultiplier");
    g_LightPosWithViewTransformationShaderVariable = g_pEffect->GetVariableByName( "g_ViewSpaceLightVec");
    g_LightPosWithViewTransformation2ShaderVariable = g_pEffect->GetVariableByName( "g_ViewSpaceLightVec2");
    g_VecPointLightEyeShaderVariable = g_pEffect->GetVariableByName( "g_VecPointLightEye");
    g_VecPointLightEye2ShaderVariable = g_pEffect->GetVariableByName( "g_VecPointLightEye2");
    g_TSVPointLightShaderVariable = g_pEffect->GetVariableByName( "g_TSVPointLight");
    g_DSVPointLightShaderVariable = g_pEffect->GetVariableByName( "g_DSVPointLight");
    g_DSVPointLight2ShaderVariable = g_pEffect->GetVariableByName( "g_DSVPointLight2");
    g_DSVPointLight3ShaderVariable = g_pEffect->GetVariableByName( "g_DSVPointLight3");
    g_deShaderVariable = g_pEffect->GetVariableByName( "g_de");
    g_UseSpotLightShaderVariable = g_pEffect->GetVariableByName( "g_useSpotLight");
    g_CosSpotLightShaderVariable = g_pEffect->GetVariableByName( "g_cosSpotlightAngle");      
    g_KdShaderVariable = g_pEffect->GetVariableByName( "g_Kd"); 
    g_KsPointShaderVariable = g_pEffect->GetVariableByName( "g_KsPoint"); 
    g_KsDirShaderVariable = g_pEffect->GetVariableByName( "g_KsDir"); 
    g_SpecPowerShaderVariable = g_pEffect->GetVariableByName( "g_specPower");
    g_FPSShaderVariable = g_pEffect->GetVariableByName( "g_FrameRate");
    g_timeCycleShaderVariable = g_pEffect->GetVariableByName( "g_timeCycle");
	g_rainSplashesXDisplaceShaderVariable = g_pEffect->GetVariableByName( "g_splashXDisplace");
	g_rainSplashesYDisplaceShaderVariable = g_pEffect->GetVariableByName( "g_splashYDisplace");

    g_pEffect->GetVariableByName( "g_heightMin" )->SetFloat(g_heightMin);
    g_pEffect->GetVariableByName( "g_radiusMin" )->SetFloat(g_radiusMin);
    g_pEffect->GetVariableByName( "g_heightRange" )->SetFloat(g_heightRange);
    g_pEffect->GetVariableByName( "g_radiusRange" )->SetFloat(g_radiusRange);
    g_pEffect->GetVariableByName( "maxHeight" )->SetFloat(-5);
    g_pEffect->GetVariableByName( "g_Near" )->SetFloat(g_znear);
    g_pEffect->GetVariableByName( "g_Far" )->SetFloat(g_zfar);

    resetVariablesToPreset1();
    frameCount = 0;

    //populate the wind animation
    WindAnimation.clear();
    int time = 0; //time in seconds between each key
    WindAnimation.push_back( WindValue( D3DXFROMWINEVECTOR3(-0.1,-0.5,0),   time ) );
    WindAnimation.push_back( WindValue( D3DXFROMWINEVECTOR3(-0.4,-0.5,0.04), time += 10 ) ); 
    WindAnimation.push_back( WindValue( D3DXFROMWINEVECTOR3(-0.2,-0.5,-0.4),   time += 5 ) );   
    WindAnimation.push_back( WindValue( D3DXFROMWINEVECTOR3(0.0,-0.5,-0.02), time += 10 ) );
    WindAnimation.push_back( WindValue( D3DXFROMWINEVECTOR3(0.0,-0.5,-0.02), time += 10 ) );
    WindAnimation.push_back( WindValue( D3DXFROMWINEVECTOR3(0.1,-0.5,0.4),  time += 6) );
    WindAnimation.push_back( WindValue( D3DXFROMWINEVECTOR3(-0.1,-0.5,0),   time += 5 ) );
    g_pTotalVelShaderVariable->SetFloatVector((float*)WindAnimation.at(0).windAmount);

    //set the variables
    D3DXFROMWINEMATRIX ViewMatrix;
    D3DXFROMWINEMATRIX InvViewMatrix;
    ViewMatrix=g_View;
    D3DXFROMWINEMatrixInverse( &InvViewMatrix, 0, &ViewMatrix );


    D3DXFROMWINEVECTOR3 VecLightEye;
    D3DXFROMWINEVec3Subtract(&VecLightEye,&g_PointLightPos, &g_EyePt);
    float lengthVecLightEye = D3DXFROMWINEVec3Length(&VecLightEye);
    D3DXFROMWINEVec3Normalize(&VecLightEye,&VecLightEye);

    D3DXFROMWINEVECTOR3 VecLightEye2;
    D3DXFROMWINEVec3Subtract(&VecLightEye2,&g_PointLightPos2, &g_EyePt);
    float lengthVecLightEye2 = D3DXFROMWINEVec3Length(&VecLightEye2);
    D3DXFROMWINEVec3Normalize(&VecLightEye2,&VecLightEye2);


    //initialize the matrices
    g_pInvViewShaderVariable->SetMatrix( (float*)&InvViewMatrix );
    g_pProjectionShaderVariable->SetMatrix( (float*)g_Proj);

     //initialize the vectors
    g_pEyePosShaderVariable->SetFloatVector((float*)&g_vecEye);
    D3DXFROMWINEVECTOR3 LightInViewSpace;
    vectorMatrixMultiply(&LightInViewSpace, ViewMatrix,g_PointLightPos);
    D3DXFROMWINEVec3Normalize(&LightInViewSpace,&LightInViewSpace);
    g_LightPosWithViewTransformationShaderVariable->SetFloatVector((float*)&LightInViewSpace);
    vectorMatrixMultiply(&LightInViewSpace, ViewMatrix,g_PointLightPos2);
    D3DXFROMWINEVec3Normalize(&LightInViewSpace,&LightInViewSpace);
    g_LightPosWithViewTransformation2ShaderVariable->SetFloatVector((float*)&LightInViewSpace);
    g_pFogThicknessShaderVariable->SetFloatVector((float*)&g_fogVector);


    //initialize the scalars
    g_pSpriteSizeShaderVariable->SetFloat(g_SpriteSize);
    g_ScreenWidthShaderVariable->SetFloat(g_ScreenWidth);
    g_ScreenHeightShaderVariable->SetFloat(g_ScreenHeight);
    g_ScreenWidthMultiplierShaderVariable->SetFloat( 2.0f/(g_ScreenWidth-1)  );
    g_ScreenHeightMultiplierShaderVariable->SetFloat( 2.0f/(g_ScreenHeight-1)  );
    g_DSVPointLightShaderVariable->SetFloat( lengthVecLightEye );
    g_DSVPointLight2ShaderVariable->SetFloat( lengthVecLightEye2 );
    g_deShaderVariable->SetFloat( (g_ScreenHeight/2.0f)/(tan(g_fov/2.0f)) );
    setShadingParametersBasedOnRain();



    //----------------------------------------------------------------------------------------------
    //vertex buffers 
    //----------------------------------------------------------------------------------------------

    
    //vertices for the rain particles---------------------------------------------------------------
    //generate vertices in a cylinder above the camera


    firstFrame = true;
    RainVertex* vertices = new RainVertex[g_numRainVertices];
    if(vertices==0) 
        exit(0);

    for(int i=0;i<g_numRainVertices;i++)
    {
        RainVertex raindrop;
        //use rejection sampling to generate random points inside a circle of radius 1 centered at 0,0
        float SeedX;
        float SeedZ;
        bool pointIsInside = false;
        while(!pointIsInside)
        { 
           SeedX = random() - 0.5f;
           SeedZ = random() - 0.5f;
           if( sqrt( SeedX*SeedX + SeedZ*SeedZ ) <= 0.5f )
               pointIsInside = true;
        }
        //save these random locations for reinitializing rain particles that have fallen out of bounds
        SeedX *= g_radiusRange;
        SeedZ *= g_radiusRange;
        float SeedY = random()*g_heightRange;
        raindrop.seed = D3DXFROMWINEVECTOR3(SeedX,SeedY,SeedZ); 
        
        //add some random speed to the particles, to prevent all the particles from following exactly the same trajectory
        //additionally, random speeds in the vertical direction ensure that temporal aliasing is minimized
        float SpeedX = 40.0f*(random()/20.0f);
        float SpeedZ = 40.0f*(random()/20.0f);
        float SpeedY = 40.0f*(random()/10.0f); 
        raindrop.speed = D3DXFROMWINEVECTOR3(SpeedX,SpeedY,SpeedZ);

        //move the rain particles to a random positions in a cylinder above the camera
        float x = SeedX + g_vecEye.x;
        float z = SeedZ + g_vecEye.z;
        float y = SeedY + g_vecEye.y;
        raindrop.pos = D3DXFROMWINEVECTOR3(x,y,z); 

        //get an integer between 1 and 8 inclusive to decide which of the 8 types of rain textures the particle will use
        raindrop.Type = int(floor(  random()*8 + 1 ));

        //this number is used to randomly increase the brightness of some rain particles
        raindrop.random = 1;
        float randomIncrease = random();
        if( randomIncrease > 0.8)
            raindrop.random += randomIncrease;

        vertices[i] = raindrop;
    }

    //create vertex buffers for the rain, two will be used to pingpong between during animation
    g_pParticleDrawFrom=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( RainVertex ) * g_numRainVertices, STATIC, vertices);
    g_pParticleStreamTo=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( RainVertex ) * g_numRainVertices, STATIC, vertices);
    delete[] vertices;

    // Create the vertex input layout for rain
#if 0
    const FormatDesc layout[] =
    {
        { "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "SEED",     0, FORMAT_R32G32B32_FLOAT, 0, 12,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "SPEED",    0, FORMAT_R32G32B32_FLOAT, 0, 24,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "RAND",     0, FORMAT_R32_FLOAT,       0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TYPE",     0, FORMAT_R8_UINT,         0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };
#elif 0
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 1,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 1
	};
	g_pVertexLayoutRainVertex = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}
#endif

    //---------------------------------------------------------------------------------------------
    
    //scene mesh

    // Create the input layout for the scene
	#if 0
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		0, TYPE_TANGENT, FORMAT_FLOAT, 3
	};
	g_pVertexLayoutScene = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}
	#endif
    // Set the input layout
    //IRenderer::GetRendererInstance()->setVertexFormat( g_pVertexLayoutScene );
    //load the scene
    #if 0
    g_Mesh.Load("/new/Rain/Bridge/Bridge.x"); // ???
    #else
    g_Mesh.CreateSphere(1.0f);
    #endif
    
    //---------------------------------------------------------------------------------------------
    //mesh and layout for arrow

    // Create the input layout
	#if 0
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	g_pVertexLayoutArrow = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}
	#endif

    //load the arrow mesh
    g_MeshArrow.Load("/new/Rain/arrow.x"); 

    //-------------------------------------------------------------------------------------------------
    //vertex buffer and layout for sky

    // Create the input layout
	#if 0
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2
	};
	g_pVertexLayoutSky = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}
	#endif

    //make the vertex buffer for the quad that will be used for the sky
    D3DXFROMWINEVECTOR2 verticesSky[4];
    verticesSky[0] = D3DXFROMWINEVECTOR2( 1.0f,  1.0f);
    verticesSky[1] = D3DXFROMWINEVECTOR2( 1.0f, -1.0f);
    verticesSky[2] = D3DXFROMWINEVECTOR2(-1.0f,  1.0f);
    verticesSky[3] = D3DXFROMWINEVECTOR2(-1.0f, -1.0f);
	g_VertexBufferSky=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( D3DXFROMWINEVECTOR2 ) * 4, STATIC, verticesSky);
   
    //---------------------------------------------------------------------------------------------
    //textures
    //---------------------------------------------------------------------------------------------

    //the textures for the bridge
    (loadTextureFromFile("/new/Rain/Bridge/bridge_color.dds","SceneTextureDiffuse")); 
    (loadTextureFromFile("/new/Rain/Bridge/bridge_spec.dds","SceneTextureSpecular")); 
    (loadTextureFromFile("/new/Rain/Bridge/bridge_normal.dds","SceneTextureNormal")); 

    //the 3D textures for the rain splashes
    (load3DTextureFromFile("/new/Rain/rainTextures/splashes/SBumpVolume.dds","SplashBumpTexture"));
    (load3DTextureFromFile("/new/Rain/rainTextures/splashes/SDiffuseVolume.dds","SplashDiffuseTexture"));

    //load the array of rain textures of point lights, see http://www1.cs.columbia.edu/CAVE/databases/rain_streak_db/rain_streak.php
    std::vector<TextureID>  rainTexture;
    stx_Effect::stx_Variable*   textureArray = g_pEffect->GetVariableByName( "rainTextureArray" );
    ( LoadTextureArray( "/new/Rain/rainTextures/cv0_vPositive_", 370 , rainTexture) );

    //////////////SAFE_RELEASE(rainTexture);
    //////////////SAFE_RELEASE(textureRV);    
   
    //load the look up tables for the fog, see http://www1.cs.columbia.edu/~bosun/sig05.htm
    if(loadLUTS("/new/Rain/F_512_data.csv","Ftable",512,512) == 1)
        loadLUTS("/new/Rain/F_512_data.csv","Ftable",512,512);             
    if(loadLUTS("/new/Rain/G0_pi_2_64_data.csv","Gtable",64,64) == 1)
        loadLUTS("/new/Rain/G0_pi_2_64_data.csv","Gtable",64,64); 
    if(loadLUTS("/new/Rain/G20_pi_2_64_data.csv","G_20table",64,64) == 1)
        loadLUTS("/new/Rain/G20_pi_2_64_data.csv","G_20table",64,64); 

    return 0;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
int  OnResizedSwapChain()
{
    int hr;

    // ??? TextureID  pNULLRT = {0};IRenderer::GetRendererInstance()->changeRenderTarget(&(pNULLRT), 0 );

    //Set viewport
    ZeroMemory( &g_SceneVP, sizeof( D3DFROMWINEVIEWPORT9 ) );
    g_SceneVP.Height = STX_Service::GetWindowInstance()->GetHeight();
    g_SceneVP.Width = STX_Service::GetWindowInstance()->GetWidth();
    g_SceneVP.MaxZ = 1.0f;

    // Setup the camera's projection parameters
    g_ScreenWidth = (float)STX_Service::GetWindowInstance()->GetWidth();
    g_ScreenHeight = (float)STX_Service::GetWindowInstance()->GetHeight();
    float fAspectRatio = IRenderer::GetRendererInstance()->GetAspectRatio();

    D3DXFROMWINEMatrixPerspectiveFovLH(&g_Proj, g_fov , fAspectRatio, g_znear , g_zfar );    

    g_ScreenWidthShaderVariable->SetFloat(g_ScreenWidth);
    g_ScreenHeightShaderVariable->SetFloat(g_ScreenHeight);
    g_ScreenWidthMultiplierShaderVariable->SetFloat( 2.0f/(g_ScreenWidth-1)  );
    g_ScreenHeightMultiplierShaderVariable->SetFloat( 2.0f/(g_ScreenHeight-1)  );
    g_deShaderVariable->SetFloat( (g_ScreenHeight/2.0f)/(tan(g_fov/2.0f)) );

    return 0;
}


void vectorMatrixMultiply(D3DXFROMWINEVECTOR3* vecOut,const D3DXFROMWINEMATRIX matrix,const D3DXFROMWINEVECTOR3 vecIn)
{
    vecOut->x = vecIn.x*matrix(0,0) + vecIn.y*matrix(1,0) + vecIn.z*matrix(2,0) + matrix(3,0);
    vecOut->y  = vecIn.x*matrix(0,1)+ vecIn.y*matrix(1,1) + vecIn.z*matrix(2,1) + matrix(3,1);
    vecOut->z = vecIn.x*matrix(0,2) + vecIn.y*matrix(1,2) + vecIn.z*matrix(2,2) + matrix(3,2);
}
//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------

void OnFrameRender(double fTime, float fElapsedTime)
{
    //set the viewport and render target
    IRenderer::GetRendererInstance()->setViewport(&g_SceneVP);
	//clear the render target and depth stencil view
	IRenderer::GetRendererInstance()->Clear(false,true, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));

    //------------------------------------------------------------------------------------
    // Set the wind vector
    //piece-wise linear interpolation for the animation curve
    //------------------------------------------------------------------------------------

     //let the frame rate stabilize before using it to determine animation speed
     if(frameCount < 50)
     { 
           D3DXFROMWINEVECTOR3 wind = WindAnimation.at(0).windAmount;
           g_FPSShaderVariable->SetFloat(40.0f);
           g_pTotalVelShaderVariable->SetFloatVector((float*)wind);
         frameCount++;
     }
     else
    
     { 
         static float time = 0;
         if(g_bMoveParticles)
         {
              //estimate a value of wind at the given time frame
              //we use piecewise linear interpolation between an array of key frames of wind values

              int lastTime = WindAnimation.back().time;
              int upperFrame = 1;
              float framesPerSecond = IRenderer::GetRendererInstance()->getFPS();
              g_FPSShaderVariable->SetFloat(framesPerSecond);

              while( time > WindAnimation.at(upperFrame).time )
                  upperFrame++;
        
              int lowerFrame = upperFrame - 1;
              float amount = float(time - WindAnimation.at(lowerFrame).time)/(WindAnimation.at(upperFrame).time - WindAnimation.at(lowerFrame).time);
              D3DXFROMWINEVECTOR3 interpolatedWind = WindAnimation.at(lowerFrame).windAmount + amount*( WindAnimation.at(upperFrame).windAmount - WindAnimation.at(lowerFrame).windAmount);

              //adjust the wind based on the current frame rate; the values were estimated for 40FPS
              interpolatedWind *= 40.0f/framesPerSecond;
              //lerp between the wind vector and just the vector pointing down depending on the amount of user chosen wind
              interpolatedWind = g_WindAmount*interpolatedWind + (1-g_WindAmount)*(D3DXFROMWINEVECTOR3(0,-0.5,0));

              g_pTotalVelShaderVariable->SetFloatVector((float*)interpolatedWind);
              time += 1.0f/framesPerSecond;
              if(time>lastTime)
                  time = 0;
         }
     }
     

    D3DXFROMWINEMATRIX ViewMatrix;
    D3DXFROMWINEMATRIX WorldViewMatrix;
    D3DXFROMWINEMATRIX WorldViewProjMatrix;
    D3DXFROMWINEMATRIX WorldInverseMatrix;
    D3DXFROMWINEMATRIX InvViewMatrix;
    D3DXFROMWINEMATRIX InvProjectionMatrix;


    //---------------------------------------------------------------------------------------
    // Draw the sky quad
    //------------------------------------------------------------------------------------
   
    if(g_bRenderBg)
    { 
         D3DXFROMWINEMATRIX ViewProjection;
         D3DXFROMWINEMatrixMultiply( &ViewProjection, &ViewMatrix, &g_Proj );
         D3DXFROMWINEMATRIX ViewProjectionInverse;    
         D3DXFROMWINEMatrixInverse( &ViewProjectionInverse, 0, &ViewProjection );
         g_pEffect->GetVariableByName("g_mViewProjectionInverse")->SetMatrix(ViewProjectionInverse);
         g_pTechniqueRenderSky->GetPassByIndex(0)->Apply(0);

         unsigned int stride[1] = {sizeof(D3DXFROMWINEVECTOR2)};
         unsigned int offset[1] = { 0 };
         //IRenderer::GetRendererInstance()->setVertexFormat(g_pVertexLayoutSky);
         IRenderer::GetRendererInstance()->setVertexBuffer(0, g_VertexBufferSky);
         IRenderer::GetRendererInstance()->Draw(PRIM_TRIANGLE_STRIP, 4, 0);
    }

    //------------------------------------------------------------------------------------
    //render the light directional control, and update the value of the light vector
    //------------------------------------------------------------------------------------

    g_lightPos = g_lightDirectionWidget;
    D3DXFROMWINEVec3Normalize(&g_lightPos,&g_lightPos);

    float arrowScale = 0.02f;
    D3DXFROMWINEMATRIX mScale;
    D3DXFROMWINEMatrixScaling( &mScale, arrowScale, arrowScale, arrowScale);

    D3DXFROMWINEMATRIX mLookAtMatrix;
    D3DXFROMWINEVECTOR3 ArrowPos =  g_vecAt - g_lightPos * g_lightDirectionWidgetRadius;
    D3DXFROMWINEVECTOR3 LightPos =  g_vecAt - g_lightPos * 1000;
    g_pLightPosShaderVariable->SetFloatVector((float*)&LightPos);
    D3DXFROMWINEVECTOR3 mOrigin = D3DXFROMWINEVECTOR3(0,0,0);
    D3DXFROMWINEVECTOR3 mUp = D3DXFROMWINEVECTOR3(0, 1, 0);
    D3DXFROMWINEMatrixLookAtLH(&mLookAtMatrix, &ArrowPos, &mOrigin, &mUp);
    D3DXFROMWINEMATRIX mLookAtInv;
    D3DXFROMWINEMatrixInverse(&mLookAtInv, 0, &mLookAtMatrix);
    D3DXFROMWINEMATRIX mWorld = mScale * mLookAtInv * (*g_World);
    D3DXFROMWINEMatrixMultiply( &WorldViewMatrix, &mWorld, &g_View);
    D3DXFROMWINEMatrixMultiply( &WorldViewProjMatrix, &WorldViewMatrix, &g_Proj );
    g_pWorldViewProjShaderVariable->SetMatrix( (float*)&WorldViewProjMatrix );

    //IRenderer::GetRendererInstance()->setVertexFormat(g_pVertexLayoutArrow);
    g_MeshArrow.Render(g_pTechniqueRenderArrow);
    

    //------------------------------------------------------------------------------------
    // Set Effects Parameters
    //------------------------------------------------------------------------------------

    ViewMatrix = g_View;
    D3DXFROMWINEVECTOR3 LightInViewSpace;
    vectorMatrixMultiply(&LightInViewSpace, ViewMatrix,g_PointLightPos);
    D3DXFROMWINEVec3Normalize(&LightInViewSpace,&LightInViewSpace);
    g_LightPosWithViewTransformationShaderVariable->SetFloatVector((float*)&LightInViewSpace);
    vectorMatrixMultiply(&LightInViewSpace, ViewMatrix,g_PointLightPos2);
    D3DXFROMWINEVec3Normalize(&LightInViewSpace,&LightInViewSpace);
    g_LightPosWithViewTransformation2ShaderVariable->SetFloatVector((float*)&LightInViewSpace);
    D3DXFROMWINEMatrixInverse( &InvViewMatrix, 0, &ViewMatrix );
    g_pInvViewShaderVariable->SetMatrix( (float*)&InvViewMatrix ); 
    g_pProjectionShaderVariable->SetMatrix( (float*)g_Proj);
    D3DXFROMWINEMatrixInverse( &InvProjectionMatrix, 0, &g_Proj );
    g_pInverseProjectionShaderVariable->SetMatrix( (float*)&InvProjectionMatrix);
    g_pEyePosShaderVariable->SetFloatVector((float*)g_EyePt);

    D3DXFROMWINEVECTOR3 VecLightEye;
    D3DXFROMWINEVec3Subtract(&VecLightEye,&g_PointLightPos, &g_EyePt);
    float lengthVecLightEye = D3DXFROMWINEVec3Length(&VecLightEye);
    D3DXFROMWINEVec3Normalize(&VecLightEye,&VecLightEye);
    g_VecPointLightEyeShaderVariable->SetFloatVector((float*)&VecLightEye);
    g_DSVPointLightShaderVariable->SetFloat( lengthVecLightEye );

    D3DXFROMWINEVECTOR3 VecLightEye2;
    D3DXFROMWINEVec3Subtract(&VecLightEye2,&g_PointLightPos2, &g_EyePt);
    float lengthVecLightEye2 = D3DXFROMWINEVec3Length(&VecLightEye2);
    D3DXFROMWINEVec3Normalize(&VecLightEye2,&VecLightEye2);
    g_VecPointLightEye2ShaderVariable->SetFloatVector((float*)&VecLightEye2);
    g_DSVPointLight2ShaderVariable->SetFloat( lengthVecLightEye2 );

    static float timeCycle = 0;
    if(g_bMoveParticles)
       timeCycle += 0.085;
    g_timeCycleShaderVariable->SetFloat(timeCycle);
    if(timeCycle>=1)
	{
		timeCycle = 0;
        g_rainSplashesXDisplaceShaderVariable->SetFloat( random()*2 );
        g_rainSplashesYDisplaceShaderVariable->SetFloat( random()*2 );
	}
    //-------------------------------------------------------------------------------------
    //draw the scene
    //-------------------------------------------------------------------------------------

    unsigned int stride[1] = { sizeof(SimpleVertex) };
    unsigned int offset[1] = { 0 };

    if(g_bRenderBg)
    { 
        D3DXFROMWINEMatrixScaling(&g_WorldMatrix, 3, 3, 3);
        D3DXFROMWINEMatrixMultiply( &WorldViewMatrix, &g_WorldMatrix, &ViewMatrix );
        D3DXFROMWINEMatrixMultiply( &WorldViewProjMatrix, &WorldViewMatrix, &g_Proj );
        g_pWorldShaderVariable->SetMatrix((float*)&g_WorldMatrix);
        g_pWorldViewProjShaderVariable->SetMatrix( (float*)&WorldViewProjMatrix );
        g_pWorldViewShaderVariable->SetMatrix( (float*)&WorldViewMatrix );

        //IRenderer::GetRendererInstance()->setVertexFormat( g_pVertexLayoutScene );
        g_Mesh.Render( g_pTechniqueRenderScene );
    }

    //-------------------------------------------------------------------------------------
    //advance rain
    //-------------------------------------------------------------------------------------

    stride[0] = sizeof(RainVertex);

    if(g_bMoveParticles)
    {
        //IRenderer::GetRendererInstance()->setVertexFormat( g_pVertexLayoutRainVertex );
    
        VertexBufferID pBuffers[1];
        if(firstFrame)
            IRenderer::GetRendererInstance()->setVertexBuffer( 0, g_pParticleStart); // ???
        else
            IRenderer::GetRendererInstance()->setVertexBuffer( 0, g_pParticleDrawFrom); // ???
 
        // Point to the correct output buffer
        IRenderer::GetRendererInstance()->setVertexBuffer( 0, g_pParticleStreamTo); // ???
        
        // draw
        TECHNIQUE_DESC techDesc;
        g_pTechniqueAdvanceRain->GetDesc( &techDesc );
        g_pTechniqueAdvanceRain->GetPassByIndex(0)->Apply(0);

        IRenderer::GetRendererInstance()->Draw(PRIM_POINTS, g_numRainVertices , 0 );

        // Get back to normal
        // ??? pBuffers[0] = 0;IRenderer::GetRendererInstance()->SOSetTargets( 1, pBuffers, offset );

        // Swap buffers
        VertexBufferID  pTemp = g_pParticleDrawFrom;
        g_pParticleDrawFrom = g_pParticleStreamTo;
        g_pParticleStreamTo = pTemp;
    
        firstFrame = false;

    }

    //-------------------------------------------------------------------------------------
    //draw rain
    //-------------------------------------------------------------------------------------


    if(g_bDrawParticles)
    {
        D3DXFROMWINEMatrixIdentity( &g_WorldMatrix );
        D3DXFROMWINEMatrixMultiply( &WorldViewMatrix, &g_WorldMatrix, &ViewMatrix );
        D3DXFROMWINEMatrixMultiply( &WorldViewProjMatrix, &WorldViewMatrix, &g_Proj );
        g_pWorldShaderVariable->SetMatrix((float*)&g_WorldMatrix);
        g_pWorldViewProjShaderVariable->SetMatrix( (float*)&WorldViewProjMatrix );
        g_pWorldViewShaderVariable->SetMatrix( (float*)&WorldViewMatrix );

        //IRenderer::GetRendererInstance()->setVertexFormat( g_pVertexLayoutRainVertex );
        IRenderer::GetRendererInstance()->setVertexBuffer( 0, g_pParticleDrawFrom);
        
        if(g_bUseCheapShader)
            g_pTechniqueDrawRainCheap->GetPassByIndex( 0 )->Apply(0);
        else
            g_pTechniqueDrawRain->GetPassByIndex( 0 )->Apply(0);

        IRenderer::GetRendererInstance()->Draw( PRIM_POINTS, int(g_numRainVertices*g_DrawFraction) , 0 );
    }
    IRenderer::GetRendererInstance()->changeToMainFramebuffer();
}

//--------------------------------------------------------------------------------------
// Helper functions
//--------------------------------------------------------------------------------------



int loadTextureFromFile(std::string file,std::string shaderTextureName)
{
    int hr;
    //texLoadInfo.Format = FORMAT_RGBA16_UNORM;
    TextureID texture=IRenderer::GetRendererInstance()->addImageLibTexture(file.c_str(), true, IRenderer::GetRendererInstance()->Getlinear());
    return 0;
}

int loadCubemapTextureFromFile(std::string file,std::string shaderTextureName)
{
    int hr;
    TextureID texture=IRenderer::GetRendererInstance()->addImageLibTexture(file.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
    stx_Effect::stx_Variable* textureRVar = g_pEffect->GetVariableByName( shaderTextureName.c_str() );
    textureRVar->SetResource( texture );
    return 0;
}

int load3DTextureFromFile(std::string file,std::string shaderTextureName)
{
    int hr;
    TextureID texture=IRenderer::GetRendererInstance()->addImageLibTexture(file.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
    stx_Effect::stx_Variable* textureRVar = g_pEffect->GetVariableByName( shaderTextureName.c_str() );
    textureRVar->SetResource( texture );
    return 0;
}


//--------------------------------------------------------------------------------------
// LoadTextureArray loads a texture array and associated view from a series
// of textures on disk.
//--------------------------------------------------------------------------------------
int LoadTextureArray( char* sTexturePrefix, unsigned int iNumTextures, std::vector<TextureID> vTex)
{
    int hr = 0;
    char str[MAX_PATH];
    for(int i=0; i<iNumTextures; i++)
    {
        stx_snprintf(str, MAX_PATH, "%s%04d.dds", sTexturePrefix, i);
    	vTex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture(str, true, IRenderer::GetRendererInstance()->Getlinear()));
        // ??? desc.ArraySize = iNumTextures;
    }
    return hr;
}

//-----------------------------------------------------------------------------------------------
//load LUTS as textures
//-----------------------------------------------------------------------------------------------

int loadLUTS(char* fileName, std::string shaderTextureName, int xRes, int yRes)
{
    int hr = 0;

    ifstream infile (fileName ,ios::in);
    if (infile.is_open())
    {
	Image3 img;
	float* data=(float*)img.create(FORMAT_R32f, xRes, yRes, 1, 1);
        int index = 0;
        char tempc;
        for(int j=0;j<yRes;j++)
        {   for(int i=0;i<xRes-1;i++)  
               infile>>data[index++]>>tempc;
            infile>>data[index++];
            
        }
	TextureID texture = IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());
        stx_Effect::stx_Variable* textureRVar = g_pEffect->GetVariableByName( shaderTextureName.c_str() );
        textureRVar->SetResource( texture );

        delete[] data;
    }
    else
       hr = 1;
    return hr;
}

//----------------------------------------------------------------------------------------
//random numbers; replace with a better quality generator for better particle distribution
//----------------------------------------------------------------------------------------

float random()
{
    return (float(   (double)rand() / ((double)(RAND_MAX)+(double)(1)) ));
}

//----------------------------------------------------------------------------------------
//function to reset all the controls to preset values
//----------------------------------------------------------------------------------------
void resetVariablesToPreset1()
{
     //set all variables to preset values
    g_bRenderBg = true;
    g_bMoveParticles = true;
    g_bDrawParticles = true;
    g_bUseCheapShader = false;
    g_bUseSpotLight = true;
    g_fogVector = g_constFogVector1;
    g_dirLightIntensity = g_constDirLightIntensity1;
    g_responseDirLight = g_constResponseDirLight1;
    g_PointLightIntensity = g_constPointLightIntensity1;
    g_cosSpotLight = g_constCosSpotLight1;
    g_responsePointLight = g_constResponsePointLight1;
    g_DrawFraction = g_constDrawFraction1;
    g_WindAmount = g_constWindAmount1;
     
     //update variables in the shader
    g_pRenderBGShaderVariable->SetInt(g_bRenderBg?1:0);
    g_pMoveParticlesShaderVariable->SetInt(g_bMoveParticles?1:0);
    g_UseSpotLightShaderVariable->SetBool(g_bUseSpotLight);
    g_pFogThicknessShaderVariable->SetFloatVector((float*)&g_fogVector);
    g_pDirLightIntensityShaderVariable->SetFloat(g_dirLightIntensity);
    g_pResponseDirLightShaderVariable->SetFloat(g_responseDirLight);
    g_pPointLightIntensityShaderVariable->SetFloat(g_PointLightIntensity);
    g_CosSpotLightShaderVariable->SetFloat(g_cosSpotLight);
    g_pResponsePointLightShaderVariable->SetFloat(g_responsePointLight);
    setShadingParametersBasedOnRain();

    g_lightDirectionWidget = g_directionalLightVector1;

    //set the camera back to where it was 
    g_vecEye = g_constVecEye1;
    g_vecAt = g_constAtVec1;
	D3DXFROMWINEVECTOR3 vecUp(0.0f, 1.0f, 0.0f);
    D3DXFROMWINEMatrixLookAtLH(&g_View, &g_vecEye, &g_vecAt, &vecUp );
}


void resetVariablesToPreset2()
{
    //set all variables to preset values
    g_bRenderBg = true;
    g_bMoveParticles = true;
    g_bDrawParticles = true;
    g_bUseCheapShader = false;
    g_bUseSpotLight = true;
    g_fogVector = g_constFogVector2;
    g_dirLightIntensity = g_constDirLightIntensity2;
    g_responseDirLight = g_constResponseDirLight2;
    g_PointLightIntensity = g_constPointLightIntensity2;
    g_cosSpotLight = g_constCosSpotLight2;
    g_responsePointLight = g_constResponsePointLight2;
    g_DrawFraction = g_constDrawFraction2;
    g_WindAmount = g_constWindAmount2;
     
    //update variables in the shader
    g_pRenderBGShaderVariable->SetInt(g_bRenderBg?1:0);
    g_pMoveParticlesShaderVariable->SetInt(g_bMoveParticles?1:0);
    g_UseSpotLightShaderVariable->SetBool(g_bUseSpotLight);
    g_pFogThicknessShaderVariable->SetFloatVector((float*)&g_fogVector);
    g_pDirLightIntensityShaderVariable->SetFloat(g_dirLightIntensity);
    g_pResponseDirLightShaderVariable->SetFloat(g_responseDirLight);
    g_pPointLightIntensityShaderVariable->SetFloat(g_PointLightIntensity);
    g_CosSpotLightShaderVariable->SetFloat(g_cosSpotLight);
    g_pResponsePointLightShaderVariable->SetFloat(g_responsePointLight);
    setShadingParametersBasedOnRain();

    g_lightDirectionWidget = g_directionalLightVector1;

    //set the camera back to where it was 
    g_vecEye = g_constVecEye2;
    g_vecAt = g_constAtVec2;
	D3DXFROMWINEVECTOR3 vecUp(0.0f, 1.0f, 0.0f);
    D3DXFROMWINEMatrixLookAtLH(&g_View, &g_vecEye, &g_vecAt, &vecUp );
}


void setShadingParametersBasedOnRain()
{
    //dry
    float DryKd = 1;
    float DryKsPoint = 0;
    float DryKsDir = 0;
    float DryspecPower = 5;
    //wet
    float WetKd = 0.4;
    float WetKsPoint = 2;
    float WetKsDir = 2;
    float WetspecPower = 100;

    g_KdShaderVariable->SetFloat( DryKd + g_DrawFraction*(WetKd-DryKd) );
    g_KsPointShaderVariable->SetFloat( DryKsPoint + g_DrawFraction*(WetKsPoint-DryKsPoint) ); 
    g_KsDirShaderVariable->SetFloat( DryKsDir + g_DrawFraction*(WetKsDir-DryKsDir) );
    g_SpecPowerShaderVariable->SetFloat( DryspecPower + g_DrawFraction*(WetspecPower-DryspecPower) );
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
	IRenderer* r=IRenderer::GetRendererInstance("Rain");
	//if(std::string(r->getString())!="D3D11") stx_exit(11);
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	STX_PRINT("Function init finished!\n");
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

