#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include <string>

#include "GUITexCoordDescriptor.h"
#include "GUIClippedRectangle.h"
#include "GUIMaterialSurface.h"
#include "GUITabbedPanel.h"
#include "GUIRadioButton.h"
#include "GUIFontManager.h"
#include "GUIRectangle.h"
#include "GUISeparator.h"
#include "GUICheckBox.h"
#include "GUIComboBox.h"
#include "GUITextBox.h"
#include "GUISlider.h"
#include "GUIButton.h"
#include "GUIPanel.h"
#include "GUIFrame.h"
#include "GUILabel.h"
#include "GUIText.h"
#include "GUIFont.h"

#include "GUIEventListener.h"

#define NSString std::string

/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	
*/
class STXGUI : public GUIEventListener
{
public:
	GUIFrame* GetGuiFrame();
	STXGUI();
	virtual ~STXGUI();
#ifndef SWIG
	static GUIFrame* guiFrame;
	static GUIPanel* userControls;
	bool mouseLocked;
#endif
	virtual void mouseScrolled     ();
	virtual void mouseExit         ();	
	virtual void update(unsigned int cookie=0);
	virtual void init(std::string afile, std::string aUserControls="User Controls");
	virtual void actionPerformed(GUIEvent &evt)=0;
};

void InitNSGUI(std::string aGUILayout, std::string aUserControls="User Controls");
void UpdateNSGUI(std::string aUserControls="User Controls");
inline void InitNSGUISWIG(const char* aGUILayout)
{
	InitNSGUI(aGUILayout, "User Controls");
}
inline void UpdateNSGUISWIG()
{
	UpdateNSGUI("User Controls");
}
GUIEvent*  PollGUIEvent();
void PushGUIEvent(GUIEvent* evt);
bool isGUIEventsEmpty();
void DisposeGUIEvent(GUIEvent* evt);
bool isClicked(GUIEvent &evt);
GUIRectangle* GetGUIRectangle(GUIEvent &evt);
#endif

