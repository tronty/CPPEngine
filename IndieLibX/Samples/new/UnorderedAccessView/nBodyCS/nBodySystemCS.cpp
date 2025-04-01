/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
// Copyright (c) 2011 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, NONINFRINGEMENT,IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA 
// OR ITS SUPPLIERS BE  LIABLE  FOR  ANY  DIRECT, SPECIAL,  INCIDENTAL,  INDIRECT,  OR  
// CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS 
// OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY 
// OTHER PECUNIARY LOSS) ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, 
// EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// Please direct any bugs or questions to SDKFeedback@nvidia.com

#include "nBodySystemCS.h"

#define MAX_PATH_STR 512

// constant buffer for rendering particles
struct CB_DRAW
{
	D3DXFROMWINEMATRIX   g_mWorldViewProjection;
	float		 g_fPointSize;
    unsigned int g_readOffset;
    float        dummy[2];
};

// constant buffer for the simulation update compute shader
struct CB_UPDATE
{
	float g_timestep;
    float g_softeningSquared;
	unsigned int g_numParticles;
    unsigned int g_readOffset;
    unsigned int g_writeOffset;
    float dummy[3];
};


struct CB_IMMUTABLE
{
	D3DXFROMWINEVECTOR4 g_positions[4];
	D3DXFROMWINEVECTOR4 g_texcoords[4];
};

CB_IMMUTABLE cbImmutable =
{
	D3DXFROMWINEVECTOR4(0.5f, -0.5f, 0, 0), D3DXFROMWINEVECTOR4(0.5f, 0.5f, 0, 0), 
    D3DXFROMWINEVECTOR4(-0.5f, -0.5f, 0, 0), D3DXFROMWINEVECTOR4(-0.5f,  0.5f, 0, 0),
	D3DXFROMWINEVECTOR4(1, 0, 0, 0), D3DXFROMWINEVECTOR4(1, 1, 0, 0), D3DXFROMWINEVECTOR4(0, 0, 0, 0), D3DXFROMWINEVECTOR4(0, 1, 0, 0),
};

// Input layout definition
	FormatDesc indLayout[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 1
	};

FLOAT blendFactors[] = {1.0f, 1.0f, 1.0f, 1.0f};

//-----------------------------------------------------------------------------
// Default Constructor
//-----------------------------------------------------------------------------
NBodySystemCS::NBodySystemCS()
{
	m_pIndLayout         = 0;
		
	// Particle texture and resource views
	m_pParticleTex    = 0; 
    m_pParticleTexSRV = 0;

	m_pVSDisplayParticleStructBuffer = 0;
	m_pGSDisplayParticle       = 0;
	m_pPSDisplayParticle       = 0;
	m_pPSDisplayParticleTex    = 0;

	m_pcbDraw                  = 0;
	m_pcbUpdate                = 0;
	m_pcbImmutable             = 0;

	m_pParticleSamplerState    = 0;
	m_pRasterizerState         = 0;

    m_pStructuredBuffer        = 0;
    m_pStructuredBufferSRV     = 0;
    m_pStructuredBufferUAV     = 0;

    m_readBuffer = 0;
}

//-----------------------------------------------------------------------------
// Render the bodies as particles using sprites
//-----------------------------------------------------------------------------
int NBodySystemCS::renderBodies(const D3DXFROMWINEMATRIX *p_mWorld, const D3DXFROMWINEMATRIX *p_mView, const D3DXFROMWINEMATRIX *p_mProj)
{
    int hr = 0;

	unsigned int particleCount = 0;

	D3DXFROMWINEMATRIX  mWorldViewProjection;
	D3DXFROMWINEMATRIX  mWorldView;
	D3DXFROMWINEMATRIX  mProjection;

	mProjection = *p_mProj;
	
	D3DXFROMWINEMatrixMultiply( &mWorldView, p_mWorld, p_mView );
	D3DXFROMWINEMatrixMultiply( &mWorldViewProjection, &mWorldView, p_mProj );
	
	// Set the input layout
	m_pd3dImmediateContext->setVertexFormat(0,  m_pIndLayout );

	m_pd3dImmediateContext->setIndexBuffer( 0, FORMAT_UNKNOWN, 0);

	D3DXFROMWINEMatrixTranspose(&mWorldViewProjection, &mWorldViewProjection);

	// constant buffers
	
	( m_pd3dImmediateContext->Map(m_pcbDraw, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) );
	CB_DRAW* pcbDraw = (CB_DRAW*) mappedResource.pData;
	pcbDraw->g_mWorldViewProjection = mWorldViewProjection;
	pcbDraw->g_fPointSize = m_fPointSize;
    pcbDraw->g_readOffset = m_readBuffer * m_numBodies;
	m_pd3dImmediateContext->Unmap(m_pcbDraw, 0);

	particleCount = m_numBodies;
	m_pd3dImmediateContext->IASetPrimitiveTopology( PRIM_POINTS);
	
	m_pd3dImmediateContext->PSSetShaderResources( 0, 1, &m_pParticleTexSRV );
	m_pd3dImmediateContext->PSSetSamplers(0, 1, &m_pParticleSamplerState);

    m_pd3dImmediateContext->VSSetShaderResources(0, 1, &m_pStructuredBufferSRV);
	
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, &m_pcbDraw );
	m_pd3dImmediateContext->VSSetConstantBuffers(1, 1, &m_pcbImmutable);
	m_pd3dImmediateContext->GSSetConstantBuffers(0, 1, &m_pcbImmutable );
	m_pd3dImmediateContext->PSSetConstantBuffers(0, 1, &m_pcbDraw );

	// Set shaders
	m_pd3dImmediateContext->GSSetShader( m_pGSDisplayParticle, 0, 0 );
	m_pd3dImmediateContext->VSSetShader( m_pVSDisplayParticleStructBuffer, 0, 0 );
	m_pd3dImmediateContext->PSSetShader( m_pPSDisplayParticleTex, 0, 0 );

	m_pd3dImmediateContext->RSSetState(m_pRasterizerState);

	// Set Blending State
    m_pd3dImmediateContext->OMSetBlendState(m_pBlendingEnabled, blendFactors, 0xFFFFFFFF);

	// Set Depth State
	m_pd3dImmediateContext->OMSetDepthStencilState(m_pDepthDisabled,0);

	m_pd3dImmediateContext->Draw(particleCount,  0);

	// Unbound all PS shader resources
	TextureID const pSRV[4] = { 0, 0, 0, 0 };
	m_pd3dImmediateContext->PSSetShaderResources( 0, 4, pSRV );
	m_pd3dImmediateContext->VSSetShaderResources( 0, 4, pSRV );
	exBufferID const pCB[4] = { 0, 0, 0, 0 };
	m_pd3dImmediateContext->PSSetConstantBuffers(0, 4, pCB );
	m_pd3dImmediateContext->GSSetConstantBuffers(0, 4, pCB );

    return hr;   
}

//-----------------------------------------------------------------------------
// Update the positions and velocities of all bodies in the system
// by invoking a Compute Shader that computes the gravitational force
// on all bodies caused by all other bodies in the system.  This computation
// is highly parallel and thus well-suited to DirectCompute shaders.
//-----------------------------------------------------------------------------
int NBodySystemCS::updateBodies(float dt)
{
    int hr = 0;

	// Set the time step
	dt = 0.016f;//min(0.166f, max(dt, 0.033f));

	// constant buffers
	
	( m_pd3dImmediateContext->Map( m_pcbUpdate, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) );
	CB_UPDATE* pcbUpdate = (CB_UPDATE*)mappedResource.pData;
	pcbUpdate->g_timestep = dt;
    pcbUpdate->g_softeningSquared = 0.01f;
	pcbUpdate->g_numParticles = m_numBodies;
    pcbUpdate->g_readOffset = m_readBuffer * m_numBodies;
    pcbUpdate->g_writeOffset = (1 - m_readBuffer) * m_numBodies;
	m_pd3dImmediateContext->Unmap(m_pcbUpdate, 0);

	unsigned int initCounts = 0;
	m_pd3dImmediateContext->CSSetShader( m_pCSUpdatePositionAndVelocity, 0, 0 );
	m_pd3dImmediateContext->CSSetConstantBuffers( 0, 1, &m_pcbUpdate );
	m_pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, &m_pStructuredBufferUAV, &initCounts ); // CS output

	// Run the CS
	m_pd3dImmediateContext->Dispatch( m_numBodies / 256, 1, 1 );

    m_readBuffer = 1 - m_readBuffer;

    return hr;
}

//-----------------------------------------------------------------------------
// Reset the body system to its initial configuration
//-----------------------------------------------------------------------------
int NBodySystemCS::resetBodies(BodyData configData)
{
    int hr = 0;
    
	m_numBodies = configData.nBodies;

	// for compute shader on CS_4_0, we can only have a single UAV per shader, so we have to store particle
	// position and velocity in the same array: all positions followed by all velocities
	D3DXFROMWINEVECTOR4 *particleArray = new D3DXFROMWINEVECTOR4[m_numBodies * 3];

	for(unsigned int i=0; i < m_numBodies; i++){
		particleArray[i] = D3DXFROMWINEVECTOR4(configData.position[i*3 + 0], 
						 			   configData.position[i*3 + 1], 
									   configData.position[i*3 + 2], 
									   1.0);
		particleArray[i + m_numBodies] = particleArray[i];
        particleArray[i + 2 * m_numBodies] = D3DXFROMWINEVECTOR4(configData.velocity[i*3 + 0], 
                                                            configData.velocity[i*3 + 1], 
												            configData.velocity[i*3 + 2], 
												            1.0);
	}

	

    //////////////////////SAFE_RELEASE( m_pStructuredBuffer );
    //////////////////////SAFE_RELEASE( m_pStructuredBufferSRV );
    //////////////////////SAFE_RELEASE( m_pStructuredBufferUAV );

	// Create Structured Buffer
	
	sbDesc.BindFlags            = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
	sbDesc.CPUAccessFlags       = 0;
	sbDesc.MiscFlags            = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sbDesc.StructureByteStride  = sizeof(D3DXFROMWINEVECTOR4);
	sbDesc.ByteWidth            = sizeof(D3DXFROMWINEVECTOR4) * m_numBodies * 3;
	sbDesc.Usage                = USAGE_DEFAULT;
	( IRenderer::GetRendererInstance()->CreateBuffer(&sbDesc, &initData, &m_pStructuredBuffer) );er, &sbUAVDesc, &m_pStructuredBufferUAV) );

	delete [] particleArray;
		
	return hr;
}

//-----------------------------------------------------------------------------
// Handle device creation (create shaders and resources)
//-----------------------------------------------------------------------------
int NBodySystemCS::onCreateDevice( )
{
    int hr = 0;

	char str[MAX_PATH_STR+1];

	// Create shaders
	// VS
	stx_Blob* pBlobVS = 0;
    ( CompileShaderFromFile( "RenderParticles.hlsl", "DisplayVS_StructBuffer", "vs_4_0", &pBlobVS ) );
    ( IRenderer::GetRendererInstance()->CreateVertexShader( pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), 0, &m_pVSDisplayParticleStructBuffer) );

	// Create vertex layout
	( IRenderer::GetRendererInstance()->CreateInputLayout( indLayout, 1, pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), &m_pIndLayout ) );
	//////////////////////SAFE_RELEASE( pBlobVS );

	// GS
	stx_Blob* pBlobGS = 0;
	( CompileShaderFromFile( "RenderParticles.hlsl", "DisplayGS", "gs_4_0", &pBlobGS ) );
	( IRenderer::GetRendererInstance()->CreateGeometryShader( pBlobGS->GetBufferPointer(), pBlobGS->GetBufferSize(), 0, &m_pGSDisplayParticle ) );
	//////////////////////SAFE_RELEASE ( pBlobGS );

	// PS
	stx_Blob*  = 0;
	( CompileShaderFromFile( "RenderParticles.hlsl", "DisplayPS", "ps_4_0", &pBlobPS ) );
	( IRenderer::GetRendererInstance()->CreatePixelShader( pBlobPS->GetBufferPointer(), pBlobPS->GetBufferSize(), 0, &m_pPSDisplayParticle ) );
	//////////////////////SAFE_RELEASE ( pBlobPS );

	( CompileShaderFromFile( "RenderParticles.hlsl", "DisplayPSTex", "ps_4_0", &pBlobPS ) );
	( IRenderer::GetRendererInstance()->CreatePixelShader( pBlobPS->GetBufferPointer(), pBlobPS->GetBufferSize(), 0, &m_pPSDisplayParticleTex ) );
	//////////////////////SAFE_RELEASE ( pBlobPS );
	
	// Update CS
	( LoadComputeShader( "nBodyCS.hlsl", "NBodyUpdate", "cs_4_0", &m_pCSUpdatePositionAndVelocity) );
	
	// Create constant buffers
	
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.ByteWidth = sizeof( CB_DRAW );
	( IRenderer::GetRendererInstance()->CreateBuffer( &cbDesc, 0, &m_pcbDraw ) );

	cbDesc.ByteWidth = sizeof( CB_UPDATE );
	( IRenderer::GetRendererInstance()->CreateBuffer( &cbDesc, 0, &m_pcbUpdate ) );
	
	cbDesc.Usage = USAGE_IMMUTABLE;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.ByteWidth = sizeof(CB_IMMUTABLE);
	
	( IRenderer::GetRendererInstance()->CreateBuffer(&cbDesc, &initData, &m_pcbImmutable ) );

	// sampler state
	
	sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sDesc.MinLOD = 0;
	sDesc.MaxLOD = 8;
	sDesc.MipLODBias = 0;
	sDesc.MaxAnisotropy = 1;
	( IRenderer::GetRendererInstance()->CreateSamplerState(&sDesc, &m_pParticleSamplerState) );

	// rasterizer state
    
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_NONE;
    rsDesc.FrontCounterClockwise = FALSE;
    rsDesc.DepthBias = 0;
    rsDesc.DepthBiasClamp = 0.0f;
    rsDesc.SlopeScaledDepthBias = 0.0f;
    rsDesc.DepthClipEnable = TRUE;
    rsDesc.ScissorEnable =	FALSE;
    rsDesc.MultisampleEnable = FALSE;
    rsDesc.AntialiasedLineEnable = FALSE;
	( IRenderer::GetRendererInstance()->CreateRasterizerState(&rsDesc, &m_pRasterizerState) );
  
	// Load the particle texture
    m_pParticleTex=IRenderer::GetRendererInstance()->addShaderFromFile("pointsprite_grey.dds", false, IRenderer::GetRendererInstance()->Getlinear());

	// Create the blending states
	

	tmpBlendState.AlphaToCoverageEnable = false;
	tmpBlendState.IndependentBlendEnable = false;

	for(unsigned int i = 0; i < 8; i++){
		tmpBlendState.RenderTarget[i].BlendEnable = false;
		tmpBlendState.RenderTarget[i].SrcBlend       = D3D11_BLEND_ONE;
		tmpBlendState.RenderTarget[i].DestBlend      = D3D11_BLEND_ONE;
		tmpBlendState.RenderTarget[i].BlendOp        = D3D11_BLEND_OP_ADD;
		tmpBlendState.RenderTarget[i].SrcBlendAlpha  = D3D11_BLEND_ZERO;
		tmpBlendState.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		tmpBlendState.RenderTarget[i].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
		tmpBlendState.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}  

	tmpBlendState.RenderTarget[0].BlendEnable = true;

	hr = IRenderer::GetRendererInstance()->CreateBlendState( &tmpBlendState, &m_pBlendingEnabled);

	// Create the depth/stencil states
	
    tmpDsDesc.DepthEnable    = false;
    tmpDsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    tmpDsDesc.DepthFunc      = D3D11_COMPARISON_LESS;
    tmpDsDesc.StencilEnable  = FALSE;

	hr = IRenderer::GetRendererInstance()->CreateDepthStencilState( &tmpDsDesc, &m_pDepthDisabled);
     
    return hr;
}

//-----------------------------------------------------------------------------
// Handle device destruction
//-----------------------------------------------------------------------------
void NBodySystemCS::onDestroyDevice()
{
	//////////////////////SAFE_RELEASE( m_pIndLayout );
	
	//////////////////////SAFE_RELEASE( m_pBlendingEnabled );
	//////////////////////SAFE_RELEASE( m_pDepthDisabled );
	//////////////////////SAFE_RELEASE( m_pParticleSamplerState );
	//////////////////////SAFE_RELEASE( m_pRasterizerState );

	// Particle texture and resource views
	//////////////////////SAFE_RELEASE( m_pParticleTex ); 
    //////////////////////SAFE_RELEASE( m_pParticleTexSRV );
	
	// shaders
    //////////////////////SAFE_RELEASE( m_pVSDisplayParticleStructBuffer );
	//////////////////////SAFE_RELEASE( m_pGSDisplayParticle );
	//////////////////////SAFE_RELEASE( m_pPSDisplayParticle );
	//////////////////////SAFE_RELEASE( m_pPSDisplayParticleTex );
	//////////////////////SAFE_RELEASE( m_pCSUpdatePositionAndVelocity );

	// constant buffers
	//////////////////////SAFE_RELEASE( m_pcbDraw );
	//////////////////////SAFE_RELEASE( m_pcbUpdate );
	//////////////////////SAFE_RELEASE( m_pcbImmutable );

	// structured buffer
	//////////////////////SAFE_RELEASE( m_pStructuredBuffer );
	//////////////////////SAFE_RELEASE( m_pStructuredBufferSRV );
	//////////////////////SAFE_RELEASE( m_pStructuredBufferUAV );
}

//--------------------------------------------------------------------------------------
// Handle resize
//--------------------------------------------------------------------------------------
void NBodySystemCS::onResizedSwapChain( )
{
	// Update the aspect ratio
	m_fAspectRatio = pBackBufferSurfaceDesc->Width * 1.0f / pBackBufferSurfaceDesc->Height; 
}

