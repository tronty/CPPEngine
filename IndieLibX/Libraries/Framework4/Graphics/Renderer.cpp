
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

#include "Renderer.h"
#include "../Util/Array.h"

#include <stdio.h>
#include <stdlib.h>

AspectRatio GetAspectRatio(const int width, const int height)
{
// 	if (width * 3 == height * 4 ) return ASPECT_RATIO_4_3;
// 	if (width * 5 == height * 8 ) return ASPECT_RATIO_16_10;
// 	if (width * 9 == height * 16) return ASPECT_RATIO_16_9;
// 	if (width * 4 == height * 5 ) return ASPECT_RATIO_5_4;
// 	if (width * 3 == height * 5 ) return ASPECT_RATIO_5_3;
// 	if (width * 2 == height * 3 ) return ASPECT_RATIO_3_2;

	// Allow some slack because of odd resolutions such as 1360x768 which is arguably 16:9, but pixel ratio doesn't match exactly
	int slack = 128; // abs(ratio - test_ratio) < 1/128
	if (abs(width * 3 - height * 4 ) < (height * 3) / slack) return ASPECT_RATIO_4_3;
	if (abs(width * 5 - height * 8 ) < (height * 5) / slack) return ASPECT_RATIO_16_10;
	if (abs(width * 9 - height * 16) < (height * 9) / slack) return ASPECT_RATIO_16_9;
	if (abs(width * 4 - height * 5 ) < (height * 4) / slack) return ASPECT_RATIO_5_4;
	if (abs(width * 3 - height * 5 ) < (height * 3) / slack) return ASPECT_RATIO_5_3;
	if (abs(width * 2 - height * 3 ) < (height * 2) / slack) return ASPECT_RATIO_3_2;

	return ASPECT_RATIO_OTHER;
}

const char *GetAspectRatioString(const AspectRatio aspect_ratio)
{
	static const char *strings[ASPECT_RATIO_OTHER + 1] =
	{
		"4:3",
		"16:10",
		"16:9",
		"5:4",
		"5:3",
		"3:2",
		"Misc",
	};
	return strings[aspect_ratio];
}

const uint g_AttribSizes[] =
{
	4,  // FLOAT
	8,  // FLOAT2
	12, // FLOAT3
	16, // FLOAT4

	4,  // HALF2
	8,  // HALF4

	4,  // UINT,
	8,  // UINT2,
	12, // UINT3,
	16, // UINT4,
};

Texture CreateTexture(Device device, const char* filename, uint mipmap_count)
{
	Image image;
	if (!image.Load(filename))
		return nullptr;

	if (mipmap_count)
	{
		uint num_mips = image.GetMaximumMipMapCount();
		num_mips = min(num_mips, mipmap_count);

		if (image.GetMipMapCount() < num_mips)
		{
			if (!image.GenerateMipMaps(num_mips))
				return nullptr;
		}
	}

	return CreateTexture(device, image, filename);
}

Texture CreateTexture(Device device, const char** filenames, uint slices, uint mipmap_count)
{
	STextureParams t_params;
	Texture texture = nullptr;

	Context context = GetMainContext(device);

	for (uint s = 0; s < slices; s++)
	{
		Image image;
		if (!image.Load(filenames[s]))
		{
			DestroyTexture(device, texture);
			return nullptr;
		}

		if (mipmap_count)
		{
			uint num_mips = image.GetMaximumMipMapCount();
			num_mips = min(num_mips, mipmap_count);

			if (image.GetMipMapCount() < num_mips)
			{
				if (!image.GenerateMipMaps(num_mips))
				{
					DestroyTexture(device, texture);
					return nullptr;
				}
			}
		}

		if (s == 0)
		{
			t_params.m_Format         = image.GetFormat();
			t_params.m_Width          = image.GetWidth();
			t_params.m_Height         = image.GetHeight();
			t_params.m_Depth          = image.GetDepth();
			t_params.m_Slices         = slices;
			t_params.m_MipCount       = image.GetMipMapCount();
			t_params.m_ShaderResource = true;
			t_params.m_Name           = filenames[0];
			t_params.m_Type           = (t_params.m_Slices == 6)? TEX_CUBE : TEX_2D;
			texture = CreateTexture(device, t_params);
		}
		else
		{
			ASSERT(t_params.m_Format   == image.GetFormat());
			ASSERT(t_params.m_Width    == image.GetWidth());
			ASSERT(t_params.m_Height   == image.GetHeight());
			ASSERT(t_params.m_Depth    == image.GetDepth());
			ASSERT(t_params.m_MipCount == image.GetMipMapCount());
		}

		uint8* data = (uint8*) image.GetData();
		for (uint m = 0; m < image.GetMipMapCount(); m++)
		{
			uint pitch = image.GetMipPitch(m);
			uint size = image.GetMipSliceSize(m);

			SetTextureData(context, texture, m, s, data, size, pitch);
			data += size;
		}
	}

	return texture;
}

Texture CreateTexture(Device device, const Image& image, const char* filename)
{
	const uint mip_count   = image.GetMipMapCount();
	const uint slice_count = image.GetSlices();

	STextureParams t_params;
	t_params.m_Format         = image.GetFormat();
	t_params.m_Width          = image.GetWidth();
	t_params.m_Height         = image.GetHeight();
	t_params.m_Depth          = image.GetDepth();
	t_params.m_Slices         = slice_count;
	t_params.m_MipCount       = mip_count;
	t_params.m_ShaderResource = true;
	t_params.m_Name           = filename;
	t_params.m_Type           = (t_params.m_Slices == 6)? TEX_CUBE : TEX_2D;
	Texture texture = CreateTexture(device, t_params);

	if (texture)
	{
		Context context = GetMainContext(device);

		uint8* data = (uint8*) image.GetData();
		for (uint s = 0; s < slice_count; s++)
		{
			for (uint m = 0; m < mip_count; m++)
			{
				uint pitch = image.GetMipPitch(m);
				uint size = image.GetMipSliceSize(m);

				SetTextureData(context, texture, m, s, data, size, pitch);
				data += size;
			}
		}
	}

	return texture;
}

void UpdateWindow(Device device, DeviceParams& params, int width, int height)
{
	params.m_Width  = width;
	params.m_Height = height;

	if (!params.m_Fullscreen)
	{
		params.m_WindowedWidth  = width;
		params.m_WindowedHeight = height;
	}

	// Update window text
	wchar_t title[128];
	wsprintf(title, L"%s (%ux%u)", params.m_Title, width, height);
	SetWindowText(GetWindow(device), title);
}

void SetBufferData(Context context, Buffer buffer, const void* data, uint size)
{
	ASSERT(size == GetBufferSize(buffer));

	SMapBufferParams map_params(context, buffer, 0, size);
	uint8* dest = MapBuffer(map_params);
	memcpy(dest, data, size);
	UnmapBuffer(map_params);
}
