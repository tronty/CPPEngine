
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

#include "Widget.h"

class PushButton;

class PushButtonListener
{
public:
	virtual ~PushButtonListener(){}

	virtual void OnButtonClicked(PushButton *button) = 0;
};


class PushButton : public Widget
{
public:
	PushButton(const float x, const float y, const float w, const float h, const char *text);
	virtual ~PushButton();

	void SetListener(PushButtonListener *listener){ m_ButtonListener = listener; }

	virtual bool OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed);
	virtual bool OnKey(const unsigned int key, const bool pressed);

	virtual void Draw(Context context, Primitives &prim, TexFont &font, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/);

protected:
	void DrawButton(Context context, Primitives &prim, TexFont &font, const char *text, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/);

	char *m_Text;

	PushButtonListener *m_ButtonListener;

	bool m_Pushed;
};

class KeyWaiterButton : public PushButton
{
public:
	KeyWaiterButton(const float x, const float y, const float w, const float h, const char *text, uint *key);
	virtual ~KeyWaiterButton();

	bool OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed);
	bool OnKey(const unsigned int key, const bool pressed);

//	void draw(Device device, const FontID defaultFont, const SamplerStateID linearClamp, const BlendStateID blendSrcAlpha, const DepthStateID depthState);
protected:
	uint *m_TargetKey;

	bool m_WaitingForKey;
};

class AxisWaiterButton : public PushButton
{
public:
	AxisWaiterButton(const float x, const float y, const float w, const float h, const char *text, int *axis, bool *axis_invert);
	virtual ~AxisWaiterButton();

	bool OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed);
	bool OnKey(const unsigned int key, const bool pressed);
	bool OnJoystickAxis(const int axis, const float value);

//	void draw(Device device, const FontID defaultFont, const SamplerStateID linearClamp, const BlendStateID blendSrcAlpha, const DepthStateID depthState);
protected:
	int *m_TargetAxis;
	bool *m_TargetAxisInvert;

	bool m_WaitingForAxis;
};

class ButtonWaiterButton : public PushButton
{
public:
	ButtonWaiterButton(const float x, const float y, const float w, const float h, const char *text, int *button);
	virtual ~ButtonWaiterButton();

	bool OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed);
	bool OnKey(const unsigned int key, const bool pressed);
	bool OnJoystickButton(const int button, const bool pressed);

//	void draw(Device device, const FontID defaultFont, const SamplerStateID linearClamp, const BlendStateID blendSrcAlpha, const DepthStateID depthState);
protected:
	int *m_TargetButton;

	bool m_WaitingForButton;
};
