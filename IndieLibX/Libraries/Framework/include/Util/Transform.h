/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
	//-----------------------------------------------------------------------------
// File: Framework\Util\Transform.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <Framework3/IRenderer.h>
#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>
#include "Animation.h"

class Transform {
protected:
	D3DXFROMWINEMATRIX transformMatrix;
	Animation animation;
	int	parent;
	__DWORD__ numChildren;
	__DWORD__ *pChildren;

public:
	Transform();
	~Transform();

	inline bool play(float step = 1.0f, bool bloop = true) {return animation.play(step, bloop); };	
	inline void setPosition(const D3DXFROMWINEVECTOR3& p) {transformMatrix.m[3][0] = p.x; transformMatrix.m[3][1] = p.y; transformMatrix.m[3][2] = p.z; };
	inline void getPosition(D3DXFROMWINEVECTOR3& p) {p.x = transformMatrix.m[3][0]; p.y = transformMatrix.m[3][1]; p.z = transformMatrix.m[3][2]; };
	inline void setTransformMatrix(const D3DXFROMWINEMATRIX& m) {transformMatrix = m; };
	inline const D3DXFROMWINEMATRIX& getTransformMatrix() {return transformMatrix; };
	inline void setRange(float s, float e) {animation.setRange(s, e);};
	inline void setTime(float t) {animation.setTime(t);};
	inline const float getTime() {return animation.getTime();};
	inline const __DWORD__ getNumChildren() {return numChildren; };
	inline const __DWORD__ getChild(int c) {return pChildren[c]; };

	friend class ATIModel;
};

#endif // _TRANSFORM_H_
