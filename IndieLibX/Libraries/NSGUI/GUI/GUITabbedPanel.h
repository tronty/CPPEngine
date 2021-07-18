#ifndef GUI_TABBED_PANEL_H
#define GUI_TABBED_PANEL_H

#include "GUIButton.h"
#include "GUIPanel.h"

class GUITabbedPanel : public GUIRectangle, public GUIEventListener
{
  public:
    GUITabbedPanel(NSString callbackString = 0);
   ~GUITabbedPanel();

    virtual GUIEventListener  *getEventsListener();
    virtual const void         computeWindowBounds();
    virtual bool               loadXMLSettings(XMLElement *node);
    virtual void               render(float clockTick);
    virtual void               checkMouseEvents(int extraInfo, bool reservedBits = false);
    virtual void               checkKeyboardEvents(int extraInfo);
    virtual void               actionPerformed(GUIEvent &evt);

    void               setTabButtonsColor(float r, float g, float b);
    void               setTabButtonsColor(const D3DXFROMWINEVECTOR3& color);
    const D3DXFROMWINEVECTOR3     &getTabButtonsColor() const;

    void               setTabButtonsBordersColor(float r, float g, float b);
    void               setTabButtonsBordersColor(const D3DXFROMWINEVECTOR3& color);
    const D3DXFROMWINEVECTOR3     &getTabButtonsBordersColor() const;

    void               setFontScales(float w, float h);
    void               setFontScales(const D3DXFROMWINEVECTOR2 &scales);
    const   D3DXFROMWINEVECTOR2   &getFontScales();

    bool               addPanel(GUIPanel *panel);
    const   GUIPanel*  getCurrentPanel()      const;
    const   int        getCurrentPanelIndex() const;

    GUIButton         *getTabButton(int index);
    GUIPanel          *getLowerPanel();
  private:
    GUIPanel      *upperPanel,
                  *lowerPanel,
                  *mainPanel;

    D3DXFROMWINEVECTOR3        tabButtonsBordersColor,
                   tabButtonsColor;
    D3DXFROMWINEVECTOR2        fontScales;
    bool           lockItems;
    int            fontIndex;
};

#endif
