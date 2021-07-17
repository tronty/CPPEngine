
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

#include "../Util/Font.h"
#include "../Graphics/Primitives.h"
#include "../Math/Vector.h"

class Widget
{
public:
	void* operator new(size_t size)
	{
		return _aligned_malloc(size, 16);
	}
	void operator delete(void *ptr)
	{
		_aligned_free(ptr);
	}

	Widget()
	{
		m_Enabled = true;
		m_Visible = true;
		m_Capture = false;
		m_IsDead = false;
	}
	virtual ~Widget(){}

	virtual bool IsInWidget(const int x, const int y) const;

	virtual bool OnMouseMove(const int x, const int y) { return false; }
	virtual bool OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed) { return false; }
	virtual bool OnMouseWheel(const int x, const int y, const int scroll) { return false; }
	virtual bool OnKey(const unsigned int key, const bool pressed) { return false; }
	virtual bool OnJoystickAxis(const int axis, const float value) { return false; }
	virtual bool OnJoystickButton(const int button, const bool pressed) { return false; }
	virtual void OnSize(const int w, const int h) {}
	virtual void OnFocus(const bool focus){}

	virtual void Draw(Context context, Primitives &prim, TexFont &font, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/) = 0;

	void SetPosition(const float2 &position) { m_Position = position; }
	void SetPosition(const float x, const float y) { m_Position.x = x; m_Position.y = y; }
	void SetSize(const float2 &size) { m_Size = size; }
	void SetSize(const float w, const float h) { m_Size.x = w; m_Size.y = h; }
	float2 GetPosition() const { return m_Position; }
	float2 GetSize() const { return m_Size; }
	float GetX() const { return m_Position.x; }
	float GetY() const { return m_Position.y; }
	float GetWidth()  const { return m_Size.x; }
	float GetHeight() const { return m_Size.y; }
	void SetColor(const float4 &color) { m_Color = color; }
//	void SetCapturing(const bool capturing) { m_Capture = capturing; }
	void SetVisible(const bool visible) { m_Visible = visible; }
	void SetEnabled(const bool enabled) { m_Enabled = enabled; }

	bool IsEnabled() const { return m_Enabled; }
	bool IsVisible() const { return m_Visible; }
	bool IsCapturing() const { return m_Capture; }
	bool IsDead() const { return m_IsDead; }

	//static void clean(){ corner = check = TEXTURE_NONE; }

protected:
	//static TextureID corner, check;
	//void drawSoftBorderQuad(Device device, const SamplerStateID linearClamp, const BlendStateID blendSrcAlpha, const DepthStateID depthState, const float x0, const float y0, const float x1, const float y1, const float borderWidth, const float colScale = 1, const float transScale = 1);

	float4 m_Color;
	float2 m_Position;
	float2 m_Size;

	bool m_Enabled;
	bool m_Visible;
	bool m_Capture;
	bool m_IsDead;
};
