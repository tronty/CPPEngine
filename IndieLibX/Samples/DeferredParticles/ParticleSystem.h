//--------------------------------------------------------------------------------------
// File: ParticleSystem.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
void NewExplosion( D3DXFROMWINEVECTOR3 vCenter, float fSize );
/*
struct appdata {
    float3 Position	: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float2 UV		: TEXCOORD0;
};
*/
struct PARTICLE_VERTEX
{
    D3DXFROMWINEVECTOR3 vPos;
    D3DXFROMWINEVECTOR2 vUV;
    D3DXFROMWINEVECTOR2 fLifefRot;
    D3DXFROMWINEVECTOR4 Color;
};

struct PARTICLE
{
    D3DXFROMWINEVECTOR3 vPos;
    D3DXFROMWINEVECTOR3 vDir;
    D3DXFROMWINEVECTOR3 vMass;
    D3DXFROMWINEVECTOR4 Color;
    float fRadius;
    D3DXFROMWINEVECTOR2 fLifefRot;
    float fFade;
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

INT CreateParticleArray( UINT MaxParticles );
void DestroyParticleArray();
void CopyParticlesToVertexBuffer( PARTICLE_VERTEX* pVB, D3DXFROMWINEVECTOR3 vEye, D3DXFROMWINEVECTOR3 vRight, D3DXFROMWINEVECTOR3 vUp );
float RPercent();
void InitParticleArray( UINT NumParticles );
UINT GetNumActiveParticles();

//-----------------------------------------------------------------------
class CParticleSystem
{
protected:
    UINT m_NumParticles;
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

    UINT m_NumStreamers;
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

    INT         CreateParticleSystem( UINT NumParticles );
    void            SetSystemAttributes( D3DXFROMWINEVECTOR3 vCenter,
                                         float fSpread, float fLifeSpan, float fFadeExponent,
                                         float fStartSize, float fEndSize, float fSizeExponent,
                                         float fStartSpeed, float fEndSpeed, float fSpeedExponent,
                                         float fRollAmount, float fWindFalloff,
                                         UINT NumStreamers, float fSpeedVariance,
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
    UINT            GetNumParticles();
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

