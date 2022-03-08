//-----------------------------------------------------------------------------
// File: GPUphysics.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "GPUphysics.h"

struct QUAD_VERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR2 uv;
    static const DWORD FVF;
};
const DWORD QUAD_VERTEX::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
GPU_Physics::GPU_Physics(IDirect3DDevice9 *d3dDevice, Renderer *renderer) : Physics(d3dDevice, renderer)
{
    HRESULT hr;

    m_renderer = renderer;

    // Load shaders
    if ((m_HorizontalButterflyShader = m_renderer->addShader("HorizontalButterfly.hlsl")) == SHADER_NONE)
        return;
    if ((m_VerticalButterflyShader = m_renderer->addShader("VerticalButterfly.hlsl")) == SHADER_NONE)
        return;
    if ((m_HorizontalScrambleShader = m_renderer->addShader("HorizontalScramble.hlsl")) == SHADER_NONE)
        return;
    if ((m_VerticalScrambleShader = m_renderer->addShader("VerticalScramble.hlsl")) == SHADER_NONE)
        return;
    if ((m_ExpandShader = m_renderer->addShader("Expand.hlsl")) == SHADER_NONE)
        return;
    if ((m_CopyShader = m_renderer->addShader("Copy.hlsl")) == SHADER_NONE)
        return;
    if ((m_WaterSpectrumShader = m_renderer->addShader("WaterSpectrum.hlsl")) == SHADER_NONE)
        return;
    if ((m_ComputeNormalsShader = m_renderer->addShader("ComputeNormals.hlsl")) == SHADER_NONE)
        return;

    // Viewports for the image processing steps
    m_LowQuadViewport.X = 0;
    m_LowQuadViewport.Y = 0;
    m_LowQuadViewport.Width = WATER_GRID_RESOLUTION;
    m_LowQuadViewport.Height = WATER_GRID_RESOLUTION;
    m_LowQuadViewport.MinZ = 0.0f;
    m_LowQuadViewport.MaxZ = 1.0f;

    m_HighQuadViewport.X = 0;
    m_HighQuadViewport.Y = 0;
    m_HighQuadViewport.Width = HIGH_FREQ_WATER_GRID_RESOLUTION;
    m_HighQuadViewport.Height = HIGH_FREQ_WATER_GRID_RESOLUTION;
    m_HighQuadViewport.MinZ = 0.0f;
    m_HighQuadViewport.MaxZ = 1.0f;

    // Create maps to hold frequencies and initial amplitudes and low and high frequencies
    V( D3DXCreateTexture( m_d3dDevice, WATER_GRID_RESOLUTION, WATER_GRID_RESOLUTION, 
        1, 0, D3DFMT_R32F, D3DPOOL_MANAGED , &m_pLowWTex ) );
    V( D3DXCreateTexture( m_d3dDevice, WATER_GRID_RESOLUTION, WATER_GRID_RESOLUTION, 
        1, 0, D3DFMT_A32B32G32R32F, D3DPOOL_MANAGED , &m_pLowAmplitude0Tex ) );
    V( D3DXCreateTexture( m_d3dDevice, HIGH_FREQ_WATER_GRID_RESOLUTION, 
        HIGH_FREQ_WATER_GRID_RESOLUTION, 1, 0, D3DFMT_R32F, D3DPOOL_MANAGED , &m_pHighWTex ) );
    V( D3DXCreateTexture( m_d3dDevice, HIGH_FREQ_WATER_GRID_RESOLUTION, 
        HIGH_FREQ_WATER_GRID_RESOLUTION, 1, 0, D3DFMT_A32B32G32R32F, D3DPOOL_MANAGED , &m_pHighAmplitude0Tex ) );

    // Fill non-renderable scramble and butterfly maps for low and high resolution grids
    FillFFTUtilityTextures(WATER_GRID_RESOLUTION, LOG2_GRID_RESOLUTION, &m_pLowScrambleMap, 
        &m_pLowFFTButterflyMap,  &m_pLowIFFTButterflyMap);
    FillFFTUtilityTextures(HIGH_FREQ_WATER_GRID_RESOLUTION, LOG2_HIGH_FREQ_GRID_RESOLUTION, 
        &m_pHighScrambleMap, &m_pHighFFTButterflyMap, &m_pHighIFFTButterflyMap);

    // Create the screen-aligned quad vertex buffers
    V( m_d3dDevice->CreateVertexBuffer( 4 * sizeof(QUAD_VERTEX), D3DUSAGE_WRITEONLY,
                                            QUAD_VERTEX::FVF, D3DPOOL_DEFAULT, &m_pLowQuadVB , NULL ) );
    V( m_d3dDevice->CreateVertexBuffer( 4 * sizeof(QUAD_VERTEX), D3DUSAGE_WRITEONLY,
                                            QUAD_VERTEX::FVF, D3DPOOL_DEFAULT, &m_pHighQuadVB , NULL ) );

    // Create the floating point renderable ping-pong textures
    V( D3DXCreateTexture (m_d3dDevice, WATER_GRID_RESOLUTION, WATER_GRID_RESOLUTION, 1,
                                D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT , &m_pLowTexPing ) );
    V( D3DXCreateTexture (m_d3dDevice, WATER_GRID_RESOLUTION, WATER_GRID_RESOLUTION, 1,
                                D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT , &m_pLowTexPong ) );
    V( D3DXCreateTexture (m_d3dDevice, HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, 1,
                                D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT , &m_pHighTexPing ) );
    V( D3DXCreateTexture (m_d3dDevice, HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, 1,
                                D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT , &m_pHighTexPong ) );

    // Create the floating point frequency domain and spatial domain surfaces
    V( D3DXCreateTexture (m_d3dDevice, WATER_GRID_RESOLUTION, WATER_GRID_RESOLUTION, 1,
                                D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT , &m_pLowFreqWaterTex ) );
    V( D3DXCreateTexture (m_d3dDevice, HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, 1,
                                D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT , &m_pHighFreqWaterTex ) );

    V( D3DXCreateTexture (m_d3dDevice, HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, 1,
                                D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT , &m_pHighDisplacementMapTex ) );

    // Get the surfaces for subsequent rendering
    V( m_pLowTexPing->GetSurfaceLevel(0,  &m_pLowSurfPing ) );
    V( m_pLowTexPong->GetSurfaceLevel(0,  &m_pLowSurfPong ) );
    V( m_pHighTexPing->GetSurfaceLevel(0, &m_pHighSurfPing ) );
    V( m_pHighTexPong->GetSurfaceLevel(0, &m_pHighSurfPong ) );

    V( m_pLowFreqWaterTex->GetSurfaceLevel(0,     &m_pLowFreqWaterSurf ) );
    V( m_pHighFreqWaterTex->GetSurfaceLevel(0,    &m_pHighFreqWaterSurf ) );
    V( m_pHighDisplacementMapTex->GetSurfaceLevel(0, &m_pHighSpatialWaterSurf ) );

    // Now for water wave synthesis utility maps
    V( D3DXCreateTexture (m_d3dDevice, WATER_GRID_RESOLUTION, WATER_GRID_RESOLUTION, 1,
                                D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT , &m_pLowAmplitudeTTex ) );
    V( D3DXCreateTexture (m_d3dDevice, HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, 1,
                                D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT , &m_pHighAmplitudeTTex ) );

    // Get the surfaces for subsequent rendering
    V( m_pLowAmplitudeTTex->GetSurfaceLevel(0, &m_pLowAmplitudeTSurf ) );
    V( m_pHighAmplitudeTTex->GetSurfaceLevel(0, &m_pHighAmplitudeTSurf ) );

    // water normal map
    V( D3DXCreateTexture (m_d3dDevice, HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, 0,
                            D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT , &m_pHighNormalTex ) );
    m_pHighNormalTex->SetAutoGenFilterType(D3DTEXF_LINEAR);
    V( m_pHighNormalTex->GetSurfaceLevel(0, &m_pHighNormalSurf ) );

    // Scalar output of GPU IFFT
    V( D3DXCreateTexture (m_d3dDevice, WATER_GEOM_RESOLUTION, WATER_GEOM_RESOLUTION, 1,
                            D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT ,
                            &m_pDisplacementMapTex ) );
    V( m_pDisplacementMapTex->GetSurfaceLevel(0,  &m_pLowSpatialWaterSurf ) );

    // initialize the vertex buffers for the off screen render targets
    FillScreenAlignedQuads();
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
GPU_Physics::~GPU_Physics()
{
    // shaders
    m_renderer->deleteShader(m_HorizontalButterflyShader);
    m_renderer->deleteShader(m_VerticalButterflyShader);
    m_renderer->deleteShader(m_HorizontalScrambleShader);
    m_renderer->deleteShader(m_VerticalScrambleShader);
    m_renderer->deleteShader(m_ExpandShader);
    m_renderer->deleteShader(m_CopyShader);
    m_renderer->deleteShader(m_WaterSpectrumShader);
    m_renderer->deleteShader(m_ComputeNormalsShader);

    // textures
    SAFE_RELEASE(m_pLowWTex);
    SAFE_RELEASE(m_pLowAmplitude0Tex);
    SAFE_RELEASE(m_pHighWTex);
    SAFE_RELEASE(m_pHighAmplitude0Tex);
    SAFE_RELEASE(m_pLowScrambleMap);
    SAFE_RELEASE(m_pLowFFTButterflyMap);
    SAFE_RELEASE(m_pLowIFFTButterflyMap);
    SAFE_RELEASE(m_pHighScrambleMap);
    SAFE_RELEASE(m_pHighFFTButterflyMap);
    SAFE_RELEASE(m_pHighIFFTButterflyMap);
    SAFE_RELEASE(m_pLowTexPing);
    SAFE_RELEASE(m_pLowTexPong);
    SAFE_RELEASE(m_pHighTexPing);
    SAFE_RELEASE(m_pHighTexPong);
    SAFE_RELEASE(m_pLowFreqWaterTex);
    SAFE_RELEASE(m_pHighFreqWaterTex);
    SAFE_RELEASE(m_pHighDisplacementMapTex);
    SAFE_RELEASE(m_pLowAmplitudeTTex);
    SAFE_RELEASE(m_pHighAmplitudeTTex);
    if(m_pHighNormalTex)
        m_pHighNormalTex->Release();
    SAFE_RELEASE(m_pHighNormalTex);
    SAFE_RELEASE(m_pDisplacementMapTex);

     // surfaces
    SAFE_RELEASE(m_pLowSurfPing);
    SAFE_RELEASE(m_pLowSurfPong);
    SAFE_RELEASE(m_pHighSurfPing);
    SAFE_RELEASE(m_pHighSurfPong);
    SAFE_RELEASE(m_pLowFreqWaterSurf);
    SAFE_RELEASE(m_pHighFreqWaterSurf);
    SAFE_RELEASE(m_pHighSpatialWaterSurf);
    SAFE_RELEASE(m_pLowAmplitudeTSurf);
    SAFE_RELEASE(m_pHighAmplitudeTSurf);
    SAFE_RELEASE(m_pLowSpatialWaterSurf);

    // vertex buffers
    SAFE_RELEASE(m_pLowQuadVB);
    SAFE_RELEASE(m_pHighQuadVB);
}



//-----------------------------------------------------------------------------
//
// FillFFTUtilityTextures
//
// Initialize the textures that will be used in the FFT algorithm on the GPU
//
//-----------------------------------------------------------------------------
HRESULT GPU_Physics::FillFFTUtilityTextures(int iResolution, int iLog2Resolution, 
                                            IDirect3DTexture9** ppScrambleMap, 
                                            IDirect3DTexture9** ppFFTButterflyMap, 
                                            IDirect3DTexture9** ppIFFTButterflyMap)
{
    HRESULT hr;
    float *fImage = new float[iResolution];
    WORD  *wImage = new WORD[iResolution];
    int offset, ii, mm, gg, jj = 0;
    float temp;

    // Initialize each element with its own address
    for (gg=0; gg < iResolution; gg++)
    {
        fImage[gg] = ((float)(gg+0.5f)) / ((float)(iResolution));
    }

    // Apply scramble
    for (gg=0; gg < iResolution; gg++)
    {
        if ( gg > jj)
        {
            temp = fImage[jj];
            fImage[jj] = fImage[gg];
            fImage[gg] = temp;
        }
        int m = iResolution>>1;
        while ((jj>=m) & (m >= 2))
        {
            jj -= m;
            m = m >>1;
        }
        jj += m;
    }

    // Convert to 16-bit WORD texels
    for (mm=0; mm < iResolution; mm++)
    {
        wImage[mm] = (WORD)(65535.0f*fImage[mm]);
    }

    // Create a single channel texture and load in this scramble map
    D3DLOCKED_RECT rect;

    V_RETURN( D3DXCreateTexture (m_d3dDevice, iResolution, 1, 1, 0, D3DFMT_L16, D3DPOOL_MANAGED , ppScrambleMap ) );
    (*ppScrambleMap)->LockRect(0, &rect, NULL, 0);
    stx_memcpy(rect.pBits, wImage, iResolution * sizeof(WORD));
    (*ppScrambleMap)->UnlockRect(0);

    // Delete the scratch scramble data
    delete []fImage;
    delete []wImage;

    // Create both Butterfly textures (16 rows just to be safe/flexible)
    V_RETURN( D3DXCreateTexture( m_d3dDevice, iResolution, 16, 1, 0, D3DFMT_A32B32G32R32F, D3DPOOL_MANAGED , ppFFTButterflyMap ) );
    V_RETURN( D3DXCreateTexture( m_d3dDevice, iResolution, 16, 1, 0, D3DFMT_A32B32G32R32F, D3DPOOL_MANAGED , ppIFFTButterflyMap ) );

    float angle;
    float wre, wim, wpre, wpim;
    int N, half_N;
    float wtemp;

    //
    // Forward FFT Butterfly map
    //
    fImage = new float[iResolution * 16 * 4];
    stx_memset( fImage, 0, iResolution * 16 * 4 * sizeof(float));

    N = 1;
    for (ii=0; ii < iLog2Resolution; ii++) // iLog2Resolution iterations
    {
        half_N = N;
        N <<= 1;
        angle = -2.0f * 3.1415927f / (float) N; //*dir
        wtemp = sinf( 0.5f * angle);
        wpre = -2.0f * wtemp *wtemp;
        wpim = sinf( angle);
        wre = 1.0f;
        wim = 0.0f;
        for (offset=0; offset<half_N; offset++)
        {
            for (gg=offset; gg<iResolution; gg+=N)
            {
                jj = gg + half_N;
                //store the coords in r and g
                fImage[(ii*iResolution+jj)*4 + 0] = -((jj+0.5f)/(float)iResolution);
                fImage[(ii*iResolution+jj)*4 + 1] = (gg+0.5f)/(float)iResolution;
                fImage[(ii*iResolution+gg)*4 + 0] = (jj+0.5f)/(float)iResolution;
                fImage[(ii*iResolution+gg)*4 + 1] = (gg+0.5f)/(float)iResolution;

                //store W in b and a
                fImage[(ii*iResolution+jj)*4 + 2] = wre;
                fImage[(ii*iResolution+jj)*4 + 3] = wim;
                fImage[(ii*iResolution+gg)*4 + 2] = wre;
                fImage[(ii*iResolution+gg)*4 + 3] = wim;
            }
            wtemp = wre;
            wre = wtemp*wpre - wim*wpim + wre;
            wim = wim*wpre + wtemp*wpim + wim;
        }
    }

    (*ppFFTButterflyMap)->LockRect(0, &rect, NULL, 0);
    stx_memcpy(rect.pBits, fImage, iResolution * 16 * 4 * sizeof(float));
    (*ppFFTButterflyMap)->UnlockRect(0);

    delete []fImage;

    //
    // Inverse FFT Butterfly map
    //
    fImage = new float[iResolution*16*4];
    stx_memset( fImage, 0, iResolution*16*4*sizeof(float));

    N = 1;
    for (ii=0; ii < iLog2Resolution; ii++) // iLog2Resolution iterations
    {
        half_N = N;
        N <<= 1;
        angle = -2.0f * 3.1415927f / (float) -N;
        wtemp = sinf( 0.5f * angle);
        wpre = -2.0f * wtemp *wtemp;
        wpim = sinf( angle);
        wre = 1.0f;
        wim = 0.0f;

        for (offset=0; offset < half_N; offset++)
        {
            for (gg=offset; gg<iResolution; gg+=N)
            {
                jj = gg + half_N;
                //store the coords in r and g
                fImage[(ii*iResolution+jj)*4 + 0] = -(jj+0.5f)/(float)iResolution;
                fImage[(ii*iResolution+jj)*4 + 1] = (gg+0.5f)/(float)iResolution;
                fImage[(ii*iResolution+gg)*4 + 0] = (jj+0.5f)/(float)iResolution;
                fImage[(ii*iResolution+gg)*4 + 1] = (gg+0.5f)/(float)iResolution;

                //store W in b and a
                fImage[(ii*iResolution+jj)*4 + 2] = wre;
                fImage[(ii*iResolution+jj)*4 + 3] = wim;
                fImage[(ii*iResolution+gg)*4 + 2] = wre;
                fImage[(ii*iResolution+gg)*4 + 3] = wim;
            }
            wtemp = wre;
            wre = wtemp*wpre - wim*wpim + wre;
            wim = wim*wpre + wtemp*wpim + wim;
        }
    }

    (*ppIFFTButterflyMap)->LockRect(0, &rect, NULL, 0);
    stx_memcpy(rect.pBits, fImage, iResolution * 16 * 4 * sizeof(float));
    (*ppIFFTButterflyMap)->UnlockRect(0);

    delete []fImage;

    return S_OK;
}

//-----------------------------------------------------------------------------
//
// FillScreenAlignedQuads()
//
// Fill the vertex buffer for the screen-aligned quad used by FFT
//-----------------------------------------------------------------------------
HRESULT GPU_Physics::FillScreenAlignedQuads()
{
    HRESULT hr = S_OK;

    float fTexelSize = 0.5f / ((float)WATER_GRID_RESOLUTION);

    //
    // Lock and fill the vertex buffer
    //
    QUAD_VERTEX* pQV;
    V_RETURN( m_pLowQuadVB->Lock( 0, 0, (void**)&pQV, 0 ) );

        pQV[0].p  = D3DXVECTOR3(-1.0f, -1.0f, 0.5f);
        pQV[0].uv = D3DXVECTOR2(fTexelSize, 1.0f+fTexelSize);

        pQV[1].p  = D3DXVECTOR3(1.0f, -1.0f, 0.5f);
        pQV[1].uv = D3DXVECTOR2(1.0f+fTexelSize, 1.0f+fTexelSize);

        pQV[2].p  = D3DXVECTOR3(-1.0f, 1.0f, 0.5f);
        pQV[2].uv = D3DXVECTOR2(fTexelSize, fTexelSize);

        pQV[3].p  = D3DXVECTOR3(1.0f, 1.0f, 0.5f);
        pQV[3].uv = D3DXVECTOR2(1.0f+fTexelSize, fTexelSize);

    V_RETURN( m_pLowQuadVB->Unlock(); );


    fTexelSize = 0.5f / ((float)HIGH_FREQ_WATER_GRID_RESOLUTION);

    // Lock and fill the vertex buffer
    V_RETURN( m_pHighQuadVB->Lock( 0, 0, (void**)&pQV, 0 ) );

        pQV[0].p  = D3DXVECTOR3(-1.0f, -1.0f, 0.5f);
        pQV[0].uv = D3DXVECTOR2(fTexelSize, 1.0f+fTexelSize);

        pQV[1].p  = D3DXVECTOR3(1.0f, -1.0f, 0.5f);
        pQV[1].uv = D3DXVECTOR2(1.0f+fTexelSize, 1.0f+fTexelSize);

        pQV[2].p  = D3DXVECTOR3(-1.0f, 1.0f, 0.5f);
        pQV[2].uv = D3DXVECTOR2(fTexelSize, fTexelSize);

        pQV[3].p  = D3DXVECTOR3(1.0f, 1.0f, 0.5f);
        pQV[3].uv = D3DXVECTOR2(1.0f+fTexelSize, fTexelSize);

    V_RETURN( m_pHighQuadVB->Unlock(); );


    return hr;
}

//--------------------------------------------------------------------------------------
//
// GenerateGPUFourierSpectrum
//
// First argument points to the grayscale input, second argument is time, 
// and the third argument is the resolution of the input surface.
//
//--------------------------------------------------------------------------------------
HRESULT GPU_Physics::GenerateFourierSpectrum(IDirect3DSurface9* pOutputSurf, float fTime, int iResolution)
{
    HRESULT hr;
    IDirect3DSurface9*      pMainBackBufferSurf;
    IDirect3DSurface9*      pMainZBufferSurf;
    IDirect3DTexture9*      pWTex;
    IDirect3DTexture9*      pAmplitude0Tex;
    IDirect3DVertexBuffer9* pQuadVB;
    D3DVIEWPORT9*           pQuadViewport;

    if (iResolution == WATER_GRID_RESOLUTION)
    {
        pWTex          = m_pLowWTex;
        pAmplitude0Tex = m_pLowAmplitude0Tex;
        pQuadVB        = m_pLowQuadVB;
        pQuadViewport  = &m_LowQuadViewport;
    }
    else
    {
        pWTex          = m_pHighWTex;
        pAmplitude0Tex = m_pHighAmplitude0Tex;
        pQuadVB        = m_pHighQuadVB;
        pQuadViewport  = &m_HighQuadViewport;
    }

    // Save these off to be restored later
    V( m_d3dDevice->GetRenderTarget(0, &pMainBackBufferSurf));
    V( m_d3dDevice->SetRenderTarget(0, pOutputSurf));
    V( m_d3dDevice->GetDepthStencilSurface(&pMainZBufferSurf));
    V( m_d3dDevice->SetDepthStencilSurface(NULL));

    V( m_d3dDevice->SetViewport(pQuadViewport); );
    V( m_d3dDevice->SetStreamSource( 0, pQuadVB , 0, sizeof(QUAD_VERTEX) ) );
    V( m_d3dDevice->SetFVF( QUAD_VERTEX::FVF ) );

    // Evaluate frequency spectrum at fTime
    m_renderer->changeShader(m_WaterSpectrumShader);
    int index = m_renderer->getSamplerIndex(m_WaterSpectrumShader, "freqSamp");
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetTexture(index, pWTex));
    index = m_renderer->getSamplerIndex(m_WaterSpectrumShader, "amplitudeSamp");
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetTexture(index, pAmplitude0Tex));
    m_renderer->changeShaderConstant1f("g_fTime", fTime);
    V(m_d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    V(m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
    V(m_d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
    V(m_d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
    V(m_d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS));

    V( m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) );

    // Subsequent rendering as before
    V( m_d3dDevice->SetRenderTarget(0, pMainBackBufferSurf));
    V( pMainBackBufferSurf->Release());
    V( m_d3dDevice->SetDepthStencilSurface(pMainZBufferSurf));
    V( pMainZBufferSurf->Release());

    return S_OK;
}


//--------------------------------------------------------------------------------------
// First argument points to WATER_GRID_RESOLUTION x WATER_GRID_RESOLUTION input texture
// Second argument points to WATER_GRID_RESOLUTION x WATER_GRID_RESOLUTION output surface
//--------------------------------------------------------------------------------------
HRESULT GPU_Physics::ComputeWaterNormals(IDirect3DTexture9* pInputTex,
                                         IDirect3DSurface9* pOutputSurf, int iResolution)
{
    HRESULT hr;
    IDirect3DSurface9*      pMainBackBufferSurf;
    IDirect3DSurface9*      pMainZBufferSurf;
    IDirect3DVertexBuffer9* pQuadVB;
    D3DVIEWPORT9*           pQuadViewport;

    if (iResolution == WATER_GRID_RESOLUTION)
    {
        pQuadVB        = m_pLowQuadVB;
        pQuadViewport  = &m_LowQuadViewport;
    }
    else
    {
        pQuadVB        = m_pHighQuadVB;
        pQuadViewport  = &m_HighQuadViewport;
    }

    assert(m_d3dDevice != NULL);

    // Save these off to be restored later
    V( m_d3dDevice->GetRenderTarget(0, &pMainBackBufferSurf));
    V( m_d3dDevice->SetRenderTarget(0, pOutputSurf));
    V( m_d3dDevice->GetDepthStencilSurface(&pMainZBufferSurf));
    V( m_d3dDevice->SetDepthStencilSurface(NULL));

    V( m_d3dDevice->SetViewport(pQuadViewport); );
    V( m_d3dDevice->SetStreamSource( 0, pQuadVB , 0, sizeof(QUAD_VERTEX) ) );
    V( m_d3dDevice->SetFVF( QUAD_VERTEX::FVF ) );

    // Run Sobel filter over input map
    m_renderer->changeShader(m_ComputeNormalsShader);
    m_renderer->changeShaderConstant1f("g_fTexelSize",  1.0f/((float)iResolution));
    int index = m_renderer->getSamplerIndex(m_ComputeNormalsShader, "amplitudeSamp");
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetTexture(index, pInputTex));

    V( m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) );

    V(m_d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    V(m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
    V(m_d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
    V(m_d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
    V(m_d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS));

    // Subsequent rendering as before
    V( m_d3dDevice->SetRenderTarget(0, pMainBackBufferSurf));
    V( pMainBackBufferSurf->Release());
    V( m_d3dDevice->SetDepthStencilSurface(pMainZBufferSurf));
    V( pMainZBufferSurf->Release());

    return S_OK;
}


//--------------------------------------------------------------------------------------
// 
// SwapTextures
//
// Pointer swapping utility for FFT implementation below
//--------------------------------------------------------------------------------------
void SwapTextures(IDirect3DTexture9** ppATex, IDirect3DTexture9** ppBTex, 
                  IDirect3DSurface9** ppASurf, IDirect3DSurface9** ppBSurf)
{
    IDirect3DTexture9* pTempTex;
    IDirect3DSurface9* pTempSurf;

    // Check the pointers in debug
    assert(ppATex != NULL);
    assert(ppBTex != NULL);
    assert(ppASurf != NULL);
    assert(ppBSurf != NULL);

    pTempTex  = *ppATex;
    *ppATex   = *ppBTex;
    *ppBTex   =  pTempTex;

    pTempSurf = *ppASurf;
    *ppASurf  = *ppBSurf;
    *ppBSurf  =  pTempSurf;
}


//--------------------------------------------------------------------------------------
//
// Optimized D3DX effect implementation of Cooley and Tukey's decimation in time.  See:
// http://www.ati.com/developer/ShaderX2_AdvancedImageProcessing.pdf for more details
//
// First argument points to grayscale input surface
// Second argument should be TRUE for FFT and FALSE for Inverse FFT
//
//--------------------------------------------------------------------------------------
HRESULT GPU_Physics::PerformFFT(IDirect3DTexture9* pInputTex, IDirect3DSurface9* pOutputSurf, BOOL bDoForwardFFT, 
                      int iResolution, int iLog2Resolution)
{
    HRESULT hr;
    int i;

    // Local pointers which will get hooked up according to resolution
    IDirect3DTexture9*      pFFTButterflyMap;
    IDirect3DTexture9*      pIFFTButterflyMap;
    IDirect3DTexture9*      pButterflyMap;
    IDirect3DTexture9*      pScrambleMap;
    IDirect3DTexture9*      pDstTex;
    IDirect3DTexture9*      pSrcTex;
    IDirect3DSurface9*      pDstSurf;
    IDirect3DSurface9*      pSrcSurf;
    IDirect3DSurface9*      pMainBackBufferSurf;
    IDirect3DSurface9*      pMainZBufferSurf;
    IDirect3DVertexBuffer9* pQuadVB;
    D3DVIEWPORT9*           pQuadViewport;

    // Check the pointers in debug
    assert(pInputTex != NULL);
    assert(pOutputSurf != NULL);

    // Set up surfaces for low resolution grid
    if (iResolution == WATER_GRID_RESOLUTION)
    {
        pDstTex           = m_pLowTexPing;
        pSrcTex           = m_pLowTexPong;
        pDstSurf          = m_pLowSurfPing;
        pSrcSurf          = m_pLowSurfPong;
        pFFTButterflyMap  = m_pLowFFTButterflyMap;
        pIFFTButterflyMap = m_pLowIFFTButterflyMap;
        pScrambleMap      = m_pLowScrambleMap;
        pQuadVB           = m_pLowQuadVB;
        pQuadViewport     = &m_LowQuadViewport;
    }
    else if (iResolution == HIGH_FREQ_WATER_GRID_RESOLUTION)
    {
        pDstTex           = m_pHighTexPing;
        pSrcTex           = m_pHighTexPong;
        pDstSurf          = m_pHighSurfPing;
        pSrcSurf          = m_pHighSurfPong;
        pFFTButterflyMap  = m_pHighFFTButterflyMap;
        pIFFTButterflyMap = m_pHighIFFTButterflyMap;
        pScrambleMap      = m_pHighScrambleMap;
        pQuadVB           = m_pHighQuadVB;
        pQuadViewport     = &m_HighQuadViewport;
    }
    else
    {
        OutputDebugString("Bad resolution in PerformGPUFFT()\n");
    }


    // Save these off to be restored later
    V( m_d3dDevice->GetRenderTarget(0, &pMainBackBufferSurf));
    V( m_d3dDevice->SetRenderTarget(0, pDstSurf));
    V( m_d3dDevice->GetDepthStencilSurface(&pMainZBufferSurf));
    V( m_d3dDevice->SetDepthStencilSurface(NULL));

    // Select proper butterfly map for subsequent steps (Forward FFT or Inverse FFT)
    if (bDoForwardFFT)
    {
        pButterflyMap = pFFTButterflyMap;
    }
    else
    {
        pButterflyMap = pIFFTButterflyMap;
    }

    // These stay the same throughout the FFT process
    V( m_d3dDevice->SetViewport(pQuadViewport); );
    V( m_d3dDevice->SetStreamSource( 0, pQuadVB , 0, sizeof(QUAD_VERTEX) ) );   // Simple quadrilateral VB
    V( m_d3dDevice->SetFVF( QUAD_VERTEX::FVF ) );                               // Simple vertex format
    V(m_d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    V(m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
    V(m_d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
    V(m_d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
    V(m_d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS));
    V(m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE));

    // Do horizontal scramble
    m_renderer->changeShader(m_HorizontalScrambleShader);
    int index = m_renderer->getSamplerIndex(m_HorizontalScrambleShader, "sampSourceImage");
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetTexture(index, pInputTex));
    index = m_renderer->getSamplerIndex(m_HorizontalScrambleShader, "sampScramble");
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));
    V(m_d3dDevice->SetTexture(index, pScrambleMap));

    V( m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) );

    // Swap the textures
    SwapTextures(&pDstTex, &pSrcTex, &pDstSurf, &pSrcSurf);
    V( m_d3dDevice->SetRenderTarget(0, pDstSurf));

    // Do horizontal butterfly passes
    m_renderer->changeShader(m_HorizontalButterflyShader);
    index = m_renderer->getSamplerIndex(m_HorizontalButterflyShader, "sampSourceImage");
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetTexture(index, pSrcTex));
    index = m_renderer->getSamplerIndex(m_HorizontalButterflyShader, "sampButterfly");
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));
    V(m_d3dDevice->SetTexture(index, pButterflyMap));

    // Perform log2(resolution) butterfly passes
    for (i=0; i < iLog2Resolution; i++)
    {
        float fPassNumber = ((float)i)/16.0f + 1.0f/32.0f; // Danger...this is a function of the size of the padded butterfly map
        m_renderer->changeShaderConstant1f("fPassNum", fPassNumber);

        V( m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) );

        SwapTextures(&pDstTex, &pSrcTex, &pDstSurf, &pSrcSurf);
        V( m_d3dDevice->SetRenderTarget(0, pDstSurf));
        V(SetTexture(m_HorizontalButterflyShader, "sampSourceImage", pSrcTex));
    }

    // Do vertical scramble
    m_renderer->changeShader(m_VerticalScrambleShader);
    index = m_renderer->getSamplerIndex(m_VerticalScrambleShader, "sampSourceImage");
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetTexture(index, pSrcTex));
    index = m_renderer->getSamplerIndex(m_VerticalScrambleShader, "sampScramble");
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));
    V(m_d3dDevice->SetTexture(index, pScrambleMap));

    V( m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) );

    // Swap the textures
    SwapTextures(&pDstTex, &pSrcTex, &pDstSurf, &pSrcSurf);
    V( m_d3dDevice->SetRenderTarget(0, pDstSurf));

    // Do vertical butterfly passes
    m_renderer->changeShader(m_VerticalButterflyShader);
    index = m_renderer->getSamplerIndex(m_VerticalButterflyShader, "sampSourceImage");
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
    V(m_d3dDevice->SetTexture(index, pSrcTex));
    index = m_renderer->getSamplerIndex(m_VerticalButterflyShader, "sampButterfly");
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
    V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));
    V(m_d3dDevice->SetTexture(index, pButterflyMap));

    // Perform log2(resolution) butterfly passes
    for (i=0; i < iLog2Resolution; i++)
    {
        float fPassNumber = ((float)i)/16.0f + 1.0f/32.0f; // Danger...this is a function of the size of the padded butterfly map
        m_renderer->changeShaderConstant1f("fPassNum", fPassNumber);

        V( m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) );

        SwapTextures(&pDstTex, &pSrcTex, &pDstSurf, &pSrcSurf);
        V( m_d3dDevice->SetRenderTarget(0, pDstSurf));
        V(SetTexture(m_VerticalButterflyShader, "sampSourceImage", pSrcTex));
    }

    //
    // Resolve whatever is in pSrcTex into output surface
    //

    if (iResolution == WATER_GRID_RESOLUTION)
    {
        V( m_d3dDevice->SetRenderTarget(0, pOutputSurf));
        m_renderer->changeShader(m_ExpandShader);
        index = m_renderer->getSamplerIndex(m_ExpandShader, "sampSourceImage");
        V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
        V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
        V(m_d3dDevice->SetTexture(index, pSrcTex));

        // Set the wave height and grid repeat rate
        m_renderer->changeShaderConstant1f("g_heightScale", HEIGHT_SCALE);
        m_renderer->changeShaderConstant1f("g_repeatRate", REPEAT_RATE);

        V( m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) );
    }
    else
    {
        V( m_d3dDevice->SetRenderTarget(0, pOutputSurf));
        m_renderer->changeShader(m_CopyShader);
        index = m_renderer->getSamplerIndex(m_CopyShader, "sampSourceImage");
        V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
        V(m_d3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
        V(m_d3dDevice->SetTexture(index, pSrcTex));

        V( m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) );
    }

    // Subsequent rendering as before
    V( m_d3dDevice->SetRenderTarget(0, pMainBackBufferSurf));
    V( pMainBackBufferSurf->Release());
    V( m_d3dDevice->SetDepthStencilSurface(pMainZBufferSurf));
    V( pMainZBufferSurf->Release());

    return S_OK;
}

//--------------------------------------------------------------------------------------
//
//  ComputeWaves
//
//  Generates waves in the frequency domain and uses an inverse FFT to store them
//  in a height field. Also calls ComputeWaterNormals to create a normal map from
//  the height map.
//--------------------------------------------------------------------------------------
void GPU_Physics::ComputeWaves(float windSpeed, float windDirection, float time)
{
    // use an orthographic projection
    m_renderer->changeProjectionMatrix(orthoMatrix(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, true));
    m_renderer->changeModelViewMatrix(identity4());

    // Generate spectra for both high and low frequencies
    GenerateFourierSpectrum(m_pLowFreqWaterSurf, time, WATER_GRID_RESOLUTION);
    GenerateFourierSpectrum(m_pHighFreqWaterSurf, time, HIGH_FREQ_WATER_GRID_RESOLUTION);

    // Perform GPU-side IFFTs on high and low frequency water textures (from frequency domain to spatial domain)
    PerformFFT(m_pLowFreqWaterTex, m_pLowSpatialWaterSurf, FALSE, 
            WATER_GRID_RESOLUTION, LOG2_GRID_RESOLUTION);
    PerformFFT(m_pHighFreqWaterTex, m_pHighSpatialWaterSurf, FALSE, 
            HIGH_FREQ_WATER_GRID_RESOLUTION, LOG2_HIGH_FREQ_GRID_RESOLUTION);

    // Compute normals of spatial water surfaces and use the hardware to mip-map them
    ComputeWaterNormals(m_pHighDisplacementMapTex, m_pHighNormalSurf, HIGH_FREQ_WATER_GRID_RESOLUTION);
}

//--------------------------------------------------------------------------------------
//
//  FillWaterUtilityTextures
//
//  See [Tessendorf99] notes section 3.4 for the details, but here we
//  initialize the amplitudes and phases at time zero.
//
//    ~           1
//    h0(k) =  -------  (Er + iEi) sqrt(Ph(k))
//             sqrt(2)
//
//        ~
//  where h0 is the initial amplitude as a function of wave direction k
//        Ph(K) is the Phillips spectrum for amplitude based on wave direction
//        Er and Ei are Gaussians with mean 0 and variance 1
//--------------------------------------------------------------------------------------
void GPU_Physics::FillWaterUtilityBuffers(float windSpeed, float windDirection)
{
    int i, j;
    D3DLOCKED_RECT rect;
    float *frequency;
    float *amp;

    // Lock and fill low frequency m_pLowWTex
    m_pLowWTex->LockRect(0, &rect, NULL, 0);

    frequency = (float *) rect.pBits;

    for (i = 0; i < WATER_GRID_RESOLUTION; i++)
    {
        for (j = 0; j < WATER_GRID_RESOLUTION; j++)
        {
            *frequency = m_LowW[i][j];
            frequency++;
        }
    }
    m_pLowWTex->UnlockRect(0);

    // Lock and fill the high frequency m_pHighWTex
    m_pHighWTex->LockRect(0, &rect, NULL, 0);

    frequency = (float *) rect.pBits;

    for (i = 0; i < HIGH_FREQ_WATER_GRID_RESOLUTION; i++)
    {
        for (j = 0; j < HIGH_FREQ_WATER_GRID_RESOLUTION; j++)
        {
            *frequency = m_HighW[i][j];
            frequency++;
        }
    }
    m_pHighWTex->UnlockRect(0);

    // Lock and fill m_pLowAmplitude0Tex
    m_pLowAmplitude0Tex->LockRect(0, &rect, NULL, 0);
    amp = (float *) rect.pBits;

    for (i = 0; i < WATER_GRID_RESOLUTION; i++)
    {
        for (j = 0; j < WATER_GRID_RESOLUTION; j++)
        {
            // These are in a weird order in memory because of the ABGR ordering of the float texture
            // See ps_water_spectrum() in WaterNormalMap.hlsl for the usage of this map
            *amp++ = c_im(m_LowAmplitude0[WATER_GRID_RESOLUTION-i][WATER_GRID_RESOLUTION-j]);
            *amp++ = c_re(m_LowAmplitude0[WATER_GRID_RESOLUTION-i][WATER_GRID_RESOLUTION-j]);
            *amp++ = c_im(m_LowAmplitude0[i][j]);
            *amp++ = c_re(m_LowAmplitude0[i][j]);
        }
    }
    m_pLowAmplitude0Tex->UnlockRect(0);


    // Lock and fill m_pHighAmplitude0Tex
    m_pHighAmplitude0Tex->LockRect(0, &rect, NULL, 0);
    amp = (float *) rect.pBits;

    for (i = 0; i < HIGH_FREQ_WATER_GRID_RESOLUTION; i++)
    {
        for (j = 0; j < HIGH_FREQ_WATER_GRID_RESOLUTION; j++)
        {
            // These are in a weird order in memory because of the ABGR ordering of the float texture
            // See ps_water_spectrum() in WaterNormalMap.hlsl for the usage of this map
            *amp++ = c_im(m_HighAmplitude0[HIGH_FREQ_WATER_GRID_RESOLUTION-i][HIGH_FREQ_WATER_GRID_RESOLUTION-j]);
            *amp++ = c_re(m_HighAmplitude0[HIGH_FREQ_WATER_GRID_RESOLUTION-i][HIGH_FREQ_WATER_GRID_RESOLUTION-j]);
            *amp++ = c_im(m_HighAmplitude0[i][j]);
            *amp++ = c_re(m_HighAmplitude0[i][j]);
        }
    }
    m_pHighAmplitude0Tex->UnlockRect(0);
}
