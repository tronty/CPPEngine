#ifndef GUI_EVENTLISTENER_H
#define GUI_EVENTLISTENER_H

#include "../GUI/GUIEvent.h"

class GUIEventListener
{
  public:
    virtual void actionPerformed(GUIEvent &evt) = 0;
};

#endif
