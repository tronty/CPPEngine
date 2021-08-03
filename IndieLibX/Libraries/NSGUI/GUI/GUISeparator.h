#ifndef GUI_SEPARATOR_H
#define GUI_SEPARATOR_H

#include "GUIRectangle.h"

enum
{
  HORIZONTAL,
  VERTICAL
};

class GUISeparator : public GUIRectangle
{
  public:
    GUISeparator(int orientation = HORIZONTAL);
    virtual bool loadXMLSettings(XMLElement *node);
    virtual void checkMouseEvents(int extraInfo);
    virtual void checkKeyboardEvents(int extraInfo);
    virtual void render(float);

    virtual const Tuple4i &getWindowBounds();
    virtual const void     computeWindowBounds();

    void    setOrientation(int orientation);
    int     getOrientation();

    void    setRatio(float ratio);
    float   getRatio();
  private:
    int   orientation;
    float ratio;
};

#endif
