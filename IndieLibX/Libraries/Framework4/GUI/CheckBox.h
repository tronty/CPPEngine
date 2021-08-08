
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

class CheckBox;
class CheckBoxListener
{
public:
	virtual ~CheckBoxListener(){}

	virtual void OnCheckBoxClicked(CheckBox *checkBox) = 0;
};


class CheckBox : public Widget
{
public:
	CheckBox(const float x, const float y, const float w, const float h, const char *txt, const bool check = false);
	virtual ~CheckBox();

	void SetListener(CheckBoxListener *listener) { m_CheckBoxListener = listener; }
	
	void SetChecked(const bool ch) { m_Checked = ch; }
	bool IsChecked() const { return m_Checked; }

	bool OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed);
	bool OnKey(const unsigned int key, const bool pressed);

	void Draw(Context context, Primitives &prim, TexFont &font, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/);

	static bool Init(Device device, RootSignature root);
	static void Destroy(Device device);

protected:
	char *m_Text;

	CheckBoxListener *m_CheckBoxListener;

	bool m_Checked;

	static Texture m_Texture;
	static ResourceTable m_ResourceTable;
};
