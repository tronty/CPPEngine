#ifndef GUI_RADIO_BUTTON_H
#define GUI_RADIO_BUTTON_H

#include "GUICheckBox.h"

class GUIRadioButton : public GUICheckBox
{
  public:
    GUIRadioButton(std::string callbackString = "");
    virtual void checkMouseEvents(int extraInfo, bool bits);
	bool  isChecked(){return GUICheckBox::isChecked();}
};

#endif
