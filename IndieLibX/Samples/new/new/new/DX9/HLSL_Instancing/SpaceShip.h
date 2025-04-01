/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//-----------------------------------------------------------------------------
// Copyright NVIDIA Corporation 2004
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED 
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS 
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL 
// NVIDIA OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR 
// CONSEQUENTIAL DAMAGES WHATSOEVER INCLUDING, WITHOUT LIMITATION, DAMAGES FOR 
// LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS 
// INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR 
// INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGES.
// 
// File: SpaceShip.h
// Desc: Simple struct to contain space ship data
//-----------------------------------------------------------------------------

#pragma once

struct SpaceShip
{
public:
	SpaceShip() 
	{
		position = D3DXFROMWINEVECTOR3(0,0,0);
		//orientation = D3DXFROMWINEVECTOR3(0.0f,1.0f,0.0f);
		D3DXFROMWINEQuaternionIdentity(&qOrientation);

		//D3DXFROMWINEMatrixIdentity(&mOrientation);

		float fscale = 4.75f + 0.25f*(float)stx_rand()/32768.0f;
		scale = 0.1f*D3DXFROMWINEVECTOR3(fscale,fscale,fscale);
		currentVelocity = D3DXFROMWINEVECTOR3(0,0,0);
		idealVelocity = D3DXFROMWINEVECTOR3(0,0,0);

		float cr = 0.25f + 0.5f*(float)stx_rand()/32768.0f;
		float cg = 0.25f + 0.5f*(float)stx_rand()/32768.0f;
		float cb = 0.25f + 0.5f*(float)stx_rand()/32768.0f;
		if(cr > cb && cr > cg) cr = 1.f;
		else if(cb > cr && cb > cg) cb = 1.f;
		else cg = 1.f;
		color = D3DXFROMWINECOLOR(cr,cg,cb,1);	

		speed = 15.f+5.f*(float)stx_rand()/32768.0f;
		radius = 1.0f*fscale;
		radiusSq = radius*radius;
		targetMS = stx_rand()/(32768/10);
;	}

	void MakeRandomPosition(D3DXFROMWINEVECTOR3 &worldMin,D3DXFROMWINEVECTOR3 &worldMax)
	{
		float x = (float)stx_rand()/32768.0f;
		float y = (float)stx_rand()/32768.0f;
		float z = (float)stx_rand()/32768.0f;

		D3DXFROMWINEVECTOR3 random = worldMax - worldMin;
		random.x *= x;
		random.y *= y;
		random.z *= z;

		position = worldMin + random;
	}

	D3DXFROMWINEVECTOR3 position;
	D3DXFROMWINEQUATERNION qOrientation;
	//D3DXFROMWINEMATRIX  mOrientation;
	D3DXFROMWINEVECTOR3 orientation;
	D3DXFROMWINEVECTOR3 scale;	

	D3DXFROMWINEVECTOR3 currentVelocity;
	D3DXFROMWINEVECTOR3 idealVelocity;

	float radius;
	float radiusSq;
	float timeTillChange;
	int quadrant;
	int targetMS;

	D3DXFROMWINECOLOR color;
	float speed;
};
