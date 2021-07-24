#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUILabel.h"

GUILabel::GUILabel(NSString ls,
                   NSString cbs) : GUIAlphaElement(cbs)

{
  setDimensions(40, 5);
  setLabelString(ls);
  widgetType = LABEL;
}

bool GUILabel::loadXMLSettings(XMLElement *element)
{
  if(!element || element->getName() != "Label")
    {LOG_PRINT_NONE("Need a Label node in the xml file");return false;}

  XMLElement *child    = 0;

  if(child = element->getChildByName("name"))
    setCallbackString(child->getValuec());

  return   GUIAlphaElement::loadXMLSettings(element);
}

void GUILabel::render(float clockTick)
{
  if(!parent || !visible)
    return;

  Tuple2i center = getCenter();

  if(label.needUpdating())
    parent->forceUpdate(true);

  switch(anchor)
  {
    case CORNERLU: label.print(windowBounds.x, windowBounds.y); break;
    case CORNERLD: label.print(windowBounds.x, windowBounds.y - label.getHeight()); break;
    case CORNERRU: label.print(windowBounds.z, windowBounds.y); break;
    case CORNERRD: label.print(windowBounds.z, windowBounds.y - label.getHeight()); break;
    default: label.printCenteredXY(center.x, center.y);
  }
}

const Tuple4i &GUILabel::getWindowBounds()
{
  if(parent && update)
  {
    int height       = 0,
        width        = 0;
    label.computeDimensions();

    if(dimensions.x > 1)
      width  = dimensions.x  < label.getWidth() ? label.getWidth() : (int)dimensions.x;
    else
      width  = getWidth()    < label.getWidth() ? label.getWidth() : getWidth();

    if(dimensions.x > 1)
      height  = dimensions.y  < label.getHeight() ? label.getHeight() : (int)dimensions.y;
    else
      height  = getHeight()   < label.getHeight() ? label.getHeight() : getHeight();

    dimensions=D3DXFROMWINEVECTOR2(float(width),  float(height));
    GUIRectangle::computeWindowBounds();
  }
  return windowBounds;
}
