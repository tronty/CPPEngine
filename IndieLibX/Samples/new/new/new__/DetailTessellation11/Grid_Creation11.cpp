/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//--------------------------------------------------------------------------------------
// File: Grid_Creation11.cpp
//
// Various functions to create a grid.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "Grid_Creation11.h"

//--------------------------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------------------------
// Only used for tangent space version of grid creation
#define GRID_OPTIMIZE_INDICES
#define GRID_OPTIMIZE_VERTICES

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
void GridOptimizeIndices( __WORD__* pIndexBuffer, int nNumIndex, int nNumVertex );
void GridOptimizeVertices( __WORD__* pIndexBuffer, void *pVertex, __DWORD__ dwVertexSize, 
                           int nNumIndex, int nNumVertex );


//--------------------------------------------------------------------------------------
// FillGrid_NonIndexed
// Creates a regular grid of non-indexed triangles.
// The w coordinate of each vertex contains its index (vertex number). 
//
// Parameters:
//
// IN
// IRenderer::GetRendererInstance():                  The D3D device
// dwWidth, dwLength:           Number of grid vertices in X and Z direction
// fGridSizeX, fGridSizeZ:      Grid extents in local space units
// uDummyStartVertices, uDummyEndVertices:  Add padding vertices to the start or end of buffer
//
// OUT
// lplpVB:                      A pointer to the vertex buffer containing grid vertices
//--------------------------------------------------------------------------------------
void FillGrid_NonIndexed( __DWORD__ dwWidth, __DWORD__ dwLength, 
                          float fGridSizeX, float fGridSizeZ, 
                          __DWORD__ uDummyStartVertices, __DWORD__ uDummyEndVertices,
                          VertexBufferID  lplpVB )
{
    int hr;
    __DWORD__   nNumVertex = 3 * 2 * dwWidth * dwLength;
    float   fStepX = fGridSizeX / dwWidth;
    float   fStepZ = fGridSizeZ / dwLength;
    
    // Allocate memory for buffer of vertices in system memory
    SIMPLEVERTEX*    pVertexBuffer = new SIMPLEVERTEX[nNumVertex + uDummyStartVertices + uDummyEndVertices];
    SIMPLEVERTEX*    pVertex = &pVertexBuffer[0];

    // Skip a number of dummy start vertices if required
    pVertex += uDummyStartVertices;

    for ( __DWORD__ i=0; i<dwLength; ++i )
    {
        for ( __DWORD__ j=0; j<dwWidth; ++j )
        {
            // Vertex_ 0
            pVertex->x = -fGridSizeX/2.0f + j*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - i*fStepZ;
            pVertex->u = 0.0f + ( (float)j / dwWidth );
            pVertex->v = 0.0f + ( (float)i / dwLength );
            pVertex++;

            // Vertex_ 1
            pVertex->x = -fGridSizeX/2.0f + ( j+1 )*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - i*fStepZ;
            pVertex->u = 0.0f + ( (float)( j+1 ) / dwWidth );
            pVertex->v = 0.0f + ( (float)i      / dwLength );
            pVertex++;

            // Vertex_ 3
            pVertex->x = -fGridSizeX/2.0f + j*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - (i+1)*fStepZ;
            pVertex->u = 0.0f + ( (float)j / dwWidth );
            pVertex->v = 0.0f + ( (float)(i+1) / dwLength );
            pVertex++;

            // Vertex_ 3
            pVertex->x = -fGridSizeX/2.0f + j*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - (i+1)*fStepZ;
            pVertex->u = 0.0f + ( (float)j / dwWidth );
            pVertex->v = 0.0f + ( (float)( i+1 ) / dwLength );
            pVertex++;

            // Vertex_ 1
            pVertex->x = -fGridSizeX/2.0f + ( j+1 )*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - i*fStepZ;
            pVertex->u = 0.0f + ((float)(j+1)/dwWidth);
            pVertex->v = 0.0f + ((float)i/dwLength);
            pVertex++;

            // Vertex_ 2
            pVertex->x = -fGridSizeX/2.0f + ( j+1 )*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - ( i+1 )*fStepZ;
            pVertex->u = 0.0f + ( (float)( j+1 ) / dwWidth );
            pVertex->v = 0.0f + ( (float)( i+1 ) / dwLength );
            pVertex++;
        }
    }

    // Set initial data info
    
    InitData.pSysMem = pVertexBuffer;

    // Fill DX11 vertex buffer description
    lplpVB=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( SIMPLEVERTEX ) * (nNumVertex + uDummyStartVertices + uDummyEndVertices), STATIC, (const void *)pVertexBuffer);

    // Release host memory vertex buffer
    delete [] pVertexBuffer;
}


//--------------------------------------------------------------------------------------
// FillGrid_Indexed
// Creates a regular grid of indexed triangles
//
// Parameters:
//
// IN
// IRenderer::GetRendererInstance():                The D3D device
// dwWidth, dwLength:         Number of grid vertices in X and Z direction
// fGridSizeX, fGridSizeZ:    Grid extents in local space units
//
// OUT
// lplpVB:                    A pointer to the vertex buffer containing grid vertices
// lplpIB:                    A pointer to the index buffer containing grid indices
//--------------------------------------------------------------------------------------
void FillGrid_Indexed( __DWORD__ dwWidth, __DWORD__ dwLength, 
                       float fGridSizeX, float fGridSizeZ, 
                       VertexBufferID  lplpVB, VertexBufferID  lplpIB )
{
    int      hr;
    __DWORD__        nNumVertex = ( dwWidth+1 ) * ( dwLength+1 );
    __DWORD__        nNumIndex = 3 * 2 * dwWidth * dwLength;
    float        fStepX = fGridSizeX / dwWidth;
    float        fStepZ = fGridSizeZ / dwLength;
    
    // Allocate memory for buffer of vertices in system memory
    SIMPLEVERTEX*    pVertexBuffer = new SIMPLEVERTEX[nNumVertex];
    SIMPLEVERTEX*    pVertex = &pVertexBuffer[0];

    // Fill vertex buffer
    for ( __DWORD__ i=0; i<=dwLength; ++i )
    {
        for ( __DWORD__ j=0; j<=dwWidth; ++j )
        {
            pVertex->x = -fGridSizeX/2.0f + j*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - i*fStepZ;
            pVertex->u = 0.0f + ( (float)j / dwWidth );
            pVertex->v = 0.0f + ( (float)i / dwLength );
            pVertex++;
        }
    }
	lplpVB=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( SIMPLEVERTEX ) * nNumVertex, STATIC, (const void *)pVertexBuffer);

    // Allocate memory for buffer of indices in system memory
    __WORD__*    pIndexBuffer = new __WORD__ [nNumIndex];
    __WORD__*    pIndex = &pIndexBuffer[0];

    // Fill index buffer
    for ( __DWORD__ i=0; i<dwLength; ++i )
    {
        for ( __DWORD__ j=0; j<dwWidth; ++j )
        {
            *pIndex++ = (__WORD__)(   i     * ( dwWidth+1 ) + j );
            *pIndex++ = (__WORD__)(   i     * ( dwWidth+1 ) + j + 1 );
            *pIndex++ = (__WORD__)( ( i+1 ) * ( dwWidth+1 ) + j );

            *pIndex++ = (__WORD__)( ( i+1 ) * ( dwWidth+1 ) + j );
            *pIndex++ = (__WORD__)(   i     * ( dwWidth+1 ) + j + 1 );
            *pIndex++ = (__WORD__)( ( i+1 ) * ( dwWidth+1 ) + j + 1 );
        }
    }
	lplpIB=IRenderer::GetRendererInstance()->addIndexBuffer(nNumIndex, 2, STATIC, (const void *)pIndexBuffer);

    // Release host memory index buffer
    delete [] pIndexBuffer;

    // Release host memory vertex buffer
    delete [] pVertexBuffer;
}


//--------------------------------------------------------------------------------------
// Helper function to calculate a normalized normal from a triangle
//--------------------------------------------------------------------------------------
D3DXFROMWINEVECTOR3 CalculateTriangleNormal( D3DXFROMWINEVECTOR3 *pP1, D3DXFROMWINEVECTOR3 *pP2, D3DXFROMWINEVECTOR3 *pP3 )
{
    D3DXFROMWINEVECTOR3 P21 = *pP2 - *pP1;
    D3DXFROMWINEVECTOR3 P31 = *pP3 - *pP1;

    D3DXFROMWINEVECTOR3 Normal;
    D3DXFROMWINEVec3Cross( &Normal, &P21, &P31 );
    D3DXFROMWINEVec3Normalize( &Normal, &Normal );

    return Normal;
}


//--------------------------------------------------------------------------------------
// FillGrid_WithNormals_Indexed
// Creates a regular grid of indexed triangles, including normals.
//
// Parameters:
//
// IN
// IRenderer::GetRendererInstance():               The D3D device
// dwWidth, dwLength:        Number of grid vertices in X and Z direction
// fGridSizeX, fGridSizeZ:   Grid extents in local space units
//
// OUT
// lplpVB:                   A pointer to the vertex buffer containing grid vertices
// lplpIB:                   A pointer to the index buffer containing grid indices
//--------------------------------------------------------------------------------------
void FillGrid_WithNormals_Indexed( __DWORD__ dwWidth, __DWORD__ dwLength, 
                                   float fGridSizeX, float fGridSizeZ, 
                                   VertexBufferID  lplpVB, VertexBufferID  lplpIB )
{
    int      hr;
    __DWORD__        nNumVertex = ( dwWidth + 1 ) * ( dwLength + 1 );
    __DWORD__        nNumIndex = 3 * 2 * dwWidth * dwLength;
    float        fStepX = fGridSizeX / dwWidth;
    float        fStepZ = fGridSizeZ / dwLength;
    
    // Allocate memory for buffer of vertices in system memory
    EXTENDEDVERTEX*    pVertexBuffer = new EXTENDEDVERTEX[nNumVertex];
    EXTENDEDVERTEX*    pVertex = &pVertexBuffer[0];

    // Fill vertex buffer
    for ( __DWORD__ i=0; i<=dwLength; ++i )
    {
        for ( __DWORD__ j=0; j<=dwWidth; ++j )
        {
            pVertex->x = -fGridSizeX/2.0f + j*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - i*fStepZ;
            pVertex->nx = 0.0f;
            pVertex->ny = 0.0f;
            pVertex->nz = 0.0f;
            pVertex->u = 0.0f + ( (float)j / dwWidth  );
            pVertex->v = 0.0f + ( (float)i / dwLength );
            pVertex++;
        }
    }

    // Allocate memory for buffer of indices in system memory
    __WORD__*    pIndexBuffer = new __WORD__ [nNumIndex];
    __WORD__*    pIndex = &pIndexBuffer[0];

    // Fill index buffer
    for ( __DWORD__ i=0; i<dwLength; ++i )
    {
        for ( __DWORD__ j=0; j<dwWidth; ++j )
        {
            *pIndex++ = (__WORD__)(   i     * ( dwWidth+1 ) + j );
            *pIndex++ = (__WORD__)(   i     * ( dwWidth+1 ) + j + 1 );
            *pIndex++ = (__WORD__)( ( i+1 ) * ( dwWidth+1 ) + j );

            *pIndex++ = (__WORD__)( ( i+1 ) * (dwWidth+1) + j );
            *pIndex++ = (__WORD__)(   i     * (dwWidth+1) + j + 1 );
            *pIndex++ = (__WORD__)( ( i+1 ) * (dwWidth+1) + j + 1 );
        }
    }
	lplpIB=IRenderer::GetRendererInstance()->addIndexBuffer(nNumIndex, 2, STATIC, (const void *)pIndexBuffer);

    // Write normals into vertex buffer
    pVertex = &pVertexBuffer[0];

    // Loop through all indices
    for ( __DWORD__ i=0; i<nNumIndex/3; ++i )
    {
        __WORD__ i1 = pIndexBuffer[3*i + 0];
        __WORD__ i2 = pIndexBuffer[3*i + 1];
        __WORD__ i3 = pIndexBuffer[3*i + 2];
        D3DXFROMWINEVECTOR3 Normal = CalculateTriangleNormal( (D3DXFROMWINEVECTOR3 *)&pVertexBuffer[i1].x, 
                                                      (D3DXFROMWINEVECTOR3 *)&pVertexBuffer[i2].x, 
                                                      (D3DXFROMWINEVECTOR3 *)&pVertexBuffer[i3].x );

        // Add normal to each vertex for this triangle
        *( (D3DXFROMWINEVECTOR3 *)&pVertexBuffer[i1].nx ) += Normal;
        *( (D3DXFROMWINEVECTOR3 *)&pVertexBuffer[i2].nx ) += Normal;
        *( (D3DXFROMWINEVECTOR3 *)&pVertexBuffer[i3].nx ) += Normal;
    }

    // Final normalization pass
    for ( __DWORD__ i=0; i<nNumVertex; ++i )
    {
        D3DXFROMWINEVec3Normalize( (D3DXFROMWINEVECTOR3 *)&pVertexBuffer[i].nx, (D3DXFROMWINEVECTOR3 *)&pVertexBuffer[i].nx );
    }


    // Set initial data info
    lplpVB=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( EXTENDEDVERTEX ) * nNumVertex, STATIC, (const void *)pVertexBuffer);

    // Release host memory index buffer
    delete [] pIndexBuffer;

    // Release host memory vertex buffer
    delete [] pVertexBuffer;

}


//--------------------------------------------------------------------------------------
// FillGrid_Quads_Indexed
// Creates a regular grid using indexed quads.
//
// Parameters:
//
// IN
// IRenderer::GetRendererInstance():                  The D3D device
// dwWidth, dwLength:           Number of grid vertices in X and Z direction
// fGridSizeX, fGridSizeZ:      Grid extents in local space units
//
// OUT
// lplpVB:                      A pointer to the vertex buffer containing grid vertices
//
//--------------------------------------------------------------------------------------
void FillGrid_Quads_Indexed( __DWORD__ dwWidth, __DWORD__ dwLength, 
                             float fGridSizeX, float fGridSizeZ, 
                             VertexBufferID  lplpVB, VertexBufferID  lplpIB )
{
    int      hr;
    __DWORD__        nNumVertex = ( dwWidth + 1 ) * ( dwLength + 1 );
    __DWORD__        nNumIndex = 4 * dwWidth * dwLength;
    float        fStepX = fGridSizeX / dwWidth;
    float        fStepZ = fGridSizeZ / dwLength;
    
    // Allocate memory for buffer of vertices in system memory
    SIMPLEVERTEX*    pVertexBuffer = new SIMPLEVERTEX[nNumVertex];
    SIMPLEVERTEX*    pVertex = &pVertexBuffer[0];

    // Fill vertex buffer
    for ( __DWORD__ i=0; i<=dwLength; ++i )
    {
        for ( __DWORD__ j=0; j<=dwWidth; ++j )
        {
            pVertex->x = -fGridSizeX/2.0f + j*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - i*fStepZ;
            pVertex->u = 0.0f + ( (float)j / dwWidth  );
            pVertex->v = 0.0f + ( (float)i / dwLength );
            pVertex++;
        }
    }

	lplpVB=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( SIMPLEVERTEX ) * nNumVertex, STATIC, (const void *)pVertexBuffer);

    // Release host memory vertex buffer
    delete [] pVertexBuffer;


    // Allocate memory for buffer of indices in system memory
    __WORD__* pIndexBuffer = new __WORD__ [nNumIndex];
    __WORD__* pIndex = &pIndexBuffer[0];

    // Fill index buffer
    for ( __DWORD__ i=0; i<dwLength; ++i )
    {
        for ( __DWORD__ j=0; j<dwWidth; ++j )
        {
            *pIndex++ = (__WORD__)(  i     * ( dwWidth+1 ) + j     );
            *pIndex++ = (__WORD__)(  i     * ( dwWidth+1 ) + j + 1 );
            *pIndex++ = (__WORD__)( ( i+1 )* ( dwWidth+1 ) + j + 1 );
            *pIndex++ = (__WORD__)( ( i+1 )* ( dwWidth+1 ) + j     );
        }
    }

	lplpIB=IRenderer::GetRendererInstance()->addIndexBuffer(nNumIndex, 2, STATIC, (const void *)pVertexBuffer);

    // Release host memory vertex buffer
    delete [] pIndexBuffer;
}

//--------------------------------------------------------------------------------------
// FillGrid_Quads_NonIndexed
// Creates a regular grid using non-indexed quads.
//
// Parameters:
//
// IN
// IRenderer::GetRendererInstance():                The D3D device
// dwWidth, dwLength:         Number of grid vertices in X and Z direction
// fGridSizeX, fGridSizeZ:    Grid extents in local space units
//
// OUT
// lplpVB:                    A pointer to the vertex buffer containing grid vertices
//
//--------------------------------------------------------------------------------------
void FillGrid_Quads_NonIndexed( __DWORD__ dwWidth, __DWORD__ dwLength, 
                                float fGridSizeX, float fGridSizeZ, 
                                VertexBufferID  lplpVB )
{
    int  hr;
    __DWORD__    nNumQuads = dwWidth * dwLength;
    __DWORD__    nNumVertex = 4 * nNumQuads;
    float    fStepX = fGridSizeX / dwWidth;
    float    fStepZ = fGridSizeZ / dwLength;
    
    // Allocate memory for buffer of vertices in system memory
    SIMPLEVERTEX*    pVertexBuffer = new SIMPLEVERTEX[nNumVertex];
    SIMPLEVERTEX*    pVertex = &pVertexBuffer[0];

    for ( __DWORD__ i=0; i<dwLength; ++i )
    {
        for ( __DWORD__ j=0; j<dwWidth; ++j )
        {
            // Vertex_ 0
            pVertex->x = -fGridSizeX/2.0f + j*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - i*fStepZ;
            pVertex->u = 0.0f + ( (float)j / dwWidth  );
            pVertex->v = 0.0f + ( (float)i / dwLength );
            pVertex++;

            // Vertex_ 1
            pVertex->x = -fGridSizeX/2.0f + (j+1)*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - i*fStepZ;
            pVertex->u = 0.0f + ( (float)(j+1)/ dwWidth);
            pVertex->v = 0.0f + ( (float) i   / dwLength);
            pVertex++;

            // Vertex_ 2
            pVertex->x = -fGridSizeX/2.0f + (j+1)*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - (i+1)*fStepZ;
            pVertex->u = 0.0f + ( (float)(j+1) / dwWidth  );
            pVertex->v = 0.0f + ( (float)(i+1) / dwLength );
            pVertex++;

            // Vertex_ 3
            pVertex->x = -fGridSizeX/2.0f + j*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - (i+1)*fStepZ;
            pVertex->u = 0.0f + ( (float) j   / dwWidth );
            pVertex->v = 0.0f + ( (float)(i+1)/ dwLength );
            pVertex++;
        }
    }
	lplpVB=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( SIMPLEVERTEX ) * nNumVertex, STATIC, (const void *)pVertexBuffer);

    // Release host memory vertex buffer
    delete [] pVertexBuffer;
}


//--------------------------------------------------------------------------------------
// FillGrid_Indexed_WithTangentSpace
// Creates a regular grid of indexed triangles
// Includes tangent space vectors: NORMAL, TANGENT & BINORMAL
//
// Parameters:
//
// IN
// IRenderer::GetRendererInstance():                   The D3D device
// dwWidth, dwLength:            Number of grid vertices in X and Z direction
// fGridSizeX, fGridSizeZ:       Grid extents in local space units
//
// OUT
// lplpVB:                       A pointer to the vertex buffer containing grid vertices
// lplpIB:                       A pointer to the index buffer containing grid indices
//--------------------------------------------------------------------------------------
void FillGrid_Indexed_WithTangentSpace( __DWORD__ dwWidth, __DWORD__ dwLength, 
                                        float fGridSizeX, float fGridSizeZ, 
                                        VertexBufferID  lplpVB, VertexBufferID  lplpIB )
{
    int hr;
    __DWORD__   nNumVertex = ( dwWidth + 1 ) * ( dwLength + 1 );
    __DWORD__   nNumIndex = 3 * 2 * dwWidth * dwLength;
    float   fStepX = fGridSizeX / dwWidth;
    float   fStepZ = fGridSizeZ / dwLength;
    
    // Allocate memory for buffer of vertices in system memory
    TANGENTSPACEVERTEX*    pVertexBuffer = new TANGENTSPACEVERTEX[nNumVertex];
    TANGENTSPACEVERTEX*    pVertex = &pVertexBuffer[0];

    // Fill vertex buffer
    for ( __DWORD__ i=0; i<=dwLength; ++i )
    {
        for ( __DWORD__ j=0; j<=dwWidth; ++j )
        {
            pVertex->x = -fGridSizeX/2.0f + j*fStepX;
            pVertex->y = 0.0f;
            pVertex->z = fGridSizeZ/2.0f - i*fStepZ;
            pVertex->u = 0.0f + ( (float)j / dwWidth  );
            pVertex->v = 0.0f + ( (float)i / dwLength );

            // Flat grid so tangent space is very basic; with more complex geometry 
            // you would have to export calculated tangent space vectors
            pVertex->nx = 0.0f;
            pVertex->ny = 1.0f;
            pVertex->nz = 0.0f;
            pVertex->bx = 0.0f;
            pVertex->by = 0.0f;
            pVertex->bz = -1.0f;
            pVertex->tx = 1.0f;
            pVertex->ty = 0.0f;
            pVertex->tz = 0.0f;
            pVertex++;
        }
    }
    
    // Allocate memory for buffer of indices in system memory
    __WORD__*    pIndexBuffer = new __WORD__ [nNumIndex];
    __WORD__*    pIndex = &pIndexBuffer[0];

    // Fill index buffer
    for ( __DWORD__ i=0; i<dwLength; ++i )
    {
        for ( __DWORD__ j=0; j<dwWidth; ++j )
        {
            *pIndex++ = (__WORD__)(  i    * (dwWidth+1) + j     );
            *pIndex++ = (__WORD__)(  i    * (dwWidth+1) + j + 1 );
            *pIndex++ = (__WORD__)( (i+1) * (dwWidth+1) + j     );

            *pIndex++ = (__WORD__)( (i+1) * (dwWidth+1) + j     );
            *pIndex++ = (__WORD__)(  i    * (dwWidth+1) + j + 1 );
            *pIndex++ = (__WORD__)( (i+1) * (dwWidth+1) + j + 1 );
        }
    }

#ifdef GRID_OPTIMIZE_INDICES
    GridOptimizeIndices(pIndexBuffer, nNumIndex, nNumVertex);
#endif

#ifdef GRID_OPTIMIZE_VERTICES
    GridOptimizeVertices(pIndexBuffer, pVertexBuffer, sizeof(TANGENTSPACEVERTEX), 
                         nNumIndex, nNumVertex);
#endif


    // Actual VB creation
    
	lplpVB=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( TANGENTSPACEVERTEX ) * nNumVertex, STATIC, (const void *)pVertexBuffer);

    // Actual IB creation

	lplpIB=IRenderer::GetRendererInstance()->addIndexBuffer(nNumIndex, 2, STATIC, (const void *)pIndexBuffer);

    // Release host memory vertex buffer
    delete [] pIndexBuffer;

    // Release host memory vertex buffer
    delete [] pVertexBuffer;
}


//--------------------------------------------------------------------------------------
// Optimize grid indices for post-vertex cache 
//--------------------------------------------------------------------------------------
void GridOptimizeIndices( __WORD__* pIndexBuffer, int nNumIndex, int nNumVertex )
{
    // Optimize faces for post-transform cache
    __DWORD__* pRemappedFaces = new __DWORD__ [nNumIndex/3];
    D3DXFROMWINEOptimizeFaces( pIndexBuffer, nNumIndex/3, nNumVertex, FALSE, pRemappedFaces );

    // Allocate temporary index buffer and copy current indices into it
    __WORD__* pTmpIndexBuffer = new __WORD__ [nNumIndex];
    stx_memcpy( pTmpIndexBuffer, pIndexBuffer, nNumIndex * sizeof( __WORD__ ) );
    
    // Remap triangles
    for ( int i=0; i < (int)nNumIndex/3; ++i )
    {
        int newFace = (int)pRemappedFaces[i];
        for ( int j=0; j < 3; ++j )
        {
            pIndexBuffer[newFace*3+j] = pTmpIndexBuffer[i*3+j];
        }
    }
    delete[] pTmpIndexBuffer;
    delete[] pRemappedFaces;
}


//--------------------------------------------------------------------------------------
// Optimize grid vertices and indices for pre-vertex cache 
//--------------------------------------------------------------------------------------
void GridOptimizeVertices( __WORD__* pIndexBuffer, void *pVertexBuffer, __DWORD__ dwVertexSize, 
                           int nNumIndex, int nNumVertex )
{
    // Optimize vertices for pre-transform cache
    __DWORD__* pRemappedVertices = new __DWORD__ [nNumVertex];
    D3DXFROMWINEOptimizeVertices( pIndexBuffer, nNumIndex/3, nNumVertex, FALSE, pRemappedVertices );

    // Allocate temporary vertex buffer and copy current vertices into it
    BYTE* pTmpVertexBuffer = new BYTE [nNumVertex * dwVertexSize];
    stx_memcpy( pTmpVertexBuffer, (BYTE *)pVertexBuffer, nNumVertex*dwVertexSize );

    // Remap vertices
    for ( int i=0; i < (int)nNumVertex; ++i )
    {
        int newVertexIndex = (int)pRemappedVertices[i];
        
        stx_memcpy( (BYTE *)pVertexBuffer+newVertexIndex*dwVertexSize, 
                (BYTE *)pTmpVertexBuffer + (i*dwVertexSize), 
                dwVertexSize );
    }

    // Remap indices
    for ( int i=0; i < (int)nNumIndex; ++i )
    {
        pIndexBuffer[i] = (__WORD__)pRemappedVertices[pIndexBuffer[i]];
    }

    delete[] pTmpVertexBuffer;
    delete[] pRemappedVertices;
}

