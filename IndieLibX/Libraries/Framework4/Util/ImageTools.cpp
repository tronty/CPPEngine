
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

#include "ImageTools.h"
#include "../Math/Half.h"

enum FormatFlags
{
	FF_SIGNED  = 0x1,
	FF_INTEGER = 0x2,
	FF_FLOAT   = 0x4,

	TYPE_UNORM  = 0x0,
	TYPE_SNORM  = FF_SIGNED,
	TYPE_UINT   = FF_INTEGER,
	TYPE_SINT   = FF_SIGNED | FF_INTEGER,
	TYPE_SFLOAT = FF_FLOAT | FF_SIGNED,
	TYPE_UFLOAT = FF_FLOAT,

	FF_COMPRESSED = 0x8,
};

struct FormatInfo
{
	uint8 ChannelCount;
	uint8 BytesPerChannel;
	uint8 BytesPerBlock; // Pixel or compressed block
	uint8 ChunkShift;
	uint8 ChunkRound;
	uint8 BlockShift;
	uint8 BlockRound;
	uint8 Flags;
};

static const FormatInfo g_FormatInfo[] =
{
	{ 0, 0, 0,  0,  0, 0, 0, 0           }, // INVALID

	{ 1, 1, 1,  4, 15, 0, 0, TYPE_UNORM  }, // R8
	{ 1, 1, 1,  4, 15, 0, 0, TYPE_UINT   }, // R8UI

	{ 2, 1, 2,  4, 15, 0, 0, TYPE_UNORM  }, // RG8
	{ 2, 1, 2,  4, 15, 0, 0, TYPE_UINT   }, // RG8UI

	{ 4, 1, 4,  4, 15, 0, 0, TYPE_UNORM  }, // RGBA8
	{ 4, 1, 4,  4, 15, 0, 0, TYPE_UINT   }, // RGBA8UI

	{ 1, 2, 2,  4, 15, 0, 0, TYPE_UNORM  }, // R16
	{ 1, 2, 2,  4, 15, 0, 0, TYPE_UINT   }, // R16UI
	{ 1, 2, 2,  4, 15, 0, 0, TYPE_SFLOAT }, // R16F

	{ 2, 2, 4,  4, 15, 0, 0, TYPE_UNORM  }, // RG16
	{ 2, 2, 4,  4, 15, 0, 0, TYPE_UINT   }, // RG16UI
	{ 2, 2, 4,  4, 15, 0, 0, TYPE_SFLOAT }, // RG16F

	{ 4, 2, 8,  4, 15, 0, 0, TYPE_UNORM  }, // RGBA16
	{ 4, 2, 8,  4, 15, 0, 0, TYPE_UINT   }, // RGBA16UI
	{ 4, 2, 8,  4, 15, 0, 0, TYPE_SFLOAT }, // RGBA16F

	{ 1, 4, 4,  4, 15, 0, 0, TYPE_UINT   }, // R32UI
	{ 1, 4, 4,  4, 15, 0, 0, TYPE_SFLOAT }, // R32F

	{ 2, 4, 8,  4, 15, 0, 0, TYPE_UINT   }, // RG32UI
	{ 2, 4, 8,  4, 15, 0, 0, TYPE_SFLOAT }, // RG32F

	{ 4, 4, 16, 4, 15, 0, 0, TYPE_UINT   }, // RGBA32UI
	{ 4, 4, 16, 4, 15, 0, 0, TYPE_SFLOAT }, // RGBA32F

	{ 3, 0, 8,  3,  7, 2, 3, FF_COMPRESSED }, // BC1 / DXT1
	{ 4, 0, 16, 4, 15, 2, 3, FF_COMPRESSED }, // BC2 / DXT3
	{ 4, 0, 16, 4, 15, 2, 3, FF_COMPRESSED }, // BC3 / DXT5
	{ 1, 0, 8,  3,  7, 2, 3, FF_COMPRESSED }, // BC4
	{ 2, 0, 16, 4, 15, 2, 3, FF_COMPRESSED }, // BC5
	{ 4, 0, 16, 4, 15, 2, 3, FF_COMPRESSED }, // BC7

	{ 1, 2, 2,  4, 15, 0, 0, TYPE_UNORM  }, // D16

	{ 4, 1, 4,  4, 15, 0, 0, TYPE_UNORM  }, // BGRA8
};
static_assert(elementsof(g_FormatInfo) == IMGFMT_COUNT, "g_FormatInfo incorrect length");

bool IsCompressed(ImageFormat format)
{
	return (g_FormatInfo[format].Flags & FF_COMPRESSED) != 0;
}

bool IsFloatFormat(ImageFormat format)
{
	return (g_FormatInfo[format].Flags & FF_FLOAT) != 0;
}

bool IsIntegerFormat(ImageFormat format)
{
	return (g_FormatInfo[format].Flags & FF_INTEGER) != 0;
}

bool IsDepthFormat(ImageFormat format)
{
	return (format == IMGFMT_D16);
}

bool IsSignedFormat(ImageFormat format)
{
	return (g_FormatInfo[format].Flags & FF_SIGNED) != 0;
}

uint GetBytesPerChannel(ImageFormat format)
{
	return (uint) g_FormatInfo[format].BytesPerChannel;
}

uint GetBytesPerBlock(ImageFormat format)
{
	return (uint) g_FormatInfo[format].BytesPerBlock;
}


uint GetUnitCount(const ImageFormat format, const uint size)
{
	const FormatInfo &info = g_FormatInfo[format];
	return (size + info.BlockRound) >> info.BlockShift;
}

uint GetChunkCount(const ImageFormat format, const uint width)
{
	const FormatInfo &info = g_FormatInfo[format];

	uint units = (width + info.BlockRound) >> info.BlockShift;
	return (units * info.BytesPerBlock + info.ChunkRound) >> info.ChunkShift;
}

uint GetChunkCount(const ImageFormat format, const uint width, const uint height)
{
	const FormatInfo &info = g_FormatInfo[format];

	uint units = (height + info.BlockRound) >> info.BlockShift;
	return units * GetChunkCount(format, width);
}

uint GetChunkCount(const ImageFormat format, const uint width, const uint height, const uint depth, const uint mipmap_count, const uint slices)
{
	const FormatInfo &info = g_FormatInfo[format];

	uint w = width;
	uint h = height;
	uint d = depth;

	uint chunks = 0;
	for (uint i = 0; i < mipmap_count; ++i)
	{
		uint units_w = (w + info.BlockRound) >> info.BlockShift;
		uint units_h = (h + info.BlockRound) >> info.BlockShift;
		chunks += d * units_h * ((units_w * info.BytesPerBlock + info.ChunkRound) >> info.ChunkShift);

		if (w > 1) w >>= 1;
		if (h > 1) h >>= 1;
		if (d > 1) d >>= 1;
	}

	return slices * chunks;
}

uint GetPitch(const ImageFormat format, const uint width)
{
	const FormatInfo &info = g_FormatInfo[format];

	uint units = (width + info.BlockRound) >> info.BlockShift;
	return (units * info.BytesPerBlock + info.ChunkRound) & ~(uint) info.ChunkRound;
}

uint GetRowSize(const ImageFormat format, const uint width)
{
	const FormatInfo &info = g_FormatInfo[format];

	uint units = (width + info.BlockRound) >> info.BlockShift;
	return (units * info.BytesPerBlock);
}

uint GetSliceSize(const ImageFormat format, const uint width, const uint height)
{
	const FormatInfo &info = g_FormatInfo[format];

	uint units = (height + info.BlockRound) >> info.BlockShift;
	return units * GetPitch(format, width);
}

uint GetBoxSize(const ImageFormat format, const uint width, const uint height, const uint depth)
{
	return depth * GetSliceSize(format, width, height);
}

uint GetImageSize(const ImageFormat format, const uint width, const uint height, const uint depth, const uint mipmap_count, const uint slices)
{
	uint w = width;
	uint h = height;
	uint d = depth;

	uint size = 0;
	for (uint i = 0; i < mipmap_count; ++i)
	{
		size += GetBoxSize(format, w, h, d);

		if (w > 1) w >>= 1;
		if (h > 1) h >>= 1;
		if (d > 1) d >>= 1;
	}

	return slices * size;
}

uint GetMipMapCount(const uint largest_dim)
{
#ifdef WIN32
	unsigned long result;
	_BitScanReverse(&result, largest_dim);
	return result + 1;
#else
	return 32 - __builtin_clz(largest_dim);
#endif
}

void ConvertBGRA8toRGBA8(__m128i *pixels, const uint count)
{
#ifdef USE_SSSE3
	const __m128i shuf = _mm_setr_epi32(0x03000102, 0x07040506, 0x0B08090A, 0x0F0C0D0E);
#else
	const __m128i mask0 = _mm_set1_epi32(0x00FF00FF);
	const __m128i mask1 = _mm_set1_epi32(0xFF00FF00);
#endif

	for (uint i = 0; i < count; ++i)
	{
		__m128i bgra = pixels[i];
#ifdef USE_SSSE3
		__m128i rgba = _mm_shuffle_epi8(bgra, shuf);
#else
		__m128i b_r_ = _mm_and_si128(bgra, mask0);
		__m128i _g_a = _mm_and_si128(bgra, mask1);

		b_r_ = _mm_shufflelo_epi16(b_r_, 0xB1);
		b_r_ = _mm_shufflehi_epi16(b_r_, 0xB1);

		__m128i rgba = _mm_or_si128(b_r_, _g_a);
#endif
		pixels[i] = rgba;
	}
}

void Convert16Fto32F(__m128i *dst, __m128i *src, const uint count)
{
	for (uint i = 0; i < count * 8; ++i)
	{
		unsigned short sh = ((unsigned short *) src)[i];
		float f = half::Convert(sh);

		((float *) dst)[i] = f;
	}
}

void Convert32Fto16F(__m128i *dst, __m128i *src, const uint count)
{
	// Based on code from here:
	// http://www.devmaster.net/forums/showthread.php?t=10924

	__m128i reg0, reg1, reg2, reg3, reg4, reg5;

	for (uint i = 0; i < count; ++i)
	{
		reg0 = src[i];

		// Sign
		reg1 = _mm_andnot_si128(g_SSE_Globals.Abs, reg0);
		reg1 = _mm_srai_epi32(reg1, 16);

		// Compute masks
		reg0 = _mm_and_si128(reg0, g_SSE_Globals.Abs);
		reg2 = _mm_castps_si128(_mm_cmpnlt_ps(_mm_castsi128_ps(reg0), _mm_castsi128_ps(g_SSE_Globals.Infinity)));
		reg3 = _mm_castps_si128(_mm_cmplt_ps(_mm_castsi128_ps(reg0), _mm_castsi128_ps(g_SSE_Globals.Denormal)));

		// Denormal case
		reg4 = _mm_and_si128(reg0, g_SSE_Globals.Exponent);
		reg4 = _mm_add_epi32(reg4, g_SSE_Globals.Adjust);
		reg5 = _mm_and_si128(reg0, g_SSE_Globals.Mantissa);
		reg5 = _mm_or_si128(reg5, g_SSE_Globals.OneDot);
		reg5 = _mm_cvtps_epi32(_mm_castsi128_ps(reg5));
		reg5 = _mm_castps_si128(_mm_mul_ps(_mm_castsi128_ps(reg5), _mm_castsi128_ps(reg4)));
		reg5 = _mm_cvttps_epi32(_mm_castsi128_ps(reg5));

		// Normal case and combine
		reg0 = _mm_add_epi32(reg0, g_SSE_Globals.Fixup);
		reg5 = _mm_and_si128(reg5, reg3);
		reg3 = _mm_andnot_si128(reg3, reg0);
		reg5 = _mm_or_si128(reg5, reg3);

		// Correct rounding
		reg0 = _mm_srli_epi32(reg5, 13);
		reg0 = _mm_and_si128(reg0, g_SSE_Globals.Round0);
		reg0 = _mm_add_epi32(reg0, g_SSE_Globals.Round1);
		reg0 = _mm_add_epi32(reg0, reg5);

		// Combine with sign and infinity
		reg0 = _mm_srli_epi32(reg0, 13);
		reg0 = _mm_or_si128(reg0, reg2);
		reg0 = _mm_and_si128(reg0, g_SSE_Globals.Base);

		// Result in lower words of each element
		reg0 = _mm_or_si128(reg0, reg1);

		// Convert to 16bit
		reg0 = _mm_packs_epi32(reg0, reg0);

		// Store result
		_mm_storel_pi(((__m64 *) dst) + i, _mm_castsi128_ps(reg0));
	}
}

void GrayscaleRGBA8(__m128i* dst, __m128i* src, const uint count)
{
	__m128i zero = _mm_setzero_si128();
	__m128i f = _mm_setr_epi16(38, 75, 15, 0, 38, 76, 15, 0);
	__m128i one = _mm_set1_epi16(1);
	for (uint i = 0; i < count; i++)
	{
		__m128i c = src[i];
		__m128i c0 = _mm_unpacklo_epi8(c, zero);
		__m128i c1 = _mm_unpackhi_epi8(c, zero);

		c0 = _mm_madd_epi16(c0, f);
		c1 = _mm_madd_epi16(c1, f);

		c0 = _mm_packs_epi32(c0, c1);
		c0 = _mm_madd_epi16(c0, one);
		c0 = _mm_packs_epi32(c0, zero);
		c0 = _mm_srli_epi16(c0, 7);
		c0 = _mm_packus_epi16(c0, zero);
		((uint32*) dst)[i] = _mm_cvtsi128_si32(c0);
	}
}

void GenerateMipMapR8(__m128i *dest, __m128i *row0, __m128i *row1, const uint count)
{
	const __m128i mask = _mm_set1_epi32(0x00FF00FF);

	const uint dst_count = count / 2;
	for (uint i = 0; i < dst_count; i++)
	{
		__m128i r00 = row0[2 * i + 0];
		__m128i r01 = row0[2 * i + 1];
		__m128i r10 = row1[2 * i + 0];
		__m128i r11 = row1[2 * i + 1];

		__m128i r0 = _mm_avg_epu8(r00, r10);
		__m128i r1 = _mm_avg_epu8(r01, r11);

		__m128i s00 = _mm_and_si128(r0, mask);
		__m128i s01 = _mm_and_si128(r1, mask);

		__m128i s10 = _mm_srli_epi16(r0, 8);
		__m128i s11 = _mm_srli_epi16(r1, 8);

		__m128i s0 = _mm_packus_epi16(s00, s01);
		__m128i s1 = _mm_packus_epi16(s10, s11);

		dest[i] = _mm_avg_epu8(s0, s1);
	}
	if (count & 1)
	{
		__m128i r0 = row0[count - 1];
		__m128i r1 = row1[count - 1];

		__m128i r = _mm_avg_epu8(r0, r1);

		__m128i s0 = _mm_and_si128(r, mask);
		__m128i s1 = _mm_srli_epi16(r, 8);

		__m128i z = _mm_setzero_si128();
		s0 = _mm_packus_epi16(s0, z);
		s1 = _mm_packus_epi16(s1, z);

		dest[dst_count] = _mm_avg_epu8(s0, s1);
	}
}

void GenerateMipMapRG8(__m128i *dest, __m128i *row0, __m128i *row1, const uint count)
{
	const uint dst_count = count / 2;
	for (uint i = 0; i < dst_count; i++)
	{
		__m128i r00 = row0[2 * i + 0];
		__m128i r01 = row0[2 * i + 1];
		__m128i r10 = row1[2 * i + 0];
		__m128i r11 = row1[2 * i + 1];

		__m128i r0 = _mm_avg_epu8(r00, r10);
		__m128i r1 = _mm_avg_epu8(r01, r11);

		__m128i s00 = _mm_srai_epi32(_mm_slli_epi32(r0, 16), 16);
		__m128i s01 = _mm_srai_epi32(_mm_slli_epi32(r1, 16), 16);

		__m128i s10 = _mm_srai_epi32(r0, 16);
		__m128i s11 = _mm_srai_epi32(r1, 16);

		__m128i s0 = _mm_packs_epi32(s00, s01);
		__m128i s1 = _mm_packs_epi32(s10, s11);

		dest[i] = _mm_avg_epu8(s0, s1);
	}
	if (count & 1)
	{
		__m128i r0 = row0[count - 1];
		__m128i r1 = row1[count - 1];

		__m128i r = _mm_avg_epu8(r0, r1);

		__m128i s0 = _mm_srai_epi32(_mm_slli_epi32(r, 16), 16);
		__m128i s1 = _mm_srai_epi32(r, 16);

		__m128i z = _mm_setzero_si128();
		s0 = _mm_packs_epi32(s0, z);
		s1 = _mm_packs_epi32(s1, z);

		dest[dst_count] = _mm_avg_epu8(s0, s1);
	}
}

void GenerateMipMapRGBA8(__m128i *dest, __m128i *row0, __m128i *row1, const uint count)
{
	const uint dst_count = count / 2;
	for (uint i = 0; i < dst_count; i++)
	{
		__m128i r00 = row0[2 * i + 0];
		__m128i r01 = row0[2 * i + 1];
		__m128i r10 = row1[2 * i + 0];
		__m128i r11 = row1[2 * i + 1];

		__m128i r0 = _mm_avg_epu8(r00, r10);
		__m128i r1 = _mm_avg_epu8(r01, r11);

		__m128i s0 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r0), _mm_castsi128_ps(r1), 0x88));
		__m128i s1 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r0), _mm_castsi128_ps(r1), 0xDD));

		dest[i] = _mm_avg_epu8(s0, s1);
	}
	if (count & 1)
	{
		__m128i r0 = row0[count - 1];
		__m128i r1 = row1[count - 1];

		__m128i r = _mm_avg_epu8(r0, r1);

		__m128i z = _mm_setzero_si128();
		__m128i s0 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r), _mm_castsi128_ps(z), 0x88));
		__m128i s1 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r), _mm_castsi128_ps(z), 0xDD));

		dest[dst_count] = _mm_avg_epu8(s0, s1);
	}
}

void GenerateMipMapR16(__m128i *dest, __m128i *row0, __m128i *row1, const uint count)
{
	const uint dst_count = count / 2;
	for (uint i = 0; i < dst_count; i++)
	{
		__m128i r00 = row0[2 * i + 0];
		__m128i r01 = row0[2 * i + 1];
		__m128i r10 = row1[2 * i + 0];
		__m128i r11 = row1[2 * i + 1];

		__m128i r0 = _mm_avg_epu16(r00, r10);
		__m128i r1 = _mm_avg_epu16(r01, r11);

		__m128i s00 = _mm_srai_epi32(_mm_slli_epi32(r0, 16), 16);
		__m128i s01 = _mm_srai_epi32(_mm_slli_epi32(r1, 16), 16);

		__m128i s10 = _mm_srai_epi32(r0, 16);
		__m128i s11 = _mm_srai_epi32(r1, 16);

		__m128i s0 = _mm_packs_epi32(s00, s01);
		__m128i s1 = _mm_packs_epi32(s10, s11);

		dest[i] = _mm_avg_epu16(s0, s1);
	}
	if (count & 1)
	{
		__m128i r0 = row0[count - 1];
		__m128i r1 = row1[count - 1];

		__m128i r = _mm_avg_epu16(r0, r1);

		__m128i s0 = _mm_srai_epi32(_mm_slli_epi32(r, 16), 16);
		__m128i s1 = _mm_srai_epi32(r, 16);

		__m128i z = _mm_setzero_si128();
		s0 = _mm_packs_epi32(s0, z);
		s1 = _mm_packs_epi32(s1, z);

		dest[dst_count] = _mm_avg_epu16(s0, s1);
	}
}

void GenerateMipMapRG16(__m128i *dest, __m128i *row0, __m128i *row1, const uint count)
{
	const uint dst_count = count / 2;
	for (uint i = 0; i < dst_count; i++)
	{
		__m128i r00 = row0[2 * i + 0];
		__m128i r01 = row0[2 * i + 1];
		__m128i r10 = row1[2 * i + 0];
		__m128i r11 = row1[2 * i + 1];

		__m128i r0 = _mm_avg_epu16(r00, r10);
		__m128i r1 = _mm_avg_epu16(r01, r11);

		__m128i s0 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r0), _mm_castsi128_ps(r1), 0x88));
		__m128i s1 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r0), _mm_castsi128_ps(r1), 0xDD));

		dest[i] = _mm_avg_epu16(s0, s1);
	}
	if (count & 1)
	{
		__m128i r0 = row0[count - 1];
		__m128i r1 = row1[count - 1];

		__m128i r = _mm_avg_epu16(r0, r1);

		__m128i z = _mm_setzero_si128();
		__m128i s0 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r), _mm_castsi128_ps(z), 0x88));
		__m128i s1 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r), _mm_castsi128_ps(z), 0xDD));

		dest[dst_count] = _mm_avg_epu16(s0, s1);
	}
}

void GenerateMipMapRGBA16(__m128i *dest, __m128i *row0, __m128i *row1, const uint count)
{
	const uint dst_count = count / 2;
	for (uint i = 0; i < dst_count; i++)
	{
		__m128i r00 = row0[2 * i + 0];
		__m128i r01 = row0[2 * i + 1];
		__m128i r10 = row1[2 * i + 0];
		__m128i r11 = row1[2 * i + 1];

		__m128i r0 = _mm_avg_epu16(r00, r10);
		__m128i r1 = _mm_avg_epu16(r01, r11);

		__m128i s0 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r0), _mm_castsi128_ps(r1), 0x44));
		__m128i s1 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r0), _mm_castsi128_ps(r1), 0xEE));

		dest[i] = _mm_avg_epu16(s0, s1);
	}
	if (count & 1)
	{
		__m128i r0 = row0[count - 1];
		__m128i r1 = row1[count - 1];

		__m128i r = _mm_avg_epu16(r0, r1);

		__m128i z = _mm_setzero_si128();
		__m128i s0 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r), _mm_castsi128_ps(z), 0x44));
		__m128i s1 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r), _mm_castsi128_ps(z), 0xEE));

		dest[dst_count] = _mm_avg_epu16(s0, s1);
	}
}

void GenerateMipMapR32F(__m128i *dest, __m128i *row0, __m128i *row1, const uint count)
{
	const __m128 quarter = _mm_set1_ps(0.25f);

	const uint dst_count = count / 2;
	for (uint i = 0; i < dst_count; i++)
	{
		__m128i r00 = row0[2 * i + 0];
		__m128i r01 = row0[2 * i + 1];
		__m128i r10 = row1[2 * i + 0];
		__m128i r11 = row1[2 * i + 1];

		__m128 r0 = _mm_add_ps(_mm_castsi128_ps(r00), _mm_castsi128_ps(r10));
		__m128 r1 = _mm_add_ps(_mm_castsi128_ps(r01), _mm_castsi128_ps(r11));
		__m128 r = _mm_hadd_ps(r0, r1);

		dest[i] = _mm_castps_si128(_mm_mul_ps(r, quarter));
	}
	if (count & 1)
	{
		__m128i r0 = row0[count - 1];
		__m128i r1 = row1[count - 1];

		__m128 r = _mm_add_ps(_mm_castsi128_ps(r0), _mm_castsi128_ps(r1));
		__m128 z = _mm_setzero_ps();
		r = _mm_hadd_ps(r, z);

		dest[dst_count] = _mm_castps_si128(_mm_mul_ps(r, quarter));
	}
}

void GenerateMipMapRG32F(__m128i *dest, __m128i *row0, __m128i *row1, const uint count)
{
	const __m128 quarter = _mm_set1_ps(0.25f);

	const uint dst_count = count / 2;
	for (uint i = 0; i < dst_count; i++)
	{
		__m128i r00 = row0[2 * i + 0];
		__m128i r01 = row0[2 * i + 1];
		__m128i r10 = row1[2 * i + 0];
		__m128i r11 = row1[2 * i + 1];

		__m128 r0 = _mm_add_ps(_mm_castsi128_ps(r00), _mm_castsi128_ps(r10));
		__m128 r1 = _mm_add_ps(_mm_castsi128_ps(r01), _mm_castsi128_ps(r11));

		__m128 s0 = _mm_shuffle_ps(r0, r1, 0x44);
		__m128 s1 = _mm_shuffle_ps(r0, r1, 0xEE);
		__m128 s = _mm_add_ps(s0, s1);

		dest[i] = _mm_castps_si128(_mm_mul_ps(s, quarter));
	}

	if (count & 1)
	{
		__m128i r0 = row0[count - 1];
		__m128i r1 = row1[count - 1];

		__m128i r = _mm_avg_epu16(r0, r1);

		__m128i z = _mm_setzero_si128();
		__m128i s0 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r), _mm_castsi128_ps(z), 0x44));
		__m128i s1 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(r), _mm_castsi128_ps(z), 0xEE));

		dest[dst_count] = _mm_avg_epu16(s0, s1);
	}
}

void GenerateMipMapRGBA32F(__m128i *dest, __m128i *row0, __m128i *row1, const uint count)
{
	const __m128 quarter = _mm_set1_ps(0.25f);

	const uint dst_count = count / 2;
	for (uint i = 0; i < dst_count; i++)
	{
		__m128i r00 = row0[2 * i + 0];
		__m128i r01 = row0[2 * i + 1];
		__m128i r10 = row1[2 * i + 0];
		__m128i r11 = row1[2 * i + 1];

		__m128 r0 = _mm_add_ps(_mm_castsi128_ps(r00), _mm_castsi128_ps(r10));
		__m128 r1 = _mm_add_ps(_mm_castsi128_ps(r01), _mm_castsi128_ps(r11));
		__m128 r = _mm_add_ps(r0, r1);

		dest[i] = _mm_castps_si128(_mm_mul_ps(r, quarter));
	}
}

