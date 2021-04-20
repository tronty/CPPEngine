/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// File: GPUphysics.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "GPUphysics.h"

#define LOG_FNLN STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
struct QUAD_VERTEX
{
    D3DXFROMWINEVECTOR3 p;
    D3DXFROMWINEVECTOR2 uv;
    //static const __DWORD__ FVF;
};
//const __DWORD__ QUAD_VERTEX::FVF = D3DFVF_XYZ | D3DFVF_TEX1;
    float* GPU_Physics::GetDisplacementMap()
    {
	int pitch=0;
        float* p=(float*)IRenderer::GetRendererInstance()->lockTexture(m_pDisplacementMapTex, 0, pitch);
        //IRenderer::GetRendererInstance()->unlockTextureVrtl(m_pDisplacementMapTex, 0);
    	return p;
    } // ??? Lock/Unlock/Image3::getPixels
    float* GPU_Physics::GetNormalMap()
    {
	int pitch=0;
        float* p=(float*)IRenderer::GetRendererInstance()->lockTexture(//m_pNormalMapTex
        								m_pDisplacementMapTex, 0, pitch);
        //IRenderer::GetRendererInstance()->unlockTextureVrtl(m_pNormalMapTex, 0);
    	return p;
    } // ??? Lock/Unlock/Image3::getPixels
//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
GPU_Physics::GPU_Physics() : Physics()
{
    int hr;


    // Load shaders
    if ((m_HorizontalButterflyShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-OceanWater/HorizontalButterfly.shd")) == SHADER_NONE)
        return;
    if ((m_VerticalButterflyShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-OceanWater/VerticalButterfly.shd")) == SHADER_NONE)
        return;
    if ((m_HorizontalScrambleShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-OceanWater/HorizontalScramble.shd")) == SHADER_NONE)
        return;
    if ((m_VerticalScrambleShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-OceanWater/VerticalScramble.shd")) == SHADER_NONE)
        return;
    if ((m_ExpandShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-OceanWater/Expand.shd")) == SHADER_NONE)
        return;
    if ((m_CopyShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-OceanWater/Copy.shd")) == SHADER_NONE)
        return;
    if ((m_WaterSpectrumShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-OceanWater/WaterSpectrum.shd")) == SHADER_NONE)
        return;
    if ((m_ComputeNormalsShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-OceanWater/ComputeNormals.shd")) == SHADER_NONE)
        return;
    {FormatDesc attribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 4, 0, TYPE_TEXCOORD, FORMAT_FLOAT, 2 };
    m_HorizontalButterflyVF = IRenderer::GetRendererInstance()->addVertexFormat(attribs, elementsOf(attribs), m_HorizontalButterflyShader);
    m_VerticalButterflyVF = IRenderer::GetRendererInstance()->addVertexFormat(attribs, elementsOf(attribs), m_VerticalButterflyShader);
    m_HorizontalScrambleVF = IRenderer::GetRendererInstance()->addVertexFormat(attribs, elementsOf(attribs), m_HorizontalScrambleShader);
    m_VerticalScrambleVF = IRenderer::GetRendererInstance()->addVertexFormat(attribs, elementsOf(attribs), m_VerticalScrambleShader);
    m_ExpandVF = IRenderer::GetRendererInstance()->addVertexFormat(attribs, elementsOf(attribs), m_ExpandShader);
    m_CopyVF = IRenderer::GetRendererInstance()->addVertexFormat(attribs, elementsOf(attribs), m_CopyShader);
    m_WaterSpectrumVF = IRenderer::GetRendererInstance()->addVertexFormat(attribs, elementsOf(attribs), m_WaterSpectrumShader);
    m_ComputeNormalsVF = IRenderer::GetRendererInstance()->addVertexFormat(attribs, elementsOf(attribs), m_ComputeNormalsShader);}


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
    Image3 tex;
    tex.create(FORMAT_R32F, WATER_GRID_RESOLUTION, WATER_GRID_RESOLUTION, 1, 1);
    m_pLowWTex = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
    tex.create(FORMAT_RGBA32F, WATER_GRID_RESOLUTION, WATER_GRID_RESOLUTION, 1, 1);
    m_pLowAmplitude0Tex = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
    tex.create(FORMAT_R32F, HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, 1, 1);
    m_pHighWTex = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
    tex.create(FORMAT_RGBA32F, HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, 1, 1);
    m_pHighAmplitude0Tex = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());

    // Fill non-renderable scramble and butterfly maps for low and high resolution grids
    FillFFTUtilityTextures(WATER_GRID_RESOLUTION, LOG2_GRID_RESOLUTION, m_pLowScrambleMap, 
        m_pLowFFTButterflyMap,  m_pLowIFFTButterflyMap);
    FillFFTUtilityTextures(HIGH_FREQ_WATER_GRID_RESOLUTION, LOG2_HIGH_FREQ_GRID_RESOLUTION, 
        m_pHighScrambleMap, m_pHighFFTButterflyMap, m_pHighIFFTButterflyMap);


    // Create the screen-aligned quad vertex buffers
    m_pLowQuadVB = IRenderer::GetRendererInstance()->addVertexBuffer(4 * sizeof(QUAD_VERTEX), DYNAMIC);
    m_pHighQuadVB = IRenderer::GetRendererInstance()->addVertexBuffer(4 * sizeof(QUAD_VERTEX), DYNAMIC);


    // Create the floating point renderable ping-pong textures
    m_pLowTexPing = IRenderer::GetRendererInstance()->addRenderTarget(WATER_GRID_RESOLUTION, WATER_GRID_RESOLUTION, FORMAT_RGBA32F, IRenderer::GetRendererInstance()->GetnearestClamp());
    m_pLowTexPong = IRenderer::GetRendererInstance()->addRenderTarget(WATER_GRID_RESOLUTION, WATER_GRID_RESOLUTION, FORMAT_RGBA32F, IRenderer::GetRendererInstance()->GetnearestClamp());
    m_pHighTexPing = IRenderer::GetRendererInstance()->addRenderTarget(HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, FORMAT_RGBA32F, IRenderer::GetRendererInstance()->GetnearestClamp());
    m_pHighTexPong = IRenderer::GetRendererInstance()->addRenderTarget(HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, FORMAT_RGBA32F, IRenderer::GetRendererInstance()->GetnearestClamp());


    // Create the floating point frequency domain and spatial domain surfaces
    m_pLowFreqWaterTex = IRenderer::GetRendererInstance()->addRenderTarget(WATER_GRID_RESOLUTION, WATER_GRID_RESOLUTION, FORMAT_RGBA32F, IRenderer::GetRendererInstance()->GetnearestClamp());
    m_pHighFreqWaterTex = IRenderer::GetRendererInstance()->addRenderTarget(HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, FORMAT_RGBA32F, IRenderer::GetRendererInstance()->GetnearestClamp());
    m_pHighDisplacementMapTex = IRenderer::GetRendererInstance()->addRenderTarget(HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, FORMAT_R32F, IRenderer::GetRendererInstance()->GetnearestClamp());


    // Get the surfaces for subsequent rendering
    // Now for water wave synthesis utility maps
    m_pLowAmplitudeTTex = IRenderer::GetRendererInstance()->addRenderTarget(WATER_GRID_RESOLUTION, WATER_GRID_RESOLUTION, FORMAT_RGBA32F, IRenderer::GetRendererInstance()->GetnearestClamp());
    m_pHighAmplitudeTTex = IRenderer::GetRendererInstance()->addRenderTarget(HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, FORMAT_RGBA32F, IRenderer::GetRendererInstance()->GetnearestClamp());


    // Get the surfaces for subsequent rendering
    // water normal map
    m_pHighNormalTex = IRenderer::GetRendererInstance()->addRenderTarget(HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, FORMAT_RGBA8, IRenderer::GetRendererInstance()->GetnearestClamp());
    //m_pHighNormalTex->SetAutoGenFilterType(D3DTEXF_LINEAR);


    // Scalar output of GPU IFFT
    m_pDisplacementMapTex = IRenderer::GetRendererInstance()->addRenderTarget(WATER_GEOM_RESOLUTION, WATER_GEOM_RESOLUTION, FORMAT_RGBA32F, IRenderer::GetRendererInstance()->GetnearestClamp());


    // initialize the vertex buffers for the off screen render targets
    FillScreenAlignedQuads();
    

}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
GPU_Physics::~GPU_Physics()
{
#if 0
    // shaders
    IRenderer::GetRendererInstance()->deleteShader(m_HorizontalButterflyShader);
    IRenderer::GetRendererInstance()->deleteShader(m_VerticalButterflyShader);
    IRenderer::GetRendererInstance()->deleteShader(m_HorizontalScrambleShader);
    IRenderer::GetRendererInstance()->deleteShader(m_VerticalScrambleShader);
    IRenderer::GetRendererInstance()->deleteShader(m_ExpandShader);
    IRenderer::GetRendererInstance()->deleteShader(m_CopyShader);
    IRenderer::GetRendererInstance()->deleteShader(m_WaterSpectrumShader);
    IRenderer::GetRendererInstance()->deleteShader(m_ComputeNormalsShader);

    // textures
    ////////////SAFE_RELEASE(m_pLowWTex);
    ////////////SAFE_RELEASE(m_pLowAmplitude0Tex);
    ////////////SAFE_RELEASE(m_pHighWTex);
    ////////////SAFE_RELEASE(m_pHighAmplitude0Tex);
    ////////////SAFE_RELEASE(m_pLowScrambleMap);
    ////////////SAFE_RELEASE(m_pLowFFTButterflyMap);
    ////////////SAFE_RELEASE(m_pLowIFFTButterflyMap);
    ////////////SAFE_RELEASE(m_pHighScrambleMap);
    ////////////SAFE_RELEASE(m_pHighFFTButterflyMap);
    ////////////SAFE_RELEASE(m_pHighIFFTButterflyMap);
    ////////////SAFE_RELEASE(m_pLowTexPing);
    ////////////SAFE_RELEASE(m_pLowTexPong);
    ////////////SAFE_RELEASE(m_pHighTexPing);
    ////////////SAFE_RELEASE(m_pHighTexPong);
    ////////////SAFE_RELEASE(m_pLowFreqWaterTex);
    ////////////SAFE_RELEASE(m_pHighFreqWaterTex);
    ////////////SAFE_RELEASE(m_pHighDisplacementMapTex);
    ////////////SAFE_RELEASE(m_pLowAmplitudeTTex);
    ////////////SAFE_RELEASE(m_pHighAmplitudeTTex);
    if(m_pHighNormalTex)
        m_pHighNormalTex->Release();
    ////////////SAFE_RELEASE(m_pHighNormalTex);
    ////////////SAFE_RELEASE(m_pDisplacementMapTex);

     // surfaces
    ////////////SAFE_RELEASE(m_pLowSurfPing);
    ////////////SAFE_RELEASE(m_pLowSurfPong);
    ////////////SAFE_RELEASE(m_pHighSurfPing);
    ////////////SAFE_RELEASE(m_pHighSurfPong);
    ////////////SAFE_RELEASE(m_pLowFreqWaterSurf);
    ////////////SAFE_RELEASE(m_pHighFreqWaterSurf);
    ////////////SAFE_RELEASE(m_pHighSpatialWaterSurf);
    ////////////SAFE_RELEASE(m_pLowAmplitudeTSurf);
    ////////////SAFE_RELEASE(m_pHighAmplitudeTSurf);
    ////////////SAFE_RELEASE(m_pLowSpatialWaterSurf);

    // vertex buffers
    ////////////SAFE_RELEASE(m_pLowQuadVB);
    ////////////SAFE_RELEASE(m_pHighQuadVB);
#endif
}



//-----------------------------------------------------------------------------
//
// FillFFTUtilityTextures
//
// Initialize the textures that will be used in the FFT algorithm on the GPU
//
//-----------------------------------------------------------------------------
int GPU_Physics::FillFFTUtilityTextures(int iResolution, int iLog2Resolution, 
                                            TextureID&  ppScrambleMap, 
                                            TextureID&  ppFFTButterflyMap, 
                                            TextureID&  ppIFFTButterflyMap)
{
    int hr;

	Image3 img, fimg, wimg;
	{
    float *fImage = new float[iResolution];
    __WORD__  *wImage = new __WORD__[iResolution];
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

    // Convert to 16-bit __WORD__ texels
    for (mm=0; mm < iResolution; mm++)
    {
        wImage[mm] = (__WORD__)(65535.0f*fImage[mm]);
    }

    // Create a single channel texture and load in this scramble map
	BYTE* pBits=img.create(FORMAT_RG8, iResolution, 1, 1, 0);
    // ??? stx_memcpy(pBits, wImage, iResolution * sizeof(__WORD__));
	ppScrambleMap=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());

    // Delete the scratch scramble data
    delete []fImage;
    delete []wImage;}

    // Create both Butterfly textures (16 rows just to be safe/flexible)
    float* fImage = (float*)fimg.create(FORMAT_RGBA32f, iResolution, 16, 1, 1 /* ??? 0 */);
    float* wImage = (float*)wimg.create(FORMAT_RGBA32f, iResolution, 16, 1, 1 /* ??? 0 */);

    float angle;
    float wre, wim, wpre, wpim;
    int ii, jj, gg, N, half_N, offset;
    float wtemp;

    //
    // Forward FFT Butterfly map
    //
    //fImage = new float[iResolution * 16 * 4];
    // ??? stx_memset( fImage, 0, iResolution * 16 * 4 * sizeof(float));

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

    //delete []fImage;

    //
    // Inverse FFT Butterfly map
    //
    //fImage = new float[iResolution*16*4];
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

    //delete []fImage;
	ppFFTButterflyMap=IRenderer::GetRendererInstance()->addTexture(&wimg, false, IRenderer::GetRendererInstance()->Getlinear());
	ppIFFTButterflyMap=IRenderer::GetRendererInstance()->addTexture(&fimg, false, IRenderer::GetRendererInstance()->Getlinear());

    return 0;
}

//-----------------------------------------------------------------------------
//
// FillScreenAlignedQuads()
//
// Fill the vertex buffer for the screen-aligned quad used by FFT
//-----------------------------------------------------------------------------
int GPU_Physics::FillScreenAlignedQuads()
{
    int hr = 0;

    float fTexelSize = 0.5f / ((float)WATER_GRID_RESOLUTION);

    //
    // Lock and fill the vertex buffer
    //
    QUAD_VERTEX* pQV=(QUAD_VERTEX*)IRenderer::GetRendererInstance()->LockVertexBuffer(m_pLowQuadVB);

        pQV[0].p  = D3DXFROMWINEVECTOR3(-1.0f, -1.0f, 0.5f);
        pQV[0].uv = D3DXFROMWINEVECTOR2(fTexelSize, 1.0f+fTexelSize);

        pQV[1].p  = D3DXFROMWINEVECTOR3(1.0f, -1.0f, 0.5f);
        pQV[1].uv = D3DXFROMWINEVECTOR2(1.0f+fTexelSize, 1.0f+fTexelSize);

        pQV[2].p  = D3DXFROMWINEVECTOR3(-1.0f, 1.0f, 0.5f);
        pQV[2].uv = D3DXFROMWINEVECTOR2(fTexelSize, fTexelSize);

        pQV[3].p  = D3DXFROMWINEVECTOR3(1.0f, 1.0f, 0.5f);
        pQV[3].uv = D3DXFROMWINEVECTOR2(1.0f+fTexelSize, fTexelSize);

    IRenderer::GetRendererInstance()->UnLockVertexBuffer(m_pLowQuadVB);


    fTexelSize = 0.5f / ((float)HIGH_FREQ_WATER_GRID_RESOLUTION);

    // Lock and fill the vertex buffer
    pQV=(QUAD_VERTEX*)IRenderer::GetRendererInstance()->LockVertexBuffer(m_pHighQuadVB);

        pQV[0].p  = D3DXFROMWINEVECTOR3(-1.0f, -1.0f, 0.5f);
        pQV[0].uv = D3DXFROMWINEVECTOR2(fTexelSize, 1.0f+fTexelSize);

        pQV[1].p  = D3DXFROMWINEVECTOR3(1.0f, -1.0f, 0.5f);
        pQV[1].uv = D3DXFROMWINEVECTOR2(1.0f+fTexelSize, 1.0f+fTexelSize);

        pQV[2].p  = D3DXFROMWINEVECTOR3(-1.0f, 1.0f, 0.5f);
        pQV[2].uv = D3DXFROMWINEVECTOR2(fTexelSize, fTexelSize);

        pQV[3].p  = D3DXFROMWINEVECTOR3(1.0f, 1.0f, 0.5f);
        pQV[3].uv = D3DXFROMWINEVECTOR2(1.0f+fTexelSize, fTexelSize);

    IRenderer::GetRendererInstance()->UnLockVertexBuffer(m_pHighQuadVB);


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
int GPU_Physics::GenerateFourierSpectrum(SurfaceID pOutputSurf, float fTime, int iResolution)
{
    int hr;
    SurfaceID      pMainBackBufferSurf;
    SurfaceID      pMainZBufferSurf;
    TextureID      pWTex;
    TextureID      pAmplitude0Tex;
    VertexFormatID pQuadVF;
    VertexBufferID pQuadVB;
    D3DFROMWINEVIEWPORT9*           pQuadViewport;

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

#if 0
    // Save these off to be restored later
    ( IRenderer::GetRendererInstance()->GetRenderTarget(0, &pMainBackBufferSurf));
    ( IRenderer::GetRendererInstance()->changeRenderTarget(pOutputSurf));
    ( IRenderer::GetRendererInstance()->GetDepthStencilSurface(&pMainZBufferSurf));
    ( IRenderer::GetRendererInstance()->SetDepthStencilSurface(0));
#endif
    ( IRenderer::GetRendererInstance()->setViewport(pQuadViewport) );
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, pQuadVB);
    ( IRenderer::GetRendererInstance()->setVertexFormat( pQuadVF ) );

    // Evaluate frequency spectrum at fTime
    IRenderer::GetRendererInstance()->setShader(m_WaterSpectrumShader);
    //int index = IRenderer::GetRendererInstance()->getSamplerIndex(m_WaterSpectrumShader, "freqSamp");
#if 0
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
#endif
    (IRenderer::GetRendererInstance()->setTexture("freqSamp", pWTex));
    //index = IRenderer::GetRendererInstance()->getSamplerIndex(m_WaterSpectrumShader, "amplitudeSamp");
#if 0
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
#endif
    (IRenderer::GetRendererInstance()->setTexture("amplitudeSamp", pAmplitude0Tex));
    IRenderer::GetRendererInstance()->setShaderConstant1f("g_fTime", fTime);
#if 0
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS));
#endif
    // ??? ( IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2) );

    // Subsequent rendering as before
  	IRenderer::GetRendererInstance()->changeToMainFramebuffer();

    return 0;
}


//--------------------------------------------------------------------------------------
// First argument points to WATER_GRID_RESOLUTION x WATER_GRID_RESOLUTION input texture
// Second argument points to WATER_GRID_RESOLUTION x WATER_GRID_RESOLUTION output surface
//--------------------------------------------------------------------------------------
int GPU_Physics::ComputeWaterNormals(TextureID pInputTex,
                                         SurfaceID pOutputSurf, int iResolution)
{
    int hr;
    SurfaceID      pMainBackBufferSurf;
    SurfaceID      pMainZBufferSurf;
    VertexFormatID pQuadVF;
    VertexBufferID pQuadVB;
    D3DFROMWINEVIEWPORT9*           pQuadViewport;

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

    assert(IRenderer::GetRendererInstance() != 0);

    // Save these off to be restored later
#if 0
    ( IRenderer::GetRendererInstance()->GetRenderTarget(0, &pMainBackBufferSurf));
    ( IRenderer::GetRendererInstance()->changeRenderTarget(pOutputSurf));
    ( IRenderer::GetRendererInstance()->GetDepthStencilSurface(&pMainZBufferSurf));
    ( IRenderer::GetRendererInstance()->SetDepthStencilSurface(0));
#endif
    ( IRenderer::GetRendererInstance()->setViewport(pQuadViewport) );
    IRenderer::GetRendererInstance()->setVertexFormat( pQuadVF );
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, pQuadVB );
    //( IRenderer::GetRendererInstance()->SetFVF( QUAD_VERTEX::FVF ) );

    // Run Sobel filter over input map
    IRenderer::GetRendererInstance()->setShader(m_ComputeNormalsShader);
    IRenderer::GetRendererInstance()->setShaderConstant1f("g_fTexelSize",  1.0f/((float)iResolution));
    //int index = IRenderer::GetRendererInstance()->getSamplerIndex(m_ComputeNormalsShader, "amplitudeSamp");
#if 0
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
#endif
    (IRenderer::GetRendererInstance()->setTexture("amplitudeSamp", pInputTex));

    ( IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2) );

#if 0
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS));
#endif

    // Subsequent rendering as before
#if 0
    ( IRenderer::GetRendererInstance()->changeRenderTarget(pMainBackBufferSurf, pMainZBufferSurf));
    ( pMainBackBufferSurf->Release());
    ( IRenderer::GetRendererInstance()->SetDepthStencilSurface(pMainZBufferSurf));
    ( pMainZBufferSurf->Release());
#else
  	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
#endif

    return 0;
}


//--------------------------------------------------------------------------------------
// 
// SwapTextures
//
// Pointer swapping utility for FFT implementation below
//--------------------------------------------------------------------------------------
void SwapTextures(TextureID&  ppATex, TextureID&  ppBTex, 
                  SurfaceID&   ppASurf, SurfaceID&   ppBSurf)
{
    TextureID pTempTex;
    SurfaceID pTempSurf;

    // Check the pointers in debug
#if 0 // ???
    assert(ppATex > -1);
    assert(ppBTex > -1);
    assert(ppASurf > -1);
    assert(ppBSurf > -1);
#endif
    pTempTex  = ppATex;
    ppATex   = ppBTex;
    ppBTex   =  pTempTex;

    pTempSurf = ppASurf;
    ppASurf  = ppBSurf;
    ppBSurf  =  pTempSurf;
}


//--------------------------------------------------------------------------------------
//
// Optimized D3DXFROMWINE effect implementation of Cooley and Tukey's decimation in time.  See:
// http://www.ati.com/developer/ShaderX2_AdvancedImageProcessing.pdf for more details
//
// First argument points to grayscale input surface
// Second argument should be TRUE for FFT and FALSE for Inverse FFT
//
//--------------------------------------------------------------------------------------
int GPU_Physics::PerformFFT(TextureID pInputTex, SurfaceID pOutputSurf, BOOL bDoForwardFFT, 
                      int iResolution, int iLog2Resolution)
{
    int hr;
    int i;

    // Local pointers which will get hooked up according to resolution
    TextureID      pFFTButterflyMap;
    TextureID      pIFFTButterflyMap;
    TextureID      pButterflyMap;
    TextureID      pScrambleMap;
    TextureID      pDstTex;
    TextureID      pSrcTex;
    SurfaceID      pDstSurf;
    SurfaceID      pSrcSurf;
    SurfaceID      pMainBackBufferSurf;
    SurfaceID      pMainZBufferSurf;
    VertexFormatID pQuadVF;
    VertexBufferID pQuadVB;
    D3DFROMWINEVIEWPORT9*           pQuadViewport;

    // Check the pointers in debug
    assert(pInputTex != -1);
    assert(pOutputSurf != -1);

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
        LOG_PRINT("Bad resolution in PerformGPUFFT()\n");
    }


    // Save these off to be restored later
#if 0
    ( IRenderer::GetRendererInstance()->GetRenderTarget(0, &pMainBackBufferSurf));
    ( IRenderer::GetRendererInstance()->changeRenderTarget(pDstSurf));
    ( IRenderer::GetRendererInstance()->GetDepthStencilSurface(&pMainZBufferSurf));
    ( IRenderer::GetRendererInstance()->SetDepthStencilSurface(0));
#else
  	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
#endif

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
    ( IRenderer::GetRendererInstance()->setViewport(pQuadViewport) );
    ( IRenderer::GetRendererInstance()->setVertexBuffer( 0, pQuadVB ) );   // Simple quadrilateral VB
    //( IRenderer::GetRendererInstance()->SetFVF( QUAD_VERTEX::FVF ) );                              // Simple vertex format 
#if 0
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS));
    (IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE)); 
#endif

    // Do horizontal scramble
    IRenderer::GetRendererInstance()->setShader(m_HorizontalScrambleShader);
    int index = IRenderer::GetRendererInstance()->getSamplerIndex(m_HorizontalScrambleShader, "sampSourceImage"); 
#if 0
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP)); 
#endif
    (IRenderer::GetRendererInstance()->SetTexture(index, &pInputTex));
    index = IRenderer::GetRendererInstance()->getSamplerIndex(m_HorizontalScrambleShader, "sampScramble"); 
#if 0
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP)); 
#endif
    (IRenderer::GetRendererInstance()->SetTexture(index, &pScrambleMap));

    // ??? ( IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2) );

    // Swap the textures
    SwapTextures(pDstTex, pSrcTex, pDstSurf, pSrcSurf);
    ( IRenderer::GetRendererInstance()->changeRenderTarget(pDstSurf));

    // Do horizontal butterfly passes
    IRenderer::GetRendererInstance()->setShader(m_HorizontalButterflyShader);
    //index = IRenderer::GetRendererInstance()->getSamplerIndex(m_HorizontalButterflyShader, "sampSourceImage"); 
#if 0
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP)); 
#endif
    (IRenderer::GetRendererInstance()->setTexture("sampSourceImage", pSrcTex));
    //index = IRenderer::GetRendererInstance()->getSamplerIndex(m_HorizontalButterflyShader, "sampButterfly"); 
#if 0
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP)); 
#endif
    (IRenderer::GetRendererInstance()->setTexture("sampButterfly", pButterflyMap));

    // Perform log2(resolution) butterfly passes
    for (i=0; i < iLog2Resolution; i++)
    {
        float fPassNumber = ((float)i)/16.0f + 1.0f/32.0f; // Danger...this is a function of the size of the padded butterfly map
        IRenderer::GetRendererInstance()->setShaderConstant1f("fPassNum", fPassNumber);

        ( IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2) );

        SwapTextures(pDstTex, pSrcTex, pDstSurf, pSrcSurf);
        // ??? ( IRenderer::GetRendererInstance()->changeRenderTarget(pDstSurf));
        (setTexture(m_HorizontalButterflyShader, "sampSourceImage", pSrcTex));
    }

    // Do vertical scramble
    IRenderer::GetRendererInstance()->setShader(m_VerticalScrambleShader);
    index = IRenderer::GetRendererInstance()->getSamplerIndex(m_VerticalScrambleShader, "sampSourceImage"); 
#if 0
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP)); 
#endif
    (IRenderer::GetRendererInstance()->SetTexture(index, &pSrcTex));
    index = IRenderer::GetRendererInstance()->getSamplerIndex(m_VerticalScrambleShader, "sampScramble"); 
#if 0
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP)); 
#endif
    (IRenderer::GetRendererInstance()->SetTexture(index, &pScrambleMap));

    ( IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2) );

    // Swap the textures
    SwapTextures(pDstTex, pSrcTex, pDstSurf, pSrcSurf);
    ( IRenderer::GetRendererInstance()->changeRenderTarget(pDstSurf));

    // Do vertical butterfly passes
    IRenderer::GetRendererInstance()->setShader(m_VerticalButterflyShader);
    index = IRenderer::GetRendererInstance()->getSamplerIndex(m_VerticalButterflyShader, "sampSourceImage"); 
#if 0
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP)); 
#endif
    (IRenderer::GetRendererInstance()->SetTexture(index, &pSrcTex));
    index = IRenderer::GetRendererInstance()->getSamplerIndex(m_VerticalButterflyShader, "sampButterfly"); 
#if 0
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
    (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP)); 
#endif
    (IRenderer::GetRendererInstance()->SetTexture(index, &pButterflyMap));

    // Perform log2(resolution) butterfly passes
    for (i=0; i < iLog2Resolution; i++)
    {
        float fPassNumber = ((float)i)/16.0f + 1.0f/32.0f; // Danger...this is a function of the size of the padded butterfly map
        IRenderer::GetRendererInstance()->setShaderConstant1f("fPassNum", fPassNumber);

        ( IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2) );

        SwapTextures(pDstTex, pSrcTex, pDstSurf, pSrcSurf);
        ( IRenderer::GetRendererInstance()->changeRenderTarget(pDstSurf));
        (setTexture(m_VerticalButterflyShader, "sampSourceImage", pSrcTex));
    }

    //
    // Resolve whatever is in pSrcTex into output surface
    //

    if (iResolution == WATER_GRID_RESOLUTION)
    {
        ( IRenderer::GetRendererInstance()->changeRenderTarget(pOutputSurf));
        IRenderer::GetRendererInstance()->setShader(m_ExpandShader);
        index = IRenderer::GetRendererInstance()->getSamplerIndex(m_ExpandShader, "sampSourceImage"); 
#if 0
        (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
        (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP)); 
#endif
        (IRenderer::GetRendererInstance()->setTexture("sampSourceImage", pSrcTex));

        // Set the wave height and grid repeat rate
        IRenderer::GetRendererInstance()->setShaderConstant1f("g_heightScale", HEIGHT_SCALE);
        IRenderer::GetRendererInstance()->setShaderConstant1f("g_repeatRate", REPEAT_RATE);

        ( IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2) );
    }
    else
    {
        ( IRenderer::GetRendererInstance()->changeRenderTarget(pOutputSurf));
        IRenderer::GetRendererInstance()->setShader(m_CopyShader);
        //index = IRenderer::GetRendererInstance()->getSamplerIndex(m_CopyShader, "sampSourceImage"); 
#if 0
        (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
        (//IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP)); 
#endif
        (IRenderer::GetRendererInstance()->setTexture("sampSourceImage", pSrcTex));

        ( IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2) );
    }

    // Subsequent rendering as before
#if 0
    ( IRenderer::GetRendererInstance()->changeRenderTarget(pMainBackBufferSurf, pMainZBufferSurf));
    ( pMainBackBufferSurf->Release());
    ( IRenderer::GetRendererInstance()->SetDepthStencilSurface(pMainZBufferSurf));
    ( pMainZBufferSurf->Release());
#else
  	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
#endif
    return 0;
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
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("ProjectionMatrix", orthoMatrix(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, true));
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelViewMatrix", identity4());

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
    float *frequency;
    float *amp;

    // Lock and fill low frequency m_pLowWTex
    int Pitch=0;
    BYTE* pBits=IRenderer::GetRendererInstance()->LockTexture(m_pLowWTex, 0, Pitch);
    frequency = (float *) pBits;

    for (i = 0; i < WATER_GRID_RESOLUTION; i++)
    {
        for (j = 0; j < WATER_GRID_RESOLUTION; j++)
        {
            /// ??? *frequency = m_LowW[i][j];
            frequency++;
        }
    }
	IRenderer::GetRendererInstance()->UnlockTexture(m_pLowWTex, 0);

    // Lock and fill the high frequency m_pHighWTex
	pBits=IRenderer::GetRendererInstance()->LockTexture(m_pHighWTex, 0, Pitch);
    frequency = (float *) pBits;

    for (i = 0; i < HIGH_FREQ_WATER_GRID_RESOLUTION; i++)
    {
        for (j = 0; j < HIGH_FREQ_WATER_GRID_RESOLUTION; j++)
        {
            // ??? *frequency = m_HighW[i][j];
            frequency++;
        }
    }
IRenderer::GetRendererInstance()->UnlockTexture(m_pHighWTex, 0);

    // Lock and fill m_pLowAmplitude0Tex
	pBits=IRenderer::GetRendererInstance()->LockTexture(m_pLowAmplitude0Tex, 0, Pitch);
    amp = (float *) pBits;

    for (i = 0; i < WATER_GRID_RESOLUTION; i++)
    {
        for (j = 0; j < WATER_GRID_RESOLUTION; j++)
        {
        #if 0 // ???
            // These are in a weird order in memory because of the ABGR ordering of the float texture
            // See ps_water_spectrum() in WaterNormalMap.shd for the usage of this map
            *amp++ = c_im(m_LowAmplitude0[WATER_GRID_RESOLUTION-i][WATER_GRID_RESOLUTION-j]);
            *amp++ = c_re(m_LowAmplitude0[WATER_GRID_RESOLUTION-i][WATER_GRID_RESOLUTION-j]);
            *amp++ = c_im(m_LowAmplitude0[i][j]);
            *amp++ = c_re(m_LowAmplitude0[i][j]);
       #endif
        }
    }
IRenderer::GetRendererInstance()->UnlockTexture(m_pLowAmplitude0Tex, 0);

    // Lock and fill m_pHighAmplitude0Tex
pBits=IRenderer::GetRendererInstance()->LockTexture(m_pHighAmplitude0Tex, 0, Pitch);
    amp = (float *) pBits;

    for (i = 0; i < HIGH_FREQ_WATER_GRID_RESOLUTION; i++)
    {
        for (j = 0; j < HIGH_FREQ_WATER_GRID_RESOLUTION; j++)
        {
       #if 0
            // These are in a weird order in memory because of the ABGR ordering of the float texture
            // See ps_water_spectrum() in WaterNormalMap.shd for the usage of this map
            *amp++ = c_im(m_HighAmplitude0[HIGH_FREQ_WATER_GRID_RESOLUTION-i][HIGH_FREQ_WATER_GRID_RESOLUTION-j]);
            *amp++ = c_re(m_HighAmplitude0[HIGH_FREQ_WATER_GRID_RESOLUTION-i][HIGH_FREQ_WATER_GRID_RESOLUTION-j]);
            *amp++ = c_im(m_HighAmplitude0[i][j]);
            *amp++ = c_re(m_HighAmplitude0[i][j]);
       #endif
        }
    }
	IRenderer::GetRendererInstance()->UnlockTexture(m_pHighAmplitude0Tex, 0);
}

	
