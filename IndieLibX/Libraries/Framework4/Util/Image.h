
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

#include "ImageTools.h"

class Image
{
public:
	Image();
	~Image();

	__m128i* GetData() const { return m_Pixels; }

	uint GetWidth () const { return m_Width;  }
	uint GetHeight() const { return m_Height; }
	uint GetDepth () const { return m_Depth;  }
	uint GetSlices() const { return m_Slices; }
	uint GetMipWidth (const uint mipmap) const { return max(m_Width  >> mipmap, 1U); }
	uint GetMipHeight(const uint mipmap) const { return max(m_Height >> mipmap, 1U); }
	uint GetMipDepth (const uint mipmap) const { return max(m_Depth  >> mipmap, 1U); }
	uint GetMipMapCount() const { return m_MipMapCount; }
	uint GetMaximumMipMapCount() const { return ::GetMipMapCount(max(max(m_Width, m_Height), m_Depth)); }
	ImageFormat GetFormat() const { return m_Format; }

	uint GetPitch() const;
	uint GetSliceSize() const;
	uint GetMipPitch(const uint mipmap) const;
	uint GetMipSliceSize(const uint mipmap) const;
	uint GetImageSize() const;

	void* Create(const ImageFormat format, const uint width, const uint height, const uint depth, const uint slices, const int mipmaps);

	bool Load(const char* filename);
	bool Save(const char* filename);

	bool GenerateMipMaps(const uint count = 127);

	bool Convert(const ImageFormat format);
	bool ToGrayScale();
	bool ToNormalMap(ImageFormat destFormat, float sZ, float mipMapScaleZ);

private:
	__m128i* m_Pixels;
	uint m_Width;
	uint m_Height;
	uint m_Depth;
	uint m_Slices;
	uint m_MipMapCount;
	ImageFormat m_Format;
};
