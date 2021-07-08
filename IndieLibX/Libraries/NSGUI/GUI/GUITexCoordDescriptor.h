#ifndef GUI_TCD_H
#define GUI_TCD_H

#include "../Tools/NamedObject.h"
#include "../Parsers/XMLParser.h"
//include "../Tools/MathUtils.h"

enum WidgetTypes
{
  SLIDER    = 1,
  BUTTON,
  CHECK_BOX,
  CHECK_BOX_MARK,
  RADIO_BUTTON,
  CHECK_RB_MARK,
  LABEL,
  TEXT_AREA,
  MATERIAL_SURFACE,
  PANEL,
  SEPARATOR,
  TEXT_BOX,
  COMBO_BOX,
  TABBED_PANEL,
  UNKNOWN
};

class GUITexCoordDescriptor
{

  private:
    void    setType(const NSString &type);

    D3DXFROMWINEVECTOR4 texCoords;
    int     widgetType;

  public:

    GUITexCoordDescriptor(const int type = UNKNOWN);
    GUITexCoordDescriptor(const GUITexCoordDescriptor &copy);
#ifndef SWIG
    GUITexCoordDescriptor &operator =(const GUITexCoordDescriptor &copy);
#endif
    static void setTextureWidth(int width);
    static void setTextureHeight(int height);

    static int getTextureWidth();
    static int getTextureHeight();

    void loadXMLSettings(XMLElement *element);
    void setType(int type);
    int  getType();

    void setTexCoords(float x, float y, float z, float w);
    void setTexCoords(const D3DXFROMWINEVECTOR4& texCoords);

    const D3DXFROMWINEVECTOR4 &getTexCoords();
};

#endif
