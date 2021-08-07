
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

#include "Primitives.h"
#include "Primitives.pipeline.h"

const uint VB_SIZE = 16*1024;

Primitives::Primitives()
{
	m_Setup[0].BufferPos = 0;
	m_Setup[1].BufferPos = 0;
	m_Setup[2].BufferPos = 0;
}

Primitives::~Primitives()
{
}

bool Primitives::CreateResources(Device device, BlendState opaque, BlendState translucent)
{
	const AttribDesc format0[] =
	{
		{ 0, VF_FLOAT2, "Vertex"   },
		{ 0, VF_FLOAT2, "TexCoord" },
	};
	const AttribDesc format1[] =
	{
		{ 0, VF_FLOAT3, "Vertex"   },
		{ 0, VF_FLOAT2, "TexCoord" },
	};

	if ((m_Setup[0].VertexBuffer = CreateBuffer(device, SBufferParams(sizeof(float) * 2, VB_SIZE, HEAP_UPLOAD, VERTEX_BUFFER, "Primitives2" ))) == nullptr) return false;
	if ((m_Setup[1].VertexBuffer = CreateBuffer(device, SBufferParams(sizeof(float) * 4, VB_SIZE, HEAP_UPLOAD, VERTEX_BUFFER, "Primitives22"))) == nullptr) return false;
	if ((m_Setup[2].VertexBuffer = CreateBuffer(device, SBufferParams(sizeof(float) * 3, VB_SIZE, HEAP_UPLOAD, VERTEX_BUFFER, "Primitives3" ))) == nullptr) return false;

	if ((m_Setup[0].VertexSetup = CreateVertexSetup(device, m_Setup[0].VertexBuffer, sizeof(float) * 2)) == nullptr) return false;
	if ((m_Setup[1].VertexSetup = CreateVertexSetup(device, m_Setup[1].VertexBuffer, sizeof(float) * 4)) == nullptr) return false;
	if ((m_Setup[2].VertexSetup = CreateVertexSetup(device, m_Setup[2].VertexBuffer, sizeof(float) * 3)) == nullptr) return false;

	if ((m_RootSig = CreateRootSignature(device, NPrimitives::RootSig)) == nullptr) return false;

	SPipelineParams p_params;
	p_params.m_Name = "Primitives";
	p_params.m_RootSignature = m_RootSig;
	p_params.m_RenderPass = GetBackBufferRenderPass(device);

	for (int i = 0; i < 3; i++)
	{
		if ((i & 1) == 0)
		{
			p_params.m_VS = NPrimitives::ColoredVS;
			p_params.m_PS = NPrimitives::ColoredPS;
		}
		else
		{
			p_params.m_VS = NPrimitives::TexturedVS;
			p_params.m_PS = NPrimitives::TexturedPS;
		}

		p_params.m_Attribs = (i < 2)? format0 : format1;
		p_params.m_AttribCount = (i & 1)? 2 : 1;

		static const PrimitiveType prim_types[3] = { PRIM_LINES, PRIM_TRIANGLES, PRIM_TRIANGLESTRIP };

		for (int p = 0; p < 3; p++)
		{
			p_params.m_PrimitiveType = prim_types[p];

			p_params.m_BlendState = opaque;
			m_Setup[i].m_Pipeline[p][0] = CreatePipeline(device, p_params);
			p_params.m_BlendState = translucent;
			m_Setup[i].m_Pipeline[p][1] = CreatePipeline(device, p_params);
		}
	}

	if ((m_VertexCB = CreateBuffer(device, SBufferParams(4 * sizeof(float4), HEAP_DEFAULT, CONSTANT_BUFFER, "PrimitivesVertexCB"))) == nullptr) return false;

	return true;
}

void Primitives::DestroyResources(Device device)
{
	for (uint i = 0; i < elementsof(m_Setup); i++)
	{
		for (int p = 0; p < 3; p++)
		{
			DestroyPipeline(device, m_Setup[i].m_Pipeline[p][0]);
			DestroyPipeline(device, m_Setup[i].m_Pipeline[p][1]);
		}

		DestroyVertexSetup(device, m_Setup[i].VertexSetup);

		DestroyBuffer(device, m_Setup[i].VertexBuffer);
	}

	DestroyBuffer(device, m_VertexCB);
	DestroyRootSignature(device, m_RootSig);
}

void Primitives::SetMatrix(Context context, const float4x4 &matrix)
{
	SMapBufferParams map_cb(context, m_VertexCB, 0, sizeof(float4x4));
	void* data = MapBuffer(map_cb);
	memcpy(data, &matrix, sizeof(float4x4));
	UnmapBuffer(map_cb);
}

void Primitives::DrawLine(Context context, BLEND_MODE blend_mode, const float* color, const float2 &v0, const float2 &v1)
{
	const float2 vertices[] = { v0, v1 };

	DrawVertices(context, m_Setup[0], vertices, 2, sizeof(float2), LINES, blend_mode, color);
}

void Primitives::DrawLine(Context context, BLEND_MODE blend_mode, const float* color, const float3 &v0, const float3 &v1)
{
	const float3 vertices[] = { v0, v1 };

	DrawVertices(context, m_Setup[2], vertices, 2, sizeof(float3), LINES, blend_mode, color);
}

void Primitives::DrawTris(Context context, BLEND_MODE blend_mode, const float* color, const uint count, const float2 *tris)
{
	DrawVertices(context, m_Setup[0], tris, count * 3, sizeof(float2), TRIANGLES, blend_mode, color);
}

void Primitives::DrawTris(Context context, BLEND_MODE blend_mode, const float* color, const uint count, const float3 *tris)
{
	DrawVertices(context, m_Setup[2], tris, count * 3, sizeof(float3), TRIANGLES, blend_mode, color);
}

void Primitives::DrawRect(Context context, BLEND_MODE blend_mode, const float* color, const float x0, const float y0, const float x1, const float y1, const float width)
{
	const float2 vertices[] 
	{
		float2(x0, y0),
		float2(x0 + width, y0 + width),
		float2(x1, y0),
		float2(x1 - width, y0 + width),
		float2(x1, y1),
		float2(x1 - width, y1 - width),
		float2(x0, y1),
		float2(x0 + width, y1 - width),
		float2(x0, y0),
		float2(x0 + width, y0 + width),
	};

	DrawVertices(context, m_Setup[0], vertices, elementsof(vertices), sizeof(float2), TRIANGLE_STRIP, blend_mode, color);
}

void Primitives::DrawQuad(Context context, BLEND_MODE blend_mode, const float* color, const float x0, const float y0, const float x1, const float y1, const float offset)
{
	const float2 vertices[] 
	{
		float2(x0 + offset, y0 + offset),
		float2(x0 + offset, y1 - offset),
		float2(x1 - offset, y0 + offset),
		float2(x1 - offset, y1 - offset),
	};

	DrawVertices(context, m_Setup[0], vertices, elementsof(vertices), sizeof(float2), TRIANGLE_STRIP, blend_mode, color);
}

void Primitives::DrawTexQuad(Context context, BLEND_MODE blend_mode, const ResourceTable resources, const SamplerTable samplers, const float* color, const float x0, const float y0, const float x1, const float y1, const float offset)
{
	struct Vertex
	{
		float2 Position;
		float2 TexCoord;
	};

	const Vertex vertices[] 
	{
		{ float2(x0 + offset, y0 + offset), float2(0, 0) },
		{ float2(x0 + offset, y1 - offset), float2(0, 1) },
		{ float2(x1 - offset, y0 + offset), float2(1, 0) },
		{ float2(x1 - offset, y1 - offset), float2(1, 1) },
	};
	DrawVertices(context, m_Setup[1], vertices, elementsof(vertices), sizeof(Vertex), TRIANGLE_STRIP, blend_mode, color, resources, samplers);
}

void Primitives::DrawVertices(Context context, DrawSetup& setup, const void* vertices, const uint vertex_count, const uint vertex_stride, PRIM_TYPE primitive_type, BLEND_MODE blend_mode, const float* color, const ResourceTable resources, const SamplerTable samplers)
{
	if (setup.BufferPos + vertex_count >= VB_SIZE)
		setup.BufferPos = 0;

	SMapBufferParams map_vb(context, setup.VertexBuffer, setup.BufferPos * vertex_stride, vertex_count * vertex_stride);
	void* data = MapBuffer(map_vb);
	memcpy(data, vertices, vertex_count * vertex_stride);
	UnmapBuffer(map_vb);


	SetRootSignature(context, m_RootSig);
	SetRootConstants(context, NPrimitives::Color, color, 4);
	SetGraphicsConstantBuffer(context, NPrimitives::Constants, m_VertexCB);
	if (resources)
		SetGraphicsResourceTable(context, NPrimitives::Resources, resources);
	if (samplers)
		SetGraphicsSamplerTable(context, NPrimitives::Samplers, samplers);

	SetPipeline(context, setup.m_Pipeline[primitive_type][blend_mode]);

	SetVertexSetup(context, setup.VertexSetup);

	Draw(context, setup.BufferPos, vertex_count);


	setup.BufferPos += vertex_count;
}

