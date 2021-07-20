/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
#define CGrowableArray std::vector

//--------------------------------------------------------------------------------------
// File: BreakableWall.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#define NUM_CHUNKS 9

struct BOUNDING_SPHERE
{
    D3DXFROMWINEVECTOR3 vCenter;
    float fRadius;
};

struct MESH_INSTANCE
{
    MeshRenderer2* pMesh;
    BOUNDING_SPHERE BS;
    D3DXFROMWINEVECTOR3 vPosition;
    D3DXFROMWINEVECTOR3 vRotation;
    D3DXFROMWINEVECTOR3 vRotationOrig;

    bool bDynamic;
    bool bVisible;
    D3DXFROMWINEVECTOR3 vVelocity;
    D3DXFROMWINEVECTOR3 vRotationSpeed;
};

//--------------------------------------------------------------------------------------
class CBreakableWall
{
private:
    MESH_INSTANCE m_BaseMesh;
    MESH_INSTANCE   m_ChunkMesh[NUM_CHUNKS];
    bool m_bBroken;

public:
                    CBreakableWall();
                    ~CBreakableWall();

    void            SetPosition( D3DXFROMWINEVECTOR3 vPosition );
    void            SetRotation( D3DXFROMWINEVECTOR3 vRotation );
    void            SetBaseMesh( MeshRenderer2* pMesh );
    void            SetChunkMesh( unsigned int iChunk, MeshRenderer2* pMesh, D3DXFROMWINEVECTOR3 vOffset, float fWallScale );
    void            RenderInstance( MESH_INSTANCE* pInstance, stx_Effect::stx_Technique*  pTechnique, float fWallScale, D3DXFROMWINEMATRIX* pmViewProj,
                                    stx_Effect::stx_Variable*  pWVP, stx_Effect::stx_Variable*  pWorld );
    void            Render( stx_Effect::stx_Technique*  pTechnique, float fWallScale,
                            D3DXFROMWINEMATRIX* pmViewProj, stx_Effect::stx_Variable*  pWVP,
                            stx_Effect::stx_Variable*  pWorld );

    D3DXFROMWINEMATRIX      GetMeshMatrix( MESH_INSTANCE* pInstance, float fWallScale );
    D3DXFROMWINEMATRIX      GetBaseMeshMatrix( float fWallScale );
    D3DXFROMWINEMATRIX      GetChunkMeshMatrix( unsigned int iChunk, float fWallScale );

    bool            IsBaseMeshVisible();
    bool            IsChunkMeshVisible( unsigned int iChunk );

    void            CreateExplosion( D3DXFROMWINEVECTOR3 vCenter, D3DXFROMWINEVECTOR3 vDirMul, float fRadius, float fMinPower,
                                     float fMaxPower, float fWallScale );
    void            AdvancePieces( float fElapsedTime, D3DXFROMWINEVECTOR3 vGravity );
};

//--------------------------------------------------------------------------------------
class CBuilding
{
private:
    unsigned int m_WidthWalls;
    unsigned int m_HeightWalls;
    unsigned int m_DepthWalls;

    unsigned int m_NumWalls;
    CBreakableWall* m_pWallList;

    BOUNDING_SPHERE m_BS;
    float m_fWallScale;

public:
            CBuilding();
            ~CBuilding();

    bool    CreateBuilding( D3DXFROMWINEVECTOR3 vCenter, float fWallScale, unsigned int WidthWalls, unsigned int HeightWalls,
                            unsigned int DepthWalls );
    void    SetBaseMesh( MeshRenderer2* pMesh );
    void    SetChunkMesh( unsigned int iChunk, MeshRenderer2* pMesh, D3DXFROMWINEVECTOR3 vOffset );
    void    Render( stx_Effect::stx_Technique*  pTechnique, D3DXFROMWINEMATRIX* pmViewProj,
                    stx_Effect::stx_Variable*  pWVP, stx_Effect::stx_Variable*  pWorld );

    void    CollectBaseMeshMatrices( CGrowableArray <D3DXFROMWINEMATRIX>* pMatrixArray );
    void    CollectChunkMeshMatrices( unsigned int iChunk, CGrowableArray <D3DXFROMWINEMATRIX>* pMatrixArray );

    void    CreateExplosion( D3DXFROMWINEVECTOR3 vCenter, D3DXFROMWINEVECTOR3 vDirMul, float fRadius, float fMinPower,
                             float fMaxPower );
    void    AdvancePieces( float fElapsedTime, D3DXFROMWINEVECTOR3 vGravity );
};

