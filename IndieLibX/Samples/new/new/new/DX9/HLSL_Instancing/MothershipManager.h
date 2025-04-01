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
// File: MotherShipManager.h
// Desc: Wraps up management of little rotating D3DXFROMWINEMesh objects.  Used by the spaceship
//			manager to collide with.
//-----------------------------------------------------------------------------

#pragma once
#include "Asteroid.h"

/*
This is a silly little class that wraps up the rendering of a D3DXFROMWINEMesh.
*/
class MotherShip : public Asteroid
{
public:
	MotherShip() 
	{
		position = D3DXFROMWINEVECTOR3(0,0,0);

		float fscale = 1.f;
		scale = D3DXFROMWINEVECTOR3(fscale,fscale,fscale);

		float cg = 0.25f + 0.25f*(float)stx_rand()/32768.0f;
		color = D3DXFROMWINECOLOR(cg,cg,cg,1);	

		float x = 2.f*((float)stx_rand()/32768.0f-0.5f);
		float y = 2.f*((float)stx_rand()/32768.0f-0.5f);
		float z = 2.f*((float)stx_rand()/32768.0f-0.5f);
		rotationAxis = D3DXFROMWINEVECTOR3(0,1+y*0.1f,0);
		D3DXFROMWINEVec3Normalize(&rotationAxis,&rotationAxis);

		radius = 70.f*fscale;
		radiusSq = radius*radius;

		D3DXFROMWINEQuaternionRotationYawPitchRoll(&rotation,0,0,0);

		speed = 0.5f + (float)stx_rand()/32768.0f;;
	}
};

const D3DVERTEXELEMENT9 g_aMSVertDecl[] =
{
	{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
	{0, 24, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
	{0, 36, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
	{0, 48, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

class MothershipManager
{
public:

	MothershipManager()
	{
		ships = 0;
		numShips = 0;
	}

	int GetNumPolysRendered()
	{
		if(shipMesh)
			return numShips*shipMesh->GetNumFaces();
		return 0;
	}

	void SetWorldExtents(D3DXFROMWINEVECTOR3 &wmin,D3DXFROMWINEVECTOR3 &wmax)
	{
		worldMin = wmin;
		worldMax = wmax;
	}

	void Init(int num)
	{
		MotherShip *newships;
		if(num > 0)
		{
			newships = new MotherShip[num];
		}

		if(ships != 0)
		{
			// preserve existing ships
			if(num > 0 && numShips > 0)
			{
				if(num > numShips)
				{
					int numToCopy = numShips;
					stx_memcpy(newships,ships,numToCopy*sizeof(MotherShip));
					for(int iShip=numShips;iShip<num;iShip++)
					{
						newships[iShip].MakeRandomPosition(worldMin,worldMax);
					}
				}
				else
				{
					int numToCopy = num;
					stx_memcpy(newships,ships,numToCopy*sizeof(MotherShip));
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

	void Update(float deltaTime)
	{
		for(int iShip=0;iShip<numShips;iShip++)
		{
			D3DXFROMWINEQUATERNION rotAdjust;
			D3DXFROMWINEQuaternionRotationAxis(&rotAdjust,&ships[iShip].rotationAxis,deltaTime*ships[iShip].speed);
			D3DXFROMWINEQuaternionMultiply(&ships[iShip].rotation,&ships[iShip].rotation,&rotAdjust);
		}
	}

	D3DXFROMWINEMATRIX MakeMatrixFor(int iShip)
	{
		D3DXFROMWINEMATRIX scaling; D3DXFROMWINEMatrixScaling(&scaling,ships[iShip].scale.x,ships[iShip].scale.y,ships[iShip].scale.z);
		D3DXFROMWINEMATRIX rotation; D3DXFROMWINEMatrixRotationQuaternion(&rotation,&ships[iShip].rotation);
		D3DXFROMWINEMATRIX translation; D3DXFROMWINEMatrixTranslation(&translation,ships[iShip].position.x,ships[iShip].position.y,ships[iShip].position.z);
		D3DXFROMWINEMATRIX world = scaling*rotation*translation;
		return world;
	}

	D3DXFROMWINECOLOR GetColorFor(int iShip)
	{
		return ships[iShip].color;
	}

	int Create()
	{
		shipMesh->Load("models\\UFO-03\\ufo3.x", eSimpleShader, true, true);

		shipTex1=IRenderer::GetRendererInstance()->addImageLibTexture("models\\UFO-03\\UFO-03_Metal-01_df.dds", false, IRenderer::GetRendererInstance()->Getlinear());

		shipTex2=IRenderer::GetRendererInstance()->addImageLibTexture("models\\UFO-03\\UFO-03_Metal-02_df.dds", false, IRenderer::GetRendererInstance()->Getlinear());

		shipGlow=IRenderer::GetRendererInstance()->addImageLibTexture("models\\UFO-03\\UFO-03-glow.dds", false, IRenderer::GetRendererInstance()->Getlinear());

		return 0;
	}

	int Destroy()
	{
		////////////////SAFE_RELEASE(shipMesh);
		////////////////SAFE_RELEASE(shipTex1);  // Mesh texture
		////////////////SAFE_RELEASE(shipTex2);  // Mesh texture
		////////////////SAFE_RELEASE(shipGlow);  // Mesh texture

		return 0;
	}

	// returns draw calls
	int Render( stx_Effect* g_pEffect)
	{
		int hr;
		unsigned int iPass, cPasses;
		int drawcalls = 0;
		IRenderer::GetRendererInstance()->BeginScene();
		{
			( g_pEffect->SetTechnique( "RenderMSNormal" ) );
			(g_pEffect->SetVector("g_MaterialDiffuseColor",&D3DXFROMWINEVECTOR4(1,1,1,1)));
			( g_pEffect->SetFloat("SpecExpon",120.0f));
			( g_pEffect->SetFloat("Ks",0.6f));
			( g_pEffect->SetFloat("Bumpy",1.5f));
			( g_pEffect->SetFloat("Kd",1.0f));

			for(int iShip=0;iShip<numShips;iShip++)
			{
				D3DXFROMWINEMATRIXA16 matrix = MakeMatrixFor(iShip);
				( g_pEffect->SetMatrix( "g_mWorld", &(matrix) ));
				( g_pEffect->SetTexture( "g_MeshBumpTexture", shipGlow) );

				( g_pEffect->Begin(&cPasses, 0) );
				for (iPass = 0; iPass < cPasses; iPass++)
				{
					( g_pEffect->BeginPass(iPass) );
					( g_pEffect->SetTexture( "g_MeshTexture", shipTex1) );
					//( g_pEffect->CommitChanges() );
					shipMesh->DrawSubset(0);
					( g_pEffect->SetTexture( "g_MeshTexture", shipTex2) );
					//( g_pEffect->CommitChanges() );

					shipMesh->DrawSubset(1);
					drawcalls+=2;
					( g_pEffect->EndPass() );
				}
				( g_pEffect->End() );
			}

			IRenderer::GetRendererInstance()->EndScene();
		}	
		return drawcalls;
	}

public:
	MeshRenderer2* shipMesh;
	TextureID				shipTex1;  // Mesh texture
	TextureID				shipTex2;  // Mesh texture
	TextureID				shipGlow;  // Mesh texture

	D3DXFROMWINEVECTOR3 worldMin;
	D3DXFROMWINEVECTOR3 worldMax;
	MotherShip *ships;
	int numShips;
	int polysPerShip;
};
