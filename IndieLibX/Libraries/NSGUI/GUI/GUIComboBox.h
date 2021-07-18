#ifndef GUI_COMBOBOX_H
#define GUI_COMBOBOX_H

#include "GUIButton.h"
#include "GUIPanel.h"
#include "GUILabel.h"

class GUIComboBox : public GUIRectangle, public GUIEventListener
{
  public:
    GUIComboBox(NSString callbackString = 0);
   ~GUIComboBox();

    virtual GUIEventListener  *getEventsListener();
    virtual const void         computeWindowBounds();
    virtual bool               loadXMLSettings(XMLElement *node);
    virtual void               render(float clockTick);
    virtual void               checkMouseEvents(int extraInfo, bool reservedBits = false);
    virtual void               actionPerformed(GUIEvent &evt);

    void                    setFontScales(float w, float h);
    void                    setFontScales(const D3DXFROMWINEVECTOR2 &scales);
    const   D3DXFROMWINEVECTOR2        &getFontScales();

    void                    addItem(const NSString &item);
    const   vector<NSString> &getItems() const;
    NSString           getSelectedItem()  const;
    NSString           getItem(size_t index) const;
    int                     getItemIndex(const NSString &item);

    void    setScrollingColor(float r, float g, float b, float alpha);
    void    setScrollingColor(const D3DXFROMWINEVECTOR4 &rgba);
    const   D3DXFROMWINEVECTOR4 &getScrollingColor() const;

    void    enableScrollingRectangle(bool);
    bool    scrollingRectangleEnabled();

    bool    isDeployed();

  private:

    void           finalizeSize();

    vector<NSString> items;
    GUIButton      *dropMenuButton;
    GUIPanel       *upperPanel,
                   *lowerPanel;
    GUILabel       *currentSelection;

    Tuple4i         scrollingRectangle;
    D3DXFROMWINEVECTOR4         scrollingColor;
    D3DXFROMWINEVECTOR2         fontScales;                   

    bool            displayScrollingRectangle,
                    lockItems,
                    deployed;

    int             selectionIndex,
                    fontIndex;
};

#endif
