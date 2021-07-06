

//-----------------------------------------------------------------------------
// File: TerrainApp.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _TERRAINAPP_H
#define _TERRAINAPP_H

#include "DXErrors.h"
#include "FlyDemo.h"
#include "SplashScreen.h"
#include <GUI/GUIUtils.h>

class CTerrainApp: public STXGUI {
protected:
    std::string m_strAppName;
    //HWND m_hWnd;

    // D3D interfaces
    //ID3DXFROMWINEContext *m_pD3DXFROMWINE;
    //LPDIRECTDRAW7 m_pDD;
    //LPDIRECT3D9 m_pD3D;
    //LPD3DXFROMWINEFONT   m_pd3dxFont;

    // Window size
    __DWORD__ m_dwFSWidth;
    __DWORD__ m_dwFSHeight;
    bool m_bActive;

    // Demo renderers
    CFlyDemo *m_pFlyDemo;
    CSplashScreen *m_pSplash;

    // Time keeping stuff
    __DWORD__ m_dwStartTime;
    __DWORD__ m_dwTime;

    // Execution mode flags
    bool m_bInitialized;
    bool m_bShowingDemo;
    bool m_bNoTnL;
    bool m_bWindowed;

    bool bTerrain;
    bool bSky;
    bool bWireframe;
    bool mouseLocked;

    // Performance stats
    __DWORD__ m_dwFPS;
    __DWORD__ m_dwPPS;
    __DWORD__ m_dwLoopFrames;
    __DWORD__ m_dwLoopCount;
    __DWORD__ m_dwLastPerfTime;
    char m_strStats[128];
/*
    int InitApplication(int hInstance);
    int InitInstance(int hInstance, INT nCmdShow);
    int ParseCommandLine(std::string lpszCmdLine);
*/
    int InitializeD3DXFROMWINE();
    int UnInitializeD3DXFROMWINE();
    int Startup();
    int InitRenderer();
    int MessageLoop();
    void SetActive(bool bActive);

    int RestoreContext();

    void StartTime();
    void UpdateTime();
    int Draw();

public:
    void SetTerrain(bool a){bTerrain=a;}
    void SetSky(bool a){bSky=a;}
    void SetWireframe(bool a){bWireframe=a;}

    bool RenderTerrain(){return bTerrain;}
    bool RenderSky(){return bSky;}
    bool RenderWireframe(){return bWireframe;}

    int DrawFrame();
    //CTerrainApp();
	void init();
    ~CTerrainApp();

    int Run();
    //LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void actionPerformed(GUIEvent &evt);
};

#endif


