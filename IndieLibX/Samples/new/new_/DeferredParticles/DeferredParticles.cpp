/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//--------------------------------------------------------------------------------------
// File: DeferredParticles.cpp
//
// This sample shows a method of rendering deferred particles systems.  This is a variation
// of the method of deferred particle shading shown at unity3d.com/blogs/nf/files/page0_blog_entry73_1.pdf
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "ParticleSystem.h"
#include "BreakableWall.h"

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
D3DXFROMWINEVECTOR3 g_EyePt;
D3DXFROMWINEMATRIX g_Proj;
D3DXFROMWINEMATRIX g_View;

// Direct3D10 resources
MeshRenderer2                        g_WallMesh;
MeshRenderer2 g_ChunkMesh[NUM_CHUNKS];

#define MAX_BUILDINGS 5
CBuilding g_Building[MAX_BUILDINGS];
CGrowableArray <D3DXFROMWINEMATRIX>         g_BaseMeshMatrices;
CGrowableArray <D3DXFROMWINEMATRIX> g_ChunkMeshMatrices[NUM_CHUNKS];

VertexFormatID                  g_pVertexLayout;
VertexFormatID                  g_pScreenQuadLayout;
VertexFormatID                  g_pMeshLayout;

unsigned int                                g_NumParticles = 200;
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
D3DXFROMWINEVECTOR3 g_LightControl;

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

unsigned int                                g_NumParticlesToDraw = 0;
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
VertexBufferID                       g_pParticleBuffer = 0;
VertexBufferID                       g_pScreenQuadVB = 0;
IndexBufferID                       g_pParticleBufferIB = 0;
IndexBufferID                       g_pScreenQuadIB = 0;

TextureID                    g_pOffscreenParticleRT = 0;
TextureID                    g_pOffscreenParticleColorRT = 0;
TextureID                   g_pParticleTexture = 0;

stx_Effect*                        g_pEffect10 = 0;
stx_Effect::stx_Technique*               g_pRenderParticlesToBuffer = 0;
stx_Effect::stx_Technique*               g_pRenderParticles = 0;
stx_Effect::stx_Technique*               g_pCompositeParticlesToScene = 0;
stx_Effect::stx_Technique*               g_pRenderMesh = 0;
stx_Effect::stx_Technique*               g_pRenderMeshInst = 0;

stx_Effect::stx_Variable*           g_ptxDiffuse = 0;
stx_Effect::stx_Variable*           g_ptxParticleColor = 0;
stx_Effect::stx_Variable*          g_pLightDir = 0;
stx_Effect::stx_Variable*          g_pmWorldViewProjection = 0;
stx_Effect::stx_Variable*          g_pmWorld = 0;
stx_Effect::stx_Variable*          g_pmInvViewProj = 0;
stx_Effect::stx_Variable*          g_pfTime = 0;
stx_Effect::stx_Variable*          g_pvEyePt = 0;
stx_Effect::stx_Variable*          g_pvRight = 0;
stx_Effect::stx_Variable*          g_pvUp = 0;
stx_Effect::stx_Variable*          g_pvForward = 0;

stx_Effect::stx_Variable*          g_pNumGlowLights = 0;
stx_Effect::stx_Variable*          g_pvGlowLightPosIntensity = 0;
stx_Effect::stx_Variable*          g_pvGlowLightColor = 0;
stx_Effect::stx_Variable*          g_pvGlowLightAttenuation = 0;
stx_Effect::stx_Variable*          g_pvMeshLightAttenuation = 0;

stx_Effect::stx_Variable*          g_pmViewProj = 0;
stx_Effect::stx_Variable*          g_pmWorldInst = 0;

bool                                g_bRenderDeferred = true;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_RESET				50
#define IDC_DEFERRED			51
#define IDC_TOGGLEWARP          5

struct App
{
	App(){}
	virtual ~App(){}

//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
void InitApp()
{
    D3DXFROMWINEVECTOR3 vDir( 1,1,0 );
    D3DXFROMWINEVec3Normalize( &vDir, &vDir );
    g_LightControl=vDir;

    // Setup the camera's view parameters
    D3DXFROMWINEVECTOR3 vecEye( 0.0f, 150.0f, 336.0f );
    D3DXFROMWINEVECTOR3 vecAt ( 0.0f, 0.0f, 0.0f );
    D3DXFROMWINEVECTOR3 vecUp ( 0.0f, 1.0f, 0.0f );
	D3DXFROMWINEMatrixLookAtLH(&g_View, &vecEye, &vecAt, &vecUp);
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void  OnFrameMove( double fTime, float fElapsedTime )
{
    // Update the camera's position based on user input
    if (fElapsedTime  > 0.1f ) fElapsedTime = 0.1f;

    D3DXFROMWINEVECTOR3 vEye;
    D3DXFROMWINEMATRIX mView;
    vEye = g_EyePt;
    mView = g_View;
    D3DXFROMWINEVECTOR3 vRight( mView._11, mView._21, mView._31 );
    D3DXFROMWINEVECTOR3 vUp( mView._12, mView._22, mView._32 );
    D3DXFROMWINEVECTOR3 vFoward( mView._13, mView._23, mView._33 );

    D3DXFROMWINEVec3Normalize( &vRight, &vRight );
    D3DXFROMWINEVec3Normalize( &vUp, &vUp );
    D3DXFROMWINEVec3Normalize( &vFoward, &vFoward );

    g_pvRight->SetFloatVector( ( float* )&vRight );
    g_pvUp->SetFloatVector( ( float* )&vUp );
    g_pvForward->SetFloatVector( ( float* )&vFoward );

    unsigned int NumActiveSystems = 0;
    D3DXFROMWINEVECTOR4 vGlowLightPosIntensity[MAX_PARTICLE_SYSTEMS];
    D3DXFROMWINEVECTOR4 vGlowLightColor[MAX_PARTICLE_SYSTEMS];

    // Advanced building pieces
    for( unsigned int i = 0; i < MAX_BUILDINGS; i++ )
    {
        g_Building[i].AdvancePieces( fElapsedTime, g_vGravity );
    }

    // Advance the system
    for( unsigned int i = 0; i < MAX_PARTICLE_SYSTEMS; i++ )
    {
        g_ppParticleSystem[i]->AdvanceSystem( ( float )fTime, fElapsedTime, vRight, vUp, g_vWindVel, g_vGravity );
    }

    PARTICLE_VERTEX* pVerts = (PARTICLE_VERTEX*)IRenderer::GetRendererInstance()->LockVertexBuffer(g_pParticleBuffer);

    CopyParticlesToVertexBuffer( pVerts, vEye, vRight, vUp );

    IRenderer::GetRendererInstance()->UnLockVertexBuffer(g_pParticleBuffer);

    for( unsigned int i = 0; i < MAX_MUSHROOM_CLOUDS; i += 2 )
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
            D3DXFROMWINEVECTOR4 vFlashColor = g_vFlashColor[ rand() % MAX_FLASH_COLORS ];

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
            vGlowLightPosIntensity[NumActiveSystems] = D3DXFROMWINEVECTOR4( vCenter.x, vCenter.y + g_fLightRaise, vCenter.z,
                                                                    fIntensity );
            vGlowLightColor[NumActiveSystems] = vFlashColor;
            NumActiveSystems ++;
        }
    }

    // Ground bursts
    for( unsigned int i = MAX_MUSHROOM_CLOUDS; i < MAX_GROUND_BURSTS; i++ )
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
            D3DXFROMWINEVECTOR4 vFlashColor = g_vFlashColor[ rand() % MAX_FLASH_COLORS ];

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
            vGlowLightPosIntensity[NumActiveSystems] = D3DXFROMWINEVECTOR4( vCenter.x, vCenter.y + g_fLightRaise, vCenter.z,
                                                                    fIntensity );
            vGlowLightColor[NumActiveSystems] = vFlashColor;
            NumActiveSystems ++;
        }
    }

    // Land mines
    for( unsigned int i = MAX_GROUND_BURSTS; i < MAX_PARTICLE_SYSTEMS; i++ )
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
            D3DXFROMWINEVECTOR4 vFlashColor = g_vFlashColor[ rand() % MAX_FLASH_COLORS ];

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
            vGlowLightPosIntensity[NumActiveSystems] = D3DXFROMWINEVECTOR4( vCenter.x, vCenter.y + g_fLightRaise, vCenter.z,
                                                                    fIntensity );
            vGlowLightColor[NumActiveSystems] = vFlashColor;
            NumActiveSystems ++;
        }
    }

    // Setup light variables
    g_pNumGlowLights->SetInt( NumActiveSystems );
    g_pvGlowLightPosIntensity->SetFloatVectorArray( ( float* )&vGlowLightPosIntensity, 0, NumActiveSystems );
    g_pvGlowLightColor->SetFloatVectorArray( ( float* )&vGlowLightColor, 0, NumActiveSystems );
    g_pvGlowLightAttenuation->SetFloatVector( ( float* )&g_vFlashAttenuation );
    g_pvMeshLightAttenuation->SetFloatVector( ( float* )&g_vMeshLightAttenuation );
}

void ResetBuildings()
{
    float f2Third = 0.6666f;
    D3DXFROMWINEVECTOR3 vChunkOffsets[NUM_CHUNKS] =
    {
        D3DXFROMWINEVECTOR3( f2Third, -f2Third, 0.0f ),
        D3DXFROMWINEVECTOR3( -f2Third, f2Third, 0.0f ),
        D3DXFROMWINEVECTOR3( f2Third, f2Third, 0.0f ),
        D3DXFROMWINEVECTOR3( -f2Third, -f2Third, 0.0f ),
        D3DXFROMWINEVECTOR3( f2Third, 0, 0.0f ),
        D3DXFROMWINEVECTOR3( 0, f2Third, 0.0f ),
        D3DXFROMWINEVECTOR3( -f2Third, 0, 0.0f ),
        D3DXFROMWINEVECTOR3( 0, -f2Third, 0.0f ),
        D3DXFROMWINEVECTOR3( 0, 0, 0.0f )
    };

    for( unsigned int i = 0; i < MAX_BUILDINGS; i++ )
    {
        g_Building[i].SetBaseMesh( &g_WallMesh );
        for( unsigned int c = 0; c < NUM_CHUNKS; c++ )
        {
            g_Building[i].SetChunkMesh( c, &g_ChunkMesh[c], vChunkOffsets[c] );
        }
    }
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
int  OnCreateDevice()
{
    int hr;

    g_pEffect10=IRenderer::GetRendererInstance()->addEffectFromFile("/new/DeferredParticles/DeferredParticles.fx");

    // Obtain technique objects
    g_pRenderParticlesToBuffer = g_pEffect10->GetTechniqueByName( "RenderParticlesToBuffer" );
    g_pRenderParticles = g_pEffect10->GetTechniqueByName( "RenderParticles" );
    g_pCompositeParticlesToScene = g_pEffect10->GetTechniqueByName( "CompositeParticlesToScene" );
    g_pRenderMesh = g_pEffect10->GetTechniqueByName( "RenderMesh" );
    g_pRenderMeshInst = g_pEffect10->GetTechniqueByName( "RenderMeshInst" );

    // Obtain variables
    g_ptxDiffuse = g_pEffect10->GetVariableByName( "g_txMeshTexture" );
    g_ptxParticleColor = g_pEffect10->GetVariableByName( "g_txParticleColor" );
    g_pLightDir = g_pEffect10->GetVariableByName( "g_LightDir" );
    g_pmWorldViewProjection = g_pEffect10->GetVariableByName( "g_mWorldViewProjection" );
    g_pmWorld = g_pEffect10->GetVariableByName( "g_mWorld" );
    g_pmInvViewProj = g_pEffect10->GetVariableByName( "g_mInvViewProj" );
    g_pfTime = g_pEffect10->GetVariableByName( "g_fTime" );
    g_pvEyePt = g_pEffect10->GetVariableByName( "g_vEyePt" );
    g_pvRight = g_pEffect10->GetVariableByName( "g_vRight" );
    g_pvUp = g_pEffect10->GetVariableByName( "g_vUp" );
    g_pvForward = g_pEffect10->GetVariableByName( "g_vForward" );

    g_pNumGlowLights = g_pEffect10->GetVariableByName( "g_NumGlowLights" );
    g_pvGlowLightPosIntensity = g_pEffect10->GetVariableByName( "g_vGlowLightPosIntensity" );
    g_pvGlowLightColor = g_pEffect10->GetVariableByName( "g_vGlowLightColor" );
    g_pvGlowLightAttenuation = g_pEffect10->GetVariableByName( "g_vGlowLightAttenuation" );
    g_pvMeshLightAttenuation = g_pEffect10->GetVariableByName( "g_vMeshLightAttenuation" );

    g_pmWorldInst = g_pEffect10->GetVariableByName( "g_mWorldInst" );
    g_pmViewProj = g_pEffect10->GetVariableByName( "g_mViewProj" );

    // Create our vertex input layout
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 1,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 1,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4
	};
	g_pVertexLayout = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}

	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4
	};
    g_pScreenQuadLayout = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}

	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	g_pMeshLayout = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}

    // Load the meshes
#if 0
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
#else
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
#endif
    // Buildings
    g_Building[0].CreateBuilding( D3DXFROMWINEVECTOR3( 0, 0, 0 ), 2.0f, 50, 0, 50 );

    float fBuildingRange = g_fWorldBounds - 20.0f;

    for( unsigned int i = 1; i < MAX_BUILDINGS; i++ )
    {
        D3DXFROMWINEVECTOR3 vCenter;
        vCenter.x = RPercent() * fBuildingRange;
        vCenter.y = 0;
        vCenter.z = RPercent() * fBuildingRange;

        unsigned int x = ( rand() % 2 ) + 2;
        unsigned int y = ( rand() % 2 ) + 3;
        unsigned int z = ( rand() % 2 ) + 2;
        g_Building[i].CreateBuilding( vCenter, 2.0f, x * 2, y * 2, z * 2 );
    }

    ResetBuildings();

    // Particle system
    unsigned int NumStalkParticles = 500;
    unsigned int NumGroundExpParticles = 345;
    unsigned int NumLandMineParticles = 125;
    unsigned int MaxParticles = MAX_MUSHROOM_CLOUDS * ( g_NumParticles + NumStalkParticles ) +
        ( MAX_GROUND_BURSTS - MAX_MUSHROOM_CLOUDS ) * NumGroundExpParticles +
        ( MAX_PARTICLE_SYSTEMS - MAX_GROUND_BURSTS ) * NumLandMineParticles;
    ( CreateParticleArray( MaxParticles ) );

    D3DXFROMWINEVECTOR4 vColor0( 1.0f,1.0f,1.0f,1 );
    D3DXFROMWINEVECTOR4 vColor1( 0.6f,0.6f,0.6f,1 );

    srand( timeGetTime() );
    g_ppParticleSystem = new CParticleSystem*[MAX_PARTICLE_SYSTEMS];
    g_NumParticlesToDraw = 0;
    for( unsigned int i = 0; i < MAX_MUSHROOM_CLOUDS; i += 2 )
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

    for( unsigned int i = MAX_MUSHROOM_CLOUDS; i < MAX_GROUND_BURSTS; i++ )
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
                                                    30, 100.0f, D3DXFROMWINEVECTOR3( 0, 0.5f, 0 ), D3DXFROMWINEVECTOR3( 1.0f, 0.5f,
                                                                                                        1.0f ),
                                                    vColor0, vColor1,
                                                    g_vPosMul, g_vDirMul );

        g_NumParticlesToDraw += NumGroundExpParticles;
    }

    for( unsigned int i = MAX_GROUND_BURSTS; i < MAX_PARTICLE_SYSTEMS; i++ )
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

    g_pParticleBuffer=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( PARTICLE_VERTEX ) * 6 * g_NumParticlesToDraw, DYNAMIC);{
    std::vector <__WORD__> inds(6 * g_NumParticlesToDraw);
    for(unsigned int i=0;i<6 * g_NumParticlesToDraw;i++)
        inds[i]=i;
    g_pParticleBufferIB=IRenderer::GetRendererInstance()->addIndexBuffer(6 * g_NumParticlesToDraw, sizeof(__WORD__), STATIC, &inds[0]);}

    g_pParticleTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/DeferredParticles/DeferredParticle.dds", false, IRenderer::GetRendererInstance()->Getlinear());
    // Create the screen quad
    D3DXFROMWINEVECTOR3 verts[4] =
    {
        D3DXFROMWINEVECTOR3( -1, -1, 0.5f ),
        D3DXFROMWINEVECTOR3( -1, 1, 0.5f ),
        D3DXFROMWINEVECTOR3( 1, -1, 0.5f ),
        D3DXFROMWINEVECTOR3( 1, 1, 0.5f )
    };
    __WORD__ inds[4] = {0, 1, 2, 3};
    g_pScreenQuadVB=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( D3DXFROMWINEVECTOR3 ) * 4, STATIC, verts);
    g_pScreenQuadIB=IRenderer::GetRendererInstance()->addIndexBuffer(4, sizeof( __WORD__ ), STATIC, inds);

    return 0;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
int  OnResizedSwapChain()
{
    int hr;

    // Setup the camera's projection parameters
    float fAspectRatio = float(IRenderer::GetRendererInstance()->GetViewportWidth())/float(IRenderer::GetRendererInstance()->GetViewportHeight());
	D3DXFROMWINEMatrixPerspectiveFovLH(&g_Proj, D3DXFROMWINE_PI / 2.0f, fAspectRatio, 2.0f, 4000.0f );

    // Create the offscreen particle buffer
	Image3 img;
	g_pOffscreenParticleRT = IRenderer::GetRendererInstance()->addRenderTarget(IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight(), FORMAT_RGBA16f, IRenderer::GetRendererInstance()->Getlinear());
	g_pOffscreenParticleColorRT = IRenderer::GetRendererInstance()->addRenderTarget(IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight(), FORMAT_RGBA8, IRenderer::GetRendererInstance()->Getlinear());

    return 0;
}

//--------------------------------------------------------------------------------------
// Render particles
//--------------------------------------------------------------------------------------
void RenderParticles( stx_Effect::stx_Technique*  pRenderTechnique )
{
    //IA setup
    //IRenderer::GetRendererInstance()->setVertexFormat( g_pVertexLayout );
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, g_pParticleBuffer );
    IRenderer::GetRendererInstance()->setIndexBuffer( g_pParticleBufferIB );
    IRenderer::GetRendererInstance()->DrawPrimitive( PRIM_TRIANGLES, 0, 2 * g_NumParticlesToDraw, sizeof( PARTICLE_VERTEX ) );

    IRenderer::GetRendererInstance()->setTexture( "g_txDiffuse", g_pParticleTexture );

    //Render
    TECHNIQUE_DESC techDesc;
    pRenderTechnique->GetDesc( &techDesc );

    g_NumParticlesToDraw = GetNumActiveParticles();
    for( unsigned int p = 0; p < techDesc.Passes; ++p )
    {
        pRenderTechnique->GetPassByIndex( p )->Apply( 0 );
        IRenderer::GetRendererInstance()->DrawPrimitive( PRIM_TRIANGLES, 0, 2 * g_NumParticlesToDraw, sizeof( PARTICLE_VERTEX ) );
    }
}

//--------------------------------------------------------------------------------------
// Render particles into the offscreen buffer
//--------------------------------------------------------------------------------------
void RenderParticlesIntoBuffer(  )
{
    // Set the new render targets
    TextureID  pViews[1];
    pViews[0] = g_pOffscreenParticleColorRT;
    IRenderer::GetRendererInstance()->changeRenderTargets(pViews, 1, g_pOffscreenParticleRT);
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

    // Render the particles
    RenderParticles( g_pRenderParticlesToBuffer );

    // restore the original render targets
  	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
    //////////////SAFE_RELEASE( pOldRTV );
    //////////////SAFE_RELEASE( pOldDSV );
}

//--------------------------------------------------------------------------------------
// Composite offscreen particle buffer back into the scene
//--------------------------------------------------------------------------------------
void CompositeParticlesIntoScene(  )
{
    // Render the particles
    stx_Effect::stx_Technique*  pRenderTechnique = g_pCompositeParticlesToScene;

    //IA setup
    //IRenderer::GetRendererInstance()->setVertexFormat( g_pScreenQuadLayout );
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, g_pScreenQuadVB );
    IRenderer::GetRendererInstance()->setIndexBuffer( g_pScreenQuadIB );
    IRenderer::GetRendererInstance()->DrawPrimitive( PRIM_TRIANGLE_STRIP, 0, 2 );

    IRenderer::GetRendererInstance()->setTexture( "g_txDiffuse", g_pOffscreenParticleRT );
    IRenderer::GetRendererInstance()->setTexture( "g_txParticleColor", g_pOffscreenParticleColorRT );

    //Render
    TECHNIQUE_DESC techDesc;
    pRenderTechnique->GetDesc( &techDesc );

    for( unsigned int p = 0; p < techDesc.Passes; ++p )
    {
        pRenderTechnique->GetPassByIndex( p )->Apply( 0 );
        IRenderer::GetRendererInstance()->DrawPrimitive( PRIM_TRIANGLE_STRIP, 0, 2 );
    }
}

void RenderInstanced( stx_Effect::stx_Technique*  pTechnique, MeshRenderer2* pMesh,
                      unsigned int NumInstances )
{
#if 0 // ???
    VertexBufferID pVB[1];
    unsigned int Strides[1];
    unsigned int Offsets[1] =
    {
        0
    };

    pVB[0] = pMesh->GetVB10( 0, 0 );
    Strides[0] = ( unsigned int )pMesh->GetVertexStride( 0, 0 );

    IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1, pVB, Strides, Offsets );
    IRenderer::GetRendererInstance()->setIndexBuffer( pMesh->GetIB10( 0 ), pMesh->GetIBFormat10( 0 ), 0 );

    TECHNIQUE_DESC techDesc;
    pTechnique->GetDesc( &techDesc );
    SDKMESH_SUBSET* pSubset = 0;
    Primitives PrimType;

    for( unsigned int p = 0; p < techDesc.Passes; ++p )
    {
        for( unsigned int subset = 0; subset < pMesh->GetNumSubsets( 0 ); ++subset )
        {
            pSubset = pMesh->GetSubset( 0, subset );

            PrimType = pMesh->GetPrimitiveType10( ( SDKMESH_PRIMITIVE_TYPE )pSubset->PrimitiveType );
            IRenderer::GetRendererInstance()->DrawPrimitive( PrimType );

            pTechnique->GetPassByIndex( p )->Apply( 0 );
            IRenderer::GetRendererInstance()->DrawIndexedInstanced( ( unsigned int )pSubset->IndexCount, NumInstances,
                                              0, ( unsigned int )pSubset->VertexStart, 0 );
        }
    }
#else
	pMesh->Render(pTechnique, -1);
#endif
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void  OnFrameRender( double fTime, float fElapsedTime )
{
    int hr;

    // Clear the render target and depth stencil

	IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

    D3DXFROMWINEVECTOR3 vEyePt;
    D3DXFROMWINEMATRIX mWorldViewProjection;
    D3DXFROMWINEVECTOR4 vLightDir;
    D3DXFROMWINEMATRIX mWorld;
    D3DXFROMWINEMATRIX mView;
    D3DXFROMWINEMATRIX mProj;
    D3DXFROMWINEMATRIX mViewProj;
    D3DXFROMWINEMATRIX mInvViewProj;

    // Get the projection & view matrix from the camera class
    D3DXFROMWINEMatrixIdentity( &mWorld );
    vEyePt = g_EyePt;
    mProj = g_Proj;
    mView = g_View;

    mWorldViewProjection = mView * mProj;
    mViewProj = mView * mProj;
    D3DXFROMWINEMatrixInverse( &mInvViewProj, 0, &mViewProj );
    D3DXFROMWINEMATRIX mSceneWorld;
    D3DXFROMWINEMatrixScaling( &mSceneWorld, 20, 20, 20 );
    D3DXFROMWINEMATRIX mSceneWVP = mSceneWorld * mViewProj;
    vLightDir = D3DXFROMWINEVECTOR4( g_LightControl, 1 );

    // Per frame variables
    ( g_pmWorldViewProjection->SetMatrix( ( float* )&mSceneWVP ) );
    ( g_pmWorld->SetMatrix( ( float* )&mSceneWorld ) );
    ( g_pLightDir->SetFloatVector( ( float* )vLightDir ) );
    ( g_pmInvViewProj->SetMatrix( ( float* )&mInvViewProj ) );
    ( g_pfTime->SetFloat( ( float )fTime ) );
    ( g_pvEyePt->SetFloatVector( ( float* )&vEyePt ) );
    ( g_pmViewProj->SetMatrix( ( float* )&mViewProj ) );

#if 0 // ???
    // Gather up the instance matrices for the buildings and pieces
    g_BaseMeshMatrices.Reset();
    for( unsigned int i = 0; i < NUM_CHUNKS; i++ )
    {
        g_ChunkMeshMatrices[i].Reset();
    }
#endif

    // Get matrices
    for( unsigned int i = 0; i < MAX_BUILDINGS; i++ )
    {
        g_Building[i].CollectBaseMeshMatrices( &g_BaseMeshMatrices );
        for( unsigned int c = 0; c < NUM_CHUNKS; c++ )
        {
            g_Building[i].CollectChunkMeshMatrices( c, &g_ChunkMeshMatrices[c] );
        }
    }

    // Set our input layout
    //IRenderer::GetRendererInstance()->setVertexFormat( g_pMeshLayout );

    // Intact walls
    D3DXFROMWINEMATRIX* pmData = &g_BaseMeshMatrices[0];
    unsigned int NumMeshes = g_BaseMeshMatrices.size();
    unsigned int numrendered = 0;
    while( numrendered < NumMeshes )
    {
        unsigned int NumToRender = std::min( MAX_INSTANCES, int(NumMeshes - numrendered) );

        g_pmWorldInst->SetMatrixArray( ( float* )&pmData[numrendered], 0, NumToRender );

        RenderInstanced( g_pRenderMeshInst, &g_WallMesh, NumToRender );

        numrendered += NumToRender;
    }

    // Chunks
    for( unsigned int c = 0; c < NUM_CHUNKS; c++ )
    {
        pmData = &g_ChunkMeshMatrices[c].at(0);
        NumMeshes = g_ChunkMeshMatrices[c].size();
        numrendered = 0;
        while( numrendered < NumMeshes )
        {
            unsigned int NumToRender = std::min( MAX_INSTANCES, int(NumMeshes - numrendered) );

            g_pmWorldInst->SetMatrixArray( ( float* )&pmData[numrendered], 0, NumToRender );

            RenderInstanced( g_pRenderMeshInst, &g_ChunkMesh[c], NumToRender );

            numrendered += NumToRender;
        }
    }

    // Render particles
    ( g_pmWorldViewProjection->SetMatrix( ( float* )&mWorldViewProjection ) );
    ( g_pmWorld->SetMatrix( ( float* )&mWorld ) );

    if( g_bRenderDeferred )
    {
        RenderParticlesIntoBuffer(  );
        CompositeParticlesIntoScene(  );
    }
    else
    {
        RenderParticles( g_pRenderParticles );
    }
}
};
App app;
void NewExplosion( D3DXFROMWINEVECTOR3 vCenter, float fSize )
{
    D3DXFROMWINEVECTOR3 vDirMul( 0.2f,1.0f,0.2f );
    float fMinPower = 5.0f;
    float fMaxPower = 30.0f;
    for( unsigned int i = 0; i < MAX_BUILDINGS; i++ )
    {
        g_Building[i].CreateExplosion( vCenter, vDirMul, fSize, fMinPower, fMaxPower );
    }
}
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
	IRenderer* r=IRenderer::GetRendererInstance("DeferredParticles");
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

