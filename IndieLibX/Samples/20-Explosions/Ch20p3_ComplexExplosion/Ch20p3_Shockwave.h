/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
// Ch20p3_Shockwave.h: interface for the CShockwave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CH20P3_SHOCKWAVE_H__9CDC04DB_3B93_4FAC_8EBA_0549E65652DA__INCLUDED_)
#define AFX_CH20P3_SHOCKWAVE_H__9CDC04DB_3B93_4FAC_8EBA_0549E65652DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommonFuncs.h"
#include "D3DHelperFuncs.h"

class CShockwave  
{
public:
	CShockwave() { 
		m_pTexture = 0; m_pVB = 0; m_pd3dDevice = 0; 
		m_iNumDivisions = 0; m_fThickness = 0.0f; m_fSize = 0.0f;
		m_iNumVerts = 0;
		m_bIsRunning = false;
	}
	
	virtual ~CShockwave();

	int RestoreDeviceObjects( pDev, const char *strTexture,
		float fSize, float fThickness, int iNumDivisions,
		float fExpandRate, float fLifetime);

	void InvalidateDeviceObjects();
	void Render();

	virtual void Begin() { m_bIsRunning = true; }
	virtual void Pause() { m_bIsRunning = false; }
	virtual void Stop()  { Pause(); m_fAge = 0.0f; m_fScale = 0.0001f; }
	virtual bool IsRunning() { return(m_bIsRunning); }

	void SetAlpha(int i255Alpha);

	void Update(float fTimeDelta)
	{
		if (IsRunning()) {
			m_fScale += m_fExpandRate * fTimeDelta;
			m_fAge += fTimeDelta;
			SetAlpha((int)(255.0f-(255.0f*(m_fAge/m_fLifetime))));
			if (m_fAge > m_fLifetime) Stop();
		}
	}

  D3DXFROMWINEVECTOR3 &Pos() { return(m_vPos); }

	VertexBufferID GetVB() { return(m_pVB); }

protected:
	bool m_bIsRunning;
	TextureID m_pTexture;
	VertexBufferID m_pVB;
	 m_pd3dDevice;
	D3DXFROMWINEVECTOR3 m_vPos;
	int m_iNumDivisions;
	float m_fThickness;
	float m_fLifetime;
	float m_fAge;
	float m_fExpandRate;
	float m_fSize;
	float m_fScale;
	int m_iNumVerts;

};

#endif // !defined(AFX_CH20P3_SHOCKWAVE_H__9CDC04DB_3B93_4FAC_8EBA_0549E65652DA__INCLUDED_)
