
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

#include "Button.h"

PushButton::PushButton(const float x, const float y, const float w, const float h, const char *text)
{
	SetPosition(x, y);
	SetSize(w, h);

	m_ButtonListener = nullptr;

	m_Text = new char[strlen(text) + 1];
	strcpy(m_Text, text);

	m_Color = float4(0.5f, 0.75f, 1, 1);

	m_Pushed = false;
}

PushButton::~PushButton()
{
	delete [] m_Text;
}

bool PushButton::OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed)
{
	if (pressed == m_Pushed)
		return false;

	if (button == MOUSE_LEFT)
	{
		if (pressed)
		{
			m_Pushed  = true;
			m_Capture = true;
		}
		else
		{
			if (m_ButtonListener && IsInWidget(x, y))
				m_ButtonListener->OnButtonClicked(this);

			m_Pushed  = false;
			m_Capture = false;
		}
	}

	return true;
}

bool PushButton::OnKey(const unsigned int key, const bool pressed)
{
	if (key == KEY_ENTER || key == KEY_SPACE)
	{
		if (m_ButtonListener && m_Pushed && !pressed)
			m_ButtonListener->OnButtonClicked(this);

		m_Pushed = pressed;
		return true;
	}

	return false;
}

void PushButton::Draw(Context context, Primitives &prim, TexFont &font, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/)
{
	ScopeMarker(context, "PushButton");

	DrawButton(context, prim, font, m_Text, linear_clamp, blend_src_alpha/*, depth_state*/);
}

void PushButton::DrawButton(Context context, Primitives &prim, TexFont &font, const char *text, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/)
{
	float4 black(0, 0, 0, 1);

	float4 col = m_Color;
	if (m_Pushed)
		col *= float4(0.5f, 0.5f, 0.5f, 1);

	prim.DrawQuad(context, Primitives::TRANSLUCENT, col, m_Position.x, m_Position.y, m_Position.x + m_Size.x, m_Position.y + m_Size.y, 2);
	prim.DrawRect(context, Primitives::OPAQUE,    black, m_Position.x, m_Position.y, m_Position.x + m_Size.x, m_Position.y + m_Size.y, 2);


	float text_width = 0.75f * m_Size.y;

	float tw = font.GetTextWidth(text);
	float maxW = m_Size.x / tw;
	if (text_width > maxW)
		text_width = maxW;

	float x = 0.5f * (m_Size.x - text_width * tw);

	font.DrawText(context, m_Position.x + x, m_Position.y, text_width, m_Size.y, text/*, linearClamp, blendSrcAlpha, depthState*/);
}

/***********************************************************************************************************/

KeyWaiterButton::KeyWaiterButton(const float x, const float y, const float w, const float h, const char *text, uint *key) : PushButton(x, y, w, h, text)
{
	m_TargetKey = key;
	m_WaitingForKey = false;
	if (*key == 0)
	{
		// Disabled
		m_Color = float4(0.1f, 0.1f, 0.1f, 1);
	}
}

KeyWaiterButton::~KeyWaiterButton()
{
}

bool KeyWaiterButton::OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed)
{
	if (pressed == m_Pushed)
		return false;

	if (button == MOUSE_LEFT)
	{
		if (pressed)
		{
			m_Pushed  = true;
			m_Capture = true;
		}
		else
		{
			if (IsInWidget(x, y))
			{
				if (m_ButtonListener)
					m_ButtonListener->OnButtonClicked(this);

				m_WaitingForKey = true;
			}
			m_Pushed = false;
		}
	}

	return true;
}

bool KeyWaiterButton::OnKey(const unsigned int key, const bool pressed)
{
	if (m_WaitingForKey)
	{
		if (key != KEY_ESCAPE)
		{
			if (key == KEY_DELETE && *m_TargetKey != 0)
			{
				*m_TargetKey = 0;
				m_Color = float4(0.1f, 0.1f, 0.1f, 1);
			}
			else
			{
				*m_TargetKey = key;
				m_Color = float4(0.5f, 0.1f, 1, 1);
			}
		}

		m_WaitingForKey = false;
		m_Capture = false;
		return true;
	}
	else if (key == KEY_SPACE || key == KEY_ENTER)
	{
		if (m_Pushed && !pressed)
		{
			if (m_ButtonListener)
				m_ButtonListener->OnButtonClicked(this);

			m_WaitingForKey = true;
		}

		m_Pushed = pressed;
		return true;
	}

	return false;
}

/*void KeyWaiterButton::draw(Device device, const FontID defaultFont, const SamplerStateID linearClamp, const BlendStateID blendSrcAlpha, const DepthStateID depthState){
	if (m_WaitingForKey)
		drawButton(renderer, "<Press key>", defaultFont, linearClamp, blendSrcAlpha, depthState);
	else
		drawButton(renderer, text, defaultFont, linearClamp, blendSrcAlpha, depthState);
}*/

/***********************************************************************************************************/

AxisWaiterButton::AxisWaiterButton(const float x, const float y, const float w, const float h, const char *text, int *axis, bool *axis_invert) : PushButton(x, y, w, h, text)
{
	m_TargetAxis = axis;
	m_TargetAxisInvert = axis_invert;
	m_WaitingForAxis = false;
	if (*axis < 0)
	{
		// Disabled
		m_Color = float4(0.1f, 0.1f, 0.1f, 1);
	}
}

AxisWaiterButton::~AxisWaiterButton()
{
}

bool AxisWaiterButton::OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed)
{
	if (pressed == m_Pushed)
		return false;

	if (button == MOUSE_LEFT)
	{
		if (pressed)
		{
			m_Pushed  = true;
			m_Capture = true;
		}
		else
		{
			if (IsInWidget(x, y))
			{
				if (m_ButtonListener)
					m_ButtonListener->OnButtonClicked(this);

				m_WaitingForAxis = true;
			}
			m_Pushed = false;
		}
	}

	return true;
}

bool AxisWaiterButton::OnKey(const unsigned int key, const bool pressed)
{
	if (m_WaitingForAxis)
	{
		if (pressed && key == KEY_DELETE)
		{
			*m_TargetAxis = -1;
			*m_TargetAxisInvert = false;
			m_Color = float4(0.1f, 0.1f, 0.1f, 1);

			m_WaitingForAxis = false;
			m_Capture = false;
			return true;
		}
	}
	else if (key == KEY_SPACE || key == KEY_ENTER)
	{
		if (m_Pushed && !pressed)
		{
			if (m_ButtonListener)
				m_ButtonListener->OnButtonClicked(this);

			m_WaitingForAxis = true;
		}

		m_Pushed = pressed;
		return true;
	}

	return false;
}

bool AxisWaiterButton::OnJoystickAxis(const int axis, const float value)
{
	if (m_WaitingForAxis && fabsf(value) > 0.5f)
	{
		*m_TargetAxis = axis;
		*m_TargetAxisInvert = (value < 0.0f);
		m_Color = float4(0.5f, 0.1f, 1, 1);

		m_WaitingForAxis = false;
		m_Capture = false;
		return true;
	}

	return false;
}

/*void AxisWaiterButton::draw(Device device, const FontID defaultFont, const SamplerStateID linearClamp, const BlendStateID blendSrcAlpha, const DepthStateID depthState)
{
	if (m_WaitingForAxis)
		drawButton(renderer, "<Push stick>", defaultFont, linearClamp, blendSrcAlpha, depthState);
	else
		drawButton(renderer, text, defaultFont, linearClamp, blendSrcAlpha, depthState);
}*/

/***********************************************************************************************************/

ButtonWaiterButton::ButtonWaiterButton(const float x, const float y, const float w, const float h, const char *text, int *button) : PushButton(x, y, w, h, text)
{
	m_TargetButton = button;
	m_WaitingForButton = false;
	if (button < 0)
	{
		// Disabled
		m_Color = float4(0.1f, 0.1f, 0.1f, 1);
	}
}

ButtonWaiterButton::~ButtonWaiterButton()
{
}

bool ButtonWaiterButton::OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed)
{
	if (pressed == m_Pushed)
		return false;

	if (button == MOUSE_LEFT)
	{
		if (pressed)
		{
			m_Pushed  = true;
			m_Capture = true;
		}
		else
		{
			if (IsInWidget(x, y))
			{
				if (m_ButtonListener)
					m_ButtonListener->OnButtonClicked(this);

				m_WaitingForButton = true;
			}
			m_Pushed = false;
		}
	}

	return true;
}

bool ButtonWaiterButton::OnKey(const unsigned int key, const bool pressed)
{
	if (m_WaitingForButton)
	{
		if (pressed && key == KEY_DELETE)
		{
			*m_TargetButton = -1;
			m_Color = float4(0.1f, 0.1f, 0.1f, 1);

			m_WaitingForButton = false;
			m_Capture = false;
			return true;
		}
	}
	else if (key == KEY_SPACE || key == KEY_ENTER)
	{
		if (m_Pushed && !pressed)
		{
			if (m_ButtonListener)
				m_ButtonListener->OnButtonClicked(this);

			m_WaitingForButton = true;
		}

		m_Pushed = pressed;
		return true;
	}

	return false;
}

bool ButtonWaiterButton::OnJoystickButton(const int button, const bool pressed)
{
	if (m_WaitingForButton)
	{
		*m_TargetButton = button;
		m_Color = float4(0.5f, 0.1f, 1, 1);

		m_WaitingForButton = false;
		m_Capture = false;
		return true;
	}

	return false;
}

/*void ButtonWaiterButton::draw(Device device, const FontID defaultFont, const SamplerStateID linearClamp, const BlendStateID blendSrcAlpha, const DepthStateID depthState)
{
	if (waitingForButton)
		drawButton(renderer, "<Press button>", defaultFont, linearClamp, blendSrcAlpha, depthState);
	else
		drawButton(renderer, text, defaultFont, linearClamp, blendSrcAlpha, depthState);
}*/
