#ifndef GUI_FRAME_H
#define GUI_FRAME_H

#include "../Appearance/Texture.h"
#include "GUIPanel.h"
#include "../Parsers/XMLParser.h"
#include "../Parsers/IOXMLObject.h"
typedef vector<GUITexCoordDescriptor> GUITexCoordsInfo;

//class Window;
class GUIFrame : public GUIPanel
{
  private:
    GUIEventListener   *listener;
    GUITexCoordsInfo    texCoords;
    Tuple2i             windowSize;
    int                 updateCount;
  public:
    static TextureID             elementsTexture;
    GUIFrame();
   ~GUIFrame();

   //    void setParentWindow(Window *parent);
    bool loadXMLSettings(XMLElement *node);

    void     addOrReplaceTexCoordsInfo(GUITexCoordDescriptor &info);
    void     setElementsTexture(NSString texturePath);
    void     setElementsTexture(const NVTexture &texture);
    void     setGUIEventListener(GUIEventListener *listener);
    //NVTexture *getElementsTexture();

    virtual GUITexCoordDescriptor *getTexCoordsInfo(int type);
    virtual GUIEventListener      *getEventsListener();

    virtual const Tuple4i &getWindowBounds();
    virtual void  forceUpdate(bool update);
    virtual void  enableGUITexture();
    virtual void  disableGUITexture();
    virtual void  render(float tick);
    virtual void  clear();
};

#endif
