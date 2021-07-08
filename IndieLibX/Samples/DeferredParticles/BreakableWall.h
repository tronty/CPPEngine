//--------------------------------------------------------------------------------------
// File: BreakableWall.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include <Framework3/IRenderer.h>


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
    void            SetChunkMesh( UINT iChunk, MeshRenderer2* pMesh, D3DXFROMWINEVECTOR3 vOffset, float fWallScale );

    void            RenderInstance( MESH_INSTANCE* pInstance,
                                    ShaderID pTechnique, float fWallScale, D3DXFROMWINEMATRIX* pmViewProj,
                                    D3DXFROMWINEMATRIX pWVP, D3DXFROMWINEMATRIX pWorld );


    void            Render( ShaderID pTechnique, float fWallScale,
                             D3DXFROMWINEMATRIX* pmViewProj, D3DXFROMWINEMATRIX pWVP,
                             D3DXFROMWINEMATRIX pWorld );

    D3DXFROMWINEMATRIX      GetMeshMatrix( MESH_INSTANCE* pInstance, float fWallScale );
    D3DXFROMWINEMATRIX      GetBaseMeshMatrix( float fWallScale );
    D3DXFROMWINEMATRIX      GetChunkMeshMatrix( UINT iChunk, float fWallScale );

    bool            IsBaseMeshVisible();
    bool            IsChunkMeshVisible( UINT iChunk );

    void            CreateExplosion( D3DXFROMWINEVECTOR3 vCenter, D3DXFROMWINEVECTOR3 vDirMul, float fRadius, float fMinPower,
                                     float fMaxPower, float fWallScale );
    void            AdvancePieces( float fElapsedTime, D3DXFROMWINEVECTOR3 vGravity );
};

//--------------------------------------------------------------------------------------
class CBuilding
{
private:
    UINT m_WidthWalls;
    UINT m_HeightWalls;
    UINT m_DepthWalls;

    UINT m_NumWalls;
    CBreakableWall* m_pWallList;

    BOUNDING_SPHERE m_BS;
    float m_fWallScale;

public:
            CBuilding();
            ~CBuilding();

    bool    CreateBuilding( D3DXFROMWINEVECTOR3 vCenter, float fWallScale, UINT WidthWalls, UINT HeightWalls,
                            UINT DepthWalls );
    void    SetBaseMesh( MeshRenderer2* pMesh );
    void    SetChunkMesh( UINT iChunk, MeshRenderer2* pMesh, D3DXFROMWINEVECTOR3 vOffset );
    void    Render( ShaderID pTechnique, D3DXFROMWINEMATRIX* pmViewProj,
                        D3DXFROMWINEMATRIX pWVP, D3DXFROMWINEMATRIX pWorld);

    void    CollectBaseMeshMatrices( std::vector <D3DXFROMWINEMATRIX>* pMatrixArray );
    void    CollectChunkMeshMatrices( UINT iChunk, std::vector <D3DXFROMWINEMATRIX>* pMatrixArray );

    void    CreateExplosion( D3DXFROMWINEVECTOR3 vCenter, D3DXFROMWINEVECTOR3 vDirMul, float fRadius, float fMinPower,
                             float fMaxPower );
    void    AdvancePieces( float fElapsedTime, D3DXFROMWINEVECTOR3 vGravity );
};
