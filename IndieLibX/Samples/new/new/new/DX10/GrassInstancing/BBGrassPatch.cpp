/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

// Copyright © 2008 Intel Corporation
// All Rights Reserved
// 
// The sample source code contained or described herein and all documents
// related to the sample source code ("Material") are owned by Intel Corporation
// or its suppliers or licensors. Title to the Material remains with Intel Corporation
// or its suppliers and licensors. The Material may contain trade secrets and
// proprietary and confidential information of Intel Corporation and its suppliers
// and licensors, and is protected by worldwide copyright and trade secret laws and
// treaty provisions. The sample source code is provided AS IS, with no warranties
// of any kind, express or implied. Except as expressly permitted by the Software
// license, neither Intel Corporation nor its suppliers assumes any responsibility
// or liability for any errors or inaccuracies that may appear herein.

#include "BBGrassPatch.h"

BOOL SphereConeTest ( D3DXFROMWINEVECTOR3 sphereCenter, float radius, float fov, D3DXFROMWINEVECTOR3 eyePt, D3DXFROMWINEVECTOR3 lookAt);

static  grassLayout[] =
{
	{ "POSITION", 0, FORMAT_RGBA32f, 0, 0,  ,   0 },
	{ "TEXCOORD", 0, FORMAT_RG32f,    0, 12, ,   0 },
	{ "vPPos",    0, FORMAT_RG32f,	  1, 0,	 D3D10_INPUT_PER_INSTANCE_DATA, 1 },

};

static const unsigned int numGrassLayoutElements = sizeof( grassLayout ) / sizeof( grassLayout[0] );


BBGrassPatch::BBGrassPatch( int numBB, int inPatchCount, D3DXFROMWINEVECTOR2* inPatchPos )
{

	m_numGrassBB = numBB;
	patchPos     = inPatchPos;
	patchCount	 = inPatchCount;
}
 
BBGrassPatch::~BBGrassPatch( )
{
	SAFE_DELETE_ARRAY ( patchPos );
	SAFE_DELETE_ARRAY ( visiblePatches );
	SAFE_DELETE_ARRAY ( m_grassVerts );
}

int BBGrassPatch::CreateDevice( stx_Effect*  pEffect )
{
	int hr = 0;
	__super::Init("texture//grassY.dds", patchPos, patchCount, 
					m_grassVerts, 6*m_numGrassBB,(*)grassLayout, numGrassLayoutElements );
	stx_Effect*     pRenderTechnique;
	pRenderTechnique = pEffect->GetTechniqueByName( "RenderGrass" );
	hr = __super::CreateDevice( pRenderTechnique );

//__super::SetInstData( patchPos, patchCount );
	return hr;

}


void BBGrassPatch::DestroyDevice()
{
	InstancedBillboard::DestroyDevice();

}


void BBGrassPatch::GeneratePatch( unsigned int bladeCnt, float scale )
{
	m_numGrassBB = bladeCnt;
	m_grassVerts = new BBGrassVertex[m_numGrassBB*6];

	D3DXFROMWINEMATRIX mat;
	D3DXFROMWINEMATRIX matRandom;
	D3DXFROMWINEMatrixIdentity( &mat);

	for( int j=0; j< m_numGrassBB; j++ )
	{
		// Generate each quad at random position, orientation, height
		D3DXFROMWINEMatrixIdentity( &matRandom);
	//	float scale = 10.0f;
		float angle = ((float)rand()/RAND_MAX - 0.5f)*2 * D3DXFROMWINE_PI; // angle = [-pi,pi]
		float dx = ((float)rand()/RAND_MAX - 0.5f)*2 * scale;
		float dz = ((float)rand()/RAND_MAX - 0.5f)*2 * scale;
		float heightScale = ((float)rand()/RAND_MAX - 0.5f) / 2.0f + 1.0f;

		D3DXFROMWINEMatrixRotationY( &mat, angle);
		D3DXFROMWINEMatrixMultiply( &matRandom, &matRandom, &mat );
		D3DXFROMWINEMatrixTranslation( &mat, dx, 0.0f, dz);			
		D3DXFROMWINEMatrixMultiply( &matRandom, &matRandom, &mat );
		D3DXFROMWINEMatrixScaling( &mat, 1.0f, 1.0f, heightScale);			
		D3DXFROMWINEMatrixMultiply( &matRandom, &matRandom, &mat );

		// Apply the transformation to each vertex of the quad
		for( __DWORD__ i=0; i< 6; i++ )
		{
			D3DXFROMWINEVECTOR4 pos, outPos;
			pos.x = grassBBVerts[i].Pos.x;
			pos.y = grassBBVerts[i].Pos.y;
			pos.z = grassBBVerts[i].Pos.z;
			pos.w = 1.0;
			D3DXFROMWINEVec3Transform(&outPos, (D3DXFROMWINEVECTOR3 *)&pos, (D3DXFROMWINEMATRIX *)&matRandom);
			int index = j * 6 + i;
			m_grassVerts[index].Pos.x = outPos.x;
			m_grassVerts[index].Pos.y = outPos.y;
			m_grassVerts[index].Pos.z = outPos.z;
			m_grassVerts[index].Tex = grassBBVerts[i].Tex;
		}
	}

}

void BBGrassPatch::Cull( float fov, D3DXFROMWINEVECTOR3 eyePt, D3DXFROMWINEVECTOR3 lookAt )
{
	int numVisibleInstances = 0;
	D3DXFROMWINEVECTOR3 patchCenter3D;
	BOOL visible;
	for( unsigned int i = 0 ; i < patchCount; i++)
	{	
		patchCenter3D.x = patchPos[i].x; 	
		patchCenter3D.z = patchPos[i].y; 	
		patchCenter3D.y = 0; // we are only storing x,z positions for the patches
		// we want to cull the patch with a sphere of radius = to diagonal of square patch 
		// hence radiusPatchBoundingSphere = 1.414*rpatch
		visible = SphereConeTest ( patchCenter3D, patchRadius*1.414f, fov, eyePt, lookAt);
		if( visible )
		{
			// visible add to draw list 
			visiblePatches[numVisibleInstances] = patchPos[i];
			numVisibleInstances++;
		}
	}

	SetInstData( visiblePatches, numVisibleInstances );
}


void BBGrassPatch::Generate( D3DXFROMWINEVECTOR3 surfaceCenter, float surfaceR, unsigned int patchBladeCnt, float inPatchRadius )
{
	patchRadius     = inPatchRadius;
	GeneratePatch( patchBladeCnt, patchRadius );

	patchCountX		= int( surfaceR/patchRadius );
	patchCountZ		= int( surfaceR/patchRadius );
	patchCount		= (2*patchCountX-1)*(2*patchCountZ-1);
	patchPos		= new D3DXFROMWINEVECTOR2[patchCount];
	visiblePatches  = new D3DXFROMWINEVECTOR2[patchCount];
	int k = 0;
	for(int i = -(patchCountX-1); i < patchCountX; i++)
	{
		for(int j = -(patchCountZ-1); j < patchCountZ; j++)
		{
			patchPos[k].x = surfaceCenter.x + i*patchRadius;
			patchPos[k].y = surfaceCenter.z + j*patchRadius;
			k++;
		}

	}
}

