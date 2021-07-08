//-----------------------------------------------------------------------------
// File: TriangleList.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <math.h>
#include "Helper.h"
#include "TriangleList.h"
#include "DXErrors.h"
#include "LitVertexVS2PS.h"
#include "TerrainApp.h"

CTriangleList::CTriangleList()
{
    m_pIndicesIB=-1;
    m_pIndices = 0;
    m_dwMaterialID = -1;
    m_pTexID = -1;
}

CTriangleList::CTriangleList(FILE *fp)
{
    m_pIndicesIB=-1;
    m_pIndices = 0;
    m_dwMaterialID = -1;
    m_pTexID = -1;

    Load(fp);
}

CTriangleList::~CTriangleList()
{
    DeleteArrays();
}

void CTriangleList::DeleteArrays()
{
    //if(m_pIndicesIB)m_pIndicesIB->Release();
    if (m_pIndices != 0)
    {
        delete []m_pIndices;
        m_pIndices = 0;
    }
}

int CTriangleList::Load(FILE *fp)
{
    DeleteArrays();

    fread(&m_dwMaterialID, sizeof(__DWORD__), 1, fp);

    m_pTexID = -1;

    fread(&m_dwIndNum, sizeof(__DWORD__), 1, fp);
    fread(&m_dwVBID, sizeof(__DWORD__), 1, fp);

    m_pIndices = new __WORD__[m_dwIndNum];
    fread(m_pIndices, sizeof(__WORD__), m_dwIndNum, fp);

	m_pIndicesIB = IRenderer::GetRendererInstance()->addIndexBuffer(m_dwIndNum,sizeof(__WORD__), STATIC, m_pIndices);

      delete []m_pIndices;
      m_pIndices = 0;

    return 1;
}
int CTriangleList::Render( CVertexStore *pVS, int hr)
{
    
	__DWORD__ size;
    VertexBufferID pVB;

    pVB = pVS->GetVertexBuffer(m_dwVBID, &size);
	
	if (pVB==-1)
		return 1;

#if 0
    GameState::m_pd3dDevice->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST, pVB, 0, size, m_pIndices, m_dwIndNum, 0);
    GameState::m_pd3dDevice->SetStreamSource( 0, pVB, 0,sizeof(LITVERTEX));
    GameState::m_pd3dDevice->SetIndices(m_pIndicesIB);
    GameState::m_pd3dDevice->SetFVF( D3DFVF_LVERTEX1 );
    GameState::m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0,0,size, 0, m_dwIndNum/3);
    GameState::m_pd3dDevice->SetFVF(0);
#else
#if 1
	//LOG_FNLN;
	//LOG_PRINT("CTriangleList::Render:\n");
	//LOG_PRINT("VF=%x\n", LitVertexVS2PS::LVERTEX1VF);
	//LOG_PRINT("VB=%x\n", pVB);
	//LOG_PRINT("IB=%x\n", m_pIndicesIB);
	//LOG_PRINT("size=%x\n", size);
	//LOG_PRINT("IndNum=%x\n", m_dwIndNum);
      	//LOG_PRINT("DrawIndexedPrimitive\n");
#endif
	LitVertexVS2PS::Begin();
	//LOG_PRINT("CTriangleList::Render::TexID=%d\n", hr);	
	LitVertexVS2PS::SetTexture(hr);
	
	//???D3DCULL_CW
	//IRenderer::GetRendererInstance()->GetcullBack();
	//IRenderer::GetRendererInstance()->GetcullFront();

	
	    IRenderer::GetRendererInstance()->setVertexBuffer(0,pVB);
	    IRenderer::GetRendererInstance()->setIndexBuffer(m_pIndicesIB);

#if 1
		if (app.bWireframe)
		{
			//LOG_PRINT("bWireframe\n");
			IRenderer::GetRendererInstance()->setRasterizerState(
				IRenderer::GetRendererInstance()->GetfillWireFrame());
		}
		else
				IRenderer::GetRendererInstance()->setRasterizerState(
				IRenderer::GetRendererInstance()->GetfillSolid());
#endif


	    IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, size, 0, m_dwIndNum/3/* , RESET_VB|RESET_IB */);
#endif
    return 1;
}
