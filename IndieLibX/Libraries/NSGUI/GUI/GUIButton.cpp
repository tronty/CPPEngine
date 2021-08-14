#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUIButton.h"
#include "GUIAlphaElement.h"
#include "GUIClippedRectangle.h"


GUIButton::GUIButton(NSString cbs) : GUIAlphaElement(cbs), GUIClippedRectangle()

{
  m_Translation=D3DXFROMWINEVECTOR3(0,0,0);
  setBordersColor(0.0f, 0.0f, 0.0f);
  setDimensions(40, 22);
  setPosition(0.5, 0.5);
  setColor(100, 150, 190);

  widgetType     = BUTTON;

  drawBackground = true;
  drawBounds     = true;
  bounce         = true;
}

bool GUIButton::loadXMLSettings(XMLElement *element)
{
  if(!element || element->getName() != "Button")
    {LOG_PRINT_NONE("Need a Button node in the xml file");return false;}

  XMLElement *child    = 0;

  if(child = element->getChildByName("bounce"))
    enableBounce((child->getValue() == "true"));

  return GUIAlphaElement::loadXMLSettings(element) &&
         GUIClippedRectangle::loadXMLClippedRectangleInfo(element);
}

void GUIButton::enableBounce(bool bounce_){ bounce = bounce_; }
bool GUIButton::bounceEnabled()           { return bounce;    }

void GUIButton::render(float clockTick)
{
  if(!parent || !visible)
    return;

  modifyCurrentAlpha(clockTick);
  bgColor = color;

  D3DXFROMWINEVECTOR3 tempColor    = label.getColor();
  float   displacement = 2.0f*(STX_Service::GetInputInstance()->IsMouseButtonPressed())*bounce;
  int     xCenter      = (windowBounds.x + windowBounds.z)/2,
          yCenter      = (windowBounds.y + windowBounds.w)/2;

  m_Translation=D3DXFROMWINEVECTOR3(displacement, displacement, 0.0);
  renderClippedBounds(&m_Translation);
  label.printCenteredXY(xCenter+m_Translation.x, yCenter+m_Translation.y);
  m_Translation=D3DXFROMWINEVECTOR3(-displacement, -displacement, 0.0f);
}

const void GUIButton::computeWindowBounds()
{
  if(parent && update)
  {
    GUIRectangle::computeWindowBounds();
    label.computeDimensions();

    int width  = windowBounds.z - windowBounds.x,
        height = windowBounds.w - windowBounds.y;

    if(width  <= label.getWidth() + 2*clipSize)
    {
      if(anchor == CENTER)
      {
        width = (label.getWidth() - width)/2 + clipSize + 2;
        windowBounds.x -=width;
        windowBounds.z +=width;
      }
      if((anchor == CORNERLU) || (anchor == CORNERLD))
      {
        width = (label.getWidth() - width)/2 + clipSize + 2;
        windowBounds.z +=2*width;
      }
    }

    if(height + 2*clipSize <  label.getHeight())
    {

      height = (label.getHeight() - height)/2 + clipSize + 2;
      windowBounds.y -= height;
      windowBounds.w += height;
    }

    computeClippedBounds(windowBounds);
  }
}
