/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//--------------------------------------------------------------------------------------
// Terrain.h
// PIX Workshop GDC2007
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once

struct TERRAIN_VERTEX
{
    D3DXFROMWINEVECTOR3 pos;
    D3DXFROMWINEVECTOR3 norm;
    D3DXFROMWINEVECTOR2 uv;
};

struct BOUNDING_BOX
{
    D3DXFROMWINEVECTOR3 min;
    D3DXFROMWINEVECTOR3 max;
};

struct TERRAIN_TILE
{
    unsigned int NumVertices;
    TERRAIN_VERTEX* pRawVertices;
    D3DXFROMWINEVECTOR4 Color;
    BOUNDING_BOX BBox;
};

class CTerrain
{
private:
    unsigned int m_SqrtNumTiles;
    unsigned int m_NumTiles;
    unsigned int m_NumSidesPerTile;
    TERRAIN_TILE* m_pTiles;
    float m_fWorldScale;
    float m_fHeightScale;
    unsigned int m_HeightMapX;
    unsigned int m_HeightMapY;
    float* m_pHeightBits;

    unsigned int m_NumIndices;
    __WORD__* m_pTerrainRawIndices;

public:
                CTerrain();
                ~CTerrain();

    int     LoadTerrain( char* strHeightMap, unsigned int SqrtNumTiles, unsigned int NumSidesPerTile, float fWorldScale,
                             float fHeightScale, bool bCreateTiles );
    float       GetHeightForTile( unsigned int iTile, D3DXFROMWINEVECTOR3* pPos );
    float       GetHeightOnMap( D3DXFROMWINEVECTOR3* pPos );
    D3DXFROMWINEVECTOR3 GetNormalOnMap( D3DXFROMWINEVECTOR3* pPos );

    float       GetWorldScale()
    {
        return m_fWorldScale;
    }
    unsigned int        GetNumTiles()
    {
        return m_NumTiles;
    }
    TERRAIN_TILE* GetTile( unsigned int iTile )
    {
        return &m_pTiles[iTile];
    }
    unsigned int        GetNumIndices()
    {
        return m_NumIndices;
    }
    __WORD__* GetIndices()
    {
        return m_pTerrainRawIndices;
    }
    unsigned int        GetNumTileVertices()
    {
        return ( m_NumSidesPerTile + 1 ) * ( m_NumSidesPerTile + 1 );
    }

protected:
    D3DXFROMWINEVECTOR2 GetUVForPosition( D3DXFROMWINEVECTOR3* pPos );
    int     LoadBMPImage( char* strHeightMap );
    int     GenerateTile( TERRAIN_TILE* pTile, BOUNDING_BOX* pBBox );
};

float RPercent();
