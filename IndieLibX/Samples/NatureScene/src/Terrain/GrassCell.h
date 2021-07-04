#ifndef GRASS_CELL_H
#define GRASS_CELL_H

#include "TerrainUtils.h"
#include "TerrainCell.h"

typedef struct grassVertex
{
  D3DXFROMWINEVECTOR3 vertex,
          normal,
          offset,
          tangent,
	  binormal;
  D3DXFROMWINEVECTOR3 uvJitter;
} GVertex;

typedef struct gBlock
{
  GVertex vertices[12];
} GrassBlock;

class GrassCell
{
  public:
    GrassCell();
   ~GrassCell();

    bool  setup(const HeightMap &distribution,
                const Tuple2i   &offset,
                const Image3     &randommap,
				const Image3     &coveragemap,ShaderID sha);

    const bool            render(ShaderID sha);
    const D3DXFROMWINEVECTOR2        &getVerticalBounds();
    const unsigned int    getGeometryBufferID();
    const unsigned int    getIndexBufferID();
    const unsigned int    getTriangleCount();
    const __WORD__ *	  getIndexStream();
  private:
    unsigned int    blockCount,
                    indexCount;
    __WORD__ *indices;
    D3DXFROMWINEVECTOR2         verticalBounds;
	VertexBufferID geometryBufferID;
	int m_NumVert, m_NumTris;
public:
		static IndexBufferID indexBufferID;
	static VertexFormatID m_pVertexDeclaration;
	ShaderID m_pShader;
};

#endif
