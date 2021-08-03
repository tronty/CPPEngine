#ifndef GUI_TEXT_BOX_H
#define GUI_TEXT_BOX_H

#include "GUIAlphaElement.h"

enum TextEditionType
{
  BACKSPACE_DELETE,
  SIMPLE_DELETE,
  INSERT_CHAR,
  BATCH_DELETE,
  PARSE_VISIBLE
};

class GUITextBox : public GUIAlphaElement
{
  public:
    GUITextBox(NSString callbackString = 0,
               NSString fieldText      = 0);

    virtual void    render(float clockTick);
    virtual void    checkMouseEvents(int extraInfo, bool reservedBits = false);
    virtual void    checkKeyboardEvents(int extraInfo);
    virtual bool    loadXMLSettings(XMLElement *node);
    virtual const   Tuple4i &getWindowBounds();
    void            setText(const NSString &text);
    const   NSString &getText() const;

    bool    textChanged();

    void  setBordersColor(const D3DXFROMWINEVECTOR3& color);
    void  setBordersColor(float r, float g, float b);
    const D3DXFROMWINEVECTOR3 &getBordersColor() const;

    void  setPadding(const Tuple2i& p);
    void  setPadding(int x, int y);
    const Tuple2i &getPadding() const;

  protected:
    Tuple2i padding;
    D3DXFROMWINEVECTOR3 bordersColor,
            textColor;
    float   blinkerTimer;
    bool    blinkerOn;
    int     blinkerPosition,
            textStartIndex,
            textEndIndex;
  private:
    void    setupText(int type, char info = 0);
    void    setupBlinker(int mouseX);
};

#endif
