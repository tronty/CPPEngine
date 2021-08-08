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

#pragma once

//#include "GrassUI.h"
//#include "Framework/DX10Base.h"
#include "TerrainPatch.h"
#include "BillboardedTrees.h"
#include "BBGrassPatch.h"
class Grass
{

	BBGrassPatch							bbGrassPatch;
	TerrainPatch							terrain;
	BillboardedTrees						trees;

public:
	void InitApp()
	{
		// call parent class
		__super::InitApp();
	}	
	// FrameUpdate

	int  OnCreateDevice		 ( );
	void     OnDestroyDevice	 ( );
	
	void     OnFrameMove		 ( double fTime, float fElapsedTime );

	void     OnFrameRender       ( double fTime, float fElapsedTime );


};

