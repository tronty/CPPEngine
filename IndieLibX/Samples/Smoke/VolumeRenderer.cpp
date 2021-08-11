/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//----------------------------------------------------------------------------------
// File:   VolumeRenderer.cpp
// Author: Ignacio Llamas and Chris (Wei-Tae) Kim and Sarah Tariq 
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

#include "nvutPCH.h"
#include "Smoke.h"
#include "SDKmisc.h"
#include "VolumeRenderer.h"
#pragma warning(disable:4995)
#include <iostream>
#include <fstream>
#include <math.h>
using namespace std;


struct VsInput
{
    D3DXFROMWINEVECTOR3 pos;  
};

// constructor
VolumeRenderer::VolumeRenderer( ) : 
    IRenderer::GetRendererInstance()(0), m_maxDim(0), m_pEffect(0), m_pTechnique(0),
    m_evTexture_rayData(0), m_evTexture_rayDataSmall(0), m_evTexture_volume(0),
    m_evTexture_rayCast(0), m_evTexture_edge(0), m_evTexture_glow(0), m_evTexture_sceneDepthTex(0),
    m_evRTWidth(0), m_evRTHeight(0), m_evUseGlow(0),
    m_evWorldViewProjectionMatrix(0), m_evInvWorldViewProjectionMatrix(0), m_evGrid2WorldMatrix(0),
    m_evZNear(0), m_evZFar(0), m_evGridScaleFactor(0), m_evEyeOnGrid(0),
    m_evGlowContribution(0), m_evFinalIntensityScale(0), m_evFinalAlphaScale(0), 
    m_evSmokeColorMultiplier(0), m_evsmokeAlphaMultiplier(0), m_evRednessFactor(0),
    m_evWorldViewMatrix(0), m_evTan_FovYhalf(0), m_evTan_FovXhalf(0),
    m_evRaycastBisection(0), m_evRaycastFilterTricubic(0), m_evRaycastShaderAsWater(0),
    m_epQuadRaycastLevelSet(0), m_epQuadRaycastFire(0), m_epQuadRaycastSmoke(0), 
    m_pGridBoxLayout(0), m_pGridBoxVertexBuffer(0), m_pGridBoxIndexBuffer(0), 
    m_pQuadLayout(0), m_pQuadVertexBuffer(0), 
    m_pRayDataSmallTex2D(0), m_pRayDataSmallRTV(0),m_pRayDataSmallSRV(0), m_pGlowTex2D(0), m_pGlowTempTex2D(0),m_pGlowSRV(0),m_pGlowTempSRV(0),
    m_pRayCastTex2D(0), m_pRayCastRTV(0), m_pRayCastSRV(0),
    m_pEdgeTex2D(0), m_pEdgeSRV(0), m_pEdgeRTV(0), m_pGlowRTV(0), m_pGlowTempRTV(0),
    m_bRaycastBisection(true), m_bRaycastFilterTricubic(true), m_bRaycastShadeAsWater(true), m_eRenderMode(RM_SMOKE),
    m_renderTextureWidth(0), m_renderTextureHeight(0), m_useFP32Blending(true)
{
    SAFE_ACQUIRE(IRenderer::GetRendererInstance(), IRenderer::GetRendererInstance());
    
    stx_memset(m_gridDim,0, sizeof(m_gridDim));
    stx_memset(pRayDataTex2D, 0, sizeof(pRayDataTex2D));
    stx_memset(m_pRayDataRTV, 0, sizeof(m_pRayDataRTV));
    stx_memset(m_pRayDataSRV, 0, sizeof(m_pRayDataSRV));

    D3DXFROMWINEMatrixIdentity(&m_gridMatrix);
}

// destructor
VolumeRenderer::~VolumeRenderer( void )
{ 
    //////////////SAFE_RELEASE(IRenderer::GetRendererInstance());

    Cleanup();
}

void VolumeRenderer::Cleanup( void )
{
    
    //////////////SAFE_RELEASE(m_pEffect);
    
    //////////////SAFE_RELEASE(m_pGridBoxLayout);
    //////////////SAFE_RELEASE(m_pGridBoxVertexBuffer);
    //////////////SAFE_RELEASE(m_pGridBoxIndexBuffer);
    
    //////////////SAFE_RELEASE(m_pQuadLayout);
    //////////////SAFE_RELEASE(m_pQuadVertexBuffer);
    
    //////////////SAFE_RELEASE(pRayDataTex2D[0]);
    //////////////SAFE_RELEASE(pRayDataTex2D[1]);
    //////////////SAFE_RELEASE(m_pRayDataSRV[0]);
    //////////////SAFE_RELEASE(m_pRayDataSRV[1]);
    //////////////SAFE_RELEASE(m_pRayDataRTV[0]);
    //////////////SAFE_RELEASE(m_pRayDataRTV[1]);
    //////////////SAFE_RELEASE(m_pRayDataSmallTex2D);
    //////////////SAFE_RELEASE(m_pRayDataSmallRTV);
    //////////////SAFE_RELEASE(m_pRayDataSmallSRV);
    //////////////SAFE_RELEASE(m_pRayCastTex2D);
    //////////////SAFE_RELEASE(m_pRayCastSRV);
    //////////////SAFE_RELEASE(m_pRayCastRTV);
    //////////////SAFE_RELEASE(m_pGlowTex2D);
    //////////////SAFE_RELEASE(m_pGlowSRV);
    //////////////SAFE_RELEASE(m_pGlowRTV);
    //////////////SAFE_RELEASE(m_pGlowTempTex2D);
    //////////////SAFE_RELEASE(m_pGlowTempSRV);
    //////////////SAFE_RELEASE(m_pGlowTempRTV);
    //////////////SAFE_RELEASE(m_pEdgeTex2D); 
    //////////////SAFE_RELEASE(m_pEdgeSRV);
    //////////////SAFE_RELEASE(m_pEdgeRTV);
    
}


int VolumeRenderer::Initialize(int gridWidth, int gridHeight, int gridDepth, RENDER_MODE renderMode)
{
    int hr(0);
    
    Cleanup();

    m_eRenderMode = renderMode;

    m_gridDim[0] = float(gridWidth);
    m_gridDim[1] = float(gridHeight);
    m_gridDim[2] = float(gridDepth);

    m_maxDim = max( max( m_gridDim[0], m_gridDim[1] ), m_gridDim[2] );

    // Initialize the grid offset matrix
    {
        // Make a scale matrix to scale the unit-sided box to be unit-length on the 
        //  side/s with maximum dimension 
        D3DXFROMWINEMATRIX scaleM;
        D3DXFROMWINEMatrixIdentity(&scaleM);
        D3DXFROMWINEMatrixScaling(&scaleM, m_gridDim[0] / m_maxDim, m_gridDim[1] / m_maxDim, m_gridDim[2] / m_maxDim);
        // offset grid to be centered at origin
        D3DXFROMWINEMATRIX translationM;
        D3DXFROMWINEMatrixTranslation(&translationM, -0.5, -0.5, -0.5);

        m_gridMatrix = translationM * scaleM;
    }

    // Check if the device supports FP32 blending to choose the right codepath depending on this
    unsigned int rgba32fFormatSupport;
    IRenderer::GetRendererInstance()->CheckFormatSupport(DXGI_FORMAT_R32G32B32A32_FLOAT, &rgba32fFormatSupport);
    m_useFP32Blending = (rgba32fFormatSupport & D3D10_FORMAT_SUPPORT_BLENDABLE) ? true : false;
    m_useFP32Blending = false;

    (initShaders());
    (createGridBox());
    (createScreenQuad());

    (createJitterTexture());
    (loadTextureFromFile(IRenderer::GetRendererInstance(), "..\\..\\Media\\FireTransferFunction.dds", m_pEffect, m_effectPath, "fireTransferFunction"));


    return hr;
}


void VolumeRenderer::Draw(TextureID * pSourceTexSRV)
{
    m_evTexture_volume->SetResource(pSourceTexSRV);
    // Set some variables required by the shaders:
    //=========================================================================

    m_evGlowContribution->SetFloat(g_glowContribution);
    m_evFinalIntensityScale->SetFloat(g_finalIntensityScale);
    m_evFinalAlphaScale->SetFloat(g_finalAlphaScale);
    m_evSmokeColorMultiplier->SetFloat(g_smokeColorMultiplier);
    m_evsmokeAlphaMultiplier->SetFloat(g_smokeAlphaMultiplier);
    m_evRednessFactor->SetInt(g_RednessFactor);

    // The near and far planes are used to unproject the scene's z-buffer values
    m_evZNear->SetFloat(g_zNear);
    m_evZFar->SetFloat(g_zFar);

    D3DMATRIX grid2World = m_gridMatrix * g_gridWorld;
    m_evGrid2WorldMatrix->SetMatrix( (float*) &grid2World );

    D3DXFROMWINEMATRIX worldView = g_gridWorld * g_View;

    // The length of one of the axis of the worldView matrix is the length of longest side of the box
    //  in view space. This is used to convert the length of a ray from view space to grid space.
    D3DXFROMWINEVECTOR3 worldXaxis = D3DXFROMWINEVECTOR3(worldView._11, worldView._12, worldView._13);
    float worldScale = D3DXFROMWINEVec3Length(&worldXaxis);
    m_evGridScaleFactor->SetFloat( worldScale );

    // We prepend the current world matrix with this other matrix which adds an offset (-0.5, -0.5, -0.5)
    //  and scale factors to account for unequal number of voxels on different sides of the volume box. 
    // This is because we want to preserve the aspect ratio of the original simulation grid when 
    //  raytracing through it, and this matrix allows us to do the raytracing in grid (texture) space: 
    //  i.e. each side of the box spans the 0 to 1 range
    worldView = m_gridMatrix * worldView;

    m_evWorldViewMatrix->SetMatrix( (float*)&worldView );
    m_evTan_FovYhalf->SetFloat( tan(g_Fovy/2.0f) );
    m_evTan_FovXhalf->SetFloat( tan(g_Fovy/2.0f)*m_renderTextureWidth/m_renderTextureHeight );

    // options for the LevelSet raytracer
    m_evRaycastBisection->SetBool(m_bRaycastBisection);
    m_evRaycastFilterTricubic->SetBool(m_bRaycastFilterTricubic);
    m_evRaycastShaderAsWater->SetBool(m_bRaycastShadeAsWater);

	// worldViewProjection is used to transform the volume box to screen space
    D3DXFROMWINEMATRIX worldViewProjection;
    worldViewProjection = worldView * g_Projection;
    m_evWorldViewProjectionMatrix->SetMatrix( (float*)&worldViewProjection );

    // invWorldViewProjection is used to transform positions in the "near" plane into grid space
    D3DXFROMWINEMATRIX invWorldViewProjection;
    D3DXFROMWINEMatrixInverse(&invWorldViewProjection, 0, &worldViewProjection);
    m_evInvWorldViewProjectionMatrix->SetMatrix((float*)&invWorldViewProjection);

    // Compute the inverse of the worldView matrix 
    D3DXFROMWINEMATRIX worldViewInv;
    D3DXFROMWINEMatrixInverse(&worldViewInv, 0, &worldView);
    // Compute the eye's position in "grid space" (the 0-1 texture coordinate cube)
    D3DXFROMWINEVECTOR4 eyeInGridSpace;
    D3DXFROMWINEVECTOR3 origin(0,0,0);
    D3DXFROMWINEVec3Transform(&eyeInGridSpace, &origin, &worldViewInv);
    m_evEyeOnGrid->SetFloatVector((float*)&eyeInGridSpace);

    float color[4] = {0, 0, 0, 0 };


    // Ray cast and render to a temporary buffer
    //=========================================================================

    // Partial init of viewport struct used below
    D3DFROMWINEVIEWPORT9 rtViewport;
    rtViewport.TopLeftX = 0;
    rtViewport.TopLeftY = 0;
    rtViewport.MinDepth = 0;
    rtViewport.MaxDepth = 1;


    // Compute the ray data required by the raycasting pass below.
    //  This function will render to a buffer of float4 vectors, where
    //  xyz is starting position of the ray in grid space
    //  w is the length of the ray in view space
    computeRayData();


    // Do edge detection on this image to find any 
    //  problematic areas where we need to raycast at higher resolution
    computeEdgeTexture();
    

    // Raycast into the temporary render target: 
    //  raycasting is done at the smaller resolution, using a fullscreen quad
    IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pRayCastRTV, color );
    IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &m_pRayCastRTV , 0 ); 

    rtViewport.Width = m_renderTextureWidth;
    rtViewport.Height = m_renderTextureHeight;
    IRenderer::GetRendererInstance()->RSSetViewports(1,&rtViewport);

    m_evRTWidth->SetFloat((float)m_renderTextureWidth);
    m_evRTHeight->SetFloat((float)m_renderTextureHeight);

    m_evTexture_rayDataSmall->SetResource(m_pRayDataSmallSRV);
   
    if( m_eRenderMode == RM_LEVELSET )
        m_epQuadRaycastLevelSet->Apply(0);
    else if(m_eRenderMode == RM_FIRE)
        m_epQuadRaycastFire->Apply(0);
    else
        m_epQuadRaycastSmoke->Apply(0);

    drawScreenQuad();

   
    IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pGlowRTV, color );

    //blur the raycast image to get a blur texture
    m_evUseGlow->SetBool(g_bRenderGlow);
    if((m_eRenderMode == RM_FIRE) && g_bRenderGlow)
    {
        IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pGlowTempRTV, color );
        IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &m_pGlowTempRTV , 0 ); 
        m_evTexture_glow->SetResource(m_pRayCastSRV);
        m_epGlowHorizontal->Apply(0);
        drawScreenQuad();
        
        IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &m_pGlowRTV , 0 ); 
        m_evTexture_glow->SetResource(m_pGlowTempSRV);
        m_epGlowVertical->Apply(0);
        drawScreenQuad();
        
    }


    // Render to the back buffer sampling from the raycast texture that we just created
    // If and edge was detected at the current pixel we will raycast again to avoid
    // smoke aliasing artifacts at scene edges
    TextureID* pRTV = DXUTGetD3D10RenderTargetView();
    TextureID* pDSV = DXUTGetD3D10DepthStencilView();
    IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &pRTV , pDSV ); 

    rtViewport.Width = g_Width;
    rtViewport.Height = g_Height;
    IRenderer::GetRendererInstance()->RSSetViewports(1,&rtViewport);

    m_evRTWidth->SetFloat((float)g_Width);
    m_evRTHeight->SetFloat((float)g_Height);

    m_evTexture_rayCast->SetResource(m_pRayCastSRV);
    m_evTexture_edge->SetResource(m_pEdgeSRV);
    m_evTexture_glow->SetResource(m_pGlowSRV);
    
    if( m_eRenderMode == RM_LEVELSET )
        m_epQuadRaycastUpsampleLevelSet->Apply(0);
    else if(m_eRenderMode == RM_FIRE)
        m_epQuadRaycastUpsampleFire->Apply(0);
    else
        m_epQuadRaycastUpsampleSmoke->Apply(0);

    drawScreenQuad();

    m_evTexture_rayCast->SetResource(0);
    m_evTexture_edge->SetResource(0);
    m_evTexture_glow->SetResource(0);
    m_epQuadRaycastUpsampleFire->Apply(0);

}

void VolumeRenderer::computeRayData( void )
{
    // Clear the color buffer to 0
    float blackColor[4] = {0, 0, 0, 0 };
    IRenderer::GetRendererInstance()->ClearRenderTargetView(m_pRayDataRTV[0], blackColor);
    if( !m_useFP32Blending )
        IRenderer::GetRendererInstance()->ClearRenderTargetView(m_pRayDataRTV[1], blackColor);

    IRenderer::GetRendererInstance()->OMSetRenderTargets(1, &m_pRayDataRTV[0], 0);

    m_evTexture_sceneDepthTex->SetResource(g_pSceneDepthSRV);
    
    // Setup viewport to match the window's backbuffer
    D3DFROMWINEVIEWPORT9 rtViewport;
    rtViewport.TopLeftX = 0;
    rtViewport.TopLeftY = 0;
    rtViewport.MinDepth = 0;
    rtViewport.MaxDepth = 1;
    rtViewport.Width = g_Width;
    rtViewport.Height = g_Height;
    IRenderer::GetRendererInstance()->RSSetViewports(1,&rtViewport);
    m_evRTWidth->SetFloat((float)g_Width);
    m_evRTHeight->SetFloat((float)g_Height);

    // Render volume back faces
    // We output xyz=(0,-1,0) and w=min(sceneDepth, boxDepth)
    m_epCompRayData_Back->Apply(0);
    drawBox();

    if( !m_useFP32Blending )
    {
        // repeat the back face pass in m_pRayDataRTV[1] - we could also do CopySubResource
        IRenderer::GetRendererInstance()->OMSetRenderTargets(1, &m_pRayDataRTV[1], 0);
        drawBox();
    }

    // Render volume front faces using subtractive blending or doing texture lookups
    //  depending on hw support for FP32 blending. Note that an FP16 RGBA buffer is
    //  does not have enough precision to represent the different XYZ postions 
    //  for each ray entry point in most common circumstances.
    // We output xyz="position in grid space" and w=boxDepth,
    //  unless the pixel is occluded by the scene, in which case we output xyzw=(1,0,0,0)
    if( m_useFP32Blending )
    {
        m_epCompRayData_Front->Apply(0);
    }
    else
    {
        m_evTexture_rayData->SetResource(m_pRayDataSRV[0]);
        m_epCompRayData_FrontNOBLEND->Apply(0);
    }
    drawBox();

}

void VolumeRenderer::computeEdgeTexture(void)
{
    // First setup viewport to match the size of the destination low-res texture
    D3DFROMWINEVIEWPORT9 rtViewport;
    rtViewport.TopLeftX = 0;
    rtViewport.TopLeftY = 0;
    rtViewport.MinDepth = 0;
    rtViewport.MaxDepth = 1;
    rtViewport.Width = m_renderTextureWidth;
    rtViewport.Height = m_renderTextureHeight;
    IRenderer::GetRendererInstance()->RSSetViewports(1,&rtViewport);
    m_evRTWidth->SetFloat((float)m_renderTextureWidth);
    m_evRTHeight->SetFloat((float)m_renderTextureHeight);

    // Downsample the rayDataTexture to a new small texture, simply using point sample (no filtering)
    IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &m_pRayDataSmallRTV , 0 ); 
    if( m_useFP32Blending )
        m_evTexture_rayData->SetResource(m_pRayDataSRV[0]);
    else
        m_evTexture_rayData->SetResource(m_pRayDataSRV[1]);
    m_epQuadDownSampleRayDataTexture->Apply(0);
    drawScreenQuad();

    // Create an edge texture, performing edge detection on 'rayDataTexSmall'
    IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &m_pEdgeRTV , 0 ); 
    m_evTexture_rayDataSmall->SetResource(m_pRayDataSmallSRV);
    m_epQuadEdgeDetect->Apply(0);
    drawScreenQuad();
}

void VolumeRenderer::drawBox(void)
{
    unsigned int stride = sizeof( VsInput );
    unsigned int offset = 0;
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1, &m_pGridBoxVertexBuffer, &stride, &offset );
    IRenderer::GetRendererInstance()->setIndexBuffer( m_pGridBoxIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
    IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLES );
    IRenderer::GetRendererInstance()->IASetInputLayout(m_pGridBoxLayout);
    IRenderer::GetRendererInstance()->DrawIndexed(36, 0, 0);
}


void VolumeRenderer::drawScreenQuad(void)
{
    unsigned int strides = sizeof(VsInput);
    unsigned int offsets = 0;
    IRenderer::GetRendererInstance()->IASetInputLayout( m_pQuadLayout );
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1, &m_pQuadVertexBuffer, &strides, &offsets );
    IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLE_STRIP );
    IRenderer::GetRendererInstance()->Draw( 4, 0 );
}

int VolumeRenderer::SetScreenSize( int width, int height )
{
    int hr(0);

    (createRayDataResources(width, height));
    return 0;
}

// Resource initialization functions below

int VolumeRenderer::initShaders(  )
{
    int hr(0);
    __DWORD__ dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
    
    (NVUTFindDXSDKMediaFileCchT( m_effectPath, MAX_PATH, "VolumeRenderer.fx" ) );
    (D3DXFROMWINE10CreateEffectFromFile(m_effectPath, g_pShadersMacros, 0, "fx_4_0", dwShaderFlags, 0, IRenderer::GetRendererInstance(), 0, 0, &m_pEffect, 0, &hr ));

    V_GET_TECHNIQUE_RET(m_pEffect, m_effectPath, m_pTechnique, "VolumeRenderer");

    (m_pEffect, m_effectPath, m_evTexture_rayData, "rayDataTex", AsShaderResource);
    (m_pEffect, m_effectPath, m_evTexture_rayDataSmall, "rayDataTexSmall", AsShaderResource);
    (m_pEffect, m_effectPath, m_evTexture_volume, "volumeTex", AsShaderResource);
    (m_pEffect, m_effectPath, m_evTexture_rayCast, "rayCastTex", AsShaderResource);
    (m_pEffect, m_effectPath, m_evTexture_edge, "edgeTex", AsShaderResource);
    (m_pEffect, m_effectPath, m_evTexture_glow, "glowTex", AsShaderResource);
    (m_pEffect, m_effectPath, m_evTexture_sceneDepthTex, "sceneDepthTex", AsShaderResource );

    (m_pEffect, m_effectPath, m_evRTWidth, "RTWidth", AsScalar);
    (m_pEffect, m_effectPath, m_evRTHeight, "RTHeight", AsScalar);
    (m_pEffect, m_effectPath, m_evUseGlow, "useGlow", AsScalar);
    (m_pEffect, m_effectPath, m_evWorldViewProjectionMatrix, "WorldViewProjection", AsMatrix);
    (m_pEffect, m_effectPath, m_evInvWorldViewProjectionMatrix, "InvWorldViewProjection", AsMatrix);
    (m_pEffect, m_effectPath, m_evGrid2WorldMatrix, "Grid2World", AsMatrix);
    (m_pEffect, m_effectPath, m_evZNear, "ZNear", AsScalar);
    (m_pEffect, m_effectPath, m_evZFar, "ZFar", AsScalar);
    (m_pEffect, m_effectPath, m_evGridScaleFactor,  "gridScaleFactor", AsScalar);
    (m_pEffect, m_effectPath, m_evEyeOnGrid, "eyeOnGrid", AsVector);

    (m_pEffect, m_effectPath, m_evGlowContribution, "glowContribution", AsScalar);
    (m_pEffect, m_effectPath, m_evFinalIntensityScale, "finalIntensityScale", AsScalar);
    (m_pEffect, m_effectPath, m_evFinalAlphaScale, "finalAlphaScale", AsScalar);
    (m_pEffect, m_effectPath, m_evSmokeColorMultiplier, "smokeColorMultiplier", AsScalar);
    (m_pEffect, m_effectPath, m_evsmokeAlphaMultiplier, "smokeAlphaMultiplier", AsScalar);
    (m_pEffect, m_effectPath, m_evRednessFactor, "rednessFactor", AsScalar);

    (m_pEffect, m_effectPath, m_evWorldViewMatrix, "WorldView", AsMatrix);
    (m_pEffect, m_effectPath, m_evTan_FovYhalf, "tan_FovYhalf", AsScalar);
    (m_pEffect, m_effectPath, m_evTan_FovXhalf, "tan_FovXhalf", AsScalar);
    (m_pEffect, m_effectPath, m_evRaycastBisection, "g_bRaycastBisection", AsScalar);
    (m_pEffect, m_effectPath, m_evRaycastFilterTricubic, "g_bRaycastFilterTricubic", AsScalar);
    (m_pEffect, m_effectPath, m_evRaycastShaderAsWater, "g_bRaycastShadeAsWater", AsScalar);

    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epQuadRaycastLevelSet, "QuadRaycastLevelSet");
    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epQuadRaycastFire, "QuadRaycastFire");
    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epQuadRaycastSmoke, "QuadRaycastSmoke");
    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epGlowHorizontal, "GlowHorizontal");
    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epGlowVertical, "GlowVertical");
    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epQuadRaycastUpsampleLevelSet, "QuadRaycastUpsampleLevelSet");
    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epQuadRaycastUpsampleFire, "QuadRaycastUpsampleFire");
    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epQuadRaycastUpsampleSmoke, "QuadRaycastUpsampleSmoke");
    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epCompRayData_Back, "CompRayData_Back");
    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epCompRayData_Front, "CompRayData_Front");
    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epCompRayData_FrontNOBLEND, "CompRayData_FrontNOBLEND");
    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epQuadDownSampleRayDataTexture, "QuadDownSampleRayDataTexture");
    V_GET_PASS_RET(m_pTechnique, m_effectPath, m_epQuadEdgeDetect, "QuadEdgeDetect");


    D3DXFROMWINEVECTOR3 recGridDim(1.0f/m_gridDim[0], 1.0f/m_gridDim[1], 1.0f/m_gridDim[2]);
    
    stx_Variable *evGridDim;
    stx_Variable *evRecGridDim;
    stx_Variable *evMaxGridDim;
    stx_ShaderResourceVariableID  *evTexture_envMap;

    (m_pEffect, m_effectPath, evGridDim, "gridDim", AsVector);
    (m_pEffect, m_effectPath, evRecGridDim, "recGridDim", AsVector);
    (m_pEffect, m_effectPath, evMaxGridDim, "maxGridDim", AsScalar);
    (m_pEffect, m_effectPath, evTexture_envMap, "envMapTex", AsShaderResource);
    
    evGridDim->SetFloatVector(m_gridDim);
    evRecGridDim->SetFloatVector(recGridDim);
    evMaxGridDim->SetFloat(m_maxDim);
    evTexture_envMap->SetResource(g_pEnvMapSRV);

    return 0;
}

int VolumeRenderer::createGridBox (void) 
{
    int hr(0);

    VsInput vertices[] =
    {
        { D3DXFROMWINEVECTOR3( 0, 0, 0 ) },
        { D3DXFROMWINEVECTOR3( 0, 0, 1 ) },
        { D3DXFROMWINEVECTOR3( 0, 1, 0 ) },
        { D3DXFROMWINEVECTOR3( 0, 1, 1 ) },
        { D3DXFROMWINEVECTOR3( 1, 0, 0 ) },
        { D3DXFROMWINEVECTOR3( 1, 0, 1 ) },
        { D3DXFROMWINEVECTOR3( 1, 1, 0 ) },
        { D3DXFROMWINEVECTOR3( 1, 1, 1 ) },
    };

    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices;
    ( IRenderer::GetRendererInstance()->CreateBuffer( &bd, &InitData, &m_pGridBoxVertexBuffer ) );


    // Create index buffer
    __DWORD__ indices[] =
    {
        0, 4, 1, 1, 4, 5,
        0, 1, 2, 2, 1, 3,
        4, 6, 5, 6, 7, 5,
        2, 3, 6, 3, 7, 6,
        1, 5, 3, 3, 5, 7,
        0, 2, 4, 2, 6, 4
    };

    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(indices);
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    InitData.pSysMem = indices;
    ( IRenderer::GetRendererInstance()->CreateBuffer( &bd, &InitData, &m_pGridBoxIndexBuffer ) );

    // Define the input layout
    D3D10_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
    };
    unsigned int numElements = sizeof(layout)/sizeof(layout[0]);

    // Create the input layout
    D3D10_PASS_DESC PassDesc;
    (m_epCompRayData_Back->GetDesc(&PassDesc));
    ( IRenderer::GetRendererInstance()->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pGridBoxLayout ) );

    return hr;
}


int VolumeRenderer::createScreenQuad(void ) 
{
    int hr(0);
    // Create our quad input layout
    const D3D10_INPUT_ELEMENT_DESC quadlayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };
    unsigned int numElements = sizeof(quadlayout)/sizeof(quadlayout[0]);

    // Create the input layout
    D3D10_PASS_DESC PassDesc;
    (m_epQuadRaycastSmoke->GetDesc( &PassDesc ));
    (IRenderer::GetRendererInstance()->CreateInputLayout( quadlayout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pQuadLayout ));

    // Create a screen quad for all render to texture operations
    VsInput svQuad[4];
    svQuad[0].pos = D3DXFROMWINEVECTOR3(-1.0f, 1.0f, 0.0f );
    svQuad[1].pos = D3DXFROMWINEVECTOR3(1.0f, 1.0f, 0.0f );
    svQuad[2].pos = D3DXFROMWINEVECTOR3(-1.0f, -1.0f, 0.0f );
    svQuad[3].pos = D3DXFROMWINEVECTOR3(1.0f, -1.0f, 0.0f );

    D3D10_BUFFER_DESC vbdesc =
    {
        4*sizeof(VsInput),
        D3D10_USAGE_DEFAULT,
        D3D10_BIND_VERTEX_BUFFER,
        0,
        0
    };

    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = svQuad;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;
    ( IRenderer::GetRendererInstance()->CreateBuffer( &vbdesc, &InitData, &m_pQuadVertexBuffer ) );

    return hr;
}

void VolumeRenderer::calculateRenderTextureSize(int screenWidth, int screenHeight)
{
    int maxProjectedSide = int(3.0 * sqrt(3.0)*m_maxDim);
    int maxScreenDim = max(screenWidth, screenHeight);
    
    float screenAspectRatio = ((float)screenWidth)/screenHeight;

    if( maxScreenDim > maxProjectedSide)
    {
        if(screenHeight > screenWidth)
        {
            m_renderTextureHeight = maxProjectedSide;
            m_renderTextureWidth = (int)(screenAspectRatio * maxProjectedSide);
        }
        else
        {
            m_renderTextureWidth = maxProjectedSide;
            m_renderTextureHeight = (int)((1.0f/screenAspectRatio) * maxProjectedSide);
        }
    }
    else
    {
        m_renderTextureWidth = screenWidth;
        m_renderTextureHeight = screenHeight;
    }
}

int VolumeRenderer::createRayDataResources( int width, int height )
{
    int hr(0);

    // clean up all resources first
    //////////////SAFE_RELEASE(pRayDataTex2D[0]);
    //////////////SAFE_RELEASE(pRayDataTex2D[1]);
    //////////////SAFE_RELEASE(m_pRayDataSRV[0]);
    //////////////SAFE_RELEASE(m_pRayDataSRV[1]);
    //////////////SAFE_RELEASE(m_pRayDataRTV[0]);
    //////////////SAFE_RELEASE(m_pRayDataRTV[1]);
    //////////////SAFE_RELEASE(m_pRayDataSmallTex2D);
    //////////////SAFE_RELEASE(m_pRayDataSmallSRV);
    //////////////SAFE_RELEASE(m_pRayDataSmallRTV);
    //////////////SAFE_RELEASE(m_pRayCastTex2D);
    //////////////SAFE_RELEASE(m_pRayCastSRV);
    //////////////SAFE_RELEASE(m_pRayCastRTV);
    //////////////SAFE_RELEASE(m_pEdgeTex2D);
    //////////////SAFE_RELEASE(m_pEdgeSRV);
    //////////////SAFE_RELEASE(m_pEdgeRTV);
    //////////////SAFE_RELEASE(m_pGlowTex2D);
    //////////////SAFE_RELEASE(m_pGlowSRV);
    //////////////SAFE_RELEASE(m_pGlowRTV);
    //////////////SAFE_RELEASE(m_pGlowTempTex2D);
    //////////////SAFE_RELEASE(m_pGlowTempSRV);
    //////////////SAFE_RELEASE(m_pGlowTempRTV);


    // find a good resolution for raycasting purposes
    calculateRenderTextureSize(width, height);

    DXGI_FORMAT rayDataFmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
    
    //create the textures
    SURFACE_DESC desc;
    desc.ArraySize = 1;
    desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
    desc.CPUAccessFlags = 0;
    desc.MipLevels = 1;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D10_USAGE_DEFAULT;
    desc.Width = width;
    desc.Height = height;
    desc.Format = rayDataFmt;
    (IRenderer::GetRendererInstance()->CreateTexture2D(&desc,0, &pRayDataTex2D[0]));
    if( !m_useFP32Blending )
        (IRenderer::GetRendererInstance()->CreateTexture2D(&desc,0, &pRayDataTex2D[1]));
    desc.Width = m_renderTextureWidth;
    desc.Height = m_renderTextureHeight;
    (IRenderer::GetRendererInstance()->CreateTexture2D(&desc,0,&m_pRayDataSmallTex2D));
    (IRenderer::GetRendererInstance()->CreateTexture2D(&desc,0,&m_pRayCastTex2D));
    (IRenderer::GetRendererInstance()->CreateTexture2D(&desc,0,&m_pGlowTex2D));
    (IRenderer::GetRendererInstance()->CreateTexture2D(&desc,0,&m_pGlowTempTex2D));
    desc.Format = DXGI_FORMAT_R32_FLOAT;
    (IRenderer::GetRendererInstance()->CreateTexture2D(&desc,0,&m_pEdgeTex2D));

    //create the render target views
    SURFACE_DESC DescRT;
    DescRT.Format = rayDataFmt;
    DescRT.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
    DescRT.Texture2D.MipSlice = 0;
    ( IRenderer::GetRendererInstance()->CreateRenderTargetView(pRayDataTex2D[0], &DescRT, &m_pRayDataRTV[0]));
    if( !m_useFP32Blending )
        ( IRenderer::GetRendererInstance()->CreateRenderTargetView(pRayDataTex2D[1], &DescRT, &m_pRayDataRTV[1]));
    ( IRenderer::GetRendererInstance()->CreateRenderTargetView(m_pRayDataSmallTex2D, &DescRT, &m_pRayDataSmallRTV));   
    ( IRenderer::GetRendererInstance()->CreateRenderTargetView(m_pRayCastTex2D, &DescRT, &m_pRayCastRTV));
    ( IRenderer::GetRendererInstance()->CreateRenderTargetView(m_pGlowTex2D, &DescRT, &m_pGlowRTV));
    ( IRenderer::GetRendererInstance()->CreateRenderTargetView(m_pGlowTempTex2D, &DescRT, &m_pGlowTempRTV));
    DescRT.Format = DXGI_FORMAT_R32_FLOAT;
    ( IRenderer::GetRendererInstance()->CreateRenderTargetView(m_pEdgeTex2D, &DescRT, &m_pEdgeRTV));

    //create the shader resource views
    D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
    ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
    SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MostDetailedMip = 0;
    SRVDesc.Texture2D.MipLevels = 1;
    SRVDesc.Format = rayDataFmt;
    (IRenderer::GetRendererInstance()->CreateShaderResourceView(pRayDataTex2D[0], &SRVDesc, &m_pRayDataSRV[0]));
    if( !m_useFP32Blending )
        (IRenderer::GetRendererInstance()->CreateShaderResourceView(pRayDataTex2D[1], &SRVDesc, &m_pRayDataSRV[1]));
    (IRenderer::GetRendererInstance()->CreateShaderResourceView(m_pRayDataSmallTex2D, &SRVDesc, &m_pRayDataSmallSRV));
    (IRenderer::GetRendererInstance()->CreateShaderResourceView(m_pRayCastTex2D, &SRVDesc, &m_pRayCastSRV));
    (IRenderer::GetRendererInstance()->CreateShaderResourceView(m_pGlowTex2D, &SRVDesc, &m_pGlowSRV));
    (IRenderer::GetRendererInstance()->CreateShaderResourceView(m_pGlowTempTex2D, &SRVDesc, &m_pGlowTempSRV));
    SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
    (IRenderer::GetRendererInstance()->CreateShaderResourceView(m_pEdgeTex2D, &SRVDesc, &m_pEdgeSRV));

    return hr;
}

int VolumeRenderer::createJitterTexture( )
{
    int hr = 0;

    TextureID* jitterTexture = 0;
    TextureID* jitterTextureSRV = 0;


    // initial data for jitter texture
    BYTE data[256 * 256];
    for (int i = 0; i < 256 * 256; i++)
    {
        data[i] = (unsigned char) (stx_rand()/float(RAND_MAX)*256);
    }

    D3D10_SUBRESOURCE_DATA dataDesc;
    dataDesc.pSysMem = data;
    dataDesc.SysMemPitch = 256;

    // create the jitter texture
    SURFACE_DESC desc;
    desc.Width = 256;
    desc.Height = 256;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8_TYPELESS;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D10_USAGE_IMMUTABLE;
    desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    ( IRenderer::GetRendererInstance()->CreateTexture2D(&desc, &dataDesc, &jitterTexture) );

    // create the shader resource view for the jitter texture
    D3D10_SHADER_RESOURCE_VIEW_DESC descSRV;
    ZeroMemory( &descSRV, sizeof(descSRV) );
    descSRV.Format = DXGI_FORMAT_R8_UNORM;
    descSRV.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    descSRV.Texture2D.MipLevels = 1;
    descSRV.Texture2D.MostDetailedMip = 0;
    ( IRenderer::GetRendererInstance()->CreateShaderResourceView( jitterTexture, &descSRV, &jitterTextureSRV ) );

    stx_ShaderResourceVariableID *evTexture_jitter;
    (m_pEffect, m_effectPath, evTexture_jitter, "jitterTex", AsShaderResource);
    evTexture_jitter->SetResource (jitterTextureSRV);


    //////////////SAFE_RELEASE(jitterTexture);
    //////////////SAFE_RELEASE(jitterTextureSRV);
    return hr;
}

