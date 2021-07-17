#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUIAlphaElement.h"

GUIAlphaElement::GUIAlphaElement(NSString cbs) :  GUIRectangle(cbs)
{
  setColor(225.0f, 225.0f, 225.0f);
  alphaFadeScale =     1.0f;
  minAlpha       =     0.6f;
  color.w        = minAlpha;
}

bool GUIAlphaElement::loadXMLSettings(XMLElement *element)
{
  if(!element)
    return false;

  XMLElement *child    = 0;

  if(child = element->getChildByName("alphaFadeScale"))
    setAlphaFadeScale(child->getValuef());

  if(child = element->getChildByName("minAlpha"))
    setMinAlpha(child->getValuef());

  if(child = element->getChildByName("Text"))
    label.loadXMLSettings(child);

  if(child = element->getChildByName("Color"))
    XMLElement::loadRX_GY_BZ_AWf(*child, color);

  setColor(color.x, color.y, color.z);
  return   GUIRectangle::loadXMLSettings(element);
}

void  GUIAlphaElement::setColor(const D3DXFROMWINEVECTOR3& color)
{
  setColor(color.x, color.y, color.z);
}

void GUIAlphaElement::setColor(float r, float g, float b)
{
  color=D3DXFROMWINEVECTOR4(clampNS(r, 0.0f, 255.0f),
            clampNS(g, 0.0f, 255.0f),
            clampNS(b, 0.0f, 255.0f),
            clampNS(color.w, 0.0f, 1.0f));

  color.x /= (color.x > 1.0) ? 255.0f : 1.0f;
  color.y /= (color.y > 1.0) ? 255.0f : 1.0f;
  color.z /= (color.z > 1.0) ? 255.0f : 1.0f;
}

const D3DXFROMWINEVECTOR4 &GUIAlphaElement::getColor()
{
  return color;
}

void  GUIAlphaElement::setLabelString(const char * labelArg)
{
  if(!labelArg)
    return;

  label = labelArg;
}

void  GUIAlphaElement::setLabelString(const NSString &labelArg)
{
  label = labelArg;
}

GUIText * GUIAlphaElement::getLabel()
{
  return &label;
}

NSString GUIAlphaElement::getLabelString()
{
  return label.getString().getBytes();
}

void  GUIAlphaElement::setAlpha(float alphaArg)
{
  color.w = clampNS(alphaArg, minAlpha, 1.0f);
}

float GUIAlphaElement::getAlpha()
{
  return color.w;
}

void  GUIAlphaElement::setAlphaFadeScale(float duration)
{
  alphaFadeScale = clampNS(duration, 0.0f, 10.0f);
}

float GUIAlphaElement::getAlphaFadeScale()
{
  return alphaFadeScale;
}

void  GUIAlphaElement::setMinAlpha(float minAlphaArg)
{
  minAlpha = clampNS(minAlphaArg, 0.2f, 1.0f);
}

float GUIAlphaElement::getMinAlpha()
{
  return minAlpha;
}

void GUIAlphaElement::modifyCurrentAlpha(float clockTick)
{
#if 0
  if(!mouseOver && !STX_Service::GetInputInstance()->IsMouseButtonPressed())
    setAlpha(color.w - clockTick * alphaFadeScale);
  else
    setAlpha(color.w + clockTick * alphaFadeScale);
#endif
}
