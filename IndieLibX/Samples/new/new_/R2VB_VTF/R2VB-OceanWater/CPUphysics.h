/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: CPUphysics.h
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _CPU_PHYSICS_H_
#define _CPU_PHYSICS_H_

#include "physics.h"
#include "FFT.h"

//-----------------------------------------------------------------------------
// Class CPU_Physics
// 
// This class implements the physics for wave synthesis using the CPU.
//-----------------------------------------------------------------------------

class CPU_Physics : public Physics
{
public:
    CPU_Physics();
    ~CPU_Physics();
    void ComputeWaves(float windSpeed, float windDirection, float time);
    TextureID GetNormalMap() {return m_pHighNormalTex;}
    float* GetDisplacementMap() {return (float*)&m_LowSpatialWaterImage[0][0];}
private:
    void GenerateFFTWFourierSpectrum(float fTime);
    void ComputeWaterNormals();

    // Buffers for CPU Physics
    complex_number  m_LowFreqWaterImage[WATER_GRID_RESOLUTION][WATER_GRID_RESOLUTION];
    complex_number  m_HighFreqWaterImage[HIGH_FREQ_WATER_GRID_RESOLUTION][HIGH_FREQ_WATER_GRID_RESOLUTION];
    complex_number  m_LowSpatialWaterImage[WATER_GRID_RESOLUTION][WATER_GRID_RESOLUTION];
    complex_number  m_HighSpatialWaterImage[HIGH_FREQ_WATER_GRID_RESOLUTION][HIGH_FREQ_WATER_GRID_RESOLUTION];
    unsigned char   m_HighNormalImage[HIGH_FREQ_WATER_GRID_RESOLUTION][HIGH_FREQ_WATER_GRID_RESOLUTION][4];

    // FFTW plans
    fftwf_plan  m_LowFreqPlan;
    fftwf_plan  m_HighFreqPlan;

    TextureID   m_pHighNormalTex;       // local normal map
    TextureID   m_pSysMemHighNormalTex; // non-local normal map
};

#endif //_CPU_PHYSICS_H_
