
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

#include "Button.h"
#include "../Util/Array.h"
#include "../Util/Queue.h"

#include <new>

struct WInfo
{
	Widget *widget;
	float x, y;
};

struct DialogTab
{
	Queue <WInfo> widgets;
	char *caption;
	float rightX;
};

class Dialog : public Widget, public PushButtonListener
{
public:
	Dialog(const float x, const float y, const float w, const float h, const bool modal, const bool hide_on_close);
	virtual ~Dialog();

	int AddTab(const char *caption);
	void AddWidget(const int tab, Widget *widget, const uint flags = 0);

	void SetCurrentTab(const int tab){ m_CurrTab = tab; }

	bool OnMouseMove(const int x, const int y);
	bool OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed);
	bool OnMouseWheel(const int x, const int y, const int scroll);
	bool OnKey(const unsigned int key, const bool pressed);
	bool OnJoystickAxis(const int axis, const float value);
	bool OnJoystickButton(const int button, const bool pressed);
	void OnButtonClicked(PushButton *button);

	void Draw(Context context, Primitives &prim, TexFont &font, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/);

protected:
	void Close();

	float m_TabHeight;
	float m_BorderWidth;

	Array <DialogTab *> m_Tabs;
	uint m_CurrTab;

	PushButton *m_CloseButton;

	int m_LastX, m_LastY;
	bool m_IsDraging;
	bool m_CloseModeHide;
	bool m_ShowSelection;
	bool m_IsModal;
};
