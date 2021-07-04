/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\TexturePacker.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <Util/TexturePacker.h>
#include <memory.h>

bool TNode::addRect(TRect *newRect){
	if (rect == NULL){
		if (back->addRect(newRect)) return true;
		if (front->addRect(newRect)) return true;
	} else {
		if (newRect->width <= rect->width && newRect->height <= rect->height){
			newRect->x = rect->x;
			newRect->y = rect->y;

			back = new TNode();
			back->rect = new TRect;
			back->rect->x = rect->x;
			back->rect->y = rect->y + newRect->height;
			back->rect->width = newRect->width;
			back->rect->height = rect->height - newRect->height;

			front = new TNode();
			front->rect = new TRect;
			front->rect->x = rect->x +  newRect->width;
			front->rect->y = rect->y;
			front->rect->width = rect->width - newRect->width;
			front->rect->height = rect->height;

			delete rect;
			rect = NULL;

			return true;
		}
	}

    return false;
}

void TexturePacker::addTRect(const int width, const int height){
	TRect *rect = new TRect;
	rect->width = width;
	rect->height = height;

	rects.add(rect);
}

void TexturePacker::addTRect(int width, int height, const int tileSize){
	if (width  < tileSize) width  = tileSize;
	if (height < tileSize) height = tileSize;

	width  = (width  + (tileSize >> 1)) & ~(tileSize - 1);
	height = (height + (tileSize >> 1)) & ~(tileSize - 1);

	addTRect(width, height);
}

TexturePacker::~TexturePacker(){
	for (unsigned int i = 0; i < rects.getCount(); i++){
		delete rects[i];
	}
}

int areaComp(const void *e0, const void *e1){
	return (*(TRect **) e1)->width * (*(TRect **) e1)->height - 
		   (*(TRect **) e0)->width * (*(TRect **) e0)->height;
}

int widthComp(const void *e0, const void *e1){
	return (*(TRect **) e1)->width - (*(TRect **) e0)->width;
}

int heightComp(const void *e0, const void *e1){
	return (*(TRect **) e1)->height - (*(TRect **) e0)->height;
}

bool TexturePacker::constructPackedTexture(const int width, const int height, compTRect compFunc){
	TRect *oldEntries = new TRect[rects.getCount()];
	stx_memcpy(oldEntries, rects.getArray(), rects.getCount() * sizeof(TRect *));

	qsort(rects.getArray(), rects.getCount(), sizeof(TRect *), compFunc);

	TNode *top = new TNode();
	top->rect = new TRect;
	top->rect->x = 0;
	top->rect->y = 0;
	top->rect->width  = width;
	top->rect->height = height;

	bool result = true;
	for (unsigned int i = 0; i < rects.getCount(); i++){
		result &= top->addRect(rects[i]);
	}

	delete top;

	stx_memcpy(rects.getArray(), oldEntries, rects.getCount() * sizeof(TRect *));
	delete oldEntries;

	return result;
}
