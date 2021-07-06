

//-----------------------------------------------------------------------------
// File: Terrain.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "Helper.h"
#include "Terrain.h"
#include "Viewer.h"
#include "DXErrors.h"

#define SFC_SIGN "SFC_FILE"

CTerrain::CTerrain() : CModel()
{
    m_pOT = 0;
    m_pVS = 0;
    m_pRQ = new CRenderQueue();
//  m_bDetailTexture = false;
	m_bLoaded = false;
}

CTerrain::~CTerrain()
{
    SAFE_DELETE(m_pOT);
    SAFE_DELETE(m_pVS);
    SAFE_DELETE(m_pRQ);
}

int CTerrain::Load()
{
    
	//LOG_FNLN;
	FILE *fp;
    CHAR signature[9];
    __DWORD__ version;

    
	std::string fstr=stx_convertpath("/RadeonTerrainDemo/terrain.dat");
	////LOG_PRINT("CTerrain::Load:fopen=%s\n", fstr.c_str());
	fp = fopen(fstr.c_str(), "rb");
    
	fread(signature, sizeof(CHAR), 8, fp);

	signature[8]='\0';
    
	fread(&version, sizeof(__DWORD__), 1, fp);

    
	if (strcmp(signature, SFC_SIGN))
    {
        fclose(fp);
        return 0;
    }

	//LOG_FNLN;
    
	m_pVS = new CVertexStore(fp);
    
	m_pOT = new COctreeNode(fp);

    
	fclose(fp);

	m_bLoaded = true;

	//LOG_FNLN;
	//LOG_PRINT("CTerrain::Load\n");
	//LOG_PRINT("m_bLoaded = %d\n",m_bLoaded?1:0);
    
	return 1;
}

int CTerrain::Init()
{
	if (!m_bLoaded)
		return 0;
	return m_pVS->Init();
}

void CTerrain::Destroy()
{
	m_pVS->Destroy();
}

int CTerrain::Render()
{
	//LOG_FNLN;
	//LOG_PRINT("CTerrain::Render\n");
	//LOG_PRINT("m_bLoaded = %d\n",m_bLoaded?1:0);
if (!m_bLoaded)
		return 0;
//LOG_PRINT("m_pOT = %d\n",m_pOT);
    if (m_pOT != 0)
    {
        CLIPVOLUME cv;
        D3DXFROMWINEVECTOR3 pos = GameState::m_Viewer->GetPos();

        GameState::m_Viewer->ComputeClipVolume(cv);
        
	m_pRQ->ClearQueue();
        
	m_pOT->Cull(m_pRQ, cv, pos);
	#if 0
        GameState::m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, true);
        GameState::m_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, true);
        GameState::m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
        GameState::m_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);

        float start = 50000.0f;
        float end = 400000.0f;
        GameState::m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, 0x00ADB4C7);
        GameState::m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(__DWORD__*)(&start));
        GameState::m_pd3dDevice->SetRenderState(D3DRS_FOGEND, *(__DWORD__*)(&end));

        GameState::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
        GameState::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	#endif
        
	
	//LOG_FNLN;
	m_pRQ->Render(m_pVS);

        //GameState::m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, false);
	
	
    }

	
    return 1;
}

__DWORD__ CTerrain::CountPolys()
{
	if (!m_bLoaded)
		return 0;
    return m_pRQ->CountPolys();
}

