

//-----------------------------------------------------------------------------
// File: TerrainApp.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "DXErrors.h"
//include "resource.h"
#include "Helper.h"
#include "TerrainApp.h"
#include "TManager.h"
#include "MManager.h"
#include "Viewer.h"
#include "LitVertexVS2PS.h"
/*
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return GameState::m_App->WndProc(hwnd, uMsg, wParam, lParam);
}
*/
CTerrainApp app;
int init(unsigned int width, unsigned int height)
{
	app.init(width, height);
	return 0;
}
int render()
{			
	app.DrawFrame();
	return 0;
}

int ApplicationLogic()
{
    	int hr=1;

	IRenderer* r=IRenderer::GetRendererInstance(640, 512, "RadeonTerrainDemo");		
	IInput*    i=STX_Service::GetInputInstance();

	GameState::m_MMan=new CMaterialManager();
    	GameState::m_TMan=new CTextureManager();
    	GameState::m_Viewer=new CViewer();
    	//GameState::m_App=new CTerrainApp();

	GameState::m_App=&app;
	init(640, 512);
	while (!i->OnKeyPress (KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}

    delete GameState::m_App;
    delete GameState::m_Viewer;
    delete GameState::m_MMan;
    delete GameState::m_TMan;

    return 0;
}

void CTerrainApp::init(unsigned int width, unsigned int height)
{
	bTerrain = true;
	bSky = true;


if(1){
	std::string fn=stx_convertpath("/Framework3/Framework3.xml");
    TiXmlDocument doc(fn.c_str());
    if (!doc.LoadFile())
	{//LOG_PRINT("!doc.LoadFile(%s)\n", fn.c_str());
      stx_exit(0);}
    TiXmlHandle docHandle(&doc);
    std::string Str = docHandle.FirstChild("Body").FirstChild("Debug").Element()->Attribute("Value");
	if(Str!="False")
	{
  bTerrain           =   false;
	}}


	bWireframe = false;

    m_bInitialized = false;
    m_bActive = false;
    m_strAppName = ("RadeonTerrainDemo");
    m_dwFSWidth = IRenderer::GetRendererInstance()->GetViewportWidth();
    m_dwFSHeight = IRenderer::GetRendererInstance()->GetViewportHeight();
    m_bShowingDemo = false;
	
	////LOG_PRINT("m_bShowingDemo = false\n");
    m_bNoTnL = false;
    m_bWindowed = true;//false;


	LitVertexVS2PS::inited=false;

	LitVertexVS2PS::init();

	LitVertexVS2PS::inited=true;

    m_pFlyDemo = new CFlyDemo();
    m_pSplash = new CSplashScreen();


    
	int hr;
/*
    hr = ParseCommandLine(lpszCmdLine);
    if (FAILED(hr))
        return 0;

    if (!hPrevInstance)
    {
        hr = InitApplication(hInstance);
        if (FAILED(hr))
            return 0;
    }

    hr = InitInstance(hInstance, nCmdShow);
    if (FAILED(hr))
        return 0;
*/
    
	hr = InitializeD3DXFROMWINE();
    //if (0)//FAILED(hr)) return;

    
	hr = Startup();

	STXGUI::init("/RadeonTerrainDemo/GUILayout.xml");

    if (m_bInitialized)
    	StartTime();
}

CTerrainApp::~CTerrainApp()
{
    guiFrame->clear();
    SAFE_DELETE(m_pFlyDemo);
    SAFE_DELETE(m_pSplash);
}
#if 0
int CTerrainApp::InitApplication(int hInstance)
{
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC)::WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON));
    wc.hCursor = (HCURSOR)0;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName =  0;
    wc.lpszClassName = m_strAppName;

    if (RegisterClass(&wc))
        return 1;

    return 0;
}

int CTerrainApp::InitInstance(int hInstance, INT nCmdShow)
{
    // Create window
    if (m_bWindowed)
    {
        m_hWnd = CreateWindow(m_strAppName,
            m_strAppName,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            m_dwFSWidth,
            m_dwFSHeight,
            (HWND)0,
            (HMENU)0,
            hInstance,
            (LPVOID)0);
    }
    else
    {
        m_hWnd = CreateWindow(m_strAppName,
            m_strAppName,
            WS_POPUP,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            m_dwFSWidth,
            m_dwFSHeight,
            (HWND)0,
            (HMENU)0,
            hInstance,
            (LPVOID)0);
    }

    // If the main window cannot be created, terminate
    // the application.
    if (m_hWnd == 0)
        return 0;

    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);

    return 1;
}

int CTerrainApp::ParseCommandLine(std::string lpszCmdLine)
{
    // Parse command line
    char *psz, *pszLim;

    psz = lpszCmdLine;
    pszLim = psz + strlen(psz);

    while (psz < pszLim)
    {
        char *pszWord;
        pszWord = psz;

        while ((psz[0] != '\0') && (psz[0] != ' '))
            psz++;

        psz[0] = '\0';
        psz++;

        if ((strcmp(pszWord, "640x480") == 0) || (strcmp(pszWord, "640X480") == 0))
        {
            m_dwFSWidth = 640;
            m_dwFSHeight = 480;
            break;
        }
        if ((strcmp(pszWord, "800x600") == 0) || (strcmp(pszWord, "800X600") == 0))
        {
            m_dwFSWidth = 800;
            m_dwFSHeight = 600;
            break;
        }
        if ((strcmp(pszWord, "1024x768") == 0) || (strcmp(pszWord, "1024X768") == 0))
        {
            m_dwFSWidth = 1024;
            m_dwFSHeight = 768;
            break;
        }
        if ((strcmp(pszWord, "1280x1024") == 0) || (strcmp(pszWord, "1280X1024") == 0))
        {
            m_dwFSWidth = 1280;
            m_dwFSHeight = 1024;
            break;
        }
        if ((strcmp(pszWord, "1600x1200") == 0) || (strcmp(pszWord, "1600X1200") == 0))
        {
            m_dwFSWidth = 1600;
            m_dwFSHeight = 1200;
            break;
        }

        if ((strcmp(pszWord, "-notnl") == 0) || (strcmp(pszWord, "-NOTNL") == 0))
        {
            m_bNoTnL = true;
            break;
        }
        if ((strcmp(pszWord, "-win") == 0) || (strcmp(pszWord, "-WIN") == 0))
        {
            m_bWindowed = true;
            break;
        }

        while ((psz < pszLim) && (psz[0] == ' '))
            psz++;
    }

    return 1;
}
#endif
int CTerrainApp::InitializeD3DXFROMWINE()
{
    int hr;

    hr = InitRenderer();

    if (FAILED(hr))
        return 0;

    GameState::m_TMan->RestoreAll();

/*
////LOG_PRINT("m_bShowingDemo %d\n",m_bShowingDemo?1:0);
////LOG_PRINT("m_pFlyDemo->m_bLoaded %d\n",m_pFlyDemo->m_bLoaded?1:0);
////LOG_PRINT("m_pSplash->m_bLoaded %d\n",m_pSplash->m_bLoaded?1:0);
//stx_exit(0);*/
    if (m_bShowingDemo)
    {

        if ((m_pFlyDemo != 0) && (m_pFlyDemo->m_bLoaded))
            m_pFlyDemo->Init();

    }
    else
    {

        if ((m_pSplash != 0) && (m_pSplash->m_bLoaded))
            m_pSplash->Init();

    }

    m_bInitialized = true;
    return 1;
}

int CTerrainApp::UnInitializeD3DXFROMWINE()
{
    //int hr;

    if (m_bShowingDemo)
    {
        if ((m_pFlyDemo != 0) && (m_pFlyDemo->m_bLoaded))
            m_pFlyDemo->Destroy();
    }
    else
    {
        if ((m_pSplash != 0) && (m_pSplash->m_bLoaded))
            m_pSplash->Destroy();
    }

    GameState::m_TMan->InvalidateAll();

    //SAFE_RELEASE(GameState::m_pd3dDevice);

    return 1;
}

int CTerrainApp::InitRenderer()
{
    int hr;
#if 0
    GameState::m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
    hr=GameState::m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &GameState::m_caps);
    D3DDISPLAYMODE d3ddm;
    hr=GameState::m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed       = true;
    d3dpp.SwapEffect         = D3DSWAPEFFECT_COPY;//D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat   = d3ddm.Format;//D3DFMT_X8R8G8B8
    d3dpp.EnableAutoDepthStencil = true;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

    __DWORD__ behaviorFlags=0;
    if( GameState::m_caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
        behaviorFlags |= D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else
        behaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    hr=GameState::m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
              behaviorFlags,//D3DCREATE_SOFTWARE_VERTEXPROCESSING,//D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING,
              &d3dpp, &GameState::m_pd3dDevice );




    HDC hDC;
    int nHeight;
    int nPointSize = 18;

    hDC = GetDC( 0 );

    nHeight = -( MulDiv( nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) );

    ReleaseDC( 0, hDC );

    hr = D3DXFROMWINECreateFont( GameState::m_pd3dDevice, nHeight, 0, FW_BOLD, 0, false,
                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
                 DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"),
                 &m_pd3dxFont );
    if (GameState::m_pd3dDevice == 0)
    {
        DD_ERR(-1);
        return 0;
    }
/*
    hr = m_pD3DXFROMWINE->SetClearColor(D3DRGBA(0.0f, 0.0f, 0.0f, 0.0f));
    if (FAILED(hr))
    {
        DD_ERR(hr);
        return 0;
    }

    hr = m_pD3DXFROMWINE->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER);
    if (FAILED(hr))
    {
        DD_ERR(hr);
        return 0;
    }
*/
#endif
    //IRenderer::GetRendererInstance()->Clear( true, true, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
    //         D3DCOLOR_COLORVALUE( 1.0f, 0.0f, 1.0f, 1.0f ), 1.0f, 0 );

	
    D3DFROMWINEVIEWPORT9 vp=IRenderer::GetRendererInstance()->GetViewport();
	
    float fAspect = (float(vp.Height)) / float(vp.Width);

    //LOG_PRINT("fAspect=%f\n", fAspect);
	
    GameState::m_Viewer->SetAspect(fAspect);
	
    D3DXFROMWINEMATRIX mat;
	
    D3DXFROMWINEMatrixIdentity(&mat);
	
    IRenderer::GetRendererInstance()->SetTransform(TS_WORLD, mat);
	
    return 1;
}

int CTerrainApp::RestoreContext()
{
    //int hr;

    GameState::m_TMan->InvalidateAll();
    GameState::m_TMan->RestoreAll();

    if (m_bShowingDemo)
    {
        if ((m_pFlyDemo != 0) && (m_pFlyDemo->m_bLoaded))
        {
            m_pFlyDemo->Destroy();
            m_pFlyDemo->Init();
        }
    }
    else
    {
        if ((m_pSplash != 0) && (m_pSplash->m_bLoaded))
            m_pSplash->Init();
    }

    return 1;
}

int CTerrainApp::Startup()
{
	
    int hr;

#if 1
    if (FAILED(GameState::m_MMan->Load("/RadeonTerrainDemo/material.dat")))
        return 1;
#endif
	
    if (m_pSplash != 0)
    {
	
        hr = m_pSplash->Load();
	
        if (FAILED(hr) || (!m_pSplash->m_bLoaded))
            return 0;
        hr = m_pSplash->Init();
	
        if (FAILED(hr))
            return 0;
	

        GameState::m_TMan->RestoreAll();
	
    }
    else
        return 1;

    
	return 0;
}
#if 0
LRESULT CTerrainApp::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_ACTIVATEAPP:
        {
            SetActive((bool)wParam);
        } break;
    case WM_CLOSE:
        {
            PostQuitMessage(0);
        } break;
    case WM_KEYUP:
        switch (wParam)
        {
        case VK_ESCAPE:
            {
                PostQuitMessage(0);
            } break;
        case 'W':
            {
                if (m_bShowingDemo && (m_pFlyDemo != 0))
                    m_pFlyDemo->ToggleWireframe();

            } break;
        } break;
    case WM_SETCURSOR:
        {
            SetCursor(0);
        } break;
    case WM_POWERBROADCAST:
        switch (wParam)
        {
        case PBT_APMQUERYSUSPEND:
            {
                // Going into suspended mode
                SetActive(false);
            } break;
        case PBT_APMRESUMESUSPEND:
            {
                // Recovering from suspended mode
                SetActive(true);
            } break;
        } break;
    };

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
#endif
int CTerrainApp::MessageLoop()
{
	int hr;
/*
    bool bGotMsg;
    MSG msg;


    PeekMessage(&msg, 0, 0U, 0U, PM_NOREMOVE);
    while (msg.message != WM_QUIT)
*/
    {/*
        bGotMsg = PeekMessage(&msg, 0, 0U, 0U, PM_REMOVE);
        if (bGotMsg)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else*/
        {
            if (m_bActive)
            {
                    //IRenderer::GetRendererInstance()->BeginScene();
                    UpdateTime();//???
                    //IRenderer::GetRendererInstance()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                    //  D3DCOLOR_COLORVALUE( 1.0f, 0.0f, 1.0f, 1.0f ), 1.0f, 0);

                //hr =
		Draw();
                //IRenderer::GetRendererInstance()->EndScene();
                //IRenderer::GetRendererInstance()->Present( 0, 0, 0, 0 );
/*
                if (FAILED(hr))
        {
                    DD_ERR(hr);
                    m_bInitialized = false;
                    PostQuitMessage(0);
                }*/
            }
            //else
              //  WaitMessage();
        }
        if (m_bShowingDemo && (!m_pFlyDemo->m_bLoaded))
        {
            if (m_pFlyDemo != 0)
            {
		#if 1
                hr = m_pFlyDemo->Load();
		#endif
                if (FAILED(hr) || (!m_pFlyDemo->m_bLoaded))
                    return 0;
                hr = m_pFlyDemo->Init();
                if (FAILED(hr))
                    return 0;

                GameState::m_TMan->RestoreAll();
                StartTime();
                m_dwLastPerfTime = m_dwStartTime;
                m_dwFPS = 0;
                m_dwPPS = 0;
                m_dwLoopCount = 0;
                m_strStats[0] = '\0';
            }
            else
                return 0;
        }
    }

    return 1;
}



void CTerrainApp::SetActive(bool bActive)
{/*
    if (bActive)
    {
        ShowCursor(false);
    }
    else
    {
        DrawMenuBar(m_hWnd);
        RedrawWindow(m_hWnd, 0, 0, RDW_FRAME);
        ShowCursor(true);
    }
*/
    m_bActive = bActive;
}

void CTerrainApp::StartTime()
{
    m_dwStartTime = timeGetTime();
}

void CTerrainApp::UpdateTime()
{
    __DWORD__ time = timeGetTime();

    if (!m_bActive)
        m_dwStartTime += time - m_dwTime;
    m_dwTime = time;
}

int CTerrainApp::Draw()
{
    int hr;

    //hr =
	DrawFrame();
    if (0)//FAILED(hr))
    {
        return 0;
    }

    return 1;
}

int CTerrainApp::DrawFrame()
{
    //
MessageLoop();
	//if (!m_bInitialized)
        //return 0;

    //LOG_PRINT("BeginScene()\n");
        IRenderer::GetRendererInstance()->BeginScene();
    {
        
	//LOG_PRINT("Clear\n");
	//IRenderer::GetRendererInstance()->Clear(0, 0, /*D3DCLEAR_TARGET |*/ D3DCLEAR_ZBUFFER,
        //    0x00000000, 1.0f, 0);

	float f=1.0f/256.0f;
	//D3DXFROMWINEVECTOR4 color(64.0f, 147.0f, 198.0f, 256.0f);
	D3DXFROMWINEVECTOR4 color(116.0f, 162.0f, 195.0f, 256.0f);
	IRenderer::GetRendererInstance()->Clear(true, true, color*f);
        
	////LOG_PRINT("Clear\n");

        //float fTime = (float)(m_dwTime - m_dwStartTime) / 1000.0f;
	float fTime = timeGetTime()/1000.0f;

	//LOG_PRINT("m_bShowingDemo	 = %d\n",m_bShowingDemo?1:0);
	//LOG_PRINT("m_pFlyDemo->m_bLoaded = %d\n",m_pFlyDemo->m_bLoaded?1:0);
	//LOG_PRINT("m_pSplash->m_bLoaded  = %d\n",m_pSplash->m_bLoaded?1:0);

	if (m_bShowingDemo && m_pFlyDemo->m_bLoaded)
        {
            
		m_pFlyDemo->Render(fTime,bTerrain,bSky,bWireframe);
            m_dwFPS++;
            m_dwLoopFrames++;
            m_dwPPS += m_pFlyDemo->CountPolys(bTerrain,bSky);

            if (m_dwTime - m_dwLastPerfTime > 1000.0)
            {
                stx_snprintf(m_strStats, 128, "Framerate: %d fps, polygon count: %.3f Mpps", m_dwFPS,
                    (float)m_dwPPS / 1000000.0);
                m_dwFPS = 0;
                m_dwPPS = 0;
                m_dwLastPerfTime = m_dwTime;
            }
            /* m_pD3DXFROMWINE->DrawDebugText(0.01f, 0.0f, 0x00ffff80, m_strStats);
            RECT destRect1;
                SetRect( &destRect1, 10, 10, 0, 0 );
                m_pd3dxFont->DrawText( 0,m_strStats, -1, &destRect1, DT_NOCLIP,
                   D3DXFROMWINECOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
		*/
		IRenderer::GetRendererInstance()->drawText(m_strStats, 10, 10, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());
		

        }
        else if (m_pSplash->m_bLoaded)
        {
            
		m_pSplash->Render(fTime);
            if ((float)(m_dwTime - m_dwStartTime) / 1000.0f > 1.0f)
		{
                m_bShowingDemo = true;
	
	////LOG_PRINT("m_bShowingDemo = true\n");
		}
        }
    	
	STXGUI::update();
	//LOG_PRINT("EndScene()\n");
        IRenderer::GetRendererInstance()->EndScene();
	////LOG_PRINT("EndScene\n");
        IRenderer::GetRendererInstance()->Present();
	//LOG_PRINT("Present\n");
	//stx_exit(0);
    }

    return 1;
}

int CTerrainApp::Run()
{
    
	int hr;
/*
    hr = ParseCommandLine(lpszCmdLine);
    if (FAILED(hr))
        return 0;

    if (!hPrevInstance)
    {
        hr = InitApplication(hInstance);
        if (FAILED(hr))
            return 0;
    }

    hr = InitInstance(hInstance, nCmdShow);
    if (FAILED(hr))
        return 0;
*/
    
	hr = InitializeD3DXFROMWINE();
    if (0)//FAILED(hr))
        return 0;

    
	hr = Startup();
    
	if (0)//FAILED(hr))
    {
        m_bInitialized = false;
    }

    if (m_bInitialized)
    {
        StartTime();
        //MessageLoop();
    }

    hr = UnInitializeD3DXFROMWINE();
    if (0)//FAILED(hr))
    {
        return 0;
    }
//
    return 1;
}

void CTerrainApp::actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == CHECK_BOX)
  {
    GUICheckBox   *checkBox = (GUICheckBox*)sourceRectangle;
    if(checkBox->isPressed())
    {
      if(callbackString == "Terrain")      	bTerrain = checkBox->isChecked();
      if(callbackString == "Sky")      		bSky = checkBox->isChecked();
      if(callbackString == "WireFrame")      	bWireframe = checkBox->isChecked();
    }
  }
}

