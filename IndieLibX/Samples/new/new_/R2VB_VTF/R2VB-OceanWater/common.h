/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//-----------------------------------------------------------------------------
// File: Common.h
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _COMMON_H_
#define _COMMON_H_

#include <assert.h>

// Common Defines
// ---------------

// Sizes for low (geometry) and high (normal map) frequency water grids
#define REPEAT_RATE                         3
#define WATER_GRID_RESOLUTION              64
#define HEIGHT_SCALE                        0.02f
#define WATER_GEOM_RESOLUTION              (WATER_GRID_RESOLUTION * REPEAT_RATE)
#define LOG2_GRID_RESOLUTION                6
#define HIGH_FREQ_WATER_GRID_RESOLUTION     512
#define LOG2_HIGH_FREQ_GRID_RESOLUTION      9
#define NUM_TILES                           9

// Some macros for accessing real and imaginary parts of complex_number numbers
#define c_re(c) ((c)[0])
#define c_im(c) ((c)[1])

// Solid color of light scattering up out of ocean water
#define WATER_COLOR              0xFF1D2A33

#if 0
// Macros for error checking
#if defined(DEBUG) | defined(_DEBUG)
    #define (x)           { hr = x; if( FAILED(hr) ) { DXTrace( __FILE__, (__DWORD__)__LINE__, hr, 0, true ); } }
    #define (x)    { hr = x; if( FAILED(hr) ) { return DXTrace( __FILE__, (__DWORD__)__LINE__, hr, 0, true ); } }
#else
    #define (x)           { hr = x; }
    #define (x)    { hr = x; if( FAILED(hr) ) { return hr; } }
#endif
#define ////////////SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=0; } }
#endif

// Common Types
// ------------

typedef float complex_number[2];

#endif //_COMMON_H_

