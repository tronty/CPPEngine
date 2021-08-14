
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

//#define USE_SSSE3
//#define USE_SSE4

//#include <mmintrin.h>  // MMX
//#include <xmmintrin.h> // SSE
//#include <emmintrin.h> // SSE2
//#include <pmmintrin.h> // SSE3
//#include <tmmintrin.h> // SSSE3
//#include <smmintrin.h> // SSE4.1
//#include <nmmintrin.h> // SSE4.2
//#include <ammintrin.h> // SSE4a
//#include <bmmintrin.h> // SSE5
//#include <mmintrin-common.h>

#if defined(USE_SSE4)
  #include <smmintrin.h>
  #define USE_SSSE3
#elif defined(USE_SSSE3)
  #include <tmmintrin.h>
#else
  #include <pmmintrin.h>
#endif

#define _mm_realloc(m_Mem, size, al) _aligned_realloc(m_Mem, size, al)

void SetMemory(__m128 *mem, __m128 value, const unsigned int count);
inline void ClearMemory(__m128 *mem, const unsigned int count)
{
	SetMemory(mem, _mm_setzero_ps(), count);
}

struct SSE_Globals
{
	__m128i Abs;
	__m128i Infinity;
	__m128i Denormal;
	__m128i Exponent;
	__m128i Adjust;
	__m128i Mantissa;
	__m128i OneDot;
	__m128i Fixup;
	__m128i Round0;
	__m128i Round1;
	__m128i Base;
};

extern const SSE_Globals g_SSE_Globals;

inline float rcpf(float x)
{
	return _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ss(x)));
}

inline float rsqrtf(float x)
{
	return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x)));
}
