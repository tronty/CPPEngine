/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
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

#pragma once
#include "Billboard.h"
#include "InstancedBillboard.h"
struct SimpleVertex;

#define MAX_TREE_TEX 3
#define DEFAULT_NUM_TREES  10

// TODO float HeightField( FLOAT x, FLOAT y );

struct TREE_INSTANCE_DATA
{
	D3DXFROMWINEVECTOR3 Pos;
	D3DXFROMWINEVECTOR2 scale;
};
class BillboardedTrees : public InstancedBillboard <SimpleVertex, TREE_INSTANCE_DATA> 
{

public: 

	int									m_treeCnt;
	SimpleVertex*						m_pTreeVerts;
	TREE_INSTANCE_DATA*					m_pTreeData;

	// tree orientation
	D3DXFROMWINEMATRIX							m_TreeOrientMatrix; 
	D3DXFROMWINEVECTOR3							m_Orientation;
	stx_Effect::stx_Variable 			m_pfxOrientation;

	stx_Effect*               m_pRenderTechnique;


	int		Initialize    ( D3DXFROMWINEVECTOR2 offPos, int numTrees = DEFAULT_NUM_TREES, float ht = 30.0f, 
								float wd = 16.0f, float spread = 30.0f);
	
	int		CreateDevice  ( stx_Effect*  m_pEffect );
	
	void		DestroyDevice ( );
	
	void		Destroy		  ( );
	
	void		Render		  ( ShaderID  m_pDiffuseVariable );

	void		OrientTreesTo ( D3DXFROMWINEVECTOR3);
	

};

