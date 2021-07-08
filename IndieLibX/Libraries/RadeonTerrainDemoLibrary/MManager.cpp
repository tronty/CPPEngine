//-----------------------------------------------------------------------------
// File: MManager.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#define D3D_OVERLOADS

//include <tchar.h>
#include <stdio.h>
#include "Helper.h"
#include "MManager.h"
#include "DXErrors.h"

#define MAT_SIGN "MAT_FILE"

CMaterialManager g_MMan;

CMaterialManager::CMaterialManager()
{
	ZeroMemory(m_Materials, sizeof(m_Materials));
}

CMaterialManager::~CMaterialManager()
{
	for(__DWORD__ i = 0; i < MAX_MATERIALS; i++)
	{
		DeleteMaterial(i);
	}
}

int CMaterialManager::Load(char *fname)
{
	FILE *fp;
	CHAR signature[9];
	__DWORD__ version;
	
	std:string fn=stx_convertpath(fname);
	fp = fopen(fn.c_str(), "rb");

	fread(signature, sizeof(CHAR), 8, fp);

	signature[8] = '\0';

	fread(&version, sizeof(__DWORD__), 1, fp);
	
	if (strcmp(signature, MAT_SIGN))
	{
		fclose(fp);
		return 0;
	}
	
	
	__DWORD__ n;
	fread(&n, sizeof(__DWORD__), 1, fp);

	
	
	//LOG_PRINT("n = %d\n", n);
	if (n > MAX_MATERIALS)
	{
		fclose(fp);
		return 0;
	}

	
	__DWORD__ i;
	/*
	for(i = 0; i < MAX_MATERIALS; i++)
	{
		DeleteMaterial(i);
	}*/

	
	CMaterial *material;
	for(i = 0; i < n; i++)
	{
		material = new CMaterial();
		if (FAILED(material->LoadMaterial(fp)))
		{
			delete material;
			fclose(fp);
			return 0;
		}
		material->m_pTextureID = g_MMan.AddMaterial(material);
		//LOG_PRINT("material->m_pTextureID=%d\n", material->m_pTextureID);
		//m_Materials[i] = material;
		
	}
	fclose(fp);

	return 1;
}

CMaterial *CMaterialManager::GetMaterial(__DWORD__ n)
{
	if (n >= MAX_MATERIALS)
		return 0;
	return m_Materials[n];
}

__DWORD__ CMaterialManager::AddMaterial(CMaterial *material)
{
	for(__DWORD__ i = 0; i < MAX_MATERIALS; i++)
	{
		if (m_Materials[i] == 0)
		{
			m_Materials[i] = material;
			return i;
		}
	}
	return -1;
}

int CMaterialManager::DeleteMaterial(__DWORD__ n)
{
	if (n >= MAX_MATERIALS)
		return 0;
	if (m_Materials[n] != 0)
	{
		delete m_Materials[n];
		m_Materials[n] = 0;
	}
	return 1;
}

