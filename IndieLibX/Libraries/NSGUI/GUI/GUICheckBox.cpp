#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUICheckBox.h"
#include "GUIFrame.h"

GUICheckBox::GUICheckBox(NSString cbs) : GUIAlphaElement(cbs)
{
  setMinAlphaMark(0.0f);
  setDimensions(20, 20);
  setAlphaMark(1.0f);
  setBGColor(0.8f, 0.8f, 0.8f);
  setChecked(false);
  widgetType = CHECK_BOX;

  secondaryTexDesc = CHECK_BOX_MARK;
  primaryTexDesc   = CHECK_BOX;
  markOffset       = 0;
  markRatio        = 0.85f;
  extra            = 0;
}

bool GUICheckBox::loadXMLSettings(XMLElement *element)
{
  if(widgetType == CHECK_BOX)
  if(!element || element->getName() != "CheckBox")
    {LOG_PRINT_NONE("Need a CheckBox node in the xml file");return false;}

  if(widgetType == RADIO_BUTTON)
  if(!element || element->getName() != "RadioButton")
    {LOG_PRINT_NONE("Need a RadioButton node in the xml file");return false;}

  XMLElement *child    = 0,
             *subChild = 0;

	

  if(child = element->getChildByName("checked"))
    setChecked((child->getValue() == "true"));

  if(child = element->getChildByName("BGColor"))
    XMLElement::loadRX_GY_BZf(*child, bgColor);

  setBGColor(bgColor);
  return GUIAlphaElement::loadXMLSettings(element);
}

void  GUICheckBox::setMinAlphaMark(float mark)
{
  minAlphaMark = clampNS(mark, 0.0f, 1.0f);
}

float  GUICheckBox::getMinAlphaMark()
{
  return minAlphaMark;
}

void  GUICheckBox::setAlphaMark(float mark)
{
  alphaMark = clampNS(mark, minAlphaMark, 1.0f);
}

float  GUICheckBox::getAlphaMark()
{
  return alphaMark;
}

void  GUICheckBox::setChecked(bool enabledArg)
{
  if(active)
    checked = enabledArg;
}

bool  GUICheckBox::isChecked()
{
  return checked;
}

void  GUICheckBox::setBGColor(const D3DXFROMWINEVECTOR3& color)
{
  setBGColor(color.x, color.y, color.z);
}

void  GUICheckBox::setBGColor(float x, float y, float z)
{
  bgColor=D3DXFROMWINEVECTOR3(clampNS(x, 0.0f, 1.0f),
              clampNS(y, 0.0f, 1.0f),
              clampNS(z, 0.0f, 1.0f));
}

const D3DXFROMWINEVECTOR3 &GUICheckBox::getBGColor()
{
  return bgColor;
}

void GUICheckBox::render(float clockTick)
{
  if(!parent || !visible)
    return;

  modifyCurrentAlpha(clockTick);
  getCenter();

  GUITexCoordDescriptor *descriptor     = parent->getTexCoordsInfo(primaryTexDesc),
                        *descriptorCB   = parent->getTexCoordsInfo(secondaryTexDesc);

  int yCenter = (windowBounds.y + windowBounds.w)/2;

	D3DXFROMWINEVECTOR4 col(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXFROMWINEVECTOR4 colblack(0.0f, 0.0f, 0.0f, 1.0f);
	unsigned ret=0;
	// Radio button
	if(widgetType == RADIO_BUTTON)
	{
			//LOG_FNLN_NONE;
		float xPos = windowBounds.x - 9;
		float yTop = windowBounds.y - 9;
	#if 1
		ret=IRenderer::GetRendererInstance()->drawCircle(xPos + 17, yTop + 20, 7, col, 2);
	#elif 0
		//IRenderer::GetRendererInstance()->RenderTex(GUIFrame::elementsTexture, D3DXFROMWINEVECTOR2(xPos + 9, yTop + 10), D3DXFROMWINEVECTOR2(25.0f,100.0f), D3DXFROMWINEVECTOR2(0.0f,0.0f), D3DXFROMWINEVECTOR2(0.25f,1.0f));
		IRenderer::GetRendererInstance()->RenderTex(GUIFrame::elementsTexture, D3DXFROMWINEVECTOR2(xPos + 9, yTop + 10), D3DXFROMWINEVECTOR2(25.0f,100.0f), D3DXFROMWINEVECTOR2(0.25f,0.0f), D3DXFROMWINEVECTOR2(0.25f,1.0f));
	#endif
		if(ret){LOG_PRINT_NONE("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);}
		if(isChecked())
		{
	#if 1
			//LOG_FNLN_NONE;
			ret=IRenderer::GetRendererInstance()->drawCircle(xPos + 17, yTop + 20, 4, col);
	#elif 0
		IRenderer::GetRendererInstance()->RenderTex(GUIFrame::elementsTexture, D3DXFROMWINEVECTOR2(xPos + 9 /* + 17 */, yTop + 10 /* + 20 */), D3DXFROMWINEVECTOR2(14.0f,100.0f), D3DXFROMWINEVECTOR2(0.64f,0.0f), D3DXFROMWINEVECTOR2(0.14f,1.0f));
	#endif
			if(ret){LOG_PRINT_NONE("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);}
		}
	} else if(widgetType == CHECK_BOX) {
			//LOG_FNLN_NONE;
		// Checkbox
		float xPos = windowBounds.x - 9;
		float yTop = windowBounds.y - 9;
		ret=IRenderer::GetRendererInstance()->drawRect(xPos + 8, yTop + 11, xPos + 24, yTop + 28, col, 2);
		if(ret){LOG_PRINT_NONE("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);}
		if(isChecked())
		{
			//LOG_FNLN_NONE;
			// Check
	#if 1
			ret=IRenderer::GetRendererInstance()->drawLine(xPos + 11, yTop + 18, xPos + 15, yTop + 25, colblack, 2);
			if(ret){LOG_PRINT_NONE("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);}
			ret=IRenderer::GetRendererInstance()->drawLine(xPos + 15, yTop + 25, xPos + 21, yTop + 14, colblack, 2);
	#elif 0
		IRenderer::GetRendererInstance()->RenderTex(GUIFrame::elementsTexture, D3DXFROMWINEVECTOR2(xPos + 6 /* + 13 */, yTop + 10 /* + 21 */), D3DXFROMWINEVECTOR2(14.0f,100.0f), D3DXFROMWINEVECTOR2(0.50f,0.0f), D3DXFROMWINEVECTOR2(0.14f,1.0f));
	#else
	const D3DXFROMWINEVECTOR4 &texCoords = descriptor->getTexCoords();
	IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLE_STRIP);
	IRenderer::GetRendererInstance()->TexCoord2f(texCoords.x, texCoords.y);
	IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x, windowBounds.y);
	IRenderer::GetRendererInstance()->TexCoord2f(texCoords.x, texCoords.w);
	IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x, windowBounds.w);
	IRenderer::GetRendererInstance()->TexCoord2f(texCoords.z, texCoords.y);
	IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x + extra, windowBounds.y);
	IRenderer::GetRendererInstance()->TexCoord2f(texCoords.z, texCoords.w);
	IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x + extra, windowBounds.w);
	IRenderer::GetRendererInstance()->End(GUIFrame::elementsTexture);
	#endif
			if(ret){LOG_PRINT_NONE("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);}
		}
	}

  //glDisable(GL_BLEND);
  label.printCenteredY(windowBounds.x + extra + 2, yCenter);
}

const Tuple4i &GUICheckBox::getWindowBounds()
{
  if(parent && update)
  {
    label.computeDimensions();
    extra = int(float(label.getHeight())*1.25);
    dimensions=D3DXFROMWINEVECTOR2(float(label.getWidth() + extra), float(extra));
    markOffset = extra - int(dimensions.y*markRatio);
    GUIRectangle::computeWindowBounds();
  }
  return windowBounds;
}
void GUICheckBox::addItem(const NSString &item)
{

  for(size_t t = 0; t < items.size(); t++)
    if(items[t] == item)
      return;

  items.push_back(item);
}
int  GUICheckBox::getItemIndex(const NSString &item)
{
  for(size_t t = 0; t < items.size(); t++)
    if(items[t] == item)
      return int(t);

  return -1;
}
void GUICheckBox::checkMouseEvents(int extraInfo, bool bits)
{
  GUIRectangle::checkMouseEvents(extraInfo, true);
  if(!mouseOver)
	return;

  if(GUIRectangle::isPressed() || GUIRectangle::isClicked()) 
  {
	GUIEvent e(this);
	GUIRectangle     *sourceRectangle = e.getEventSource();
	NSString callbackString=sourceRectangle->getCallbackString();
	int i=((GUIPanel*)parent)->getWidgetIndexByCallbackString(callbackString);
	
	__DWORD__ t1=((GUIPanel*)parent)->getTime();
	__DWORD__ t2=timeGetTime();
	if((t2-t1)<1000)
		return;

	((GUIPanel*)parent)->setTime(t2);
	((GUIPanel*)parent)->setCurrentElement(i);
	
	setChecked(!checked);
	pressed = false;
    	clicked = false;
	//((GUIPanel*)parent)->notify(this);
	//LOG_FNLN_NONE;
	//LOG_PRINT_NONE("callbackString=%s,%d,%d\n", callbackString.data, i, c);
	return;
  }
}
