

//-----------------------------------------------------------------------------
// File: SplashScreen.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "Helper.h"
#include "SplashScreen.h"
#include "Texture.h"
#include "TManager.h"
#include "DXErrors.h"


CSplashScreen::CSplashScreen() : CDemo()
{
    m_pTextureID = -1;
    m_dwTexIndx = -1;
    m_bLoaded = false;
	
	////LOG_PRINT("m_bLoaded = false\n");
    m_bInitialized = false;
    m_vMeshVB=-1;
}

CSplashScreen::~CSplashScreen()
{
    DeleteTexture();
    //if(m_vMeshVB)m_vMeshVB->Release();
}

int CSplashScreen::Load()
{
    int hr;

    //D3DXFROMWINEVECTOR3 n = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
    m_vMesh[0] = D3DVERTEX2(D3DXFROMWINEVECTOR3(-1.0f,-1.0f, 0.0f), /*n,*/ 1.0f, 1.0f);
    m_vMesh[1] = D3DVERTEX2(D3DXFROMWINEVECTOR3(-1.0f, 1.0f, 0.0f), /*n,*/ 1.0f, 0.0f);
    m_vMesh[2] = D3DVERTEX2(D3DXFROMWINEVECTOR3( 1.0f,-1.0f, 0.0f), /*n,*/ 0.0f, 1.0f);
    m_vMesh[3] = D3DVERTEX2(D3DXFROMWINEVECTOR3( 1.0f, 1.0f, 0.0f), /*n,*/ 0.0f, 0.0f);
#if 0
    ////LOG_PRINT("GameState::m_pd3dDevice=%d\n",GameState::m_pd3dDevice);
    hr=GameState::m_pd3dDevice->CreateVertexBuffer( 4*sizeof(D3DVERTEX),D3DUSAGE_WRITEONLY,
                         D3DVERTEX::FVF,
                      /*D3DPOOL_MANAGED*/D3DPOOL_DEFAULT, &m_vMeshVB, 0 );
    ////LOG_PRINT("hr=%d\n",hr);
    D3DVERTEX* pVertices0=0;
    m_vMeshVB->Lock( 0, 4*sizeof(D3DVERTEX), (void**)&pVertices0, 0 );
    stx_memcpy( pVertices0, m_vMesh, 4*sizeof(D3DVERTEX) );
    m_vMeshVB->Unlock();
#else
	m_vMeshVB =  IRenderer::GetRendererInstance()->addVertexBuffer(4 * sizeof(D3DVERTEX2), STATIC, m_vMesh);
#endif
    CTexture *pTexture = new CTexture();
    m_pTextureID = pTexture->Create("splash", false, false);
    m_dwTexIndx=GameState::m_TMan->AddTexture(pTexture);

	m_bLoaded = true;
	
	////LOG_PRINT("m_bLoaded = true\n");

    return 1;
}

int CSplashScreen::Init()
{
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

    GameState::m_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_DITHERENABLE, true);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, true);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, true);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, false);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_LOCALVIEWER, false);
    GameState::m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, false);
#endif
    D3DFROMWINEVIEWPORT9 vp=IRenderer::GetRendererInstance()->GetViewport();//&vp);
    float fAspect = ((float)vp.Height) / vp.Width;

    // Load world transform
    D3DXFROMWINEMATRIX matWorld;
    D3DXFROMWINEMatrixIdentity(&matWorld);
    IRenderer::GetRendererInstance()->SetTransform(TS_WORLD, matWorld);

    // Setup projection
    D3DXFROMWINEMATRIX matProj;
    D3DXFROMWINEMatrixPerspectiveFovLH(&matProj, D3DXFROMWINE_PI / 3.0f, fAspect, 0.1f, 100.0f);
    IRenderer::GetRendererInstance()->SetTransform(TS_PROJECTION, matProj);

    // Load view transform
    D3DXFROMWINEVECTOR3 vViewer(0.0f, 0.0f, 2.0f);
    D3DXFROMWINEVECTOR3 vDir(0.0f, 0.0f, -1.0f);
    D3DXFROMWINEVECTOR3 vUp(0.0f, 1.0f, 0.0f);
    D3DXFROMWINEMATRIX matView;
    ComputeViewMatrix(&matView, &vViewer, &vDir, &vUp);
    IRenderer::GetRendererInstance()->SetTransform(TS_VIEW, matView);

    m_bInitialized = true;

    return 1;
}

void CSplashScreen::Destroy()
{
    m_bInitialized = false;
}

int CSplashScreen::Render(float fTime )
{
	//LOG_PRINT("m_bLoaded=%x\n", m_bLoaded);
	//LOG_PRINT("m_bInitialized=%x\n", m_bInitialized);
	//LOG_PRINT("m_pTextureID=%d\n", m_pTextureID);	
    if ((m_bLoaded) && (m_bInitialized) && (m_pTextureID != -1))
    {
        //CTexture *pTexture = GameState::m_TMan->GetTexture(m_pTextureID);
        ////LOG_PRINT("pTexture=%x\n", pTexture);
	if (//pTexture
		m_pTextureID != -1)
	{
#if 1
	//LOG_FNLN;
	//LOG_PRINT("CSplashScreen::Render:\n");
	//LOG_PRINT("m_pTextureID=%d\n", //pTexture->GetSurface() m_pTextureID);
	//LOG_PRINT("GetViewportWidth()=%d\n", IRenderer::GetRendererInstance()->GetViewportWidth());
	//LOG_PRINT("GetViewportWidth()=%d\n", IRenderer::GetRendererInstance()->GetViewportHeight());
      	//LOG_PRINT("drawTexture\n");
#if 1
	IRenderer::GetRendererInstance()->InitTex(IRenderer::GetRendererInstance()->GettexShader(),IRenderer::GetRendererInstance()->GettexVF());
	float dx=IRenderer::GetRendererInstance()->GetViewportWidth();
	float dy=IRenderer::GetRendererInstance()->GetViewportHeight();

	float vertices[] =
	{
		dx, 0.0f, 1.0f, 0.0f,
		dx, dy, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, dy, 0.0f, 1.0f
	};
		
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GettexVF());
//LOG_PRINT("CSplashScreen::Render:setTexture:TexID=%d\n", m_pTextureID);
	
	IRenderer::GetRendererInstance()->setTexture("Base", m_pTextureID);
	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f));
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(
		PRIM_TRIANGLE_STRIP,
		IRenderer::GetRendererInstance()->getPrimitiveCount(PRIM_TRIANGLE_STRIP, 4),
		vertices, vertices, 4*sizeof(float));
#endif
#endif
        }
        //else
        //    IRenderer::GetRendererInstance()->SetTexture(0, 0);
    }

    return 1;
}

void CSplashScreen::DeleteTexture()
{
    if (m_dwTexIndx != -1)
    {
        GameState::m_TMan->DeleteTexture(m_dwTexIndx);
	m_dwTexIndx = -1;
        m_pTextureID = -1;
    }
}

