#ifndef __Terrain___
#define __Terrain___
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//--------------------------------------------------------------------------------------
// Terrain.h
// PIX Workshop GDC2007
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

struct TERRAIN_VERTEX
{
    D3DXFROMWINEVECTOR3 pos;
    D3DXFROMWINEVECTOR3 norm;
    D3DXFROMWINEVECTOR2 uv;
};

struct GRASS_VERTEX
{
    D3DXFROMWINEVECTOR3 pos;
    D3DXFROMWINEVECTOR2 uv;
};

struct BOUNDING_BOX
{
    D3DXFROMWINEVECTOR3 min;
    D3DXFROMWINEVECTOR3 max;
};

struct TERRAIN_TILE
{
    VertexBufferID pVB;
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
    IndexBufferID m_pTerrainIB;
    unsigned int* m_pTerrainRawIndices;

    unsigned int m_NumGrassBlades;
    float m_fGrassWidth;
    float m_fGrassHeight;
    VertexBufferID m_pGrassVB;
    IndexBufferID *  m_ppGrassIB;

    unsigned int m_NumDirections;
    D3DXFROMWINEVECTOR3* m_pDirections;

public:
                            CTerrain();
                            ~CTerrain();

    void                    OnLostDevice();
    void                    OnDestroyDevice();
    int                 OnResetDevice();
    int                 OnCreateDevice();

    int                 LoadTerrain( char* strHeightMap, unsigned int SqrtNumTiles, unsigned int NumSidesPerTile,
                                         float fWorldScale, float fHeightScale,
                                         unsigned int NumGrassBlades, float fGrassWidth, float fGrassHeight );
    float                   GetHeightForTile( unsigned int iTile, D3DXFROMWINEVECTOR3* pPos );
    float                   GetHeightOnMap( D3DXFROMWINEVECTOR3* pPos );
    D3DXFROMWINEVECTOR3             GetNormalOnMap( D3DXFROMWINEVECTOR3* pPos );
    void                    RenderTile( TERRAIN_TILE* pTile );
    void                    RenderGrass( D3DXFROMWINEVECTOR3* pViewDir, unsigned int NumInstances );

    float                   GetWorldScale()
    {
        return m_fWorldScale;
    }
    IndexBufferID  GetTerrainIB()
    {
        return m_pTerrainIB;
    }
    unsigned int                    GetNumTiles()
    {
        return m_NumTiles;
    }
    TERRAIN_TILE* GetTile( unsigned int iTile )
    {
        return &m_pTiles[iTile];
    }

protected:
    D3DXFROMWINEVECTOR2             GetUVForPosition( D3DXFROMWINEVECTOR3* pPos );
    int                 LoadBMPImage( char* strHeightMap );
    int                 GenerateTile( TERRAIN_TILE* pTile, BOUNDING_BOX* pBBox );
    int                 CreateTileResources( TERRAIN_TILE* pTile );
    int                 CreateGrass();
};

float RPercent();

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
#endif

