/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//--------------------------------------------------------------------------------------
// Terrain.cpp
// PIX Workshop GDC2007
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "Terrain.h"

static D3DXFROMWINEVECTOR3 s_vDirections[8] =
{
    D3DXFROMWINEVECTOR3( -1, 0, 0 ),			//left
    D3DXFROMWINEVECTOR3( -0.707f, 0, 0.707f ),	//up-left
    D3DXFROMWINEVECTOR3( 0, 0, 1 ),				//up
    D3DXFROMWINEVECTOR3( 0.707f, 0, 0.707f ),	//up-right
    D3DXFROMWINEVECTOR3( 1, 0, 0 ),				//right
    D3DXFROMWINEVECTOR3( 0.707f, 0, -0.707f ),	//down-right
    D3DXFROMWINEVECTOR3( 0, 0, -1 ),			//down
    D3DXFROMWINEVECTOR3( -0.707f, 0, -0.707f )  //down-left
};

//--------------------------------------------------------------------------------------
CTerrain::CTerrain() : m_SqrtNumTiles( 0 ),
                       m_NumTiles( 0 ),
                       m_NumSidesPerTile( 0 ),
                       m_pTiles( 0 ),
                       m_fWorldScale( 0.0f ),
                       m_fHeightScale( 0.0f ),
                       m_HeightMapX( 0 ),
                       m_HeightMapY( 0 ),
                       m_pHeightBits( 0 ),
                       m_NumIndices( 0 ),
                       m_pTerrainIB( 0 ),
                       m_pTerrainRawIndices( 0 ),
                       m_pGrassVB( 0 ),
                       m_ppGrassIB( 0 ),
                       m_NumDirections( 0 ),
                       m_pDirections( 0 )
{
}


//--------------------------------------------------------------------------------------
CTerrain::~CTerrain()
{
    for( unsigned int i = 0; i < m_NumTiles; i++ )
    {
        SAFE_DELETE_ARRAY( m_pTiles[i].pRawVertices );
    }

    SAFE_DELETE_ARRAY( m_pTiles );
    SAFE_DELETE_ARRAY( m_pHeightBits );
    SAFE_DELETE_ARRAY( m_pTerrainRawIndices );
}


//--------------------------------------------------------------------------------------
void CTerrain::OnLostDevice()
{
    for( unsigned int i = 0; i < m_NumTiles; i++ )
    {
        //////////////SAFE_RELEASE( m_pTiles[i].pVB );
    }
    //////////////SAFE_RELEASE( m_pTerrainIB );
    //////////////SAFE_RELEASE( m_pGrassVB );
    for( unsigned int i = 0; i < m_NumDirections; i++ )
        //////////////SAFE_RELEASE( m_ppGrassIB[i] );

    // SAFE_DELETE_ARRAY( *m_ppGrassIB );
    SAFE_DELETE_ARRAY( m_pDirections );
}

//--------------------------------------------------------------------------------------
int CTerrain::OnResetDevice( )
{
    int hr = 0;

    if( 0 == m_NumTiles )
        return 1;

    // Create the terrain tile vertex buffers
    for( unsigned int i = 0; i < m_NumTiles; i++ )
    {
        ( CreateTileResources( &m_pTiles[i] ) );
    }

    // Create the index buffer
    m_pTerrainIB=IRenderer::GetRendererInstance()->addIndexBuffer(m_NumIndices, 4, STATIC, m_pTerrainRawIndices);

    ( CreateGrass() );
    return hr;
}


//--------------------------------------------------------------------------------------
int CTerrain::OnCreateDevice(  )
{
    int hr = 0;

    if( 0 == m_NumTiles )
        return 1;

    // Create the terrain tile vertex buffers
    for( unsigned int i = 0; i < m_NumTiles; i++ )
    {
        ( CreateTileResources( &m_pTiles[i] ) );
    }

    // Create the index buffer
    m_pTerrainIB=IRenderer::GetRendererInstance()->addIndexBuffer(m_NumIndices, 4, STATIC, m_pTerrainRawIndices);

    ( CreateGrass() );
    return hr;
}

//--------------------------------------------------------------------------------------
int CTerrain::LoadTerrain( char* strHeightMap, unsigned int SqrtNumTiles, unsigned int NumSidesPerTile, float fWorldScale,
                               float fHeightScale,
                               unsigned int NumGrassBlades, float fGrassWidth, float fGrassHeight )
{
    int hr = 0;

    // Store variables
    m_SqrtNumTiles = SqrtNumTiles;
    m_fWorldScale = fWorldScale;
    m_fHeightScale = fHeightScale;
    m_NumSidesPerTile = NumSidesPerTile;
    m_NumTiles = SqrtNumTiles * SqrtNumTiles;
    m_NumGrassBlades = NumGrassBlades;
    m_fGrassWidth = fGrassWidth;
    m_fGrassHeight = fGrassHeight;

    // Load the image
    TextureID texID /* ??? */ =IRenderer::GetRendererInstance()->addImageLibTexture(strHeightMap, false, IRenderer::GetRendererInstance()->Getlinear());

    // Create tiles
    m_pTiles = new TERRAIN_TILE[ m_NumTiles ];
    if( !m_pTiles )
        return 1;

    unsigned int iTile = 0;
    float zStart = -m_fWorldScale / 2.0f;
    float zDelta = m_fWorldScale / ( float )m_SqrtNumTiles;
    float xDelta = m_fWorldScale / ( float )m_SqrtNumTiles;
    for( unsigned int z = 0; z < m_SqrtNumTiles; z++ )
    {
        float xStart = -m_fWorldScale / 2.0f;
        for( unsigned int x = 0; x < m_SqrtNumTiles; x++ )
        {
            BOUNDING_BOX BBox;
            BBox.min = D3DXFROMWINEVECTOR3( xStart, 0, zStart );
            BBox.max = D3DXFROMWINEVECTOR3( xStart + xDelta, 0, zStart + zDelta );

            ( GenerateTile( &m_pTiles[iTile], &BBox ) );

            iTile ++;
            xStart += xDelta;
        }
        zStart += zDelta;
    }

    // Create the indices for the tile strips
    m_NumIndices = ( m_NumSidesPerTile + 2 ) * 2 * ( m_NumSidesPerTile )- 2;
    m_pTerrainRawIndices = new unsigned int[ m_NumIndices ];
    if( !m_pTerrainRawIndices )
        return 1;

    unsigned int vIndex = 0;
    unsigned int iIndex = 0;
    for( unsigned int z = 0; z < m_NumSidesPerTile; z++ )
    {
        for( unsigned int x = 0; x < m_NumSidesPerTile + 1; x++ )
        {
            m_pTerrainRawIndices[iIndex] = vIndex;
            iIndex++;
            m_pTerrainRawIndices[iIndex] = vIndex + ( unsigned int )m_NumSidesPerTile + 1;
            iIndex++;
            vIndex++;
        }
        if( z != m_NumSidesPerTile - 1 )
        {
            // add a degenerate tri
            m_pTerrainRawIndices[iIndex] = vIndex + ( unsigned int )m_NumSidesPerTile;
            iIndex++;
            m_pTerrainRawIndices[iIndex] = vIndex;
            iIndex++;
        }
    }

    return hr;
}


//--------------------------------------------------------------------------------------
float CTerrain::GetHeightForTile( unsigned int iTile, D3DXFROMWINEVECTOR3* pPos )
{
    // TODO: impl
    return 0.0f;
}


//--------------------------------------------------------------------------------------
#define HEIGHT_INDEX( a, b ) ( (b)*m_HeightMapX + (a) )
#define LINEAR_INTERPOLATE(a,b,x) (a*(1.0f-x) + b*x)
float CTerrain::GetHeightOnMap( D3DXFROMWINEVECTOR3* pPos )
{
    // move x and z into [0..1] range
    D3DXFROMWINEVECTOR2 uv = GetUVForPosition( pPos );
    float x = uv.x;
    float z = uv.y;

    // scale into heightmap space
    x *= m_HeightMapX;
    z *= m_HeightMapY;
    x += 0.5f;
    z += 0.5f;
    if( x >= m_HeightMapX - 1 )
        x = ( float )m_HeightMapX - 2;
    if( z >= m_HeightMapY - 1 )
        z = ( float )m_HeightMapY - 2;
    z = max( 0.0f, z );
    x = max( 0.0f, x );

    // bilinearly interpolate
	unsigned long integer_X = x;
    float fractional_X = x - integer_X;

	unsigned long integer_Z = z;
    float fractional_Z = z - integer_Z;

    float v1 = m_pHeightBits[ HEIGHT_INDEX( integer_X,    integer_Z ) ];
    float v2 = m_pHeightBits[ HEIGHT_INDEX( integer_X + 1,integer_Z ) ];
    float v3 = m_pHeightBits[ HEIGHT_INDEX( integer_X,    integer_Z + 1 ) ];
    float v4 = m_pHeightBits[ HEIGHT_INDEX( integer_X + 1,integer_Z + 1 ) ];

    float i1 = LINEAR_INTERPOLATE( v1 , v2 , fractional_X );
    float i2 = LINEAR_INTERPOLATE( v3 , v4 , fractional_X );

    float result = LINEAR_INTERPOLATE( i1 , i2 , fractional_Z );

    return result;
}


//--------------------------------------------------------------------------------------
D3DXFROMWINEVECTOR3 CTerrain::GetNormalOnMap( D3DXFROMWINEVECTOR3* pPos )
{
    // Calculate the normal
    float xDelta = ( m_fWorldScale / ( float )m_SqrtNumTiles ) / ( float )m_NumSidesPerTile;
    float zDelta = ( m_fWorldScale / ( float )m_SqrtNumTiles ) / ( float )m_NumSidesPerTile;

    D3DXFROMWINEVECTOR3 vLeft = *pPos - D3DXFROMWINEVECTOR3( xDelta, 0, 0 );
    D3DXFROMWINEVECTOR3 vRight = *pPos + D3DXFROMWINEVECTOR3( xDelta, 0, 0 );
    D3DXFROMWINEVECTOR3 vUp = *pPos + D3DXFROMWINEVECTOR3( 0, 0, zDelta );
    D3DXFROMWINEVECTOR3 vDown = *pPos - D3DXFROMWINEVECTOR3( 0, 0, zDelta );

    vLeft.y = GetHeightOnMap( &vLeft );
    vRight.y = GetHeightOnMap( &vRight );
    vUp.y = GetHeightOnMap( &vUp );
    vDown.y = GetHeightOnMap( &vDown );

    D3DXFROMWINEVECTOR3 e0 = vRight - vLeft;
    D3DXFROMWINEVECTOR3 e1 = vUp - vDown;
    D3DXFROMWINEVECTOR3 ortho;
    D3DXFROMWINEVECTOR3 norm;
    D3DXFROMWINEVec3Cross( &ortho, &e1, &e0 );
    D3DXFROMWINEVec3Normalize( &norm, &ortho );

    return norm;
}


//--------------------------------------------------------------------------------------
void CTerrain::RenderTile( TERRAIN_TILE* pTile )
{
    IRenderer::GetRendererInstance()->setVertexBuffer(0, pTile->pVB);
    IRenderer::GetRendererInstance()->DrawIndexedPrimitive( PRIM_TRIANGLE_STRIP, 0, 0, pTile->NumVertices, 0, m_NumIndices - 2 );
}


//--------------------------------------------------------------------------------------
void CTerrain::RenderGrass( D3DXFROMWINEVECTOR3* pViewDir, unsigned int NumInstances )
{
    // Pick the IB
    float fMaxDot = -1.0f;
    //unsigned 
	int maxDir = 0;
    for( unsigned int i = 0; i < m_NumDirections; i++ )
    {
        float fDot = D3DXFROMWINEVec3Dot( pViewDir, &m_pDirections[i] );
        if( fDot > fMaxDot )
        {
            fMaxDot = fDot;
            maxDir = i;
        }
    }

    IRenderer::GetRendererInstance()->setIndexBuffer( m_ppGrassIB[maxDir] );
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, m_pGrassVB);
    IRenderer::GetRendererInstance()->DrawIndexedPrimitive( PRIM_TRIANGLES, 0, 0, m_NumGrassBlades * 4, 0, m_NumGrassBlades * 2 );
}


//--------------------------------------------------------------------------------------
D3DXFROMWINEVECTOR2 CTerrain::GetUVForPosition( D3DXFROMWINEVECTOR3* pPos )
{
    D3DXFROMWINEVECTOR2 uv;
    uv.x = ( pPos->x / m_fWorldScale ) + 0.5f;
    uv.y = ( pPos->z / m_fWorldScale ) + 0.5f;
    return uv;
}

//--------------------------------------------------------------------------------------
int CTerrain::GenerateTile( TERRAIN_TILE* pTile, BOUNDING_BOX* pBBox )
{
    int hr = 0;

    // Alloc memory for the vertices
    pTile->NumVertices = ( m_NumSidesPerTile + 1 ) * ( m_NumSidesPerTile + 1 );
    pTile->pRawVertices = new TERRAIN_VERTEX[ pTile->NumVertices ];
    if( !pTile->pRawVertices )
        return 1;

    pTile->BBox = *pBBox;
    pTile->Color.x = 0.60f + RPercent() * 0.40f;
    pTile->Color.y = 0.60f + RPercent() * 0.40f;
    pTile->Color.z = 0.60f + RPercent() * 0.40f;
    pTile->Color.w = 1.0f;

    unsigned int iVertex = 0;
    float zStart = pBBox->min.z;
    float xDelta = ( pBBox->max.x - pBBox->min.x ) / ( float )m_NumSidesPerTile;
    float zDelta = ( pBBox->max.z - pBBox->min.z ) / ( float )m_NumSidesPerTile;

    // Loop through terrain vertices and get height from the heightmap
    for( unsigned int z = 0; z < m_NumSidesPerTile + 1; z++ )
    {
        float xStart = pBBox->min.x;
        for( unsigned int x = 0; x < m_NumSidesPerTile + 1; x++ )
        {
            D3DXFROMWINEVECTOR3 pos( xStart,0,zStart );
            D3DXFROMWINEVECTOR3 norm;
            pos.y = GetHeightOnMap( &pos );
            pTile->pRawVertices[iVertex].pos = pos;
            pTile->pRawVertices[iVertex].uv = GetUVForPosition( &pos );
            pTile->pRawVertices[iVertex].uv.y = 1.0f - pTile->pRawVertices[iVertex].uv.y;
            pTile->pRawVertices[iVertex].norm = GetNormalOnMap( &pos );

            iVertex ++;
            xStart += xDelta;
        }
        zStart += zDelta;
    }

    return hr;
}


//--------------------------------------------------------------------------------------
int CTerrain::CreateTileResources( TERRAIN_TILE* pTile )
{
    int hr = 0;
    pTile->pVB=IRenderer::GetRendererInstance()->addVertexBuffer( m_pTiles->NumVertices * sizeof( TERRAIN_VERTEX ), STATIC, pTile->pRawVertices);
    return hr;
}


//--------------------------------------------------------------------------------------
float RPercent()
{
    float ret = ( float )( ( stx_rand() % 20000 ) - 10000 );
    return ret / 10000.0f;
}


//--------------------------------------------------------------------------------------
int CTerrain::CreateGrass()
{
    int hr = 0;

    float fTileSize = m_fWorldScale / ( float )m_SqrtNumTiles;
    fTileSize /= 2.0f;
    float fGrassWidth = m_fGrassWidth / 2.0f;
    float fGrassHeight = m_fGrassHeight;

    D3DXFROMWINEVECTOR3* pGrassCenters = new D3DXFROMWINEVECTOR3[ m_NumGrassBlades ];
    if( !pGrassCenters )
        return 1;

    for( unsigned int i = 0; i < m_NumGrassBlades; i++ )
    {
        pGrassCenters[i].x = RPercent() * fTileSize;
        pGrassCenters[i].y = 0.0f;
        pGrassCenters[i].z = RPercent() * fTileSize;
    }

    GRASS_VERTEX* pVertices = 0;
    unsigned int vIndex = 0;
    for( unsigned int i = 0; i < m_NumGrassBlades; i++ )
    {
        D3DXFROMWINEVECTOR3 vRandRight( RPercent(), 0, RPercent() );
        D3DXFROMWINEVec3Normalize( &vRandRight, &vRandRight );

        pVertices[vIndex  ].pos = pGrassCenters[i] - vRandRight * fGrassWidth;
        pVertices[vIndex  ].uv = D3DXFROMWINEVECTOR2( 0, 1 );
        pVertices[vIndex + 1].pos = pGrassCenters[i] + vRandRight * fGrassWidth;
        pVertices[vIndex + 1].uv = D3DXFROMWINEVECTOR2( 1, 1 );
        pVertices[vIndex + 2].pos = pVertices[vIndex + 1].pos + D3DXFROMWINEVECTOR3( 0, fGrassHeight, 0 );
        pVertices[vIndex + 2].uv = D3DXFROMWINEVECTOR2( 1, 0 );
        pVertices[vIndex + 3].pos = pVertices[vIndex  ].pos + D3DXFROMWINEVECTOR3( 0, fGrassHeight, 0 );
        pVertices[vIndex + 3].uv = D3DXFROMWINEVECTOR2( 0, 0 );
    }
    m_pGrassVB=IRenderer::GetRendererInstance()->addVertexBuffer(m_NumGrassBlades*4, STATIC, pVertices);

    // Alloc indices and distances
    __WORD__* pGrassIndices = new __WORD__[ m_NumGrassBlades ];
    float* pGrassDistances = new float[ m_NumGrassBlades ];

    m_NumDirections = 16;

    m_ppGrassIB = new IndexBufferID[ m_NumDirections ];
    m_pDirections = new D3DXFROMWINEVECTOR3[ m_NumDirections ];

    D3DXFROMWINEVECTOR3 vStartDir( -1,0,0 );
    float fAngleDelta = ( D3DXFROMWINE_PI * 2.0f ) / ( float )( m_NumDirections );
    for( unsigned int i = 0; i < m_NumDirections; i++ )
    {
        D3DXFROMWINEMATRIX mRot;
        D3DXFROMWINEMatrixRotationY( &mRot, i * fAngleDelta );
        D3DXFROMWINEVec3TransformNormal( &m_pDirections[i], &vStartDir, &mRot );

        // init indices and distances
        for( unsigned int g = 0; g < m_NumGrassBlades; g++ )
        {
            pGrassIndices[g] = ( __WORD__ )g;
            pGrassDistances[g] = -D3DXFROMWINEVec3Dot( &m_pDirections[i], &pGrassCenters[g] );
        }

        // sort indices
        QuickDepthSort( pGrassIndices, pGrassDistances, 0, m_NumGrassBlades - 1 );

        __WORD__* pIndices = new __WORD__[ m_NumGrassBlades * 6 ];
        unsigned int iIndex = 0;
        for( unsigned int g = 0; g < m_NumGrassBlades; g++ )
        {
            //Starting vert
            __WORD__ GrassIndex = pGrassIndices[g] * 4;
            //Tri1
            pIndices[iIndex  ] = GrassIndex;
            pIndices[iIndex + 1] = GrassIndex + 3;
            pIndices[iIndex + 2] = GrassIndex + 1;
            //Tri2
            pIndices[iIndex + 3] = GrassIndex + 1;
            pIndices[iIndex + 4] = GrassIndex + 3;
            pIndices[iIndex + 5] = GrassIndex + 2;

            iIndex += 6;
        }
	m_ppGrassIB[i]=IRenderer::GetRendererInstance()->addIndexBuffer( m_NumGrassBlades * 6, sizeof( __WORD__ ), STATIC, pIndices );
    }									

    SAFE_DELETE_ARRAY( pGrassIndices );
    SAFE_DELETE_ARRAY( pGrassDistances );
    SAFE_DELETE_ARRAY( pGrassCenters );

    return hr;
}

