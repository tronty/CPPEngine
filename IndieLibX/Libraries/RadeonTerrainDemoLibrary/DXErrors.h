

//-----------------------------------------------------------------------------
// File: DXErrors.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _DDERRORS_H
#define _DDERRORS_H

#define STRICT
#define D3D_OVERLOADS

#include <stdio.h>
#if 0
//include <tchar.h>
#include <windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#else
#include <Framework3/IRenderer.h>
#endif

inline D3DXFROMWINEMATRIX mmm()
{
	D3DXFROMWINEMATRIX matWorld=IRenderer::GetRendererInstance()->Getww();
	D3DXFROMWINEMATRIX matView=IRenderer::GetRendererInstance()->Getvv();
	D3DXFROMWINEMATRIX matProj=IRenderer::GetRendererInstance()->Getpp();
  	D3DXFROMWINEMATRIX matVP = matWorld*(matView) * (matProj);
  	//D3DXFROMWINEMatrixTranspose(&matVP, &matVP);
//D3DXFROMWINEMatrixIdentity(&matVP);
	return matVP;
}

#include "LitVertex.h"

class CMaterialManager;
class CTextureManager;
class CViewer;
class CTerrainApp;
struct GameState
{
    static CMaterialManager* m_MMan;
    static CTextureManager* m_TMan;
    static CViewer* m_Viewer;
    static CTerrainApp* m_App;
};

void _DbgOut(char* strFile, __DWORD__ dwLine, int hr, char *strMsg);
void _ErrorOut(char *strFile, __DWORD__ dwLine, int hr);

#if defined(DEBUG) | defined(_DEBUG)
    #define DD_ERR(hr) _ErrorOut(__FILE__, (__DWORD__)__LINE__, hr)
#else
    #define DD_ERR(hr)
#endif

#endif


