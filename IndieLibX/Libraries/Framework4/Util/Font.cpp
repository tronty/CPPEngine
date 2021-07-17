
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

#include "Font.h"
#include "Font.pipeline.h"

#include <stdio.h>
#include <string.h>

const ushort MAX_CHARACTERS = 1024;
const uint VB_SIZE = MAX_CHARACTERS * 4;
const uint IB_SIZE = MAX_CHARACTERS * 6;

struct FontVertex
{
	float x, y;
	float u, v;
};

TexFont::TexFont()
{
	memset(&m_Extents, 0, sizeof(m_Extents));
	m_BufferPos = MAX_CHARACTERS; // First Map() call is DISCARD


#define CH(c, x0, x1) m_Extents[ubyte(c)][0] = x0; m_Extents[ubyte(c)][1] = x1

	CH('A', 6, 37);
	CH('B', 46, 73);
	CH('C', 82, 110);
	CH('D', 120, 149);
	CH('E', 160, 184);
	CH('F', 197, 221);
	CH('G', 232, 260);
	CH('H', 270, 298);
	CH('I', 309, 334);
	CH('J', 347, 370);
	CH('K', 384, 411);
	CH('L', 423, 447);
	CH('M', 457, 486);
	CH('N', 495, 523);
	CH('O', 531, 561);
	CH('P', 571, 598);
	CH('Q', 606, 637);
	CH('R', 646, 674);
	CH('S', 683, 710);
	CH('T', 720, 748);
	CH('U', 757, 785);
	CH('V', 793, 825);
	CH('W', 831, 861);
	CH('X', 868, 900);
	CH('Y', 905, 937);
	CH('Z', 945, 972);
//	CH(143, 980, 1012);
//	CH(142, 1018, 1050);
//	CH(153, 1056, 1086);
	CH('Å', 980, 1012);
	CH('Ä', 1018, 1050);
	CH('Ö', 1056, 1086);

	CH('a', 1095, 1121);
	CH('b', 1134, 1161);
	CH('c', 1171, 1196);
	CH('d', 1207, 1234);
	CH('e', 1245, 1272);
	CH('f', 1282, 1310);
	CH('g', 1320, 1348);
	CH('h', 1359, 1384);
	CH('i', 1396, 1422);
	CH('j', 1433, 1457);
	CH('k', 1471, 1499);
	CH('l', 1509, 1534);
	CH('m', 1544, 1573);
	CH('n', 1583, 1609);
	CH('o', 1619, 1648);
	CH('p', 1658, 1685);
	CH('q', 1694, 1721);
	CH('r', 1734, 1760);
	CH('s', 1771, 1796);
	CH('t', 1806, 1833);
	CH('u', 1846, 1871);
	CH('v', 1881, 1912);
	CH('w', 1918, 1949);
	CH('x', 1955, 1986);
	CH('y', 1993, 2024);
	CH('z', 2033, 2058);
//	CH(134, 2070, 2096);
//	CH(132, 2107, 2133);
//	CH(148, 2144, 2172);
	CH('å', 2070, 2096);
	CH('ä', 2107, 2133);
	CH('ö', 2144, 2172);

	CH('0', 2181, 2210);
	CH('1', 2220, 2247);
	CH('2', 2258, 2284);
	CH('3', 2296, 2322);
	CH('4', 2331, 2361);
	CH('5', 2371, 2396);
	CH('6', 2407, 2435);
	CH('7', 2445, 2472);
	CH('8', 2482, 2509);
	CH('9', 2519, 2547);

	CH('@', 2565, 2610);
	CH('!', 2630, 2642);
	CH('"', 2659, 2687);
	CH('#', 2706, 2735);
	CH('$', 2744, 2772);
	CH('%', 2779, 2811);
	CH('&', 2818, 2850);
	CH('/', 2856, 2884);
	CH('{', 2895, 2918);
	CH('}', 2935, 2958);
	CH('(', 2973, 2993);
	CH(')', 3010, 3030);
	CH('[', 3049, 3067);
	CH(']', 3086, 3103);
	CH('=', 3120, 3146);
	CH('?', 3161, 3183);
	CH('+', 3194, 3222);
	CH('\\', 3232, 3260);
	CH('^', 3269, 3297);
	CH('~', 3306, 3335);
	CH('*', 3345, 3371);
	CH('\'', 3385, 3402);

	CH('>', 3422, 3446);
	CH('<', 3457, 3482);
	CH('|', 3503, 3513);
	CH(',', 3534, 3552);
	CH(';', 3572, 3590);
	CH('.', 3613, 3627);
	CH(':', 3651, 3664);
	CH('-', 3685, 3705);
	CH('_', 3717, 3749);

	CH(' ', 3759, 3779);
}

TexFont::~TexFont()
{
}

bool TexFont::LoadFont(const char *font_file)
{
	FILE *file = fopen(font_file, "rb");
	if (file == nullptr)
		return false;

	uint version = 0;
	size_t bytes_read = fread(&version, 1, sizeof(version), file);
	if (bytes_read != sizeof(version) || version != 1)
	{
		fclose(file);
		return false;
	}
	bytes_read = fread(&m_Extents, 1, sizeof(m_Extents), file);
	fclose(file);

	return (bytes_read == sizeof(m_Extents));
}

bool TexFont::SaveFont(const char *font_file) const
{
	FILE *file = fopen(font_file, "wb");
	if (file == nullptr)
		return false;

	uint version = 1;
	fwrite(&version, 1, sizeof(version), file);
	fwrite(&m_Extents, 1, sizeof(m_Extents), file);
	fclose(file);

	return true;
}

bool TexFont::CreateResources(Device device, const char *texture_file, const BlendState blend_state)
{
	if ((m_RootSig = CreateRootSignature(device, NFont::RootSig)) == nullptr) return false;

	const AttribDesc vertex_format[] =
	{
		{ 0, VF_FLOAT2, "Vertex"   },
		{ 0, VF_FLOAT2, "TexCoord" },
	};

	SPipelineParams p_params;
	p_params.m_Name = "Font";
	p_params.m_RootSignature = m_RootSig;
	p_params.m_RenderPass = GetBackBufferRenderPass(device);
	p_params.m_VS = NFont::VertexShader;
	p_params.m_PS = NFont::PixelShader;
	p_params.m_PrimitiveType = PRIM_TRIANGLES;
	p_params.m_Attribs = vertex_format;
	p_params.m_AttribCount = elementsof(vertex_format);
	p_params.m_BlendState = blend_state;
	if ((m_Pipeline = CreatePipeline(device, p_params)) == nullptr) return false;

	if ((m_VertexBuffer = CreateBuffer(device, SBufferParams(sizeof(FontVertex), VB_SIZE, HEAP_UPLOAD, VERTEX_BUFFER, "FontVB"))) == nullptr) return false;
	if ((m_IndexBuffer = CreateBuffer(device, SBufferParams(sizeof(uint16), IB_SIZE, HEAP_DEFAULT, INDEX_BUFFER, "FontIB"))) == nullptr) return false;

	SMapBufferParams map_ib(GetMainContext(device), m_IndexBuffer, 0, IB_SIZE * sizeof(uint16));
	uint16* indices = (uint16*) MapBuffer(map_ib);
	for (ushort i = 0; i < MAX_CHARACTERS; ++i)
	{
		indices[6 * i + 0] = 4 * i;
		indices[6 * i + 1] = 4 * i + 1;
		indices[6 * i + 2] = 4 * i + 2;

		indices[6 * i + 3] = 4 * i + 2;
		indices[6 * i + 4] = 4 * i + 1;
		indices[6 * i + 5] = 4 * i + 3;
	}
	UnmapBuffer(map_ib);


	if ((m_VertexSetup = CreateVertexSetup(device, m_VertexBuffer, sizeof(FontVertex), m_IndexBuffer, sizeof(uint16))) == nullptr) return false;

	if ((m_Texture = CreateTexture(device, texture_file, 5)) == nullptr) return false;

	SResourceDesc resources[] = { m_Texture };
	if ((m_ResourceTable = CreateResourceTable(device, m_RootSig, NFont::Resources, resources)) == nullptr) return false;

	const SSamplerDesc samplers[] = { FILTER_TRILINEAR, 1, AM_CLAMP, AM_CLAMP, AM_CLAMP };
	if ((m_SamplerTable = CreateSamplerTable(device, m_RootSig, NFont::Samplers, samplers)) == nullptr) return false;

	return true;
}

void TexFont::DestroyResources(Device device)
{
	DestroyPipeline(device, m_Pipeline);
	DestroyRootSignature(device, m_RootSig);

	DestroyTexture(device, m_Texture);
	DestroySamplerTable(device, m_SamplerTable);
	DestroyResourceTable(device, m_ResourceTable);

	DestroyBuffer(device, m_VertexBuffer);
	DestroyBuffer(device, m_IndexBuffer);
	DestroyVertexSetup(device, m_VertexSetup);
}

void TexFont::SetWindowSize(Device device, const int width, const int height)
{
	m_Width = width;
	m_Height = height;
}

void TexFont::DrawText(Context context, const float x_pos, const float y_pos, const float size_x, const float size_y, const char *text, uint length)
{
	if (length == ~0U)
		length = (uint) strlen(text);

	if (length == 0)
		return;

	if (m_BufferPos + length <= MAX_CHARACTERS)
	{
		//dest = (FontVertex *) MapBuffer(context, m_VertexBuffer, 4 * m_BufferPos, 4 * length, MAP_WRITE_NO_OVERWRITE);
	}
	else
	{
		//dest = (FontVertex *) MapBuffer(context, m_VertexBuffer, 0, 4 * length, MAP_WRITE_DISCARD);
		m_BufferPos = 0;
	}

	SMapBufferParams map_vb(GetDevice(context), /*context, */m_VertexBuffer, sizeof(FontVertex) * 4 * m_BufferPos, sizeof(FontVertex) * 4 * length);
	FontVertex *dest = (FontVertex*) MapBuffer(map_vb);

	float x = x_pos;
	float y = y_pos;

	for (uint i = 0; i < length; i++)
	{
		ubyte ch = text[i];

		float w = float(m_Extents[ch][1] - m_Extents[ch][0]) / 64.0f;

		//if (ch != ' ')
		{
			float u0 = float(m_Extents[ch][0]) * (1.0f / 4096.0f);
			float u1 = float(m_Extents[ch][1]) * (1.0f / 4096.0f);

			dest[0].x = x;
			dest[0].y = y;
			dest[0].u = u0;
			dest[0].v = 0.0f;

			dest[1].x = x + size_x * w;
			dest[1].y = y;
			dest[1].u = u1;
			dest[1].v = 0.0f;

			dest[2].x = x;
			dest[2].y = y + size_y;
			dest[2].u = u0;
			dest[2].v = 1.0f;

			dest[3].x = x + size_x * w;
			dest[3].y = y + size_y;
			dest[3].u = u1;
			dest[3].v = 1.0f;

			dest += 4;
		}

		x += size_x * w * 1.1f;
	}
	UnmapBuffer(map_vb);


	SetRootSignature(context, m_RootSig);
	SetPipeline(context, m_Pipeline);
	SetGraphicsResourceTable(context, NFont::Resources, m_ResourceTable);
	SetGraphicsSamplerTable(context, NFont::Samplers, m_SamplerTable);

	const float scale_bias[4] = { 2.0f / (float) m_Width, -2.0f / (float) m_Height, -1.0f, 1.0f };
	SetRootConstants(context, NFont::ScaleBias, scale_bias, elementsof(scale_bias));

	//float4* cb = (float4*) SetRootConstantBuffer(context, 0, sizeof(float4));
	//*cb = float4(z, 0, 0, 0);

	SetVertexSetup(context, m_VertexSetup);

	DrawIndexed(context, 6 * m_BufferPos, 6 * length);

	m_BufferPos += length;
}

float TexFont::GetTextWidth(const char *text, uint length) const
{
	if (length == ~0U)
		length = (uint) strlen(text);

	float len = 0;
	for (uint i = 0; i < length; i++)
	{
		uint ch = ((unsigned char *) text)[i];
		float w = float(m_Extents[ch][1] - m_Extents[ch][0]) / 64.0f;

		len += w * 1.1f;
	}

	return len;
}
