/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//--------------------------------------------------------------------------------------
// File: ParticleSystem.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

struct PARTICLE_VERTEX
{
    D3DXFROMWINEVECTOR3 vPos;
    D3DXFROMWINEVECTOR2 vUV;
    float fLife;
    float fRot;
    __DWORD__ Color;
};

struct PARTICLE
{
    D3DXFROMWINEVECTOR3 vPos;
    D3DXFROMWINEVECTOR3 vDir;
    D3DXFROMWINEVECTOR3 vMass;
    __DWORD__ Color;
    float fRadius;
    float fLife;
    float fFade;
    float fRot;
    float fRotRate;
    bool bVisible;
};

enum PARTICLE_SYSTEM_TYPE
{
    PST_DEFAULT = 0,
    PST_MUSHROOM,
    PST_STALK,
    PST_GROUNDEXP,
    PST_LANDMIND,
};

int CreateParticleArray( unsigned int MaxParticles );
void DestroyParticleArray();
void CopyParticlesToVertexBuffer( PARTICLE_VERTEX* pVB, D3DXFROMWINEVECTOR3 vEye, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp );
float RPercent();
void InitParticleArray( unsigned int NumParticles );
unsigned int GetNumActiveParticles();

//-----------------------------------------------------------------------
class CParticleSystem
{
protected:
    unsigned int m_NumParticles;
    PARTICLE* m_pParticles;

    float m_fSpread;
    float m_fLifeSpan;
    float m_fStartSize;
    float m_fEndSize;
    float m_fSizeExponent;
    float m_fStartSpeed;
    float m_fEndSpeed;
    float m_fSpeedExponent;
    float m_fFadeExponent;
    float m_fRollAmount;
    float m_fWindFalloff;

    unsigned int m_NumStreamers;
    float m_fSpeedVariance;
    D3DXFROMWINEVECTOR3 m_vDirection;
    D3DXFROMWINEVECTOR3 m_vDirVariance;

    D3DXFROMWINEVECTOR4 m_vColor0;
    D3DXFROMWINEVECTOR4 m_vColor1;

    D3DXFROMWINEVECTOR3 m_vPosMul;
    D3DXFROMWINEVECTOR3 m_vDirMul;

    float m_fCurrentTime;
    D3DXFROMWINEVECTOR3 m_vCenter;
    float m_fStartTime;

    D3DXFROMWINEVECTOR4 m_vFlashColor;

    bool m_bStarted;

    PARTICLE_SYSTEM_TYPE m_PST;

public:
                    CParticleSystem();
    virtual         ~CParticleSystem();

    int         CreateParticleSystem( unsigned int NumParticles );
    void            SetSystemAttributes( D3DXFROMWINEVECTOR3 vCenter,
                                         float fSpread, float fLifeSpan, float fFadeExponent,
                                         float fStartSize, float fEndSize, float fSizeExponent,
                                         float fStartSpeed, float fEndSpeed, float fSpeedExponent,
                                         float fRollAmount, float fWindFalloff,
                                         unsigned int NumStreamers, float fSpeedVariance,
                                         D3DXFROMWINEVECTOR3 vDirection, D3DXFROMWINEVECTOR3 vDirVariance,
                                         D3DXFROMWINEVECTOR4 vColor0, D3DXFROMWINEVECTOR4 vColor1,
                                         D3DXFROMWINEVECTOR3 vPosMul, D3DXFROMWINEVECTOR3 vDirMul );

    void            SetCenter( D3DXFROMWINEVECTOR3 vCenter );
    void            SetStartTime( float fStartTime );
    void            SetStartSpeed( float fStartSpeed );
    void            SetFlashColor( D3DXFROMWINEVECTOR4 vFlashColor );
    D3DXFROMWINEVECTOR4     GetFlashColor();
    float           GetCurrentTime();
    float           GetLifeSpan();
    unsigned int            GetNumParticles();
    D3DXFROMWINEVECTOR3     GetCenter();

    virtual void    Init();
    virtual void    AdvanceSystem( float fTime, float fElapsedTime, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp,
                                   D3DXFROMWINEVECTOR3 vWindVel, D3DXFROMWINEVECTOR3 vGravity );
};

//-----------------------------------------------------------------------
class CMushroomParticleSystem : public CParticleSystem
{
public:
                    CMushroomParticleSystem();
    virtual         ~CMushroomParticleSystem();

    virtual void    Init();
    virtual void    AdvanceSystem( float fTime, float fElapsedTime, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp,
                                   D3DXFROMWINEVECTOR3 vWindVel, D3DXFROMWINEVECTOR3 vGravity );
};

//-----------------------------------------------------------------------
class CStalkParticleSystem : public CParticleSystem
{
public:
                    CStalkParticleSystem();
    virtual         ~CStalkParticleSystem();

    virtual void    Init();
    virtual void    AdvanceSystem( float fTime, float fElapsedTime, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp,
                                   D3DXFROMWINEVECTOR3 vWindVel, D3DXFROMWINEVECTOR3 vGravity );
};

//-----------------------------------------------------------------------
class CGroundBurstParticleSystem : public CParticleSystem
{
public:
                    CGroundBurstParticleSystem();
    virtual         ~CGroundBurstParticleSystem();

    virtual void    Init();
    virtual void    AdvanceSystem( float fTime, float fElapsedTime, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp,
                                   D3DXFROMWINEVECTOR3 vWindVel, D3DXFROMWINEVECTOR3 vGravity );
};

//-----------------------------------------------------------------------
class CLandMineParticleSystem : public CParticleSystem
{
public:
                    CLandMineParticleSystem();
    virtual         ~CLandMineParticleSystem();

    virtual void    Init();
    virtual void    AdvanceSystem( float fTime, float fElapsedTime, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp,
                                   D3DXFROMWINEVECTOR3 vWindVel, D3DXFROMWINEVECTOR3 vGravity );
};

