#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUIText.h"
#include "GUIFont.h"
#include "GUIFontManager.h"

GUIText::GUIText(const NSString &text)
{
	
  fontIndex =    -1;
  update    = true;

  color=D3DXFROMWINEVECTOR3(1.0,1.0,1.0);
  scales=D3DXFROMWINEVECTOR2(1.0f, 1.0f);
  setString(text);
}

GUIText::GUIText(const char* text)
{
	text   = "";
  fontIndex  =   -1;
  update     = true;
  color=D3DXFROMWINEVECTOR3(1.0,1.0,1.0);
  scales=D3DXFROMWINEVECTOR2(1.0f, 1.0f);
  setString(text);
}

GUIText::GUIText(const GUIText & copy)
{
  this->operator=(copy);
}

GUIText &GUIText::operator =(const GUIText & copy)
{
  if(this != &copy)
  {
    fontIndex = copy.fontIndex;
    scales    = copy.scales;
    update    = copy.update;
    color     = copy.color;
    size      = copy.size;
    text      = copy.text;
  }
  return *this;
}

GUIText &GUIText::operator =(const NSString & str)
{
  setString(str);
  return *this;
}

GUIText &GUIText::operator =(const char*  str)
{
  setString(str);
  return *this;
}

bool GUIText::loadXMLSettings(XMLElement *element)
{
  if(!element || element->getName() != "Text")
    return false;//LOG_PRINT_NONE("Need a Text node in the xml file");

  XMLElement *child    = 0;

  if(child = element->getChildByName("Font"))
    fontIndex = GUIFontManager::addFont(child);

  if(child = element->getChildByName("string"))
    setString(child->getValuec());

  if(child = element->getChildByName("fontIndex"))
    setFontIndex(child->getValuei());

  if(child = element->getChildByName("hScale"))
    setHeightScale(child->getValuef());

  if(child = element->getChildByName("wScale"))
    setWidthScale(child->getValuef());

  if(child = element->getChildByName("Color"))
  {
    XMLElement::loadRX_GY_BZf(*child, color);
    setColor(color);
  }
  return true;
}

NSString   GUIText::getCharString()
{
  return text;
}

const NSString& GUIText::getString(){  return text; }

void GUIText::setString(const NSString &textArg)
{
  if(textArg.getLength() && text != textArg)
  {
	 //text.data=0;//???
    text   = textArg;
    update = true;
  }
}

void GUIText::setString(const char* textArg)
{
  if(textArg && text != textArg)
  {
    text   = textArg;
    update = true;
  }
}

void GUIText::clear()
{
  text.clear();
  update = false;
  size.set(0,0);
}

int GUIText::getFontIndex()
{
  return fontIndex;
}

void GUIText::setFontIndex(int index)
{
  fontIndex = index;
}

void  GUIText::print(int x, int y, int startIndex, int endIndex)
{
  if(!text.getLength())
    return;

  endIndex   = (endIndex  < 0) ? int(text.getLength()) : endIndex;
  startIndex = clampNS(startIndex, 0, endIndex);

  GUIFontManager::setCurrentFont(fontIndex);
  GUIFont *currentFont = GUIFontManager::getCurrentFont();
  computeDimensions();

  if(!currentFont && !(currentFont = GUIFontManager::getDefaultFont()))
    return;

  currentFont->getFontObject()->printSubString(float(x), float(y), scales.x, scales.y,
                                               color.x, color.y, color.z,
                                               startIndex, endIndex,
                                               text);
}

void  GUIText::printCenteredX (int x, int y, int startIndex, int endIndex)
{
  if(!text.getLength())
    return;

  computeDimensions();
  print(x - size.x/2, y, startIndex, endIndex);
}

void  GUIText::printCenteredY (int x, int y, int startIndex, int endIndex)
{
  if(!text.getLength())
    return;

  computeDimensions();
  print(x,  y - size.y/2, startIndex, endIndex);
}

void  GUIText::printCenteredXY(int x, int y, int startIndex, int endIndex)
{
  if(!text.getLength())
    return;

  computeDimensions();
  print(x - size.x/2, y - size.y/2, startIndex, endIndex);
}

void GUIText::computeDimensions()
{
  if(needUpdating())
  {
    GUIFontManager::setCurrentFont(fontIndex);
    GUIFont *currentFont = GUIFontManager::getCurrentFont();

    if(!currentFont && !(currentFont = GUIFontManager::getDefaultFont()))
      return;

    if(currentFont == GUIFontManager::getDefaultFont())
      fontIndex = GUIFontManager::findFontIndex(currentFont);

    if(text.getLength())
    {
      size = currentFont->getFontObject()->getStringDimensions(text);
      size.x = int(float(size.x)*scales.x);
    }
    size.y = int(float(currentFont->getFontObject()->getHeight())*scales.y);
    forceUpdate(false);
  }
}

void GUIText::setSize(const Tuple2i& sizeArg){ size = sizeArg; }
void GUIText::setSize(int x, int y){ size.set(x, y); }

int  GUIText::getHeight(){ return abs(size.y); }
int  GUIText::getWidth() { return abs(size.x); }

const Tuple2i& GUIText::getSize(){ return size; }
bool  GUIText::needUpdating(){ return update; }

void  GUIText::forceUpdate(bool updateArg){ update = updateArg; }
void  GUIText::setColor(const D3DXFROMWINEVECTOR3 &color){ setColor(color.x, color.y, color.z); }

void GUIText::setColor(float r, float g, float b)
{
  color=D3DXFROMWINEVECTOR3(clampNS(r, 0.0f, 255.0f),
            clampNS(g, 0.0f, 255.0f),
            clampNS(b, 0.0f, 255.0f));

  color.x /= (color.x > 1.0) ? 255.0f : 1.0f;
  color.y /= (color.y > 1.0) ? 255.0f : 1.0f;
  color.z /= (color.z > 1.0) ? 255.0f : 1.0f;
}

void   GUIText::setHeightScale(float hs)
{
  scales.y = clampNS(hs, 0.1f, 20.0f);
}
void   GUIText::setWidthScale(float ws)
{
  scales.x = clampNS(ws, 0.1f, 20.0f);
}
void   GUIText::setScales(D3DXFROMWINEVECTOR2 scales)
{
  setHeightScale(scales.y);
  setWidthScale(scales.x);
}

float  GUIText::getHeightScale()
{
  return scales.y;
}

float  GUIText::getWidthScale()
{
  return scales.x;
}

const  D3DXFROMWINEVECTOR2 &GUIText::getScales()
{
  return scales;
}
const D3DXFROMWINEVECTOR3 &GUIText::getColor()
{
  return color;
}
