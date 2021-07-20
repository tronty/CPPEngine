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

#include "Grass.h"

int  Grass::OnCreateDevice()
{
	
	int hr = __super::OnCreateDevice( pBackBufferSurfaceDesc );
	D3DXFROMWINEVECTOR3 terrainCenter = D3DXFROMWINEVECTOR3(0,-10,0);
	float terrainRadius = 45.0;
	float terrainTexRepeats = 10.0;
	terrain.Generate( terrainCenter,terrainRadius,terrainTexRepeats ); // Center, radius, texRepeats
	terrain.CreateDevice ( m_pEffect );
	float patchRadius = 15.0;
	unsigned int  grassBladesPerPatch = 70;
	bbGrassPatch.Generate( terrainCenter, terrainRadius, grassBladesPerPatch, patchRadius );
	bbGrassPatch.CreateDevice( m_pEffect );
	D3DXFROMWINEVECTOR2 treeCenter = D3DXFROMWINEVECTOR2(0, -25);
	trees.Initialize( treeCenter );
	trees.CreateDevice( m_pEffect );

	return hr;
}

void     Grass::OnFrameMove( double fTime, float fElapsedTime )
{
	__super::OnFrameMove(fTime, fElapsedTime);
	
	D3DXFROMWINEVECTOR3 eyePt    = *m_pCamera->GetEyePt();
	D3DXFROMWINEVECTOR3 lookAtPt = *m_pCamera->GetLookAtPt();
	trees.OrientTreesTo(lookAtPt - eyePt);
	// Cull geometry not visible
	bbGrassPatch.Cull( D3DXFROMWINE_PI/8 /* frustum cone angle = camera FOV/2*/, eyePt, lookAtPt);


}

void     Grass::OnFrameRender( double fTime, float fElapsedTime )
{
	__super::OnFrameRender( fTime, fElapsedTime );
	terrain.Render( m_ptxDiffuse );
	trees.Render(  m_ptxDiffuse );
	bbGrassPatch.Render( m_ptxDiffuse );
	m_UI.Render(fElapsedTime); // No UI by default

}

void     Grass::OnDestroyDevice()
{
	terrain.DestroyDevice();
	bbGrassPatch.DestroyDevice();
	trees.DestroyDevice();
	trees.Destroy();
	__super::OnDestroyDevice( );

}

