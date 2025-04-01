//--------------------------------------------------------------------------------------
// File: DeferredParticles.cpp
//
// This sample shows a simple example of the Microsoft Direct3D's High-Level 
// Shader Language (HLSL) using the Effect interface. 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

#include "Terrain.h"

#define MAX_LIGHTS 1

//--------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
D3DXFROMWINEVECTOR3			g_EyePt;
D3DXFROMWINEMATRIX                  g_ProjMatrix;
D3DXFROMWINEMATRIX                  g_ViewMatrix;
D3DXFROMWINEVECTOR3 g_LightControl[MAX_LIGHTS];
D3DXFROMWINEMATRIX                       g_mCenterMesh;
float                               g_fLightScale;
int                                 g_nNumActiveLights;
int                                 g_nActiveLight;
bool                                g_bShowHelp = false;    // If true, it renders the UI control text
bool                                g_bRenderShadows = true;
bool                                g_bRenderDetail = false;
float                               g_fShadowBias = 0.003f;
float                               g_fDetailAmount = 0.01f;
float                               g_fDetailRepeat = 35.0f;
float                               g_fDetailDistance = 1.0f;
float                               g_fHeightRatio;
CTerrain                            g_Terrain;
bool                                g_bRenderGeometry = false;
bool                                g_bWireframe = false;

float                               g_fMapHeight = 200.0f;
float                               g_fMapWidth = 2000.0f;
int                                 g_ShadowMapSize = 1024;
D3DXFROMWINEVECTOR3*                        g_pvBoxPositions = 0;
int                                 g_NumBoxes = 20;

VertexFormatID                  g_pBoxLayout = 0;
VertexFormatID                  g_pMeshLayout = 0;
VertexBufferID                       g_pScreenQuadVB = 0;
IndexBufferID                       g_pScreenQuadIB = 0;
MeshRenderer2                        g_BoxMesh;

stx_Effect*                        g_pEffect = 0;

// Raycast terrain techniques
stx_Effect::stx_Technique*               g_pRenderTerrain_Inside = 0;
stx_Effect::stx_Technique*               g_pRenderTerrain_Outside = 0;
stx_Effect::stx_Technique*               g_pRenderTerrain_Ortho = 0;
stx_Effect::stx_Technique*               g_pRenderTerrain_Inside_Detail = 0;
stx_Effect::stx_Technique*               g_pRenderTerrain_Outside_Detail = 0;
stx_Effect::stx_Technique*               g_pRenderTerrain_Ortho_Detail = 0;
stx_Effect::stx_Technique*               g_pRenderTerrain_Inside_Shadow = 0;
stx_Effect::stx_Technique*               g_pRenderTerrain_Outside_Shadow = 0;
stx_Effect::stx_Technique*               g_pRenderTerrain_Ortho_Shadow = 0;
stx_Effect::stx_Technique*               g_pRenderTerrain_Inside_Detail_Shadow = 0;
stx_Effect::stx_Technique*               g_pRenderTerrain_Outside_Detail_Shadow = 0;
stx_Effect::stx_Technique*               g_pRenderTerrain_Ortho_Detail_Shadow = 0;
stx_Effect::stx_Technique*               g_pRenderTerrain_Wire = 0;

// Geometry terrain techniques
stx_Effect::stx_Technique*               g_pRenderGeometryTerrain = 0;
stx_Effect::stx_Technique*               g_pRenderGeometryTerrain_Shadow = 0;
stx_Effect::stx_Technique*               g_pRenderGeometryTerrain_Wire = 0;
stx_Effect::stx_Technique*               g_pRenderMesh = 0;
stx_Effect::stx_Technique*               g_pRenderMesh_Shadow = 0;

ShaderResourceViewID           g_pDetailHeightTexture = 0;
ShaderResourceViewID           g_pDetailColorTexture[4] =
{
    0, 0, 0, 0
};
ShaderResourceViewID           g_pDetailGrad_RedGreen = 0;
ShaderResourceViewID           g_pDetailGrad_BlueAlpha = 0;

// Shadows
TextureID                    g_pShadowDepthTexture = 0;
DepthStencilViewID             g_pShadowDSV = 0;
ShaderResourceViewID           g_pShadowSRV = 0;

stx_Effect::stx_Variable*  g_ptxDiffuse = 0;
stx_Effect::stx_Variable*  g_ptxDetailDiffuse = 0;
stx_Effect::stx_Variable*  g_ptxDetailGrad_RedGreen = 0;
stx_Effect::stx_Variable*  g_ptxDetailGrad_BlueAlpha = 0;
stx_Effect::stx_Variable*  g_ptxHeight = 0;
stx_Effect::stx_Variable*  g_ptxMask = 0;
stx_Effect::stx_Variable*  g_ptxDetailHeight = 0;
stx_Effect::stx_Variable*  g_ptxDepthMap = 0;

stx_Effect::stx_Variable*          g_pLightDir = 0;
stx_Effect::stx_Variable*          g_pLightDirTex = 0;
stx_Effect::stx_Variable*          g_pLightDiffuse = 0;
stx_Effect::stx_Variable*          g_pmWorldViewProjection = 0;
stx_Effect::stx_Variable*          g_pmWorld = 0;
stx_Effect::stx_Variable*          g_pmWorldToTerrain = 0;
stx_Effect::stx_Variable*          g_pmTexToViewProj = 0;
stx_Effect::stx_Variable*          g_pmLightViewProj = 0;
stx_Effect::stx_Variable*          g_pmTexToLightViewProj = 0;
stx_Effect::stx_Variable*          g_pvTextureEyePt = 0;

stx_Effect::stx_Variable*          g_pInvMapSize = 0;
stx_Effect::stx_Variable*          g_pMapSize = 0;

stx_Effect::stx_Variable*          g_pInvDetailMapSize = 0;
stx_Effect::stx_Variable*          g_pDetailMapSize = 0;
stx_Effect::stx_Variable*          g_pDetailRepeat = 0;
stx_Effect::stx_Variable*          g_pInvDetailRepeat = 0;
stx_Effect::stx_Variable*          g_pDetailHeight = 0;
stx_Effect::stx_Variable*          g_pShadowBias = 0;
stx_Effect::stx_Variable*          g_pDetailDistanceSq = 0;
stx_Effect::stx_Variable*          g_pHeightRatio = 0;

// We can load multiple tiles, but for now, just show one
struct RAYCAST_TERRAIN_TILE
{
    char   m_szTerrainTextureDDS[MAX_PATH];
    char   m_szTerrainTextureBMP[MAX_PATH];
    char   m_szTerrainMask[MAX_PATH];
    ShaderResourceViewID m_pTerrainHeightTexture;
    ShaderResourceViewID m_pTerrainMask;
    D3DXFROMWINEVECTOR3 m_vOffset;
    D3DXFROMWINEMATRIX m_mTerrainMatrix;
    D3DXFROMWINEMATRIX m_mInvTerrainMatrix;
};
 
#if 1 
RAYCAST_TERRAIN_TILE* g_TerrainTiles = 0;
#else // ???
RAYCAST_TERRAIN_TILE g_TerrainTiles[] =
{
    { 
	  "RaycastTerrain/MSH1024.dds", 
	  "RaycastTerrain/MSH1024.bmp", 
	  "RaycastTerrain/MSH1024_Mask.dds",		
	  0, 0, D3DXFROMWINEVECTOR3( -0.5f, 0, -0.5f ) 
	}
};
#endif
int                                 g_NumTerrainTiles = 1;

struct App
{
	App(){}
	virtual ~App(){}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    for( int i = 0; i < MAX_LIGHTS; i++ )
    {
        D3DXFROMWINEVECTOR3 vDir( -1, 1, 0 );
        D3DXFROMWINEVec3Normalize( &vDir, &vDir );
        g_LightControl[i]=vDir;
    }

    g_nActiveLight = 0;
    g_nNumActiveLights = 1;
    g_fLightScale = 1.0f;

    // Setup the camera's view parameters
    D3DXFROMWINEVECTOR3 vecEye( 0.0f, 100.0f, -500.0f );
    D3DXFROMWINEVECTOR3 vecAt ( 0.0f, 0.0f, 0.0f );
	D3DXFROMWINEVECTOR3 vecUp (0.0f, 1.0f, 0.0f);
	g_EyePt=vecEye;
	D3DXFROMWINEMatrixLookAtLH(&g_ViewMatrix, &vecEye, &vecAt, &vecUp);
}

//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
int OnCreateDevice()
{
    int hr;

    // Read the D3DXFROMWINE effect file
    #if 0
    g_pEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/new/RaycastTerrain/RaycastTerrain.fx");
    #else
    g_pEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/SimpleTexture/SimpleTexture.fx");
    #endif

    // raycast technique objects
    g_pRenderTerrain_Inside = g_pEffect->GetTechniqueByName( "RenderTerrain_Inside" );
    g_pRenderTerrain_Outside = g_pEffect->GetTechniqueByName( "RenderTerrain_Outside" );
    g_pRenderTerrain_Ortho = g_pEffect->GetTechniqueByName( "RenderTerrain_Ortho" );
    g_pRenderTerrain_Inside_Detail = g_pEffect->GetTechniqueByName( "RenderTerrain_Inside_Detail" );
    g_pRenderTerrain_Outside_Detail = g_pEffect->GetTechniqueByName( "RenderTerrain_Outside_Detail" );
    g_pRenderTerrain_Ortho_Detail = g_pEffect->GetTechniqueByName( "RenderTerrain_Ortho_Detail" );
    g_pRenderTerrain_Inside_Shadow = g_pEffect->GetTechniqueByName( "RenderTerrain_Inside_Shadow" );
    g_pRenderTerrain_Outside_Shadow = g_pEffect->GetTechniqueByName( "RenderTerrain_Outside_Shadow" );
    g_pRenderTerrain_Ortho_Shadow = g_pEffect->GetTechniqueByName( "RenderTerrain_Ortho_Shadow" );
    g_pRenderTerrain_Inside_Detail_Shadow = g_pEffect->GetTechniqueByName( "RenderTerrain_Inside_Detail_Shadow" );
    g_pRenderTerrain_Outside_Detail_Shadow = g_pEffect->GetTechniqueByName( "RenderTerrain_Outside_Detail_Shadow" );
    g_pRenderTerrain_Ortho_Detail_Shadow = g_pEffect->GetTechniqueByName( "RenderTerrain_Ortho_Detail_Shadow" );
    g_pRenderTerrain_Wire = g_pEffect->GetTechniqueByName( "RenderTerrain_Wire" );

    // geometry technique objects
    g_pRenderGeometryTerrain = g_pEffect->GetTechniqueByName( "RenderGeometryTerrain" );
    g_pRenderGeometryTerrain_Shadow = g_pEffect->GetTechniqueByName( "RenderGeometryTerrain_Shadow" );
    g_pRenderGeometryTerrain_Wire = g_pEffect->GetTechniqueByName( "RenderGeometryTerrain_Wire" );
    g_pRenderMesh = g_pEffect->GetTechniqueByName( "RenderMesh" );
    g_pRenderMesh_Shadow = g_pEffect->GetTechniqueByName( "RenderMesh_Shadow" );

    // texture variables
    g_ptxDiffuse = g_pEffect->GetVariableByName( "g_txDiffuse" );
    g_ptxDetailDiffuse = g_pEffect->GetVariableByName( "g_txDetailDiffuse" );
    g_ptxDetailGrad_RedGreen = g_pEffect->GetVariableByName( "g_txDetailGrad_RedGreen" );
    g_ptxDetailGrad_BlueAlpha = g_pEffect->GetVariableByName( "g_txDetailGrad_BlueAlpha" );
    g_ptxHeight = g_pEffect->GetVariableByName( "g_txHeight" );
    g_ptxMask = g_pEffect->GetVariableByName( "g_txMask" );
    g_ptxDetailHeight = g_pEffect->GetVariableByName( "g_txDetailHeight" );
    g_ptxDepthMap = g_pEffect->GetVariableByName( "g_txDepthMap" );

    // constant variables
    g_pLightDir = g_pEffect->GetVariableByName( "g_LightDirc" );
    g_pLightDirTex = g_pEffect->GetVariableByName( "g_LightDirTex" );
    g_pLightDiffuse = g_pEffect->GetVariableByName( "g_LightDiffuse" );
    g_pmWorldViewProjection = g_pEffect->GetVariableByName( "g_mWorldViewProjection" );
    g_pmWorld = g_pEffect->GetVariableByName( "g_mWorld" );
    g_pmWorldToTerrain = g_pEffect->GetVariableByName( "g_mWorldToTerrain" );
    g_pmTexToViewProj = g_pEffect->GetVariableByName( "g_mTexToViewProj" );
    g_pmTexToLightViewProj = g_pEffect->GetVariableByName( "g_mTexToLightViewProj" );
    g_pmLightViewProj = g_pEffect->GetVariableByName( "g_mLightViewProj" );
    g_pvTextureEyePt = g_pEffect->GetVariableByName( "g_vTextureEyePt" );
    g_pInvMapSize = g_pEffect->GetVariableByName( "g_InvMapSize" );
    g_pMapSize = g_pEffect->GetVariableByName( "g_MapSize" );
    g_pInvDetailMapSize = g_pEffect->GetVariableByName( "g_InvDetailMapSize" );
    g_pDetailMapSize = g_pEffect->GetVariableByName( "g_DetailMapSize" );
    g_pDetailRepeat = g_pEffect->GetVariableByName( "g_DetailRepeat" );
    g_pInvDetailRepeat = g_pEffect->GetVariableByName( "g_InvDetailRepeat" );
    g_pDetailHeight = g_pEffect->GetVariableByName( "g_DetailHeight" );
    g_pShadowBias = g_pEffect->GetVariableByName( "g_ShadowBias" );
    g_pDetailDistanceSq = g_pEffect->GetVariableByName( "g_DetailDistanceSq" );
    g_pHeightRatio = g_pEffect->GetVariableByName( "g_HeightRatio" );

	#if 0
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4
	};
	g_pBoxLayout = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	g_pMeshLayout = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}
	#endif

    float fFloor = -0.01f;
    //unsigned int N=36;
    D3DXFROMWINEVECTOR3 vVertices[36];
    __WORD__ vIndices[36];
    for(unsigned int i=0;i<36;i++)
	vIndices[i]=i;
    // Bottom
#if 1//ndef _MSC_VER // ???
    vVertices[0] = D3DXFROMWINEVECTOR3( 0, fFloor, 0 );
    vVertices[1] = D3DXFROMWINEVECTOR3( 0, fFloor, 1 );
    vVertices[2] = D3DXFROMWINEVECTOR3( 1, fFloor, 0 );
    vVertices[3] = D3DXFROMWINEVECTOR3( 1, fFloor, 0 );
    vVertices[4] = D3DXFROMWINEVECTOR3( 0, fFloor, 1 );
    vVertices[5] = D3DXFROMWINEVECTOR3( 1, fFloor, 1 );

    // Left
    vVertices[6] = D3DXFROMWINEVECTOR3( 0, fFloor, 0 );
    vVertices[7] = D3DXFROMWINEVECTOR3( 0, 1, 0 );
    vVertices[8] = D3DXFROMWINEVECTOR3( 0, fFloor, 1 );
    vVertices[9] = D3DXFROMWINEVECTOR3( 0, fFloor, 1 );
    vVertices[10] = D3DXFROMWINEVECTOR3( 0, 1, 0 );
    vVertices[11] = D3DXFROMWINEVECTOR3( 0, 1, 1 );

    // Right
    vVertices[12] = D3DXFROMWINEVECTOR3( 1, fFloor, 1 );
    vVertices[13] = D3DXFROMWINEVECTOR3( 1, 1, 1 );
    vVertices[14] = D3DXFROMWINEVECTOR3( 1, fFloor, 0 );
    vVertices[15] = D3DXFROMWINEVECTOR3( 1, fFloor, 0 );
    vVertices[16] = D3DXFROMWINEVECTOR3( 1, 1, 1 );
    vVertices[17] = D3DXFROMWINEVECTOR3( 1, 1, 0 );

    // Back
    vVertices[18] = D3DXFROMWINEVECTOR3( 0, fFloor, 1 );
    vVertices[19] = D3DXFROMWINEVECTOR3( 0, 1, 1 );
    vVertices[20] = D3DXFROMWINEVECTOR3( 1, fFloor, 1 );
    vVertices[21] = D3DXFROMWINEVECTOR3( 1, fFloor, 1 );
    vVertices[22] = D3DXFROMWINEVECTOR3( 0, 1, 1 );
    vVertices[23] = D3DXFROMWINEVECTOR3( 1, 1, 1 );

    // Front
    vVertices[24] = D3DXFROMWINEVECTOR3( 1, fFloor, 0 );
    vVertices[25] = D3DXFROMWINEVECTOR3( 1, 1, 0 );
    vVertices[26] = D3DXFROMWINEVECTOR3( 0, fFloor, 0 );
    vVertices[27] = D3DXFROMWINEVECTOR3( 0, fFloor, 0 );
    vVertices[28] = D3DXFROMWINEVECTOR3( 1, 1, 0 );
    vVertices[29] = D3DXFROMWINEVECTOR3( 0, 1, 0 );

    // Top
    vVertices[30] = D3DXFROMWINEVECTOR3( 0, 1, 1 );
    vVertices[31] = D3DXFROMWINEVECTOR3( 0, 1, 0 );
    vVertices[32] = D3DXFROMWINEVECTOR3( 1, 1, 1 );
    vVertices[33] = D3DXFROMWINEVECTOR3( 1, 1, 1 );
    vVertices[34] = D3DXFROMWINEVECTOR3( 0, 1, 0 );
    vVertices[35] = D3DXFROMWINEVECTOR3( 1, 1, 0 );
#endif

    g_pScreenQuadVB=IRenderer::GetRendererInstance()->addVertexBuffer(36*sizeof(D3DXFROMWINEVECTOR3), STATIC, vVertices);
    g_pScreenQuadIB=IRenderer::GetRendererInstance()->addIndexBuffer(36, 2, STATIC, vIndices);

    // Setup shader variables based on terrain sizes
    g_fMapHeight = 200.0f;
    g_fMapWidth = 2000.0f;
    g_pMapSize->SetFloat( 1024.0f );
    g_pInvMapSize->SetFloat( 1.0f / 1024.0f );
    g_pDetailMapSize->SetFloat( 256.0f );
    g_pInvDetailMapSize->SetFloat( 1.0f / 256.0f );
    g_pDetailRepeat->SetFloat( g_fDetailRepeat );
    g_pInvDetailRepeat->SetFloat( 1.0f / g_fDetailRepeat );
    g_pDetailHeight->SetFloat( g_fDetailAmount );
    g_pShadowBias->SetFloat( g_fShadowBias );
    g_pDetailDistanceSq->SetFloat( g_fDetailDistance * g_fDetailDistance );
    g_fHeightRatio = g_fMapHeight / g_fMapWidth;
    g_pHeightRatio->SetFloat( g_fHeightRatio );

    // Load the detail texture
    g_pDetailHeightTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/new/RaycastTerrain/DetailMapHeight.dds", false);

    // Load the detail diffuse textures
    g_pDetailColorTexture[0]=IRenderer::GetRendererInstance()->addImageLibTexture("/new/RaycastTerrain/DetailMap_Red.dds", false);
    g_pDetailColorTexture[1]=IRenderer::GetRendererInstance()->addImageLibTexture("/new/RaycastTerrain/DetailMap_Green.dds", false);
    g_pDetailColorTexture[2]=IRenderer::GetRendererInstance()->addImageLibTexture("/new/RaycastTerrain/DetailMap_Blue.dds", false);
    g_pDetailColorTexture[3]=IRenderer::GetRendererInstance()->addImageLibTexture("/new/RaycastTerrain/DetailMap_Alpha.dds", false);

    g_pDetailGrad_RedGreen=IRenderer::GetRendererInstance()->addImageLibTexture("/new/RaycastTerrain/DetailMapGrad_RedGreen.dds", false);
    g_pDetailGrad_BlueAlpha=IRenderer::GetRendererInstance()->addImageLibTexture("/new/RaycastTerrain/DetailMapGrad_BlueAlpha.dds", false);

    g_NumTerrainTiles = elementsOf( g_TerrainTiles );
    for( int i = 0; i < g_NumTerrainTiles; i++ )
    {
        // Create a terrain from a bitmap
        hr=IRenderer::GetRendererInstance()->addImageLibTexture(g_TerrainTiles[i].m_szTerrainTextureDDS, false);
        if( hr==-1 )
        {
            hr=IRenderer::GetRendererInstance()->addImageLibTexture(g_TerrainTiles[i].m_szTerrainTextureBMP, false);
            ( PreprocessTerrain( g_TerrainTiles[i].m_szTerrainTextureBMP, g_TerrainTiles[i].m_szTerrainTextureDDS ) );
        }

        // Load the height and ddx,ddy texture
        g_TerrainTiles[i].m_pTerrainHeightTexture=IRenderer::GetRendererInstance()->addImageLibTexture(g_TerrainTiles[i].m_szTerrainTextureDDS, false);

        // Load the mask
        g_TerrainTiles[i].m_pTerrainMask=IRenderer::GetRendererInstance()->addImageLibTexture(g_TerrainTiles[i].m_szTerrainMask, false);

        // Setup our terrain matrix
        D3DXFROMWINEMATRIX mTranslation;
        D3DXFROMWINEMATRIX mScaling;
        D3DXFROMWINEMatrixTranslation( &mTranslation, g_TerrainTiles[i].m_vOffset.x, g_TerrainTiles[i].m_vOffset.y,
                               g_TerrainTiles[i].m_vOffset.z );

        D3DXFROMWINEMatrixScaling( &mScaling, g_fMapWidth, g_fMapHeight, g_fMapWidth );
        g_TerrainTiles[i].m_mTerrainMatrix = mTranslation * mScaling;
        D3DXFROMWINEMatrixInverse( &g_TerrainTiles[i].m_mInvTerrainMatrix, 0, &g_TerrainTiles[i].m_mTerrainMatrix );
    }

    //g_BoxMesh.Create( "/new/RaycastTerrain/bobble.sdkmesh" );
    g_BoxMesh.CreateTexturedBox(1.0f, 1.0f, 1.0f, eSimpleShader);

    // Load the mesh version of the terrain
    ( g_Terrain.LoadTerrain( "/new/RaycastTerrain/MSH1024.bmp", 1, 1200, g_fMapWidth, g_fMapHeight, 10, 1.0f, 2.0f ) );

    // Load terrain device objects
    ( g_Terrain.OnCreateDevice(  ) );

    // Create positions for boxes
    g_pvBoxPositions = new D3DXFROMWINEVECTOR3[ g_NumBoxes ];
    for( int i = 0; i < g_NumBoxes; i++ )
    {
        g_pvBoxPositions[i].x = RPercent() * g_fMapWidth * 0.4f;
        g_pvBoxPositions[i].z = RPercent() * g_fMapWidth * 0.4f;
        g_pvBoxPositions[i].y = g_Terrain.GetHeightOnMap( &g_pvBoxPositions[i] ) - 5;
    }

    // Create shadow textures
	Image3 tex;
    tex.create(FORMAT_R32I, g_ShadowMapSize, g_ShadowMapSize, 1, 1);
	g_pShadowDepthTexture = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());

    return 0;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
int  OnResizedSwapChain()
{
    // Setup the camera's projection parameters
	float fAspectRatio = IRenderer::GetRendererInstance()->GetAspectRatio();
	D3DXFROMWINEMatrixPerspectiveFovLH(&g_ProjMatrix, D3DXFROMWINE_PI/4, fAspectRatio, 10.0f, 30000.0f );
    return 0;
}

//--------------------------------------------------------------------------------------
stx_Effect::stx_Technique*  GetInsideTechnique( bool bShadowDepthPass )
{
    if( bShadowDepthPass )
    {
        // If we're in the shadow pass, assume that the light is outside the terrain and use an orthographic raycast
        return g_pRenderTerrain_Ortho;
    }
    else
    {
        if( g_bRenderDetail )
        {
            if( g_bRenderShadows )
                return g_pRenderTerrain_Inside_Detail_Shadow;
            else
                return g_pRenderTerrain_Inside_Detail;
        }
        else
        {
            if( g_bRenderShadows )
                return g_pRenderTerrain_Inside_Shadow;
            else
                return g_pRenderTerrain_Inside;
        }
    }
}

//--------------------------------------------------------------------------------------
stx_Effect::stx_Technique*  GetOutsideTechnique( bool bShadowDepthPass )
{
    if( bShadowDepthPass )
    {
        // If we're in the shadow pass, assume that the light is outside the terrain and use an orthographic raycast
        return g_pRenderTerrain_Ortho;
    }
    else
    {
        if( g_bRenderDetail )
        {
            if( g_bRenderShadows )
                return g_pRenderTerrain_Outside_Detail_Shadow;
            else
                return g_pRenderTerrain_Outside_Detail;
        }
        else
        {
            if( g_bRenderShadows )
                return g_pRenderTerrain_Outside_Shadow;
            else
                return g_pRenderTerrain_Outside;
        }
    }
}

//--------------------------------------------------------------------------------------
// Renders the terrain using geometry
//--------------------------------------------------------------------------------------
void RenderTerrain( D3DXFROMWINEMATRIX* pmViewProj, stx_Effect::stx_Technique*  pTech )
{
    D3DXFROMWINEMATRIX mWorld;
    D3DXFROMWINEMatrixIdentity( &mWorld );	

    //IRenderer::GetRendererInstance()->setVertexFormat( g_pMeshLayout );

    g_pmWorldViewProjection->SetMatrix( ( float* )pmViewProj );
    g_pmWorld->SetMatrix( ( float* )&mWorld );

    IRenderer::GetRendererInstance()->setIndexBuffer(g_Terrain.GetTerrainIB());

    if( g_bWireframe )
        pTech = g_pRenderGeometryTerrain_Wire;

    TECHNIQUE_DESC techDesc;
    pTech->GetDesc( &techDesc );

    unsigned int NumTiles = g_Terrain.GetNumTiles();

    for( unsigned int p = 0; p < techDesc.Passes; ++p )
    {
        for( unsigned int i = 0; i < NumTiles; i++ )
        {
            TERRAIN_TILE* pTile = g_Terrain.GetTile( i );

            pTech->GetPassByIndex( p )->Apply( 0 );

            g_Terrain.RenderTile( pTile );
        }
    }
}

//--------------------------------------------------------------------------------------
// Raycasts the terrain
//--------------------------------------------------------------------------------------
void RaycastTerrain( D3DXFROMWINEMATRIX* pmViewProj, D3DXFROMWINEMATRIX* pmLightViewProj,
                     D3DXFROMWINEVECTOR3* pvLightDir,
                     D3DXFROMWINEVECTOR3* pvEye, bool bShadowDepthPass )
{
    int hr = 0;

    g_ptxDetailDiffuse->SetResourceArray( g_pDetailColorTexture, 0, 4 );
    g_ptxDetailGrad_RedGreen->SetResource( g_pDetailGrad_RedGreen );
    g_ptxDetailGrad_BlueAlpha->SetResource( g_pDetailGrad_BlueAlpha );
    g_ptxDetailHeight->SetResource( g_pDetailHeightTexture );

    //IA setup
    //IRenderer::GetRendererInstance()->setVertexFormat( g_pBoxLayout );
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, g_pScreenQuadVB );
    IRenderer::GetRendererInstance()->setIndexBuffer( g_pScreenQuadIB );
    IRenderer::GetRendererInstance()->DrawPrimitive( PRIM_TRIANGLES, 0, 36);

    for( int i = 0; i < g_NumTerrainTiles; i++ )
    {
        // Get the projection & view matrix from the camera class
        D3DXFROMWINEMATRIX mWorld = g_TerrainTiles[i].m_mTerrainMatrix;
        D3DXFROMWINEMATRIX mWorldViewProjection = mWorld * *pmViewProj;

        // Move the light into terrain texture space
        D3DXFROMWINEVECTOR4 vLightDir4;
        D3DXFROMWINEVec3TransformNormal( ( D3DXFROMWINEVECTOR3* )&vLightDir4, pvLightDir, &g_TerrainTiles[i].m_mInvTerrainMatrix );
        D3DXFROMWINEVec3Normalize( ( D3DXFROMWINEVECTOR3* )&vLightDir4, ( D3DXFROMWINEVECTOR3* )&vLightDir4 );
        D3DXFROMWINEVECTOR4 vSwizzle( vLightDir4.x, vLightDir4.z, vLightDir4.y, 1 );
        g_pLightDirTex->SetFloatVector( ( float* )&vSwizzle );

        D3DXFROMWINEMATRIX mTexToViewProj;
        mTexToViewProj = g_TerrainTiles[i].m_mTerrainMatrix * ( *pmViewProj );

        D3DXFROMWINEMATRIX mTexToLightViewProj;
        mTexToLightViewProj = g_TerrainTiles[i].m_mTerrainMatrix * ( *pmLightViewProj );

        // Set projection matrices
        ( g_pmWorldViewProjection->SetMatrix( ( float* )&mWorldViewProjection ) );
        ( g_pmWorld->SetMatrix( ( float* )&mWorld ) );
        ( g_pmTexToViewProj->SetMatrix( ( float* )&mTexToViewProj ) );
        ( g_pmTexToLightViewProj->SetMatrix( ( float* )&mTexToLightViewProj ) );

        // Move the eye point into texture space
        D3DXFROMWINEVECTOR4 vEye4;
        D3DXFROMWINEVec3Transform( &vEye4, pvEye, &g_TerrainTiles[i].m_mInvTerrainMatrix );
        g_pvTextureEyePt->SetFloatVector( ( float* )&vEye4 );

        // Set the terrain texture
        g_ptxHeight->SetResource( g_TerrainTiles[i].m_pTerrainHeightTexture );
        g_ptxMask->SetResource( g_TerrainTiles[i].m_pTerrainMask );

        stx_Effect::stx_Technique*  pTechnique = 0;
        if( g_bWireframe )
        {
            pTechnique = g_pRenderTerrain_Wire;
        }
        else
        {
            // Determine whether we're inside or outside the box
            if( vEye4.x < 0 || vEye4.y < 0 || vEye4.z < 0 ||
                vEye4.x > 1 || vEye4.y > 1 || vEye4.z > 1 )
            {
                pTechnique = GetOutsideTechnique( bShadowDepthPass );
            }
            else
            {
                pTechnique = GetInsideTechnique( bShadowDepthPass );
            }
        }

        //Render
        TECHNIQUE_DESC techDesc;
        pTechnique->GetDesc( &techDesc );
        for( unsigned int p = 0; p < techDesc.Passes; ++p )
        {
            pTechnique->GetPassByIndex( p )->Apply( 0 );
    	    IRenderer::GetRendererInstance()->DrawPrimitive( PRIM_TRIANGLES, 0, 36); // ???
        }
    }
}

//--------------------------------------------------------------------------------------
// Renders boxes
//--------------------------------------------------------------------------------------
void RenderBoxes( D3DXFROMWINEMATRIX* pmViewProj, stx_Effect::stx_Technique*  pTechnique )
{
    int hr = 0;
    // Set the layout
    //IRenderer::GetRendererInstance()->setVertexFormat( g_pMeshLayout );
    D3DXFROMWINEMATRIX mTrans;
    D3DXFROMWINEMATRIX mScale;

    for( int i = 0; i < g_NumBoxes; i++ )
    {
        D3DXFROMWINEMatrixScaling( &mScale, 20, 20, 20 );
        D3DXFROMWINEMatrixTranslation( &mTrans, g_pvBoxPositions[i].x, g_pvBoxPositions[i].y, g_pvBoxPositions[i].z );
        D3DXFROMWINEMATRIX mWorld = mScale * mTrans;
        D3DXFROMWINEMATRIX mWorldViewProjection = mWorld * ( *pmViewProj );

        // Set projection matrices
        ( g_pmWorldViewProjection->SetMatrix( ( float* )&mWorldViewProjection ) );
        ( g_pmWorld->SetMatrix( ( float* )&mWorld ) );
        g_BoxMesh.Render(pTechnique, g_ptxDiffuse);
	//g_BoxMesh.Draw(&g_Box, g_ptxDiffuse);
    }
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void  OnFrameRender( double fTime, float fElapsedTime )
{
    // Clear the render target and depth stencil
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.1f, 0.25f, 0.4f, 1.0f));

    D3DXFROMWINEMATRIX mWorldViewProjection;
    D3DXFROMWINEVECTOR4 vLightDir4;
    D3DXFROMWINEVECTOR4 vLightDiffuse;
    D3DXFROMWINEMATRIX mWorld;
    D3DXFROMWINEMATRIX mView;
    D3DXFROMWINEMATRIX mProj;
    D3DXFROMWINEVECTOR4 vEye4;

    D3DXFROMWINEVECTOR3 vEye = g_EyePt;
    mProj = g_ProjMatrix;
    mView = g_ViewMatrix;
    D3DXFROMWINEMATRIX mViewProj = mView * mProj;

    // Get and set light direction
    D3DXFROMWINEVECTOR3 vLightDir = g_LightControl[0];
    vLightDiffuse = g_fLightScale * D3DXFROMWINEVECTOR4( 1, 1, 1, 1 );
    g_pLightDiffuse->SetFloatVector( ( float* )&vLightDiffuse );
    g_pLightDir->SetFloatVector( ( float* )&vLightDir );

    // Create light frustum
    float fMapWidthFudge = g_fMapWidth * 1.2f;
    D3DXFROMWINEMATRIX mLightView;
    D3DXFROMWINEVECTOR3 vLightPos = vLightDir * g_fMapWidth * 1.2f;
    D3DXFROMWINEVECTOR3 vLightDirNeg = -vLightDir;
    D3DXFROMWINEVECTOR3 vUp( 0,1,0 );
    if( D3DXFROMWINEVec3Dot( &vUp, &vLightDirNeg ) > 0.99f )
        vUp = D3DXFROMWINEVECTOR3( 0, 0, 1 );
    D3DXFROMWINEMatrixLookAtLH( &mLightView, &vLightPos, &vLightDirNeg, &vUp );
    D3DXFROMWINEMATRIX mLightProj;
    D3DXFROMWINEMatrixOrthoLH( &mLightProj, fMapWidthFudge, fMapWidthFudge, 0.5f, g_fMapWidth * 2.4f );
    D3DXFROMWINEMATRIX mLightViewProj = mLightView * mLightProj;
    g_pmLightViewProj->SetMatrix( ( float* )&mLightViewProj );

    // shadow depthmap pass
    if( g_bRenderShadows )
    {
        // Setup render targets
        IRenderer::GetRendererInstance()->ClearDepthStencilView( g_pShadowDSV, eDEPTH, 1.0f, 0 );
        IRenderer::GetRendererInstance()->setViewportVrtl(0, 0, g_ShadowMapSize, g_ShadowMapSize, 0.0f, 1.0f);

        if( g_bRenderGeometry )
        {
            RenderTerrain( &mLightViewProj, g_pRenderGeometryTerrain );
        }
        else
        {
            RaycastTerrain( &mLightViewProj, &mLightViewProj, &vLightDir, &vLightPos, true );
        }
        RenderBoxes( &mLightViewProj, g_pRenderMesh );
        IRenderer::GetRendererInstance()->changeToMainFramebuffer();
        IRenderer::GetRendererInstance()->setViewport(0, 0, STX_Service::GetWindowInstance()->GetWidth(), STX_Service::GetWindowInstance()->GetHeight(), 0.0f, 1.0f);
    }

    // Foward pass
    g_ptxDepthMap->SetResource( g_pShadowSRV );
    if( g_bRenderGeometry )
    {
        if( g_bRenderShadows )
            RenderTerrain( &mViewProj, g_pRenderGeometryTerrain_Shadow );
        else
            RenderTerrain( &mViewProj, g_pRenderGeometryTerrain );
    }
    else
    {        
        RaycastTerrain( &mViewProj, &mLightViewProj, &vLightDir, &vEye, false );
    }

    if( g_bRenderShadows )
        RenderBoxes( &mViewProj, g_pRenderMesh_Shadow );
    else
        RenderBoxes( &mViewProj, g_pRenderMesh );
}
#if 0
//--------------------------------------------------------------------------------------
// This preprocessing code isn't used in the sample, but is provided for completeness.
// It shows one way to create the cone ratios for an input texture.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// For an explanation of the cone-step mapping technique see
// www.lonesock.net/files/ConeStepMapping.pdf
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
float GetMaxConeRatio( int StartX, int StartY, float* pBits, int Width, int Height )
{
    float fMinRatio = 27.0f; // Radius / DeltaHeight
    float fConeStartHeight = pBits[ StartY * Width + StartX ];
    float fStartX = StartX / ( float )Width;
    float fStartY = StartY / ( float )Height;
    float fY = 0;
    float fStepX = 1.0f / ( float )Width;
    float fStepY = 1.0f / ( float )Height;

    for( int y = 0; y < Height; y++ )
    {
        float fDeltaY = fStartY - fY;
        float fX = 0;
        for( int x = 0; x < Width; x++ )
        {
            if( StartX != x && StartY != y )
            {
                float fTestHeight = pBits[ y * Width + x ];
                float fHeightDeltaSq = fTestHeight - fConeStartHeight;

                if( fHeightDeltaSq > 0.00001f )
                {
                    float fDeltaX = fStartX - fX;
                    float fRadiusToTestSq = ( fDeltaX * fDeltaX + fDeltaY * fDeltaY );
                    fHeightDeltaSq *= fHeightDeltaSq;

                    if( fHeightDeltaSq * fMinRatio > fRadiusToTestSq )
                    {
                        fMinRatio = fRadiusToTestSq / fHeightDeltaSq;
                    }
                }
            }
            fX += fStepX;
        }
        fY += fStepY;
    }

    return min( 1, sqrtf( sqrtf( fMinRatio ) ) );
}
#endif
//--------------------------------------------------------------------------------------
int PreprocessTerrain( char* strHeightMap, char* strMapOut )
{
#if 0
    int hr = 0;
    void* hFile = CreateFile( strHeightMap, FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING,
                               FILE_FLAG_SEQUENTIAL_SCAN, 0 );
    if( INVALID_HANDLE_VALUE == hFile )
        return E_INVALIDARG;

    __DWORD__ dwBytesRead = 0;

    // read the bfh
    BITMAPFILEHEADER bfh;
    ReadFile( hFile, &bfh, sizeof( BITMAPFILEHEADER ), &dwBytesRead, 0 );
    unsigned long toBitmapData = bfh.bfOffBits;
    unsigned long bitmapSize = bfh.bfSize;

    // read the header
    BITMAPINFOHEADER bih;
    ReadFile( hFile, &bih, sizeof( BITMAPINFOHEADER ), &dwBytesRead, 0 );
    if( bih.biCompression != BI_RGB )
        goto Error;

    int HeightMapX;
    int HeightMapY;
    float* pHeightBits;

    // alloc memory
    int U = HeightMapX = bih.biWidth;
    int V = HeightMapY = abs( bih.biHeight );
    pHeightBits = new float[ U * V ];
    if( !pHeightBits )
        return 1;

    // find the step size
    int iStep = 4;
    if( 24 == bih.biBitCount )
        iStep = 3;

    // final check for size
    int UNew = ( ( U * iStep * 8 + 31 ) & ~31 ) / ( 8 * iStep );
    if( bitmapSize < UNew * V * iStep * sizeof( BYTE ) )
        goto Error;

    // seek
    LARGE_INTEGER liMove;
    liMove.QuadPart = toBitmapData;
    if( !SetFilePointerEx( hFile, liMove, 0, FILE_BEGIN ) )
        goto Error;

    // read in the bits
    BYTE* pBits = new BYTE[ bitmapSize ];
    if( !pBits )
        return 1;
    ReadFile( hFile, pBits, bitmapSize * sizeof( BYTE ), &dwBytesRead, 0 );

    CloseHandle( hFile );

    // Load the Height Information
    int iHeight = 0;
    int iBitmap = 0;
    for( int y = 0; y < V; y++ )
    {
        iBitmap = y * UNew * iStep;
        for( int x = 0; x < U * iStep; x += iStep )
        {
            pHeightBits[iHeight] = pBits[ iBitmap ] / 255.0f;

            iHeight ++;
            iBitmap += iStep;
        }
    }

    SAFE_DELETE_ARRAY( pBits );

    // Now create the cone ratios
    float* pConeRatios = new float[ U * V ];
    if( !pConeRatios )
        return 1;

    char str[100];
    int index = 0;
    for( int y = 0; y < V; y++ )
    {
        for( int x = 0; x < U; x++ )
        {
            pConeRatios[index] = GetMaxConeRatio( x, y, pHeightBits, U, V );
            index ++;
        }
        swprintf_s( str, 100, "Line: %d\n", y );
        OutputDebugString( str );
    }

    // Put it all into one texture
    TEXTURE2D_DESC Desc;
    Desc.Width = U;
    Desc.Height = V;
    Desc.MipLevels = 1;
    Desc.elementsOf = 1;
    Desc.Format = FORMAT_RGBA8;
    Desc.SampleDesc.Count = 1;
    Desc.SampleDesc.Quality = 0;
    Desc.Usage = USAGE_IMMUTABLE;
    Desc.BindFlags = BIND_SHADER_RESOURCE;
    Desc.CPUAccessFlags = 0;
    Desc.MiscFlags = 0;

    
    BYTE* pSysData = new BYTE[ U * V * 4 ];
    InitData.pSysMem = pSysData;
    InitData.SysMemPitch = V * 4;

    index = 0;
    for( int i = 0; i < U * V * 4; i += 4 )
    {
        pSysData[i    ] = ( BYTE )( pHeightBits[index] * 255.0f );

        // cone ratios
        pSysData[i + 1] = ( BYTE )( pConeRatios[index] * 255.0f );

        index ++;
    }

    TextureID pTerrainTexture = -1;
	{
		Image3 img;
		img.create(FORMAT_RGBA8, U, V, 1, 1);
		img.setPixels(pSysData);
		pTerrainTexture=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear()); 
	}

    SAFE_DELETE_ARRAY( InitData.pSysMem );

    // Save it out to a file
    D3DXFROMWINE10SaveTextureToFile( pTerrainTexture, D3DXFROMWINE10_IFF_DDS, strMapOut );

    //////////////SAFE_RELEASE( pTerrainTexture );
    SAFE_DELETE_ARRAY( pHeightBits );
    SAFE_DELETE_ARRAY( pConeRatios );
    return 0;

Error:
    CloseHandle( hFile );
#endif
    return 1;

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
	IRenderer* r=IRenderer::GetRendererInstance("RaycastTerrain");
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

