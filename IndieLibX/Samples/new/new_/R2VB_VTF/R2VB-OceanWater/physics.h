/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: physics.h
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include <Framework3/IRenderer.h>
#include "common.h"

//-----------------------------------------------------------------------------
// Class Physics
// Physics is a base class responsible for the ocean wave synthesis.
//-----------------------------------------------------------------------------

class Physics
{
public:
    Physics();
    virtual ~Physics();
    virtual void ComputeWaves(float windSpeed, float windDirection, float time) = 0;
    virtual float* GetNormalMap() = 0;
    virtual float* GetDisplacementMap() = 0;
    virtual void FillWaterUtilityBuffers(float windSpeed, float windDirection);
protected:
    

    // System-memory scratch buffers, note that some are larger than the water grid by one in each dimension
    static float            m_LowW[WATER_GRID_RESOLUTION+1][WATER_GRID_RESOLUTION+1];
    static complex_number   m_LowAmplitude0[WATER_GRID_RESOLUTION+1][WATER_GRID_RESOLUTION+1];
    static complex_number   m_LowAmplitude[WATER_GRID_RESOLUTION][WATER_GRID_RESOLUTION];

    static float            m_HighW[HIGH_FREQ_WATER_GRID_RESOLUTION+1][HIGH_FREQ_WATER_GRID_RESOLUTION+1];
    static complex_number   m_HighAmplitude0[HIGH_FREQ_WATER_GRID_RESOLUTION+1][HIGH_FREQ_WATER_GRID_RESOLUTION+1];
    static complex_number   m_HighAmplitude[HIGH_FREQ_WATER_GRID_RESOLUTION][HIGH_FREQ_WATER_GRID_RESOLUTION];
};


#endif //_PHYSICS_H_

