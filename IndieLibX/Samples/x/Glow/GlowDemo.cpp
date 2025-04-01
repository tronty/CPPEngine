/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
/*--------------------------------------------------------------------- NVMH5 -|----------------------
Path:  Sdk\Demos\Direct3D9\src\Glow\
File:  GlowDemo.cpp

Copyright NVIDIA_Corporation 2003
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED *AS IS* AND NVIDIA AND
AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA
OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER
INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR INABILITY TO USE THIS
SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:


-------------------------------------------------------------------------------|--------------------*/

#include "nvafx.h"
#include "GlowDemo.h"
#include "GlowScene.h"
#include "GlowProfiles.h"
#include "GlowShaderCollection.h"
#include "..\..\..\..\media\programs\D3D9_Glow\Constants.h"

GlowDemo::GlowDemo()
{
	SetAllNull();
}
GlowDemo::~GlowDemo()
{
	Free();
}

int GlowDemo::Free()
{
	SAFE_DELETE( m_pTextureFactory );
	SAFE_DELETE( m_pShaderManager );
	SAFE_DELETE( m_pTextureDisplay );
	SAFE_DELETE( m_pRenderTargetFactory );
	SAFE_DELETE( m_pScene );
	SAFE_DELETE( m_pPlot );
	SAFE_DELETE( m_pGlowShaderCollection );
	//SAFE_RELEASE( m_pD3DDev );
	return( 0 );
}

int GlowDemo::Initialize( IDirect3DDevice9 * pDev )
{
	int hr = 0;
	Free();
	FAIL_IF_NULL( pDev );
	m_pD3DDev = pDev;
	m_pD3DDev->AddRef();

	m_pShaderManager = new ShaderManager;
	FAIL_IF_NULL( m_pShaderManager );
	hr = m_pShaderManager->Initialize( m_pD3DDev, GetFilePath::GetFilePath );
	MSG_AND_RET_VAL_IF( FAILED(hr), "shadermanager\n", hr );

	m_pTextureFactory = new TextureFactory;
	FAIL_IF_NULL( m_pTextureFactory );
	hr = m_pTextureFactory->Initialize( GetFilePath::GetFilePath );
	MSG_AND_RET_VAL_IF( FAILED(hr), "textureFactory\n", hr );

	m_pTextureDisplay = new TextureDisplay2;
	FAIL_IF_NULL( m_pTextureDisplay );
	hr = m_pTextureDisplay->Initialize( m_pD3DDev );
	
	m_pRenderTargetFactory = new RenderTargetFactory;
	FAIL_IF_NULL( m_pRenderTargetFactory );
	m_pRenderTargetFactory->Initialize();

	m_pScene = new GlowScene;
	FAIL_IF_NULL( m_pScene );
	hr = m_pScene->Initialize( m_pD3DDev );
	DBG_ONLY( MSG_AND_RET_VAL_IF( FAILED(hr), "GlowScene Initialize failed\n", hr ));

	m_pPlot = new Plot;
	FAIL_IF_NULL( m_pPlot );
	m_pPlot->Initialize( m_pD3DDev );

	m_pGlowShaderCollection = new GlowShaderCollection;
	FAIL_IF_NULL( m_pGlowShaderCollection );
	m_pGlowShaderCollection->Initialize( m_pD3DDev, &m_pShaderManager );

	m_FlipChainBuffers = m_pRenderTargetFactory->GetCurrentTargets( m_pD3DDev );

	D3DVIEWPORT9 viewport;
	m_pD3DDev->GetViewport( &viewport );
	int w, h;
	w = viewport.Width;
	h = viewport.Height;

	m_ppRTTHalfSize = m_pRenderTargetFactory->CreateRenderTarget( m_pD3DDev, w/2, h/2, D3DFMT_A8R8G8B8 );
	m_ppRTTGlowSource = m_pRenderTargetFactory->CreateRenderTarget( m_pD3DDev, w/2, h/2, D3DFMT_A8R8G8B8 );
	m_ppRTTBlurHoriz = m_ppRTTHalfSize; 
	m_ppRTTBlurVert = m_ppRTTGlowSource;

	// Create TextureDisplay entities to render the textures into various targets.
	// Add half-pixel offsets to the rectangle coordinates to properly map texels to pixels
	float hlf_bb_pixel_w, hlf_bb_pixel_h;
	hlf_bb_pixel_w = 0.5f / ((float)w);
	hlf_bb_pixel_h = 0.5f / ((float)h);
	float hlf_hs_pixel_w, hlf_hs_pixel_h;
	hlf_hs_pixel_w = 1.0f / ((float)w);
	hlf_hs_pixel_h = 1.0f / ((float)h);
	m_pTextureDisplay->AddTexture( &m_TID_HalfSizeToBlurHoriz, (*m_ppRTTHalfSize)->m_ppRTTTexture, 
						FRECT( -hlf_hs_pixel_w, -hlf_hs_pixel_h, 1.0f - hlf_hs_pixel_w, 1.0f - hlf_hs_pixel_h ) );
	m_pTextureDisplay->AddTexture( &m_TID_BlurHorizToBlurVert, (*m_ppRTTBlurHoriz)->m_ppRTTTexture,
						FRECT( -hlf_hs_pixel_w, -hlf_hs_pixel_h, 1.0f - hlf_hs_pixel_w, 1.0f - hlf_hs_pixel_h ) );
	m_pTextureDisplay->AddTexture( &m_TID_BlurToFlipChain, (*m_ppRTTBlurVert)->m_ppRTTTexture,
						FRECT( -hlf_bb_pixel_w, hlf_bb_pixel_h, 1.0f - hlf_bb_pixel_w, 1.0f - hlf_bb_pixel_h ) );
	// Create geometry with no position offset.  The offset to sample from texel centers will be
	// applied in the vertex shader.
	// The texture set here doesn't matter.
	m_pTextureDisplay->AddTexture( &m_TID_FullscreenQuadNoOffsets, (*m_ppRTTHalfSize)->m_ppRTTTexture,
						FRECT( 0.0f, 0.0f, 1.0f, 1.0f ) );

	vector< float > vf;
	GlowProfiles gp;
	int i;
	float x, y, xsize, gap;
	gap = 0.03f;
	x = y = gap;
	xsize = 0.28f;
	for( i=0; i < gp.GetNumProfiles(); i++ )
	{
		if( x + xsize > 1.0f )
		{
			x = gap;
			y += xsize + gap;
		}
		gp.GetProfile( i, &vf );
		m_pPlot->CreatePlot( &m_PID_PlotHBlur, IPlot::PT_BARS_FILLED, &vf, 0x00FFFF00, 
								FRECT( x, y, x+xsize, y+xsize ),
								0.0f, 1.0f );
		x += gap + xsize;
	}

	gp.m_fSourceResX = (float)w/2;
	gp.m_fSourceResY = (float)h/2;
	gp.m_fTexCoordOffsetX = hlf_hs_pixel_w;
	gp.m_fTexCoordOffsetY = hlf_hs_pixel_h;
	// Create blur profiles used to create the blur
	gp.GetProfile( 0, &m_vBlurHCoefficients, &m_vBlurHCoords, GlowProfiles::KT_HORIZONTAL );
	gp.GetProfile( 0, &m_vBlurVCoefficients, &m_vBlurVCoords, GlowProfiles::KT_VERTICAL );
	// make sure the number of elements is even
	// pad the last with 0 if necessary
	if( (m_vBlurHCoefficients.size() % 2) != 0 )
	{
		m_vBlurHCoefficients.push_back( D3DXFROMWINEVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f ) );
		m_vBlurHCoords.push_back( D3DXFROMWINEVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f ) );
	}
	if( (m_vBlurVCoefficients.size() % 2) != 0 )
	{
		m_vBlurVCoefficients.push_back( D3DXFROMWINEVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f ) );
		m_vBlurVCoords.push_back( D3DXFROMWINEVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f ) );
	}
	return( hr );
}

int GlowDemo::Render()
{
	int hr = 0;
	FAIL_IF_NULL( m_pD3DDev );
	FAIL_IF_NULL( m_ppRTTHalfSize );
	FAIL_IF_NULL( *m_ppRTTHalfSize );
	FAIL_IF_NULL( m_ppRTTGlowSource );
	FAIL_IF_NULL( *m_ppRTTGlowSource );
	FAIL_IF_NULL( m_ppRTTBlurHoriz );
	FAIL_IF_NULL( *m_ppRTTBlurHoriz );
	FAIL_IF_NULL( m_ppRTTBlurVert );
	FAIL_IF_NULL( *m_ppRTTBlurVert );
	FAIL_IF_NULL( m_pGlowShaderCollection );
	MSG_AND_RET_VAL_IF( m_pGlowShaderCollection->m_vShaders.size() < 1, "no shaders in shader collection!\n", E_FAIL );

	if( m_pScene != 0 )
	{
		// Set to ordinary flip chain
		m_FlipChainBuffers.SetAsCurrent();
		m_pD3DDev->Clear( 0L, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
							0x00000000, 1.0f, 0L);		// color is ARGB

		// Render scene
		m_pScene->RenderObjectsWithGlowStrengthInAlpha();
	
		// Copy scene to lower resolution render target texture
		hr = m_pD3DDev->StretchRect( (*m_FlipChainBuffers.m_ppColorTarget)->GetSurfaceP(), 0,
									 (*m_ppRTTHalfSize)->GetSurfaceP(), 0, D3DTEXF_LINEAR );
		MSG_AND_RET_VAL_IF( FAILED(hr), "StretchRect() failed!\n", hr );

		// Multiply texture alpha * RGB to get the glow sources
		m_pD3DDev->SetRenderTarget( 0, (*m_ppRTTGlowSource)->GetSurfaceP() );
		m_pD3DDev->SetDepthStencilSurface( 0 );
		// Select quad geometry with half-texel size offset to sample from texel centers
		m_pTextureDisplay->SetStateForRendering( m_TID_HalfSizeToBlurHoriz );
		// D3DTOP_BLENDTEXTUREALPHA reads the alpha at it's stage and does s = arg1 * alpha + arg2 * alpha
		m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_BLENDTEXTUREALPHA );
		m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG1 );	// texture alpha
		m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );
		m_pD3DDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );
		m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR,			0x00000000 );		// ARGB black
		m_pD3DDev->SetTexture( 0, (*m_ppRTTHalfSize)->GetTextureP() );
		m_pTextureDisplay->Render( m_TID_HalfSizeToBlurHoriz, false, false );

#if 0
		// display m_pRTTGlowSource
		m_FlipChainBuffers.SetAsCurrent();
		m_pD3DDev->SetDepthStencilSurface( 0 );
		m_pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET, 0x00, 1.0f, 0 );
		m_pTextureDisplay->SetStateForRendering( m_TID_BlurToFlipChain );
		m_pD3DDev->SetTexture( 0, (*m_ppRTTGlowSource)->GetTextureP() );
		m_pTextureDisplay->Render( m_TID_BlurToFlipChain, false, false );
		return( hr );
#endif

		// Blur glow sources in the horizontal axis
		// set texture 0 to 0 so same texture is never simultaneously a source and render target
		m_pD3DDev->SetTexture( 0, 0 );
		m_pD3DDev->SetRenderTarget( 0, (*m_ppRTTBlurHoriz)->GetSurfaceP() );
		m_pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET, 0x00, 1.0f, 0 );
		m_pTextureDisplay->SetStateForRendering( m_TID_FullscreenQuadNoOffsets );
		m_pD3DDev->SetRenderState( D3DRS_ZENABLE,				false );
		m_pD3DDev->SetRenderState( D3DRS_STENCILENABLE,			false );
		// additive alpha blending
		m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE,		true );
		m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_ONE );
		m_pD3DDev->SetRenderState( D3DRS_DESTBLEND,				D3DBLEND_ONE );

		size_t i, num_shaders;
		num_shaders = m_pGlowShaderCollection->m_vShaders.size();
		int elem, num_samples, t, num_elems;
		num_elems = (int) m_vBlurHCoefficients.size();
		elem = 0;
		MSG_AND_RET_VAL_IF( num_elems != m_vBlurHCoords.size(), "H sizes do not match!\n", E_FAIL );
		while( num_elems > 0 )
		{
			// find a shader set that can process the most elems
			for( i=0; i < num_shaders; i++ )
			{
				if( m_pGlowShaderCollection->m_vShaders.at(i).m_nNumSamples <= num_elems )
					break;
			}
			if( i >= num_shaders )
			{
				// The shaders from the shader set that process the fewest samples still process more
				// samples than we have elements left, so have to set the last elements to zero.
				FMsg("odd # remaining elements!\n");
				assert( false );
				num_elems = 0;
			}
			else
			{
				num_samples = m_pGlowShaderCollection->m_vShaders.at(i).m_nNumSamples;
				// if first time, set textures
				if( elem == 0 )
				{
					for( t=0; t < num_samples; t++ )
					{
						m_pD3DDev->SetTexture( t, (*m_ppRTTGlowSource)->GetTextureP() );
						m_pD3DDev->SetSamplerState( t, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP );
						m_pD3DDev->SetSamplerState( t, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP );
						m_pD3DDev->SetSamplerState( t, D3DSAMP_MINFILTER,	D3DTEXF_POINT );
						m_pD3DDev->SetSamplerState( t, D3DSAMP_MAGFILTER,	D3DTEXF_POINT );
						m_pD3DDev->SetTextureStageState( t, D3DTSS_TEXCOORDINDEX,	t );
					}
				}

				m_pShaderManager->SetShader( m_pGlowShaderCollection->m_vShaders.at(i).m_VSHI );
				m_pShaderManager->SetShader( m_pGlowShaderCollection->m_vShaders.at(i).m_PSHI );
				// Set sample coordinates for the blur convolution
				m_pD3DDev->SetVertexShaderConstantF( CV_TEXCOORD_OFFSET_0, (float*)(&m_vBlurHCoords.at(elem)), num_samples );
				// Set sample coefficients for the blur convolution
				m_pD3DDev->SetPixelShaderConstantF( CP_WEIGHT_0, (float*)(&m_vBlurHCoefficients.at(elem)), num_samples );
				// Render the fullscreen quad, adding convolution samples into the blur result
				// false, false to not override the vertex and pixel state we've set
				m_pTextureDisplay->Render( m_TID_FullscreenQuadNoOffsets, false, false );
				elem = elem + num_samples;
				num_elems = num_elems - num_samples;
			}
		}			// while horiz blur elements remaining

		// Blur the horizontal blur in the vertical direction
		// Set all textures to 0 to prevent RTT being bound as source and dest at the same time,
		//  regardless of whether it's read from
		num_samples = m_pGlowShaderCollection->m_vShaders.at(0).m_nNumSamples;
		for( t=0; t < num_samples; t++ )
		{
			m_pD3DDev->SetTexture( t, 0 );
		}
		m_pD3DDev->SetRenderTarget( 0, (*m_ppRTTBlurVert)->GetSurfaceP() );
		//@@@ eliminate clear - do no alphablend the first time
		m_pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET, 0x00, 1.0f, 0 );

		num_elems = (int) m_vBlurVCoefficients.size();
		elem = 0;
		MSG_AND_RET_VAL_IF( num_elems != m_vBlurVCoords.size(), "H sizes do not match!\n", E_FAIL );
		while( num_elems > 0 )
		{
			// find a shader set that can process the most elems
			for( i=0; i < num_shaders; i++ )
			{
				if( m_pGlowShaderCollection->m_vShaders.at(i).m_nNumSamples <= num_elems )
					break;
			}
			if( i >= num_shaders )
			{
				// The shaders from the shader set that process the fewest samples still process more
				// samples than we have elements left, so have to set the last elements to zero.
				FMsg("odd # remaining elements!\n");
				assert( false );
				num_elems = 0;
			}
			else
			{
				num_samples = m_pGlowShaderCollection->m_vShaders.at(i).m_nNumSamples;
				// if first time, set textures
				if( elem == 0 )
				{
					for( t=0; t < num_samples; t++ )
					{
						m_pD3DDev->SetTexture( t, (*m_ppRTTBlurHoriz)->GetTextureP() );
					}
				}

				m_pShaderManager->SetShader( m_pGlowShaderCollection->m_vShaders.at(i).m_VSHI );
				m_pShaderManager->SetShader( m_pGlowShaderCollection->m_vShaders.at(i).m_PSHI );
				// Set sample coordinates for the blur convolution
				m_pD3DDev->SetVertexShaderConstantF( CV_TEXCOORD_OFFSET_0, (float*)(&m_vBlurVCoords.at(elem)), num_samples );
				// Set sample coefficients for the blur convolution
				m_pD3DDev->SetPixelShaderConstantF( CP_WEIGHT_0, (float*)(&m_vBlurVCoefficients.at(elem)), num_samples );
				// Render the fullscreen quad, adding convolution samples into the blur result
				// false, false to not override the vertex and pixel state we've set
				m_pTextureDisplay->Render( m_TID_FullscreenQuadNoOffsets, false, false );
				elem = elem + num_samples;
				num_elems = num_elems - num_samples;
			}
		}			// while vertical blur elements remaining


		// Set all textures to 0 to prevent RTT being bound as source and dest at the same time,
		//  regardless of whether it's read from
		num_samples = m_pGlowShaderCollection->m_vShaders.at(0).m_nNumSamples;
		for( t=0; t < num_samples; t++ )
		{
			m_pD3DDev->SetTexture( t, 0 );
		}

		// Add the horizontal blur to the backbuffer
		m_FlipChainBuffers.SetAsCurrent();
		m_pD3DDev->SetDepthStencilSurface( 0 );
		if( m_bDisplayFinalBlurTexture )
		{
			m_pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET, 0x00, 1.0f, 0 );
		}
		m_pTextureDisplay->SetStateForRendering( m_TID_BlurToFlipChain );
		m_pD3DDev->SetTexture( 0, (*m_ppRTTBlurVert)->GetTextureP() );
		// multiply glow texture by constant TEXTURE_FACTOR
		m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );
		m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR,			m_dwGlowTextureAttenuation );
		// additive alpha blend src*1 + dest*1
		m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE,		true );
		m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_ONE );
		m_pD3DDev->SetRenderState( D3DRS_DESTBLEND,				D3DBLEND_ONE );
		m_pTextureDisplay->Render( m_TID_BlurToFlipChain, false, false );

		if( m_pPlot != 0 && m_bPlotProfiles == true )
		{
			m_pPlot->Render();
		}
	}
	return( hr );
}



