/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: GPUphysics.h
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _GPU_PHYSICS_H_
#define _GPU_PHYSICS_H_

#include "physics.h"

//-----------------------------------------------------------------------------
// Class GPU_Physics
//
// This class implements the physics for wave synthesis using the GPU.
//-----------------------------------------------------------------------------

class GPU_Physics : public Physics
{
public:
    GPU_Physics();
    ~GPU_Physics();
    void ComputeWaves(float windSpeed, float windDirection, float time);
    TextureID GetNormalMap(){return m_pHighNormalTex;}
    float* GetDisplacementMap()
    {
    	return m_pDisplacementMapTex;
    } // ??? Lock/Unlock/Image3::getPixels
    void FillWaterUtilityBuffers(float windSpeed, float windDirection);
private:
    int FillFFTUtilityTextures(int iResolution, int iLog2Resolution, 
        TextureID&  ppScrambleMap, TextureID&  ppFFTButterflyMap, 
        TextureID&  ppIFFTButterflyMap);

    int FillScreenAlignedQuads();
    int PerformFFT(TextureID  pInputTex, SurfaceID  pOutputSurf, BOOL bDoForwardFFT, 
        int iResolution, int iLog2Resolution);

    int GenerateFourierSpectrum(SurfaceID  pOutputSurf, float fTime, int iResolution);
    int ComputeWaterNormals(TextureID  pInputTex, SurfaceID  pOutputSurf, int iResolution);
    int setTexture(ShaderID shader, char* name, TextureID  texture)
    {
        int index = IRenderer::GetRendererInstance()->getSamplerIndex(shader, name);
        IRenderer::GetRendererInstance()->setTexture(name, texture);
	return index;
    }

    ShaderID                m_HorizontalButterflyShader;
    ShaderID                m_VerticalButterflyShader;
    ShaderID                m_HorizontalScrambleShader;
    ShaderID                m_VerticalScrambleShader;
    ShaderID                m_ExpandShader;
    ShaderID                m_CopyShader;
    ShaderID                m_WaterSpectrumShader;
    ShaderID                m_ComputeNormalsShader;

    VertexFormatID                m_HorizontalButterflyVF;
    VertexFormatID                m_VerticalButterflyVF;
    VertexFormatID                m_HorizontalScrambleVF;
    VertexFormatID                m_VerticalScrambleVF;
    VertexFormatID                m_ExpandVF;
    VertexFormatID                m_CopyVF;
    VertexFormatID                m_WaterSpectrumVF;
    VertexFormatID                m_ComputeNormalsVF;

    // Ocean water textures (and corresponding surfaces)
    TextureID       m_pLowFreqWaterTex;         // Low  frequency ocean water Fourier spectrum
    TextureID       m_pHighFreqWaterTex;        // High frequency ocean water Fourier spectrum
    SurfaceID       m_pLowFreqWaterSurf;        // Low  frequency ocean water Fourier spectrum (surface for changeRenderTarget)
    SurfaceID       m_pHighFreqWaterSurf;       // High frequency ocean water Fourier spectrum (surface for changeRenderTarget)
    TextureID       m_pDisplacementMapTex;      // Low  frequency scalar Ocean water height in the spatial domain to be used for deforming geometry

    TextureID       m_pHighDisplacementMapTex;  // High frequency scalar Ocean water height in the spatial domain
    SurfaceID       m_pLowSpatialWaterSurf;     // Low  frequency ocean water height in the spatial domain (surface for changeRenderTarget)
    SurfaceID       m_pHighSpatialWaterSurf;    // High frequency ocean water height in the spatial domain (surface for changeRenderTarget)


    // FFT utility textures (and corresponding surfaces for changeRenderTarget)

    // Low resolution                               
    TextureID       m_pLowScrambleMap;          // FFT scramble texture (pre-generated for specific input map size)
    TextureID       m_pLowTexPing;              // FFT floating point complex scratch texture
    SurfaceID       m_pLowSurfPing;             // FFT floating point complex scratch surface pointer (for changeRenderTarget)
    TextureID       m_pLowTexPong;              // FFT floating point complex scratch texture
    SurfaceID       m_pLowSurfPong;             // FFT floating point complex scratch surface pointer (for changeRenderTarget)
    TextureID       m_pLowFFTButterflyMap;      // Butterfly map for FFT
    TextureID       m_pLowIFFTButterflyMap;     // Butterfly map for Inverse FFT

    // High Resolution                              
    TextureID       m_pHighScrambleMap;         // FFT scramble texture (pre-generated for specific input map size)
    TextureID       m_pHighTexPing;             // FFT floating point complex scratch texture
    SurfaceID       m_pHighSurfPing;            // FFT floating point complex scratch surface pointer (for changeRenderTarget)
    TextureID       m_pHighTexPong;             // FFT floating point complex scratch texture
    SurfaceID       m_pHighSurfPong;            // FFT floating point complex scratch surface pointer (for changeRenderTarget)
    TextureID       m_pHighFFTButterflyMap;     // Butterfly map for FFT
    TextureID       m_pHighIFFTButterflyMap;    // Butterfly map for Inverse FFT

    // Water frequency synthesis utility textures (and corresponding surfaces)
    TextureID       m_pLowWTex;                 // Low Frequency
    TextureID       m_pHighWTex;                // High Frequency
    TextureID       m_pLowAmplitude0Tex;        // Amplitude at time zero
    TextureID       m_pHighAmplitude0Tex;       // Amplitude at time zero
    TextureID       m_pLowAmplitudeTTex;        // Amplitude at time T
    TextureID       m_pHighAmplitudeTTex;       // Amplitude at time T
    SurfaceID       m_pLowAmplitudeTSurf;       // Amplitude at time T (surface for changeRenderTarget)
    SurfaceID       m_pHighAmplitudeTSurf;      // Amplitude at time T (surface for changeRenderTarget)
    TextureID       m_pLowNormalTex;            // Normal at time T
    TextureID       m_pHighNormalTex;           // Normal at time T
    SurfaceID       m_pLowNormalSurf;           // Normal at time T (surface for changeRenderTarget)
    SurfaceID       m_pHighNormalSurf;          // Normal at time T (surface for changeRenderTarget)

    // Vertex and index buffers
    VertexBufferID m_pHighQuadVB;              // Vertex buffer for low frequency screen-aligned quads
    VertexBufferID m_pLowQuadVB;               // Vertex buffer for high frequency screen-aligned quads

    // viewports
    D3DFROMWINEVIEWPORT9            m_LowQuadViewport;          // Viewport for image processing
    D3DFROMWINEVIEWPORT9            m_HighQuadViewport;         // Viewport for image processing
};

#endif //_GPU_PHYSICS_H_
