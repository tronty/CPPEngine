#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUISlider.h"
#include "GUIFrame.h"
GUISlider::GUISlider(NSString callback,
                     const int   orientation_) :
                     GUIAlphaElement(callback)
{
  m_Translation=D3DXFROMWINEVECTOR3(0,0,0);
  setDiscDimensions(18, 18);
  setProgress(0.0);
  orientation = orientation_ == VERTICAL ? VERTICAL : HORIZONTAL;
  widgetType  = SLIDER;
  offset      = 0;

  setDimensions((orientation_ == VERTICAL) ?  18.0f : 100.0f,
                (orientation_ == VERTICAL) ?  85.0f :  18.0f);
}

bool GUISlider::loadXMLSettings(XMLElement *element)
{
  if(!element || element->getName() != "Slider")
    {LOG_PRINT_NONE("Need a Slider node in the XML File");
  return false;}

  XMLElement *child    = 0,
             *subChild = 0;

  if(child = element->getChildByName("orientation"))
    orientation = (child->getValue() == "VERTICAL") ? VERTICAL : HORIZONTAL;

  if(child = element->getChildByName("progress"))
    setProgress(child->getValuef());

  if(child = element->getChildByName("discWidth"))
    discDimensions.x = child->getValuei();

  if(child = element->getChildByName("discHeight"))
    discDimensions.y = child->getValuei();

  setDimensions((orientation == VERTICAL) ? 18.0f : 100.0f,
                (orientation == VERTICAL) ? 85.0f :  18.0f);
  setDiscDimensions(discDimensions);
  return   GUIAlphaElement::loadXMLSettings(element);
}

void  GUISlider::setDiscDimensions(const Tuple2i& dimensions)
{
  setDiscDimensions(dimensions.x, dimensions.y);
}

void  GUISlider::setDiscDimensions(int width, int height)
{
  discDimensions.set(clampNS(width, 5, 500), clampNS(height, 5, 500));
  setDimensions((orientation == HORIZONTAL) ? dimensions.x            : float(discDimensions.x),
                (orientation == HORIZONTAL) ? float(discDimensions.y) : dimensions.y);
}

const Tuple2i &GUISlider::getDiscDimensions()
{
  return discDimensions;
}

void  GUISlider::setProgress(float zeroToOne)
{
  if(!STX_Service::GetInputInstance()->IsMouseButtonPressed())
    progress = clampNS(zeroToOne, 0.0f, 1.0f);
}

float GUISlider::getProgress()
{
  return progress;
}

void GUISlider::render(float clockTick)
{
  if(!parent || !visible)
    return;

  modifyCurrentAlpha(clockTick);

  if(orientation == VERTICAL)
  {
    renderVertical();
    return;
  }

  GUITexCoordDescriptor *descriptor    = parent->getTexCoordsInfo(SLIDER);
  Tuple2i discXBounds,
          center     = getCenter();


  discXBounds.x  = windowBounds.x;
  discXBounds.x += int(float(windowBounds.z - windowBounds.x)*progress);
  discXBounds.x -= discDimensions.x/2;
  discXBounds.y  = discXBounds.x + discDimensions.x;
  float xPos = windowBounds.x - 9;
  float yTop = windowBounds.y - 9;
  int yCenter = (windowBounds.y + windowBounds.w)/2;
	D3DXFROMWINEVECTOR4 col(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXFROMWINEVECTOR4 colblack(0.0f, 0.0f, 0.0f, 1.0f);
	unsigned ret=0;
  ret=IRenderer::GetRendererInstance()->drawRect(xPos + 8, yTop + 11, xPos + dimensions.x, yTop + dimensions.y, col, 2);

  float off=float(offset);
  //IRenderer::GetRendererInstance()->SetTranslation
  m_Translation=D3DXFROMWINEVECTOR3(float(offset), 0, 0);

  if(descriptor)
  {
	TextureID id=GUIFrame::elementsTexture;
	#if 0
	  ret=IRenderer::GetRendererInstance()->drawCircle(discXBounds.x+m_Translation.x, yTop + 11, 4, colblack);
	#elif 1
		IRenderer::GetRendererInstance()->RenderTex(GUIFrame::elementsTexture, D3DXFROMWINEVECTOR2(discXBounds.x+m_Translation.x, yTop + 6 /* + 11 */ ), D3DXFROMWINEVECTOR2(14.0f,100.0f), D3DXFROMWINEVECTOR2(0.78f,0.0f), D3DXFROMWINEVECTOR2(0.14f,1.0f));
	#else
	const D3DXFROMWINEVECTOR4 &texCoords = descriptor->getTexCoords();
	IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLE_STRIP);
	IRenderer::GetRendererInstance()->TexCoord2f(texCoords.x, texCoords.y); 
	IRenderer::GetRendererInstance()->Vertex2i(discXBounds.x, windowBounds.y);
	IRenderer::GetRendererInstance()->TexCoord2f(texCoords.x, texCoords.w);
	IRenderer::GetRendererInstance()->Vertex2i(discXBounds.x, windowBounds.w);
	IRenderer::GetRendererInstance()->TexCoord2f(texCoords.z, texCoords.y); 
	IRenderer::GetRendererInstance()->Vertex2i(discXBounds.y, windowBounds.y);
	IRenderer::GetRendererInstance()->TexCoord2f(texCoords.z, texCoords.w); 
	IRenderer::GetRendererInstance()->Vertex2i(discXBounds.y, windowBounds.w);
	IRenderer::GetRendererInstance()->End(GUIFrame::elementsTexture);
	#endif
  }
  m_Translation=D3DXFROMWINEVECTOR3(float(-offset), 0, 0);
  label.printCenteredX(center.x + offset+m_Translation.x, windowBounds.w+m_Translation.y);
}

void    GUISlider::renderVertical()
{
  GUITexCoordDescriptor *descriptor    = parent->getTexCoordsInfo(SLIDER);
  Tuple2i discYBounds,
          center     = getCenter();
  discYBounds.x  = windowBounds.w;
  discYBounds.x -= int(float(getHeight())*progress);
  discYBounds.x -= discDimensions.y/2;
  discYBounds.y  = discYBounds.x + discDimensions.y;
  float xPos = windowBounds.x - 9;
  float yTop = windowBounds.y - 9;
  int yCenter = (windowBounds.y + windowBounds.w)/2;
	D3DXFROMWINEVECTOR4 col(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXFROMWINEVECTOR4 colblack(0.0f, 0.0f, 0.0f, 1.0f);
	unsigned ret=0;
  IRenderer::GetRendererInstance()->Color4fv(color);
  float off=float(+offset);
  m_Translation=D3DXFROMWINEVECTOR3(0, float(+offset), 0);
  ret=IRenderer::GetRendererInstance()->drawRect(xPos + 8, yTop + 11, xPos + dimensions.x, yTop + dimensions.y, col, 2);

  if(descriptor)
  {
	TextureID id=GUIFrame::elementsTexture;
	#if 1
	  ret=IRenderer::GetRendererInstance()->drawCircle(discYBounds.x+m_Translation.x, yTop + 11, 4, colblack);
	#else
	const D3DXFROMWINEVECTOR4 &texCoords = descriptor->getTexCoords();
	IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLE_STRIP);
	IRenderer::GetRendererInstance()->TexCoord2f(texCoords.x, texCoords.y); 
	IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x, discYBounds.x);
	IRenderer::GetRendererInstance()->TexCoord2f(texCoords.x, texCoords.w);
	IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x, discYBounds.y);
	IRenderer::GetRendererInstance()->TexCoord2f(texCoords.z, texCoords.y); 
	IRenderer::GetRendererInstance()->Vertex2i(windowBounds.z, discYBounds.x);
	IRenderer::GetRendererInstance()->TexCoord2f(texCoords.z, texCoords.w); 
	IRenderer::GetRendererInstance()->Vertex2i(windowBounds.z, discYBounds.y);
	IRenderer::GetRendererInstance()->End(GUIFrame::elementsTexture);
	#endif
  }
  m_Translation=D3DXFROMWINEVECTOR3(0, float(-offset), 0);
}

const Tuple4i &GUISlider::getWindowBounds()
{
  if(parent && update)
  {
    GUIRectangle::computeWindowBounds();
    label.computeDimensions();
    realWindowBounds    = windowBounds;

    if(orientation == HORIZONTAL)
    {
      int difference = label.getWidth() -  getWidth();

      realWindowBounds.x -= discDimensions.x/2;
      realWindowBounds.z += discDimensions.x/2;

      if(difference > 0)
      {
        difference /= 2;
        realWindowBounds.x -= difference;
        realWindowBounds.z += difference;
      }

      realWindowBounds.w += int(label.getHeight()/1.25f);
      offset = clampNS(windowBounds.x - realWindowBounds.x, 0, 500);
    }
    else
    {
      offset               = discDimensions.y/2;
      realWindowBounds.y  -= discDimensions.y/2;
      realWindowBounds.w  += discDimensions.y/2;
    }
  }
  return realWindowBounds;
}

void GUISlider::checkMouseEvents(int extraInfo, bool reservedBits)
{

  GUIEventListener *eventsListener = getEventsListener();
  bool nRelease = STX_Service::GetInputInstance()->IsMouseButtonReleased();

  windowBounds.x += (orientation == HORIZONTAL) ? offset : 0;
  windowBounds.y += (orientation != HORIZONTAL) ? offset : 0;
  windowBounds.z += (orientation == HORIZONTAL) ? offset : 0;
  windowBounds.w += (orientation != HORIZONTAL) ? offset : 0;

  GUIRectangle::checkMouseEvents(extraInfo, reservedBits);
  if(!mouseOver)
	return;

  if(GUIRectangle::isPressed() || GUIRectangle::isClicked()) 
  {
    progress = (orientation == HORIZONTAL) ?
                clampNS(float(STX_Service::GetInputInstance()->GetMouseX() - windowBounds.x)/ (windowBounds.z - windowBounds.x), 0.0f, 1.0f) :
                clampNS(1.0f - float(STX_Service::GetInputInstance()->GetMouseY() - windowBounds.y)/ (windowBounds.w - windowBounds.y), 0.0f, 1.0f);

    //if(eventsListener)
	{
      GUIEvent* event = new GUIEvent(this);
      if(eventsListener)
	eventsListener->actionPerformed(*event);
	else
      		PushGUIEvent(event);
    }
  }

  windowBounds.x -= (orientation == HORIZONTAL) ? offset : 0;
  windowBounds.y -= (orientation != HORIZONTAL) ? offset : 0;
  windowBounds.z -= (orientation == HORIZONTAL) ? offset : 0;
  windowBounds.w -= (orientation != HORIZONTAL) ? offset : 0;
}
