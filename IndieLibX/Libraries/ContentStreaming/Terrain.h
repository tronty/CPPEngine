//--------------------------------------------------------------------------------------
// Terrain.h
// PIX Workshop GDC2007
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once

struct TERRAIN_VERTEX
{
    float3 pos;
    float3 norm;
    float2 uv;
};

struct BOUNDING_BOX
{
    float3 min;
    float3 max;
};

struct TERRAIN_TILE
{
    UINT NumVertices;
    TERRAIN_VERTEX* pRawVertices;
    float4 Color;
    BOUNDING_BOX BBox;
};

class CTerrain
{
private:
    UINT m_SqrtNumTiles;
    UINT m_NumTiles;
    UINT m_NumSidesPerTile;
    TERRAIN_TILE* m_pTiles;
    float m_fWorldScale;
    float m_fHeightScale;
    UINT m_HeightMapX;
    UINT m_HeightMapY;
    float* m_pHeightBits;

    UINT m_NumIndices;
    SHORT* m_pTerrainRawIndices;

public:
                CTerrain();
                ~CTerrain();

    HRESULT     LoadTerrain( CHAR* strHeightMap, UINT SqrtNumTiles, UINT NumSidesPerTile, float fWorldScale,
                             float fHeightScale, bool bCreateTiles );
    float       GetHeightForTile( UINT iTile, float3* pPos );
    float       GetHeightOnMap( float3* pPos );
    float3 GetNormalOnMap( float3* pPos );

    float       GetWorldScale()
    {
        return m_fWorldScale;
    }
    UINT        GetNumTiles()
    {
        return m_NumTiles;
    }
    TERRAIN_TILE* GetTile( UINT iTile )
    {
        return &m_pTiles[iTile];
    }
    UINT        GetNumIndices()
    {
        return m_NumIndices;
    }
    SHORT* GetIndices()
    {
        return m_pTerrainRawIndices;
    }
    UINT        GetNumTileVertices()
    {
        return ( m_NumSidesPerTile + 1 ) * ( m_NumSidesPerTile + 1 );
    }

protected:
    float2 GetUVForPosition( float3* pPos );
    HRESULT     LoadBMPImage( CHAR* strHeightMap );
    HRESULT     GenerateTile( TERRAIN_TILE* pTile, BOUNDING_BOX* pBBox );
};

float RPercent();
