#ifndef GUI_FONT_H
#define GUI_FONT_H

#include "../Tools/Font.h"

class GUIFont : public NamedObject
{
  public:
    GUIFont(const char* path = 0);
   ~GUIFont();

    CFont *getFontObject();
 
    bool operator ==(GUIFont &compare);
    bool operator !=(GUIFont &compare);

    bool loadXMLSettings(XMLElement *node);
    bool build();

  private:
    CFont  font;
};
#endif
