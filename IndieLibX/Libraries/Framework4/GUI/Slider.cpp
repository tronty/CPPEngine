
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

#include "Slider.h"

Slider::Slider(const float x, const float y, const float w, const float h, const float min_val, const float max_val, const float val)
{
	SetPosition(x, y);
	SetSize(w, h);

	m_SliderListener = nullptr;

	m_Color = float4(1, 0.2f, 0.2f, 0.65f);

	m_MinValue = min_val;
	m_MaxValue = max_val;
	SetValue(val);
}

Slider::~Slider()
{
}

void Slider::SetValue(const float val)
{
	m_Value = clamp(val, m_MinValue, m_MaxValue);
}

void Slider::SetRange(const float min_val, const float max_val)
{
	m_MinValue = min_val;
	m_MaxValue = max_val;
	m_Value = clamp(m_Value, min_val, max_val);
}

bool Slider::OnMouseMove(const int x, const int y)
{
	if (m_Capture)
	{
		UpdateValue(x);
		return true;
	}
	return false;
}

bool Slider::OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed)
{
	if (button == MOUSE_LEFT)
	{
		if (pressed)
			UpdateValue(x);

		m_Capture = pressed;
	}

	return true;
}

void Slider::Draw(Context context, Primitives &prim, TexFont &font, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/)
{
	ScopeMarker(context, "Slider");

	float black[] = { 0, 0, 0, 1 };

	prim.DrawQuad(context, Primitives::TRANSLUCENT, m_Color, m_Position.x, m_Position.y, m_Position.x + m_Size.x, m_Position.y + m_Size.y, 2);

	prim.DrawRect(context, Primitives::OPAQUE, black, m_Position.x, m_Position.y, m_Position.x + m_Size.x, m_Position.y + m_Size.y, 2);

	prim.DrawQuad(context, Primitives::OPAQUE, black, m_Position.x + 0.5f * m_Size.y, m_Position.y + 0.5f * m_Size.y - 1, m_Position.x + m_Size.x - 0.5f * m_Size.y, m_Position.y + 0.5f * m_Size.y + 1, 0);

	float x = lerp(m_Position.x + 0.5f * m_Size.y, m_Position.x + m_Size.x - 0.5f * m_Size.y, (m_Value - m_MinValue) / (m_MaxValue - m_MinValue));
	prim.DrawQuad(context, Primitives::OPAQUE, black, x - 0.2f * m_Size.y, m_Position.y + 0.2f * m_Size.y, x + 0.2f * m_Size.y, m_Position.y + 0.8f * m_Size.y, 0);
}

void Slider::UpdateValue(const int x)
{
	float t = saturate((x - (m_Position.x + 0.5f * m_Size.y)) / (m_Size.x - m_Size.y));
	m_Value = lerp(m_MinValue, m_MaxValue, t);

	if (m_SliderListener)
		m_SliderListener->OnSliderChanged(this);
}
