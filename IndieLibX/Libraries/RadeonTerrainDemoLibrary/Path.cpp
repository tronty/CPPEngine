

//-----------------------------------------------------------------------------
// File: Path.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "DXErrors.h"
#include "Helper.h"
#include "Path.h"

CPath::CPath()
{
    m_dwCount = 0;
    m_pPoints = 0;
}

CPath::~CPath()
{
    SAFE_DELETE_ARRAY(m_pPoints);
}

int CPath::Load(char *strPathName)
{
    
	FILE *fp;
    
	
    SAFE_DELETE_ARRAY(m_pPoints);
        
	
	m_dwCount = 0;

       
	 
	std:string fn=stx_convertpath(strPathName);
	//LOG_PRINT("CPath::Load:fopen:%s\n", fn.c_str());
	fp = fopen(fn.c_str(),"r");
    if (fp == 0)
        return 0;
    
	
    
	fscanf(fp, "%d", &m_dwCount);
        
	
	fscanf(fp, "%f", &m_fPathTime);
    
	
    
	m_pPoints = new PATHPOINT[m_dwCount + 1];

        
	
	for(INT i = 0; i < m_dwCount; i++)
    {
        fscanf(fp, "%f", &m_pPoints[i].fTime);
        // Load position
        fscanf(fp, "%f %f %f", &m_pPoints[i].vPosition.x,
            &m_pPoints[i].vPosition.y, &m_pPoints[i].vPosition.z);
        // Load orientation
        fscanf(fp, "%f %f %f %f", &m_pPoints[i].qOrientation.x,
            &m_pPoints[i].qOrientation.y, &m_pPoints[i].qOrientation.z, &m_pPoints[i].qOrientation.w);
    }
        
	
	fclose(fp);

        
	
	// Set ending point of the path
    m_pPoints[m_dwCount].fTime = m_fPathTime;
    m_pPoints[m_dwCount].qOrientationD2 = m_pPoints[0].qOrientationD2;
    m_pPoints[m_dwCount].qOrientation = m_pPoints[0].qOrientation;
    m_pPoints[m_dwCount].vPosition = m_pPoints[0].vPosition;
    m_pPoints[m_dwCount].vPositionD2 = m_pPoints[0].vPositionD2;

        
	
	InitInterpolation();
    
	
    
	return 1;
}

void CPath::CalcOrientationD2(D3DXFROMWINEQUATERNION &qOrientationD2, D3DXFROMWINEQUATERNION &q0,
                    D3DXFROMWINEQUATERNION &q1, D3DXFROMWINEQUATERNION &q2)
{
    D3DXFROMWINEQUATERNION q, qt1, qt2, qInv;

    D3DXFROMWINEQuaternionInverse(&qInv, &q1);
    //
    if (D3DXFROMWINEQuaternionDot(&q0, &q1) < 0.0f)
        q0 *= -1.0f;
    if (D3DXFROMWINEQuaternionDot(&q2, &q1) < 0.0f)
        q2 *= -1.0f;

    D3DXFROMWINEQuaternionLn(&qt1, D3DXFROMWINEQuaternionMultiply(&q, &qInv, &q0));
    D3DXFROMWINEQuaternionLn(&qt2, D3DXFROMWINEQuaternionMultiply(&q, &qInv, &q2));
    q = -(qt1 + qt2) / 4.0f;
    D3DXFROMWINEQuaternionMultiply(&qOrientationD2, &q1, D3DXFROMWINEQuaternionExp(&qt1, &q));
}

void CPath::InitInterpolation()
{
    INT i;
    D3DXFROMWINEVECTOR3 *vP;
    D3DXFROMWINEVECTOR3 *vD2;
    D3DXFROMWINEVECTOR3 *vIV;
    D3DXFROMWINEVECTOR3 v;
    float *fT;
    float dT, f1, fP;

    // Calculate second derivatives for path interpolation
    vP = new D3DXFROMWINEVECTOR3[m_dwCount + 11];
    vD2 = new D3DXFROMWINEVECTOR3[m_dwCount + 11];
    vIV = new D3DXFROMWINEVECTOR3[m_dwCount + 11];
    fT = new float[m_dwCount + 11];

    for(i = 0; i < 5; i++)
    {
        vP[i] = m_pPoints[m_dwCount - 5 + i].vPosition;
        fT[i] = m_pPoints[m_dwCount - 5 + i].fTime - m_fPathTime;
    }
    for(i = 0; i < m_dwCount; i++)
    {
        vP[i + 5] = m_pPoints[i].vPosition;
        fT[i + 5] = m_pPoints[i].fTime;
    }
    for(i = 0; i < 5; i++)
    {
        vP[m_dwCount + 5 + i] = m_pPoints[i].vPosition;
        fT[m_dwCount + 5 + i] = m_pPoints[i].fTime + m_fPathTime;
    }
    //
    vD2[0] = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
    vIV[0] = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
    for(i = 0; i < m_dwCount + 10; i++)
    {
        dT = (fT[i] - fT[i - 1]) / (fT[i + 1] - fT[i - 1]);

        fP = vD2[i - 1].x * dT + 2.0f;
        vD2[i].x = (dT - 1.0f) / fP;
        f1 = (vP[i + 1].x - vP[i].x) / (fT[i + 1] - fT[i]) -
            (vP[i].x - vP[i - 1].x) / (fT[i] - fT[i - 1]);
        vIV[i].x = (6.0f * f1 / (fT[i + 1] - fT[i - 1]) - dT * vIV[i - 1].x) / fP;

        fP = vD2[i - 1].y * dT + 2.0f;
        vD2[i].y = (dT - 1.0f) / fP;
        f1 = (vP[i + 1].y - vP[i].y) / (fT[i + 1] - fT[i]) -
            (vP[i].y - vP[i - 1].y) / (fT[i] - fT[i - 1]);
        vIV[i].y = (6.0f * f1 / (fT[i + 1] - fT[i - 1]) - dT * vIV[i - 1].y) / fP;

        fP = vD2[i - 1].z * dT + 2.0f;
        vD2[i].z = (dT - 1.0f) / fP;
        f1 = (vP[i + 1].z - vP[i].z) / (fT[i + 1] - fT[i]) -
            (vP[i].z - vP[i - 1].z) / (fT[i] - fT[i - 1]);
        vIV[i].z = (6.0f * f1 / (fT[i + 1] - fT[i - 1]) - dT * vIV[i - 1].z) / fP;
    }
    vD2[m_dwCount + 10] = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);

    for(i = m_dwCount + 9; i >= 0; i--)
    {
        vD2[i].x = vD2[i].x * vD2[i + 1].x + vIV[i].x;
        vD2[i].y = vD2[i].y * vD2[i + 1].y + vIV[i].y;
        vD2[i].z = vD2[i].z * vD2[i + 1].z + vIV[i].z;
    }
    // Copy computed data
    for(i = 0; i < m_dwCount; i++)
    {
        m_pPoints[i].vPositionD2 = vD2[i + 5];
    }

    delete []vP;
    delete []vD2;
    delete []vIV;
    delete []fT;

    // Calculate second derivatives for orientation interpolation
    for(i = 0; i < m_dwCount; i++)
    {
        if (i == 0)
        {
            // Wrap around the path
            CalcOrientationD2(m_pPoints[i].qOrientationD2, m_pPoints[m_dwCount - 1].qOrientation,
                m_pPoints[i].qOrientation, m_pPoints[i + 1].qOrientation);
        }
        else
        {
            // Ensure shortest rotation
            if (D3DXFROMWINEQuaternionDot(&m_pPoints[i].qOrientation, &m_pPoints[i - 1].qOrientation) < 0.0f)
                m_pPoints[i].qOrientation *= -1.0f;
            CalcOrientationD2(m_pPoints[i].qOrientationD2, m_pPoints[i - 1].qOrientation,
                m_pPoints[i].qOrientation, m_pPoints[i + 1].qOrientation);
        }
    }
}

void CPath::Play(float fTime, D3DXFROMWINEVECTOR3 &vPosition, D3DXFROMWINEQUATERNION &qOrientation)
{
    fTime = fmodf(fTime, m_fPathTime);

    // Find path segment
    __DWORD__ dwLBound, dwHBound;
    dwLBound = 0;
    dwHBound = m_dwCount;
    while (dwHBound - dwLBound > 1)
    {
        __DWORD__ m = (dwHBound + dwLBound ) >> 1;
        if (m_pPoints[m].fTime > fTime)
            dwHBound = m;
        else
            dwLBound = m;
    }

    float dT = m_pPoints[dwHBound].fTime - m_pPoints[dwLBound].fTime;
    float fT = (fTime - m_pPoints[dwLBound].fTime) / dT;

    // Interpolate position
    float fTr = 1.0f - fT;
    vPosition = fTr * m_pPoints[dwLBound].vPosition +
        fT * m_pPoints[dwHBound].vPosition +
        ((fTr * fTr * fTr - fTr) * m_pPoints[dwLBound].vPositionD2 +
        (fT * fT * fT - fT) * m_pPoints[dwHBound].vPositionD2) * (dT * dT / 6.0f);

    // Interpolate orientation
    D3DXFROMWINEQuaternionSquad(&qOrientation, &m_pPoints[dwLBound].qOrientation,
        &m_pPoints[dwLBound].qOrientationD2, &m_pPoints[dwHBound].qOrientationD2,
            &m_pPoints[dwHBound].qOrientation, fT);
}

float CPath::GetPathTime()
{
    return m_fPathTime;
}

