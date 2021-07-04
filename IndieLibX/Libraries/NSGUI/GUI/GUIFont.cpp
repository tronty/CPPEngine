#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUIFont.h"

GUIFont::GUIFont(const char* name) : NamedObject(name)
{

}

bool GUIFont::build()
{
//  if(!getName().getBytes())
  //  return LOG_PRINT_NONE("Null font path");

		//LOG_FNLN_NONE;
	//LOG_PRINT_NONE("getName():%s\n",getName().c_str());
  std::string path=stx_convertpath(getName().c_str());

	  MediaPathManager::registerPath(path.c_str());

  return font.load(path.c_str());
}

bool GUIFont::loadXMLSettings(XMLElement *element)
{
  //if(!element || element->getName() != "Font")
//    return LOG_PRINT_NONE("Need a Font node in the xml file");

  XMLElement *child    = 0;

 if(child = element->getChildByName("path"))
{
    setName(child->getValuec());
}

  return true;
}

CFont *GUIFont::getFontObject()
{
  return &font;
}

bool  GUIFont::operator ==(GUIFont &compare)
{
  return (name   == compare.getName());
}

bool  GUIFont::operator !=(GUIFont &compare)
{
  return !operator ==(compare);
}

GUIFont::~GUIFont()
{
  font.~CFont();
}

