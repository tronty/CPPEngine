//-----------------------------------------------------------------------------
// File: Viewer.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <math.h>
#include "Helper.h"
#include "Viewer.h"
#include "DXErrors.h"

CViewer::CViewer()
{
    D3DXFROMWINEVECTOR3 up = D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);
    D3DXFROMWINEVECTOR3 dir = D3DXFROMWINEVECTOR3(0.0f, 0.0f, -1.0f);
    D3DXFROMWINEVECTOR3 pos = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);

    D3DXFROMWINEMatrixIdentity(&m_matRot);
}

void CViewer::SetAspect(float aspect)
{
    m_fAspect = aspect;
    CalculateTransform();
}

void CViewer::UpdateViewer(
                           D3DXFROMWINEVECTOR3 &pos, D3DXFROMWINEQUATERNION &quat)
{
    D3DXFROMWINEVECTOR3 v;

    m_vPos = pos;

    D3DXFROMWINEMatrixRotationQuaternion(&m_matRot, &quat);
    v = D3DXFROMWINEVECTOR3(0.0f, 0.0f, -1.0f);
    D3DXFROMWINEVec3TransformCoord(&m_vDir, &v, &m_matRot);
    v = D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);
    D3DXFROMWINEVec3TransformCoord(&m_vUp, &v, &m_matRot);

    D3DXFROMWINEMATRIX m1, m2;
    D3DXFROMWINEMatrixScaling(&m1, 1.0f, 1.0f, -1.0f);
    D3DXFROMWINEMatrixRotationX(&m2, PI / 2.0f);
    D3DXFROMWINEMatrixMultiply(&m2, &m1, &m2);

    D3DXFROMWINEVec3TransformCoord(&m_vDir, &m_vDir, &m2);
    D3DXFROMWINEVec3TransformCoord(&m_vUp, &m_vUp, &m2);

    //m_vDir = Normalize(m_vDir);
    //m_vUp = Normalize(m_vUp);
    //m_vCross = Normalize(CrossProduct(m_vUp, m_vDir));

    D3DXFROMWINEVec3Normalize(&m_vDir,&m_vDir);
    D3DXFROMWINEVec3Normalize(&m_vUp,&m_vUp);
    D3DXFROMWINEVECTOR3 tmp;
    D3DXFROMWINEVec3Cross(&tmp,&m_vUp, &m_vDir);
    D3DXFROMWINEVec3Normalize(&m_vCross,&tmp);

    CalculateTransform();

    IRenderer::GetRendererInstance()->SetTransform(TS_PROJECTION, m_matProj);
    IRenderer::GetRendererInstance()->SetTransform(TS_VIEW, m_matView);
}

D3DXFROMWINEVECTOR3& CViewer::GetPos()
{
    return m_vPos;
}

void CViewer::CalculateTransform()
{
    D3DXFROMWINEMatrixPerspectiveFovLH(&m_matProj, FOV, m_fAspect, NEAR_CLIP, 1200000.0f);

    ComputeViewMatrix(&m_matView, &m_vPos, &m_vDir, &m_vUp);
}

void CViewer::ComputeClipVolume(CLIPVOLUME& cv)
{
    float dist, t;
    D3DXFROMWINEVECTOR3 p, pt[8];
    D3DXFROMWINEVECTOR3 v1, v2, n;

    for(INT i = 0; i < 8; i++)
    {
        dist = (i & 0x4) ? FAR_CLIP : NEAR_CLIP;
        pt[i].x = dist * m_vDir.x;
        pt[i].y = dist * m_vDir.y;
        pt[i].z = dist * m_vDir.z;
        t = dist * tanf(FOV);
        t = (i & 0x2) ? t : -t;
        pt[i].x += m_vUp.x * t;
        pt[i].y += m_vUp.y * t;
        pt[i].z += m_vUp.z * t;
        t = dist * tanf(FOV) / m_fAspect; // take into account screen proportions
        t = (i & 0x1) ? -t : t;
        pt[i].x += m_vCross.x * t;
        pt[i].y += m_vCross.y * t;
        pt[i].z += m_vCross.z * t;
        pt[i].x = m_vPos.x + pt[i].x;
        pt[i].y = m_vPos.y + pt[i].y;
        pt[i].z = m_vPos.z + pt[i].z;
    }
    //compute the near plane
    v1.x = pt[2].x - pt[0].x;
    v1.y = pt[2].y - pt[0].y;
    v1.z = pt[2].z - pt[0].z;
    v2.x = pt[1].x - pt[0].x;
    v2.y = pt[1].y - pt[0].y;
    v2.z = pt[1].z - pt[0].z;
    //n = D3DXFROMWINEVec3Normalize(D3DXFROMWINEVec3CrossProduct(v2, v1));
    D3DXFROMWINEVECTOR3 tmp;
    D3DXFROMWINEVec3Cross(&tmp,&v2, &v1);
    D3DXFROMWINEVec3Normalize(&n,&tmp);
    cv.pNear.a = n.x;
    cv.pNear.b = n.y;
    cv.pNear.c = n.z;
    cv.pNear.d = -(n.x * pt[0].x + n.y * pt[0].y + n.z * pt[0].z);

    //compute the far plane
    v1.x = pt[5].x - pt[4].x;
    v1.y = pt[5].y - pt[4].y;
    v1.z = pt[5].z - pt[4].z;
    v2.x = pt[6].x - pt[4].x;
    v2.y = pt[6].y - pt[4].y;
    v2.z = pt[6].z - pt[4].z;
    //n = D3DXFROMWINEVec3Normalize(D3DXFROMWINEVec3CrossProduct(v2, v1));
    D3DXFROMWINEVec3Cross(&tmp,&v2, &v1);
    D3DXFROMWINEVec3Normalize(&n,&tmp);
    cv.pFar.a = n.x;
    cv.pFar.b = n.y;
    cv.pFar.c = n.z;
    cv.pFar.d = -(n.x * pt[4].x + n.y * pt[4].y + n.z * pt[4].z);

    //compute the top plane
    v1.x = pt[6].x - pt[2].x;
    v1.y = pt[6].y - pt[2].y;
    v1.z = pt[6].z - pt[2].z;
    v2.x = pt[3].x - pt[2].x;
    v2.y = pt[3].y - pt[2].y;
    v2.z = pt[3].z - pt[2].z;
    //n = D3DXFROMWINEVec3Normalize(D3DXFROMWINEVec3CrossProduct(v2, v1));
    D3DXFROMWINEVec3Cross(&tmp,&v2, &v1);
    D3DXFROMWINEVec3Normalize(&n,&tmp);
    cv.pTop.a = n.x;
    cv.pTop.b = n.y;
    cv.pTop.c = n.z;
    cv.pTop.d = -(n.x * pt[2].x + n.y * pt[2].y + n.z * pt[2].z);

    //compute the bottom plane
    v1.x = pt[1].x - pt[0].x;
    v1.y = pt[1].y - pt[0].y;
    v1.z = pt[1].z - pt[0].z;
    v2.x = pt[4].x - pt[0].x;
    v2.y = pt[4].y - pt[0].y;
    v2.z = pt[4].z - pt[0].z;
    //n = D3DXFROMWINEVec3Normalize(D3DXFROMWINEVec3CrossProduct(v2, v1));
    D3DXFROMWINEVec3Cross(&tmp,&v2, &v1);
    D3DXFROMWINEVec3Normalize(&n,&tmp);
    cv.pBottom.a = n.x;
    cv.pBottom.b = n.y;
    cv.pBottom.c = n.z;
    cv.pBottom.d = -(n.x * pt[0].x + n.y * pt[0].y + n.z * pt[0].z);

    //compute the left plane
    v1.x = pt[3].x - pt[1].x;
    v1.y = pt[3].y - pt[1].y;
    v1.z = pt[3].z - pt[1].z;
    v2.x = pt[5].x - pt[1].x;
    v2.y = pt[5].y - pt[1].y;
    v2.z = pt[5].z - pt[1].z;
    //n = D3DXFROMWINEVec3Normalize(D3DXFROMWINEVec3CrossProduct(v2, v1));
    D3DXFROMWINEVec3Cross(&tmp,&v2, &v1);
    D3DXFROMWINEVec3Normalize(&n,&tmp);
    cv.pLeft.a = n.x;
    cv.pLeft.b = n.y;
    cv.pLeft.c = n.z;
    cv.pLeft.d = -(n.x * pt[1].x + n.y * pt[1].y + n.z * pt[1].z);

    //compute the right plane
    v1.x = pt[4].x - pt[0].x;
    v1.y = pt[4].y - pt[0].y;
    v1.z = pt[4].z - pt[0].z;
    v2.x = pt[2].x - pt[0].x;
    v2.y = pt[2].y - pt[0].y;
    v2.z = pt[2].z - pt[0].z;
    //n = D3DXFROMWINEVec3Normalize(D3DXFROMWINEVec3CrossProduct(v2, v1));
    D3DXFROMWINEVec3Cross(&tmp,&v2, &v1);
    D3DXFROMWINEVec3Normalize(&n,&tmp);
    cv.pRight.a = n.x;
    cv.pRight.b = n.y;
    cv.pRight.c = n.z;
    cv.pRight.d = -(n.x * pt[0].x + n.y * pt[0].y + n.z * pt[0].z);
}
