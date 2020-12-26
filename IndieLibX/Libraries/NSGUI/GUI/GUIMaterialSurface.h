#ifndef GUI_Material_SURFACE_H
#define GUI_Material_SURFACE_H

#include "GUIAlphaElement.h"
#include "GUIClippedRectangle.h"
#include "../Appearance/Appearance.h"

class GUIMaterialSurface : public GUIAlphaElement, public GUIClippedRectangle
{
  public:
    GUIMaterialSurface(NSString callback = 0);

    virtual void render(float clockTick);
    virtual bool loadXMLSettings(XMLElement *node);


    void        bindAppearance(bool applyItorCancelIt);
    Appearance *getAppearance();
  protected:
    Appearance appearance;
};

#endif
