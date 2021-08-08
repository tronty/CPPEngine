
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

#include "Dialog.h"

Dialog::Dialog(const float x, const float y, const float w, const float h, const bool modal, const bool hide_on_close)
{
	SetPosition(x, y);
	SetSize(w, h);

	m_BorderWidth = 8;
	m_TabHeight = 32;

	m_Color = float4(0.75f, 1.0f, 0.5f, 0.65f);

	m_IsDraging = false;
	m_CloseModeHide = hide_on_close;
	m_ShowSelection = false;
	m_IsModal = modal;
	m_CurrTab = 0;

	m_CloseButton = new PushButton(x + w - 2 * m_BorderWidth - 24, y + 2 * m_BorderWidth, 24, 24, "x");
	m_CloseButton->SetListener(this);
}

Dialog::~Dialog()
{
	const uint count = m_Tabs.GetCount();
	for (uint i = 0; i < count; i++)
	{
		DialogTab *tab = m_Tabs[i];
		if (tab->widgets.GoToFirst())
		{
			do
			{
				delete tab->widgets.GetCurrent().widget;
			} while (tab->widgets.GoToNext());
		}

		delete [] tab->caption;
		delete tab;
	}

	delete m_CloseButton;
}

int Dialog::AddTab(const char *caption)
{
	DialogTab *tab = new DialogTab;
	tab->caption = new char[strlen(caption) + 1];
	strcpy(tab->caption, caption);

	return m_Tabs.Add(tab);
}

void Dialog::AddWidget(const int tab, Widget *widget, const uint flags)
{
	WInfo wInfo;
	wInfo.widget = widget;
	wInfo.x = widget->GetX();
	wInfo.y = widget->GetY();

	widget->SetPosition(m_Position.x + wInfo.x + 2 * m_BorderWidth, m_Position.y + wInfo.y + 2 * m_BorderWidth + m_TabHeight);
	m_Tabs[tab]->widgets.AddFirst(wInfo);
}

bool Dialog::OnMouseMove(const int x, const int y)
{
	if (m_CurrTab < m_Tabs.GetCount())
	{
		if (m_IsDraging)
		{
			float dx = float(x - m_LastX);
			float dy = float(y - m_LastY);

			m_Position.x += dx;
			m_Position.y += dy;

			for (uint i = 0; i < m_Tabs.GetCount(); i++)
			{
				DialogTab *tab = m_Tabs[i];

				if (tab->widgets.GoToFirst())
				{
					do
					{
						WInfo wi = tab->widgets.GetCurrent();
						wi.widget->SetPosition(m_Position.x + wi.x + 2 * m_BorderWidth, m_Position.y + wi.y + 2 * m_BorderWidth + m_TabHeight);
					} while (tab->widgets.GoToNext());
				}
			}
			m_CloseButton->SetPosition(m_Position.x + m_Size.x - 2 * m_BorderWidth - 24, m_Position.y + 2 * m_BorderWidth);

			m_LastX = x;
			m_LastY = y;
		}
		else
		{
			DialogTab *tab = m_Tabs[m_CurrTab];

			if (tab->widgets.GoToFirst())
			{
				do
				{
					Widget *widget = tab->widgets.GetCurrent().widget;
					if (widget->IsEnabled() && widget->IsVisible() && (widget->IsInWidget(x, y) || widget->IsCapturing()))
					{
						if (widget->OnMouseMove(x, y))
						{
							tab->widgets.MoveCurrentToTop();
							m_Capture = m_IsModal || widget->IsCapturing();
							return true;
						}
					}
				} while (tab->widgets.GoToNext());
			}

		}
	}

	return true;
}

bool Dialog::OnMouseButton(const int x, const int y, const MouseButton button, const bool pressed)
{
	m_ShowSelection = false;

	if (m_CloseButton->IsCapturing() || m_CloseButton->IsInWidget(x, y))
	{
		m_CloseButton->OnMouseButton(x, y, button, pressed);
		m_Capture = true;
		return true;
	}

	if (m_CurrTab < m_Tabs.GetCount())
	{
		DialogTab *tab = m_Tabs[m_CurrTab];

		if (tab->widgets.GoToFirst())
		{
			do
			{
				Widget *widget = tab->widgets.GetCurrent().widget;
				if (widget->IsEnabled() && widget->IsVisible() && (widget->IsInWidget(x, y) || widget->IsCapturing()))
				{
					if (widget->OnMouseButton(x, y, button, pressed))
					{
						tab->widgets.MoveCurrentToTop();
						m_Capture = m_IsModal || widget->IsCapturing();
						return true;
					}
				}
			} while (tab->widgets.GoToNext());
		}
	}

	if (button == MOUSE_LEFT)
	{
		m_Capture = m_IsModal || pressed;
		if (IsInWidget(x, y))
		{
			m_IsDraging = pressed;
			m_LastX = x;
			m_LastY = y;

			if (pressed)
			{
				if (x > m_Position.x + 2 * m_BorderWidth && y > m_Position.y + 2 * m_BorderWidth && y < m_Position.y + 2 * m_BorderWidth + m_TabHeight)
				{
					for (uint i = 0; i < m_Tabs.GetCount(); i++)
					{
						if (x < m_Tabs[i]->rightX)
						{
							m_CurrTab = i;
							m_IsDraging = false;
							break;
						}
					}
				}
			}
		}
	}

	return true;
}

bool Dialog::OnMouseWheel(const int x, const int y, const int scroll)
{
	if (m_CurrTab < m_Tabs.GetCount())
	{
		DialogTab *tab = m_Tabs[m_CurrTab];

		if (tab->widgets.GoToFirst())
		{
			do
			{
				Widget *widget = tab->widgets.GetCurrent().widget;
				if (widget->IsEnabled() && widget->IsVisible() && (widget->IsInWidget(x, y) || widget->IsCapturing()))
				{
					if (widget->OnMouseWheel(x, y, scroll))
					{
						tab->widgets.MoveCurrentToTop();
						m_Capture = m_IsModal || widget->IsCapturing();
						return true;
					}
				}
			} while (tab->widgets.GoToNext());
		}
	}

	return true;
}

bool Dialog::OnKey(const unsigned int key, const bool pressed)
{
	if (m_CurrTab < m_Tabs.GetCount())
	{
		DialogTab *tab = m_Tabs[m_CurrTab];

		if (tab->widgets.GoToFirst())
		{
			if (tab->widgets.GetCurrent().widget->OnKey(key, pressed))
				return true;
		}
		if (pressed)
		{
			if (key == KEY_ESCAPE)
			{
				Close();
				return true;
			}
			else if (key == KEY_TAB)
			{
				if (tab->widgets.GoToFirst())
				{
					Widget *currTop = tab->widgets.GetCurrent().widget;

					tab->widgets.GoToLast();
					do
					{
						Widget *widget = tab->widgets.GetCurrent().widget;
						if (widget->IsEnabled())
						{
							tab->widgets.MoveCurrentToTop();

							currTop->OnFocus(false);
							widget->OnFocus(true);

							m_ShowSelection = true;
							break;
						}
					} while (tab->widgets.GoToPrev());
				}
				return true;
			}
			m_ShowSelection = false;
		}
	}

	return false;
}

bool Dialog::OnJoystickAxis(const int axis, const float value)
{
	if (m_CurrTab < m_Tabs.GetCount())
	{
		DialogTab *tab = m_Tabs[m_CurrTab];

		if (tab->widgets.GoToFirst())
		{
			if (tab->widgets.GetCurrent().widget->OnJoystickAxis(axis, value))
				return true;
		}
	}

	return false;
}

bool Dialog::OnJoystickButton(const int button, const bool pressed)
{
	if (m_CurrTab < m_Tabs.GetCount())
	{
		DialogTab *tab = m_Tabs[m_CurrTab];

		if (tab->widgets.GoToFirst())
		{
			if (tab->widgets.GetCurrent().widget->OnJoystickButton(button, pressed))
				return true;
		}
	}

	return false;
}

void Dialog::OnButtonClicked(PushButton *button)
{
	Close();
}

void Dialog::Draw(Context context, Primitives &prim, TexFont &font, const SamplerTable linear_clamp, const BlendState blend_src_alpha/*, const DepthState depth_state*/)
{
	ScopeMarker(context, "Dialog");

	{
		ScopeMarker(context, "Background");
		float4 black(0, 0, 0, 1);
		float4 blue(0.3f, 0.4f, 1.0f, 0.65f);

		prim.DrawQuad(context, Primitives::TRANSLUCENT, m_Color, m_Position.x, m_Position.y, m_Position.x + m_Size.x, m_Position.y + m_Size.y, 2);
		prim.DrawRect(context, Primitives::OPAQUE,        black, m_Position.x, m_Position.y, m_Position.x + m_Size.x, m_Position.y + m_Size.y, 2);


		float x = m_Position.x + 2 * m_BorderWidth;
		float y = m_Position.y + 2 * m_BorderWidth;

		const float tab_width = 0.75f * m_TabHeight;

		const uint count = m_Tabs.GetCount();
		for (uint i = 0; i < count; i++)
		{
			float cw = font.GetTextWidth(m_Tabs[i]->caption);
			float new_x = x + tab_width * cw + 10;

			if (i == m_CurrTab)
			{
				prim.DrawQuad(context, Primitives::TRANSLUCENT, blue, x, y, new_x, y + m_TabHeight, 2);
			}

			prim.DrawRect(context, Primitives::OPAQUE, black, x, y, new_x, y + m_TabHeight, 2);

			font.DrawText(context, x + 5, y, tab_width, m_TabHeight, m_Tabs[i]->caption/*, linearClamp, blendSrcAlpha, depthState*/);

			m_Tabs[i]->rightX = x = new_x;
		}

		prim.DrawQuad(context, Primitives::OPAQUE, black, m_Position.x + 2 * m_BorderWidth, y + m_TabHeight - 1, m_Position.x + m_Size.x - 2 * m_BorderWidth, y + m_TabHeight + 1, 0);
	}

	{
		ScopeMarker(context, "Widgets");
		m_CloseButton->Draw(context, prim, font, linear_clamp, blend_src_alpha/*, depth_state*/);

		if (m_CurrTab < m_Tabs.GetCount())
		{
			DialogTab *tab = m_Tabs[m_CurrTab];

			if (tab->widgets.GoToLast())
			{
				do
				{
					Widget *widget = tab->widgets.GetCurrent().widget;
					if (widget->IsVisible())
						widget->Draw(context, prim, font, linear_clamp, blend_src_alpha/*, depth_state*/);
				} while (tab->widgets.GoToPrev());
			}
			if (m_ShowSelection)
			{
				if (tab->widgets.GoToFirst())
				{
	/*				Widget *w = tab->widgets.GetCurrent().widget;

					float x = w->GetX();
					float y = w->GetY();
					float2 rect[] = { MAKERECT(x - 5, y - 5, x + w->GetWidth() + 5, y + w->GetHeight() + 5, 1) };
					device->drawPlain(PRIM_TRIANGLE_STRIP, rect, elementsOf(rect), BS_NONE, depthState, &black);
	*/
				}
			}
		}
	}
}

void Dialog::Close()
{
	if (m_CloseModeHide)
	{
		m_Visible = false;
		m_ShowSelection = false;
	}
	else
	{
		m_IsDead = true;
	}
	m_Capture = false;
}
