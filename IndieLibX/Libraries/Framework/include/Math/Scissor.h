/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
	//-----------------------------------------------------------------------------
// File: Framework\Math\Scissor.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _SCISSOR_H_
#define _SCISSOR_H_

#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>

/** Returns the scissor rect bounds of given light's influence */
bool getScissorFromLight(const D3DXFROMWINEMATRIX &modelview, const D3DXFROMWINEVECTOR3 &lPos, const float r, const float fov, const int width, const int height, int *x, int *y, int *w, int *h);

#endif // _SCISSOR_H_
