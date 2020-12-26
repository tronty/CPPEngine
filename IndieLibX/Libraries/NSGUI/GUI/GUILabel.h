#ifndef GUI_LABEL_H
#define GUI_LABEL_H

#include "GUIAlphaElement.h"

class GUILabel : public GUIAlphaElement
{
  public:
    GUILabel(NSString labelString    = 0,
             NSString callbackString = 0);

    virtual void  render(float clockTick);
    virtual bool  loadXMLSettings(XMLElement *node);
    virtual const Tuple4i &getWindowBounds();
};

#endif
