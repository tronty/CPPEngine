#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUIFrame.h"
#include "GUIFontManager.h"

#if 1
#define LOG_PRINT printf
#define LOG_PRINT_NONE printf
#define LOG_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)
#endif

TextureID             GUIFrame::elementsTexture=-1;

GUIFrame::GUIFrame() : GUIPanel("GUI MAIN PANEL")
{
  listener = 0;
  update   = true;
}

bool GUIFrame::loadXMLSettings(XMLElement *element)
{
  LOG_FNLN;
  LOG_PRINT("element=%x\n", element);
  if(element) LOG_PRINT("element->getName()=%s\n", element->getName().c_str());
  if(!element || element->getName() != "Panel")
    return LOG_PRINT("Need a Panel node in the xml file\n");

    LOG_FNLN;
  XMLElement *child       =  0,
             *subChild    =  0;
  bool        defaultFont = true;

#if 0
  if(child = element->getChildByName("Texture"))
  {
    elementsTexture.loadXMLSettings(child);
    if(subChild = child->getChildByName("path"))
    {
      const String &path = child->getValue();
      Image texture;
      if(texture.load(path.getBytes()))
      {
        GUITexCoordDescriptor::setTextureHeight(texture.getHeight());
        GUITexCoordDescriptor::setTextureWidth (texture.getWidth());
      }
    }
  }
  #endif

  for(size_t i = 0; i < element->getChildrenCount(); i++)
  {
    LOG_FNLN;
    if(!(child = element->getChild(i)))
      continue;
    
    const std::string &elementName = child->getName();
    LOG_FNLN;
    LOG_PRINT("child=%x\n", child);
    LOG_PRINT("elementName=%s\n", elementName.c_str());
    if(elementName == "Font")
    {
      int fontIndex = GUIFontManager::addFont(child);
      if(defaultFont && fontIndex >= 0)
      {
        GUIFontManager::setDefaultFont(fontIndex);
        defaultFont = false;
      }
      continue;
    }

    LOG_FNLN;
    if(elementName == "TexCoordsDesc")
    {
      GUITexCoordDescriptor descriptor;
      descriptor.loadXMLSettings(child);
      addOrReplaceTexCoordsInfo(descriptor);
    }
  }
    LOG_FNLN;
  return   GUIPanel::loadXMLSettings(element);
}

void  GUIFrame::render(float tick)
{
  if(!visible)
    return;

  size_t t = 0;

  while(updateCount)
  {
    for(t = 0; t < elements.size(); t++)
      elements[t]->forceUpdate(true);
    updateCount--;
  }

  for(t = 0; t < elements.size(); t++)
    elements[t]->render(tick);
}

void GUIFrame::addOrReplaceTexCoordsInfo(GUITexCoordDescriptor &info)
{
  for(size_t t = 0; t < texCoords.size(); t++)
    if(texCoords[t].getType() == info.getType())
    {
      texCoords[t].setTexCoords(info.getTexCoords());
      return;
    }
  texCoords.push_back(info);
}

GUITexCoordDescriptor *GUIFrame::getTexCoordsInfo(int type)
{
  for(size_t t = 0; t < texCoords.size(); t++)
    if(texCoords[t].getType() == type)
      return &texCoords[t];
  return 0;
}

void  GUIFrame::setGUIEventListener(GUIEventListener *listener_)
{
  listener = listener_;
}

GUIEventListener *GUIFrame::getEventsListener()
{
  return listener;
}

void  GUIFrame::setElementsTexture(std::string texturePath)
{
	//elementsTexture=IRenderer::GetRendererInstance()->addImageLibTexture(texturePath,false, IRenderer::GetRendererInstance()->GetlinearClamp());
}

void  GUIFrame::setElementsTexture(const NVTexture &texture)
{
  elementsTexture = texture.id;
}
/*
NVTexture *GUIFrame::getElementsTexture()
{
  return elementsTexture==-1 ? 0 : &elementsTexture;
}
*/
void  GUIFrame::enableGUITexture()
{
  //elementsTexture.activate();
}

void  GUIFrame::disableGUITexture()
{
  //elementsTexture.deactivate();
}

void GUIFrame::forceUpdate(bool update)
{
  updateCount = update ? getTreeHeight() + 1 : 0;
}

void GUIFrame::clear()
{
  //elementsTexture.destroy();
  texCoords.clear();
  GUIPanel::clear();
}

const Tuple4i &GUIFrame::getWindowBounds()
{
  windowBounds.set(0, 0, int(dimensions.x), int(dimensions.y));
  return windowBounds;
}

GUIFrame::~GUIFrame()
{
  clear();
}

    XMLElement *XMLTree::getChildByName(const char *name)
    {
    #if 0
    	LOG_FNLN;
    	LOG_PRINT("name=%s\n", name);
    	LOG_PRINT("children.size()=%d\n", children.size());
    #endif
    	XMLElement* child=0;
    		if(name){
    	for(unsigned int i=0;i<children.size();i++){
    	//LOG_PRINT("children{%d]->getName()=%s\n", i, children[i]->getName().c_str());
    		if(children[i]->getName()==std::string(name))
    		{
    			child=children[i];
    			break;
    		}}}
    	return child;
    }

    XMLElement *XMLTree::getChild(size_t index)
    {
    #if 0
    	LOG_FNLN;
    	LOG_PRINT("index=%d\n", index);
    	LOG_PRINT("children.size()=%d\n",children.size());
    #endif
    	XMLElement* child=0;
    	if(index<children.size())
    		child=children[index];
    	return child;
    }


    XMLAttribute *XMLElement::getAttributeByName(const char *name)
    {
    #if 0
    	LOG_FNLN;
    	LOG_PRINT("name=%s\n", name);
    	LOG_PRINT("attributes.size()=%d\n", attributes.size());
    	#endif
    	XMLAttribute* child=0;
    		if(name){
    	for(unsigned int i=0;i<attributes.size();i++){
    	//LOG_PRINT("attributes{%d]->getName()=%s\n", i, attributes[i]->getName().c_str());
    		if(attributes[i]->getName()==std::string(name))
    		{
    			child=attributes[i];
    			break;
    		}}}
    	return child;
    }

    XMLAttribute *XMLElement::getAttribute(size_t index)
    {
    #if 0
    	LOG_FNLN;
    	LOG_PRINT("index=%d\n", index);
    	LOG_PRINT("attributes.size()=%d\n", attributes.size());
    	#endif
    	XMLAttribute* child=0;
    	if(index<attributes.size())
    		child=attributes[index];
    	return child;
    }
    int  XMLStack::loadXMLFile(const char *xmlFilePath)
    {
    	LOG_FNLN;
    	LOG_PRINT("xmlFilePath=%s\n", xmlFilePath); // ???
	std::string fn=stx_convertpath(xmlFilePath);
	LOG_PRINT("fn=%s\n", fn.c_str());
	
	TiXmlDocument document(fn.c_str());
	document.LoadFile();
	TiXmlHandle docHandle=TiXmlHandle( &document );
	TiXmlHandle rootPanelHandle = docHandle.FirstChild("Panel");
	rootPanelElement = rootPanelHandle.Element();

	#if 0
    	LOG_FNLN;
	LOG_PRINT("rootPanelElement=%x\n", rootPanelElement);
	LOG_PRINT("<Panel name=\"%s\" visible=\"%s\" >\n", rootPanelHandle.Element()->Attribute("name"), rootPanelHandle.Element()->Attribute("visible"));
	TiXmlHandle fontHandle = rootPanelHandle.FirstChild("Font");
	LOG_PRINT("<Font path=\"%s\" />\n", fontHandle.Element()->Attribute("path"));
	TiXmlHandle panelHandle = rootPanelHandle.FirstChild("Panel");
	LOG_PRINT("<Panel name=\"%s\" layout=\"%s\" anchorPoint=\"%s\">\n", panelHandle.Element()->Attribute("name"), panelHandle.Element()->Attribute("layout"), panelHandle.Element()->Attribute("anchorPoint"));
	TiXmlHandle PositionHandle = panelHandle.FirstChild("Position");
	LOG_PRINT("<Position x=\"%s\" y=\"%s\" />\n", PositionHandle.Element()->Attribute("x"), PositionHandle.Element()->Attribute("y"));
	TiXmlHandle IntervalHandle = panelHandle.FirstChild("Interval");
	LOG_PRINT("<Interval x=\"%s\" y=\"%s\" />\n", IntervalHandle.Element()->Attribute("x"), IntervalHandle.Element()->Attribute("y"));
	TiXmlHandle TabbedPanelHandle = panelHandle.FirstChild("TabbedPanel");
	LOG_PRINT("<TabbedPanel callbackString=\"%s\" >\n", TabbedPanelHandle.Element()->Attribute("callbackString"));
	TiXmlHandle TabButtonsBordersColorHandle = TabbedPanelHandle.FirstChild("TabButtonsBordersColor");
	TiXmlHandle TabButtonsColorHandle = TabbedPanelHandle.FirstChild("TabButtonsColor");
	TiXmlHandle BordersColorHandle = TabbedPanelHandle.FirstChild("BordersColor");
	TiXmlHandle BGColorHandle = TabbedPanelHandle.FirstChild("BGColor");
	TiXmlHandle PanelHandle = TabbedPanelHandle.FirstChild("Panel");
 	LOG_PRINT("<TabButtonsBordersColor x=\"%s\" g=\"%s\" b=\"%s\" />\n", TabButtonsBordersColorHandle.Element()->Attribute("x"), TabButtonsBordersColorHandle.Element()->Attribute("g"), TabButtonsBordersColorHandle.Element()->Attribute("b"));
 	LOG_PRINT("<TabButtonsColor x=\"%s\" g=\"%s\" b=\"%s\" />\n", TabButtonsColorHandle.Element()->Attribute("x"), TabButtonsColorHandle.Element()->Attribute("g"), TabButtonsColorHandle.Element()->Attribute("b"));
	LOG_PRINT("<BordersColor x=\"%s\" y=\"%s\" z=\"%s\" />\n", BordersColorHandle.Element()->Attribute("x"), BordersColorHandle.Element()->Attribute("y"), BordersColorHandle.Element()->Attribute("z"));
	LOG_PRINT("<BGColor x=\"%s\" y=\"%s\" z=\"%s\" />\n", BGColorHandle.Element()->Attribute("x"), BGColorHandle.Element()->Attribute("y"), BGColorHandle.Element()->Attribute("z"));
	LOG_PRINT("<Panel name=\"%s\" layout=\"%s\">\n", PanelHandle.Element()->Attribute("name"), PanelHandle.Element()->Attribute("layout"));
	int i=0;
	TiXmlHandle RadioButtonHandle = PanelHandle.Child("RadioButton", i++);
	TiXmlHandle TextHandle = RadioButtonHandle.FirstChild("Text");
	TiXmlElement* RadioButtonElement = RadioButtonHandle.Element();
	if(RadioButtonHandle.Element())
	{
	while ( RadioButtonElement )
	{
		//Sequence::FrameInSequence frame(frameElement->Attribute("name"), t);	
		//sequence.frames.push_back(frame);
		LOG_PRINT("<RadioButton callbackString=\"%s\" checked=\"%s\" >\n", RadioButtonHandle.Element()->Attribute("callbackString"), RadioButtonHandle.Element()->Attribute("checked"));
		LOG_PRINT("<Text string=\"%s\" />\n", TextHandle.Element()->Attribute("string"));
		RadioButtonHandle = PanelHandle.Child( "RadioButton", i++ );
		TextHandle = RadioButtonHandle.FirstChild("Text");
		RadioButtonElement = RadioButtonHandle.Element();
	}}	
	#endif
	#if 1
    	LOG_FNLN;
	LOG_PRINT("rootPanelElement=%x\n", rootPanelElement);
	XMLElement* p=new XMLElement(rootPanelElement);
	
	rootElement=p;
    	LOG_FNLN;	
	LOG_PRINT("new XMLElement(rootPanelElement=%x\n", p);
	LOG_PRINT("STXGUI::guiFrame=%x\n", STXGUI::guiFrame);
	bool r=STXGUI::guiFrame->loadXMLSettings(p);
	#if 0
	TiXmlElement* pElement=rootPanelElement;
    	if(!pElement) return;
    	for( TiXmlElement* child = pElement->FirstChild(); child; child = child->NextSibling() )
    		children.push_back(new XMLElement(child));
    	LOG_PRINT("children.size()=%d\n", children.size());
    	for( TiXmlAttribute* attribute = pElement->FirstAttribute(); attribute; attribute = attribute->Next() )
    		attributes.push_back(new XMLAttribute(attribute));
    	LOG_PRINT("attributes.size()=%d\n", attributes.size());
    	#endif
    	
    	p->Dump();
	
	#endif
    	return XML_SUCCESS;
    }

    XMLTree::XMLTree(TiXmlElement* pElement) : NamedObject()
    {
    	if(!pElement) return;
    	if(!pElement->ChildElementCount()) return;
    	for( TiXmlElement* child = (TiXmlElement*) pElement->FirstChild(); child; child = (TiXmlElement*) child->NextSibling() )
    	{
    		XMLElement* p=new XMLElement(child);
    		
    		
    	#if 1
    	for( TiXmlAttribute* attribute = child->FirstAttribute(); attribute; attribute = attribute->Next() )
    		p->attributes.push_back(new XMLAttribute(attribute));
    	//LOG_FNLN;
    	//LOG_PRINT("attributes.size()=%d\n", attributes.size());
    	#endif
    		
    		
    		children.push_back(p);
    	}
    	
    	#if 0
    	LOG_FNLN;
    	LOG_PRINT("children.size()=%d\n", children.size());
    	#endif
    }
    
        void XMLTree::Dump()
    {
    	LOG_FNLN;
    	for(unsigned int i=9;i<children.size();i++)
    	{
    		children[i]->Dump();
    	}
    }
    
