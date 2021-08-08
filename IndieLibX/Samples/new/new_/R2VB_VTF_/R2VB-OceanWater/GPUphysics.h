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
    GPU_Physics(IDirect3DDevice9 *d3dDevice, Renderer *renderer);
    ~GPU_Physics();
    void ComputeWaves(float windSpeed, float windDirection, float time);
    IDirect3DTexture9 *GetNormalMap(){return m_pHighNormalTex;}
    void *GetDisplacementMap() {return m_pDisplacementMapTex;}
    void FillWaterUtilityBuffers(float windSpeed, float windDirection);
private:
    HRESULT FillFFTUtilityTextures(int iResolution, int iLog2Resolution, 
        IDirect3DTexture9** ppScrambleMap, IDirect3DTexture9** ppFFTButterflyMap, 
        IDirect3DTexture9** ppIFFTButterflyMap);

    HRESULT FillScreenAlignedQuads();
    HRESULT PerformFFT(IDirect3DTexture9* pInputTex, IDirect3DSurface9* pOutputSurf, BOOL bDoForwardFFT, 
        int iResolution, int iLog2Resolution);

    HRESULT GenerateFourierSpectrum(IDirect3DSurface9* pOutputSurf, float fTime, int iResolution);
    HRESULT ComputeWaterNormals(IDirect3DTexture9* pInputTex, IDirect3DSurface9* pOutputSurf, int iResolution);
    HRESULT SetTexture(ShaderID shader, char* name, IDirect3DTexture9* texture)
    {
        int index = m_renderer->getSamplerIndex(shader, name);
        return m_d3dDevice->SetTexture(index, texture);
    }

    Renderer*               m_renderer;                         // SDK framework render class
    ShaderID                m_HorizontalButterflyShader;
    ShaderID                m_VerticalButterflyShader;
    ShaderID                m_HorizontalScrambleShader;
    ShaderID                m_VerticalScrambleShader;
    ShaderID                m_ExpandShader;
    ShaderID                m_CopyShader;
    ShaderID                m_WaterSpectrumShader;
    ShaderID                m_ComputeNormalsShader;

    // Ocean water textures (and corresponding surfaces)
    IDirect3DTexture9*      m_pLowFreqWaterTex;         // Low  frequency ocean water Fourier spectrum
    IDirect3DTexture9*      m_pHighFreqWaterTex;        // High frequency ocean water Fourier spectrum
    IDirect3DSurface9*      m_pLowFreqWaterSurf;        // Low  frequency ocean water Fourier spectrum (surface for SetRenderTarget)
    IDirect3DSurface9*      m_pHighFreqWaterSurf;       // High frequency ocean water Fourier spectrum (surface for SetRenderTarget)
    IDirect3DTexture9*      m_pDisplacementMapTex;      // Low  frequency scalar Ocean water height in the spatial domain to be used for deforming geometry

    IDirect3DTexture9*      m_pHighDisplacementMapTex;  // High frequency scalar Ocean water height in the spatial domain
    IDirect3DSurface9*      m_pLowSpatialWaterSurf;     // Low  frequency ocean water height in the spatial domain (surface for SetRenderTarget)
    IDirect3DSurface9*      m_pHighSpatialWaterSurf;    // High frequency ocean water height in the spatial domain (surface for SetRenderTarget)


    // FFT utility textures (and corresponding surfaces for SetRenderTarget)

    // Low resolution                               
    IDirect3DTexture9*      m_pLowScrambleMap;          // FFT scramble texture (pre-generated for specific input map size)
    IDirect3DTexture9*      m_pLowTexPing;              // FFT floating point complex scratch texture
    IDirect3DSurface9*      m_pLowSurfPing;             // FFT floating point complex scratch surface pointer (for SetRenderTarget)
    IDirect3DTexture9*      m_pLowTexPong;              // FFT floating point complex scratch texture
    IDirect3DSurface9*      m_pLowSurfPong;             // FFT floating point complex scratch surface pointer (for SetRenderTarget)
    IDirect3DTexture9*      m_pLowFFTButterflyMap;      // Butterfly map for FFT
    IDirect3DTexture9*      m_pLowIFFTButterflyMap;     // Butterfly map for Inverse FFT

    // High Resolution                              
    IDirect3DTexture9*      m_pHighScrambleMap;         // FFT scramble texture (pre-generated for specific input map size)
    IDirect3DTexture9*      m_pHighTexPing;             // FFT floating point complex scratch texture
    IDirect3DSurface9*      m_pHighSurfPing;            // FFT floating point complex scratch surface pointer (for SetRenderTarget)
    IDirect3DTexture9*      m_pHighTexPong;             // FFT floating point complex scratch texture
    IDirect3DSurface9*      m_pHighSurfPong;            // FFT floating point complex scratch surface pointer (for SetRenderTarget)
    IDirect3DTexture9*      m_pHighFFTButterflyMap;     // Butterfly map for FFT
    IDirect3DTexture9*      m_pHighIFFTButterflyMap;    // Butterfly map for Inverse FFT

    // Water frequency synthesis utility textures (and corresponding surfaces)
    IDirect3DTexture9*      m_pLowWTex;                 // Low Frequency
    IDirect3DTexture9*      m_pHighWTex;                // High Frequency
    IDirect3DTexture9*      m_pLowAmplitude0Tex;        // Amplitude at time zero
    IDirect3DTexture9*      m_pHighAmplitude0Tex;       // Amplitude at time zero
    IDirect3DTexture9*      m_pLowAmplitudeTTex;        // Amplitude at time T
    IDirect3DTexture9*      m_pHighAmplitudeTTex;       // Amplitude at time T
    IDirect3DSurface9*      m_pLowAmplitudeTSurf;       // Amplitude at time T (surface for SetRenderTarget)
    IDirect3DSurface9*      m_pHighAmplitudeTSurf;      // Amplitude at time T (surface for SetRenderTarget)
    IDirect3DTexture9*      m_pLowNormalTex;            // Normal at time T
    IDirect3DTexture9*      m_pHighNormalTex;           // Normal at time T
    IDirect3DSurface9*      m_pLowNormalSurf;           // Normal at time T (surface for SetRenderTarget)
    IDirect3DSurface9*      m_pHighNormalSurf;          // Normal at time T (surface for SetRenderTarget)

    // Vertex and index buffers
    LPDIRECT3DVERTEXBUFFER9 m_pHighQuadVB;              // Vertex buffer for low frequency screen-aligned quads
    LPDIRECT3DVERTEXBUFFER9 m_pLowQuadVB;               // Vertex buffer for high frequency screen-aligned quads

    // viewports
    D3DVIEWPORT9            m_LowQuadViewport;          // Viewport for image processing
    D3DVIEWPORT9            m_HighQuadViewport;         // Viewport for image processing
};

#endif //_GPU_PHYSICS_H_
