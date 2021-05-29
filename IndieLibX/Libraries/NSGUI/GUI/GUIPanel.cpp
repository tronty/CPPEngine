#include <Framework3/IRenderer.h>
#include <GUI/GUIUtils.h>

GUIPanel::GUIPanel(NSString cbs) : GUIRectangle(cbs), GUIClippedRectangle()
{
  setInterval(4, 4);
  widgetType = PANEL;
  layout     = FREE_LAYOUT;
  currentElement=-1;
}

bool GUIPanel::loadXMLSettings(NSString stackPath)
{
  NSString xmlFile = //MediaPathManager::lookUpMediaPath(
	  stackPath;
  XMLStack    cfgStack;
  bool        result = false;
  if(!xmlFile)
    {LOG_PRINT_NONE("Failed to locate the XML GUI file");return false;}


//LOG_PRINT_NONE("xmlFile=%s\n", xmlFile);

  if(cfgStack.loadXMLFile(xmlFile) != XML_SUCCESS)
  {LOG_PRINT_NONE("Invalid XML GUI description file");return false;}

  result = loadXMLSettings(cfgStack.getChildByName("Panel"));
  forceUpdate(true);
	setIndexes();
  return result;
}

bool GUIPanel::loadXMLSettings(XMLElement *element)
{
  if(!element || element->getName() != "Panel")
  {
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("Need a Panel node in the xml file\n");
	LOG_PRINT_NONE("element=%x\n", element);
	if(element) LOG_PRINT_NONE("name=%s\n", element->getName().data);
	return false;
  }

  XMLElement *child    = 0,
             *subChild = 0;

  if(child = element->getChildByName("description"))
    return  loadXMLSettings(child->getValuec());

  for(size_t i = 0; i < element->getChildrenCount(); i++)
  {
    if(!(child = element->getChild(i)))
      continue;

    const NSString &childName = child->getName();

    if(childName == "Interval")
    {
      XMLElement::loadRX_GYi(*child, interval);
      setInterval(interval);
      continue;
    }

    if(childName == "layout")
    {
      const NSString& type = child->getValue();
      layout = (type == "CEN_YAXIS") ? YAXIS_CEN_LAYOUT :
               (type == "YAXIS"    ) ? YAXIS_LAYOUT     :
               (type == "XAXIS"    ) ? XAXIS_LAYOUT     :
               (type == "GRID"     ) ? GRID_LAYOUT      : FREE_LAYOUT;
      continue;
    }

    if(childName == "Panel")
    {
      GUIPanel *panel = new GUIPanel();
      if(!panel->loadXMLSettings(child) || !addWidget(panel))
        deleteObject(panel);
      continue;
    }

    if(childName == "CheckBox")
    {
      GUICheckBox *newCheckBox = new GUICheckBox();
      if(!newCheckBox->loadXMLSettings(child) || !addWidget(newCheckBox))
        deleteObject(newCheckBox);
      continue;
    }

    if(childName == "TabbedPanel")
    {
      GUITabbedPanel *newTabbedPanel = new GUITabbedPanel();
      if(!newTabbedPanel->loadXMLSettings(child) || !addWidget(newTabbedPanel))
        deleteObject(newTabbedPanel);
      continue;
    }

    if(childName == "RadioButton")
    {
      GUIRadioButton *newRadioButton = new GUIRadioButton();
      if(!newRadioButton->loadXMLSettings(child) || !addWidget(newRadioButton))
      {
        deleteObject(newRadioButton);
      }
      else
      {
        if(newRadioButton->isChecked())
          notify(newRadioButton);
      }
      continue;
    }

    if(childName == "MaterialSurface")
    {
      GUIMaterialSurface *newMaterialSurface = new GUIMaterialSurface();
      if(!newMaterialSurface->loadXMLSettings(child) || !addWidget(newMaterialSurface))
        deleteObject(newMaterialSurface);
      continue;
    }

    if(childName == "ComboBox")
    {
      GUIComboBox *newComboBox = new GUIComboBox();
      if(!newComboBox->loadXMLSettings(child) || !addWidget(newComboBox))
        deleteObject(newComboBox);
      continue;
    }

    if(childName == "TextBox")
    {
      GUITextBox *newTextBox = new GUITextBox();
      if(!newTextBox->loadXMLSettings(child) || !addWidget(newTextBox))
        deleteObject(newTextBox);
      continue;
    }

    if(childName == "Slider")
    {
      GUISlider *newSlider = new GUISlider();
      if(!newSlider->loadXMLSettings(child) || !addWidget(newSlider))
        deleteObject(newSlider);
      continue;
    }

    if(childName == "Separator")
    {
      GUISeparator *newSeparator = new GUISeparator();
      if((layout != YAXIS_LAYOUT &&
          layout != XAXIS_LAYOUT &&
          layout != YAXIS_CEN_LAYOUT) ||
         (!newSeparator->loadXMLSettings(child)))
      {
        deleteObject(newSeparator);
      }
      else
      {
        newSeparator->setParent(this);
        newSeparator->setOrientation((layout != YAXIS_LAYOUT && layout != YAXIS_CEN_LAYOUT));
        elements.push_back(newSeparator);
      }
      continue;
    }

    if(childName == "Button")
    {
      GUIButton *newButton = new GUIButton();
      if(!newButton->loadXMLSettings(child) || !addWidget(newButton))
        deleteObject(newButton);
      continue;
    }

    if(childName == "Label")
    {
      GUILabel *newLabel = new GUILabel();
      if(!newLabel->loadXMLSettings(child) || !addWidget(newLabel))
        deleteObject(newLabel);
      continue;
    }

  }
  return  GUIRectangle::loadXMLSettings(element)  &&
          GUIClippedRectangle::loadXMLClippedRectangleInfo(element);
}

void GUIPanel::setIndexes()
{
	size_t t = 0;
  for(t = 0; t < elements.size(); t++)
	{
	elements[t]->setIndex(t);
	}
}

int GUIPanel::getWidgetIndexByCallbackString(NSString callbackString) const
{
  if(!callbackString)
    return 0;

  int element = -1;

  size_t t = 0;

  for(t = 0; t < elements.size(); t++)
  {
    if(elements[t]->getCallbackString() == callbackString)
      return t;
  }
	
  return element;
}

GUIRectangle *GUIPanel::getWidgetByCallbackString(NSString callbackString)
{
  if(!callbackString)
    return 0;

  GUIRectangle *element = 0,
               *tPanel  = 0;

  size_t t = 0;

  for(t = 0; t < elements.size(); t++)
  {
    if(elements[t]->getCallbackString() == callbackString)
      return elements[t];
  }

  for(t = 0; t < elements.size(); t++)
  {
    if(elements[t]->getWidgetType() == PANEL)
    if(element = ((GUIPanel*)elements[t])->getWidgetByCallbackString(callbackString))
      break;

    if(elements[t]->getWidgetType() == TABBED_PANEL)
    if(element = ((GUITabbedPanel*)elements[t])->getLowerPanel()->getWidgetByCallbackString(callbackString))
      break;
  }
  return element;
}

void GUIPanel::checkMouseEvents(int extraInfo, bool infoBits)
{
  if(visible && active)
  {
    GUIRectangle::checkMouseEvents(extraInfo, infoBits);
    for(size_t t = 0; t < elements.size(); t++)
    {
      if(elements[t]->isActive())
        elements[t]->checkMouseEvents(extraInfo, infoBits);
    }
  }
}
void GUIPanel::checkKeyboardEvents(int extraInfo)
{
  LOG_FNLN_NONE;
  if(visible && active)
  {
    GUIRectangle::checkKeyboardEvents(extraInfo);
    for(size_t t = 0; t < elements.size(); t++)
    if(elements[t]->isActive())
      elements[t]->checkKeyboardEvents(extraInfo);
  }
}

void  GUIPanel::render(float tick)
{
  if(!isAttached() || !parent)
    return;

  Widgets comboBoxes;
  GUIComboBox *cbPTR;

  if(visible)
  {
    renderClippedBounds();
    for(size_t t = 0; t < elements.size(); t++)
      if(elements[t]->getWidgetType() != COMBO_BOX)
	  {
/*
		  LOG_PRINT_NONE("cbs=%s\n",
			elements[t]->getCallbackString().data);
		if(stx_memcmp("Visibility",elements[t]->getCallbackString().data,10)==0)DBG_HALT;
*/
		elements[t]->render(tick);
	  }
      else
      {
        cbPTR = (GUIComboBox*)elements[t];
        if(!cbPTR->isDeployed())
          cbPTR->render(tick);
        else
          comboBoxes.push_back(elements[t]);
      }

    for(size_t t = 0; t < comboBoxes.size(); t++)
      comboBoxes[t]->render(tick);
  }
//	if(stx_memcmp("Visibility",getCallbackString().data,10)==0)DBG_HALT;
	//if(stx_memcmp("Visibility",getCallbackString().data,10)==0)
	//	visible=true;


}

void  GUIPanel::collectZWidgets(ZWidgets &container)
{
  if(visible)
  {
    for(size_t t = 0; t < elements.size(); t++)
      if(elements[t]->getWidgetType() != PANEL)
      {
        ZWidget zWidget(elements[t]);
        zWidget.setDistance(float(elements[t]->getZCoordinate()));
        container.push_back(zWidget);
      }
      else
      {
         ((GUIPanel*)elements[t])->collectZWidgets(container);
      }

    ZWidget zWidget(this);
    zWidget.setDistance(float(getZCoordinate()));

    container.push_back(zWidget);
  }
}

void GUIPanel::pack()
{
  if(!update)
    return;

  size_t t = 0;

  for(t = 0; t < elements.size(); t++)
    elements[t]->forceUpdate(update);

  if(!t)
  {
    correctPosition();
    return;
  }

  switch(layout)
  {
    case YAXIS_LAYOUT:
    case YAXIS_CEN_LAYOUT:
      packYAxisLayout(); break;
    case XAXIS_LAYOUT: packXAxisLayout(); break;
    default: packFreeLayout();
  }

  for(t = 0; t < elements.size(); t++)
	{
    elements[t]->forceUpdate(update);
	}
}
void GUIPanel::forceUpdate(bool updateArg)
{
  update = updateArg;
  pack();
}

void GUIPanel::packYAxisLayout()
{
  computeWindowBounds();

  std::vector<int> childrenHeights,
              childrenWidths;

  size_t      t          = 0;
  int         height     = 0,
              xOffset    = 0,
              panelWidth = getWidth();

  for(t = 0; t < elements.size(); t++)
  {
    const Tuple4i &childBounds = elements[t]->getWindowBounds();
    childrenHeights.push_back(childBounds.w - childBounds.y + interval.y);
    childrenWidths.push_back(childBounds.z - childBounds.x);
    height       += childrenHeights[t];
    panelWidth    = childrenWidths[t] > panelWidth ? childrenWidths[t] : panelWidth;
  }

  dimensions=D3DXFROMWINEVECTOR2(float(panelWidth), float(height));
  GUIRectangle::computeWindowBounds();

  windowBounds.z += interval.x*2;
  windowBounds.w += interval.y;

  update = false;
  height = interval.y;

  correctPosition();
  computeClippedBounds(windowBounds);

  for(t = 0; t < elements.size(); t++)
  {
    xOffset = (layout == YAXIS_CEN_LAYOUT) * (panelWidth - childrenWidths[t])/2;

    elements[t]->setAnchorPoint(CORNERLU);
    elements[t]->setPosition(float(interval.x + xOffset), float(height));
    elements[t]->computeWindowBounds();
    height += childrenHeights[t];
  }
}

void GUIPanel::packXAxisLayout()
{
  computeWindowBounds();

  std::vector<int> childrenWidths,
              childrenHeights;
  float       offset      = 0;
  size_t      t           = 0;
  int         height      = 0,
              width       = 0,
              panelHeight = windowBounds.w - windowBounds.y;

  for(t = 0; t < elements.size(); t++)
  {
    const Tuple4i &childBounds = elements[t]->getWindowBounds();
    childrenHeights.push_back(childBounds.w - childBounds.y);
    childrenWidths.push_back (childBounds.z - childBounds.x + interval.x);

    width       += childrenWidths[t];
    height       = childBounds.w - childBounds.y;
    panelHeight  = height > panelHeight ? height : panelHeight;
  }

  dimensions=D3DXFROMWINEVECTOR2(float(width), float(panelHeight));
  GUIRectangle::computeWindowBounds();

  windowBounds.z += interval.x;
  windowBounds.w += interval.y*2;

  update = false;
  width  = interval.x;

  correctPosition();
  computeClippedBounds(windowBounds);

  for(t = 0; t < elements.size(); t++)
  {
    offset = clampNS(float(panelHeight - childrenHeights[t])/2.0f + interval.y, 0.0f, 1000.0f);
    elements[t]->setAnchorPoint(CORNERLU);
    elements[t]->setPosition(float(width), offset);
    elements[t]->computeWindowBounds();
    width += childrenWidths[t];
  }
}

void GUIPanel::packFreeLayout()
{
  if(!update || !parent)
    return;

  Tuple4i newBounds = getWindowBounds();
  size_t  t         = 0;
  int     temp1     = 0,
          temp2     = 0;

  for(t = 0; t < elements.size(); t++)
  {
    elements[t]->forceUpdate(update);
    const Tuple4i &childBounds = elements[t]->getWindowBounds();

    newBounds.x = (childBounds.x - clipSize) < newBounds.x ?
                   childBounds.x - clipSize : newBounds.x;
    newBounds.y = (childBounds.y - clipSize) < newBounds.y ?
                   childBounds.y - clipSize : newBounds.y;
    newBounds.z = (childBounds.z + clipSize) > newBounds.z ?
                   childBounds.z + clipSize: newBounds.z;
    newBounds.w = (childBounds.w + clipSize) > newBounds.w ?
                   childBounds.w + clipSize : newBounds.w;
  }

  windowBounds = newBounds;
  update       = false;

  correctPosition();
  computeClippedBounds(windowBounds);

  for(t = 0; t < elements.size(); t++)
    elements[t]->computeWindowBounds();
}

void GUIPanel::correctPosition()
{
  if(!parent)
    return;

  const Tuple4i&  parentBounds = parent->getWindowBounds();
  bool            update       = false;
  int             temp         = windowBounds.z - windowBounds.x;

  if(position.x < 0)
  if(parentBounds.z - windowBounds.z < stx_abs(position.x))
  {
    windowBounds.z  = parentBounds.z + int(position.x);
    windowBounds.x  = windowBounds.z - temp;
    update          = true;
  }

  if(position.x > 1)
  if(windowBounds.x <  position.x + parentBounds.x)
  {
    windowBounds.x  = parentBounds.x + int(position.x);
    windowBounds.z  = windowBounds.x + temp;
    update          = true;
  }

  if(position.y < 0)
  if(parentBounds.w - windowBounds.w < stx_abs(position.y))
  {
    temp            = windowBounds.w - windowBounds.y;
    windowBounds.w  = parentBounds.w + int(position.y);
    windowBounds.y  = windowBounds.w - temp;
    update          = true;
  }

  if(update)
    for(size_t t = 0; t < elements.size(); t++)
    {
      elements[t]->forceUpdate(true);
      elements[t]->computeWindowBounds();
    }
}

bool  GUIPanel::addWidget(GUIRectangle *element)
{
  if(element)
  {
    for(size_t t = 0; t < elements.size(); t++)
      if(elements[t]->getCallbackString() == element->getCallbackString())
        return false;//LOG_PRINT_NONE("Panel already has a child with CBS -> ")
                       //              + element->getCallbackString());
    elements.push_back(element);
    element->setParent(this);
    return true;
  }
  return false;
}

void GUIPanel::clear()
{
  parent        = 0;

  for(size_t t = 0; t < elements.size(); t++)
    deleteObject(elements[t]);

  elements.clear();
}

void GUIPanel::notify(GUIRectangle *element)
{
  if(!element)
    return;

  switch(element->getWidgetType())
  {
    case RADIO_BUTTON:
      for(size_t t = 0; t < elements.size(); t++)
        if(elements[t]->getWidgetType() == RADIO_BUTTON &&
           elements[t]!= element)
        {
          GUIRadioButton *rButton = (GUIRadioButton*)elements[t];
          rButton->setChecked(false);
        }
    break;
  }
}

int  GUIPanel::getWidgetCountByType(int type)
{
  int  counter =  0;

  for(size_t t = 0; t < elements.size(); t++)
    if(elements[t]->getWidgetType() == type)
      counter++;

  return counter;
}

int  GUIPanel::getTreeHeight()
{
  int height = 1;

  for(size_t t = 0; t < elements.size(); t++)
   if(elements[t]->getWidgetType() == PANEL)
     height += ((GUIPanel*)elements[t])->getTreeHeight();
  
  return height;
}

void  GUIPanel::setInterval(const Tuple2i &dimensions)
{
  setInterval(dimensions.x, dimensions.y);
}

void  GUIPanel::setInterval(int width, int height)
{
  interval.set(clampNS(width , 0, 2500),
               clampNS(height, 0, 2500));
}

const Tuple2i &GUIPanel::getInterval()
{
  return interval;
}

void  GUIPanel::setLayout(int layoutArg)
{
  switch(layoutArg)
  {
    case GRID_LAYOUT:
    case YAXIS_CEN_LAYOUT:
    case YAXIS_LAYOUT:
    case XAXIS_LAYOUT:
      layout = layoutArg;
    break;
    default: layout = FREE_LAYOUT;
  }
}

int   GUIPanel::getLayout()
{
  return layout;
}

Widgets &GUIPanel::getWidgets()
{
  return elements;
}

GUIPanel::~GUIPanel()
{
  clear();
}
