
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

#include "SIMD.h"

void SetMemory(__m128 *mem, __m128 value, const unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		mem[i] = value;
	}
}

const SSE_Globals g_SSE_Globals =
{
	_mm_set1_epi32(0x7FFFFFFF), // Abs
	_mm_set1_epi32(0x47FFEFFF), // Infinity
	_mm_set1_epi32(0x38800000), // Denormal
	_mm_set1_epi32(0x7F800000), // Exponent
	_mm_set1_epi32(0x07000000), // Adjust
	_mm_set1_epi32(0x007FFFFF), // Mantissa
	_mm_set1_epi32(0x00800000), // OneDot
	_mm_set1_epi32(0x48000000), // Fixup
	_mm_set1_epi32(0x00000001), // Round0
	_mm_set1_epi32(0x00000FFF), // Round1
	_mm_set1_epi32(0x00007FFF), // Base
};
