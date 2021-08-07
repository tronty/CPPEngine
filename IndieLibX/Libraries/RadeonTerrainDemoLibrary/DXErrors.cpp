

//-----------------------------------------------------------------------------
// File: DXErrors.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "DXErrors.h"

CMaterialManager* GameState::m_MMan=0;
CTextureManager* GameState::m_TMan=0;
CViewer* GameState::m_Viewer=0;
CTerrainApp* GameState::m_App=0;

void _DbgOut(char* strFile, __DWORD__ dwLine, int hr, char *strMsg)
{
    char buffer[256];

    sprintf(buffer, ("%s(%ld): \n"), strFile, dwLine);
    //LOG_PRINT(buffer);
    //LOG_PRINT(strMsg);
    if (hr)
    {
        sprintf(buffer, ("(hr=%08lx)\n"), hr);
        //LOG_PRINT(buffer);
    }
    //LOG_PRINT(("\n"));
}

void _ErrorOut(char *strFile, __DWORD__ dwLine, int hr)
{

//    char strErr[100];

    //D3DXFROMWINEGetErrorString(hr, 100, strErr);
    //sprintf(strErr,"%s\n%s\n",DXGetErrorString9(hr),DXGetErrorDescription9(hr));
    _DbgOut(strFile, dwLine, hr, "");//strErr);

}


