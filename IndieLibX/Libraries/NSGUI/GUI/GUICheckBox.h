#ifndef GUI_CBOX_H
#define GUI_CBOX_H

#include "GUIAlphaElement.h"

class GUICheckBox : public GUIAlphaElement
{
  public:
	inline int getItemIndex()
	{
		return getItemIndex(getCallbackString());
	}
    int                     getItemIndex(const NSString &item);
    void                    addItem(const NSString &item);
	  D3DXFROMWINEVECTOR3 m_Translation;
    GUICheckBox(NSString callbackString = 0);

    virtual void render(float clockTick);
    virtual void checkMouseEvents(int extraInfo, bool reservedBits = false);
    virtual bool loadXMLSettings(XMLElement *node);
    virtual void setChecked(bool enabled);

    void  setAlphaMark(float mark);
    float getAlphaMark();

    void  setMinAlphaMark(float mark);
    float getMinAlphaMark();

    bool  isChecked();

    void  setBGColor(const D3DXFROMWINEVECTOR3& color);
    void  setBGColor(float x, float y, float z);
    const D3DXFROMWINEVECTOR3 &getBGColor();
    virtual const Tuple4i &getWindowBounds();

  protected:
    D3DXFROMWINEVECTOR3 bgColor;
    float   markRatio, 
            alphaMark,
            minAlphaMark;
    bool    checked;
    int     extra,
            markOffset,
            primaryTexDesc,
            secondaryTexDesc;
    vector<NSString> items;
	NSString cb;
};

#endif
