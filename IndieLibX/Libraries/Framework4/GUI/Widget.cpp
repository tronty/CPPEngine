
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

#include "Widget.h"

// TextureID Widget::corner = TEXTURE_NONE;
// TextureID Widget::check  = TEXTURE_NONE;

bool Widget::IsInWidget(const int x, const int y) const
{
	float2 p0 = m_Position;
	float2 p1 = m_Position + m_Size;
	return ((x >= p0.x) && (x <= p1.x) && (y >= p0.y) && (y <= p1.y));
}

/*void Widget::drawSoftBorderQuad(Device device, const SamplerStateID linearClamp, const BlendStateID blendSrcAlpha, const DepthStateID depthState, const float x0, const float y0, const float x1, const float y1, const float borderWidth, const float colScale, const float transScale)
{
	if (corner == TEXTURE_NONE)
	{
		ubyte pixels[32][32][4];

		for (int y = 0; y < 32; y++)
		{
			for (int x = 0; x < 32; x++)
			{
				int r = 255 - int(powf(sqrtf(float(x * x + y * y)) * (255.0f / 31.0f), 1.0f));
				if (r < 0) r = 0;
				pixels[y][x][0] = r;
				pixels[y][x][1] = r;
				pixels[y][x][2] = r;
				pixels[y][x][3] = r;
			}
		}

		Image img;
		img.loadFromMemory(pixels, FORMAT_RGBA8, 32, 32, 1, 1, false);
		corner = device->addTexture(img, false, linearClamp);
	}

	float x0bw = x0 + borderWidth;
	float y0bw = y0 + borderWidth;
	float x1bw = x1 - borderWidth;
	float y1bw = y1 - borderWidth;

	TexVertex border[] =
	{
		TexVertex(vec2(x0,   y0bw), vec2(1, 0)),
		TexVertex(vec2(x0,   y0  ), vec2(1, 1)),
		TexVertex(vec2(x0bw, y0bw), vec2(0, 0)),
		TexVertex(vec2(x0bw, y0  ), vec2(0, 1)),
		TexVertex(vec2(x1bw, y0bw), vec2(0, 0)),
		TexVertex(vec2(x1bw, y0  ), vec2(0, 1)),

		TexVertex(vec2(x1bw, y0  ), vec2(0, 1)),
		TexVertex(vec2(x1,   y0  ), vec2(1, 1)),
		TexVertex(vec2(x1bw, y0bw), vec2(0, 0)),
		TexVertex(vec2(x1,   y0bw), vec2(1, 0)),
		TexVertex(vec2(x1bw, y1bw), vec2(0, 0)),
		TexVertex(vec2(x1,   y1bw), vec2(1, 0)),

		TexVertex(vec2(x1,   y1bw), vec2(1, 0)),
		TexVertex(vec2(x1,   y1  ), vec2(1, 1)),
		TexVertex(vec2(x1bw, y1bw), vec2(0, 0)),
		TexVertex(vec2(x1bw, y1  ), vec2(0, 1)),
		TexVertex(vec2(x0bw, y1bw), vec2(0, 0)),
		TexVertex(vec2(x0bw, y1  ), vec2(0, 1)),

		TexVertex(vec2(x0bw, y1  ), vec2(0, 1)),
		TexVertex(vec2(x0,   y1  ), vec2(1, 1)),
		TexVertex(vec2(x0bw, y1bw), vec2(0, 0)),
		TexVertex(vec2(x0,   y1bw), vec2(1, 0)),
		TexVertex(vec2(x0bw, y0bw), vec2(0, 0)),
		TexVertex(vec2(x0,   y0bw), vec2(1, 0)),
	};
	vec4 col = color * vec4(colScale, colScale, colScale, transScale);

	device->drawTextured(PRIM_TRIANGLE_STRIP, border, elementsOf(border), corner, linearClamp, blendSrcAlpha, depthState, &col);

	// Center
	vec2 center[] = { vec2(x0bw, y0bw), vec2(x1bw, y0bw), vec2(x0bw, y1bw), vec2(x1bw, y1bw) };
	device->drawPlain(PRIM_TRIANGLE_STRIP, center, 4, blendSrcAlpha, depthState, &col);
}
*/
