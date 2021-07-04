/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
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
//-----------------------------------------------------------------------------
#include "Asteroid.h"

Asteroid::Asteroid() 
{
	position = D3DXFROMWINEVECTOR3(0,0,0);

	float rscale = 1.f;
	for(int exp=0;exp<3;exp++)
		rscale *= (float)rand()/32768.0f;
	float fscale = 3.f +10.f*rscale;
	scale = D3DXFROMWINEVECTOR3(fscale,fscale,fscale);

	float cg = 0.25f + 0.25f*(float)rand()/32768.0f;
	color = D3DXFROMWINECOLOR(cg,cg,cg,1);	

	float x = 2.f*((float)rand()/32768.0f-0.5f);
	float y = 2.f*((float)rand()/32768.0f-0.5f);
	float z = 2.f*((float)rand()/32768.0f-0.5f);
	rotationAxis = D3DXFROMWINEVECTOR3(x,y,z);
	D3DXFROMWINEVec3Normalize(&rotationAxis,&rotationAxis);

	radius = 3.0f*fscale;
	radiusSq = radius*radius;

	D3DXFROMWINEQuaternionRotationYawPitchRoll(&rotation,0,0,0);

	speed = 0.5f + (float)rand()/32768.0f;;
}

void Asteroid::MakeRandomPosition(D3DXFROMWINEVECTOR3 &worldMin,D3DXFROMWINEVECTOR3 &worldMax)
{
	float x = (float)rand()/32768.0f;
	float y = (float)rand()/32768.0f;
	float z = (float)rand()/32768.0f;

	D3DXFROMWINEVECTOR3 random = worldMax - worldMin;
	random.x *= x;
	random.y *= y;
	random.z *= z;

	position = worldMin + random;
	quadrant = (position.x > 0.f?1:0)<<2 &&
		(position.y > 0.f?1:0)<<1 &&
		(position.z > 0.f?1:0);
}
