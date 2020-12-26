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
#include "AsteroidManager.h"

AsteroidManager::AsteroidManager()
{
	rocks = 0;
	numRocks = 0;
}

void AsteroidManager::SetWorldExtents(D3DXFROMWINEVECTOR3 &wmin,D3DXFROMWINEVECTOR3 &wmax)
{
	worldMin = wmin;
	worldMax = wmax;
}

void AsteroidManager::Init(int num)
{
	Asteroid *newrocks;
	if(num > 0)
	{
		newrocks = new Asteroid[num];
	}

	if(rocks != 0)
	{
		// preserve existing rocks
		if(num > 0 && numRocks > 0)
		{
			if(num > numRocks)
			{
				int numToCopy = numRocks;
				stx_memcpy(newrocks,rocks,numToCopy*sizeof(Asteroid));
				for(int iRock=numRocks;iRock<num;iRock++)
				{
					newrocks[iRock].MakeRandomPosition(worldMin,worldMax);
				}
			}
			else
			{
				int numToCopy = num;
				stx_memcpy(newrocks,rocks,numToCopy*sizeof(Asteroid));
			}
		}

		delete[]rocks;
	}
	else
	{
		for(int iRock=0;iRock<num;iRock++)
		{
			newrocks[iRock].MakeRandomPosition(worldMin,worldMax);
		}
	}
	rocks = newrocks;
	numRocks = num;
}

void AsteroidManager::Update(float deltaTime)
{
	for(int iRock=0;iRock<numRocks;iRock++)
	{
		D3DXFROMWINEQUATERNION rotAdjust;
		D3DXFROMWINEQuaternionRotationAxis(&rotAdjust,&rocks[iRock].rotationAxis,deltaTime*rocks[iRock].speed);
		D3DXFROMWINEQuaternionMultiply(&rocks[iRock].rotation,&rocks[iRock].rotation,&rotAdjust);
	}
}

D3DXFROMWINEMATRIX AsteroidManager::MakeMatrixFor(int iRock)
{
	D3DXFROMWINEMATRIX scaling; D3DXFROMWINEMatrixScaling(&scaling,rocks[iRock].scale.x,rocks[iRock].scale.y,rocks[iRock].scale.z);
	D3DXFROMWINEMATRIX rotation; D3DXFROMWINEMatrixRotationQuaternion(&rotation,&rocks[iRock].rotation);
	D3DXFROMWINEMATRIX translation; D3DXFROMWINEMatrixTranslation(&translation,rocks[iRock].position.x,rocks[iRock].position.y,rocks[iRock].position.z);
	D3DXFROMWINEMATRIX world = scaling*rotation*translation;
	return world;
}

D3DXFROMWINECOLOR AsteroidManager::GetColorFor(int iRock)
{
	return rocks[iRock].color;
}
