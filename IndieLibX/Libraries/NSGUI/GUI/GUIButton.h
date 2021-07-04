#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "GUIAlphaElement.h"
#include "GUIClippedRectangle.h"

class GUIButton : public GUIAlphaElement, public GUIClippedRectangle
{
  public:
	  D3DXFROMWINEVECTOR3 m_Translation;
    GUIButton(std::string callbackString = "");

    virtual void  render(float clockTick);
    virtual bool  loadXMLSettings(XMLElement *node);
    virtual const void computeWindowBounds();

    void enableBounce(bool bounce);
    bool bounceEnabled();

  private:
    bool bounce;
};

#endif
