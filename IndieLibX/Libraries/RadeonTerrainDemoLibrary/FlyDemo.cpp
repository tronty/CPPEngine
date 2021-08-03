

//-----------------------------------------------------------------------------
// File: FlyDemo.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "Helper.h"
#include "FlyDemo.h"
#include "Viewer.h"
#include "DXErrors.h"


CFlyDemo::CFlyDemo() : CDemo()
{
    m_pSky = new CSkyDome();
    m_pTerrain = new CTerrain();
    m_pPath = new CPath();

    m_bLoaded = false;
	
	////LOG_PRINT("m_bLoaded = false\n");
    m_bInitialized = false;
}

CFlyDemo::~CFlyDemo()
{
    SAFE_DELETE(m_pSky);
    SAFE_DELETE(m_pTerrain);
    SAFE_DELETE(m_pPath);
}

int CFlyDemo::Load()
{
	////LOG_PRINT("CFlyDemo::Load\n");
    // Load geometry
    #if 1
        if (FAILED(m_pSky->Load()))
        return 0;
	#endif
    #if 1
        if (FAILED(m_pTerrain->Load()))
        return 0;
	#endif

#if 1

    // Load fly path
    m_pPath->Load("/RadeonTerrainDemo/path.dat");
#endif
    m_bLoaded = true;
	
	////LOG_PRINT("m_bLoaded = true\n");

    return 1;
}

int CFlyDemo::Init()
{
    ////LOG_PRINT("CFlyDemo::Init3\n");
    int hr;
#if 0
    GameState::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    GameState::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    GameState::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    GameState::m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    GameState::m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    GameState::m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    GameState::m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);//D3DTSS_ADDRESS, D3DTADDRESS_WRAP);
    GameState::m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);//D3DTSS_ADDRESS, D3DTADDRESS_WRAP);
    GameState::m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);//D3DTSS_ADDRESS, D3DTADDRESS_WRAP);

    //GameState::m_pd3dDevice->SetRenderState(D3DRS_TEXTUREPERSPECTIVE, true);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_DITHERENABLE, true);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, true);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, true);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif
#if 1
    hr = m_pSky->Init();
#endif
#if 1
    hr = m_pTerrain->Init();
#endif
    m_bInitialized = true;

    return 1;
}

void CFlyDemo::Destroy()
{
    m_bInitialized = false;
    m_pSky->Destroy();
    m_pTerrain->Destroy();
}

int CFlyDemo::Render(float fTime, bool bTerrain, bool bSky, bool bWireframe )
{
	//stx_exit(0);
	//LOG_PRINT("CFlyDemo::Render\n");
	//LOG_PRINT("m_bLoaded = %d\n",m_bLoaded?1:0);
	//LOG_PRINT("m_bInitialized  = %d\n",m_bInitialized?1:0);
    if ((m_bLoaded) && (m_bInitialized))
    {
        D3DXFROMWINEVECTOR3 v, vpos;
        D3DXFROMWINEQUATERNION q;

        // Calculate position
        m_pPath->Play(fTime, vpos, q);

#if 0
        if (m_bWireframe)
        {
            IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
            // Clear only when drawing wireframe to prevent overdraw
            //  use some weird color so wireframe stands out
            IRenderer::GetRendererInstance()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                0x00FF00FF, 1.0f, 0);
        }
#elif 0
		if (bWireframe)
		{
			//LOG_PRINT("bWireframe\n");
			IRenderer::GetRendererInstance()->setRasterizerState(
				IRenderer::GetRendererInstance()->GetfillWireFrame());
		}
		else
				IRenderer::GetRendererInstance()->setRasterizerState(
				IRenderer::GetRendererInstance()->GetfillSolid());
#endif

	//LOG_FNLN;
        if(bSky)
	{
		//LOG_PRINT("bSky\n");
        	// For sky rendering place viewer in the center of skydome
        	v = D3DXFROMWINEVECTOR3(0.0f, vpos.y  * 0.2f , 0.0f);
        	GameState::m_Viewer->UpdateViewer(v, q);
        	// Render sky
	//LOG_FNLN;
        	m_pSky->Render();
	//LOG_FNLN;
	}

	//LOG_FNLN;
	if(bTerrain)
	{
		//LOG_PRINT("bTerrain\n");
        	GameState::m_Viewer->UpdateViewer(vpos, q);
        	// Render terrain
        	//LOG_FNLN;
		m_pTerrain->Render();
	}

        //if (m_bWireframe)
        //    GameState::m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    }

    return 1;
}

__DWORD__ CFlyDemo::CountPolys(bool bTerrain, bool bSky)
{
	__DWORD__ r=0;
        if(bSky) r+=m_pSky->CountPolys();
	if(bTerrain) r+=m_pTerrain->CountPolys();
    return r;
}

float CFlyDemo::GetFlightTime()
{
    return m_pPath->GetPathTime();
}

