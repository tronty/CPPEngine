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
// 
// File: SpaceShipManager.h
// Desc: This wraps up the physics simulation of the space ships.
//			it includes collision with the asteroids and motherships
//-----------------------------------------------------------------------------
#pragma once
#include "SpaceShip.h"
#include "AsteroidManager.h"
#include "MothershipManager.h"

class SpaceShipManager
{
public:
	SpaceShipManager();
	void SetWorldExtents(D3DXFROMWINEVECTOR3 &wmin,D3DXFROMWINEVECTOR3 &wmax);
	void Init(int num);

	// Mega simple netwtonian physics
	void Update(float deltaTime,AsteroidManager *asteroidManager, MothershipManager *mothershipManager);
	D3DXFROMWINEMATRIX MakeMatrixFor(int iShip);
	D3DXFROMWINECOLOR GetColorFor(int iShip);

	D3DXFROMWINEVECTOR3 AsteroidCollisionCheck(int iShip,AsteroidManager *asteroidManager);
	D3DXFROMWINEVECTOR3 MothershipCollisionCheck(int iShip,MothershipManager *mothershipManager);

	D3DXFROMWINEQUATERNION VectorToQuat(D3DXFROMWINEVECTOR3 v);
	D3DXFROMWINEMATRIX VectorToMatrix(D3DXFROMWINEVECTOR3 v);
	D3DXFROMWINEVECTOR3 QuatToVector(D3DXFROMWINEQUATERNION q);
	D3DXFROMWINEVECTOR3 *SafeNormalize(D3DXFROMWINEVECTOR3 *dest,D3DXFROMWINEVECTOR3 *src);

	float accel;
	D3DXFROMWINEVECTOR3 worldMin;
	D3DXFROMWINEVECTOR3 worldMax;
	SpaceShip *ships;
	int numShips;
	D3DXFROMWINEVECTOR3 lastAveragePosition;
};
