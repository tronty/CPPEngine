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

#pragma once´
#include "Billboard.h"

struct TerrainVertex
{
	D3DXFROMWINEVECTOR3 Pos;
	D3DXFROMWINEVECTOR2 Tex;

};

static TerrainVertex g_DefaultGroundVerts[]=
{
	//  x       y      z     tu1   tv1    
	{D3DXFROMWINEVECTOR3(-60.0f,  -10.0f, 60.0f), D3DXFROMWINEVECTOR2(10.0f, 10.0f )},
	{D3DXFROMWINEVECTOR3( 60.0f,  -10.0f, 60.0f), D3DXFROMWINEVECTOR2(0.0f, 10.0f )},
	{D3DXFROMWINEVECTOR3( 60.0f,   -10.0f, -60.0f), D3DXFROMWINEVECTOR2(0.0f, 0.0f )},

	{D3DXFROMWINEVECTOR3(-60.0f,  -10.0f, 60.0f), D3DXFROMWINEVECTOR2(10.0f, 10.0f )},
	{D3DXFROMWINEVECTOR3( 60.0f,  -10.0f, -60.0f), D3DXFROMWINEVECTOR2(0.0f, 0.0f )},
	{D3DXFROMWINEVECTOR3(-60.0f, -10.0f, -60.0f), D3DXFROMWINEVECTOR2(10.0f, 0.0f )},
};



class TerrainPatch : public Billboard <TerrainVertex>
{
	exBufferID                       g_pVertexBuffer;
	stx_Effect*               pRenderTechnique;
	TerrainVertex*						m_pGroundVerts;
public:
	TerrainPatch  ( );
	~TerrainPatch ( );
	void	Generate	( D3DXFROMWINEVECTOR3 center, float radius, float texRepeat ); /* TODO - Extend to add height field*/
	int CreateDevice	( stx_Effect*  m_pEffect );
	void	DestroyDevice	( );

};

