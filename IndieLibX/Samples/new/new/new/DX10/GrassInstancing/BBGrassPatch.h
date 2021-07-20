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

#define DEFAULT_NUM_GRASS_BB 70
#include "InstancedBillboard.h"

struct BBGrassVertex
{
	D3DXFROMWINEVECTOR3 Pos;
	D3DXFROMWINEVECTOR2 Tex;
};

//  geometry for single grass blade 
static BBGrassVertex grassBBVerts[] =
{
	//					x     y      z					 tu1   tv1    
	{  D3DXFROMWINEVECTOR3( -10.0f,   0.0f,   0.0f),  D3DXFROMWINEVECTOR2( 1.0f,  0.01f )},
	{  D3DXFROMWINEVECTOR3(  10.0f,   0.0f,   0.0f),  D3DXFROMWINEVECTOR2( 0.01f, 0.01f )},
	{  D3DXFROMWINEVECTOR3(  10.0f,  -10.0f,  0.0f),  D3DXFROMWINEVECTOR2( 0.01f, 1.0f ) },

	{  D3DXFROMWINEVECTOR3(  -10.0f,   0.0f,  0.0f),  D3DXFROMWINEVECTOR2( 1.0f,  0.01f )},
	{  D3DXFROMWINEVECTOR3(   10.0f,  -10.0f, 0.0f),  D3DXFROMWINEVECTOR2( 0.01f, 1.0f ) },
	{  D3DXFROMWINEVECTOR3(  -10.0f,  -10.0f, 0.0f),  D3DXFROMWINEVECTOR2( 1.0f,  1.0f ) }, 
};

//  x,z instance positions ...
static D3DXFROMWINEVECTOR2 defaultPatchPos[] =
{
	//			   x      z					
	D3DXFROMWINEVECTOR2(  20.0f, 0.0f ) ,
	D3DXFROMWINEVECTOR2(  0.0f,  0.0f ) ,
	D3DXFROMWINEVECTOR2( -20.0f, 0.0f ) ,

	D3DXFROMWINEVECTOR2(  20.0f, -20.0f ) ,
	D3DXFROMWINEVECTOR2(  0.0f,  -20.0f  ) ,
	D3DXFROMWINEVECTOR2( -20.0f, -20.0f ) ,

	D3DXFROMWINEVECTOR2(  20.0f,  40.0f ) ,
	D3DXFROMWINEVECTOR2(  20.0f,  40.0f ) ,
	D3DXFROMWINEVECTOR2(  0.0f,   40.0f ) ,
	D3DXFROMWINEVECTOR2( -20.0f,  40.0f ),
	D3DXFROMWINEVECTOR2( -40.0,	  40.0f ),

	D3DXFROMWINEVECTOR2(  40.0f,  20.0f ) ,
	D3DXFROMWINEVECTOR2(  20.0f,  20.0f ) ,
	D3DXFROMWINEVECTOR2(  0.0f,   20.0f ) ,
	D3DXFROMWINEVECTOR2( -20.0f,  20.0f ) ,
	D3DXFROMWINEVECTOR2( -40.0f,  20.0f ) ,
};

const int defaultPatchCount = 16;

class BBGrassPatch : public InstancedBillboard <BBGrassVertex, D3DXFROMWINEVECTOR2> 
{
	int									m_numGrassBB;
	BBGrassVertex*						m_grassVerts;
	float								patchRadius;
	int									patchCountX;
	int									patchCountZ;
	unsigned int						patchCount;
	D3DXFROMWINEVECTOR2*						patchPos;
	D3DXFROMWINEVECTOR2*						visiblePatches;
	// Generates & Randomly Orients Blades in a patch
	void	GeneratePatch( unsigned int bladeCnt, float scale);

public:
	
	BBGrassPatch( int numBB = DEFAULT_NUM_GRASS_BB, int inPatchCount = defaultPatchCount, D3DXFROMWINEVECTOR2* inPatchPos = defaultPatchPos );
	~BBGrassPatch( );
	int CreateDevice ( stx_Effect*  m_pEffect );
	void    Cull		 ( float fov, D3DXFROMWINEVECTOR3 eyePt, D3DXFROMWINEVECTOR3 lookAt );
	void	DestroyDevice( );
	void	Generate	 ( D3DXFROMWINEVECTOR3 surfaceCenter, float surfaceR, unsigned int patchBladeCnt, float inPatchRadius );
	
};

