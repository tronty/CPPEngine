/* Font
 *
 * Copyright (C) 2003-2004, Alexander Zaprjagaev <frustum@frustum.org>
 * Modified by Abdul Bezrati <abezrati@hotmail.com>
 */

#ifndef __FONT_H__
#define __FONT_H__

#include "../Appearance/Texture.h"

class CFont
{
  public:
    CFont();
   ~CFont();

    void printProportional(float widthRatio , float heightRatio,
                           float width,       float height,
                           float r,  float g, float b,
                           NSString &string);

    void print(float x,     float y,
               float width, float height,
               float r,     float g, float b,
               NSString &string);

    void printSubString(float x,     float y,
                        float width, float height,
                        float r,     float g, float b,
                        int   start, int end,
                        NSString &string);


    Tuple2i getStringDimensions(NSString &string);
    bool    load(NSString fontPath);
    int     getMaxFittingLength(const NSString &string, int bounds);
    int     getHeight();
    const   int *getCharHorizontalGlyphs() const;

  private:
    TextureID   fontTexture;
    int       spaces[256],
              fontHeight;
    int     fontBaseRange;
};

#endif /* __FONT_H__ */
