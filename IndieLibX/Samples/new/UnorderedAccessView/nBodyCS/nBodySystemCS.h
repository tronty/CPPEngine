#ifndef __NBODYSYSTEM_H__
#define __NBODYSYSTEM_H__
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

typedef struct{
    unsigned int nBodies;
    float       *position;
    float       *velocity;
} BodyData;

class NBodySystemCS
{
public:
    NBodySystemCS();

    // Update the simulation
    int updateBodies (float dt);

    // Reset the simulation
	int resetBodies  (BodyData config);

    // Render the particles
    int renderBodies (const D3DXFROMWINEMATRIX *p_mWorld, const D3DXFROMWINEMATRIX *p_mWiew, const D3DXFROMWINEMATRIX *p_mProj);

    void    setPointSize(float size) { m_fPointSize = size; } 
    float	getPointSize()  const	 { return m_fPointSize; } 

    // D3D Event handlers
	int onCreateDevice      ( );
    void    onDestroyDevice     ( );
	void    onResizedSwapChain  ( );
	void    onReleasingSwapChain( );

protected:

	float m_fAspectRatio;

	// Particle texture and resource views
	TextureID m_pParticleTex;    // Texture for displaying particles
    TextureID m_pParticleTexSRV;

	// Vertex layout
	VertexFormatID m_pIndLayout;
	
    // state
    unsigned int    m_numBodies;
    float   m_fPointSize;
    unsigned int    m_readBuffer;

	//Render states
	BlendStateID m_pBlendingEnabled;
	DepthStateID m_pDepthDisabled;
	SamplerStateID		m_pParticleSamplerState;
	RasterizerStateID   m_pRasterizerState;

	// shaders
	VertexShaderID m_pVSDisplayParticleStructBuffer;
	GeometryShaderID m_pGSDisplayParticle;
	PixelShaderID m_pPSDisplayParticle;
	PixelShaderID m_pPSDisplayParticleTex;

	ComputeShaderID m_pCSUpdatePositionAndVelocity;

	// structured buffer
    TextureID m_pBodiesTex1D[2];
    TextureID m_pBodiesTexSRV[2];
    ID3D11UnorderedAccessView *m_pBodiesTexUAV[2];
	exBufferID m_pStructuredBuffer;
	TextureID m_pStructuredBufferSRV;
	ID3D11UnorderedAccessView *m_pStructuredBufferUAV;

	// constant buffers
	exBufferID m_pcbDraw;
	exBufferID m_pcbUpdate;
	exBufferID m_pcbImmutable;

private:
};

#endif // __NBODYSYSTEM_H__

