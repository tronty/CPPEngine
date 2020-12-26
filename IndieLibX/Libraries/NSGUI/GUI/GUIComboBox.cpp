#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUIFontManager.h"
#include "GUIComboBox.h"
#include "GUIPanel.h"
#include "GUIFrame.h"

GUIComboBox::GUIComboBox(NSString cbs) : GUIRectangle(cbs)

{
  setScrollingColor(0.2f, 0.75f, 0.35f, 1.0f);
  setFontScales(1.0f, 1.0f);
  setPosition(0.5, 0.5);

  currentSelection = new GUILabel(0, "itemCBS_0");
  dropMenuButton   = new GUIButton("cbddb");
  upperPanel       = new GUIPanel("cbup");
  lowerPanel       = new GUIPanel("cblp");

  upperPanel->setLayout(XAXIS_LAYOUT);

  upperPanel->setVisibleBounds(true);
  upperPanel->enableBGColor(true);
  upperPanel->setInterval(6, 3);
  upperPanel->addWidget(currentSelection);
  upperPanel->addWidget(dropMenuButton);

  lowerPanel->setLayout(YAXIS_LAYOUT);
  lowerPanel->setInterval(6, 2);
  lowerPanel->setVisible(false);
  lowerPanel->enableBGColor(true);
  lowerPanel->setVisibleBounds(true);
  lowerPanel->setAnchorPoint(CORNERLU);
  lowerPanel->setClipSize(0);

  displayScrollingRectangle =      true;
  selectionIndex            =        -1;
  widgetType                = COMBO_BOX;
  fontIndex                 =         0;
  lockItems                 =     false;
  deployed                  =     false;
}

GUIComboBox::~GUIComboBox()
{
  deleteObject(currentSelection);
  deleteObject(dropMenuButton);
  deleteObject(upperPanel);
  deleteObject(lowerPanel);
}

GUIEventListener  *GUIComboBox::getEventsListener()
{
  return this;
}

void  GUIComboBox::setFontScales(const D3DXFROMWINEVECTOR2 &scales)
{
  setFontScales(scales.x, scales.y);
}

void  GUIComboBox::setFontScales(float wScale, float hScale)
{
  fontScales.x = clampNS(hScale, 0.01f, 20.0f);
  fontScales.y = clampNS(wScale, 0.01f, 20.0f);
}

const D3DXFROMWINEVECTOR2  &GUIComboBox::getFontScales()
{
  return fontScales;
}

bool GUIComboBox::loadXMLSettings(XMLElement *element)
{
  if(!element || element->getName() != "ComboBox")
    {LOG_PRINT_NONE("Need a ComboBox node in the xml file");return false;}

  XMLElement *child                = 0,
             *gChild               = 0;
  D3DXFROMWINEVECTOR3     bordersColor         = upperPanel->getBordersColor();
  D3DXFROMWINEVECTOR4     bgColor              = upperPanel->getBGColor();
  float       lowerPanelColorScale = 1.0f;

  if(child = element->getChildByName("Button"))
  {
    dropMenuButton->loadXMLClippedRectangleInfo(child);
    dropMenuButton->loadXMLSettings(child);
    dropMenuButton->setCallbackString("cbddb");
    dropMenuButton->setLabelString("");
    dropMenuButton->setActive(true);
    dropMenuButton->setVisible(true);
  }

  if(child = element->getChildByName("lowerPanelColorScale"))
    lowerPanelColorScale = child->getValuef();

  if(child = element->getChildByName("displayScrollingRectangle"))
    displayScrollingRectangle = (child->getValue() == "true");

  if(child = element->getChildByName("hScale"))
    fontScales.y = child->getValuef();

  if(child = element->getChildByName("wScale"))
    fontScales.x = child->getValuef();

  if(child = element->getChildByName("BordersColor"))
    XMLElement::loadRX_GY_BZf(*child, bordersColor);

  if(child = element->getChildByName("ScrollingColor"))
    XMLElement::loadRX_GY_BZ_AWf(*child, scrollingColor);

  if(child = element->getChildByName("BGColor"))
    XMLElement::loadRX_GY_BZ_AWf(*child, bgColor);

  for(size_t i = 0; i < element->getChildrenCount(); i++)
  {
    if(!(child = element->getChild(i)))
      continue;

    const NSString &childName = child->getName();

    if(childName == "Item")
    if(gChild    = child->getChildByName("string"))
      addItem(gChild->getValue());

    if(childName == "fontIndex")
      fontIndex =  clampNS(child->getValuei(), 0, 50);
  }

  upperPanel->setBordersColor(bordersColor);
  lowerPanel->setBordersColor(bordersColor);

  upperPanel->setBGColor(bgColor);
  lowerPanel->setBGColor(bgColor.x*lowerPanelColorScale,
                         bgColor.y*lowerPanelColorScale,
                         bgColor.z*lowerPanelColorScale,
                         bgColor.w);

  setScrollingColor(scrollingColor);
  setFontScales(fontScales);
  GUIRectangle::loadXMLSettings(element);
  return (items.size() != 0);
}

void GUIComboBox::checkMouseEvents(int extraInfo, bool rBits)
{
  upperPanel->checkMouseEvents(extraInfo, rBits);
  scrollingRectangle.x = 1;

  if(lowerPanel->isVisible())
    lowerPanel->checkMouseEvents(extraInfo, rBits);

  if(!currentSelection->isFocused() && !dropMenuButton->isFocused())
    deployed = false;

  if(deployed)
  {
    lowerPanel->setVisible(true);
 //???   evt->consume();
  }
  else
    lowerPanel->setVisible(false);
}

void GUIComboBox::actionPerformed(GUIEvent &evt)
{
      LOG_FNLN_NONE;
  GUIEventListener *eventsListener  = parent->getEventsListener();
  const NSString     &cbs             = evt.getCallbackString();
  GUIRectangle     *sourceRectangle = evt.getEventSource();
  int               widgetType      = sourceRectangle->getWidgetType();

  if(currentSelection->isPressed() || dropMenuButton->isPressed())
    deployed = !deployed;

  if(widgetType == LABEL && (sourceRectangle->getParent() == lowerPanel))
  {
    GUILabel *newSelection = (GUILabel*)sourceRectangle;
    if(newSelection->isMouseOver())
      scrollingRectangle = newSelection->getWindowBounds();

    if(newSelection->isClicked() &&
       strcmp(newSelection->getLabelString(), currentSelection->getLabelString()))
    {
      currentSelection->setLabelString(newSelection->getLabelString());
      selectionIndex = getItemIndex(newSelection->getLabelString());
      update         = true;
      //if(eventsListener)
	{
	GUIEvent* guiEvent=new GUIEvent(this);
	if(eventsListener) 
		eventsListener->actionPerformed(*guiEvent);
	else
		PushGUIEvent(guiEvent);
	}
    }
  }
}

void GUIComboBox::render(float clockTick)
{
  if(!parent || !visible)
    return;

  computeWindowBounds();
  upperPanel->render(clockTick);

  if(lowerPanel->isVisible())
  {
    lowerPanel->renderClippedBounds();
    lowerPanel->enableBGColor(false);
    lowerPanel->setVisibleBounds(false);

    if(scrollingRectangle.x >= lowerPanel->getWindowBounds().x && displayScrollingRectangle)
    {

      //glEnable(GL_BLEND);
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      IRenderer::GetRendererInstance()->Color4fv(scrollingColor);

      IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLE_STRIP);
#if 1
      IRenderer::GetRendererInstance()->Vertex2i(scrollingRectangle.x - 2, scrollingRectangle.y);
      IRenderer::GetRendererInstance()->Vertex2i(scrollingRectangle.x - 2, scrollingRectangle.w);
      IRenderer::GetRendererInstance()->Vertex2i(scrollingRectangle.z + 1, scrollingRectangle.y);
      IRenderer::GetRendererInstance()->Vertex2i(scrollingRectangle.z + 1, scrollingRectangle.w);
#else
#endif

      IRenderer::GetRendererInstance()->End();

      //glDisable(GL_BLEND);
    }

    lowerPanel->render(clockTick);
    lowerPanel->enableBGColor(true);
    lowerPanel->setVisibleBounds(true);
  }
}

void GUIComboBox::finalizeSize()
{
  if(!items.size() || lockItems )
    return;

  GUIFont *font             = GUIFontManager::getFont(fontIndex);
  const    int    *spaces   = font ? font->getFontObject()->getCharHorizontalGlyphs() : 0;

  LOG_FNLN_NONE;
  LOG_PRINT_NONE("spaces = %x\n", font->getFontObject()->getCharHorizontalGlyphs());
  LOG_PRINT_NONE("spaces = %d\n", *font->getFontObject()->getCharHorizontalGlyphs());
  NSString           itemCBS  = "itemCBS_";
  float            maxWidth = 0,
                   height   = 0;
  int              cbsIndex = 1;
  if(spaces)
  {
    float  width    = 0;
    int    length   = 0;

    height   =  float(font->getFontObject()->getHeight());

  LOG_FNLN_NONE;
  LOG_PRINT_NONE("height = %f\n", height);

    for(size_t l = 0; l < items.size(); l++)
    {
      length = items[l].getLength();
      width  = 0.0f;

      for(int t = 0; t < length; t++)
        width += spaces[items[l].getBytes()[t]];

      maxWidth = width > maxWidth ? width : maxWidth;
    }
  }
  else
    return;

  currentSelection->setLabelString(items[0]);
  currentSelection->setDimensions(maxWidth*fontScales.x + 2.0f, height*fontScales.y);
  currentSelection->getLabel()->setFontIndex(fontIndex);
  currentSelection->getLabel()->setScales(fontScales);
  dropMenuButton->setDimensions(height*fontScales.y + 4.0f, height*fontScales.y + 4.0f);

  for(size_t l = 0; l < items.size(); l++)
  {
    GUILabel *newLabel = new GUILabel(items[l], itemCBS + (cbsIndex++));
    newLabel->getLabel()->setFontIndex(fontIndex);
    newLabel->setDimensions(maxWidth*fontScales.x + 2.0f /*+ height*fontScales.y + 10.0f*/, height*fontScales.y);
    newLabel->getLabel()->setScales(fontScales);
    if(!lowerPanel->addWidget(newLabel))
      deleteObject(newLabel);
  }
  selectionIndex =    0;
  lockItems      = true;
}

const void GUIComboBox::computeWindowBounds()
{
  finalizeSize();

  if(parent && update)
  {
    upperPanel->setAnchorPoint(getAnchorPoint());
    upperPanel->setPosition(getPosition());
    upperPanel->setParent(parent);
    upperPanel->forceUpdate(true);
    upperPanel->computeWindowBounds();
    upperPanel->forceUpdate(false);
    windowBounds = upperPanel->getWindowBounds();

    lowerPanel->setPosition(0.0f, float(upperPanel->getHeight()));
    lowerPanel->setParent(upperPanel);
    lowerPanel->forceUpdate(true);

    //Redirect mouse events to combo box action listener
    lowerPanel->setParent(this);
    upperPanel->setParent(this);
  }
}

void GUIComboBox::addItem(const NSString &item)
{
  if(lockItems || !item.getLength())
    return;

  for(size_t t = 0; t < items.size(); t++)
    if(items[t] == item)
      return;

  items.push_back(item);
}

int  GUIComboBox::getItemIndex(const NSString &item)
{
  for(size_t t = 0; t < items.size(); t++)
    if(items[t] == item)
      return int(t);

  return -1;
}

const std::vector<NSString> &GUIComboBox::getItems() const
{
  return items;
}

NSString GUIComboBox::getSelectedItem()  const
{
  return selectionIndex < 0 ? 0 : items[selectionIndex];
}

NSString GUIComboBox::getItem(size_t index) const
{
  return index >= items.size() ? 0 :  items[index];
}

void    GUIComboBox::setScrollingColor(float r, float g, float b, float a)
{
  scrollingColor=D3DXFROMWINEVECTOR4(clampNS(r, 0.0f, 255.0f),
                     clampNS(g, 0.0f, 255.0f),
                     clampNS(b, 0.0f, 255.0f),
                     clampNS(a, 0.0f, 255.0f));

  scrollingColor.x /= (scrollingColor.x > 1.0) ? 255.0f : 1.0f;
  scrollingColor.y /= (scrollingColor.y > 1.0) ? 255.0f : 1.0f;
  scrollingColor.z /= (scrollingColor.z > 1.0) ? 255.0f : 1.0f;
  scrollingColor.w /= (scrollingColor.w > 1.0) ? 255.0f : 1.0f;
}

void    GUIComboBox::setScrollingColor(const D3DXFROMWINEVECTOR4 &rgba)
{
  setScrollingColor(rgba.x, rgba.y, rgba.z, rgba.w);
}

const   D3DXFROMWINEVECTOR4 &GUIComboBox::getScrollingColor() const
{
  return  scrollingColor;
}

void    GUIComboBox::enableScrollingRectangle(bool on)
{
  displayScrollingRectangle = on;
}

bool    GUIComboBox::scrollingRectangleEnabled()
{
  return displayScrollingRectangle;
}

bool GUIComboBox::isDeployed()
{
  if(lowerPanel) return lowerPanel->isVisible();
  return false;
}
