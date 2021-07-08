#ifndef GUI_PANEL_H
#define GUI_PANEL_H

#include "../Tools/MathUtils.h"
#include "../Appearance/Texture.h"


#include "../Geometry/DistanceObject.h"
#include "GUITexCoordDescriptor.h"
#include "GUIClippedRectangle.h"
#include "GUIAlphaElement.h"
#include "GUIText.h"

typedef DistanceObject<GUIRectangle*> ZWidget;
typedef vector<GUIRectangle *>        Widgets;
typedef vector<ZWidget>               ZWidgets;

enum {
  FREE_LAYOUT,
  YAXIS_LAYOUT,
  YAXIS_CEN_LAYOUT,
  XAXIS_LAYOUT,
  GRID_LAYOUT
};

class GUIPanel : public GUIRectangle,
                 public GUIClippedRectangle
{
  protected:
	__DWORD__ time;
    Widgets  elements;
    Tuple2i  interval;
    int      layout;
	void setIndexes();

  private:
    void    packFreeLayout();
    void    packYAxisLayout();
    void    packXAxisLayout();
    void    correctPosition();

  public:
    GUIPanel(NSString callback = 0);
   ~GUIPanel();
	int getWidgetIndexByCallbackString(NSString callbackString) const;
    GUIRectangle *getWidgetByCallbackString(NSString name);

	__DWORD__ getTime() const {return time;}
	void setTime(__DWORD__ t2){time=t2;}

    void  setInterval(int width, int height);
    void  setInterval(const Tuple2i &dimensions);
    const Tuple2i &getInterval();

    void  setLayout(int layout);
    int   getLayout();
 
    int   getTreeHeight();

    virtual bool  loadXMLSettings(NSString stackPath);
    virtual bool  loadXMLSettings(XMLElement *node);
 
    virtual void  notify(GUIRectangle *element);
    virtual void  checkMouseEvents(int extraInfo, bool bitInfo = false);
    virtual void  checkKeyboardEvents(int extraInfo);
    virtual void  render(float tick);

    virtual void  collectZWidgets(ZWidgets &container);
    virtual void  forceUpdate(bool);
    virtual bool  addWidget(GUIRectangle *widget);    
    virtual void  pack();
    virtual void  clear();
   
    int           getWidgetCountByType(int type);
	int           getCurrentWidgetIndex() const;
    Widgets      &getWidgets();
};

#endif
