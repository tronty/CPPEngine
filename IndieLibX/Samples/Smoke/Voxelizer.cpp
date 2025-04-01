/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//----------------------------------------------------------------------------------
// File:   Voxelizer.cpp
// Author: Ignacio Llamas
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
#include "Voxelizer.h"
#include <stddef.h>


Voxelizer::Voxelizer(void) : IRenderer::GetRendererInstance()(0), m_pDstInOutTexture3D(0), m_pDstInOutTexRTView(0),
    m_pDstVelTexture3D(0), m_pDstVelTexRTView(0),
    m_width(0), m_height(0), m_depth(0), m_cols(0), m_rows(0), m_initialized(false), 
    m_pSkinnedMesh(0), m_timeStep(1.0f), m_velocityMultiplier(1.0f),
    m_pDSTex2D(0), m_pDSTex2DDSView(0), m_pDSTex2DSRView(0), 
    m_pVoxEffect(0), m_etNZ(0), m_etResolveWithPS(0), m_etGenVelocityWireframe(0),
    m_evWorldViewProjectionMatrix(0), m_evProjSpacePixDim(0), m_evGridDim(0), m_evRecTimeStep(0),
    m_evVelocityMultiplier(0), m_evSliceIdx(0), m_evSliceZ(0), m_evTexture_depthStencil(0),
    m_pSlicesLayout(0), m_pSlicesVB(0)
{
    D3DXFROMWINEMatrixIdentity(&m_objToVolumeXForm);
}

Voxelizer::~Voxelizer(void)
{
    Cleanup();
}

void Voxelizer::Cleanup(void)
{
    //////////////SAFE_RELEASE(IRenderer::GetRendererInstance());

    //////////////SAFE_RELEASE(m_pDstInOutTexture3D);
    //////////////SAFE_RELEASE(m_pDstInOutTexRTView);
    //////////////SAFE_RELEASE(m_pDstVelTexture3D);
    //////////////SAFE_RELEASE(m_pDstVelTexRTView);

    m_initialized = false;

    //////////////SAFE_RELEASE(m_pDSTex2D);
    //////////////SAFE_RELEASE(m_pDSTex2DDSView);
    //////////////SAFE_RELEASE(m_pDSTex2DSRView);

    //////////////SAFE_RELEASE(m_pVoxEffect);

    m_etNZ = 0;
    m_etResolveWithPS = 0;
    m_etGenVelocityWireframe = 0;

    m_evWorldViewProjectionMatrix = 0;
    m_evProjSpacePixDim = 0;
    m_evGridDim = 0;
    m_evRecTimeStep = 0;
    m_evVelocityMultiplier = 0;
    m_evSliceIdx = 0;
    m_evSliceZ = 0;
    m_evTexture_depthStencil = 0;

    //////////////SAFE_RELEASE(m_pSlicesLayout);
    //////////////SAFE_RELEASE(m_pSlicesVB);

    //////////////SAFE_RELEASE(m_pSOInputLayout);
}

int Voxelizer::SetDestination(
        TextureID *pDstInOutTexture3D, TextureID *pDstVelocityTexture3D)
{
    SAFE_ACQUIRE(IRenderer::GetRendererInstance(), pD3DDevice);
    SAFE_ACQUIRE(m_pDstInOutTexture3D, pDstInOutTexture3D);
    SAFE_ACQUIRE(m_pDstVelTexture3D, pDstVelocityTexture3D);

    return Initialize();
}

int Voxelizer::Initialize(void)
{
    int hr(0);

    m_initialized = false;
    //////////////SAFE_RELEASE(m_pDstInOutTexRTView);
    //////////////SAFE_RELEASE(m_pDstVelTexRTView);

    // Assert inputs are valid
    assert(IRenderer::GetRendererInstance());
    assert(m_pDstInOutTexture3D != 0);

    {
        // Create a rendertarget view for the InOut 3D texture
        D3D10_TEXTURE3D_DESC tex3Ddesc;
        m_pDstInOutTexture3D->GetDesc(&tex3Ddesc);
        SURFACE_DESC rtDesc;
        rtDesc.Format = tex3Ddesc.Format;
        rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE3D;
        rtDesc.Texture3D.MipSlice = 0;
        rtDesc.Texture3D.FirstWSlice = 0;
        rtDesc.Texture3D.WSize = tex3Ddesc.Depth;
        (IRenderer::GetRendererInstance()->CreateRenderTargetView(m_pDstInOutTexture3D, &rtDesc, &m_pDstInOutTexRTView));

        // Get witdh, height and depth
        m_width = tex3Ddesc.Width;
        m_height = tex3Ddesc.Height;
        m_depth = tex3Ddesc.Depth;

        ComputeRowColsForFlat3DTexture(m_depth, &m_cols, &m_rows);
    }

    assert((m_width > 0) && (m_height > 0) && (m_depth > 0));
    assert((m_cols > 0) && (m_rows > 0));
    assert((m_cols * m_rows) >= m_depth);

    if( m_pDstVelTexture3D )
    {
        // Create a rendertarget view for the Velocity 3D texture
        D3D10_TEXTURE3D_DESC velTex3Ddesc;
        m_pDstVelTexture3D->GetDesc(&velTex3Ddesc);

        // Make sure both destination textures have the same dimensions
        if((velTex3Ddesc.Width != m_width) || (velTex3Ddesc.Height != m_height) ||
            (velTex3Ddesc.Depth != m_depth))
        {
            Cleanup();
            return E_INVALIDARG;
        }

        SURFACE_DESC velRtDesc;
        velRtDesc.Format = velTex3Ddesc.Format;
        velRtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE3D;
        velRtDesc.Texture3D.MipSlice = 0;
        velRtDesc.Texture3D.FirstWSlice = 0;
        velRtDesc.Texture3D.WSize = velTex3Ddesc.Depth;
        (IRenderer::GetRendererInstance()->CreateRenderTargetView(m_pDstVelTexture3D, &velRtDesc, &m_pDstVelTexRTView));

    }

    // Initialize internal texture resources
    hr = InitTextures();
    if(FAILED(hr))
    {
        Cleanup();
        return hr;
    }

    // Load Voxelizer.fx, and get techniques and variables to use (if needed)
    hr = InitShaders();
    if(FAILED(hr))
    {
        Cleanup();
        return hr;
    }

    // Init vertex buffer for a m_depth quads (to convert a "flat 3D texture" to a "3D texture");
    hr = InitSlices();
    if(FAILED(hr))
    {
        Cleanup();
        return hr;
    }

    // create input layout for use with streamout buffers from SkinnedMesh
    {
        const D3D10_INPUT_ELEMENT_DESC inputElemDesc[] =
        {
            { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
            { "POSITION",  1, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        };
        unsigned int numElements = sizeof(inputElemDesc)/sizeof(inputElemDesc[0]);

        D3D10_PASS_DESC passDesc;
        m_etGenVelocityWireframe->GetPassByIndex(0)->GetDesc(&passDesc);
        (IRenderer::GetRendererInstance()->CreateInputLayout(inputElemDesc, numElements, 
            passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pSOInputLayout));
    }


    m_initialized = true;
    return hr;
}

int Voxelizer::InitTextures(void)
{
    int hr(0);

    // release the textures if they were allocated before
    //////////////SAFE_RELEASE(m_pDSTex2D);
    //////////////SAFE_RELEASE(m_pDSTex2DDSView);
    //////////////SAFE_RELEASE(m_pDSTex2DSRView);

    // create DXGI_FORMAT_R24G8_TYPELESS depth-stencil buffer and view
    SURFACE_DESC dsTexDesc;
    dsTexDesc.Width = m_width * m_cols;
    dsTexDesc.Height = m_height * m_rows;
    dsTexDesc.MipLevels = 1;
    dsTexDesc.ArraySize = 1;
    dsTexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    dsTexDesc.SampleDesc.Count = 1;
    dsTexDesc.SampleDesc.Quality = 0;
    dsTexDesc.Usage = D3D10_USAGE_DEFAULT;
    dsTexDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
    dsTexDesc.CPUAccessFlags = 0;
    dsTexDesc.MiscFlags = 0;
    (IRenderer::GetRendererInstance()->CreateTexture2D( &dsTexDesc, 0, &m_pDSTex2D ));

    // Create the depth stencil view
    D3D10_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
    dsViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    dsViewDesc.Texture2D.MipSlice = 0;
    (IRenderer::GetRendererInstance()->CreateDepthStencilView( m_pDSTex2D, &dsViewDesc, &m_pDSTex2DDSView ));

    // Create the shader resource view for the depth-stencil buffer
    D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
    srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    
    (IRenderer::GetRendererInstance()->CreateShaderResourceView(m_pDSTex2D, &srvDesc, &m_pDSTex2DSRView));
    
    return hr;
}

int Voxelizer::InitShaders(void)
{
    int hr(0);
    char fullPath[MAX_PATH];

    if(m_pVoxEffect != 0)
        return hr;

    (NVUTFindDXSDKMediaFileCchT( fullPath, MAX_PATH, "Voxelizer.fx" ));
    (D3DXFROMWINE10CreateEffectFromFile(fullPath, g_pShadersMacros, 0, "fx_4_0", 0, 0, IRenderer::GetRendererInstance(), 
        0, 0, &m_pVoxEffect, 0, &hr ));

    V_GET_TECHNIQUE_RET(m_pVoxEffect, fullPath, m_etNZ, "VoxelizeNZ" );
    V_GET_TECHNIQUE_RET(m_pVoxEffect, fullPath, m_etResolveWithPS, "VoxelizeResolveWithPS" );
    V_GET_TECHNIQUE_RET(m_pVoxEffect, fullPath, m_etGenVelocityWireframe, "GenVelocityWireframe" );

    (m_pVoxEffect, fullPath, m_evWorldViewProjectionMatrix, "WorldViewProjection", AsMatrix);
    (m_pVoxEffect, fullPath, m_evProjSpacePixDim, "projSpacePixDim", AsVector);
    (m_pVoxEffect, fullPath, m_evGridDim, "gridDim", AsVector);
    (m_pVoxEffect, fullPath, m_evRecTimeStep, "recTimeStep", AsScalar);
    (m_pVoxEffect, fullPath, m_evVelocityMultiplier, "velocityMultiplier", AsScalar);
    (m_pVoxEffect, fullPath, m_evSliceIdx, "sliceIdx", AsScalar);
    (m_pVoxEffect, fullPath, m_evSliceZ, "sliceZ", AsScalar);
    (m_pVoxEffect, fullPath, m_evTexture_depthStencil, "stencilbufferTex2D", AsShaderResource);

    assert(m_etNZ && m_etResolveWithPS && m_etGenVelocityWireframe &&
        m_evWorldViewProjectionMatrix && m_evTexture_depthStencil);
    
    return hr;
}

struct SliceVertex
{
    D3DXFROMWINEVECTOR3 pos;
    D3DXFROMWINEVECTOR3 tex;
};

int Voxelizer::InitSlices(void)
{
    int hr(0);

    if( m_pSlicesVB != 0 )
    {
        assert(m_pSlicesLayout != 0);
        return hr;
    }

    // Create full-screen quad input layout
    const D3D10_INPUT_ELEMENT_DESC slicesLayout[] =
    {
        { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "Texcoord", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };
    unsigned int numElements = sizeof(slicesLayout)/sizeof(slicesLayout[0]);

    // Create the input layout
    D3D10_PASS_DESC passDesc;
    (m_etResolveWithPS->GetPassByIndex(0)->GetDesc( &passDesc ));

    (IRenderer::GetRendererInstance()->CreateInputLayout( slicesLayout, numElements, 
        passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pSlicesLayout ));

#define SLICEQUAD_VTXCNT 6
    
    // Create a vertex buffers of quads, one per slice, wit texcoords to lookup from a flat 3D texture
    //  and with homogenous coordinates to cover a fullscreen quad
    SliceVertex *slicesVertices(0);
    try {
         slicesVertices = new SliceVertex[SLICEQUAD_VTXCNT*m_depth];
    }
    catch(...) {
        return E_OUTOFMEMORY;
    }

    SliceVertex sliceVtx[4];
    int row, col;
    float x, y;
    int vtxIdx = 0;
    for(unsigned int z= 0; z<m_depth; z++) {
        row = z / m_cols;
        col = z % m_cols;
        x = float(col) * m_width;
        y = float(row) * m_height;
        vtxIdx = z*SLICEQUAD_VTXCNT;

        sliceVtx[0].pos = D3DXFROMWINEVECTOR3(-1.0f, 1.0f, 0.5f);
        sliceVtx[0].tex = D3DXFROMWINEVECTOR3(x, y, float(z));
        
        sliceVtx[1].pos = D3DXFROMWINEVECTOR3(-1.0f, -1.0f, 0.5f);
        sliceVtx[1].tex = D3DXFROMWINEVECTOR3(x, y+m_height, float(z));
        
        sliceVtx[2].pos = D3DXFROMWINEVECTOR3(1.0f, -1.0f, 0.5f);
        sliceVtx[2].tex = D3DXFROMWINEVECTOR3(x+m_width, y+m_height, float(z));
        
        sliceVtx[3].pos = D3DXFROMWINEVECTOR3(1.0f, 1.0f, 0.5f);
        sliceVtx[3].tex = D3DXFROMWINEVECTOR3(x+m_width, y, float(z));

        slicesVertices[vtxIdx+0] = sliceVtx[0];
        slicesVertices[vtxIdx+1] = sliceVtx[1];
        slicesVertices[vtxIdx+2] = sliceVtx[2];
        slicesVertices[vtxIdx+3] = sliceVtx[0];
        slicesVertices[vtxIdx+4] = sliceVtx[2];
        slicesVertices[vtxIdx+5] = sliceVtx[3];
    }

    D3D10_BUFFER_DESC vbDesc =
    {
        SLICEQUAD_VTXCNT*m_depth*sizeof(SliceVertex),
        D3D10_USAGE_DEFAULT,
        D3D10_BIND_VERTEX_BUFFER,
        0,
        0
    };

    D3D10_SUBRESOURCE_DATA initialData;
    initialData.pSysMem = slicesVertices;
    initialData.SysMemPitch = 0;
    initialData.SysMemSlicePitch = 0;
    (IRenderer::GetRendererInstance()->CreateBuffer( &vbDesc, &initialData, &m_pSlicesVB ));

    delete []slicesVertices;

    return hr;
}

void Voxelizer::DrawSlices(void)
{
    assert(m_pSlicesLayout);
    assert(m_pSlicesVB);

    unsigned int strides = sizeof(SliceVertex);
    unsigned int offsets = 0;

    IRenderer::GetRendererInstance()->IASetInputLayout( m_pSlicesLayout );
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1, &m_pSlicesVB, &strides, &offsets );
    IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLES );

    IRenderer::GetRendererInstance()->Draw(SLICEQUAD_VTXCNT*m_depth, 0);
}


int Voxelizer::Voxelize(D3DXFROMWINEMATRIX& objToVolumeXForm, SkinnedMesh *pMesh,
                            int srcSoBuf, int prevSrcSoBuf, float timeStep)
{
    m_objToVolumeXForm = objToVolumeXForm;
    m_pSkinnedMesh = pMesh;
    m_srcSoBuffer = srcSoBuf;
    m_prevSrcSoBuffer = prevSrcSoBuf;
    m_timeStep = timeStep;

    int hr = DoVoxelization();

    m_pSkinnedMesh = 0;
    
    return hr;
}

int Voxelizer::DoVoxelization(void)
{
    int hr(0);

    // Do the actual voxelization
    if( m_bInOutEnabled )
    {
        hr = StencilClipVolume();
    }

    if( m_bVelocityEnabled && !FAILED(hr) && m_pDstVelTexture3D)
    {
        hr = VoxelizeVelocity();
    }

    IRenderer::GetRendererInstance()->ClearState();

    return hr;
}

//
// StencilClipVolume algorithm summary:
// ====================================
// 
// function DrawClippedMesh():
//   set a vertex shader that transforms the vertices to the volume space
//   set stencil functions for NZ rule (incr on back and decr on front)
//   set the near plane to be aligned with each slice 
//   render the mesh
// done
//
// function StencilClipVolume():
//   clear depthstencil buffer
//   bind as rendertarget a 0 color buffer and the depthstencil buffer
//
//   for each slice:
//     set the viewport and cliprect to match the slice's 2D region
//     DrawClippedMesh
//   done
//
//   bind as rendertarget the 3D-texture and set viewport and scissor to match a slice
//   do a resolve pass to write out 1 for inside voxels on each slice in the 3D-texture
// done

int Voxelizer::StencilClipVolume(void)
{
    int hr(0);
    int x, y;

    assert(m_initialized);
    
    // clear depthstencil buffer to 0
    IRenderer::GetRendererInstance()->ClearDepthStencilView(m_pDSTex2DDSView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 0, 0);        
    // set the depthstencil buffer as rendertarget (no color buffer)
    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, m_pDSTex2DDSView);
    for( unsigned int z=0; z<m_depth; z++)
    {
        // compute x and y coordinates for the TOP-LEFT corner of the slice in the flat 3D texture
        x = (z % m_cols) * m_width;
        y = (z / m_cols) * m_height;

        // set viewport and scissor to match the size of single slice
        D3DFROMWINEVIEWPORT9 viewport = { x, y, m_width, m_height, 0.0f, 1.0f };
        IRenderer::GetRendererInstance()->RSSetViewports(1, &viewport);
        D3D10_RECT scissorRect = { x, y, x+m_width, y+m_height };
        IRenderer::GetRendererInstance()->RSSetScissorRects(1, &scissorRect);

        (RenderClippedMesh((float)z/m_depth - 0.5f, 100000.0f, m_etNZ));
    }

    // set texture as rendertarget
    IRenderer::GetRendererInstance()->OMSetRenderTargets(1, &m_pDstInOutTexRTView, 0 );
    // Set a resolve PixelShader (instead of using stencil test)
    //  to resolve the stencil buffer into the final texture
    (m_evTexture_depthStencil->SetResource(m_pDSTex2DSRView));
    (m_etResolveWithPS->GetPassByIndex(0)->Apply(0));

    // Set viewport and scissor to match the size of a single slice 
    D3DFROMWINEVIEWPORT9 viewport = { 0, 0, m_width, m_height, 0.0f, 1.0f };
    IRenderer::GetRendererInstance()->RSSetViewports(1, &viewport);
    D3D10_RECT scissorRect = { 0, 0, m_width, m_height };
    IRenderer::GetRendererInstance()->RSSetScissorRects(1, &scissorRect);

    DrawSlices();

    (m_evTexture_depthStencil->SetResource(0));
    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0 );

    return hr;
}

int Voxelizer::VoxelizeVelocity(void)
{
    int hr(0);

    assert(m_initialized);
    assert(m_pDstVelTexture3D);
    assert(m_pDstVelTexRTView);

    // set viewport and scissor to match the size of a single slice
    D3DFROMWINEVIEWPORT9 viewport = { 0, 0, m_width, m_height, 0.0f, 1.0f };
    IRenderer::GetRendererInstance()->RSSetViewports(1, &viewport);
    D3D10_RECT scissorRect = { 0, 0, m_width, m_height };
    IRenderer::GetRendererInstance()->RSSetScissorRects(1, &scissorRect);


    // clear the velocity texture to 0
    FLOAT color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    IRenderer::GetRendererInstance()->ClearRenderTargetView(m_pDstVelTexRTView, color);

    if(m_pSkinnedMesh == 0)
        return hr;

    // set the velocity 3D texture as rendertarget
    TextureID  *rtViews[2] = { m_pDstVelTexRTView, m_pDstInOutTexRTView};
    IRenderer::GetRendererInstance()->OMSetRenderTargets(2, rtViews, 0);


    // set some shader globals
    FLOAT projSpacePixDim[2] = { 2.0f/m_width, 2.0f/m_height };
    FLOAT gridDim[3] = { (float)m_width, (float)m_height, (float)m_depth };
    m_evProjSpacePixDim->SetFloatVector(projSpacePixDim);
    m_evGridDim->AsVector()->SetFloatVector(gridDim);
    m_evRecTimeStep->SetFloat(1.0f/m_timeStep);
    m_evVelocityMultiplier->SetFloat(m_velocityMultiplier);
    
    for( unsigned int z=0; z<m_depth; z++)
    {
        m_evSliceIdx->SetInt(z);
        float sliceZ = ((float)z)/m_depth;
        m_evSliceZ->SetFloat(sliceZ);
        (RenderClippedMesh(-0.5, 0.5, m_etGenVelocityWireframe));
    }

    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);

    return hr;
}

int Voxelizer::RenderClippedMesh(float zNear, float zFar, stx_Technique *pTechnique)
{
    int hr(0);
    D3DXFROMWINEMATRIX proj;
    D3DXFROMWINEMATRIX worldViewProj;

    D3DXFROMWINEMatrixOrthoOffCenterLH(&proj, -0.5, 0.5, 0.5, -0.5, zNear, zFar);
    D3DXFROMWINEMatrixMultiply(&worldViewProj, &m_objToVolumeXForm, &proj);
    (m_evWorldViewProjectionMatrix->SetMatrix(reinterpret_cast<float*>(&worldViewProj)));
    
    if(pTechnique == m_etGenVelocityWireframe)
    {
        exBufferID *buffers[] = { m_pSkinnedMesh->GetSOBuffer(m_srcSoBuffer), m_pSkinnedMesh->GetSOBuffer(m_prevSrcSoBuffer) };
        unsigned int strides[] = {sizeof(SkinnedMeshD3D10::SOVertex), sizeof(SkinnedMeshD3D10::SOVertex) };
        unsigned int offsets[] = {0, 0};
        IRenderer::GetRendererInstance()->setVertexBuffer(0, 2, buffers, strides, offsets);
        m_pSkinnedMesh->RenderFromSO(m_pSOInputLayout, pTechnique);
    }
    else
    {
        m_pSkinnedMesh->RenderFromSO(m_srcSoBuffer, pTechnique);
    }

    return hr;
}

