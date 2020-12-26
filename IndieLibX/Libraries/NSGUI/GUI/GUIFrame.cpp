#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUIFrame.h"
#include "GUIFontManager.h"

TextureID             GUIFrame::elementsTexture=-1;
GUIFrame::GUIFrame() : GUIPanel("GUI MAIN PANEL")
{
  listener = 0;
  update   = true;

  //elementsTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/NSGUI/GUIElements.png",false,IRenderer::GetRendererInstance()->GetlinearClamp());

	LOG_PRINT_NONE("GUIFrame::GUIFrame():\n");
	LOG_PRINT_NONE("Image3.loadImageLibImage(/NSGUI/GUIElements.png)\n");
		
		std::string f=std::string("/NSGUI/GUIElements.png");
		std::string f2=stx_convertpath(f);
  Image3 img;
  img.loadImageLibImage(f2.c_str(), false);
  //img.flipY();
  elementsTexture=IRenderer::GetRendererInstance()->addTexture(&img,false,IRenderer::GetRendererInstance()->GetlinearClamp());
  #if 0
	RenderTex(elementsTexture, D3DXFROMWINEVECTOR2(0.0f,50.0f), D3DXFROMWINEVECTOR2(25.0f,100.0f), D3DXFROMWINEVECTOR2(0.0f,0.0f), D3DXFROMWINEVECTOR2(0.25f,1.0f));

	RenderTex(elementsTexture, D3DXFROMWINEVECTOR2(0.0f,75.0f), D3DXFROMWINEVECTOR2(25.0f,100.0f), D3DXFROMWINEVECTOR2(0.25f,0.0f), D3DXFROMWINEVECTOR2(0.25f,1.0f));

	RenderTex(elementsTexture, D3DXFROMWINEVECTOR2(0.0f,100.0f), D3DXFROMWINEVECTOR2(14.0f,100.0f), D3DXFROMWINEVECTOR2(0.50f,0.0f), D3DXFROMWINEVECTOR2(0.14f,1.0f)); // checkbox check

	RenderTex(elementsTexture, D3DXFROMWINEVECTOR2(0.0f,125.0f), D3DXFROMWINEVECTOR2(14.0f,100.0f), D3DXFROMWINEVECTOR2(0.64f,0.0f), D3DXFROMWINEVECTOR2(0.14f,1.0f)); // radiobutton

	RenderTex(elementsTexture, D3DXFROMWINEVECTOR2(0.0f,150.0f), D3DXFROMWINEVECTOR2(14.0f,100.0f), D3DXFROMWINEVECTOR2(0.78f,0.0f), D3DXFROMWINEVECTOR2(0.14f,1.0f)); // sliderbutton
  #endif
}

bool GUIFrame::loadXMLSettings(XMLElement *element)
{

  if(!element || element->getName() != "Panel")
  {
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("Need a Panel node in the xml file\n");
	LOG_PRINT_NONE("element=%x\n", element);
	if(element) LOG_PRINT_NONE("name=%s\n", element->getName().data);
	return false;
  }

  XMLElement *child       =  0,
             *subChild    =  0;
  bool        defaultFont = true;
#if 0
  if(child = element->getChildByName("Texture"))
  {
	NVTexture tex;    
		tex.loadXMLSettings(child);
		elementsTexture=tex.id;
    if(subChild = child->getChildByName("path"))
    {
      NSString path = child->getValue();
      {
	elementsTexture=IRenderer::GetRendererInstance()->addImageLibTexture(
		"/NSGUI/GUIElements.png"//path.data
		,false,IRenderer::GetRendererInstance()->GetlinearClamp()
		);
        GUITexCoordDescriptor::setTextureHeight(texture.getHeight());
        GUITexCoordDescriptor::setTextureWidth (texture.getWidth());
      }
    }
  }
#endif
  for(size_t i = 0; i < element->getChildrenCount(); i++)
  {
    if(!(child = element->getChild(i)))
      continue;

    const NSString &elementName = child->getName();
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

    if(elementName == "TexCoordsDesc")
    {
	#if 0
      LOG_FNLN_NONE;
      LOG_PRINT_NONE("if(elementName == \"TexCoordsDesc\")\n");
      LOG_PRINT_NONE("elementsTexture=%x\n", elementsTexture);
	#endif
      GUITexCoordDescriptor descriptor;
      descriptor.loadXMLSettings(child);
      addOrReplaceTexCoordsInfo(descriptor);
    }
  }
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

void  GUIFrame::setElementsTexture(NSString texturePath)
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
