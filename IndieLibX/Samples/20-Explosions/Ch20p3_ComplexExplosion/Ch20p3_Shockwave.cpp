/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
// Ch20p3_Shockwave.cpp: implementation of the CShockwave class.
//
//////////////////////////////////////////////////////////////////////

#include "Ch20p3_Shockwave.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShockwave::~CShockwave()
{

}

int CShockwave::RestoreDeviceObjects( pDev, 
																			const char *strTexture,
																			float fSize, float fThickness,
																			int iNumDivisions,
																			float fExpandRate,
																			float fLifetime)
{
	int hr;
	m_pd3dDevice = pDev;
	m_iNumDivisions = iNumDivisions;
	m_fSize = fSize;
	m_fThickness = fThickness;
	m_fExpandRate = fExpandRate;
	m_fLifetime = fLifetime;

	if (iNumDivisions < 4) return(E_FAIL);

	// load texture
	{
		if (FAILED(hr = D3DXFROMWINECreateTextureFromFile(m_pd3dDevice, 
    strTexture, &m_pTexture))) {
			return(hr);
		}
  }

	// create shockwave
	{
		m_iNumVerts = iNumDivisions * 6;

		// Create vertex buffer for shockwave
		hr = m_pd3dDevice->CreateVertexBuffer(m_iNumVerts*sizeof(VERTEX_XYZ_DIFFUSE_TEX1),
																					 D3DUSAGE_WRITEONLY, D3DFVF_XYZ_DIFFUSE_TEX1,
																					 D3DPOOL_MANAGED, &m_pVB);
		if(FAILED(hr)) return(hr);

		// Fill vertex buffer
	  VERTEX_XYZ_DIFFUSE_TEX1* pVertices;
		if(FAILED(m_pVB->Lock(0, 0, (BYTE**)&pVertices, 0))) return hr;


		// calculate number of vertices
		float fStep = 360.0f / iNumDivisions;
		

		for (float q=0.0f; q < 360.0f; q+= fStep)	 {
			// calculate x1, z1, x2, z2, x3,z3 and x4,z4 points
			float x1 = m_fSize * cos(TORADIANS(q));
			float z1 = m_fSize * sin(TORADIANS(q));
			float x2 = (m_fSize-m_fThickness) * cos(TORADIANS(q));
			float z2 = (m_fSize-m_fThickness) * sin(TORADIANS(q));

			float x3 = m_fSize * cos(TORADIANS(q+fStep));
			float z3 = m_fSize * sin(TORADIANS(q+fStep));
			float x4 = (m_fSize-m_fThickness) * cos(TORADIANS(q+fStep));
			float z4 = (m_fSize-m_fThickness) * sin(TORADIANS(q+fStep));

			pVertices->position.x   = x2;
      pVertices->position.y   = 0.0f;
      pVertices->position.z   = z2;
      pVertices->color        = D3DCOLOR_ARGB(255,255,255,255);
      pVertices->tu           = 0.0f; 
      pVertices->tv           = 1.0f;
      pVertices++;

			pVertices->position.x   = x1;
      pVertices->position.y   = 0.0f;
      pVertices->position.z   = z1;
      pVertices->color        = D3DCOLOR_ARGB(255,255,255,255);
      pVertices->tu           = 0.0f; 
      pVertices->tv           = 0.0f;
      pVertices++;

  		pVertices->position.x   = x4;
      pVertices->position.y   = 0.0f;
      pVertices->position.z   = z4;
      pVertices->color        = D3DCOLOR_ARGB(255,255,255,255);
      pVertices->tu           = 1.0f; 
      pVertices->tv           = 1.0f;
      pVertices++;

			pVertices->position.x   = x1;
      pVertices->position.y   = 0.0f;
      pVertices->position.z   = z1;
      pVertices->color        = D3DCOLOR_ARGB(255,255,255,255);
      pVertices->tu           = 0.0f; 
      pVertices->tv           = 0.0f;
      pVertices++;

			pVertices->position.x   = x3;
      pVertices->position.y   = 0.0f;
      pVertices->position.z   = z3;
      pVertices->color        = D3DCOLOR_ARGB(255,255,255,255);
      pVertices->tu           = 1.0f;
      pVertices->tv           = 0.0f;
      pVertices++;

  		pVertices->position.x   = x4;
      pVertices->position.y   = 0.0f;
      pVertices->position.z   = z4;
      pVertices->color        = D3DCOLOR_ARGB(255,255,255,255);
      pVertices->tu           = 1.0f; 
      pVertices->tv           = 1.0f;
      pVertices++;

		}

	}

  m_pVB->Unlock();
	return 0;
}

void CShockwave::SetAlpha(int i255Alpha)
{
	VERTEX_XYZ_DIFFUSE_TEX1* pVertices;
	if(FAILED(m_pVB->Lock(0, 0, (BYTE**)&pVertices, 0))) return;

	for (int q=0; q < m_iNumVerts; q++) {
		pVertices->color = D3DCOLOR_ARGB(i255Alpha,255,255,255);
		pVertices++;
	}

  m_pVB->Unlock();
}
	
void CShockwave::InvalidateDeviceObjects()
{
	////SAFE_RELEASE(m_pTexture);
	////SAFE_RELEASE(m_pVB);
}

void CShockwave::Render()
{
	D3DXFROMWINEMATRIX matWorld, matScale, mat;
  D3DXFROMWINEMatrixIdentity( &matWorld );
	D3DXFROMWINEMatrixIdentity( &matScale );
	D3DXFROMWINEMatrixTranslation(&matWorld, m_vPos.x, m_vPos.y, m_vPos.z);
	D3DXFROMWINEMatrixScaling(&matScale, m_fScale, 1.0f, m_fScale);
	mat = matScale * matWorld;
	
  m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat);

	m_pd3dDevice->SetTexture(0, m_pTexture);
  m_pd3dDevice->SetVertexShader(D3DFVF_XYZ_DIFFUSE_TEX1);
  m_pd3dDevice->SetStreamSource(0, m_pVB, sizeof(VERTEX_XYZ_DIFFUSE_TEX1));
  m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumVerts/3);
}


