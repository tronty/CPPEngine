#ifndef GUI_CLIPPED_RECTANGLE_H
#define GUI_CLIPPED_RECTANGLE_H

#include "GUIRectangle.h"
#include "../Appearance/Texture.h"

class GUIClippedRectangle
{
  protected:
    NVTexture  texture;
    D3DXFROMWINEVECTOR2  texCoords[7];

    D3DXFROMWINEVECTOR4  textureRectangle,
             bgColor;

    D3DXFROMWINEVECTOR3  bordersColor;
    Tuple2i  vertices[7];
    bool     drawBackground,
             drawBounds;
    int      clipSize;

  public:
    GUIClippedRectangle(int clipSize = 5);
    void  enableBGColor(bool enable);
    bool  isBGColorOn();

    void  computeClippedBounds(const Tuple4i &windowBounds);

    void  setClipSize(int clipSize);
    int   getClipSize();

    void  setVisibleBounds(bool visible);
    bool  boundsVisible();

    void  setBGColor(float x, float y, float z, float alpha);
    void  setBGColor(const D3DXFROMWINEVECTOR4& color);
    const D3DXFROMWINEVECTOR4 &getBGColor();
    const Tuple2i *getVertices() const;

    void  setBordersColor(const D3DXFROMWINEVECTOR3& color);
    void  setBordersColor(float x, float y, float z);
    const D3DXFROMWINEVECTOR3 &getBordersColor();
    const D3DXFROMWINEVECTOR2 *getTexCoords() const;

    void  setTextureRectangle(float x, float y, float z, float w);
    void  setTextureRectangle(const D3DXFROMWINEVECTOR4 &textureRectangle);
    const D3DXFROMWINEVECTOR4 &getTextureRectangle();

    NVTexture &getTexture();
    void     setTexture(const NVTexture & texture);

    virtual void renderClippedBounds(D3DXFROMWINEVECTOR3* Translation=0);
    virtual bool loadXMLClippedRectangleInfo(XMLElement *node);
};

#endif
