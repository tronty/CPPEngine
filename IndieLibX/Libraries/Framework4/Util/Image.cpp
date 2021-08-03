
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

#include "Image.h"
#include "ImageTools.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <string.h>

#pragma pack (push, 1)

#define DDPF_ALPHAPIXELS 0x00000001 
#define DDPF_FOURCC      0x00000004 
#define DDPF_RGB         0x00000040

#define DDSD_CAPS        0x00000001
#define DDSD_HEIGHT      0x00000002
#define DDSD_WIDTH       0x00000004
#define DDSD_PITCH       0x00000008
#define DDSD_PIXELFORMAT 0x00001000
#define DDSD_MIPMAPCOUNT 0x00020000
#define DDSD_LINEARSIZE  0x00080000
#define DDSD_DEPTH       0x00800000

#define DDSCAPS_COMPLEX  0x00000008 
#define DDSCAPS_TEXTURE  0x00001000 
#define DDSCAPS_MIPMAP   0x00400000 

#define DDSCAPS2_CUBEMAP 0x00000200 
#define DDSCAPS2_VOLUME  0x00200000 

#define DDSCAPS2_CUBEMAP_POSITIVEX 0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX 0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY 0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY 0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ 0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ 0x00008000
#define DDSCAPS2_CUBEMAP_ALL_FACES (DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ)

#define D3D10_RESOURCE_MISC_TEXTURECUBE 0x4
#define D3D10_RESOURCE_DIMENSION_BUFFER    1
#define D3D10_RESOURCE_DIMENSION_TEXTURE1D 2
#define D3D10_RESOURCE_DIMENSION_TEXTURE2D 3
#define D3D10_RESOURCE_DIMENSION_TEXTURE3D 4

struct DDSHeader
{
	uint32 Magic;
	uint32 Size;
	uint32 Flags;
	uint32 Height;
	uint32 Width;
	uint32 PitchOrLinearSize;
	uint32 Depth; 
	uint32 MipMapCount;
	uint32 Reserved[11];

	struct
	{
		uint32 Size;
		uint32 Flags;
		uint32 FourCC;
		uint32 RGBBitCount;
		uint32 RBitMask;
		uint32 GBitMask;
		uint32 BBitMask;
		uint32 RGBAlphaBitMask; 
	} PixelFormat;

	struct
	{
		uint32 Caps1;
		uint32 Caps2;
		uint32 Reserved[2];
	} Caps;

	uint32 Reserved2;
};

struct DDSHeaderDX10
{
	uint32 DXGIFormat;
	uint32 ResourceDimension;
	uint32 MiscFlag;
	uint32 ArraySize;
	uint32 Reserved;
};

#pragma pack (pop)


Image::Image()
	: m_Pixels(nullptr)
	, m_Width(0)
	, m_Height(0)
	, m_Depth(0)
	, m_Slices(0)
	, m_Format(IMGFMT_INVALID)
{

}

Image::~Image()
{
	_mm_free(m_Pixels);
}

uint Image::GetPitch() const
{
	return ::GetPitch(m_Format, m_Width);
}

uint Image::GetSliceSize() const
{
	return ::GetSliceSize(m_Format, m_Width, m_Height);
}

uint Image::GetMipPitch(const uint mipmap) const
{
	return ::GetPitch(m_Format, GetMipWidth(mipmap));
}

uint Image::GetMipSliceSize(const uint mipmap) const
{
	return ::GetSliceSize(m_Format, GetMipWidth(mipmap), GetMipHeight(mipmap));
}

uint Image::GetImageSize() const
{
	return ::GetImageSize(m_Format, m_Width, m_Height, m_Depth, m_MipMapCount, m_Slices);
}

void *Image::Create(const ImageFormat format, const uint width, const uint height, const uint depth, const uint slices, const int mipmaps)
{
	_mm_free(m_Pixels);

	m_Width  = width;
	m_Height = height;
	m_Depth  = depth;
	m_Slices = slices;
	m_Format = format;
	m_MipMapCount = mipmaps;

	const uint size = GetImageSize();
	m_Pixels = (__m128i *) _mm_malloc(size, 16);
	
	return m_Pixels;
}

bool Image::Load(const char* filename)
{
	DDSHeader header;

	const char *ext = strrchr(filename, '.');
	if (ext && stricmp(ext + 1, "dds") == 0)
	{
		// DDS file
		FILE *file = fopen(filename, "rb");
		if (file == nullptr)
			return false;

		size_t bytes_read = fread(&header, 1, sizeof(header), file);
		if (bytes_read < 4 || header.Magic != MCHAR4('D','D','S',' '))
		{
			fclose(file);
			return false;
		}

		m_Width  = header.Width;
		m_Height = header.Height;
		m_Depth  = (header.Caps.Caps2 & DDSCAPS2_VOLUME)? header.Depth : 1;
		m_Slices = (header.Caps.Caps2 & DDSCAPS2_CUBEMAP)? 6 : 1;
		m_MipMapCount = (header.Caps.Caps1 & DDSCAPS_MIPMAP)? header.MipMapCount : 1;
		//arraySize = 1;
		//m_Format = IMGFMT_RGBA8;

		if (header.PixelFormat.FourCC == MCHAR4('D','X','1','0'))
		{
			DDSHeaderDX10 dx10Header;
			bytes_read = fread(&dx10Header, 1, sizeof(dx10Header), file);
			if (bytes_read < sizeof(dx10Header))
			{
				fclose(file);
				return false;
			}

			m_Slices = dx10Header.ArraySize;

			switch (dx10Header.DXGIFormat)
			{
				case 2:  m_Format = IMGFMT_RGBA32F; break;
				case 3:  m_Format = IMGFMT_RGBA32UI; break;

				case 10: m_Format = IMGFMT_RGBA16F; break;
				case 11: m_Format = IMGFMT_RGBA16; break;
				case 12: m_Format = IMGFMT_RGBA16UI; break;

				case 16: m_Format = IMGFMT_RG32F; break;
				case 17: m_Format = IMGFMT_RG32UI; break;

				case 28: m_Format = IMGFMT_RGBA8; break;
				case 30: m_Format = IMGFMT_RGBA8UI; break;

				case 34: m_Format = IMGFMT_RG16F; break;
				case 35: m_Format = IMGFMT_RG16; break;
				case 36: m_Format = IMGFMT_RG16UI; break;

				case 41: m_Format = IMGFMT_R32F; break;
				case 42: m_Format = IMGFMT_R32UI; break;

				case 49: m_Format = IMGFMT_RG8; break;
				case 50: m_Format = IMGFMT_RG8UI; break;

				case 54: m_Format = IMGFMT_R16F; break;
				case 56: m_Format = IMGFMT_R16; break;
				case 57: m_Format = IMGFMT_R16UI; break;

				case 61: m_Format = IMGFMT_R8; break;
				case 62: m_Format = IMGFMT_R8UI; break;

	// 			case 6:  m_Format = IMGFMT_RGB32F; break;
	// 			case 67: m_Format = IMGFMT_RGB9E5; break;
	// 			case 26: m_Format = IMGFMT_RG11B10F; break;
	// 			case 24: m_Format = IMGFMT_RGB10A2; break;

				case 71: m_Format = IMGFMT_BC1; break;
				case 74: m_Format = IMGFMT_BC2; break;
				case 77: m_Format = IMGFMT_BC3; break;
				case 80: m_Format = IMGFMT_BC4; break;
				case 83: m_Format = IMGFMT_BC5; break;
				case 98: m_Format = IMGFMT_BC7; break;
				default:
					fclose(file);
					return false;
			}

		}
		else
		{
			switch (header.PixelFormat.FourCC)
			{
				case 34:  m_Format = IMGFMT_RG16; break;
				case 36:  m_Format = IMGFMT_RGBA16; break;
				case 111: m_Format = IMGFMT_R16F; break;
				case 112: m_Format = IMGFMT_RG16F; break;
				case 113: m_Format = IMGFMT_RGBA16F; break;
				case 114: m_Format = IMGFMT_R32F; break;
				case 115: m_Format = IMGFMT_RG32F; break;
				case 116: m_Format = IMGFMT_RGBA32F; break;
				case MCHAR4('D','X','T','1'): m_Format = IMGFMT_BC1; break;
				case MCHAR4('D','X','T','3'): m_Format = IMGFMT_BC2; break;
				case MCHAR4('D','X','T','5'): m_Format = IMGFMT_BC3; break;
				case MCHAR4('A','T','I','1'): m_Format = IMGFMT_BC4; break;
				case MCHAR4('A','T','I','2'): m_Format = IMGFMT_BC5; break;
				default:
					switch (header.PixelFormat.RGBBitCount)
					{
						case 8: m_Format = IMGFMT_R8; break;
						case 16:
							m_Format = (header.PixelFormat.RBitMask == 0xFFFF)? IMGFMT_R16 : IMGFMT_RG8;
							break;
						case 32:
							m_Format = /*(header.PixelFormat.RBitMask == 0x3FF00000)? IMGFMT_RGB10A2 :*/
									   (header.PixelFormat.RBitMask == 0xFFFF)? IMGFMT_RG16 :IMGFMT_RGBA8;
							break;
						default:
							fclose(file);
							return false;
					}
			}
		}


		uint size = GetImageSize();
		_mm_free(m_Pixels);
		m_Pixels = (__m128i *) _mm_malloc(size, 16);
		//ClearMemory((__m128 *) m_Pixels, size >> 4);


		/*if (isCube()){
			for (int face = 0; face < 6; face++){
				for (int mipMapLevel = 0; mipMapLevel < nMipMaps; mipMapLevel++){
					int faceSize = getMipMappedSize(mipMapLevel, 1) / 6;
					unsigned char *src = getPixels(mipMapLevel) + face * faceSize;

					fread(src, 1, faceSize, file);
				}
				if ((flags & DONT_LOAD_MIPMAPS) && header.dwMipMapCount > 1){
					fseek(file, getMipMappedSize(1, header.dwMipMapCount - 1) / 6, SEEK_CUR);
				}
			}
		} else {
			fread(m_Pixels, 1, size, file);
		}*/

		ubyte *dest = (ubyte *) m_Pixels;
		for (uint s = 0; s < m_Slices; ++s)
		{
			for (uint m = 0; m < m_MipMapCount; ++m)
			{
				const uint mip_size = GetMipSliceSize(m);

				const uint src_size = GetRowSize(m_Format, GetMipWidth(m));
				if (IsCompressed(m_Format) || (src_size & 0xF) == 0)
				{
					// Aligned source data
					bytes_read = fread(dest, 1, mip_size, file);
				}
				else
				{
					// Unaligned source data
					const uint row_count = GetMipDepth(m) * GetUnitCount(m_Format, GetMipHeight(m));
					const uint row_size = (src_size + 0xF) & ~0xF;

					for (uint r = 0; r < row_count; ++r)
					{
						bytes_read = fread(dest + r * row_size, 1, src_size, file);
						memset(dest + r * row_size + src_size, 0, row_size - src_size);
					}
				}

				dest += mip_size;
			}
		}

		fclose(file);

		if (m_Format == IMGFMT_RGBA8 && header.PixelFormat.BBitMask == 0xFF)
		{
			ConvertBGRA8toRGBA8(m_Pixels, GetChunkCount(m_Format, m_Width, m_Height, m_Depth, m_MipMapCount, m_Slices));
		}

		return true;
	}
	else
	{
		// Other file formats, load with stb_image.c
		
		int w, h, ch;
		ubyte *data = stbi_load(filename, &w, &h, &ch, 0);
		if (data)
		{
			switch (ch)
			{
				case 1: m_Format = IMGFMT_R8; break;
				case 2: m_Format = IMGFMT_RG8; break;
				case 3:
				case 4: m_Format = IMGFMT_RGBA8; break;
			};
			m_Width  = w;
			m_Height = h;
			m_Depth  = 1;
			m_Slices = 1;
			m_MipMapCount = 1;

			uint size = GetImageSize();
			_mm_free(m_Pixels);
			m_Pixels = (__m128i *) _mm_malloc(size, 16);

			ubyte *dst = (ubyte *) m_Pixels;
			ubyte *src = data;
			if (ch == 3)
			{
				const uint extra_zeros = ((4 * w + 0xF) & ~0xF) - 4 * w;

				for (int y = 0; y < h; y++)
				{
					for (int x = 0; x < w; x++)
					{
						dst[0] = src[0];
						dst[1] = src[1];
						dst[2] = src[2];
						dst[3] = 0xFF;
						dst += 4;
						src += 3;
					}
					memset(dst, 0, extra_zeros);
					dst += extra_zeros;
				}
			}
			else
			{
				const uint src_row_size = ch * w;
				const uint dst_row_size = (src_row_size + 0xF) & ~0xF;

				for (int y = 0; y < h; y++)
				{
					memcpy(dst, data, src_row_size);
					memset(dst + src_row_size, 0, dst_row_size - src_row_size);

					src += src_row_size;
					dst += dst_row_size;
				}
			}

			ASSERT(dst == ((ubyte *) m_Pixels) + size);

			stbi_image_free(data);
			return true;
		}

		return false;
	}
}

bool Image::Save(const char* fileName)
{
	// Set up the header
	DDSHeader header;
	memset(&header, 0, sizeof(header));
	DDSHeaderDX10 headerDX10;
	memset(&headerDX10, 0, sizeof(headerDX10));

	header.Magic = MCHAR4('D', 'D', 'S', ' ');
	header.Size = 124;
	header.Flags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | (m_MipMapCount > 1? DDSD_MIPMAPCOUNT : 0) | (m_Depth > 1? DDSD_DEPTH : 0);
	header.Height = m_Height;
	header.Width = m_Width;
	header.PitchOrLinearSize = 0;
	header.Depth = (m_Depth > 1)? m_Depth : 0;
	header.MipMapCount = (m_MipMapCount > 1)? m_MipMapCount : 0;

	int nChannels = 4;// getChannelCount(m_Format);

	header.PixelFormat.Size = 32;
	if (m_Format <= IMGFMT_R16 && !IsIntegerFormat(m_Format)/* || m_Format == IMGFMT_RGB10A2*/)
	{
		header.PixelFormat.Flags = ((nChannels < 3)? 0x00020000 : DDPF_RGB) | ((nChannels & 1)? 0 : DDPF_ALPHAPIXELS);
		if (m_Format <= IMGFMT_RGBA8)
		{
			header.PixelFormat.RGBBitCount = 8 * nChannels;
			header.PixelFormat.RBitMask = 0xFF;
			header.PixelFormat.GBitMask = (nChannels > 1)? 0x0000FF00 : 0;
			header.PixelFormat.BBitMask = (nChannels > 1)? 0x00FF0000 : 0;
			header.PixelFormat.RGBAlphaBitMask = (nChannels == 4)? 0xFF000000 : (nChannels == 2)? 0xFF00 : 0;
		}
		else if (m_Format == IMGFMT_R16)
		{
			header.PixelFormat.RGBBitCount = 16;
			header.PixelFormat.RBitMask = 0xFFFF;
		}
		else
		{
			header.PixelFormat.RGBBitCount = 32;
			header.PixelFormat.RBitMask = 0x3FF00000;
			header.PixelFormat.GBitMask = 0x000FFC00;
			header.PixelFormat.BBitMask = 0x000003FF;
			header.PixelFormat.RGBAlphaBitMask = 0xC0000000;
		}
	}
	else
	{
		header.PixelFormat.Flags = DDPF_FOURCC;

		switch (m_Format)
		{
		case IMGFMT_RG16:    header.PixelFormat.FourCC = 34; break;
		case IMGFMT_RGBA16:  header.PixelFormat.FourCC = 36; break;
		case IMGFMT_R16F:    header.PixelFormat.FourCC = 111; break;
		case IMGFMT_RG16F:   header.PixelFormat.FourCC = 112; break;
		case IMGFMT_RGBA16F: header.PixelFormat.FourCC = 113; break;
		case IMGFMT_R32F:    header.PixelFormat.FourCC = 114; break;
		case IMGFMT_RG32F:   header.PixelFormat.FourCC = 115; break;
		case IMGFMT_RGBA32F: header.PixelFormat.FourCC = 116; break;
		case IMGFMT_BC1:     header.PixelFormat.FourCC = MCHAR4('D', 'X', 'T', '1'); break;
		case IMGFMT_BC2:     header.PixelFormat.FourCC = MCHAR4('D', 'X', 'T', '3'); break;
		case IMGFMT_BC3:     header.PixelFormat.FourCC = MCHAR4('D', 'X', 'T', '5'); break;
		case IMGFMT_BC4:     header.PixelFormat.FourCC = MCHAR4('A', 'T', 'I', '1'); break;
		case IMGFMT_BC5:     header.PixelFormat.FourCC = MCHAR4('A', 'T', 'I', '2'); break;
		default:
			header.PixelFormat.FourCC = MCHAR4('D', 'X', '1', '0');
			headerDX10.ArraySize = 1;
			headerDX10.MiscFlag = (m_Depth == 0)? D3D10_RESOURCE_MISC_TEXTURECUBE : 0;
			headerDX10.ResourceDimension = /*is1D() ? D3D10_RESOURCE_DIMENSION_TEXTURE1D : is3D() ? D3D10_RESOURCE_DIMENSION_TEXTURE3D : */D3D10_RESOURCE_DIMENSION_TEXTURE2D;
			switch (m_Format)
			{
			//case IMGFMT_RGBA8:    headerDX10.DXGIFormat = 28; break;
			//case IMGFMT_RGB32F:   headerDX10.DXGIFormat = 6; break;
			//case IMGFMT_R16I:     headerDX10.DXGIFormat = 59; break;
			case IMGFMT_R16UI:    headerDX10.DXGIFormat = 57; break;
			//case IMGFMT_RGB9E5:   headerDX10.DXGIFormat = 67; break;
			//case IMGFMT_RG11B10F: headerDX10.DXGIFormat = 26; break;
			default:
				return false;
			}
		}
	}

	header.Caps.Caps1 = DDSCAPS_TEXTURE | (m_MipMapCount > 1? DDSCAPS_MIPMAP | DDSCAPS_COMPLEX : 0) | (m_Depth != 1? DDSCAPS_COMPLEX : 0);
	header.Caps.Caps2 = (m_Depth > 1)? DDSCAPS2_VOLUME : (m_Depth == 0) ? DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALL_FACES : 0;
	header.Caps.Reserved[0] = 0;
	header.Caps.Reserved[1] = 0;
	header.Reserved2 = 0;

	FILE *file;
	if ((file = fopen(fileName, "wb")) == nullptr)
		return false;

	fwrite(&header, sizeof(header), 1, file);
	if (headerDX10.DXGIFormat)
		fwrite(&headerDX10, sizeof(headerDX10), 1, file);


	int size = GetImageSize();

	// RGB to BGR
	//if (/*m_Format == IMGFMT_RGB8 || */m_Format == IMGFMT_RGBA8)
	//	swapChannels(pixels, size / nChannels, nChannels, 0, 2);

	/*if (isCube())
	{
		for (int face = 0; face < 6; face++)
		{
			for (int mipMapLevel = 0; mipMapLevel < nMipMaps; mipMapLevel++)
			{
				int faceSize = getMipMappedSize(mipMapLevel, 1) / 6;
				ubyte *src = getPixels(mipMapLevel) + face * faceSize;
				fwrite(src, 1, faceSize, file);
			}
		}
	}
	else*/
	{
		fwrite(m_Pixels, size, 1, file);
	}
	fclose(file);

	// Restore to RGB
	//if (/*m_Format == IMGFMT_RGB8 || */m_Format == IMGFMT_RGBA8)
	//	swapChannels(pixels, size / nChannels, nChannels, 0, 2);

	return true;
}


bool Image::GenerateMipMaps(const uint count)
{
	if (IsCompressed(m_Format) || m_Slices != 1)
		return false;

	const uint max_mips = ::GetMipMapCount(max(max(m_Width, m_Height), m_Depth));
	const uint mip_count = min(count, max_mips);

	if (m_MipMapCount != mip_count)
	{
		uint size = ::GetImageSize(m_Format, m_Width, m_Height, m_Depth, mip_count, m_Slices);

		//m_Pixels = (__m128i *) _mm_realloc(m_Pixels, size, 16);

		uint curr_size = ::GetImageSize(m_Format, m_Width, m_Height, m_Depth, 1, m_Slices);

		__m128i *new_pixels = (__m128i *) _mm_malloc(size, 16);
		memcpy(new_pixels, m_Pixels, curr_size);

		_mm_free(m_Pixels);
		m_Pixels = new_pixels;
	}



	__m128i *src = m_Pixels;
	uint w = m_Width;
	uint h = m_Height;
	uint d = m_Depth;


	bool is_half = (IsFloatFormat(m_Format) && GetBytesPerChannel(m_Format));

	__m128i *tmp_row0 = nullptr;
	__m128i *tmp_row1 = nullptr;
	__m128i *tmp_row2 = nullptr;
	if (is_half)
	{
		int size = GetPitch();
		tmp_row0 = (__m128i *) alloca(size * 2);
		tmp_row1 = (__m128i *) alloca(size * 2);
		tmp_row2 = (__m128i *) alloca(size);
	}


	for (uint i = 1; i < mip_count; ++i)
	{
		uint dstW = (w > 1)? (w >> 1) : 1;
		uint dstH = (h > 1)? (h >> 1) : 1;
		uint dstD = (d > 1)? (d >> 1) : 1;

		uint src_chunks = GetChunkCount(m_Format, w);
		uint dst_chunks = GetChunkCount(m_Format, dstW);

		//__m128i *dst = (__m128i *) (((ubyte *) src) + size);

		__m128i *dst = src + h * src_chunks;

		__m128i *dest = dst;
		__m128i *row0 = src;
		__m128i *row1 = src + ((h > 1)? src_chunks : 0);

		for (uint y = 0; y < dstH; ++y)
		{

			if (m_Format <= IMGFMT_RGBA8UI)
			{
				if (m_Format <= IMGFMT_R8UI)
				{
					GenerateMipMapR8(dest, row0, row1, src_chunks);
				}
				else if (m_Format <= IMGFMT_RG8UI)
				{
					GenerateMipMapRG8(dest, row0, row1, src_chunks);
				}
				else/* if (m_Format <= IMGFMT_RGBA8UI)*/
				{
					GenerateMipMapRGBA8(dest, row0, row1, src_chunks);
				}
			}
			else if (m_Format <= IMGFMT_RGBA16F)
			{
				if (is_half)
				{
					Convert16Fto32F(tmp_row0, row0, src_chunks);
					Convert16Fto32F(tmp_row1, row1, src_chunks);
					if (m_Format == IMGFMT_R16F)
						GenerateMipMapR32F(tmp_row2, tmp_row0, tmp_row1, src_chunks * 2);
					else if (m_Format == IMGFMT_RG16F)
						GenerateMipMapRG32F(tmp_row2, tmp_row0, tmp_row1, src_chunks * 2);
					else /*if (m_Format == IMGFMT_RGBA16F)*/
						GenerateMipMapRGBA32F(tmp_row2, tmp_row0, tmp_row1, src_chunks * 2);

					Convert32Fto16F(dest, tmp_row2, src_chunks);
				}
				else if (m_Format <= IMGFMT_R16UI)
				{
					GenerateMipMapR16(dest, row0, row1, src_chunks);
				}
				else if (m_Format <= IMGFMT_RG16UI)
				{
					GenerateMipMapRG16(dest, row0, row1, src_chunks);
				}
				else /*if (m_Format <= IMGFMT_RGBA16UI)*/
				{
					GenerateMipMapRGBA16(dest, row0, row1, src_chunks);
				}

			}
			else if (m_Format <= IMGFMT_RGBA32F)
			{
				if (m_Format == IMGFMT_R32F)
				{
					GenerateMipMapR32F(dest, row0, row1, src_chunks);
				}
				else if (m_Format == IMGFMT_RG32F)
				{
					GenerateMipMapRG32F(dest, row0, row1, src_chunks);
				}
				else if (m_Format == IMGFMT_RGBA32F)
				{
					GenerateMipMapRGBA32F(dest, row0, row1, src_chunks);
				}
				else
				{
					// TODO: Add support for UINT
				}

			}

			dest += dst_chunks;
			row0 += 2 * src_chunks;
			row1 += 2 * src_chunks;
		}

		src = dst;
		w = dstW;
		h = dstH;
		d = dstD;
	}

	m_MipMapCount = mip_count;

	return true;
}

bool Image::Convert(const ImageFormat format)
{
	// TODO: Make real conversion ...
	m_Format = format;

	return true;
}

bool Image::ToGrayScale()
{
	if (m_Format != IMGFMT_RGBA8)
		return false;

	__m128i* src = m_Pixels;
	__m128i* dst = m_Pixels;
	for (uint s = 0; s < m_Slices; ++s)
	{
		for (uint mip = 0; mip < m_MipMapCount; ++mip)
		{
			uint chunks = GetChunkCount(m_Format, GetMipWidth(mip), GetMipHeight(mip)) * GetMipDepth(mip);
			GrayscaleRGBA8(dst, src, chunks);
		
			(ubyte*&) src += ::GetBoxSize(m_Format, GetMipWidth(mip), GetMipHeight(mip), GetMipDepth(mip));
			(ubyte*&) dst += ::GetBoxSize(IMGFMT_R8, GetMipWidth(mip), GetMipHeight(mip), GetMipDepth(mip));
		}
	}

	m_Format = IMGFMT_R8;
	m_Pixels = (__m128i*) _mm_realloc(m_Pixels, GetImageSize(), 16);

	return true;
}

bool Image::ToNormalMap(ImageFormat dest_format, float size_z, float mipmap_scale_z)
{
	if (/*m_Format == IMGFMT_RGB8 || */m_Format == IMGFMT_RGBA8)
	{
		ToGrayScale();
	}

	if (m_Format != IMGFMT_R8)
		return false;
	if (m_Depth > 1 || m_Slices != 1)
		return false;

	uint xMask = 0, yMask = 0, zMask = 0, hMask = 0;
	uint xShift = 0, yShift = 0, zShift = 0, hShift = 0, hFactor = 1;

	switch (dest_format)
	{
	case IMGFMT_RG8:
	//case IMGFMT_RG8S:
		xMask = yMask = 0xFF;
		xShift = 8;
		break;
	/*case IMGFMT_RGB565:
		xMask = zMask = 0x1F;
		yMask = 0x3F;
		xShift = 11;
		yShift = 5;
		break;
	case IMGFMT_RGBA4:
		xMask = yMask = zMask = hMask = 0xF;
		yShift = 4;
		zShift = 8;
		hShift = 12;
		break;*/
	case IMGFMT_RGBA8:
	//case IMGFMT_RGBA8S:
		xMask = yMask = zMask = hMask = 0xFF;
		yShift = 8;
		zShift = 16;
		hShift = 24;
		break;
	/*case IMGFMT_RGB10A2:
		xMask = yMask = zMask = 0x3FF;
		hMask = 0x03;
		yShift = 10;
		zShift = 20;
		hShift = 30;
		break;*/
	case IMGFMT_RGBA16:
	//case IMGFMT_RGBA16S:
		xMask = yMask = zMask = hMask = 0xFFFF;
		yShift = 16;
		zShift = 32;
		hShift = 48;
		hFactor = 257;
		break;
	default:
		return false;
	}

	int bpp = GetBytesPerBlock(dest_format);
	bool use16 = (bpp == 2);
	bool use32 = (bpp == 4);

	float xFactor = 0.5f * xMask;
	float yFactor = 0.5f * yMask;
	float zFactor = 0.5f * zMask;
	float bias = IsSignedFormat(dest_format)? 0.0f : 1.0f;


	// Size of the z component. (48*255 is to normalize for sum of Sobel factors and 0-255 range in bytes)
	float sZ = size_z * 128.0f * (48.0f * 255.0f) / max(m_Width, m_Height);

	__m128i* new_pixels = (__m128i *) _mm_malloc(::GetImageSize(dest_format, m_Width, m_Height, m_Depth, m_MipMapCount, m_Slices), 16);

	union
	{
		ushort *dest16;
		uint32 *dest32;
		uint64 *dest64;
	};
	dest32 = (uint32 *) new_pixels;

	ubyte* src = (ubyte*) m_Pixels;
	for (uint mip = 0; mip < m_MipMapCount; ++mip)
	{
		int w = GetMipWidth(mip);
		int h = GetMipHeight(mip);

		bool is_pot = IsPowerOf2(w);

		for (int y = 0; y < h; y++)
		{
			uint baseY = y + h;
			uint fy0w = ((baseY - 2) % h) * w;
			uint fy1w = ((baseY - 1) % h) * w;
			uint fy2w = y * w;
			uint fy3w = ((baseY + 1) % h) * w;
			uint fy4w = ((baseY + 2) % h) * w;

			for (int x = 0; x < w; x++)
			{
				// Apply a 5x5 Sobel filter, unrolled for performance

				//         SobelX                    SobelY
				// { 1,  2,  0,  -2, -1 }  {  1,  4,   6,  4,  1 }
				// { 4,  8,  0,  -8, -4 }  {  2,  8,  12,  8,  2 }
				// { 6, 12,  0, -12, -6 }  {  0,  0,   0,  0,  0 }
				// { 4,  8,  0,  -8, -4 }  { -2, -8, -12, -8, -2 }
				// { 1,  2,  0,  -2, -1 }  { -1, -4,  -6, -4, -1 }

				uint fx0, fx1, fx3, fx4;
				if (is_pot)
				{
					// Optimized POT path
					fx0 = (x - 2) & (w - 1);
					fx1 = (x - 1) & (w - 1);
					fx3 = (x + 1) & (w - 1);
					fx4 = (x + 2) & (w - 1);
				}
				else
				{
					uint baseX = x + w;
					fx0 = (baseX - 2) % w;
					fx1 = (baseX - 1) % w;
					fx3 = (baseX + 1) % w;
					fx4 = (baseX + 2) % w;
				}

#if 0			// Experimental version, reducing register pressure and stack spill. Didn't affect performance much.

				int s = src[fy0w + fx0];
				int sumX = s;
				int sumY = s;

				s = src[fy0w + fx1];
				sumX += 2 * s;
				sumY += 4 * s;

				s = src[fy0w + x];
				sumY += 6 * s;

				s = src[fy0w + fx3];
				sumX -= 2 * s;
				sumY += 2 * s;

				s = src[fy0w + fx4];
				sumX -= s;
				sumY += s;



				s = src[fy1w + fx0];
				sumX += 4 * s;
				sumY += 2 * s;

				s = src[fy1w + fx1];
				sumX += 8 * s;
				sumY += 8 * s;

				s = src[fy1w + x];
				sumY += 12 * s;

				s = src[fy1w + fx3];
				sumX -= 8 * s;
				sumY += 8 * s;

				s = src[fy1w + fx4];
				sumX -= 4 * s;
				sumY += 2 * s;

				sumX += 6  * src[fy2w + fx0];
				sumX += 12 * src[fy2w + fx1];
				sumX -= 12 * src[fy2w + fx3];
				sumX -= 6  * src[fy2w + fx4];



				s = src[fy3w + fx0];
				sumX += 4 * s;
				sumY -= 2 * s;

				s = src[fy3w + fx1];
				sumX += 8 * s;
				sumY -= 8 * s;

				s = src[fy3w + x];
				sumY -= 12 * s;

				s = src[fy3w + fx3];
				sumX -= 8 * s;
				sumY -= 8 * s;

				s = src[fy3w + fx4];
				sumX -= 4 * s;
				sumY -= 2 * s;


				s = src[fy4w + fx0];
				sumX += s;
				sumY -= s;

				s = src[fy4w + fx1];
				sumX += 2 * s;
				sumY -= 4 * s;

				s = src[fy4w + x];
				sumY -= 6 * s;

				s = src[fy4w + fx3];
				sumX -= 2 * s;
				sumY += 4 * s;

				s = src[fy4w + fx4];
				sumX -= s;
				sumY -= s;
#else

				int s00 = src[fy0w + fx0];
				int s01 = src[fy0w + fx1];
				int s02 = src[fy0w + x];
				int s03 = src[fy0w + fx3];
				int s04 = src[fy0w + fx4];
				int sumX = (s00 + 2 * s01           - 2 * s03 - s04);
				int sumY = (s00 + 4 * s01 + 6 * s02 + 4 * s03 + s04);

				int s10 = src[fy1w + fx0];
				int s11 = src[fy1w + fx1];
				int s12 = src[fy1w + x];
				int s13 = src[fy1w + fx3];
				int s14 = src[fy1w + fx4];
				sumX += (4 * s10 + 8 * s11            - 8 * s13 - 4 * s14);
				sumY += (2 * s10 + 8 * s11 + 12 * s12 + 8 * s13 + 2 * s14);

				int s20 = src[fy2w + fx0];
				int s21 = src[fy2w + fx1];
				int s23 = src[fy2w + fx3];
				int s24 = src[fy2w + fx4];
				sumX += (6 * s20 + 12 * s21 - 12 * s23 - 6 * s24);

				int s30 = src[fy3w + fx0];
				int s31 = src[fy3w + fx1];
				int s32 = src[fy3w + x];
				int s33 = src[fy3w + fx3];
				int s34 = src[fy3w + fx4];
				sumX += (4 * s30 + 8 * s31            - 8 * s33 - 4 * s34);
				sumY -= (2 * s30 + 8 * s31 + 12 * s32 + 8 * s33 + 2 * s34);

				int s40 = src[fy4w + fx0];
				int s41 = src[fy4w + fx1];
				int s42 = src[fy4w + x];
				int s43 = src[fy4w + fx3];
				int s44 = src[fy4w + fx4];
				sumX += (s40 + 2 * s41           - 2 * s43 - s44);
				sumY -= (s40 + 4 * s41 + 6 * s42 + 4 * s43 + s44);
#endif

				// Construct the components
				float sX = float(sumX);// *(1.0f / (48 * 255));
				float sY = float(sumY);// *(1.0f / (48 * 255));

				// Normalize and store
				float invLen = rsqrtf(sX * sX + sY * sY + sZ * sZ);
				float rX = xFactor * (sX * invLen + bias);
				float rY = yFactor * (sY * invLen + bias);
				float rZ = zFactor * (sZ * invLen + bias);

				uint64 result = 0;
				result |= uint64(int(rX) & xMask) << xShift;
				result |= uint64(int(rY) & yMask) << yShift;
				result |= uint64(int(rZ) & zMask) << zShift;
				result |= uint64((src[y * w + x] * hFactor) & hMask) << hShift;

				if (use32)
					*dest32++ = (uint32) result;
				else if (use16)
					*dest16++ = (uint16) result;
				else
					*dest64++ = result;

			}
		}
		sZ *= mipmap_scale_z;

		src += GetMipSliceSize(mip);
	}

	m_Format = dest_format;
	_mm_free(m_Pixels);
	m_Pixels = new_pixels;

	return true;
}
