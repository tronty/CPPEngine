/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

	//-----------------------------------------------------------------------------
// File: physics.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "physics.h"

#define WX 128.0f	// width of wave tile, in meters
#define WY 128.0f	// length of wave tile, in meters

// buffers for storing frequency and amplitude data
float            Physics::m_LowW[WATER_GRID_RESOLUTION+1][WATER_GRID_RESOLUTION+1];
complex_number   Physics::m_LowAmplitude0[WATER_GRID_RESOLUTION+1][WATER_GRID_RESOLUTION+1];
complex_number   Physics::m_LowAmplitude[WATER_GRID_RESOLUTION][WATER_GRID_RESOLUTION];

float            Physics::m_HighW[HIGH_FREQ_WATER_GRID_RESOLUTION+1][HIGH_FREQ_WATER_GRID_RESOLUTION+1];
complex_number   Physics::m_HighAmplitude0[HIGH_FREQ_WATER_GRID_RESOLUTION+1][HIGH_FREQ_WATER_GRID_RESOLUTION+1];
complex_number   Physics::m_HighAmplitude[HIGH_FREQ_WATER_GRID_RESOLUTION][HIGH_FREQ_WATER_GRID_RESOLUTION];

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Physics::Physics(){}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Physics::~Physics()
{
}


//-----------------------------------------------------------------------------
//
// The following few functions are based on code from http://www.fas.harvard.edu/~reichard/water
//
//-----------------------------------------------------------------------------
static float normalDistTable[][2] = {
    {.00f, .5f}, {.01f, .504f}, {.02f, .508f}, {.03f, .512f}, {.06f, .5239f}, {.08f, .5319f}, {.11f, .5438f}, {.13f, .5517f},
    {.16f, .5636f}, {.18f, .5714f}, {.21f, .5832f}, {.23f, .5910f}, {.26f, .6026f}, {.28f, .6103f}, {.31f, .6217f}, {.34f, .6331f},
    {.36f, .6406f}, {.39f, .6517f}, {.42f, .6628f}, {.44f, .6700f}, {.47f, .6808f}, {.50f, .6915f}, {.53f, .7019f}, {.56f, .7123f},
    {.59f, .7224f}, {.62f, .7324f}, {.65f, .7422f}, {.68f, .7517f}, {.71f, .7611f}, {.74f, .7703f}, {.78f, .7823f}, {.81f, .7910f},
    {.85f, .8023f}, {.88f, .8106f}, {.92f, .8212f}, {.96f, .8315f}, {1.0f, .8413f}, {1.04f, .8508f}, {1.09f, .8621f}, {1.13f, .8708f},
    {1.18f, .8810f}, {1.23f, .8907f}, {1.29f, .9015f}, {1.35f, .9115f}, {1.41f, .9207f}, {1.48f, .9306f}, {1.56f, .9406f},
    {1.65f, .9505f}, {1.76f, .9608f}, {1.89f, .9706f}, {2.06f, .9803f}, {2.33f, .9901f}, {99.0f, 1.0f}
};


//-----------------------------------------------------------------------------
//
// RandNormal
//
//-----------------------------------------------------------------------------

float RandNormal(float mean, float stdev)
{
    int i = 0;
    float u = stx_rand() / (float) RAND_MAX;
    float n;

    if (u >= 0.5)
    {
        while (u > normalDistTable[i][1])
        {
            i++;
        }
        n = normalDistTable[i-1][0];
    }
    else
    {
        u = 1 - u;
        while (u > normalDistTable[i][1])
        {
            i++;
        }
        n = 1 - normalDistTable[i-1][0];
    }

    //STX_PRINT("u: %f, n: %f, i: %d\n", u, n, i); //debug

    return (mean + stdev * n);
}


//-----------------------------------------------------------------------------
//
//  Phillips
//
//  We use the Philips spectrum to decide wave amplitudes at different (spatial or temporal) frequencies:
//  P_h(K) = A (exp(-1/sqrt(kL)^2)/k^4) |norm(K) dot norm(w)|^2
//  Here k = the magnitude of K (the wave vector) which is 2pi/lambda, where lambda is the length of the wave. 
//  A is a constant globally affecting wave height.  L = (v^2 / g), is the largest possible wave 
//  arising from a continuous wind with speed v, where g = 9.81, is the gravitational constant.  
//  norm(w) is the normalized wind vector (wind direction), and norm(K) is the normalized wave vector (wave direction).
//  The term |norm(K) dot norm(w)|^2 removes waves moving perpendicular to wind direction, but 
//  allows waves to move against the against the wind.  For rough, clashing waves, leave in the waves 
//  moving against the wind; otherwise, some subjective filtering is useful (e.g., scale waves going 
//  against the wind by a factor of 4). 
//
//  Some nice values for the constants: (all using WX = WY = 100)
//   v = 31
// 
//   w = 0 for rough, nicely-textured waves
//   w = 1 for smooth waves
// 
//   A = .00000375f with m_fWindVelocity = 9 and N = 128 for slower, calm seas
//   A = .00001     with N = 128 for calm seas
//   A = .0000075   with N = 128 gives beautiful, very calm seas
//   A = 0.000005    with N = 64, m_fWindVelocity = 9
//
//  Units are standard SI units.
//-----------------------------------------------------------------------------

float Phillips(float kx, float ky, float windSpeed, float windDirection)
{
    float fWindDir = windDirection * D3DXFROMWINE_PI / 180.0f;  // Convert wind direction from global degrees

    static float A = .000005f;  // constant affecting global height of waves

    float L = windSpeed * windSpeed / 9.81f;      // largest possible wave from constant wind of velocity 
    float w = L / 75;                             // damp out waves with very small length w << L

    float ksqr = kx * kx + ky * ky;
    float kdotwhat = (float) (kx * cos(fWindDir) + ky * sin(fWindDir));
    kdotwhat = max(0.0f, kdotwhat);
    float phillips = (float) (A * (pow(2.7183f, -1.0f / (L * L * ksqr)) * (kdotwhat * kdotwhat)) / (ksqr * ksqr * ksqr));  // Equation (23) in [Tessendorf99]
    float damp = (float) exp(-ksqr * w * w);                                                                               // Equation (24) in [Tessendorf99]

    if (kdotwhat < 0)
    {
        phillips *= 0.25f;  // filter out waves moving opposite to wind
    }

    return (phillips * damp);
}


//-----------------------------------------------------------------------------
//
// FillWaterUtillityBuffers
//
// Compute CPU-side buffers for FFTW and call routine which uploads them to GPU as well
//
//-----------------------------------------------------------------------------

void Physics::FillWaterUtilityBuffers(float windSpeed, float windDirection)
{
    int i, j;
    float kx, ky;
    float fMultiplier, fAmplitude, fTheta;

    // Check to see if this is the first time through this code
    static bool bFirstTimeThrough = TRUE;

    //
    // Fill up the low frequency arrays which will get copied into textures later
    //
    for (i = 0; i <= WATER_GRID_RESOLUTION; i++)
    {
        kx = (-((float)WATER_GRID_RESOLUTION) / 2.0f + ((float)i)) * (2.0f * D3DXFROMWINE_PI / WX);

        for (j = 0; j <= WATER_GRID_RESOLUTION; j++)
        {
            ky = (-((float)WATER_GRID_RESOLUTION) / 2.0f + ((float)j)) * (2.0f * D3DXFROMWINE_PI / WY);

            if (bFirstTimeThrough)
            {
                m_LowW[i][j] = sqrt(9.81f * sqrt(kx * kx + ky * ky));  // set angular frequency w = sqrt{gk}
            }

            if ((kx == 0.0f) && (ky == 0.0f))
                fMultiplier = 0.0f;
            else
                fMultiplier = sqrt(Phillips(kx, ky, windSpeed, windDirection));

            fAmplitude = RandNormal(0.0f, 1.0f);
            fTheta = stx_rand() / (float) RAND_MAX * 2 * D3DXFROMWINE_PI;

            c_re(m_LowAmplitude0[i][j]) = fMultiplier * fAmplitude * sin(fTheta);
            c_im(m_LowAmplitude0[i][j]) = fMultiplier * fAmplitude * sin(fTheta);
        }
    }


    //
    // Fill up the high frequency arrays which will get copied into textures later
    //
    for (i = 0; i <= HIGH_FREQ_WATER_GRID_RESOLUTION; i++)
    {
        kx = (-((float)HIGH_FREQ_WATER_GRID_RESOLUTION) / 2.0f + ((float)i)) * (2.0f * D3DXFROMWINE_PI / WX);

        for (j = 0; j <= HIGH_FREQ_WATER_GRID_RESOLUTION; j++)
        {
            ky = (-((float)HIGH_FREQ_WATER_GRID_RESOLUTION) / 2.0f + ((float)j)) * (2.0f * D3DXFROMWINE_PI / WY);

            if (bFirstTimeThrough)
            {
                m_HighW[i][j] = sqrt(9.81f * sqrt(kx * kx + ky * ky));  // set angular frequency w = sqrt{gk}
            }

            // 
            //  With the high frequency buffer at a higher the rez than
            //  the low frequency buffer, we have something like
            //  the following scenario, with the numbered areas in the high
            //  frequency buffer which need to match the low frequency buffer
            //
            //  +--------+--------+--------+--------+
            //  |        |                 |        |
            //  |  (1)   |                 |  (2)   |
            //  |        |                 |        |
            //  +--------+                 +--------+
            //  |                                   |
            //  |                                   |
            //  |                                   |
            //  +                                   +
            //  |                                   |
            //  |                                   |
            //  |                                   |
            //  +--------+                 +--------+
            //  |        |                 |        |
            //  |  (3)   |                 |  (4)   |
            //  |        |                 |        |
            //  +--------+--------+--------+--------+
            //
            //

            // int to make following code more readable / compact
            int h = WATER_GRID_RESOLUTION / 2;

            if ((i < h) && (j < h))                                              // Case (1) above
            {
                c_re(m_HighAmplitude0[i][j]) = c_re(m_HighAmplitude0[i][j]);
                c_im(m_HighAmplitude0[i][j]) = c_im(m_HighAmplitude0[i][j]);
            }
            else if ( (i > (HIGH_FREQ_WATER_GRID_RESOLUTION - h)) && (j < h))    // Case (2) above
            {
                c_re(m_HighAmplitude0[i][j]) = c_re(m_HighAmplitude0[i - (HIGH_FREQ_WATER_GRID_RESOLUTION - 2*h)][j]);
                c_im(m_HighAmplitude0[i][j]) = c_im(m_HighAmplitude0[i - (HIGH_FREQ_WATER_GRID_RESOLUTION - 2*h)][j]);
            }
            else if ((i < h) && (j > (HIGH_FREQ_WATER_GRID_RESOLUTION - h)))      // Case (3) above
            {
                c_re(m_HighAmplitude0[i][j]) = c_re(m_HighAmplitude0[i][j - (HIGH_FREQ_WATER_GRID_RESOLUTION - 2*h)]);
                c_im(m_HighAmplitude0[i][j]) = c_im(m_HighAmplitude0[i][j - (HIGH_FREQ_WATER_GRID_RESOLUTION - 2*h)]);
            }
            else if ((i > (HIGH_FREQ_WATER_GRID_RESOLUTION - h)) &&
                (j > (HIGH_FREQ_WATER_GRID_RESOLUTION - h)))                // Case (4) above
            {
                c_re(m_HighAmplitude0[i][j]) = c_re(m_HighAmplitude0[i - (HIGH_FREQ_WATER_GRID_RESOLUTION - 2*h)][j - (HIGH_FREQ_WATER_GRID_RESOLUTION - 2*h)]);
                c_im(m_HighAmplitude0[i][j]) = c_im(m_HighAmplitude0[i - (HIGH_FREQ_WATER_GRID_RESOLUTION - 2*h)][j - (HIGH_FREQ_WATER_GRID_RESOLUTION - 2*h)]);
            }
            else
            {
                if ((kx == 0.0f) && (ky == 0.0f))
                    fMultiplier = 0.0f;
                else
                    fMultiplier = sqrt(Phillips(kx, ky, windSpeed, windDirection));

                fAmplitude = RandNormal(0.0f, 1.0f);
                fTheta = stx_rand() / (float) RAND_MAX * 2 * D3DXFROMWINE_PI;

                c_re(m_HighAmplitude0[i][j]) = fMultiplier * fAmplitude * sin(fTheta);
                c_im(m_HighAmplitude0[i][j]) = fMultiplier * fAmplitude * sin(fTheta);
            }
        }
    }
}

