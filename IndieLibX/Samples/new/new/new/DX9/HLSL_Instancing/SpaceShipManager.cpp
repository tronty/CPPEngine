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
//-----------------------------------------------------------------------------

#include "SpaceShipManager.h"

SpaceShipManager::SpaceShipManager()
{
	ships = 0;
	numShips = 0;
	accel = 5.f;
}



void SpaceShipManager::SetWorldExtents(D3DXFROMWINEVECTOR3 &wmin,D3DXFROMWINEVECTOR3 &wmax)
{
	worldMin = wmin;
	worldMax = wmax;
}

void SpaceShipManager::Init(int num)
{
	SpaceShip *newships;
	if(num > 0)
	{
		newships = new SpaceShip[num];
	}

	if(ships != 0)
	{
		// preserve existing ships
		if(num > 0 && numShips > 0)
		{
			int numToCopy = num>numShips?numShips:num;
			stx_memcpy(newships,ships,numToCopy*sizeof(SpaceShip));
			for(int iShip=numShips;iShip<num;iShip++)
			{
				newships[iShip].MakeRandomPosition(worldMin,worldMax);
			}
		}

		delete[]ships;
	}
	else
	{
		for(int iShip=0;iShip<num;iShip++)
		{
			newships[iShip].MakeRandomPosition(worldMin,worldMax);
		}
	}
	ships = newships;
	numShips = num;
}

D3DXFROMWINEVECTOR3 SpaceShipManager::AsteroidCollisionCheck(int iShip,AsteroidManager *asteroidManager)
{
	for(int iRock=0;iRock<asteroidManager->numRocks;iRock++)
	{
		// early out for speed
		if(ships[iShip].quadrant != asteroidManager->rocks[iRock].quadrant) 
			continue;

		D3DXFROMWINEVECTOR3 diff = ships[iShip].position - asteroidManager->rocks[iRock].position;
		float actualDistSq = D3DXFROMWINEVec3LengthSq(&diff);
		float minDistSq = ships[iShip].radiusSq+asteroidManager->rocks[iRock].radiusSq;
		if(actualDistSq < minDistSq)
			return diff;
	}
	return D3DXFROMWINEVECTOR3(0,0,0);
}

D3DXFROMWINEVECTOR3 SpaceShipManager::MothershipCollisionCheck(int iShip,MothershipManager *mothershipManager)
{
	for(int iMShip=0;iMShip<mothershipManager->numShips;iMShip++)
	{
		// early out for speed
		if(ships[iShip].quadrant != mothershipManager->ships[iMShip].quadrant) 
			continue;

		D3DXFROMWINEVECTOR3 diff = ships[iShip].position - mothershipManager->ships[iMShip].position;
		float actualDistSq = D3DXFROMWINEVec3LengthSq(&diff);
		float minDistSq = ships[iShip].radiusSq+mothershipManager->ships[iMShip].radiusSq;
		if(actualDistSq < minDistSq)
			return diff;
	}
	return D3DXFROMWINEVECTOR3(0,0,0);
}

// Mega simple netwtonian physics
void SpaceShipManager::Update(float deltaTime,AsteroidManager *asteroidManager, MothershipManager *mothershipManager)
{
	if(deltaTime > .2f) deltaTime = 0.2f;
	D3DXFROMWINEVECTOR3 aggregatePosition;
	for(int iShip=0;iShip<numShips;iShip++)
	{
		// First check position against extents and maybe turn the ship around
		D3DXFROMWINEVECTOR3 diffMin = ships[iShip].position - worldMin;
		D3DXFROMWINEVECTOR3 diffMax = worldMax - ships[iShip].position;
		float moveAmmount = deltaTime*ships[iShip].speed;
		D3DXFROMWINEVECTOR3 fromRock;

		ships[iShip].timeTillChange -= deltaTime;

		// Hit world extents or an asteroid, turn us around
		if(diffMin.x < 0.f || diffMin.y < 0.f || diffMin.z < 0.f ||
			diffMax.x < 0.f || diffMax.y < 0.f || diffMax.z < 0.f )
		{
			// head toward origin
			D3DXFROMWINEVECTOR3 temp;
			SafeNormalize(&temp,&(-ships[iShip].position));
			ships[iShip].idealVelocity = temp*ships[iShip].speed;
		}
		// if collided with a rock, then head away
		else if((fromRock = AsteroidCollisionCheck(iShip,asteroidManager)).x !=0.f ||
				fromRock.y != 0.f || fromRock.z != 0.f ||
				(fromRock = MothershipCollisionCheck(iShip,mothershipManager)).x !=0.f ||
				fromRock.y != 0.f || fromRock.z != 0.f)
		{
			SafeNormalize(&fromRock,&fromRock);
			ships[iShip].idealVelocity = fromRock*ships[iShip].speed;
		}
		// randomly tweak the ships heading
		else if(ships[iShip].timeTillChange < 0.f)
		{
			// randomly pick a -0.1 to 0.1 range
			float x = 0.1f*((((float)stx_rand()/32768.f)-0.5f)*2.f);
			float y = 0.1f*((((float)stx_rand()/32768.f)-0.5f)*2.f);
			float z = 0.1f*((((float)stx_rand()/32768.f)-0.5f)*2.f);

			ships[iShip].timeTillChange = 0.5f + 0.5f*((float)stx_rand()/32768.f);

			// jitter direction based on orientation
			D3DXFROMWINEVECTOR3 adjust = D3DXFROMWINEVECTOR3(x,y,z);
			D3DXFROMWINEVECTOR3 orientation;
			SafeNormalize(&orientation,&ships[iShip].currentVelocity);
			D3DXFROMWINEVECTOR3 temp = adjust + orientation;

			// if not heading generally toward common position, then try negative adjust
			D3DXFROMWINEVECTOR3 toAverage = lastAveragePosition - ships[iShip].position;
			if(D3DXFROMWINEVec3Dot(&temp,&toAverage) < 0.f)
			{
				temp = orientation - adjust; 
			}

			// renorm
			D3DXFROMWINEVec3Normalize(&temp,&temp);

			// assign based on speed
			ships[iShip].idealVelocity = temp*ships[iShip].speed;
		}

		D3DXFROMWINEQUATERNION qIdealOrientation;
		D3DXFROMWINEVECTOR3 temp;
		SafeNormalize(&temp,&ships[iShip].idealVelocity);
		qIdealOrientation = VectorToQuat(temp);

		// Adjust current Vel towards ideal velocity
		D3DXFROMWINEVECTOR3 diffVel = ships[iShip].idealVelocity - ships[iShip].currentVelocity;
		float diffLen = D3DXFROMWINEVec3LengthSq(&diffVel);
		float pct = 0.f;

		// No change to velocity unless necessary.  if no diff, then we angle to ideal orientation
		if(diffLen > 0.f)
		{
			diffLen = sqrt(diffLen);

			D3DXFROMWINEVec3Normalize(&diffVel,&diffVel);
			float velChange = min(diffLen , accel * deltaTime);
			pct = velChange / diffLen;
			ships[iShip].currentVelocity += diffVel * velChange;
		}

		D3DXFROMWINEQuaternionSlerp(&ships[iShip].qOrientation,&ships[iShip].qOrientation,&qIdealOrientation,deltaTime);
		//ships[iShip].qOrientation = qIdealOrientation;

		ships[iShip].position += ships[iShip].currentVelocity * deltaTime;
		aggregatePosition += ships[iShip].position;

		ships[iShip].quadrant = (ships[iShip].position.x > 0.f?1:0)<<2 &&
								(ships[iShip].position.y > 0.f?1:0)<<1 &&
								(ships[iShip].position.z > 0.f?1:0);
	
	}

	// store a last average position to "flock" toward
	lastAveragePosition = ships[0].position;//aggregatePosition / (float)numShips;
}

D3DXFROMWINEMATRIX SpaceShipManager::MakeMatrixFor(int iShip)
{
	D3DXFROMWINEMATRIX rotation;
	D3DXFROMWINEMatrixRotationQuaternion(&rotation,&ships[iShip].qOrientation);
	D3DXFROMWINEMATRIX temp;
	D3DXFROMWINEMatrixRotationAxis(&temp,&D3DXFROMWINEVECTOR3(0,0,-1),3.1415f/2.f);
	rotation = temp*rotation;
	
	D3DXFROMWINEMATRIX scaling; D3DXFROMWINEMatrixScaling(&scaling,ships[iShip].scale.x,ships[iShip].scale.y,ships[iShip].scale.z);
	D3DXFROMWINEMATRIX translation; D3DXFROMWINEMatrixTranslation(&translation,ships[iShip].position.x,ships[iShip].position.y,ships[iShip].position.z);
	D3DXFROMWINEMATRIX world = scaling*rotation*translation;
	return world;
}

D3DXFROMWINECOLOR SpaceShipManager::GetColorFor(int iShip)
{
	return ships[iShip].color;
}

D3DXFROMWINEQUATERNION SpaceShipManager::VectorToQuat(D3DXFROMWINEVECTOR3 v)
{
	D3DXFROMWINEMATRIX rotation = VectorToMatrix(v);

	// turn into quaternion
	D3DXFROMWINEQUATERNION quat;
	D3DXFROMWINEQuaternionRotationMatrix(&quat,&rotation);
	return quat;
}

D3DXFROMWINEMATRIX SpaceShipManager::VectorToMatrix(D3DXFROMWINEVECTOR3 v)
{
	D3DXFROMWINEMATRIX rotation;
	D3DXFROMWINEVec3Normalize(&v,&v);
	D3DXFROMWINEVECTOR3 basisX;
	D3DXFROMWINEVECTOR3 basisY;
	D3DXFROMWINEVECTOR3 basisZ;

	// The X component is face forward, i.e. the passed in vector
	basisX = v;

	// Y is up, just find an appropriate value by simple math
	basisY = v;
	float x = fabs(v.x);
	float y = fabs(v.y);
	float z = fabs(v.z);
	if(x < y && x < z)
	{
		basisY.x = 0.0f;
		float temp = basisY.y;
		basisY.y = -basisY.z;
		basisY.z = temp;
	}
	else if(y < x && y < z)
	{
		basisY.y = 0.0f;
		float temp = basisY.x;
		basisY.x = -basisY.z;
		basisY.z = temp;
	}
	else
	{
		basisY.z = 0.0f;
		float temp = basisY.x;
		basisY.x = -basisY.y;
		basisY.y = temp;
	}
	D3DXFROMWINEVec3Normalize(&basisY,&basisY);

	// Z is just gotten from crossing X and Y
	D3DXFROMWINEVec3Cross(&basisZ,&basisX,&basisY);
	D3DXFROMWINEVec3Normalize(&basisZ,&basisZ);

	rotation = D3DXFROMWINEMATRIX(	basisX.x,basisY.x,basisZ.x,0,
		basisX.y,basisY.y,basisZ.y,0,
		basisX.z,basisY.z,basisZ.z,0,
		0,0,0,1);

	// invers transpose to ensure rotation only
	D3DXFROMWINEMatrixTranspose(&rotation,D3DXFROMWINEMatrixInverse(&rotation,0,&rotation));

	// invert(transpose is same for rotations only) to provide a transform from object space to world space.
	D3DXFROMWINEMatrixTranspose(&rotation,&rotation);

	return rotation;
}

D3DXFROMWINEVECTOR3 SpaceShipManager::QuatToVector(D3DXFROMWINEQUATERNION q)
{
	return D3DXFROMWINEVECTOR3(q.x,q.y,q.z);
}

D3DXFROMWINEVECTOR3 *SpaceShipManager::SafeNormalize(D3DXFROMWINEVECTOR3 *dest,D3DXFROMWINEVECTOR3 *src)
{
	if(fabs(src->x) > 0.001f && fabs(src->y) > 0.001f && fabs(src->z) > 0.001f)
	{
		return D3DXFROMWINEVec3Normalize(dest,src);			
	}
	else
	{
		*dest = D3DXFROMWINEVECTOR3(0,1,0);
		return dest;
	}
}
