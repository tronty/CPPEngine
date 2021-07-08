/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\VArray_.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <stdlib.h>

template <class ARG_TYPE>
class VArray_ {
public:
	VArray_(){
		count = capasity = 0;
		list = NULL;
	}
	
	~VArray_(){
		free(list);
	}

	ARG_TYPE &operator [] (const unsigned int index) const {
		return list[index];
	}

	ARG_TYPE *getArray() const { return list; }
	unsigned int getCount() const { return count; }
	unsigned int getCapasity() const { return capasity; }

	unsigned int add(const ARG_TYPE object){
		if (count >= capasity){
			if (capasity) capasity += capasity; else capasity = 8;
			list = (ARG_TYPE *) realloc(list, capasity * sizeof(ARG_TYPE));
		}
		list[count] = object;
		return count++;
	}

	void fastRemove(const unsigned int index){
		count--;
		list[index] = list[count];
	}

	void orderedRemove(const unsigned int index){
		count--;
		memmove(list + index, list + index + 1, (count - index) * sizeof(ARG_TYPE));
	}

	void setCount(const unsigned int newCount){
		capasity = count = newCount;
		list = (ARG_TYPE *) realloc(list, capasity * sizeof(ARG_TYPE));		
	}

	void clear(){ count = 0; }
	void reset(){
		count = capasity = 0;
		free(list);
		list = NULL;
	}

protected:
	unsigned int capasity;
	unsigned int count;
	ARG_TYPE *list;
};

#endif // _ARRAY_H_

