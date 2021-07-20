
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

#include "../Platform.h"
#include "../SIMD.h"

enum ImageFormat
{
	IMGFMT_INVALID,

	IMGFMT_R8,
	IMGFMT_R8UI,

	IMGFMT_RG8,
	IMGFMT_RG8UI,

	IMGFMT_RGBA8,
	IMGFMT_RGBA8UI,

	IMGFMT_R16,
	IMGFMT_R16UI,
	IMGFMT_R16F,

	IMGFMT_RG16,
	IMGFMT_RG16UI,
	IMGFMT_RG16F,

	IMGFMT_RGBA16,
	IMGFMT_RGBA16UI,
	IMGFMT_RGBA16F,

	IMGFMT_R32UI,
	IMGFMT_R32F,

	IMGFMT_RG32UI,
	IMGFMT_RG32F,

	IMGFMT_RGBA32UI,
	IMGFMT_RGBA32F,

	IMGFMT_BC1,
	IMGFMT_BC2,
	IMGFMT_BC3,
	IMGFMT_BC4,
	IMGFMT_BC5,
	IMGFMT_BC7,

	IMGFMT_D16,

	IMGFMT_BGRA8,

	IMGFMT_COUNT,
};

bool IsCompressed(ImageFormat format);
bool IsFloatFormat(ImageFormat format);
bool IsIntegerFormat(ImageFormat format);
bool IsDepthFormat(ImageFormat format);
bool IsSignedFormat(ImageFormat format);
uint GetBytesPerChannel(ImageFormat format);
uint GetBytesPerBlock(ImageFormat format);

uint GetChunkCount(const ImageFormat format, const uint width);
uint GetChunkCount(const ImageFormat format, const uint width, const uint height);
uint GetChunkCount(const ImageFormat format, const uint width, const uint height, const uint depth, const uint mipmap_count, const uint slices);

uint GetUnitCount(const ImageFormat format, const uint size);

uint GetPitch(const ImageFormat format, const uint width);
uint GetRowSize(const ImageFormat format, const uint width);
uint GetSliceSize(const ImageFormat format, const uint width, const uint height);
uint GetBoxSize(const ImageFormat format, const uint width, const uint height, const uint depth);
uint GetImageSize(const ImageFormat format, const uint width, const uint height, const uint depth, const uint mipmap_count, const uint slices);

uint GetMipMapCount(const uint largest_dim);

void ConvertBGRA8toRGBA8(__m128i *pixels, const uint count);
void Convert16Fto32F(__m128i *dst, __m128i *src, const uint count);
void Convert32Fto16F(__m128i *dst, __m128i *src, const uint count);

void GrayscaleRGBA8(__m128i* dst, __m128i* src, const uint count);

void GenerateMipMapR8(__m128i *dest, __m128i *row0, __m128i *row1, const uint count);
void GenerateMipMapRG8(__m128i *dest, __m128i *row0, __m128i *row1, const uint count);
void GenerateMipMapRGBA8(__m128i *dest, __m128i *row0, __m128i *row1, const uint count);
void GenerateMipMapR16(__m128i *dest, __m128i *row0, __m128i *row1, const uint count);
void GenerateMipMapRG16(__m128i *dest, __m128i *row0, __m128i *row1, const uint count);
void GenerateMipMapRGBA16(__m128i *dest, __m128i *row0, __m128i *row1, const uint count);
void GenerateMipMapR32F(__m128i *dest, __m128i *row0, __m128i *row1, const uint count);
void GenerateMipMapRG32F(__m128i *dest, __m128i *row0, __m128i *row1, const uint count);
void GenerateMipMapRGBA32F(__m128i *dest, __m128i *row0, __m128i *row1, const uint count);
