#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUISeparator.h"
#include "GUIPanel.h"
#include "GUIFrame.h"

GUISeparator::GUISeparator(int orientation_) : GUIRectangle()
{
  setOrientation(orientation);
  setRatio(1.0f);
  widgetType = SEPARATOR;
}

bool GUISeparator::loadXMLSettings(XMLElement *element)
{
  if(!element || (element->getName() != "Separator"))
    {LOG_PRINT_NONE("Need a <Separator> tag");return false;}

  XMLElement *child    = 0;

  if(child = element->getChildByName("ratio"))
    setRatio(child->getValuef());
  else
   {LOG_PRINT_NONE("Need a <ratio> float attribute between 0 and 1");return false;}

  return true;
}

void  GUISeparator::setRatio(float ratio_)
{
  ratio = clampNS(ratio_, 0.1f, 1.0f);
}

float GUISeparator::getRatio()
{
  return ratio;
}

void GUISeparator::setOrientation(int orientation_)
{
  orientation =  orientation_ == HORIZONTAL ? HORIZONTAL : VERTICAL;
}

int  GUISeparator::getOrientation()
{
  return orientation;
}

void GUISeparator::render(float tick)
{
  if(!parent || !((GUIPanel*)parent)->isBGColorOn())
    return;

#if 0
  D3DXFROMWINEVECTOR3 col(0.0f, 1.0f, 0.0f);
#else
  D3DXFROMWINEVECTOR3 col=((GUIPanel*)parent)->getBordersColor();
#endif
  IRenderer::GetRendererInstance()->Color3fv(&col.x);
#if 0//???
  IRenderer::GetRendererInstance()->Begin(PRIM_LINES);
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x, windowBounds.y);
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.z, (orientation == VERTICAL) ? windowBounds.w : windowBounds.y);
  IRenderer::GetRendererInstance()->End();
#endif
  IRenderer::GetRendererInstance()->Color3f(1.0f, 1.0f, 1.0f);
}

void const GUISeparator::computeWindowBounds()
{
  const Tuple4i &parentBounds = parent->getWindowBounds();
  if(orientation == HORIZONTAL)
  {
    float offset = (float(parent->getWidth()) - float(ratio*parent->getWidth()))/2.0f;
    windowBounds.set(int(parentBounds.x + offset), int(parentBounds.y + position.y),
                     int(parentBounds.z - offset), int(parentBounds.y + position.y));
  }
  else
  {
    float offset = (float(parent->getHeight()) - float(parent->getHeight())*ratio)/2.0f;
    windowBounds.set(int(parentBounds.x + position.x), int(parentBounds.y + offset),
                     int(parentBounds.x + position.x), int(parentBounds.w - offset));
  }
}

const Tuple4i &GUISeparator::getWindowBounds()
{
  windowBounds.set(0,0,0,1);
  return windowBounds;
}

void GUISeparator::checkMouseEvents(int extraInfo){}
void GUISeparator::checkKeyboardEvents(int extraInfo){}
