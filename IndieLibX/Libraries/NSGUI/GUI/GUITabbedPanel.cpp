#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUIFontManager.h"
#include "GUITabbedPanel.h"
#include "GUIButton.h"
#include "GUIFrame.h"

GUITabbedPanel::GUITabbedPanel(NSString cbs) : GUIRectangle(cbs)
{
  setTabButtonsBordersColor(0.0f, 0.0f, 0.0f);
  setTabButtonsColor(100, 150, 190);
  setFontScales(1.0f, 1.0f);
  setPosition(0.5, 0.5);

  upperPanel = new GUIPanel("tpup");
  lowerPanel = new GUIPanel("tpdown");
  mainPanel  = new GUIPanel("tpmp");

  mainPanel->setLayout(YAXIS_LAYOUT);
  mainPanel->setInterval(2, 2);

  mainPanel->setVisibleBounds(true);
  mainPanel->enableBGColor(true);

  upperPanel->setLayout(XAXIS_LAYOUT);
  upperPanel->setClipSize(0);

  lowerPanel->setInterval(0,0);
  lowerPanel->setClipSize(0);

  mainPanel->addWidget(upperPanel);
  mainPanel->addWidget(lowerPanel);

  widgetType = TABBED_PANEL;
  fontIndex  =            0;
  lockItems  =        false;
}

GUITabbedPanel::~GUITabbedPanel()
{
  deleteObject(mainPanel);
}

GUIEventListener  *GUITabbedPanel::getEventsListener()
{
  return this;
}

void  GUITabbedPanel::setFontScales(const D3DXFROMWINEVECTOR2 &scales)
{
  setFontScales(scales.x, scales.y);
}

void  GUITabbedPanel::setFontScales(float wScale, float hScale)
{
  fontScales.x = clampNS(hScale, 0.01f, 20.0f);
  fontScales.y = clampNS(wScale, 0.01f, 20.0f);
}

const D3DXFROMWINEVECTOR2  &GUITabbedPanel::getFontScales()
{
  return fontScales;
}

bool GUITabbedPanel::loadXMLSettings(XMLElement *element)
{
  if(!element || element->getName() != "TabbedPanel")
  {
     LOG_PRINT_NONE("Need a TabbedPanel node in the xml file");
	 return false;
  }

  XMLElement *child                = 0,
             *subChild             = 0;
  D3DXFROMWINEVECTOR3     bordersColor         = upperPanel->getBordersColor();
  D3DXFROMWINEVECTOR4     bgColor              = upperPanel->getBGColor();
  int         count                = 0;

  if(child = element->getChildByName("hScale"))
    fontScales.y = child->getValuef();

  if(child = element->getChildByName("wScale"))
    fontScales.x = child->getValuef();

  if(child = element->getChildByName("TabButtonsBordersColor"))
    XMLElement::loadRX_GY_BZf(*child, tabButtonsBordersColor);

  if(child = element->getChildByName("TabButtonsColor"))
    XMLElement::loadRX_GY_BZf(*child, tabButtonsColor);

  if(child = element->getChildByName("BordersColor"))
    XMLElement::loadRX_GY_BZf(*child, bordersColor);

  if(child = element->getChildByName("BGColor"))
    XMLElement::loadRX_GY_BZ_AWf(*child, bgColor);

  if(child = element->getChildByName("fontIndex"))
    fontIndex =  clampNS(child->getValuei(), 0, 50);

  setFontScales(fontScales);

  for(size_t i = 0; i < element->getChildrenCount(); i++)
  {
    if(!(child = element->getChild(i)))
      continue;

    const NSString &childName = child->getName();

    if(childName == "Panel")
    {
      GUIPanel *panel = new GUIPanel();
      if(!panel->loadXMLSettings(child) || !addPanel(panel))
        deleteObject(panel);
      continue;
    }
  }

  mainPanel->setBordersColor(bordersColor);
  mainPanel->setBGColor(bgColor);

  return GUIRectangle::loadXMLSettings(element) && lowerPanel->getWidgets().size();
}

bool GUITabbedPanel::addPanel(GUIPanel *panel)
{
  if(lockItems)
    return false;

  if(lowerPanel->addWidget(panel))
  {
    int count = int(lowerPanel->getWidgets().size()) - 1;
    GUIButton *tabButton = new GUIButton(NSString(" ") + count + " tb");
    tabButton->setBordersColor(tabButtonsBordersColor);
    tabButton->setColor(tabButtonsColor);
    NSString ls=NSString("  ") + panel->getCallbackString() + "  ";
    tabButton->setLabelString(ls);
    tabButton->getLabel()->setFontIndex(fontIndex);
    tabButton->getLabel()->setScales(fontScales);
    tabButton->setMinAlpha((count == 0) ? 0.75f : 0.5f);
    tabButton->enableBounce(false);
    tabButton->setClipSize(0);
    upperPanel->addWidget(tabButton);
    panel->setVisible((count == 0));
    return true;
  }
  return false;
}

const   GUIPanel*  GUITabbedPanel::getCurrentPanel() const
{
  const Widgets &widgets = lowerPanel->getWidgets();
  int   count  = int(widgets.size());

  for(int t = 0; t < count; t++)
    if(widgets[t]->isVisible())
       return (GUIPanel*)widgets[t];

  return 0;
}

const   int  GUITabbedPanel::getCurrentPanelIndex() const
{
  const Widgets &widgets = lowerPanel->getWidgets();
  int   count  = int(widgets.size());

  for(int t = 0; t < count; t++)
    if(widgets[t]->isVisible())
       return t;

  return -1;
}

void GUITabbedPanel::checkKeyboardEvents(int extraInfo)
{
  upperPanel->checkKeyboardEvents(extraInfo);
  lowerPanel->checkKeyboardEvents(extraInfo);
}

void GUITabbedPanel::checkMouseEvents(int extraInfo, bool rBits)
{
  upperPanel->checkMouseEvents(extraInfo, rBits);
  lowerPanel->checkMouseEvents(extraInfo, rBits);
}

void GUITabbedPanel::actionPerformed(GUIEvent &evt)
{
  GUIEventListener *eventsListener  = parent->getEventsListener();
  const NSString     &cbs             = evt.getCallbackString();
  GUIRectangle     *sourceRectangle = evt.getEventSource();
  int               widgetType      = sourceRectangle->getWidgetType();

  if((widgetType == BUTTON) && sourceRectangle->isPressed())
  {
    const Widgets &widgets = lowerPanel->getWidgets(),
                  &buttons = upperPanel->getWidgets();
    int   target = atoi(sourceRectangle->getCallbackString()),
          count  = int(widgets.size());
    for(int t = 0; t < count; t++)
    {
      widgets[t]->setVisible(t == target);
      ((GUIAlphaElement*)buttons[t])->setMinAlpha((t == target) ? 1.0f : 0.5f);
    }
  }
}

GUIButton *GUITabbedPanel::getTabButton(int index)
{
  const Widgets &widgets = upperPanel->getWidgets();
  int   count  = int(widgets.size());

  for(int t = 0; t < count; t++)
    if(t == index)
      return (GUIButton*)widgets[t];

  return 0;
}

void GUITabbedPanel::render(float clockTick)
{
  if(!parent || !visible)
    return;

  const Tuple4i &buttonBounds = upperPanel->getWidgets()[0]->getWindowBounds(),
                &mainBounds   = mainPanel->getWindowBounds();

#if 0
  D3DXFROMWINEVECTOR3 borderClr(0.0f, 0.0f, 1.0f);
#else
  const D3DXFROMWINEVECTOR3 &borderClr    = mainPanel->getBordersColor();
#endif

  lockItems = true;
  mainPanel->render(clockTick);

  IRenderer::GetRendererInstance()->Color3fv(borderClr);
#if 0//???
  IRenderer::GetRendererInstance()->Begin(PRIM_LINES);
  IRenderer::GetRendererInstance()->Vertex2i(mainBounds.x, buttonBounds.w);
  IRenderer::GetRendererInstance()->Vertex2i(mainBounds.z, buttonBounds.w);
  IRenderer::GetRendererInstance()->End();
#endif
  IRenderer::GetRendererInstance()->Color3f(1,1,1);
	//if(stx_memcmp("Visibility",getCallbackString().data,10)==0)DBG_HALT;
	//if(stx_memcmp("Visibility",getCallbackString().data,10)==0)
	//	visible=true;

}

const void GUITabbedPanel::computeWindowBounds()
{
  if(parent && update)
  {
    upperPanel->setParent(mainPanel);

    mainPanel->setAnchorPoint(getAnchorPoint());
    mainPanel->setPosition(getPosition());
    mainPanel->setParent(parent);
    mainPanel->forceUpdate(true);

    windowBounds = mainPanel->getWindowBounds();

    const Widgets & widgets     = lowerPanel->getWidgets();
    float           height      = 0;
    int             widgetCount = (int)widgets.size();

    for(int i = 0; i < widgetCount; i++)
      height = height < widgets[i]->getHeight() ? widgets[i]->getHeight() : height;
    lowerPanel->setDimensions(1.0f, height);
    upperPanel->setParent(this);
  }
}

void  GUITabbedPanel::setTabButtonsColor(const D3DXFROMWINEVECTOR3& color)
{
  setTabButtonsColor(color.x, color.y, color.z);
}

void  GUITabbedPanel::setTabButtonsColor(float r, float g, float b)
{
  tabButtonsColor=D3DXFROMWINEVECTOR3(clampNS(r, 0.0f, 255.0f),
                        clampNS(g, 0.0f, 255.0f),
                        clampNS(b, 0.0f, 255.0f));

  tabButtonsColor.x /= (tabButtonsColor.x > 1.0) ? 255.0f : 1.0f;
  tabButtonsColor.y /= (tabButtonsColor.y > 1.0) ? 255.0f : 1.0f;
  tabButtonsColor.z /= (tabButtonsColor.z > 1.0) ? 255.0f : 1.0f;
}

const D3DXFROMWINEVECTOR3 &GUITabbedPanel::getTabButtonsColor() const
{
  return tabButtonsColor;
}

void  GUITabbedPanel::setTabButtonsBordersColor(const D3DXFROMWINEVECTOR3& color)
{
  setTabButtonsBordersColor(color.x, color.y, color.z);
}

void  GUITabbedPanel::setTabButtonsBordersColor(float r, float g, float b)
{
  tabButtonsBordersColor=D3DXFROMWINEVECTOR3(clampNS(r, 0.0f, 255.0f),
                             clampNS(g, 0.0f, 255.0f),
                             clampNS(b, 0.0f, 255.0f));
  tabButtonsBordersColor.x /= (tabButtonsBordersColor.x > 1.0) ? 255.0f : 1.0f;
  tabButtonsBordersColor.y /= (tabButtonsBordersColor.y > 1.0) ? 255.0f : 1.0f;
  tabButtonsBordersColor.z /= (tabButtonsBordersColor.z > 1.0) ? 255.0f : 1.0f;
}

GUIPanel*  GUITabbedPanel::getLowerPanel()
{
  return lowerPanel;
}

const D3DXFROMWINEVECTOR3 &GUITabbedPanel::getTabButtonsBordersColor() const
{
  return tabButtonsBordersColor;
}
