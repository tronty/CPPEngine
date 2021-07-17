#ifndef TERRAIN_H
#define TERRAIN_H
#include <Framework3/camera3.h>

class Terrain
{
  public:
    Terrain();
   ~Terrain();

    D3DXFROMWINEVECTOR4 &renderSimple(float elapsedTime);
    D3DXFROMWINEVECTOR4 &render(Camera3   &camera,
                    const Frustum &frustum,
                    float elapsedTime );

    bool  initialize(bool compile = false);

    const void adjustCameraYLocation(Camera3 *camera, float cameraRadius);

    void  setOcclusionCullingFlag(bool occCull);
    void  setAlphaToCoverageFlag(bool alpha);
    void  setFrustumCullingFlag(bool frCull);
    void  setRenderAABBFlag(bool  rAABBs);
    void  setWireFrameFlag(bool  wFrame);
    void  setDrawGrassFlag(bool  dGrass);
    void  setDrawTerrainFlag(bool  dTerrain);
    void  setDrawTBNFlag(bool  dGrass);

    void  setAlphaReference(float refrence);
    void  setAlphaBooster(float boost);

    const float getAlphaReference();
    const float getAlphaBooster();

    const bool  getOcclusionCullingFlag();
    const bool  getAlphaToCoverageFlag();
    const bool  getFrustumCullingFlag();
    const bool  getRenderAABBFlag();
    const bool  getWireFrameFlag();
    const bool  getDrawGrassFlag();
    const bool  getDrawTerrainFlag();
    const bool  getDrawTBNFlag();

    const Texture &getDiffuse();
    const Texture &getDetail();
    const ShaderID &getShader();
 
    
  private:
    void  createRandomMap(int w, int h, Image3 *image);
    void  resetRenderingRoutine();
    bool  appendToGreenChannel(Image3 *dst, const  Image3 *src);
    bool  Channel(Image3 *dst, const  Image3 *src, const unsigned int N);

    D3DXFROMWINEMATRIX  prevView;
    D3DXFROMWINEVECTOR4   info;

  public:
    TextureID   grassPack,
              fungus,
              weight,
              weightR,
              weightG,
              weightB,
              grass,
              dirt, tree1, tree2, tree3;

    ShaderID   terrainShader,
              grassShader;

    VertexFormatID   terrainVF,
              grassVF;
  private:
    LPPortalManager  portalsManager;
    Portal          *portals;

    HeightMap    terrainInfo;
   public:
    float        alphaReference,
                 alphaBooster;
    bool         drawGrass,
                 drawTerrain;
  private:
    bool         occlusionCullCells,
                 frustumCullCells,
                 renderCellsAABBs,
                 alphaToCoverage,
                 queryClock,
                 wireFrame,
                 drawTBN,
                 dirty;
};
#endif
