
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

class Slider;
class SliderListener
{
public:
	virtual ~SliderListener(){}

	virtual void OnSliderChanged(Slider *slider) = 0;
};


class Slider : public Widget
{
public:
	Slider(const float x, const float y, const float w, const float h, const float min_val = 0, const float max_val = 1, const float val = 0);
	virtual ~Slider();

	float GetValue() const { return m_Value; }
	void SetValue(const float val);
	void SetRange(const float min_val, const float max_val);

	void SetListener(SliderListener *listener) { m_SliderListener = listener; }

	bool OnMouseMove(const int x, const int y);
	bool OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed);

	void Draw(Context context, Primitives &prim, TexFont &font, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/);

protected:
	void UpdateValue(const int x);

	char *m_Text;
	float m_MinValue, m_MaxValue, m_Value;

	SliderListener *m_SliderListener;
};
