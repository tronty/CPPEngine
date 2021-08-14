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


#include "DXUT.h"
#include "BillboardedTrees.h"

static  treeLayout[] =
{
	{ "POSITION", 0, FORMAT_RGBA32f, 0, 0,  ,   0 },
	{ "TEXCOORD", 0, FORMAT_RG32f,    0, 12, ,   0 },
	{ "offPos",   0, FORMAT_RGBA32f, 1, 0,	 D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "scale",    0, FORMAT_RG32f,	  1, 12, D3D10_INPUT_PER_INSTANCE_DATA, 1 },

};

static const unsigned int numTreeLayoutElements = sizeof( treeLayout ) / sizeof( treeLayout[0] );


int BillboardedTrees::Initialize( D3DXFROMWINEVECTOR2 offPos, int numTrees, float height, float width, float spread )
{
	m_treeCnt = numTrees;
	D3DXFROMWINEMatrixIdentity( &m_TreeOrientMatrix);
	float yPos = -10.0; // = pos.y
	m_pTreeVerts			= new SimpleVertex[6];
	m_pTreeData				= new TREE_INSTANCE_DATA[numTrees];

	m_pTreeVerts[0].Pos		= D3DXFROMWINEVECTOR3(-width,  2*height, 0 );
	m_pTreeVerts[1].Pos		= D3DXFROMWINEVECTOR3( width,  2*height, 0 );
	m_pTreeVerts[2].Pos		= D3DXFROMWINEVECTOR3( width,  0,		 0 );

	m_pTreeVerts[3].Pos		= D3DXFROMWINEVECTOR3(-width,  2*height, 0 );
	m_pTreeVerts[4].Pos		= D3DXFROMWINEVECTOR3( width,  0,		 0 );
	m_pTreeVerts[5].Pos		= D3DXFROMWINEVECTOR3(-width,  0,		 0 );

	m_pTreeVerts[0].Tex     =  D3DXFROMWINEVECTOR2( 1.0f,  0.01f );
	m_pTreeVerts[1].Tex		= D3DXFROMWINEVECTOR2( 0.0f,  0.01f );
	m_pTreeVerts[2].Tex		= D3DXFROMWINEVECTOR2( 0.01f, 1.0f );

	m_pTreeVerts[3].Tex		= D3DXFROMWINEVECTOR2( 1.0f,  0.01f );
	m_pTreeVerts[4].Tex		= D3DXFROMWINEVECTOR2( 0.0f,  1.0f );
	m_pTreeVerts[5].Tex		= D3DXFROMWINEVECTOR2( 1.0f,  1.0f );

	// randomly position trees
	
	for( int i=0; i<m_treeCnt; i++ )
	{

		float rot  = 2.0f*D3DXFROMWINE_PI*(float)rand()/RAND_MAX;
		float spreadRadius = spread + 2.0f*spread*(float)rand()/RAND_MAX;

		m_pTreeData[i].Pos.x	= spreadRadius * sinf(rot)+offPos.x;
		m_pTreeData[i].Pos.z    = spreadRadius * cosf(rot)+offPos.y;
		m_pTreeData[i].Pos.y    = yPos; 	//TODO HeightField( xPos, yPos );
		
		// Vary the size of the trees a bit randomly
		 m_pTreeData[i].scale.x  = (width  + 0.3f * (float)(rand()-rand())/RAND_MAX)/width;
		 m_pTreeData[i].scale.y  = (height + 0.5f * (float)(rand()-rand())/RAND_MAX)/height;
		
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Name: TreeSortCB()
// Desc: Callback function for sorting trees in back-to-front order
//-----------------------------------------------------------------------------

int BillboardedTrees::CreateDevice(  stx_Effect*  pEffect )
{

	int hr = 0;

	// Obtain the technique
	__super::Init("texture//tree02s.dds", m_pTreeData, m_treeCnt, 
					m_pTreeVerts, 6,(*)treeLayout, numTreeLayoutElements );

	stx_Effect*     pRenderTechnique;
	pRenderTechnique = pEffect->GetTechniqueByName( "RenderTree" );
	hr = __super::CreateDevice( pRenderTechnique );
	m_pfxOrientation = pEffect->GetVariableByName( "Orientation" );

	return hr;

}
void BillboardedTrees::DestroyDevice()
{

	__super::DestroyDevice();
	SAFE_DELETE_ARRAY( m_pTreeVerts );
	SAFE_DELETE_ARRAY( m_pTreeData  );
}

void BillboardedTrees::OrientTreesTo(D3DXFROMWINEVECTOR3 vDir)
{

	m_Orientation = vDir;

	if( vDir.x < 0.0f )
		D3DXFROMWINEMatrixRotationY( &m_TreeOrientMatrix, -atanf(vDir.z/vDir.x)-D3DXFROMWINE_PI/2 );
	else
		D3DXFROMWINEMatrixRotationY( &m_TreeOrientMatrix, -atanf(vDir.z/vDir.x)+D3DXFROMWINE_PI/2 );

	// Sort trees in back-to-front order
//	qsort( m_Trees, m_treeCnt, sizeof(Tree), TreeSortCB );

}
//-----------------------------------------------------------------------------
// Name: DrawTrees()
// Desc:
//-----------------------------------------------------------------------------
void BillboardedTrees::Render(ShaderID  pDiffuseVariable )
{
	m_pfxOrientation->SetMatrix((float*) &m_TreeOrientMatrix );
	__super::Render( pDiffuseVariable );

}

void BillboardedTrees::Destroy()
{
	SAFE_DELETE_ARRAY (m_pTreeVerts);
}

