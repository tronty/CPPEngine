#ifndef HEIGHT_MAP_H
#define HEIGHT_MAP_H

typedef struct heightmapVertex
{
  D3DXFROMWINEVECTOR3 vertex, normal, tangent, binormal;
  D3DXFROMWINEVECTOR2 uv;
} HMVertex;

class HeightMap
{
  public:
    HeightMap();
   ~HeightMap();

    bool  setup(const Image3*  heightmap,
                const Image3*  watermap);
     
    const HMVertex *getVertexStream()  const;
    
    bool  buildWeightMap(Image3 & image);
    const float getInterpolatedHeight(const D3DXFROMWINEVECTOR3 &location) const;
    const int   getIndexCount() const;
    const int   getHeight()     const;
    const int   getWidth()      const;
    bool render();

  private:
    std::vector<HMVertex> vertices;
    std::vector<__WORD__> indices;
    void      computeBTangents();
    int       height,
              width;
};

#endif

