#ifndef TERRAIN_CELL_H
#define TERRAIN_CELL_H

#include "TerrainUtils.h"
#include "HeightMap.h"

#ifndef OFFSET
  #define OFFSET(x) ((char *)0+(x))
#endif

typedef struct terrainVertex
{
  D3DXFROMWINEVECTOR3 vertex, normal;
  D3DXFROMWINEVECTOR2 uv;
} TVertex;

class TerrainCell
{
  public:
    TerrainCell();
   ~TerrainCell();

    bool  setup(const HeightMap& heightMap,
                const Tuple2i&   start,ShaderID sha);
   
    const unsigned int  getGeometryBufferID() const;
    const D3DXFROMWINEVECTOR2      &getVerticalBounds();
    const bool          render(ShaderID sha);

    static const __WORD__ *		 getIndexStream();
    static const unsigned int    getIndexBufferID();
    static void                  setupIndices(bool compile);

  private:
    VertexBufferID     		geometryBufferID;
    D3DXFROMWINEVECTOR2          verticalBounds;

    static unsigned int    userCount;
    static IndexBufferID   indexBufferID;
    static __WORD__ indices[INDEX_COUNT];
	VertexFormatID m_pVertexDeclaration;
	int m_NumVert;
	static int m_NumTris;
	ShaderID m_pShader;
};

#endif
