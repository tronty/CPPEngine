#ifndef GUI_TEXT_H
#define GUI_TEXT_H

#include "../Tools/NSString.h"
#include "GUIRectangle.h"

class GUIFont;
class GUIText
{
  public:
    GUIText(const NSString &text);
    GUIText(const char* text = 0);

    GUIText(const GUIText & text);
    GUIText &operator =(const GUIText& text);
    GUIText &operator =(const NSString & text);
    GUIText &operator =(const char*    text);

    virtual bool loadXMLSettings(XMLElement *node);

    void  computeDimensions();
    void  printCenteredXY(int x, int y, int startIndex = 0, int endIndex = -1);
    void  printCenteredX (int x, int y, int startIndex = 0, int endIndex = -1);
    void  printCenteredY (int x, int y, int startIndex = 0, int endIndex = -1);
    void  print(int x, int y, int startIndex = 0, int endIndex = -1);

    const NSString& getString();
    NSString   getCharString();

    void setString(const NSString &text);
    void setString(const char* text);
    void clear();

    void  setSize(int x, int y);
    void  setSize(const Tuple2i& size);
    const Tuple2i& getSize();

    void  setColor(float r, float g, float b);
    void  setColor(const D3DXFROMWINEVECTOR3 &color);
    const D3DXFROMWINEVECTOR3 &getColor();

    int   getHeight();
    int   getWidth();

    void   setHeightScale(float hs);
    void   setWidthScale(float ws);
    void   setScales(D3DXFROMWINEVECTOR2 scales);
 
    float  getHeightScale();
    float  getWidthScale();  
    const  D3DXFROMWINEVECTOR2 & getScales();

    bool  needUpdating();
    void  forceUpdate(bool update);

    void setFontIndex(int fontIndex);
    int  getFontIndex();

  protected:
    bool     update;
    NSString   text;
    Tuple2i  position,
             size;
    D3DXFROMWINEVECTOR2  scales;
    D3DXFROMWINEVECTOR3  color;
    int      fontIndex;
};

#endif
