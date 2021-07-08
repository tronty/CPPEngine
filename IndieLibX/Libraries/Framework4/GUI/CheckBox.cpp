
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

#include "CheckBox.h"

CheckBox::CheckBox(const float x, const float y, const float w, const float h, const char *txt, const bool check)
{
	SetPosition(x, y);
	SetSize(w, h);

	m_CheckBoxListener = nullptr;

	m_Text = new char[strlen(txt) + 1];
	strcpy(m_Text, txt);

	m_Color = float4(0.5f, 0.75f, 1, 1);

	m_Checked = check;
}

CheckBox::~CheckBox()
{
	delete [] m_Text;
}

bool CheckBox::OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed)
{
	if (button == MOUSE_LEFT && pressed)
	{
		m_Checked = !m_Checked;
		if (m_CheckBoxListener)
			m_CheckBoxListener->OnCheckBoxClicked(this);
	}

	return true;
}

bool CheckBox::OnKey(const unsigned int key, const bool pressed)
{
	if (key == KEY_ENTER || key == KEY_SPACE)
	{
		if (pressed)
		{
			m_Checked = !m_Checked;
			if (m_CheckBoxListener)
				m_CheckBoxListener->OnCheckBoxClicked(this);
		}
		return true;
	}

	return false;
}

void CheckBox::Draw(Context context, Primitives &prim, TexFont &font, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/)
{
	ScopeMarker(context, "CheckBox");

	float black[] = { 0, 0, 0, 1 };
	float white[] = { 1, 1, 1, 1 };

	if (m_Checked)
	{
		prim.DrawTexQuad(context, Primitives::OPAQUE, m_ResourceTable, linear_clamp, white, m_Position.x, m_Position.y + 0.2f * m_Size.y, m_Position.x + 0.6f * m_Size.y, m_Position.y + 0.8f * m_Size.y, 3);
	}
	else
	{
		prim.DrawQuad(context, Primitives::OPAQUE, white, m_Position.x, m_Position.y + 0.2f * m_Size.y, m_Position.x + 0.6f * m_Size.y, m_Position.y + 0.8f * m_Size.y, 3);
	}


	prim.DrawRect(context, Primitives::OPAQUE, black, m_Position.x, m_Position.y + 0.2f * m_Size.y, m_Position.x + 0.6f * m_Size.y, m_Position.y + 0.8f * m_Size.y, 3);


	float text_width = 0.75f * m_Size.y;

	float w = m_Size.x - 0.7f * m_Size.y;
	float tw = font.GetTextWidth(m_Text);
	float maxW = w / tw;
	if (text_width > maxW)
		text_width = maxW;

	float x = 0.7f * m_Size.y;

	font.DrawText(context, m_Position.x + x, m_Position.y, text_width, m_Size.y, m_Text/*, linearClamp, blendSrcAlpha, depthState*/);
}

Texture CheckBox::m_Texture;
ResourceTable CheckBox::m_ResourceTable;

bool CheckBox::Init(Device device, RootSignature root)
{
	static const uint32 check_pic[] =
	{
		0xFFDFFFFF, 0x00000000, 0xFFDFFFFF, 0x000000000, 0x0000FFFF, 0x50400000, 0x0000FFFF, 0x0595BE00,
		0xFFDFFFFF, 0x00000000, 0x0000FFFF, 0x060C00000, 0x0000FFFF, 0x25955754, 0x18C3FFFF, 0x00000209,
		0x0000FFFF, 0x5757D600, 0x0000FFFF, 0x015D75C50, 0x0000FFFF, 0x00000309, 0xFFDFFFFF, 0x00000000,
		0x0000FFFF, 0x00F05C56, 0x0000FFFF, 0x000030D25, 0xFFDFFFFF, 0x00000000, 0xFFDFFFFF, 0x00000000,
	};

	STextureParams t_params;
	t_params.m_Format = IMGFMT_BC1;
	t_params.m_Width  = 16;
	t_params.m_Height = 16;
	t_params.m_ShaderResource = true;
	t_params.m_Name = "CheckBox";
	m_Texture = CreateTexture(device, t_params);
	if (m_Texture == nullptr)
		return false;

	SetTextureData(GetMainContext(device), m_Texture, 0, 0, (void*) check_pic, sizeof(check_pic), 8 * sizeof(uint32));

	SResourceDesc resources[] = { m_Texture };
	m_ResourceTable = CreateResourceTable(device, root, 2, resources);

	return (m_ResourceTable != nullptr);
}

void CheckBox::Destroy(Device device)
{
	DestroyResourceTable(device, m_ResourceTable);
	DestroyTexture(device, m_Texture);
}
