#ifndef GUI_SLIDER_H
#define GUI_SLIDER_H

#include "GUIAlphaElement.h"
#include "GUISeparator.h"

class GUISlider : public GUIAlphaElement
{
  public:
	  D3DXFROMWINEVECTOR3 m_Translation;
    GUISlider(NSString callback    = 0,
              const int   orientation = HORIZONTAL);

    virtual const Tuple4i &getWindowBounds();
    virtual void  checkMouseEvents(int extreaInfo, bool reservedBits = false);
    virtual bool  loadXMLSettings(XMLElement *node);
    virtual void  render(float clockTick);

    float getProgress();
    void  setProgress(float zeroToOne);

    void  setDiscDimensions(const Tuple2i& dimensions);
    void  setDiscDimensions(int width, int height);
    int   getOrientation();

    const Tuple2i &getDiscDimensions();

  private:
    Tuple4i realWindowBounds;
    Tuple2i discDimensions; 
    float   progress;
    int     orientation, 
            offset;
  
    void    renderVertical();
};

#endif
