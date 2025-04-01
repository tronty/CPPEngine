/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//-----------------------------------------------------------------------------
// File: fft.h
//
// Wrapper functions for the fftw3 library
//
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _FFT_H_
#define _FFT_H_

#ifdef _MSC_VER
#ifdef FFT_EXPORTS
#define FFT_API __declspec(dllexport)
#else
#define FFT_API __declspec(dllimport)
#endif
#else
#define FFT_API
#endif

#include "fftw3.h"

// inverse fast forier transform
inline FFT_API fftwf_plan ifft_plan(int resolution, fftwf_complex *frequencyData, fftwf_complex *spatialData){return fftwf_plan();}
inline FFT_API void fft_execute(fftwf_plan plan){}
inline FFT_API void destroy_plan(fftwf_plan plan){}

#endif
