/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// File: CPUphysics.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "CPUphysics.h"

float* CPU_Physics::GetNormalMap()
    {
	int pitch=0;
        float* p=(float*)IRenderer::GetRendererInstance()->lockTexture(m_pHighNormalTex, 0, pitch);
        //IRenderer::GetRendererInstance()->unlockTexture(m_pHighNormalTex, 0);
    	return p;
    } // ??? Lock/Unlock/Image3::getPixels
float* CPU_Physics::GetDisplacementMap()
    {
	int pitch=0;
        float* p=(float*)IRenderer::GetRendererInstance()->lockTexture(m_pHighNormalTex, 0, pitch);
        //IRenderer::GetRendererInstance()->unlockTexture(m_pHighNormalTex, 0);
    	return p;
    } // ??? Lock/Unlock/Image3::getPixels

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CPU_Physics::CPU_Physics() : Physics()
{
    int hr;
	Image3 tex;
	// water normal map local texture
    	tex.create(FORMAT_RGBA8, HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, 1, 0);
	m_pHighNormalTex = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
    	//m_pHighNormalTex->SetAutoGenFilterType(D3DTEXF_LINEAR);
	// water normal map non-local texture
    	tex.create(FORMAT_RGBA8, HIGH_FREQ_WATER_GRID_RESOLUTION, HIGH_FREQ_WATER_GRID_RESOLUTION, 1, 0);
	m_pSysMemHighNormalTex = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());

    // create the FFTW plans
    m_LowFreqPlan = ifft_plan(WATER_GRID_RESOLUTION, (fftwf_complex *)m_LowFreqWaterImage, 
                            (fftwf_complex *)m_LowSpatialWaterImage);

    m_HighFreqPlan = ifft_plan(HIGH_FREQ_WATER_GRID_RESOLUTION, (fftwf_complex *)m_HighFreqWaterImage,
                            (fftwf_complex *)m_HighSpatialWaterImage);
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CPU_Physics::~CPU_Physics()
{
    //////////////SAFE_RELEASE(m_pHighNormalTex);
    //////////////SAFE_RELEASE(m_pSysMemHighNormalTex);

    if (m_LowFreqPlan)
    {
        destroy_plan(m_LowFreqPlan);
        m_LowFreqPlan = 0;
    }
    if (m_HighFreqPlan)
    {
        destroy_plan(m_HighFreqPlan);
        m_HighFreqPlan = 0;
    }
}


//--------------------------------------------------------------------------------------
// GenerateFFTWFourierSpectrum
// Generates both low frequency and high frequency images
//--------------------------------------------------------------------------------------
void CPU_Physics::GenerateFFTWFourierSpectrum(float fTime)
{
    int i, j;
    int mi, mj;                // indices corresponding to -K
    complex_number plus, minus;

   //
   // Low Frequency
   //
   for (i = 0; i < WATER_GRID_RESOLUTION; i++)
   {
      mi = WATER_GRID_RESOLUTION - i;

      for (j = 0; j < WATER_GRID_RESOLUTION; j++)
      {
         mj = WATER_GRID_RESOLUTION - j;
         c_re(plus)  = (float) (c_re(m_LowAmplitude0[i][j])   * cosf( m_LowW[i][j] * fTime) -   c_im(m_LowAmplitude0[i][j])    * sinf( m_LowW[i][j] * fTime));
         c_im(plus)  = (float) (c_re(m_LowAmplitude0[i][j])   * sinf( m_LowW[i][j] * fTime) +   c_im(m_LowAmplitude0[i][j])    * cosf( m_LowW[i][j] * fTime));
         c_re(minus) = (float) (c_re(m_LowAmplitude0[mi][mj]) * cosf(-m_LowW[i][j] * fTime) - (-c_im(m_LowAmplitude0[mi][mj])) * sinf(-m_LowW[i][j] * fTime));
         c_im(minus) = (float) (c_re(m_LowAmplitude0[mi][mj]) * sinf(-m_LowW[i][j] * fTime) + (-c_im(m_LowAmplitude0[mi][mj])) * cosf(-m_LowW[i][j] * fTime));

         c_re(m_LowFreqWaterImage[i][j]) = c_re(plus) + c_re(minus);
         c_im(m_LowFreqWaterImage[i][j]) = c_im(plus) + c_im(minus);
      }
   }

   //
   // High frequency
   //
   for (i = 0; i < HIGH_FREQ_WATER_GRID_RESOLUTION; i++)
   {
      mi = HIGH_FREQ_WATER_GRID_RESOLUTION - i;

      for (j = 0; j < HIGH_FREQ_WATER_GRID_RESOLUTION; j++)
      {
         mj = HIGH_FREQ_WATER_GRID_RESOLUTION - j;
         c_re(plus)  = (float) (c_re(m_HighAmplitude0[i][j])   * cosf( m_HighW[i][j] * fTime) -   c_im(m_HighAmplitude0[i][j])    * sinf( m_HighW[i][j] * fTime));
         c_im(plus)  = (float) (c_re(m_HighAmplitude0[i][j])   * sinf( m_HighW[i][j] * fTime) +   c_im(m_HighAmplitude0[i][j])    * cosf( m_HighW[i][j] * fTime));
         c_re(minus) = (float) (c_re(m_HighAmplitude0[mi][mj]) * cosf(-m_HighW[i][j] * fTime) - (-c_im(m_HighAmplitude0[mi][mj])) * sinf(-m_HighW[i][j] * fTime));
         c_im(minus) = (float) (c_re(m_HighAmplitude0[mi][mj]) * sinf(-m_HighW[i][j] * fTime) + (-c_im(m_HighAmplitude0[mi][mj])) * cosf(-m_HighW[i][j] * fTime));

         c_re(m_HighFreqWaterImage[i][j]) = c_re(plus) + c_re(minus);
         c_im(m_HighFreqWaterImage[i][j]) = c_im(plus) + c_im(minus);
      }
   }
}

//--------------------------------------------------------------------------------------
// ComputeWaterNormals
// CPU-only version of creating a normal map from a height field
//--------------------------------------------------------------------------------------
void CPU_Physics::ComputeWaterNormals()
{
    // Run the Sobel filter over the height map to generate the normals

    int i, j, left, right, top, bottom;
    float tl, t, tr, r, br, b, bl, l;
    float N[3];

    for (i = 0; i < HIGH_FREQ_WATER_GRID_RESOLUTION; i++)
    {
        // wrap the coordinates if necessary
        top = i - 1;
        if (top < 0)
            top = HIGH_FREQ_WATER_GRID_RESOLUTION - 1;
        bottom = i + 1;
        if (bottom >= HIGH_FREQ_WATER_GRID_RESOLUTION)
            bottom = 0;
        for (j = 0; j < HIGH_FREQ_WATER_GRID_RESOLUTION; j++)
        {
            // wrap
            left = j - 1;
            if (left < 0)
                left = HIGH_FREQ_WATER_GRID_RESOLUTION - 1;
            right = j + 1;
            if (right >= HIGH_FREQ_WATER_GRID_RESOLUTION)
                right = 0;

            float fDamp = 0.4f;

            // sample the taps
            tl =    fDamp * fabs(c_re(m_HighSpatialWaterImage[top][left]));
            t =     fDamp * fabs(c_re(m_HighSpatialWaterImage[top][j]));
            tr =    fDamp * fabs(c_re(m_HighSpatialWaterImage[top][right]));
            r =     fDamp * fabs(c_re(m_HighSpatialWaterImage[i][right]));
            br =    fDamp * fabs(c_re(m_HighSpatialWaterImage[bottom][right]));
            b =     fDamp * fabs(c_re(m_HighSpatialWaterImage[bottom][j]));
            bl =    fDamp * fabs(c_re(m_HighSpatialWaterImage[bottom][left]));
            l =     fDamp * fabs(c_re(m_HighSpatialWaterImage[i][left]));

            // Compute dx using Sobel:
            //
            //           -1 0 1 
            //           -2 0 2
            //           -1 0 1
            float dX = -tl - 2.0f*l - bl + tr + 2.0f*r + br;

            // Compute dy using Sobel:
            //
            //           -1 -2 -1 
            //            0  0  0
            //            1  2  1
            float dY = -tl - 2.0f*t - tr + bl + 2.0f*b + br;

            // Compute cross-product and renormalize (flip dX for correct handedness)
            float magnitude = sqrt((dX * dX) + (dY * dY) + 1);
            N[0] = -dX / magnitude;
            N[1] = 1.0f / magnitude;
            N[2] = dY / magnitude;

            // Bias signed values from -1..1 to 0..1 range
            m_HighNormalImage[i][j][0] =  (unsigned char)((N[0] * 0.5f + 0.5f) * 255.0f);
            m_HighNormalImage[i][j][1] =  (unsigned char)((N[1] * 0.5f + 0.5f) * 255.0f);
            m_HighNormalImage[i][j][2] =  (unsigned char)((N[2] * 0.5f + 0.5f) * 255.0f);
            m_HighNormalImage[i][j][3] =  (unsigned char)(1.0f * 255.0f);
       }
    }
}

//--------------------------------------------------------------------------------------
//
//  ComputeWaves
//
//  Generates waves in the frequency domain and uses an inverse FFT to store them
//  in a height field. Also calls ComputeWaterNormals to create a normal map from
//  the height map.
//--------------------------------------------------------------------------------------
void CPU_Physics::ComputeWaves(float windSpeed, float windDirection, float time)
{
    // Generate spectra for both high and low frequencies
    GenerateFFTWFourierSpectrum(time);

    // Perform CPU-side IFFTs on high and low frequency water textures (from frequency domain to spatial domain)
    fft_execute(m_LowFreqPlan);
    fft_execute(m_HighFreqPlan);

    // Compute normals of spatial water surfaces and use the hardware to mip-map them
    ComputeWaterNormals();

    // Update the normal map
    int Pitch=0;
    BYTE* pBits=IRenderer::GetRendererInstance()->LockTexture(m_pSysMemHighNormalTex, 0, Pitch);
    stx_memcpy(pBits, m_HighNormalImage,
        HIGH_FREQ_WATER_GRID_RESOLUTION * HIGH_FREQ_WATER_GRID_RESOLUTION * 4);
    IRenderer::GetRendererInstance()->UnlockTexture(m_pSysMemHighNormalTex, 0);
    //IRenderer::GetRendererInstance()->UpdateTexture(m_pSysMemHighNormalTex, m_pHighNormalTex);
}
	
