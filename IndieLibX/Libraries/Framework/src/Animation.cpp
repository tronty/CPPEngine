/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\Animation.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <Util/Animation.h>

void Quaternion2Mat4(D3DXFROMWINEMATRIX& m, float* q)
{
	float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

	s =2.0f;
	//s = 2.0f/(q[0]*q[0] + q[0]*q[1] + q[2]*q[2] + q[3]*q[3]);		

	xs = q[0]*s;        ys = q[1]*s;        zs = q[2]*s;
	wx = q[3]*xs;       wy = q[3]*ys;       wz = q[3]*zs;
	xx = q[0]*xs;       xy = q[0]*ys;       xz = q[0]*zs;
	yy = q[1]*ys;       yz = q[1]*zs;       zz = q[2]*zs;

	
	m.m[0][0] = 1.0f - (yy+zz);
	m.m[0][1] = xy + wz;
	m.m[0][2] = xz - wy;

	m.m[1][0] = xy - wz;
	m.m[1][1] = 1.0f - (xx+zz);	
	m.m[1][2] = yz + wx;
	
	m.m[2][0] = xz + wy;
	m.m[2][1] = yz - wx;
	m.m[2][2] = 1.0f - (xx + yy);

	m.m[3][0] = m.m[3][1] = m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;		
}

Animation::Animation(){
	rangeStart = rangeEnd = 0.0f;
	currentTime = 0.0f;
	positionKeys.numKeys = 0;
	positionKeys.pKey = NULL;
	rotationKeys.numKeys = 0;
	rotationKeys.pKey = NULL;
	scaleKeys.numKeys = 0;
	scaleKeys.pKey = NULL;
	transformMatrix = identity4();
}

Animation::~Animation(){
	delete [] positionKeys.pKey;
	delete [] rotationKeys.pKey;
	delete [] scaleKeys.pKey;
}

void Animation::getAnimationKeyValue(AnimationKeys &ak, float time, float *lpf, int num) {
	float					Offset;
    __DWORD__					i, Key;
	int						Top, Bottom, Mid;
	AnimationKey*			lpCK;	
	float					*lpV1, *lpV2, *lpR = lpf;
	
	if (ak.numKeys >= 2)
	{
		Top = 0;
		Key = ak.numKeys;
		Bottom = ak.numKeys;
		while (Top <= Bottom)
		{
			Mid = ((Top + Bottom) >> 1);					
			if (time >= ak.pKey[Mid].endTime)
			{
				Top = Mid + 1;
			}
			else if (time < ak.pKey[Mid].startTime)			
			{
				Bottom = Mid - 1;
			}
			else
			{
				Key = Mid;
				break;
			}
		}			
		lpCK = &ak.pKey[Key];
		if (Key >= ak.numKeys-1)
		{
			stx_memcpy(lpf, ak.pKey[Key].value, sizeof(float) * num);
			return;
		}
		lpV1 = lpCK->value;
		lpV2 = (lpCK + 1)->value;    

		Offset = (time-lpCK->startTime)/(lpCK->endTime - lpCK->startTime);		
		for(i=num; i>0; i--) 
		{		
			*lpR = (*lpV1) + (((*lpV2) - (*lpV1))*Offset);
			lpV1++;
			lpV2++;
			lpR++;
		}
	}
	else
	{
		stx_memcpy(lpf, ak.pKey[0].value, sizeof(float) * num);
	}
}

void Animation::CalculateTransformMatrix() {
	float value[4];
	transformMatrix = identity4();
	if (rotationKeys.numKeys)
	{
		getAnimationKeyValue(rotationKeys, currentTime, value, 4);
		Quaternion2Mat4(transformMatrix, value);
	}	
	if (scaleKeys.numKeys)
	{
		getAnimationKeyValue(scaleKeys, currentTime, value, 3);
		transformMatrix.m[0][0] *= value[0];
		transformMatrix.m[1][0] *= value[1];
		transformMatrix.m[2][0] *= value[2];

		transformMatrix.m[0][1] *= value[0];
		transformMatrix.m[1][1] *= value[1];
		transformMatrix.m[2][1] *= value[2];

		transformMatrix.m[0][2] *= value[0];
		transformMatrix.m[1][2] *= value[1];
		transformMatrix.m[2][2] *= value[2];
	}
	if (positionKeys.numKeys)
	{
		getAnimationKeyValue(positionKeys, currentTime, value, 3);
		transformMatrix.m[3][0] += value[0];
		transformMatrix.m[3][1] += value[1];
		transformMatrix.m[3][2] += value[2];
	}
}

bool Animation::play(float step, bool bloop) {
	bool bReachEnd = false;

	CalculateTransformMatrix();
	currentTime += step;
	if (bloop)
	{
		if (currentTime >= rangeEnd)
		{
			currentTime = rangeStart;
			bReachEnd = true;
		}
	}
	else
	{
		if (currentTime >= rangeEnd)
		{
			currentTime = rangeEnd;
			bReachEnd = true;
		}
	}
	return bReachEnd;
}
