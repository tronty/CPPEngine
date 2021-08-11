
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
#include "../Util/Array.h"

class DropDownList;
class DropDownListener
{
public:
	virtual ~DropDownListener(){}

	virtual void OnDropDownChanged(DropDownList *list) = 0;
};

class DropDownList : public Widget
{
public:
	DropDownList(const float x, const float y, const float w, const float h);
	virtual ~DropDownList();

	int AddItem(const char *str);
	int AddItemUnique(const char *str);

	void SelectItem(const int item);
	int GetItem(const char *str) const;

	const char *GetText(const int index) const { return m_Items[index]; }
	const char *GetSelectedText() const { return m_Items[m_SelectedItem]; }
	int GetSelectedItem() const { return m_SelectedItem; }

	void Sort();
	void Clear();

	void SetListener(DropDownListener *listener) { m_DropDownListener = listener; }

	bool OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed);
	bool OnMouseWheel(const int x, const int y, const int scroll);
	bool OnKey(const unsigned int key, const bool pressed);
	void OnFocus(const bool focus);

	void Draw(Context context, Primitives &prim, TexFont &font, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/);

protected:
	Array <char *> m_Items;
	int m_SelectedItem;

	DropDownListener *m_DropDownListener;

	bool m_IsDroppedDown;
};
