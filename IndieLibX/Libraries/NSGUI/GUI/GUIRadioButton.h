#ifndef GUI_RADIO_BUTTON_H
#define GUI_RADIO_BUTTON_H

#include "GUICheckBox.h"

class GUIRadioButton : public GUICheckBox
{
  public:
    GUIRadioButton(NSString callbackString = 0);
    virtual void checkMouseEvents(int extraInfo, bool bits);
	bool  isChecked(){return GUICheckBox::isChecked();}
};

#endif
