//-----------------------------------------------------------------------------
// File: VertexStore.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "Helper.h"
#include "VertexStore.h"
#include "DXErrors.h"

CVertexStore::CVertexStore(FILE *fp)
{
    D3DXFROMWINEVECTOR3 min_( 1e10f, 1e10f, 1e10f);
    D3DXFROMWINEVECTOR3 max_( -1e10f, -1e10f, -1e10f);
    fread(&m_dwVBCount, sizeof(__DWORD__), 1, fp);
    m_pVB = new VB[m_dwVBCount];
    for(__DWORD__ i = 0; i < m_dwVBCount; i++)
    {
        fread(&m_pVB[i].dwSize, sizeof(__DWORD__), 1, fp);
	LITVERTEX* pVertices = new LITVERTEX[m_pVB[i].dwSize];
	fread(pVertices, sizeof(LITVERTEX), m_pVB[i].dwSize, fp);
        m_pVB[i].pVertices = new LITVERTEX2[m_pVB[i].dwSize];
	for(__DWORD__ ii = 0; ii < m_pVB[i].dwSize; ii++)
	{
		m_pVB[i].pVertices[ii]=LITVERTEX2(pVertices[ii]);
		max_.x = std::max( max_.x, m_pVB[i].pVertices[ii].x);
		max_.y = std::max( max_.y, m_pVB[i].pVertices[ii].y);
		max_.z = std::max( max_.z, m_pVB[i].pVertices[ii].z);
		min_.x = std::min( min_.x, m_pVB[i].pVertices[ii].x);
		min_.y = std::min( min_.y, m_pVB[i].pVertices[ii].y);
		min_.z = std::min( min_.z, m_pVB[i].pVertices[ii].z);
	}

        m_pVB[i].pVB = -1;
    }
	LOG_PRINT("min.x=%f\n", min_.x);
	LOG_PRINT("min.y=%f\n", min_.y);
	LOG_PRINT("min.z=%f\n", min_.z);
	LOG_PRINT("max.x=%f\n", max_.x);
	LOG_PRINT("max.y=%f\n", max_.y);
	LOG_PRINT("max.z=%f\n", max_.z);
/*
min.x=-190080.000000
min.y=-14532.769531
min.z=-190080.000000
max.x=447661.750000
max.y=23905.882812
max.z=348207.750000

*/
}

CVertexStore::~CVertexStore()
{
/*
    for(__DWORD__ i = 0; i < m_dwVBCount; i++)
    {
        SAFE_DELETE_ARRAY(m_pVB[i].pVertices);
    }
    SAFE_DELETE_ARRAY(m_pVB);
*/
}

int CVertexStore::Init()
{
	////LOG_PRINT("m_dwVBCount=%d\n", m_dwVBCount);
    for(__DWORD__ i = 0; i < m_dwVBCount; i++)
    {
#if 0
        if (m_pVB[i].pVB == 0)
        {
            SAFE_RELEASE(m_pVB[i].pVB);

            GameState::m_pd3dDevice->CreateVertexBuffer(m_pVB[i].dwSize*sizeof(LITVERTEX),
                               D3DUSAGE_WRITEONLY,
                               D3DFVF_LVERTEX1,
                               /*D3DPOOL_MANAGED*/D3DPOOL_DEFAULT,
                               &m_pVB[i].pVB,
                               0 );
        }

        LPLITVERTEX pvbVertices;
        if (SUCCEEDED(m_pVB[i].pVB->Lock( 0, m_pVB[i].dwSize*sizeof(LITVERTEX), (void**)&pvbVertices, 0 )))
        {
            stx_memcpy(pvbVertices, m_pVB[i].pVertices, m_pVB[i].dwSize * sizeof(LITVERTEX));
            m_pVB[i].pVB->Unlock();
        }
        else
            return 0;
#else
	m_pVB[i].pVB =	IRenderer::GetRendererInstance()->addVertexBuffer(m_pVB[i].dwSize*sizeof(LITVERTEX2), STATIC, m_pVB[i].pVertices);//,m_pVB[i].pVB);
#endif
    }

    return 1;
}

void CVertexStore::Destroy()
{
/*
    for(__DWORD__ i = 0; i < m_dwVBCount; i++)
    {
        SAFE_RELEASE(m_pVB[i].pVB);
    }
*/
}

VertexBufferID CVertexStore::GetVertexBuffer(__DWORD__ index, __DWORD__ *size)
{
    if (index < m_dwVBCount)
    {
        *size = m_pVB[index].dwSize;
        return m_pVB[index].pVB;
    }
    else
        return -1;
}
