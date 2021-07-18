#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

#include "GUIRectangle.h"
#include "GUIText.h"

class GUIAlphaElement : public GUIRectangle
{

  protected:
    GUIText    label;
    D3DXFROMWINEVECTOR4    color;
    float      alphaFadeScale,
               minAlpha;

    void       modifyCurrentAlpha(float tick);
 public:
    GUIAlphaElement(NSString callback = 0);

    virtual void render(float clockTick) = 0;
    virtual bool loadXMLSettings(XMLElement *node);

    void  setMinAlpha(float minAlpha);
    float getMinAlpha();

    void  setAlphaFadeScale(float scale);
    float getAlphaFadeScale();

    void  setAlpha(float alpha);
    float getAlpha();

    void  setLabelString(const char * label);
    void  setLabelString(const NSString &label);

    NSString   getLabelString();
    void  setColor(const D3DXFROMWINEVECTOR3& color);
    void  setColor(float x, float y, float z);
    const D3DXFROMWINEVECTOR4 &getColor();

    GUIText * getLabel();
};

#endif
