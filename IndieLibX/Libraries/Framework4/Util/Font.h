
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

#include "../Graphics/Renderer.h"

class TexFont
{
public:
	TexFont();
	~TexFont();

	bool LoadFont(const char *font_file);
	bool SaveFont(const char *font_file) const;

	bool CreateResources(Device device, const char *texture_file, const BlendState blend_state);
	void DestroyResources(Device device);

	void SetWindowSize(Device device, const int width, const int height);

	void DrawText(Context context, const float x_pos, const float y_pos, const float size_x, const float size_y, const char *text, uint length = ~0);

	float GetTextWidth(const char *text, uint length = ~0) const;

private:
	int m_Extents[256][2];

	VertexSetup m_VertexSetup;
	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
	uint m_BufferPos;

	Texture m_Texture;

	RootSignature m_RootSig;
	Pipeline m_Pipeline;
	ResourceTable m_ResourceTable;
	SamplerTable m_SamplerTable;

	int m_Width, m_Height;
};
