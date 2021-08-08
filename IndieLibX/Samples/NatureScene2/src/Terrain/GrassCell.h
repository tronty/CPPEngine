#ifndef GRASS_CELL_H
#define GRASS_CELL_H

#include <Framework3/IRenderer.h>
#include "HeightMap.h"

typedef struct grassVertex
{
  D3DXFROMWINEVECTOR3 vertex,normal,offset,tangent,binormal;
  D3DXFROMWINEVECTOR3 uvJitter;
} GVertex;
/*
typedef struct gBlock
{
  GVertex vertices[12];
} GrassBlock;
*/
class GrassCell
{
  public:
    GrassCell();
   ~GrassCell();

    bool  setup(const HeightMap &distribution,
                const D3DXFROMWINEVECTOR2   &offset,
                const Image3     *randommap,
				const Image3     *coveragemap);

    const bool            render();
    const D3DXFROMWINEVECTOR2        &getVerticalBounds();
    const unsigned int    getTriangleCount();
    const GVertex *getVertexStream();
    const __WORD__ *getIndexStream();
  private:
    unsigned int    blockCount, m_iTriangleCount;
    
    std::vector<GVertex> grass;
    std::vector<__WORD__> indices;
    D3DXFROMWINEVECTOR2         verticalBounds;
};

#endif

