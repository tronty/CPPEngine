#ifndef GUI_RECTANGLE_H
#define GUI_RECTANGLE_H

#include "../Tools/MathUtils.h"
#include "GUITexCoordDescriptor.h"
//include "../Extensions/GLee.h"


#include "GUIEventListener.h"
#include "Event.h"

enum WidgetEvents
{
  CLICKED = 1,
  DOUBLE_CLICKED,
  PRESSED,
  DRAGGED,
  RELEASED,
  MOVED
};

enum  AnchorPoints
{
  CENTER,
  CORNERLU,
  CORNERRU,
  CORNERLD,
  CORNERRD
};

class GUIRectangle
{
  protected:
    GUIRectangle *parent;

    Tuple4i       windowBounds;
    D3DXFROMWINEVECTOR2       position,
                  dimensions;

    NSString        callbackString;

    bool          mouseOver,
                  released,
                  focused,
                  pressed,
                  clicked,
                  visible,
                  active,
                  update;

    int           lastAction,
                  widgetType,
                  anchor,
                  z,
		index;
	int currentElement;               

  public:
    GUIRectangle(const char* callback = 0);
	virtual ~GUIRectangle(){}
	int getCurrentElement() const {return currentElement;}
	void setCurrentElement(int a){currentElement=a;}

    void setIndex(int t){index=t;}

    void           setCallbackString(const NSString& callback);
    void           setCallbackString(const char*   callback);
    const NSString & getCallbackString();   
   
    bool     loadXMLSettings(XMLElement *node);

    void     setDimensions(float width, float height);
    void     setDimensions(const D3DXFROMWINEVECTOR2 &dimensions);
    const    D3DXFROMWINEVECTOR2 &getDimensions();

    void     setPosition(float xScaleOrPosition, float yScaleOrPosition);
    void     setPosition(const D3DXFROMWINEVECTOR2 &scalesOrPosition);
    const    D3DXFROMWINEVECTOR2 &getPosition();
    virtual  Tuple2i  getCenter();

    int   getWidth();
    int   getHeight();

    void  setActive(bool active);
    bool  isActive();

    void  setAnchorPoint(NSString anchor);
    void  setAnchorPoint(int anchor);
    int   getAnchorPoint();

    virtual void  forceUpdate(bool update);

    void  setVisible(bool active);
    bool  isVisible();

    bool  isAttached();
    int   getWidgetType();

    virtual void checkMouseEvents(int extraInfo, bool reservedBits = false);
    virtual void checkKeyboardEvents(int extraInfo);
    virtual void render(float) = 0;

    virtual void          setParent(GUIRectangle *parent);
    virtual GUIRectangle *getParent();

    virtual  GUITexCoordDescriptor *getTexCoordsInfo(int type);
    virtual  GUIEventListener      *getEventsListener();

    virtual void  enableGUITexture();
    virtual void  disableGUITexture();

    virtual void  setZCoordinate(int z);
    virtual int   getZCoordinate();

    virtual const void     computeWindowBounds();
    virtual const Tuple4i &getWindowBounds();

    bool  eventDetected();
    bool  isMouseOver();
    bool  isReleased();
    bool  isFocused();
    bool  isDragged();
    bool  isClicked();
    bool  isPressed();
};

#endif
