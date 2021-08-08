/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\Animation.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>
#include <Framework3/IRenderer.h>

struct AnimationKey{
	int startTime, endTime;
	float value[4]; 
};

struct AnimationKeys{
	__DWORD__ numKeys;
	AnimationKey *pKey; 
};

class Animation {
protected:
	float currentTime, rangeStart, rangeEnd;
	D3DXFROMWINEMATRIX transformMatrix;
	AnimationKeys positionKeys, rotationKeys, scaleKeys;

 public:
	Animation();
	~Animation();

	void getAnimationKeyValue(AnimationKeys &ak, float time, float *lpf, int num);
	inline void setCurrentTime(float t) {currentTime = t; };
	inline void setRange(float s, float e) {rangeStart = s; rangeEnd = e; };
	inline void setTime(float t) {currentTime = t;};
	inline const float getTime() {return currentTime;};
	bool play(float step, bool bloop = true);
	inline const D3DXFROMWINEMATRIX& getTransformMatrix() {return transformMatrix; };

protected:
	void CalculateTransformMatrix();

	friend class ATIModel;
};
#endif // _ANIMATION_H_

