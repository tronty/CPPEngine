#ifndef TERRAIN_H
#define TERRAIN_H

#include <Appearance/Appearance.h>
//include "../Tools/FPSCounter.h"
#include <Viewer/Frustum.h>
#include <Viewer/Camera.h>
#include "HeightMap.h"
#include "Portal.h"


typedef DistanceObject<Portal*> LPPortal;
typedef vector<LPPortal>        LPPortalManager;
class Terrain
{
  public:
    bool portalsCompstd(const LPPortal & s0, const LPPortal &s1) const;
    Terrain();
   ~Terrain();

    Tuple4i &renderSimple(float elapsedTime);
    Tuple4i &render(//const Camera   &camera,
                    const Frustum &frustum,
                    float elapsedTime );

    bool  initialize(bool compile = false);

    const void adjustCameraYLocation(NSCamera &camera, float cameraRadius);

    void  clearFlags();

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

    const TextureID &getDiffuse();
    const TextureID &getDetail();
    const TextureID &getWeightTexture(){return weight;}
    const TextureID &getFungusTexture(){return fungus;}
    const TextureID &getDirtTexture(){return dirt;}
    const ShaderID &getTerrainShader(){return terrainShader;}
    const ShaderID &getGrassShader(){return grassShader;}
    
  private:
    void  createRandomMap(int w, int h, Image3 &image);
    void  resetRenderingRoutine();
    bool  appendToGreenChannel(Image3 &dst, const  Image3 &src);

    D3DXFROMWINEMATRIX  prevView;
    Tuple4i   info;

    TextureID  fungus,
              weight,
              dirt;

    ShaderID terrainShader;
    ShaderID grassShader;

    LPPortalManager  portalsManager;
    Portal          *portals;

    HeightMap    terrainInfo;
  
    float        alphaReference,
                 alphaBooster;

    bool  	 wireFrame;

    bool         occlusionCullCells,
                 frustumCullCells,
                 renderCellsAABBs,
                 alphaToCoverage,
                 queryClock,
                 drawGrass,
                 drawTerrain,
                 drawTBN,
                 dirty;
};
#endif
