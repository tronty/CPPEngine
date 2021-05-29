#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUITexCoordDescriptor.h"

D3DXFROMWINEVECTOR2 textureGUIDimensions(256.0f,256.0f);

void GUITexCoordDescriptor::setTextureWidth(int width)
{
  //textureGUIDimensions.x = clampNS(width, 8, 1024);
}

void GUITexCoordDescriptor::setTextureHeight(int height)
{
  //textureGUIDimensions.y = clampNS(height, 8, 1024);
}

GUITexCoordDescriptor::GUITexCoordDescriptor(int widgetTypeArg)
{
  texCoords=D3DXFROMWINEVECTOR4(0.0f,0.0f,1.0f,1.0f);
  widgetType = widgetTypeArg;
}

GUITexCoordDescriptor::GUITexCoordDescriptor(const GUITexCoordDescriptor &copy)
{
  this->operator=(copy);
}

GUITexCoordDescriptor &GUITexCoordDescriptor::operator =(const GUITexCoordDescriptor &copy)
{

  if(this != &copy)
  {
    texCoords  = copy.texCoords;
    widgetType = copy.widgetType;
  }
  return *this;
}

void GUITexCoordDescriptor::setType(int type)
{
  switch(type)
  {
    case SLIDER:
    case BUTTON:
    case CHECK_BOX:
    case CHECK_BOX_MARK:
    case RADIO_BUTTON:
    case CHECK_RB_MARK:
      widgetType = type;
    break;
    default: widgetType = UNKNOWN;
  }
}

void GUITexCoordDescriptor::setType(const NSString &type)
{
  if(!type.getLength())
  {
    LOG_PRINT_NONE("0 GUITexCoordDescriptor type");
    return;
  }

  if(type == "MATERIAL_SURFACE"){ widgetType = MATERIAL_SURFACE; return; }
  if(type == "CHECK_BOX_MARK")  { widgetType = CHECK_BOX_MARK;   return; }
  if(type == "CHECK_RB_MARK")   { widgetType = CHECK_RB_MARK;    return; }
  if(type == "RADIO_BUTTON")    { widgetType = RADIO_BUTTON;     return; }
  if(type == "SEPARATOR")       { widgetType = SEPARATOR;        return; }
  if(type == "CHECK_BOX")       { widgetType = CHECK_BOX;        return; }
  if(type == "TEXT_AREA")       { widgetType = TEXT_AREA;        return; }
  if(type == "BUTTON")          { widgetType = BUTTON;           return; }
  if(type == "SLIDER")          { widgetType = SLIDER;           return; }
  if(type == "PANEL")           { widgetType = PANEL;            return; }
  if(type == "LABEL")           { widgetType = LABEL;            return; }

  //Logger::writeErrorLog(NSString("Unknow GUITexCoordDescriptor type -> ") + type);
  widgetType = UNKNOWN;
}

int GUITexCoordDescriptor::getType()
{
  return widgetType;
}

void GUITexCoordDescriptor::loadXMLSettings(XMLElement *element)
{
  if(!element || element->getName() != "TexCoordsDesc")
    return;

  XMLElement *child = 0;

  string name;
  int    xStart        = 0,
         yStart        = 0,
         xEnd          = 0,
         yEnd          = 0;

  if(child = element->getChildByName("xStart"))
    xStart = child->getValuei();

  if(child = element->getChildByName("yStart"))
    yStart = child->getValuei();

  if(child = element->getChildByName("xEnd"))
    xEnd = child->getValuei();

  if(child = element->getChildByName("yEnd"))
    yEnd = child->getValuei();

  if(child = element->getChildByName("type"))
    if(!(widgetType = child->getValuei()));
      setType(child->getValue());

    //LOG_PRINT_NONE("textureGUIDimensions.x=%d\n", textureGUIDimensions.x);
    //LOG_PRINT_NONE("textureGUIDimensions.y=%d\n", textureGUIDimensions.y);
    //LOG_PRINT_NONE("xStart=%d\n", xStart);
    //LOG_PRINT_NONE("xEnd=%d\n", xEnd);
    //LOG_PRINT_NONE("yStart=%d\n", yStart);
    //LOG_PRINT_NONE("yEnd=%d\n", yEnd);

  setTexCoords(xStart/float(textureGUIDimensions.x), 1.0f-yEnd/float(textureGUIDimensions.y),
               xEnd  /float(textureGUIDimensions.x), 1.0f-yStart  /float(textureGUIDimensions.y));
  setType(widgetType);


    //LOG_PRINT_NONE("texCoords.x=%f\n", texCoords.x);
    //LOG_PRINT_NONE("texCoords.y=%f\n", texCoords.y);
    //LOG_PRINT_NONE("texCoords.z=%f\n", texCoords.z);
    //LOG_PRINT_NONE("texCoords.w=%f\n", texCoords.w);


}

int GUITexCoordDescriptor::getTextureWidth()
{
  return textureGUIDimensions.x;
}

int GUITexCoordDescriptor::getTextureHeight()
{
  return textureGUIDimensions.y;
}

void GUITexCoordDescriptor::setTexCoords(float x, float y, float z, float w)
{
  texCoords=D3DXFROMWINEVECTOR4(x, y, z, w);//clampNS(x,0.0f, 1.0f), clampNS(y,0.0f, 1.0f), clampNS(z,0.0f, 1.0f), clampNS(w,0.0f, 1.0f));
}

void GUITexCoordDescriptor::setTexCoords(const D3DXFROMWINEVECTOR4& texCoordsArg)
{
  texCoords =  texCoordsArg;
};

const D3DXFROMWINEVECTOR4 &GUITexCoordDescriptor::getTexCoords()
{
  return texCoords;
}
