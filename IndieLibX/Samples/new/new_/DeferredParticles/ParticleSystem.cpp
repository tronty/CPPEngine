/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//--------------------------------------------------------------------------------------
// File: ParticleSystem.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "ParticleSystem.h"

void NewExplosion( D3DXFROMWINEVECTOR3 vCenter, float fSize );

//--------------------------------------------------------------------------------------
float RPercent()
{
    float ret = ( float )( ( rand() % 20000 ) - 10000 );
    return ret / 10000.0f;
}

//--------------------------------------------------------------------------------------
template <class T> void QuickDepthSort( T* indices, float* depths, int lo, int hi )
{
    //  lo is the lower index, hi is the upper index
    //  of the region of array a that is to be sorted
    int i = lo, j = hi;
    float h;
    T index;
    float x = depths[( lo + hi ) / 2];

    //  partition
    do
    {
        while( depths[i] < x ) i++;
        while( depths[j] > x ) j--;
        if( i <= j )
        {
            h = depths[i]; depths[i] = depths[j]; depths[j] = h;
            index = indices[i]; indices[i] = indices[j]; indices[j] = index;
            i++; j--;
        }
    } while( i <= j );

    //  recursion
    if( lo < j ) QuickDepthSort( indices, depths, lo, j );
    if( i < hi ) QuickDepthSort( indices, depths, i, hi );
}

//--------------------------------------------------------------------------------------
unsigned int        g_NumUsedParticles = 0;
PARTICLE*   g_pParticleArray = 0;
unsigned int*       g_pParticleIndices = 0;
float*      g_pParticleDepths = 0;
unsigned int        g_NumActiveParticles = 0;

//--------------------------------------------------------------------------------------
unsigned int GetNumActiveParticles()
{
    return g_NumActiveParticles;
}

//--------------------------------------------------------------------------------------
int CreateParticleArray( unsigned int MaxParticles )
{
    g_NumUsedParticles = 0;
    g_pParticleArray = new PARTICLE[ MaxParticles ];
    if( !g_pParticleArray )
        return 1;

    g_pParticleIndices = new unsigned int[ MaxParticles ];
    if( !g_pParticleIndices )
        return 1;

    g_pParticleDepths = new float[ MaxParticles ];
    if( !g_pParticleDepths )
        return 1;

    return 0;
}

//--------------------------------------------------------------------------------------
void DestroyParticleArray()
{
    g_NumUsedParticles = 0;
    SAFE_DELETE_ARRAY( g_pParticleArray );
    SAFE_DELETE_ARRAY( g_pParticleIndices );
    SAFE_DELETE_ARRAY( g_pParticleDepths );
}

//--------------------------------------------------------------------------------------
void SortParticles( D3DXFROMWINEVECTOR3 vEye )
{
    for( unsigned int i = 0; i < g_NumUsedParticles; i++ )
    {
        g_pParticleIndices[i] = i;
        D3DXFROMWINEVECTOR3 vDelta = vEye - g_pParticleArray[i].vPos;
        g_pParticleDepths[i] = D3DXFROMWINEVec3LengthSq( &vDelta );
    }

    QuickDepthSort( g_pParticleIndices, g_pParticleDepths, 0, g_NumUsedParticles - 1 );
}

//--------------------------------------------------------------------------------------
D3DXFROMWINEVECTOR2 g_sQuad[4] =
{
    D3DXFROMWINEVECTOR2( -1, -1 ),
    D3DXFROMWINEVECTOR2( 1, -1 ),
    D3DXFROMWINEVECTOR2( 1, 1 ),
    D3DXFROMWINEVECTOR2( -1, 1 )
};

//--------------------------------------------------------------------------------------
void CopyParticlesToVertexBuffer( PARTICLE_VERTEX* pVB, D3DXFROMWINEVECTOR3 vEye, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp )
{
    SortParticles( vEye );

    g_NumActiveParticles = 0;
    unsigned int iVBIndex = 0;
    for( int i = g_NumUsedParticles - 1; i >= 0; i-- )
    {
        unsigned int index = g_pParticleIndices[i];

        D3DXFROMWINEVECTOR3 vPos = g_pParticleArray[index].vPos;
        float fRadius = g_pParticleArray[index].fRadius;
        float fRot = g_pParticleArray[index].fRot;
        float fFade = g_pParticleArray[index].fFade;
        __DWORD__ vColor = g_pParticleArray[index].Color;

        if( !g_pParticleArray[index].bVisible )
            continue;

        // rotate
        float fSinTheta = sinf( fRot );
        float fCosTheta = cosf( fRot );

        D3DXFROMWINEVECTOR2 New[4];
        for( int v = 0; v < 4; v++ )
        {
            New[v].x = fCosTheta * g_sQuad[v].x - fSinTheta * g_sQuad[v].y;
            New[v].y = fSinTheta * g_sQuad[v].x + fCosTheta * g_sQuad[v].y;

            New[v].x *= fRadius;
            New[v].y *= fRadius;
        }

        // Tri 0 (0,1,3)
        pVB[ iVBIndex + 2 ].vPos = vPos + vRight * New[0].x + vUp * New[0].y;
        pVB[ iVBIndex + 2 ].vUV = D3DXFROMWINEVECTOR2( 0, 1 );
        pVB[ iVBIndex + 2 ].fLife = fFade;
        pVB[ iVBIndex + 2 ].fRot = fRot;
        pVB[ iVBIndex + 2 ].Color = vColor;
        pVB[ iVBIndex + 1 ].vPos = vPos + vRight * New[1].x + vUp * New[1].y;
        pVB[ iVBIndex + 1 ].vUV = D3DXFROMWINEVECTOR2( 1, 1 );
        pVB[ iVBIndex + 1 ].fLife = fFade;
        pVB[ iVBIndex + 1 ].fRot = fRot;
        pVB[ iVBIndex + 1 ].Color = vColor;
        pVB[ iVBIndex + 0 ].vPos = vPos + vRight * New[3].x + vUp * New[3].y;
        pVB[ iVBIndex + 0 ].vUV = D3DXFROMWINEVECTOR2( 0, 0 );
        pVB[ iVBIndex + 0 ].fLife = fFade;
        pVB[ iVBIndex + 0 ].fRot = fRot;
        pVB[ iVBIndex + 0 ].Color = vColor;

        // Tri 1 (3,1,2)
        pVB[ iVBIndex + 5 ].vPos = vPos + vRight * New[3].x + vUp * New[3].y;
        pVB[ iVBIndex + 5 ].vUV = D3DXFROMWINEVECTOR2( 0, 0 );
        pVB[ iVBIndex + 5 ].fLife = fFade;
        pVB[ iVBIndex + 5 ].fRot = fRot;
        pVB[ iVBIndex + 5 ].Color = vColor;
        pVB[ iVBIndex + 4 ].vPos = vPos + vRight * New[1].x + vUp * New[1].y;
        pVB[ iVBIndex + 4 ].vUV = D3DXFROMWINEVECTOR2( 1, 1 );
        pVB[ iVBIndex + 4 ].fLife = fFade;
        pVB[ iVBIndex + 4 ].fRot = fRot;
        pVB[ iVBIndex + 4 ].Color = vColor;
        pVB[ iVBIndex + 3 ].vPos = vPos + vRight * New[2].x + vUp * New[2].y;
        pVB[ iVBIndex + 3 ].vUV = D3DXFROMWINEVECTOR2( 1, 0 );
        pVB[ iVBIndex + 3 ].fLife = fFade;
        pVB[ iVBIndex + 3 ].fRot = fRot;
        pVB[ iVBIndex + 3 ].Color = vColor;

        iVBIndex += 6;

        g_NumActiveParticles ++;
    }
}

//-----------------------------------------------------------------------
CParticleSystem::CParticleSystem()
{
    m_NumParticles = 0;
    m_pParticles = 0;

    m_fSpread = 0.0f;
    m_fLifeSpan = 0.0f;
    m_fStartSize = 0.0f;
    m_fEndSize = 0.0f;
    m_fSizeExponent = 0.0f;
    m_fStartSpeed = 0.0f;
    m_fEndSpeed = 0.0f;
    m_fSpeedExponent = 0.0f;
    m_fFadeExponent = 0.0f;
    m_fRollAmount = 0.0f;
    m_fWindFalloff = 0.0f;
    m_vPosMul = D3DXFROMWINEVECTOR3( 0, 0, 0 );
    m_vDirMul = D3DXFROMWINEVECTOR3( 0, 0, 0 );

    m_fCurrentTime = 0.0f;
    m_vCenter = D3DXFROMWINEVECTOR3( 0, 0, 0 );
    m_fStartTime = 0.0f;

    m_vFlashColor = D3DXFROMWINEVECTOR4( 0, 0, 0, 0 );

    m_PST = PST_DEFAULT;
}

//--------------------------------------------------------------------------------------
CParticleSystem::~CParticleSystem()
{
}

//--------------------------------------------------------------------------------------
int CParticleSystem::CreateParticleSystem( unsigned int NumParticles )
{
    m_NumParticles = NumParticles;
    m_pParticles = &g_pParticleArray[ g_NumUsedParticles ];
    g_NumUsedParticles += m_NumParticles;

    return 0;
}

//--------------------------------------------------------------------------------------
void CParticleSystem::SetSystemAttributes( D3DXFROMWINEVECTOR3 vCenter,
                                           float fSpread, float fLifeSpan, float fFadeExponent,
                                           float fStartSize, float fEndSize, float fSizeExponent,
                                           float fStartSpeed, float fEndSpeed, float fSpeedExponent,
                                           float fRollAmount, float fWindFalloff,
                                           unsigned int NumStreamers, float fSpeedVariance,
                                           D3DXFROMWINEVECTOR3 vDirection, D3DXFROMWINEVECTOR3 vDirVariance,
                                           D3DXFROMWINEVECTOR4 vColor0, D3DXFROMWINEVECTOR4 vColor1,
                                           D3DXFROMWINEVECTOR3 vPosMul, D3DXFROMWINEVECTOR3 vDirMul )
{
    m_vCenter = vCenter;
    m_fSpread = fSpread;
    m_fLifeSpan = fLifeSpan;
    m_fStartSize = fStartSize;
    m_fEndSize = fEndSize;
    m_fSizeExponent = fSizeExponent;
    m_fStartSpeed = fStartSpeed;
    m_fEndSpeed = fEndSpeed;
    m_fSpeedExponent = fSpeedExponent;
    m_fFadeExponent = fFadeExponent;

    m_fRollAmount = fRollAmount;
    m_fWindFalloff = fWindFalloff;
    m_vPosMul = vPosMul;
    m_vDirMul = vDirMul;

    m_NumStreamers = NumStreamers;
    m_fSpeedVariance = fSpeedVariance;
    m_vDirection = vDirection;
    m_vDirVariance = vDirVariance;

    m_vColor0 = vColor0;
    m_vColor1 = vColor1;

    Init();
}

//--------------------------------------------------------------------------------------
void CParticleSystem::SetCenter( D3DXFROMWINEVECTOR3 vCenter )
{
    m_vCenter = vCenter;
}

//--------------------------------------------------------------------------------------
void CParticleSystem::SetStartTime( float fStartTime )
{
    m_fStartTime = fStartTime;
}

//--------------------------------------------------------------------------------------
void CParticleSystem::SetStartSpeed( float fStartSpeed )
{
    m_fStartSpeed = fStartSpeed;
}

//--------------------------------------------------------------------------------------
void CParticleSystem::SetFlashColor( D3DXFROMWINEVECTOR4 vFlashColor )
{
    m_vFlashColor = vFlashColor;
}

//--------------------------------------------------------------------------------------
D3DXFROMWINEVECTOR4 CParticleSystem::GetFlashColor()
{
    return m_vFlashColor;
}

//--------------------------------------------------------------------------------------
float CParticleSystem::GetCurrentTime()
{
    return m_fCurrentTime;
}

//--------------------------------------------------------------------------------------
float CParticleSystem::GetLifeSpan()
{
    return m_fLifeSpan;
}

//--------------------------------------------------------------------------------------
unsigned int CParticleSystem::GetNumParticles()
{
    return m_NumParticles;
}

//--------------------------------------------------------------------------------------
D3DXFROMWINEVECTOR3 CParticleSystem::GetCenter()
{
    return m_vCenter;
}

//--------------------------------------------------------------------------------------
void CParticleSystem::Init()
{
    for( unsigned int i = 0; i < m_NumParticles; i++ )
    {
        m_pParticles[i].vPos.x = RPercent() * m_fSpread;
        m_pParticles[i].vPos.y = RPercent() * m_fSpread;
        m_pParticles[i].vPos.z = RPercent() * m_fSpread;
        m_pParticles[i].vPos.x *= m_vPosMul.x;
        m_pParticles[i].vPos.y *= m_vPosMul.y;
        m_pParticles[i].vPos.z *= m_vPosMul.z;
        m_pParticles[i].vPos += m_vCenter;

        m_pParticles[i].vDir.x = RPercent();
        m_pParticles[i].vDir.y = fabs( RPercent() );
        m_pParticles[i].vDir.z = RPercent();
        m_pParticles[i].vDir.x *= m_vDirMul.x;
        m_pParticles[i].vDir.y *= m_vDirMul.y;
        m_pParticles[i].vDir.z *= m_vDirMul.z;

        D3DXFROMWINEVec3Normalize( &m_pParticles[i].vDir, &m_pParticles[i].vDir );
        m_pParticles[i].fRadius = m_fStartSize;
        m_pParticles[i].fLife = m_fStartTime;
        m_pParticles[i].fFade = 0.0f;

        m_pParticles[i].fRot = RPercent() * 3.14159f * 2.0f;

        float fLerp = RPercent();
        D3DXFROMWINEVECTOR4 vColor = m_vColor0 * fLerp + m_vColor1 * ( 1.0f - fLerp );
        m_pParticles[i].Color = ( __DWORD__ )( vColor.w * 255.0f ) << 24;
        m_pParticles[i].Color |= ( ( __DWORD__ )( vColor.z * 255.0f ) & 255 ) << 16;
        m_pParticles[i].Color |= ( ( __DWORD__ )( vColor.y * 255.0f ) & 255 ) << 8;
        m_pParticles[i].Color |= ( ( __DWORD__ )( vColor.x * 255.0f ) & 255 );
    }

    m_bStarted = false;
    m_fCurrentTime = m_fStartTime;
}

//--------------------------------------------------------------------------------------
void CParticleSystem::AdvanceSystem( float fTime, float fElapsedTime, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp,
                                     D3DXFROMWINEVECTOR3 vWindVel, D3DXFROMWINEVECTOR3 vGravity )
{
    if( m_fCurrentTime > 0 )
    {
        for( unsigned int i = 0; i < m_NumParticles; i++ )
        {
            float t = m_pParticles[i].fLife / m_fLifeSpan;
            float tm1 = t - 1.0f;
            float fSizeLerp = 1.0f - pow( tm1, m_fSizeExponent );
            float fSpeedLerp = 1.0f - pow( tm1, m_fSpeedExponent );
            float fFadeLerp = 1.0f - pow( tm1, m_fFadeExponent );

            float fSize = fSizeLerp * m_fEndSize + ( 1.0f - fSizeLerp ) * m_fStartSize;
            float fSpeed = fSpeedLerp * m_fEndSpeed + ( 1.0f - fSpeedLerp ) * m_fStartSpeed;
            float fFade = fFadeLerp;

            D3DXFROMWINEVECTOR3 vVel = m_pParticles[i].vDir * fSpeed;
            float fRot = 0.0f;
            float fWindAmt = 1.0f;

            vVel += vWindVel * fWindAmt;

            m_pParticles[i].vPos += fElapsedTime * vVel;

            m_pParticles[i].fRadius = fSize;

            m_pParticles[i].fLife += fElapsedTime;
            m_pParticles[i].fFade = fFade;

            m_pParticles[i].fRot += fRot;

            m_pParticles[i].bVisible = true;
        }
        if( !m_bStarted )
        {
            m_bStarted = true;
        }
    }
    else
    {
        for( unsigned int i = 0; i < m_NumParticles; i++ )
        {
            m_pParticles[i].bVisible = false;
            m_pParticles[i].fLife += fElapsedTime;
        }
    }

    m_fCurrentTime += fElapsedTime;
}

//-----------------------------------------------------------------------
CMushroomParticleSystem::CMushroomParticleSystem() : CParticleSystem()
{
    m_PST = PST_MUSHROOM;
}

//--------------------------------------------------------------------------------------
CMushroomParticleSystem::~CMushroomParticleSystem()
{
}

//--------------------------------------------------------------------------------------
void CMushroomParticleSystem::Init()
{
    for( unsigned int i = 0; i < m_NumParticles; i++ )
    {
        m_pParticles[i].vPos.x = RPercent() * m_fSpread;
        m_pParticles[i].vPos.y = RPercent() * m_fSpread;
        m_pParticles[i].vPos.z = RPercent() * m_fSpread;
        m_pParticles[i].vPos.x *= m_vPosMul.x;
        m_pParticles[i].vPos.y *= m_vPosMul.y;
        m_pParticles[i].vPos.z *= m_vPosMul.z;
        m_pParticles[i].vPos += m_vCenter;

        m_pParticles[i].vDir.x = RPercent();
        m_pParticles[i].vDir.y = fabs( RPercent() );
        m_pParticles[i].vDir.z = RPercent();
        m_pParticles[i].vDir.x *= m_vDirMul.x;
        m_pParticles[i].vDir.y *= m_vDirMul.y;
        m_pParticles[i].vDir.z *= m_vDirMul.z;

        D3DXFROMWINEVec3Normalize( &m_pParticles[i].vDir, &m_pParticles[i].vDir );
        m_pParticles[i].fRadius = m_fStartSize;
        m_pParticles[i].fLife = m_fStartTime;
        m_pParticles[i].fFade = 0.0f;

        m_pParticles[i].fRot = RPercent() * 3.14159f * 2.0f;

        float fLerp = RPercent();
        D3DXFROMWINEVECTOR4 vColor = m_vColor0 * fLerp + m_vColor1 * ( 1.0f - fLerp );
        m_pParticles[i].Color = ( __DWORD__ )( vColor.w * 255.0f ) << 24;
        m_pParticles[i].Color |= ( ( __DWORD__ )( vColor.z * 255.0f ) & 255 ) << 16;
        m_pParticles[i].Color |= ( ( __DWORD__ )( vColor.y * 255.0f ) & 255 ) << 8;
        m_pParticles[i].Color |= ( ( __DWORD__ )( vColor.x * 255.0f ) & 255 );
    }

    m_bStarted = false;
    m_fCurrentTime = m_fStartTime;
}

//--------------------------------------------------------------------------------------
void CMushroomParticleSystem::AdvanceSystem( float fTime, float fElapsedTime, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp,
                                             D3DXFROMWINEVECTOR3 vWindVel, D3DXFROMWINEVECTOR3 vGravity )
{
    if( m_fCurrentTime > 0 )
    {
        for( unsigned int i = 0; i < m_NumParticles; i++ )
        {
            float t = m_pParticles[i].fLife / m_fLifeSpan;
            float tm1 = t - 1.0f;
            float fSizeLerp = 1.0f - pow( tm1, m_fSizeExponent );
            float fSpeedLerp = 1.0f - pow( tm1, m_fSpeedExponent );
            float fFadeLerp = 1.0f - pow( tm1, m_fFadeExponent );

            float fSize = fSizeLerp * m_fEndSize + ( 1.0f - fSizeLerp ) * m_fStartSize;
            float fSpeed = fSpeedLerp * m_fEndSpeed + ( 1.0f - fSpeedLerp ) * m_fStartSpeed;
            float fFade = fFadeLerp;

            D3DXFROMWINEVECTOR3 vDelta = m_pParticles[i].vPos - m_vCenter;
            float rightDist = D3DXFROMWINEVec3Dot( &vDelta, &vRight );

            D3DXFROMWINEVECTOR3 vVel = m_pParticles[i].vDir * fSpeed;
            float fRot = 0.0f;
            float fWindAmt = 1.0f;

            // Higher level should roll outward
            fRot = -rightDist * m_fRollAmount * fElapsedTime * ( 1.0f - t );

            vVel += vWindVel * fWindAmt;

            m_pParticles[i].vPos += fElapsedTime * vVel;

            m_pParticles[i].fRadius = fSize;

            m_pParticles[i].fLife += fElapsedTime;
            m_pParticles[i].fFade = fFade;

            m_pParticles[i].fRot += fRot;

            m_pParticles[i].bVisible = true;
        }

        if( !m_bStarted )
        {
            D3DXFROMWINEVECTOR3 vCenter = m_vCenter;
            vCenter.y = -2.0f;
            float fSize = 6.0f;
            NewExplosion( vCenter, fSize );

            m_bStarted = true;
        }
    }
    else
    {
        for( unsigned int i = 0; i < m_NumParticles; i++ )
        {
            m_pParticles[i].bVisible = false;
            m_pParticles[i].fLife += fElapsedTime;
        }
    }

    m_fCurrentTime += fElapsedTime;
}

//-----------------------------------------------------------------------
CStalkParticleSystem::CStalkParticleSystem() : CParticleSystem()
{
    m_PST = PST_STALK;
}

//--------------------------------------------------------------------------------------
CStalkParticleSystem::~CStalkParticleSystem()
{
}

//--------------------------------------------------------------------------------------
void CStalkParticleSystem::Init()
{
    for( unsigned int i = 0; i < m_NumParticles; i++ )
    {
        m_pParticles[i].vPos.x = RPercent() * m_fSpread;
        m_pParticles[i].vPos.y = RPercent() * m_fSpread;
        m_pParticles[i].vPos.z = RPercent() * m_fSpread;
        m_pParticles[i].vPos.x *= m_vPosMul.x;
        m_pParticles[i].vPos.y *= m_vPosMul.y;
        m_pParticles[i].vPos.z *= m_vPosMul.z;
        m_pParticles[i].vPos += m_vCenter;

        m_pParticles[i].vDir.x = RPercent();
        m_pParticles[i].vDir.y = fabs( RPercent() );
        m_pParticles[i].vDir.z = RPercent();
        m_pParticles[i].vDir.x *= m_vDirMul.x;
        m_pParticles[i].vDir.y *= m_vDirMul.y;
        m_pParticles[i].vDir.z *= m_vDirMul.z;

        D3DXFROMWINEVec3Normalize( &m_pParticles[i].vDir, &m_pParticles[i].vDir );
        m_pParticles[i].fRadius = m_fStartSize;
        m_pParticles[i].fLife = m_fStartTime;
        m_pParticles[i].fFade = 0.0f;

        m_pParticles[i].fRot = RPercent() * 3.14159f * 2.0f;

        float fLerp = RPercent();
        D3DXFROMWINEVECTOR4 vColor = m_vColor0 * fLerp + m_vColor1 * ( 1.0f - fLerp );
        m_pParticles[i].Color = ( __DWORD__ )( vColor.w * 255.0f ) << 24;
        m_pParticles[i].Color |= ( ( __DWORD__ )( vColor.z * 255.0f ) & 255 ) << 16;
        m_pParticles[i].Color |= ( ( __DWORD__ )( vColor.y * 255.0f ) & 255 ) << 8;
        m_pParticles[i].Color |= ( ( __DWORD__ )( vColor.x * 255.0f ) & 255 );
    }

    m_bStarted = false;
    m_fCurrentTime = m_fStartTime;
}

//--------------------------------------------------------------------------------------
void CStalkParticleSystem::AdvanceSystem( float fTime, float fElapsedTime, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp,
                                          D3DXFROMWINEVECTOR3 vWindVel, D3DXFROMWINEVECTOR3 vGravity )
{
    if( m_fCurrentTime > 0 )
    {
        for( unsigned int i = 0; i < m_NumParticles; i++ )
        {
            float t = m_pParticles[i].fLife / m_fLifeSpan;
            float tm1 = t - 1.0f;
            float fSizeLerp = 1.0f - pow( tm1, m_fSizeExponent );
            float fSpeedLerp = 1.0f - pow( tm1, m_fSpeedExponent );
            float fFadeLerp = 1.0f - pow( tm1, m_fFadeExponent );

            float fSize = fSizeLerp * m_fEndSize + ( 1.0f - fSizeLerp ) * m_fStartSize;
            float fSpeed = fSpeedLerp * m_fEndSpeed + ( 1.0f - fSpeedLerp ) * m_fStartSpeed;
            float fFade = fFadeLerp;

            D3DXFROMWINEVECTOR3 vDelta = m_pParticles[i].vPos - m_vCenter;
            float rightDist = D3DXFROMWINEVec3Dot( &vDelta, &vRight );

            D3DXFROMWINEVECTOR3 vVel = m_pParticles[i].vDir * fSpeed;
            float fRot = 0.0f;
            float fWindAmt = 1.0f;

            D3DXFROMWINEVECTOR3 vDeltaXZ = vDelta;
            vDeltaXZ.y = 0.0f;

            float fLenSq = D3DXFROMWINEVec3LengthSq( &vDeltaXZ ) / ( m_fSpread );
            float fExp = min( vWindVel.y, ( 1.0f / fLenSq ) );//exp( -fLenSq );
            vVel.x -= 0.1f * vDeltaXZ.x;
            vVel.z -= 0.1f * vDeltaXZ.z;
            vVel.y = fExp;

            fWindAmt = max( 0.0f, min( 1.0f, vDelta.y / m_fWindFalloff ) );

            // Lower level should roll inward
            fRot = rightDist * m_fRollAmount * fElapsedTime * ( 1.0f - t );

            vVel += vWindVel * fWindAmt;

            m_pParticles[i].vPos += fElapsedTime * vVel;

            m_pParticles[i].fRadius = fSize;

            m_pParticles[i].fLife += fElapsedTime;
            m_pParticles[i].fFade = fFade;

            m_pParticles[i].fRot += fRot;

            m_pParticles[i].bVisible = true;
        }

        if( !m_bStarted )
        {
            m_bStarted = true;
        }
    }
    else
    {
        for( unsigned int i = 0; i < m_NumParticles; i++ )
        {
            m_pParticles[i].bVisible = false;
            m_pParticles[i].fLife += fElapsedTime;
        }
    }

    m_fCurrentTime += fElapsedTime;
}

//-----------------------------------------------------------------------
CGroundBurstParticleSystem::CGroundBurstParticleSystem() : CParticleSystem()
{
    m_PST = PST_GROUNDEXP;
}

//--------------------------------------------------------------------------------------
CGroundBurstParticleSystem::~CGroundBurstParticleSystem()
{
}

//--------------------------------------------------------------------------------------
void CGroundBurstParticleSystem::Init()
{
    unsigned int index = 0;
    unsigned int ParticlesPerStream = ( m_NumParticles / m_NumStreamers ) + 1;
    for( unsigned int s = 0; s < m_NumStreamers; s++ )
    {
        D3DXFROMWINEVECTOR3 vStreamerDir = m_vDirection;
        vStreamerDir.x += RPercent() * m_vDirVariance.x;
        vStreamerDir.y += RPercent() * m_vDirVariance.y;
        vStreamerDir.z += RPercent() * m_vDirVariance.z;
        D3DXFROMWINEVec3Normalize( &vStreamerDir, &vStreamerDir );

        D3DXFROMWINEVECTOR3 vStreamerPos;
        vStreamerPos.x = RPercent() * m_fSpread;
        vStreamerPos.y = RPercent() * m_fSpread;
        vStreamerPos.z = RPercent() * m_fSpread;

        for( unsigned int i = 0; i < ParticlesPerStream; i++ )
        {
            if( index < m_NumParticles )
            {
                m_pParticles[index].vPos = vStreamerPos;
                m_pParticles[index].vPos.x *= m_vPosMul.x;
                m_pParticles[index].vPos.y *= m_vPosMul.y;
                m_pParticles[index].vPos.z *= m_vPosMul.z;
                m_pParticles[index].vPos += m_vCenter;

                float fSpeed = m_fStartSpeed + RPercent() * m_fSpeedVariance;

                m_pParticles[index].vDir = vStreamerDir * fSpeed;
                m_pParticles[index].vDir.x *= m_vDirMul.x;
                m_pParticles[index].vDir.y *= m_vDirMul.y;
                m_pParticles[index].vDir.z *= m_vDirMul.z;

                float fRadiusLerp = ( fSpeed / ( m_fStartSpeed + m_fSpeedVariance ) );

                m_pParticles[index].fRadius = m_fStartSize * fRadiusLerp + m_fEndSize * ( 1 - fRadiusLerp );
                m_pParticles[index].fLife = m_fStartTime;
                m_pParticles[index].fFade = 0.0f;

                m_pParticles[index].fRot = RPercent() * 3.14159f * 2.0f;

                m_pParticles[index].fRotRate = RPercent() * 1.5f;

                float fLerp = RPercent();
                D3DXFROMWINEVECTOR4 vColor = ( m_vColor0 * fLerp ) + ( m_vColor1 * ( 1.0f - fLerp ) );
                m_pParticles[index].Color = ( __DWORD__ )( vColor.w * 255.0f ) << 24;
                m_pParticles[index].Color |= ( ( __DWORD__ )( vColor.z * 255.0f ) & 255 ) << 16;
                m_pParticles[index].Color |= ( ( __DWORD__ )( vColor.y * 255.0f ) & 255 ) << 8;
                m_pParticles[index].Color |= ( ( __DWORD__ )( vColor.x * 255.0f ) & 255 );

                index++;
            }
        }
    }

    m_bStarted = false;

    m_fCurrentTime = m_fStartTime;
}

//--------------------------------------------------------------------------------------
void CGroundBurstParticleSystem::AdvanceSystem( float fTime, float fElapsedTime, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp,
                                                D3DXFROMWINEVECTOR3 vWindVel, D3DXFROMWINEVECTOR3 vGravity )
{
    if( m_fCurrentTime > 0 )
    {
        for( unsigned int i = 0; i < m_NumParticles; i++ )
        {
            float t = m_pParticles[i].fLife / m_fLifeSpan;
            float tm1 = t - 1.0f;
            float fSizeLerp = 1.0f - pow( tm1, m_fSizeExponent );
            float fSpeedLerp = pow( tm1, m_fSpeedExponent );
            float fFadeLerp = 1.0f - pow( tm1, m_fFadeExponent );
            float fFade = fFadeLerp;

            D3DXFROMWINEVECTOR3 vDelta = m_pParticles[i].vPos - m_vCenter;

            float fRot = m_pParticles[i].fRotRate * fElapsedTime;
            float fWindAmt = 1.0f;

            fWindAmt = max( 0.0f, min( 1.0f, vDelta.y / m_fWindFalloff ) );
            D3DXFROMWINEVECTOR3 vWind = vWindVel * fWindAmt;
            vWind.y = 0;

            m_pParticles[i].vPos += fElapsedTime * ( m_pParticles[i].vDir + vWind );
            if( m_pParticles[i].vPos.y < 0 )
                m_pParticles[i].vPos.y = 0;
            m_pParticles[i].vDir += vGravity * fElapsedTime;

            float fDrag = 8.0f * fSpeedLerp;
            m_pParticles[i].vDir *= 1.0f - fDrag * fElapsedTime;

            m_pParticles[i].fRadius += fSizeLerp * fElapsedTime;

            m_pParticles[i].fLife += fElapsedTime;
            m_pParticles[i].fFade = fFade;

            m_pParticles[i].fRot += fRot;

            m_pParticles[i].bVisible = true;
        }

        if( !m_bStarted )
        {
            D3DXFROMWINEVECTOR3 vCenter = m_vCenter;
            vCenter.y = -2.0f;
            float fSize = 5.0f;
            NewExplosion( vCenter, fSize );

            m_bStarted = true;
        }
    }
    else
    {
        for( unsigned int i = 0; i < m_NumParticles; i++ )
        {
            m_pParticles[i].bVisible = false;
            m_pParticles[i].fLife += fElapsedTime;
        }
    }

    m_fCurrentTime += fElapsedTime;
}


//-----------------------------------------------------------------------
CLandMineParticleSystem::CLandMineParticleSystem() : CParticleSystem()
{
    m_PST = PST_LANDMIND;
}

//--------------------------------------------------------------------------------------
CLandMineParticleSystem::~CLandMineParticleSystem()
{
}

//--------------------------------------------------------------------------------------
void CLandMineParticleSystem::Init()
{	
    for( unsigned int i = 0; i < m_NumParticles; i++ )
    {
        D3DXFROMWINEVECTOR3 vDir = m_vDirection;
        vDir.x += RPercent() * m_vDirVariance.x;
        vDir.y += RPercent() * m_vDirVariance.y;
        vDir.z += RPercent() * m_vDirVariance.z;
        D3DXFROMWINEVec3Normalize( &vDir, &vDir );

        m_pParticles[i].vPos.x = RPercent() * m_fSpread;
        m_pParticles[i].vPos.y = RPercent() * m_fSpread;
        m_pParticles[i].vPos.z = RPercent() * m_fSpread;
        m_pParticles[i].vPos.x *= m_vPosMul.x;
        m_pParticles[i].vPos.y *= m_vPosMul.y;
        m_pParticles[i].vPos.z *= m_vPosMul.z;
        float fDist = D3DXFROMWINEVec3Length( &m_pParticles[i].vPos );
        fDist /= m_fSpread;
        m_pParticles[i].vPos += m_vCenter;

        float fSpeed = m_fStartSpeed + RPercent() * m_fSpeedVariance;

        float speedMod = 1.0f - fDist;

        m_pParticles[i].vDir = vDir * fSpeed * speedMod;
        m_pParticles[i].vDir.x *= m_vDirMul.x;
        m_pParticles[i].vDir.y *= m_vDirMul.y;
        m_pParticles[i].vDir.z *= m_vDirMul.z;

        float fRadiusLerp = ( fSpeed / ( m_fStartSpeed + m_fSpeedVariance ) );

        m_pParticles[i].fRadius = m_fStartSize * fRadiusLerp + m_fEndSize * ( 1 - fRadiusLerp );
        m_pParticles[i].fLife = m_fStartTime;
        m_pParticles[i].fFade = 0.0f;

        m_pParticles[i].fRot = RPercent() * 3.14159f * 2.0f;

        m_pParticles[i].fRotRate = RPercent() * 1.5f;

        float fLerp = RPercent();
        D3DXFROMWINEVECTOR4 vColor = ( m_vColor0 * fLerp ) + ( m_vColor1 * ( 1.0f - fLerp ) );
        m_pParticles[i].Color = ( __DWORD__ )( vColor.w * 255.0f ) << 24;
        m_pParticles[i].Color |= ( ( __DWORD__ )( vColor.z * 255.0f ) & 255 ) << 16;
        m_pParticles[i].Color |= ( ( __DWORD__ )( vColor.y * 255.0f ) & 255 ) << 8;
        m_pParticles[i].Color |= ( ( __DWORD__ )( vColor.x * 255.0f ) & 255 );
    }

    m_bStarted = false;
    m_fCurrentTime = m_fStartTime;
}

//--------------------------------------------------------------------------------------
void CLandMineParticleSystem::AdvanceSystem( float fTime, float fElapsedTime, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp,
                                             D3DXFROMWINEVECTOR3 vWindVel, D3DXFROMWINEVECTOR3 vGravity )
{
    if( m_fCurrentTime > 0 )
    {
        for( unsigned int i = 0; i < m_NumParticles; i++ )
        {
            float t = m_pParticles[i].fLife / m_fLifeSpan;
            float tm1 = t - 1.0f;
            float fSizeLerp = 1.0f - pow( tm1, m_fSizeExponent );
            float fSpeedLerp = pow( tm1, m_fSpeedExponent );
            float fFadeLerp = 1.0f - pow( tm1, m_fFadeExponent );

            float fFade = fFadeLerp;

            D3DXFROMWINEVECTOR3 vDelta = m_pParticles[i].vPos - m_vCenter;

            float fRot = m_pParticles[i].fRotRate * fElapsedTime;
            float fWindAmt = 1.0f;

            D3DXFROMWINEVECTOR3 vWind = vWindVel;
            vWind.y = 0;

            m_pParticles[i].vPos += fElapsedTime * ( m_pParticles[i].vDir + vWind );
            if( m_pParticles[i].vPos.y < 0 )
                m_pParticles[i].vPos.y = 0;
            m_pParticles[i].vDir += vGravity * ( 1 - fWindAmt ) * fElapsedTime;

            float fDrag = 8.0f * fSpeedLerp;
            m_pParticles[i].vDir *= 1.0f - fDrag * fElapsedTime;

            m_pParticles[i].fRadius += fSizeLerp * fElapsedTime * fWindAmt;

            m_pParticles[i].fLife += fElapsedTime;
            m_pParticles[i].fFade = fFade;

            m_pParticles[i].fRot += fRot;

            m_pParticles[i].bVisible = true;
        }

        if( !m_bStarted )
        {
            D3DXFROMWINEVECTOR3 vCenter = m_vCenter;
            vCenter.y = -2.0f;
            float fSize = 3.0f;
            NewExplosion( vCenter, fSize );

            m_bStarted = true;
        }
    }
    else
    {
        for( unsigned int i = 0; i < m_NumParticles; i++ )
        {
            m_pParticles[i].bVisible = false;
            m_pParticles[i].fLife += fElapsedTime;
        }
    }

    m_fCurrentTime += fElapsedTime;
}

