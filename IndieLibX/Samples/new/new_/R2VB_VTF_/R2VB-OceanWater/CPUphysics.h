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
    CPU_Physics(IDirect3DDevice9 *d3dDevice, Renderer *renderer);
    ~CPU_Physics();
    void ComputeWaves(float windSpeed, float windDirection, float time);
    IDirect3DTexture9 *GetNormalMap() {return m_pHighNormalTex;}
    void *GetDisplacementMap() {return m_LowSpatialWaterImage;}
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

    IDirect3DTexture9*  m_pHighNormalTex;       // local normal map
    IDirect3DTexture9*  m_pSysMemHighNormalTex; // non-local normal map
};

#endif //_CPU_PHYSICS_H_
