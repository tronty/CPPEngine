/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
	//-----------------------------------------------------------------------------
// File: Framework\Util\Bone.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _BONE_H_
#define _BONE_H_

#include <Framework3/IRenderer.h>
#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>
#include "Transform.h"

class Bone : public Transform {
protected:
public:
	Bone();
	~Bone();
};

#endif // _BONE_H_
