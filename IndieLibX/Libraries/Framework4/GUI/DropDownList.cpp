
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

#include "DropDownList.h"

DropDownList::DropDownList(const float x, const float y, const float w, const float h)
{
	SetPosition(x, y);
	SetSize(w, h);

	m_Color = float4(1.0f, 1.0f, 0.0f, 0.65f);

	m_DropDownListener = nullptr;

	m_SelectedItem = -1;
	m_IsDroppedDown = false;
}

DropDownList::~DropDownList()
{
	Clear();
}

int DropDownList::AddItem(const char *str)
{
	char *string = new char[strlen(str) + 1];
	strcpy(string, str);

	return m_Items.Add(string);
}

int DropDownList::AddItemUnique(const char *str)
{
	uint count = m_Items.GetCount();
	for (uint i = 0; i < count; i++)
	{
		if (strcmp(str, m_Items[i]) == 0)
			return i;
	}

	return AddItem(str);
}

void DropDownList::SelectItem(const int item)
{
	m_SelectedItem = item;
	if (m_DropDownListener)
		m_DropDownListener->OnDropDownChanged(this);
}

int DropDownList::GetItem(const char *str) const
{
	uint count = m_Items.GetCount();
	for (uint i = 0; i < count; i++)
	{
		if (strcmp(str, m_Items[i]) == 0)
			return i;
	}

	return -1;
}

void DropDownList::Clear()
{
	uint count = m_Items.GetCount();
	for (uint i = 0; i < count; i++)
	{
		delete [] m_Items[i];
	}

	m_Items.Clear();
}

//int comp(char * const &elem0, char * const &elem1)
//{
//	return strcmp(elem0, elem1);
//}

void DropDownList::Sort()
{
	char *curr = nullptr;
	if (m_SelectedItem >= 0) curr = m_Items[m_SelectedItem];

	// TODO: Fix
	//m_Items.Sort(comp);

	if (m_SelectedItem >= 0)
	{
		m_SelectedItem = GetItem(curr);
	}
}

bool DropDownList::OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed)
{
	if (button == MOUSE_LEFT)
	{
		if (pressed)
		{
			if (x > m_Position.x + m_Size.x - m_Size.y && x < m_Position.x + m_Size.x && y > m_Position.y && y < m_Position.y + m_Size.y)
			{
				if (y < m_Position.y + 0.5f * m_Size.y)
				{
					if (m_SelectedItem > 0)
						SelectItem(m_SelectedItem - 1);
				}
				else
				{
					if (m_SelectedItem + 1 < (int) m_Items.GetCount())
						SelectItem(m_SelectedItem + 1);
				}
			}
			else
			{
				if (m_IsDroppedDown)
				{
					int item = int((y - m_Position.y) / m_Size.y + m_SelectedItem);
					if (item >= 0 && item < (int) m_Items.GetCount())
						SelectItem(item);
				}
				m_IsDroppedDown = !m_IsDroppedDown;
			}
		}
		m_Capture = m_IsDroppedDown;

		return true;
	}

	return false;
}

bool DropDownList::OnMouseWheel(const int x, const int y, const int scroll)
{
	m_SelectedItem -= scroll;

	int count = m_Items.GetCount();

	if (m_SelectedItem >= count)
		m_SelectedItem = count - 1;
	else if (m_SelectedItem < 0)
		m_SelectedItem = 0;

	return true;
}

bool DropDownList::OnKey(const unsigned int key, const bool pressed)
{
	if (pressed)
	{
		switch (key)
		{
		case KEY_UP:
			if (m_SelectedItem > 0)
				SelectItem(m_SelectedItem - 1);
			return true;
		case KEY_DOWN:
			if (m_SelectedItem + 1 < (int) m_Items.GetCount())
				SelectItem(m_SelectedItem + 1);
			return true;
		case KEY_ENTER:
		case KEY_SPACE:
			m_IsDroppedDown = !m_IsDroppedDown;
			return true;
		case KEY_ESCAPE:
			if (!m_IsDroppedDown)
				return false;
			m_Capture = m_IsDroppedDown = false;
			return true;
		}
	}

	return false;
}

void DropDownList::OnFocus(const bool focus)
{
	m_Capture = m_IsDroppedDown = false;
}

void DropDownList::Draw(Context context, Primitives &prim, TexFont &font, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/)
{
	ScopeMarker(context, "DropDownList");

	float4 col = m_Color;
	if (!m_Enabled)
	{
		col.x *= 0.5f;
		col.y *= 0.5f;
		col.z *= 0.5f;
		col.w  = 1.0f;
	}

	float black[] = { 0, 0, 0, 1 };

	prim.DrawQuad(context, Primitives::TRANSLUCENT, col, m_Position.x, m_Position.y, m_Position.x + m_Size.x, m_Position.y + m_Size.y, 2);
	prim.DrawRect(context, Primitives::OPAQUE,    black, m_Position.x, m_Position.y, m_Position.x + m_Size.x, m_Position.y + m_Size.y, 2);

	prim.DrawQuad(context, Primitives::OPAQUE, black, m_Position.x + m_Size.x - m_Size.y, m_Position.y + 2, m_Position.x + m_Size.x - m_Size.y + 2, m_Position.y + m_Size.y - 2, 0);
	prim.DrawQuad(context, Primitives::OPAQUE, black, m_Position.x + m_Size.x - m_Size.y + 1, m_Position.y + 0.5f * m_Size.y - 1, m_Position.x + m_Size.x - 2, m_Position.y + 0.5f * m_Size.y + 1, 0);


	float2 triangles[] =
	{
		float2(m_Position.x + m_Size.x - 0.5f * m_Size.y, m_Position.y + 0.1f * m_Size.y),
		float2(m_Position.x + m_Size.x - 0.2f * m_Size.y, m_Position.y + 0.4f * m_Size.y),
		float2(m_Position.x + m_Size.x - 0.8f * m_Size.y, m_Position.y + 0.4f * m_Size.y),
		float2(m_Position.x + m_Size.x - 0.5f * m_Size.y, m_Position.y + 0.9f * m_Size.y),
		float2(m_Position.x + m_Size.x - 0.8f * m_Size.y, m_Position.y + 0.6f * m_Size.y),
		float2(m_Position.x + m_Size.x - 0.2f * m_Size.y, m_Position.y + 0.6f * m_Size.y),
	};
	prim.DrawTris(context, Primitives::OPAQUE, black, elementsof(triangles) / 3, triangles);

	float text_width = 0.75f * m_Size.y;
	float w = m_Size.x - 1.3f * m_Size.y;
	if (m_SelectedItem >= 0)
	{
		float tw = font.GetTextWidth(m_Items[m_SelectedItem]);
		float max_w = w / tw;
		if (text_width > max_w)
			text_width = max_w;

		font.DrawText(context, m_Position.x + 0.15f * m_Size.y, m_Position.y, text_width, m_Size.y, m_Items[m_SelectedItem]/*, linearClamp, blendSrcAlpha, depthState*/);
	}

	if (m_IsDroppedDown)
	{
		prim.DrawQuad(context, Primitives::TRANSLUCENT, col, m_Position.x, m_Position.y - m_SelectedItem * m_Size.y, m_Position.x + m_Size.x - m_Size.y + 2, m_Position.y + (m_Items.GetCount() - m_SelectedItem) * m_Size.y, 2);
		prim.DrawRect(context, Primitives::OPAQUE,    black, m_Position.x, m_Position.y - m_SelectedItem * m_Size.y, m_Position.x + m_Size.x - m_Size.y + 2, m_Position.y + (m_Items.GetCount() - m_SelectedItem) * m_Size.y, 2);

		uint count = m_Items.GetCount();
		for (uint i = 0; i < count; i++)
		{
			float tw = font.GetTextWidth(m_Items[i]);
			float max_w = w / tw;
			if (text_width > max_w)
				text_width = max_w;

			font.DrawText(context, m_Position.x + 0.15f * m_Size.y, m_Position.y + (int(i) - m_SelectedItem) * m_Size.y, text_width, m_Size.y, m_Items[i]/*, linearClamp, blendSrcAlpha, depthState*/);
		}		
	}
}
