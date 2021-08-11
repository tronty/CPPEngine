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
#include "TerrainPatch.h"


TerrainPatch::TerrainPatch()
{
	g_pVertexBuffer  = 0;
	pRenderTechnique = 0;
	m_pGroundVerts   = 0;

}

int TerrainPatch::CreateDevice( stx_Effect*  pEffect)
{
	int hr = 0;
	
	pRenderTechnique = pEffect->GetTechniqueByName( "Render" );
	if( m_pGroundVerts )
		__super::Init("texture//seafloor.dds", m_pGroundVerts, 6);
	else 
		__super::Init("texture//seafloor.dds", g_DefaultGroundVerts, 6);

	__super::CreateDevice(pRenderTechnique );
	
	return hr;

}

void TerrainPatch::DestroyDevice()
{
	__super::DestroyDevice();
	//////////////SAFE_RELEASE( g_pVertexBuffer );

}

void TerrainPatch::Generate( D3DXFROMWINEVECTOR3 center, float r, float texRepeat )
{
	m_pGroundVerts = new TerrainVertex[6];
	//D3DXFROMWINEVECTOR3(-60.0f,  -10.0f, 60.0f)
	m_pGroundVerts[0].Pos = D3DXFROMWINEVECTOR3( center.x - r, center.y, center.z + r );  
	m_pGroundVerts[0].Tex = D3DXFROMWINEVECTOR2( texRepeat, texRepeat );
//	{D3DXFROMWINEVECTOR3( 60.0f,  -10.0f, 60.0f), D3DXFROMWINEVECTOR2(0.0f, 10.0f )},
	m_pGroundVerts[1].Pos = D3DXFROMWINEVECTOR3( center.x + r, center.y, center.z + r );  
	m_pGroundVerts[1].Tex = D3DXFROMWINEVECTOR2(0, texRepeat );
//	{D3DXFROMWINEVECTOR3( 60.0f,   -10.0f, -60.0f), D3DXFROMWINEVECTOR2(0.0f, 0.0f )},
	m_pGroundVerts[2].Pos = D3DXFROMWINEVECTOR3( center.x + r, center.y, center.z - r );  
	m_pGroundVerts[2].Tex = D3DXFROMWINEVECTOR2(0, 0 );

//	{D3DXFROMWINEVECTOR3(-60.0f,  -10.0f, 60.0f), D3DXFROMWINEVECTOR2(10.0f, 10.0f )},
	m_pGroundVerts[3].Pos = D3DXFROMWINEVECTOR3( center.x - r, center.y, center.z + r );  
	m_pGroundVerts[3].Tex = D3DXFROMWINEVECTOR2(texRepeat, texRepeat );
//	{D3DXFROMWINEVECTOR3( 60.0f,  -10.0f, -60.0f), D3DXFROMWINEVECTOR2(0.0f, 0.0f )},
	m_pGroundVerts[4].Pos = D3DXFROMWINEVECTOR3( center.x + r, center.y, center.z - r );  
	m_pGroundVerts[4].Tex = D3DXFROMWINEVECTOR2(0, 0 );
//	{D3DXFROMWINEVECTOR3(-60.0f, -10.0f, -60.0f), D3DXFROMWINEVECTOR2(10.0f, 0.0f )},
	m_pGroundVerts[5].Pos = D3DXFROMWINEVECTOR3( center.x - r, center.y, center.z - r );  
	m_pGroundVerts[5].Tex = D3DXFROMWINEVECTOR2(texRepeat, 0.0 );

}

TerrainPatch::~TerrainPatch()
{
	SAFE_DELETE_ARRAY( m_pGroundVerts );
}

