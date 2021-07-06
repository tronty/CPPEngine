//--------------------------------------------------------------------------------------
// File: DeferredParticles.cpp
//
// This sample shows a method of rendering deferred particles systems.  This is a variation
// of the method of deferred particle shading shown at unity3d.com/blogs/nf/files/page0_blog_entry73_1.pdf
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include <Framework3/IRenderer.h>
#include <GUI/GUIUtils.h>
#include <FW3.h>

#include "ParticleSystem.h"
#include "BreakableWall.h"

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
UINT NumActiveSystems;
D3DXFROMWINEMATRIX* pmData;

    D3DXFROMWINEMATRIX mSceneWorld, mSceneWVP;
    D3DXFROMWINEVECTOR4 vLightDir;
    D3DXFROMWINEMATRIX mInvViewProj;
    D3DXFROMWINEVECTOR3 vEyePt;
    D3DXFROMWINEMATRIX mViewProj;
    double fTime;
PARTICLE_VERTEX* pVerts = 0;

D3DXFROMWINEVECTOR3 vecEye( 0.0f, 150.0f, 336.0f );
D3DXFROMWINEVECTOR3 vecAt ( 0.0f, 0.0f, 0.0f );
//D3DXFROMWINEMATRIX g_Camera;
D3DXFROMWINEMATRIX mWorld;
D3DXFROMWINEMATRIX mView;
D3DXFROMWINEMATRIX mProj;

MeshRenderer2 g_WallMesh;
MeshRenderer2 g_ChunkMesh[NUM_CHUNKS];

#define MAX_BUILDINGS 1
CBuilding g_Building[MAX_BUILDINGS];
std::vector <D3DXFROMWINEMATRIX> g_BaseMeshMatrices;
std::vector <D3DXFROMWINEMATRIX> g_ChunkMeshMatrices[NUM_CHUNKS];

ShaderID g_pRenderParticlesDeferredSHD = -1;
VertexFormatID g_pRenderParticlesDeferredVF = -1;
ShaderID g_pRenderParticlesSHD = -1;
VertexFormatID g_pRenderParticlesVF = -1;
ShaderID g_pCompositeParticlesSHD = -1;
VertexFormatID g_pCompositeParticlesVF = -1;
ShaderID g_pRenderMeshInstSHD = -1;
VertexFormatID g_pRenderMeshInstVF = -1;
ShaderID g_pRenderMeshSHD = -1;
VertexFormatID g_pRenderMeshVF = -1;

bool supportsHalf=false;

TextureID g_pParticleTexture = -1;

UINT                                g_NumParticles = 200;
float                               g_fSpread = 4.0f;
float                               g_fStartSize = 0.0f;
float                               g_fEndSize = 10.0f;
float                               g_fSizeExponent = 128.0f;

float                               g_fMushroomCloudLifeSpan = 10.0f;
float                               g_fGroundBurstLifeSpan = 9.0f;
float                               g_fPopperLifeSpan = 9.0f;


float                               g_fMushroomStartSpeed = 20.0f;
float                               g_fStalkStartSpeed = 50.0f;
float                               g_fGroundBurstStartSpeed = 100.0f;
float                               g_fLandMineStartSpeed = 250.0f;

float                               g_fEndSpeed = 4.0f;
float                               g_fSpeedExponent = 32.0f;
float                               g_fFadeExponent = 4.0f;
float                               g_fRollAmount = 0.2f;
float                               g_fWindFalloff = 20.0f;
D3DXFROMWINEVECTOR3                         g_vPosMul( 1,1,1 );
D3DXFROMWINEVECTOR3                         g_vDirMul( 1,1,1 );
D3DXFROMWINEVECTOR3                         g_vWindVel( -2.0f,10.0f,0 );
D3DXFROMWINEVECTOR3                         g_vGravity( 0,-9.8f,0.0f );

float                               g_fGroundPlane = 0.5f;
float                               g_fLightRaise = 1.0f;

float                               g_fWorldBounds = 100.0f;

#define MAX_FLASH_COLORS 4
D3DXFROMWINEVECTOR4 g_vFlashColor[MAX_FLASH_COLORS] =
{
    D3DXFROMWINEVECTOR4( 1.0f, 0.5f, 0.00f, 0.9f ),
    D3DXFROMWINEVECTOR4( 1.0f, 0.3f, 0.05f, 0.9f ),
    D3DXFROMWINEVECTOR4( 1.0f, 0.4f, 0.00f, 0.9f ),
    D3DXFROMWINEVECTOR4( 0.8f, 0.3f, 0.05f, 0.9f )
};

D3DXFROMWINEVECTOR4                         g_vFlashAttenuation( 0,0.0f,3.0f,0 );
D3DXFROMWINEVECTOR4                         g_vMeshLightAttenuation( 0,0,1.5f,0 );
float                               g_fFlashLife = 0.50f;
float                               g_fFlashIntensity = 1000.0f;

UINT                                g_NumParticlesToDraw = 0;
/*#define MAX_MUSHROOM_CLOUDS 16
   #define MAX_GROUND_BURSTS 46
   #define MAX_PARTICLE_SYSTEMS 60
   #define MAX_FLASH_LIGHTS 8
   #define MAX_INSTANCES 200*/
#define MAX_MUSHROOM_CLOUDS 8
#define MAX_GROUND_BURSTS 23
#define MAX_PARTICLE_SYSTEMS 30
#define MAX_FLASH_LIGHTS 8
#define MAX_INSTANCES 200

CParticleSystem**                   g_ppParticleSystem = 0;
VertexBufferID                       g_pParticleBuffer = -1;

TextureID                    g_pOffscreenParticleTex = -1;
TextureID                    g_pOffscreenParticleColorTex = -1;
TextureID g_ptxDiffuse = -1;
TextureID g_ptxParticleColor = -1;
D3DXFROMWINEVECTOR4         g_pLightDir;
D3DXFROMWINEMATRIX         g_pmWorldViewProjection;
D3DXFROMWINEMATRIX         g_pmWorld;
D3DXFROMWINEMATRIX         g_pmInvViewProj;
float         g_pfTime;
D3DXFROMWINEVECTOR4         g_pvEyePt;
D3DXFROMWINEVECTOR4         g_pvRight;
D3DXFROMWINEVECTOR4         g_pvUp;
D3DXFROMWINEVECTOR4         g_pvForward;

float         g_pNumGlowLights;
D3DXFROMWINEVECTOR4         g_pvGlowLightPosIntensity[MAX_PARTICLE_SYSTEMS];
D3DXFROMWINEVECTOR4         g_pvGlowLightColor[MAX_PARTICLE_SYSTEMS];
D3DXFROMWINEVECTOR4         g_pvGlowLightAttenuation;
D3DXFROMWINEVECTOR4         g_pvMeshLightAttenuation;

D3DXFROMWINEMATRIX         g_pmViewProj;
D3DXFROMWINEMATRIX         g_pmWorldInst;

bool                       g_bRenderDeferred = false;

D3DXFROMWINEVECTOR3 g_LightControl;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_RESET				50
#define IDC_DEFERRED			51
#define IDC_TOGGLEWARP          5

class DeferredParticles : public STXGUI {
public:
	DeferredParticles()
	{
	}
	virtual ~DeferredParticles()
	{
	}
	virtual void actionPerformed(GUIEvent &evt)
	{
  const std::string &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
	GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
	if(radioButton->isPressed() && radioButton->isChecked())
	{
		if(callbackString == "RenderDeferred") 
			g_bRenderDeferred = !g_bRenderDeferred;
	}
  }
  else if(widgetType == BUTTON)
  {
	GUIButton   *button = (GUIButton*)sourceRectangle;
	if(button->isPressed())
	{
		if(callbackString == "ResetBuildings") 
			ResetBuildings();
	}
  }
}
//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
void InitApp()
{
    D3DXFROMWINEVECTOR3 vDir( 1,1,0 );
    D3DXFROMWINEVec3Normalize( &vDir, &vDir );
    g_LightControl=vDir;

#if 0
    // Setup the camera's view parameters
    //D3DXFROMWINEVECTOR3 vecEye( 0.0f, 150.0f, 336.0f );
    //D3DXFROMWINEVECTOR3 vecAt ( 0.0f, 0.0f, 0.0f );
    g_Camera.SetViewParams( &vecEye, &vecAt );
#else
	D3DXFROMWINEVECTOR3 vecUp ( 0.0f, 1.0f, 0.0f );
    D3DXFROMWINEMatrixLookAtLH(&mView, &vecEye, &vecAt, &vecUp);
    D3DXFROMWINEMatrixIdentity( &mWorld );
    D3DXFROMWINEMatrixIdentity( &mProj );
#endif
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void OnFrameMove( double fTime, float fElapsedTime )
{
    // Update the camera's position based on user input
    //g_Camera.FrameMove( fElapsedTime );
    if (fElapsedTime  > 0.1f ) fElapsedTime = 0.1f;

    D3DXFROMWINEVECTOR3 vEye;
    //D3DXFROMWINEMATRIX mView;
    vEye = vecEye;//*g_Camera.GetEyePt();
    //mView = *g_Camera.GetViewMatrix();
    D3DXFROMWINEVECTOR3 vRight( mView.index(0,0), mView.index(1,0), mView.index(2,0) );
    D3DXFROMWINEVECTOR3 vUp( mView.index(0,1), mView.index(1,1), mView.index(2,1) );
    D3DXFROMWINEVECTOR3 vFoward( mView.index(0,2), mView.index(1,2), mView.index(2,2) );

    D3DXFROMWINEVec3Normalize( &vRight, &vRight );
    D3DXFROMWINEVec3Normalize( &vUp, &vUp );
    D3DXFROMWINEVec3Normalize( &vFoward, &vFoward );

    g_pvRight=D3DXFROMWINEVECTOR4(vRight.x, vRight.y, vRight.z, 0.0f );
    g_pvUp=D3DXFROMWINEVECTOR4(vUp.x, vUp.y, vUp.z, 0.0f );
    g_pvForward=D3DXFROMWINEVECTOR4(vFoward.x, vFoward.y, vFoward.z, 0.0f );

    NumActiveSystems = 0;
    D3DXFROMWINEVECTOR4 g_pvGlowLightPosIntensity[MAX_PARTICLE_SYSTEMS];
    D3DXFROMWINEVECTOR4 g_pvGlowLightColor[MAX_PARTICLE_SYSTEMS];

    // Advanced building pieces
    for( UINT i = 0; i < MAX_BUILDINGS; i++ )
    {
        g_Building[i].AdvancePieces( fElapsedTime, g_vGravity );
    }

    // Advance the system
    for( UINT i = 0; i < MAX_PARTICLE_SYSTEMS; i++ )
    {
        g_ppParticleSystem[i]->AdvanceSystem( ( float )fTime, fElapsedTime, vRight, vUp, g_vWindVel, g_vGravity );
    }

    PARTICLE_VERTEX* pVerts = (PARTICLE_VERTEX*)IRenderer::GetRendererInstance()->lockVertexBuffer(g_pParticleBuffer);

    CopyParticlesToVertexBuffer( pVerts, vEye, vRight, vUp );

    IRenderer::GetRendererInstance()->unlockVertexBuffer(g_pParticleBuffer);

    for( UINT i = 0; i < MAX_MUSHROOM_CLOUDS; i += 2 )
    {
        float fCurrentTime = g_ppParticleSystem[i]->GetCurrentTime();
        float fLifeSpan = g_ppParticleSystem[i]->GetLifeSpan();
        if( fCurrentTime > fLifeSpan )
        {
            D3DXFROMWINEVECTOR3 vCenter;
            vCenter.x = RPercent() * g_fWorldBounds;
            vCenter.y = g_fGroundPlane;
            vCenter.z = RPercent() * g_fWorldBounds;
            float fStartTime = -fabs( RPercent() ) * 4.0f;
            D3DXFROMWINEVECTOR4 vFlashColor = g_vFlashColor[ stx_rand() % MAX_FLASH_COLORS ];

            g_ppParticleSystem[i]->SetCenter( vCenter );
            g_ppParticleSystem[i]->SetStartTime( fStartTime );
            g_ppParticleSystem[i]->SetFlashColor( vFlashColor );
            g_ppParticleSystem[i]->Init();

            g_ppParticleSystem[i + 1]->SetCenter( vCenter );
            g_ppParticleSystem[i + 1]->SetStartTime( fStartTime );
            g_ppParticleSystem[i + 1]->SetFlashColor( vFlashColor );
            g_ppParticleSystem[i + 1]->Init();
        }
        else if( fCurrentTime > 0.0f && fCurrentTime < g_fFlashLife && NumActiveSystems < MAX_FLASH_LIGHTS )
        {
            D3DXFROMWINEVECTOR3 vCenter = g_ppParticleSystem[i]->GetCenter();
            D3DXFROMWINEVECTOR4 vFlashColor = g_ppParticleSystem[i]->GetFlashColor();

            float fIntensity = g_fFlashIntensity * ( ( g_fFlashLife - fCurrentTime ) / g_fFlashLife );
            g_pvGlowLightPosIntensity[NumActiveSystems] = D3DXFROMWINEVECTOR4( vCenter.x, vCenter.y + g_fLightRaise, vCenter.z,
                                                                    fIntensity );
            g_pvGlowLightColor[NumActiveSystems] = vFlashColor;
            NumActiveSystems ++;
        }
    }

    // Ground bursts
    for( UINT i = MAX_MUSHROOM_CLOUDS; i < MAX_GROUND_BURSTS; i++ )
    {
        float fCurrentTime = g_ppParticleSystem[i]->GetCurrentTime();
        float fLifeSpan = g_ppParticleSystem[i]->GetLifeSpan();
        if( fCurrentTime > fLifeSpan )
        {
            D3DXFROMWINEVECTOR3 vCenter;
            vCenter.x = RPercent() * g_fWorldBounds;
            vCenter.y = g_fGroundPlane;
            vCenter.z = RPercent() * g_fWorldBounds;
            float fStartTime = -fabs( RPercent() ) * 4.0f;
            D3DXFROMWINEVECTOR4 vFlashColor = g_vFlashColor[ stx_rand() % MAX_FLASH_COLORS ];

            float fStartSpeed = g_fGroundBurstStartSpeed + RPercent() * 30.0f;
            g_ppParticleSystem[i]->SetCenter( vCenter );
            g_ppParticleSystem[i]->SetStartTime( fStartTime );
            g_ppParticleSystem[i]->SetStartSpeed( fStartSpeed );
            g_ppParticleSystem[i]->SetFlashColor( vFlashColor );
            g_ppParticleSystem[i]->Init();
        }
        else if( fCurrentTime > 0.0f && fCurrentTime < g_fFlashLife && NumActiveSystems < MAX_FLASH_LIGHTS )
        {
            D3DXFROMWINEVECTOR3 vCenter = g_ppParticleSystem[i]->GetCenter();
            D3DXFROMWINEVECTOR4 vFlashColor = g_ppParticleSystem[i]->GetFlashColor();

            float fIntensity = g_fFlashIntensity * ( ( g_fFlashLife - fCurrentTime ) / g_fFlashLife );
            g_pvGlowLightPosIntensity[NumActiveSystems] = D3DXFROMWINEVECTOR4( vCenter.x, vCenter.y + g_fLightRaise, vCenter.z,
                                                                    fIntensity );
            g_pvGlowLightColor[NumActiveSystems] = vFlashColor;
            NumActiveSystems ++;
        }
    }

    // Land mines
    for( UINT i = MAX_GROUND_BURSTS; i < MAX_PARTICLE_SYSTEMS; i++ )
    {
        float fCurrentTime = g_ppParticleSystem[i]->GetCurrentTime();
        float fLifeSpan = g_ppParticleSystem[i]->GetLifeSpan();
        if( fCurrentTime > fLifeSpan )
        {
            D3DXFROMWINEVECTOR3 vCenter;
            vCenter.x = RPercent() * g_fWorldBounds;
            vCenter.y = g_fGroundPlane;
            vCenter.z = RPercent() * g_fWorldBounds;
            float fStartTime = -fabs( RPercent() ) * 4.0f;
            D3DXFROMWINEVECTOR4 vFlashColor = g_vFlashColor[ stx_rand() % MAX_FLASH_COLORS ];

            float fStartSpeed = g_fLandMineStartSpeed + RPercent() * 100.0f;
            g_ppParticleSystem[i]->SetCenter( vCenter );
            g_ppParticleSystem[i]->SetStartTime( fStartTime );
            g_ppParticleSystem[i]->SetStartSpeed( fStartSpeed );
            g_ppParticleSystem[i]->SetFlashColor( vFlashColor );
            g_ppParticleSystem[i]->Init();
        }
        else if( fCurrentTime > 0.0f && fCurrentTime < g_fFlashLife && NumActiveSystems < MAX_FLASH_LIGHTS )
        {
            D3DXFROMWINEVECTOR3 vCenter = g_ppParticleSystem[i]->GetCenter();
            D3DXFROMWINEVECTOR4 vFlashColor = g_ppParticleSystem[i]->GetFlashColor();

            float fIntensity = g_fFlashIntensity * ( ( g_fFlashLife - fCurrentTime ) / g_fFlashLife );
            g_pvGlowLightPosIntensity[NumActiveSystems] = D3DXFROMWINEVECTOR4( vCenter.x, vCenter.y + g_fLightRaise, vCenter.z,
                                                                    fIntensity );
            g_pvGlowLightColor[NumActiveSystems] = vFlashColor;
            NumActiveSystems ++;
        }
    }
}

void ResetBuildings()
{
    float f2Third = 0.6666f;
    D3DXFROMWINEVECTOR3 vChunkOffsets[NUM_CHUNKS] =
    {/*
        D3DXFROMWINEVECTOR3( f2Third, -f2Third, 0.0f ),
        D3DXFROMWINEVECTOR3( -f2Third, f2Third, 0.0f ),
        D3DXFROMWINEVECTOR3( f2Third, f2Third, 0.0f ),
        D3DXFROMWINEVECTOR3( -f2Third, -f2Third, 0.0f ),
        D3DXFROMWINEVECTOR3( f2Third, 0, 0.0f ),
        D3DXFROMWINEVECTOR3( 0, f2Third, 0.0f ),
        D3DXFROMWINEVECTOR3( -f2Third, 0, 0.0f ),
        D3DXFROMWINEVECTOR3( 0, -f2Third, 0.0f ), */
        D3DXFROMWINEVECTOR3( 0, 0, 0.0f )
    };

    for( UINT i = 0; i < MAX_BUILDINGS; i++ )
    {
        g_Building[i].SetBaseMesh( &g_WallMesh );
        for( UINT c = 0; c < NUM_CHUNKS; c++ )
        {
            g_Building[i].SetChunkMesh( c, &g_ChunkMesh[c], vChunkOffsets[c] );
        }
    }
}

void setShaderConstants()
{
    // Obtain variables
    IRenderer::GetRendererInstance()->setTexture("g_txMeshTexture", g_ptxDiffuse);
    IRenderer::GetRendererInstance()->setTexture("g_txParticleColor", g_ptxParticleColor);
    IRenderer::GetRendererInstance()->setShaderConstant4f("g_LightDir", g_pLightDir);
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorldViewProjection", g_pmWorldViewProjection);

    IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorld", g_pmWorld);
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mInvViewProj", g_pmInvViewProj);
    IRenderer::GetRendererInstance()->setShaderConstant1f("g_fTime", g_pfTime);
    IRenderer::GetRendererInstance()->setShaderConstant4f("g_vEyePt", g_pvEyePt);
    IRenderer::GetRendererInstance()->setShaderConstant4f("g_vRight", g_pvRight);
    IRenderer::GetRendererInstance()->setShaderConstant4f("g_vUp", g_pvUp);
    IRenderer::GetRendererInstance()->setShaderConstant4f("g_vForward", g_pvForward);

    IRenderer::GetRendererInstance()->setShaderConstant1i("g_NumGlowLights", g_pNumGlowLights);

    IRenderer::GetRendererInstance()->setShaderConstantArray4f("g_vGlowLightPosIntensity", g_pvGlowLightPosIntensity, MAX_PARTICLE_SYSTEMS);
    IRenderer::GetRendererInstance()->setShaderConstantArray4f("g_vGlowLightColor", g_pvGlowLightColor, MAX_PARTICLE_SYSTEMS);

    IRenderer::GetRendererInstance()->setShaderConstant4f("g_vGlowLightAttenuation", g_pvGlowLightAttenuation);
    IRenderer::GetRendererInstance()->setShaderConstant4f("g_vMeshLightAttenuation", g_pvMeshLightAttenuation);

    IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorldInst", g_pmWorldInst);
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mViewProj", g_pmViewProj);

    // Setup light variables
    IRenderer::GetRendererInstance()->setShaderConstant1i("NumGlowLights", NumActiveSystems );
    IRenderer::GetRendererInstance()->setShaderConstant4f("GlowLightAttenuation", g_vFlashAttenuation );
    IRenderer::GetRendererInstance()->setShaderConstant4f("MeshLightAttenuation", g_vMeshLightAttenuation );

    // Per frame variables
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorldViewProjection", mSceneWVP);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorld", mSceneWorld);
	IRenderer::GetRendererInstance()->setShaderConstant4f("g_pLightDir", vLightDir );
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mInvViewProj", mInvViewProj );
	IRenderer::GetRendererInstance()->setShaderConstant1f("g_pfTime", ( float )fTime );
	IRenderer::GetRendererInstance()->setShaderConstant3f("g_pvEyePt", vEyePt );
	//IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mViewProj", mViewProj );
}
void CreateBox(MeshRenderer2& aMesh, bool aBox=true)
{
	if(aBox)
    		aMesh.CreateTexturedBox(1.0f, 1.0f, 1.0f, eShaderNone);
    		//aMesh.CreateBox(1.0f, 1.0f, 1.0f, eShaderNone);
	else
		aMesh.CreateTexturedQuad(1.0f, 1.0f, eShaderNone);
}
//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
virtual int init(const char* aTitle)
{
    INT hr;

    //STXGUI::init("/GUIVF.xml");
/*
    // Read the D3DXFROMWINE effect file
    WCHAR str[MAX_PATH];
    char strMaxGlowLights[MAX_PATH];
    char strMaxInstances[MAX_PATH];
    sprintf_s( strMaxGlowLights, MAX_PATH, "%d", MAX_FLASH_LIGHTS );
    sprintf_s( strMaxInstances, MAX_PATH, "%d", MAX_INSTANCES );
    D3D10_SHADER_MACRO macros[3] =
    {
        { "MAX_GLOWLIGHTS", strMaxGlowLights },
        { "MAX_INSTANCES", strMaxInstances },
        { 0, 0 }
    };
*/
    // Obtain technique objects
/*
VertexFormatID                   g_pVertexLayout = NULL; 3 2 2 4
VertexFormatID                   g_pScreenQuadLayout = NULL; 3
VertexFormatID                   g_pMeshLayout = NULL; 3 3 1
*/
    g_pRenderParticlesDeferredSHD = IRenderer::GetRendererInstance()->addShaderFromFile("/DeferredParticles/DeferredParticles.shd", "RenderParticlesVS", "RenderParticlesDeferredPS");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,//TextureUV
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,//fLife,fRot
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4 //Color
	};
	g_pRenderParticlesDeferredVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), g_pRenderParticlesDeferredSHD);

	    g_pRenderParticlesSHD = IRenderer::GetRendererInstance()->addShaderFromFile("/DeferredParticles/DeferredParticles.shd", "RenderParticlesVS", "RenderParticlesPS");
	g_pRenderParticlesVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), g_pRenderParticlesSHD);

    g_pCompositeParticlesSHD = IRenderer::GetRendererInstance()->addShaderFromFile("/DeferredParticles/DeferredParticles.shd", "CompositeParticlesVS", "CompositeParticlesPS");
#if 0
	FormatDesc format3[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3
	};
	g_pCompositeParticlesVF = IRenderer::GetRendererInstance()->addVertexFormat(format3, elementsOf(format3), g_pCompositeParticlesSHD);
    g_pRenderMeshSHD = IRenderer::GetRendererInstance()->addShaderFromFile("/DeferredParticles/DeferredParticles.shd", "MeshVS", "MeshPS");
	FormatDesc format4[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 1
	};
	g_pRenderMeshVF = IRenderer::GetRendererInstance()->addVertexFormat(format4, elementsOf(format4), g_pRenderMeshSHD);
    g_pRenderMeshInstSHD = IRenderer::GetRendererInstance()->addShaderFromFile("/DeferredParticles/DeferredParticles.shd", "MeshInstVS", "MeshPS");
	FormatDesc format5[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	g_pRenderMeshInstVF = IRenderer::GetRendererInstance()->addVertexFormat(format5, elementsOf(format5), g_pRenderMeshInstSHD);
#endif
/*
    // Obtain variables
    g_ptxDiffuse = g_pEffect10->GetVariableByName( "g_txMeshTexture" )->AsShaderResource();
    g_ptxParticleColor = g_pEffect10->GetVariableByName( "g_txParticleColor" )->AsShaderResource();
    g_pLightDir = g_pEffect10->GetVariableByName( "g_LightDir" )->AsVector();
    g_pmWorldViewProjection = g_pEffect10->GetVariableByName( "g_mWorldViewProjection" )->AsMatrix();
    g_pmWorld = g_pEffect10->GetVariableByName( "g_mWorld" )->AsMatrix();
    g_pmInvViewProj = g_pEffect10->GetVariableByName( "g_mInvViewProj" )->AsMatrix();
    g_pfTime = g_pEffect10->GetVariableByName( "g_fTime" )->AsScalar();
    g_pvEyePt = g_pEffect10->GetVariableByName( "g_vEyePt" )->AsVector();
    g_pvRight = g_pEffect10->GetVariableByName( "g_vRight" )->AsVector();
    g_pvUp = g_pEffect10->GetVariableByName( "g_vUp" )->AsVector();
    g_pvForward = g_pEffect10->GetVariableByName( "g_vForward" )->AsVector();

    g_pNumGlowLights = g_pEffect10->GetVariableByName( "g_NumGlowLights" )->AsScalar();
    g_pvGlowLightPosIntensity = g_pEffect10->GetVariableByName( "g_pvGlowLightPosIntensity" )->AsVector();
    g_pvGlowLightColor = g_pEffect10->GetVariableByName( "g_pvGlowLightColor" )->AsVector();
    g_pvGlowLightAttenuation = g_pEffect10->GetVariableByName( "g_vGlowLightAttenuation" )->AsVector();
    g_pvMeshLightAttenuation = g_pEffect10->GetVariableByName( "g_vMeshLightAttenuation" )->AsVector();

    g_pmWorldInst = g_pEffect10->GetVariableByName( "g_mWorldInst" )->AsMatrix();
    g_pmViewProj = g_pEffect10->GetVariableByName( "g_mViewProj" )->AsMatrix();
*/

/*
    // Load the meshes
    ( g_WallMesh.Create( "/DeferredParticles/wallsegment.sdkmesh" ) );
    ( g_ChunkMesh[0].Create( "/DeferredParticles/wallchunk0.sdkmesh" ) );
    ( g_ChunkMesh[1].Create( "/DeferredParticles/wallchunk1.sdkmesh" ) );
    ( g_ChunkMesh[2].Create( "/DeferredParticles/wallchunk2.sdkmesh" ) );
    ( g_ChunkMesh[3].Create( "/DeferredParticles/wallchunk3.sdkmesh" ) );
    ( g_ChunkMesh[4].Create( "/DeferredParticles/wallchunk4.sdkmesh" ) );
    ( g_ChunkMesh[5].Create( "/DeferredParticles/wallchunk5.sdkmesh" ) );
    ( g_ChunkMesh[6].Create( "/DeferredParticles/wallchunk6.sdkmesh" ) );
    ( g_ChunkMesh[7].Create( "/DeferredParticles/wallchunk7.sdkmesh" ) );
    ( g_ChunkMesh[8].Create( "/DeferredParticles/wallchunk8.sdkmesh" ) );
*/
	CreateBox(g_WallMesh /* , false ??? */ );
	CreateBox(g_ChunkMesh[0]);
	CreateBox(g_ChunkMesh[1]);
	CreateBox(g_ChunkMesh[2]);
	CreateBox(g_ChunkMesh[3]);
	CreateBox(g_ChunkMesh[4]);
	CreateBox(g_ChunkMesh[5]);
	CreateBox(g_ChunkMesh[6]);
	CreateBox(g_ChunkMesh[7]);
	CreateBox(g_ChunkMesh[8]);
    // Buildings
    g_Building[0].CreateBuilding( D3DXFROMWINEVECTOR3( 0, 0, 0 ), 2.0f, 50, 0, 50 );

    float fBuildingRange = g_fWorldBounds - 20.0f;

    for( UINT i = 1; i < MAX_BUILDINGS; i++ )
    {
        D3DXFROMWINEVECTOR3 vCenter;
        vCenter.x = RPercent() * fBuildingRange;
        vCenter.y = 0;
        vCenter.z = RPercent() * fBuildingRange;

        UINT x = ( stx_rand() % 2 ) + 2;
        UINT y = ( stx_rand() % 2 ) + 3;
        UINT z = ( stx_rand() % 2 ) + 2;
        g_Building[i].CreateBuilding( vCenter, 2.0f, x * 2, y * 2, z * 2 );
    }

    ResetBuildings();

    // Particle system
    UINT NumStalkParticles = 500;
    UINT NumGroundExpParticles = 345;
    UINT NumLandMineParticles = 125;
    UINT MaxParticles = MAX_MUSHROOM_CLOUDS * ( g_NumParticles + NumStalkParticles ) +
        ( MAX_GROUND_BURSTS - MAX_MUSHROOM_CLOUDS ) * NumGroundExpParticles +
        ( MAX_PARTICLE_SYSTEMS - MAX_GROUND_BURSTS ) * NumLandMineParticles;
    ( CreateParticleArray( MaxParticles ) );

    D3DXFROMWINEVECTOR4 vColor0( 1.0f,1.0f,1.0f,1 );
    D3DXFROMWINEVECTOR4 vColor1( 0.6f,0.6f,0.6f,1 );

    stx_srand( timeGetTime() );
    g_ppParticleSystem = new CParticleSystem*[MAX_PARTICLE_SYSTEMS];
    g_NumParticlesToDraw = 0;
    for( UINT i = 0; i < MAX_MUSHROOM_CLOUDS; i += 2 )
    {
        D3DXFROMWINEVECTOR3 vLocation;
        vLocation.x = RPercent() * 50.0f;
        vLocation.y = g_fGroundPlane;
        vLocation.z = RPercent() * 50.0f;

        g_ppParticleSystem[i] = new CMushroomParticleSystem();
        g_ppParticleSystem[i]->CreateParticleSystem( g_NumParticles );
        g_ppParticleSystem[i]->SetSystemAttributes( vLocation,
                                                    g_fSpread, g_fMushroomCloudLifeSpan, g_fFadeExponent,
                                                    g_fStartSize, g_fEndSize, g_fSizeExponent,
                                                    g_fMushroomStartSpeed, g_fEndSpeed, g_fSpeedExponent,
                                                    g_fRollAmount, g_fWindFalloff,
                                                    1, 0, D3DXFROMWINEVECTOR3( 0, 0, 0 ), D3DXFROMWINEVECTOR3( 0, 0, 0 ),
                                                    vColor0, vColor1,
                                                    g_vPosMul, g_vDirMul );

        g_NumParticlesToDraw += g_NumParticles;

        g_ppParticleSystem[i + 1] = new CStalkParticleSystem();
        g_ppParticleSystem[i + 1]->CreateParticleSystem( NumStalkParticles );
        g_ppParticleSystem[i + 1]->SetSystemAttributes( vLocation,
                                                        15.0f, g_fMushroomCloudLifeSpan, g_fFadeExponent * 2.0f,
                                                        g_fStartSize * 0.5f, g_fEndSize * 0.5f, g_fSizeExponent,
                                                        g_fStalkStartSpeed, -1.0f, g_fSpeedExponent,
                                                        g_fRollAmount, g_fWindFalloff,
                                                        1, 0, D3DXFROMWINEVECTOR3( 0, 0, 0 ), D3DXFROMWINEVECTOR3( 0, 0, 0 ),
                                                        vColor0, vColor1,
                                                        D3DXFROMWINEVECTOR3( 1, 0.1f, 1 ), D3DXFROMWINEVECTOR3( 1, 0.1f, 1 ) );

        g_NumParticlesToDraw += NumStalkParticles;
    }

    for( UINT i = MAX_MUSHROOM_CLOUDS; i < MAX_GROUND_BURSTS; i++ )
    {
        D3DXFROMWINEVECTOR3 vLocation;
        vLocation.x = RPercent() * 50.0f;
        vLocation.y = g_fGroundPlane;
        vLocation.z = RPercent() * 50.0f;

        g_ppParticleSystem[i] = new CGroundBurstParticleSystem();
        g_ppParticleSystem[i]->CreateParticleSystem( NumGroundExpParticles );
        g_ppParticleSystem[i]->SetSystemAttributes( vLocation,
                                                    1.0f, g_fGroundBurstLifeSpan, g_fFadeExponent,
                                                    0.5f, 8.0f, 1.0f,
                                                    g_fGroundBurstStartSpeed, g_fEndSpeed, 4.0f,
                                                    g_fRollAmount, 1.0f,
                                                    30, 100.0f, D3DXFROMWINEVECTOR3( 0, 0.5f, 0 ), 		D3DXFROMWINEVECTOR3( 1.0f, 0.5f,
                                                                                                        1.0f ),
                                                    vColor0, vColor1,
                                                    g_vPosMul, g_vDirMul );

        g_NumParticlesToDraw += NumGroundExpParticles;
    }

    for( UINT i = MAX_GROUND_BURSTS; i < MAX_PARTICLE_SYSTEMS; i++ )
    {
        D3DXFROMWINEVECTOR3 vLocation;
        vLocation.x = RPercent() * 50.0f;
        vLocation.y = g_fGroundPlane;
        vLocation.z = RPercent() * 50.0f;

        g_ppParticleSystem[i] = new CLandMineParticleSystem();
        g_ppParticleSystem[i]->CreateParticleSystem( NumLandMineParticles );
        g_ppParticleSystem[i]->SetSystemAttributes( vLocation,
                                                    1.5f, g_fPopperLifeSpan, g_fFadeExponent,
                                                    1.0f, 6.0f, 1.0f,
                                                    g_fLandMineStartSpeed, g_fEndSpeed, 2.0f,
                                                    g_fRollAmount, 4.0f,
                                                    0, 70.0f, D3DXFROMWINEVECTOR3( 0, 0.8f, 0 ), D3DXFROMWINEVECTOR3( 0.3f, 0.2f,
                                                                                                      0.3f ),
                                                    vColor0, vColor1,
                                                    g_vPosMul, g_vDirMul );

        g_NumParticlesToDraw += NumGroundExpParticles;
    }

	g_pParticleBuffer=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( PARTICLE_VERTEX ) * 6 * g_NumParticlesToDraw, DYNAMIC);

	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	g_pParticleTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/DeferredParticles/DeferredParticle.png", false, ss);

    g_pOffscreenParticleColorTex = IRenderer::GetRendererInstance()->addRenderTarget(
		IRenderer::GetRendererInstance()->GetViewportWidth(),
		IRenderer::GetRendererInstance()->GetViewportHeight(),
		FORMAT_RGBA8, 
		IRenderer::GetRendererInstance()->Getlinear());
    g_pOffscreenParticleTex = IRenderer::GetRendererInstance()->addRenderTarget(
		IRenderer::GetRendererInstance()->GetViewportWidth(),
		IRenderer::GetRendererInstance()->GetViewportHeight(),
		FORMAT_RGBA16F, 
		IRenderer::GetRendererInstance()->Getlinear/*Aniso*/());

    return 0;
}

//--------------------------------------------------------------------------------------
// Render particles
//--------------------------------------------------------------------------------------
void RenderParticles( ShaderID pRenderTechnique, VertexFormatID pVertexVF )
{
    //IA setup
    IRenderer::GetRendererInstance()->setShader(pRenderTechnique);
    IRenderer::GetRendererInstance()->setVertexFormat( pVertexVF );
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, g_pParticleBuffer );
    IRenderer::GetRendererInstance()->setTexture("g_ptxDiffuse", g_pParticleTexture );
    setShaderConstants();

    //Render

    g_NumParticlesToDraw = GetNumActiveParticles();
        IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLES, 6 * g_NumParticlesToDraw, 0 );
}

//--------------------------------------------------------------------------------------
// Render particles into the offscreen buffer
//--------------------------------------------------------------------------------------
void RenderParticlesIntoBuffer( )
{
    IRenderer::GetRendererInstance()->changeRenderTarget(g_pOffscreenParticleColorTex, g_pOffscreenParticleTex);

    IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

    // Render the particles
    RenderParticles( g_pRenderParticlesSHD, g_pRenderParticlesVF );

    // restore the original render targets
    IRenderer::GetRendererInstance()->changeToMainFramebuffer();
}

//--------------------------------------------------------------------------------------
// Composite offscreen particle buffer back into the scene
//--------------------------------------------------------------------------------------
void CompositeParticlesIntoScene( )
{
    // Render the particles
    IRenderer::GetRendererInstance()->setShader( g_pCompositeParticlesSHD);
    IRenderer::GetRendererInstance()->setVertexFormat( g_pCompositeParticlesVF );

    IRenderer::GetRendererInstance()->setTexture("g_txMeshTexture", g_pOffscreenParticleTex );
    IRenderer::GetRendererInstance()->setTexture("g_txParticleColor", g_pOffscreenParticleColorTex );
    setShaderConstants();

    D3DXFROMWINEVECTOR3 verts[4] =
    {
        D3DXFROMWINEVECTOR3(  1.0f, -1.0f, 0.5f ),
        D3DXFROMWINEVECTOR3(  1.0f,  1.0f, 0.5f ),
        D3DXFROMWINEVECTOR3( -1.0f, -1.0f, 0.5f ),
        D3DXFROMWINEVECTOR3( -1.0f,  1.0f, 0.5f )
    };

    IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, verts, verts, sizeof(float3));
}

void RenderInstanced( MeshRenderer2& pMesh,
                      UINT NumInstances )
{
	D3DXFROMWINEMATRIX w;
	D3DXFROMWINEMatrixIdentity(&w);
	pMesh.BeginDraw(&w); // ???
	setShaderConstants();
	pMesh.EndDraw();
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
virtual void render()
{
    INT hr;
/*
    RenderInstanced
    RenderInstanced
    if( g_bRenderDeferred )
    {
        RenderParticlesIntoBuffer( );
        CompositeParticlesIntoScene( );
    }
    else
    {
        RenderParticles( g_pRenderParticlesSHD, g_pRenderParticlesVF );
    }
*/
	fTime=timeGetTime();
	float fElapsedTime=timeGetTime();
	OnFrameMove( fTime, fElapsedTime );

    IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
    IRenderer::GetRendererInstance()->BeginScene();

    D3DXFROMWINEMATRIX mWorldViewProjection;
    D3DXFROMWINEMATRIX mWorld;
    D3DXFROMWINEMATRIX mView;
    D3DXFROMWINEMATRIX mProj;

    // Get the projection & view matrix from the camera class
    D3DXFROMWINEMatrixIdentity( &mWorld );
    vEyePt = vecEye;//*g_Camera.GetEyePt();
    //mProj = *g_Camera.GetProjMatrix();
    //mView = *g_Camera.GetViewMatrix();

    mWorldViewProjection = mView * mProj;
    mViewProj = mView * mProj;
    D3DXFROMWINEMatrixInverse( &mInvViewProj, 0, &mViewProj );

    D3DXFROMWINEMatrixScaling( &mSceneWorld, 20, 20, 20 );
    mSceneWVP = mSceneWorld * mViewProj;
    vLightDir = D3DXFROMWINEVECTOR4( g_LightControl.x, g_LightControl.y, g_LightControl.z, 1 );

#if 1//def RENDER_CHUNKS
    // Gather up the instance matrices for the buildings and pieces
    g_BaseMeshMatrices.clear();
    for( UINT i = 0; i < NUM_CHUNKS; i++ )
    {
        g_ChunkMeshMatrices[i].clear();
    }

    // Get matrices
    for( UINT i = 0; i < MAX_BUILDINGS; i++ )
    {
        g_Building[i].CollectBaseMeshMatrices( &g_BaseMeshMatrices );
        for( UINT c = 0; c < NUM_CHUNKS; c++ )
        {
            g_Building[i].CollectChunkMeshMatrices( c, &g_ChunkMeshMatrices[c] );
        }
    }

    // Set our input layout
    //IRenderer::GetRendererInstance()->setVertexFormat( g_pMeshVF );

    // Intact walls
    pmData = &g_BaseMeshMatrices[0];
    UINT NumMeshes = g_BaseMeshMatrices.size();
    UINT numrendered = 0;
    while( numrendered < NumMeshes )
    {
        const uint NumToRender = stx_min( MAX_INSTANCES, NumMeshes - numrendered );
	
	IRenderer::GetRendererInstance()->setShaderConstantArray4x4f("g_mWorldInst", ( const D3DXFROMWINEMATRIX * )&pmData[numrendered], NumToRender);

        RenderInstanced( g_WallMesh, NumToRender );

        numrendered += NumToRender;
    }

    // Chunks
    for( UINT c = 0; c < NUM_CHUNKS; c++ )
    {
        pmData = &g_ChunkMeshMatrices[c][0];
        NumMeshes = g_ChunkMeshMatrices[c].size();
        numrendered = 0;
        while( numrendered < NumMeshes )
        {
            const uint NumToRender = stx_min( MAX_INSTANCES, NumMeshes - numrendered );

            IRenderer::GetRendererInstance()->setShaderConstantArray4x4f("g_mWorldInst", ( const D3DXFROMWINEMATRIX * )&pmData[numrendered], NumToRender);

            RenderInstanced( g_ChunkMesh[c], NumToRender );

            numrendered += NumToRender;
        }
    }
#endif
    // Render particles
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_pmWorldViewProjection", mWorldViewProjection );
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_pmWorld", mWorld );

    if( g_bRenderDeferred )
    {
        RenderParticlesIntoBuffer( );
        CompositeParticlesIntoScene( );
    }
    else
    {
        RenderParticles( g_pRenderParticlesSHD, g_pRenderParticlesVF );
    }

    //STXGUI::update();

    IRenderer::GetRendererInstance()->EndScene();
    IRenderer::GetRendererInstance()->Present();
}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("DeferredParticles");
	IInput*    i=STX_Service::GetInputInstance();
	DeferredParticles app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}

void NewExplosion( D3DXFROMWINEVECTOR3 vCenter, float fSize )
{
    D3DXFROMWINEVECTOR3 vDirMul( 0.2f,1.0f,0.2f );
    float fMinPower = 5.0f;
    float fMaxPower = 30.0f;
    for( UINT i = 0; i < MAX_BUILDINGS; i++ )
    {
        g_Building[i].CreateExplosion( vCenter, vDirMul, fSize, fMinPower, fMaxPower );
    }
}

