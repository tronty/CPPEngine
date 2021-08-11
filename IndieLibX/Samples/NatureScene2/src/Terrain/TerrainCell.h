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
                const D3DXFROMWINEVECTOR2&   start);
   
    const TVertex*  getVertexStream() const;
    const D3DXFROMWINEVECTOR2      &getVerticalBounds();
    const bool          render();

    static const __WORD__ *getIndexStream();
    static void setupIndices(bool compile);
    const unsigned int getTriangleCount();

  private:
    std::vector<TVertex>     	 vertices;
    D3DXFROMWINEVECTOR2          verticalBounds;
    unsigned int m_iTriangleCount;

    static unsigned int   userCount;
    static std::vector<__WORD__> indices;
};

#endif
