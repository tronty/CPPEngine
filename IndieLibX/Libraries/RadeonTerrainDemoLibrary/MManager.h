//-----------------------------------------------------------------------------
// File: MManager.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _MMANAGER_H
#define _MMANAGER_H

//include <d3dx.h>
#include "Material.h"

#define MAX_MATERIALS 50

class CMaterialManager
{
	// List of all materials
	CMaterial *m_Materials[MAX_MATERIALS];

public:
	CMaterialManager();
	~CMaterialManager();
	int Load(char *fname);
	CMaterial *GetMaterial(__DWORD__ n);
	__DWORD__ AddMaterial(CMaterial *material);
	int DeleteMaterial(__DWORD__ n);
};

extern CMaterialManager g_MMan;

#endif
