/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\Transform.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <Util/Transform.h>

Transform::Transform(){
	transformMatrix = identity4();
	parent = -1;
	numChildren = 0;
	pChildren = NULL;
}

Transform::~Transform(){
	delete [] pChildren;
}

