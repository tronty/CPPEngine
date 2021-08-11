/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\TexturePacker.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _TEXTUREPACKER_H_
#define _TEXTUREPACKER_H_

/** \file
    A texture packing class. It takes rectangles as input and tries to pack them into one large
	texture. This is useful for instance for light-mapping. The packing isn't neccesarily optimal,
	but fairly good. A simple BSP-esque method is used to distribute the rectangles over the
	available space.
*/

#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>
#include "VArray.h"

struct TRect {
	int x, y;
	int width, height;
};

struct TNode {
	TNode(){
		front = back = NULL;
        rect = NULL;
	}
	~TNode(){
		delete front;
		delete back;
		delete rect;
	}

	bool addRect(TRect *newRect);

	TNode *front;
	TNode *back;

	TRect *rect;
};

typedef int (*compTRect)(const void *e0, const void *e1);

int areaComp(const void *e0, const void *e1);
int widthComp(const void *e0, const void *e1);
int heightComp(const void *e0, const void *e1);

class TexturePacker {
public:
	~TexturePacker();

	void addTRect(const int width, const int height);
	// tileSize must be power of 2.
	void addTRect(const int width, const int height, const int tileSize);

	TRect *getRect(const unsigned int index) const { return rects[index]; }

	bool constructPackedTexture(int width, int height, compTRect compFunc = areaComp);


protected:
	VArray_ <TRect *> rects;
};


#endif
