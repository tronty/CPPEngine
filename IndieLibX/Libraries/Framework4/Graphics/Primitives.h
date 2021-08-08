
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
#include "../Math/Vector.h"

class Primitives
{
public:
	Primitives();
	~Primitives();

	bool CreateResources(Device device, BlendState opaque, BlendState translucent);
	void DestroyResources(Device device);

	void SetMatrix(Context context, const float4x4 &matrix);

	RootSignature GetRootSignature() const { return m_RootSig; }

	enum BLEND_MODE { OPAQUE, TRANSLUCENT };

	void DrawLine(Context context, BLEND_MODE blend_mode, const float* color, const float2 &v0, const float2 &v1);
	void DrawLine(Context context, BLEND_MODE blend_mode, const float* color, const float3 &v0, const float3 &v1);
	void DrawTris(Context context, BLEND_MODE blend_mode, const float* color, const uint count, const float2 *tris);
	void DrawTris(Context context, BLEND_MODE blend_mode, const float* color, const uint count, const float3 *tris);
	void DrawRect(Context context, BLEND_MODE blend_mode, const float* color, const float x0, const float y0, const float x1, const float y1, const float width = 0.0f);
	void DrawQuad(Context context, BLEND_MODE blend_mode, const float* color, const float x0, const float y0, const float x1, const float y1, const float offset = 0.0f);
	void DrawTexQuad(Context context, BLEND_MODE blend_mode, const ResourceTable resources, const SamplerTable samplers, const float *color, const float x0, const float y0, const float x1, const float y1, const float offset = 0.0f);

private:
	struct DrawSetup
	{
		Pipeline m_Pipeline[3/*PRIM_TYPE*/][2/*BLEND_MODE*/];

		VertexSetup VertexSetup;
		Buffer VertexBuffer;
		uint BufferPos;
	};
	DrawSetup m_Setup[3];

	RootSignature m_RootSig;

	enum PRIM_TYPE { LINES, TRIANGLES, TRIANGLE_STRIP };
	void DrawVertices(Context context, DrawSetup& setup, const void* vertices, const uint vertex_count, const uint vertex_stride, PRIM_TYPE primitive_type, BLEND_MODE blend_mode, const float* color, const ResourceTable resources = nullptr, const SamplerTable samplers = nullptr);

	Buffer m_VertexCB;
};
