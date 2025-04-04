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

#ifndef _OCEAN_WAVE_H
#define _OCEAN_WAVE_H

/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

#include "fft_512x512.h"

//#define CS_DEBUG_BUFFER
#define PAD16(n) (((n)+15)/16*16)

struct OceanParameter
{
	// Must be power of 2.
	int dmap_dim;
	// Typical value is 1000 ~ 2000
	float patch_length;

	// Adjust the time interval for simulation.
	float time_scale;
	// Amplitude for transverse wave. Around 1.0
	float wave_amplitude;
	// Wind direction. Normalization not required.
	D3DXFROMWINEVECTOR2 wind_dir;
	// Around 100 ~ 1000
	float wind_speed;
	// This value damps out the waves against the wind direction.
	// Smaller value means higher wind dependency.
	float wind_dependency;
	// The amplitude for longitudinal wave. Must be positive.
	float choppy_scale;
};


class OceanSimulator
{
public:
	OceanSimulator(OceanParameter& params);
	~OceanSimulator();

	// -------------------------- Initialization & simulation routines ------------------------

	// Update ocean wave when tick arrives.
	void updateDisplacementMap(float time);

	// Texture access
	ShaderResourceViewID getD3D11DisplacementMap();
	ShaderResourceViewID getD3D11GradientMap();

	const OceanParameter& getParameters();


protected:
	OceanParameter m_param;

	// ---------------------------------- GPU shading asset -----------------------------------

	// D3D objects
	
	
	
	// Displacement map
	TextureID m_pDisplacementMap;		// (RGBA32F)

	// Gradient field
	TextureID m_pGradientMap;			// (RGBA16F)

	// Samplers
	SamplerStateID  m_pPointSamplerState;

	// Initialize the vector field.
	void initHeightMap(OceanParameter& params, D3DXFROMWINEVECTOR2* out_h0, float* out_omega);


	// ----------------------------------- CS simulation data ---------------------------------

	// Initial height field H(0) generated by Phillips spectrum & Gauss distribution.
	VertexBufferID m_pBuffer_Float2_H0;

	// Angular frequency
	VertexBufferID m_pBuffer_Float_Omega;

	// Height field H(t), choppy field Dx(t) and Dy(t) in frequency domain, updated each frame.
	VertexBufferID m_pBuffer_Float2_Ht;

	// Height & choppy buffer in the space domain, corresponding to H(t), Dx(t) and Dy(t)
	VertexBufferID m_pBuffer_Float_Dxyz;

	VertexBufferID m_pQuadVB;

	// Shaders, layouts and constants
	ComputeShaderID  m_pUpdateSpectrumCS;

	ShaderID   m_pQuadVS;
	ShaderID   m_pUpdateDisplacementPS;
	ShaderID   m_pGenGradientFoldingPS;

	VertexFormatID m_pQuadLayout;

	VertexBufferID m_pImmutableCB;
	VertexBufferID m_pPerFrameCB;

	// FFT wrap-up
	CSFFT512x512_Plan m_fft_plan;

#ifdef CS_DEBUG_BUFFER
	VertexBufferID m_pDebugBuffer;
#endif
};

#endif	// _OCEAN_WAVE_H

