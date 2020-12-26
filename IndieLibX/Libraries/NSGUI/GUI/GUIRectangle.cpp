#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUIRectangle.h"

GUIRectangle::GUIRectangle(const char* callback)
{
  setCallbackString(callback);
  setDimensions(5, 5);
  setPosition(0.5f, 0.5f);

  lastAction =      0;
  widgetType =UNKNOWN;
  anchor     = CENTER;
  parent     =   0;
  z          =      1;

  mouseOver  =  false;
  released   =  false;
  pressed    =  false;
  clicked    =  false;
  focused    =  false;
  visible    =   true;
  active     =   true;
  update     =  false;
}

void GUIRectangle::setParent(GUIRectangle *parentArg)
{
  parent = parentArg;
  update = (parent != 0);
}

GUIRectangle *GUIRectangle::getParent()
{
  return parent;
}

int GUIRectangle::getWidgetType()
{
  	int r=widgetType;
  	if(r<SLIDER)
		r=UNKNOWN;
  	if(r>UNKNOWN)
		r=UNKNOWN;
	return r;
}

void GUIRectangle::forceUpdate(bool updateArg)
{
  update = updateArg;
}

bool GUIRectangle::loadXMLSettings(XMLElement *element)
{
  if(!element)
    return false;

  XMLElement *child    = 0;

  if((child = element->getChildByName("callbackString")) ||
     (child = element->getChildByName("name")          ))
    setCallbackString(child->getValuec());
  else
    return false;//LOG_PRINT_NONE("Need a <name> or <callbackString> attribute, check ")
                   //              + element->getName());

  if(child = element->getChildByName("anchorPoint"))
    setAnchorPoint(child->getValuec());

  if(child = element->getChildByName("visible"))
    setVisible(child->getValue() == "true");

  if(child = element->getChildByName("active"))
    setActive(child->getValue() == "true");

  if(child = element->getChildByName("Position"))
    XMLElement::loadRX_GYf(*child, position);

  if(child = element->getChildByName("Dimensions"))
    XMLElement::loadRX_GYf(*child, dimensions);

  setDimensions(dimensions);
  setPosition(position);
  return true;
}

void  GUIRectangle::enableGUITexture()
{
  //if(parent)
    //parent->enableGUITexture();
}

void  GUIRectangle::disableGUITexture()
{
    
}

void  GUIRectangle::setCallbackString(const NSString& callback)
{
  callbackString = callback;
}

void  GUIRectangle::setCallbackString(const char*   callback)
{
  callbackString = callback;
}

const NSString & GUIRectangle::getCallbackString()
{
  return callbackString;
}

void  GUIRectangle::setAnchorPoint(NSString anchorArg)
{
  if(!anchorArg)
    return;

  if(!strcmp(anchorArg, "CENTER")  ){ anchor = CENTER;   return; }
  if(!strcmp(anchorArg, "CORNERLU")){ anchor = CORNERLU; return; }
  if(!strcmp(anchorArg, "CORNERRU")){ anchor = CORNERRU; return; }
  if(!strcmp(anchorArg, "CORNERLD")){ anchor = CORNERLD; return; }
  if(!strcmp(anchorArg, "CORNERRD")){ anchor = CORNERRD; return; }
}

void  GUIRectangle::setAnchorPoint(int anchorArg)
{
  switch(anchorArg)
  {
    case CORNERLU:
    case CORNERRU:
    case CORNERLD:
    case CORNERRD:
      anchor = anchorArg;
    break;
    default:
      anchor = CENTER;
  }
  update = true;
}

int   GUIRectangle::getAnchorPoint()
{
  return anchor;
}

GUITexCoordDescriptor *GUIRectangle::getTexCoordsInfo(int type)
{
  return !parent? 0 : parent->getTexCoordsInfo(type);
}

void GUIRectangle::setDimensions(const D3DXFROMWINEVECTOR2 &dimensions)
{
  setDimensions(dimensions.x, dimensions.y);
}

void GUIRectangle::setDimensions(float width, float height)
{
  dimensions=D3DXFROMWINEVECTOR2(clampNS(width , 0.01f, 2048.0f),
                 clampNS(height, 0.01f, 2048.0f));
  update = true;
}

const D3DXFROMWINEVECTOR2 &GUIRectangle::getDimensions()
{
  return  dimensions;
}

void GUIRectangle::setPosition(const D3DXFROMWINEVECTOR2 &scales)
{
  setPosition(scales.x, scales.y);
}

void GUIRectangle::setPosition(float xScale, float yScale)
{
  position=D3DXFROMWINEVECTOR2(xScale, yScale);
  update = true;
}

const D3DXFROMWINEVECTOR2 &GUIRectangle::getPosition()
{
  return position;
}

void GUIRectangle::setActive(bool activeArg)
{
  active = activeArg;
}

bool GUIRectangle::isActive()
{
  return active;
}

void GUIRectangle::setVisible(bool visibleArg)
{
  visible = visibleArg;
}

bool  GUIRectangle::isVisible()
{
  return visible;
}

bool  GUIRectangle::isAttached()
{
  return (parent != 0);
}

int GUIRectangle::getWidth()
{
  return windowBounds.z - windowBounds.x;
}

int GUIRectangle::getHeight()
{
  return windowBounds.w - windowBounds.y;
}

const Tuple4i &GUIRectangle::getWindowBounds()
{
  computeWindowBounds();
  return windowBounds;
}

const void GUIRectangle::computeWindowBounds()
{
  if(parent && update)
  {
    const Tuple4i &parentBounds = parent->getWindowBounds();
    z  = parent->getZCoordinate() + 1;
    D3DXFROMWINEVECTOR2 newDimensions,
            newPosition;

    newDimensions.x  = float(parentBounds.z - parentBounds.x);
    newDimensions.y  = float(parentBounds.w - parentBounds.y);

    newPosition.x    = float(parentBounds.x);
    newPosition.y    = float(parentBounds.y);

    newPosition.x    = (position.x <= 1.0f) && (position.x >= 0.0f) ?
                        newDimensions.x*position.x + parentBounds.x :
                       (position.x < 0.0f) ?
                        parentBounds.z + position.x    :
                        position.x     + parentBounds.x;

    newPosition.y    = (position.y <= 1.0f) && (position.y >= 0.0f) ?
                        newDimensions.y*position.y + parentBounds.y :
                       (position.y < 0.0f) ?
                        parentBounds.w + position.y    :
                        position.y     + parentBounds.y;

    newDimensions.x  = (dimensions.x <= 1.0f) ? newDimensions.x*dimensions.x :
                                                dimensions.x;
    newDimensions.y  = (dimensions.y <= 1.0f) ? newDimensions.y*dimensions.y :
                                                dimensions.y;

    windowBounds.x = int(newPosition.x);
    windowBounds.y = int(newPosition.y);
    windowBounds.z = int(newPosition.x + newDimensions.x);
    windowBounds.w = int(newPosition.y + newDimensions.y);

    switch(anchor)
    {
      case CORNERLD:
        windowBounds.y -= int(newDimensions.y);
        windowBounds.w -= int(newDimensions.y);
      break;

      case CORNERRU:
        windowBounds.x -= int(newDimensions.x);
        windowBounds.z -= int(newDimensions.x);
      break;

      case CORNERRD:
        windowBounds.y -= int(newDimensions.y);
        windowBounds.w -= int(newDimensions.y);
        windowBounds.x -= int(newDimensions.x);
        windowBounds.z -= int(newDimensions.x);
      break;

      case CENTER:
        newDimensions /= 2;

        windowBounds.y -= int(newDimensions.y);
        windowBounds.w -= int(newDimensions.y);
        windowBounds.x -= int(newDimensions.x);
        windowBounds.z -= int(newDimensions.x);
      break;
    }
  }
}

GUIEventListener *GUIRectangle::getEventsListener()
{
  return !parent ? 0 : parent->getEventsListener();
}

void GUIRectangle::checkKeyboardEvents(int extraInfo){}
void GUIRectangle::checkMouseEvents(int extraInfo, bool bits)
{
  GUIEventListener *eventsListener = getEventsListener();

  focused = false;
  mouseOver = false;
  clicked = false;
  pressed = false;
  released   = false;

  int mx=STX_Service::GetInputInstance()->GetMouseX();
  int my=STX_Service::GetInputInstance()->GetMouseY();
  bool mouseOver2  = (my >= windowBounds.y) &&
               (my <= windowBounds.w) &&
               (mx >= windowBounds.x) &&
               (mx <= windowBounds.z);
#if 0
  if(mouseOver2)
{
  switch(getWidgetType())
  {
	case SLIDER:
		LOG_PRINT_NONE("SLIDER\n");
		break;
	case BUTTON:
		LOG_PRINT_NONE("BUTTON\n");
		break;
	case CHECK_BOX:
		LOG_PRINT_NONE("CHECK_BOX\n");
		break;
	case CHECK_BOX_MARK:
		LOG_PRINT_NONE("CHECK_BOX_MARK\n");
		break;
	case RADIO_BUTTON:
		LOG_PRINT_NONE("RADIO_BUTTON\n");
		break;
	case CHECK_RB_MARK:
		LOG_PRINT_NONE("CHECK_RB_MARK\n");
		break;
	case LABEL:
		LOG_PRINT_NONE("LABEL\n");
		break;
	case TEXT_AREA:
		LOG_PRINT_NONE("TEXT_AREA\n");
		break;
	case MATERIAL_SURFACE:
		LOG_PRINT_NONE("MATERIAL_SURFACE\n");
		break;
	case PANEL:
		LOG_PRINT_NONE("PANEL\n");
		break;
	case SEPARATOR:
		LOG_PRINT_NONE("SEPARATOR\n");
		break;
	case TEXT_BOX:
		LOG_PRINT_NONE("TEXT_BOX\n");
		break;
	case COMBO_BOX:
		LOG_PRINT_NONE("COMBO_BOX\n");
		break;
	case TABBED_PANEL:
		LOG_PRINT_NONE("TABBED_PANEL\n");
		break;
	case UNKNOWN:
		LOG_PRINT_NONE("UNKNOWN\n");
		break;
	};
#if 0
  LOG_PRINT_NONE("windowBounds.x=%d\n", windowBounds.x);
  LOG_PRINT_NONE("windowBounds.y=%d\n", windowBounds.y);
  LOG_PRINT_NONE("windowBounds.z=%d\n", windowBounds.z);
  LOG_PRINT_NONE("windowBounds.w=%d\n", windowBounds.w);
  LOG_PRINT_NONE("mx=%d\n", mx);
  LOG_PRINT_NONE("my=%d\n", my);
#endif
}
#endif
  if(!mouseOver2)
  {
  	focused = false;
	mouseOver = false;
  }
  if(mouseOver2 && !mouseOver)
  {
  	focused = true;
	mouseOver = true;

  	if(STX_Service::GetInputInstance()->IsMouseClick())
		clicked = true;
  	if(STX_Service::GetInputInstance()->IsMouseButtonPressed())
  		pressed = true;
  	if(STX_Service::GetInputInstance()->IsMouseButtonReleased())
  		released   = true;
  }
#if 0
  if(mouseOver)
	LOG_PRINT_NONE("focused\n");
  if(clicked)
	LOG_PRINT_NONE("clicked\n");
  if(pressed)
	LOG_PRINT_NONE("pressed\n");
  if(released)
	LOG_PRINT_NONE("released\n");
#endif

  if(mouseOver)
  //if(eventsListener && 
	if(eventDetected()){
    GUIEvent* event = new GUIEvent(this);
    if(eventsListener)
	eventsListener->actionPerformed(*event);
    else
	PushGUIEvent(event);
  }
}

bool GUIRectangle::eventDetected(){ return mouseOver ||
                                           released  ||
                                           focused   ||
                                           pressed;   }

bool GUIRectangle::isMouseOver()  { return mouseOver; }
bool GUIRectangle::isReleased()   { return released;  }
bool GUIRectangle::isFocused()    { return focused;   }
bool GUIRectangle::isPressed()    { return pressed;   }
bool GUIRectangle::isClicked()    { return clicked;   }

void GUIRectangle::setZCoordinate(int zArg)
{
  z = clampNS(zArg, 0, 65553);
}

int    GUIRectangle::getZCoordinate()
{
  return z;
}

Tuple2i  GUIRectangle::getCenter()
{
  getWindowBounds();
  return Tuple2i((windowBounds.x + windowBounds.z)/2,
                 (windowBounds.y + windowBounds.w)/2);
}
