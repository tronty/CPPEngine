#ifndef SCENEFRAME_H
#define SCENEFRAME_H

#define USEGROUPS 1

#include "Terrain/TerrainUtils.h"
#include <Framework3/camera3.h>

struct WaterMesh;
struct SkyMesh;

class SceneFrame : public STXGUI //, public IOXMLObject
{


  public:
    SceneFrame();
   ~SceneFrame();

    bool initialize();
    void destroy();
    bool run();

    void setActive(bool act){ active = act;  }
    bool isActive()         { return active; }

    virtual void actionPerformed(GUIEvent &evt);
  //private:
    int    renderWaterSurface(float timer);
    void   setPerspective(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
    void   updateWaterSurface(float timer);
    void   updateClipPlanes(float nearClip, float farClip);
    int    renderSkyDome();

    /*
	Benchmark        benchmark;
	

    GUIPanel        *userControls,
                    *statistics;
	*/

    GUILabel        *cellCounter, 
                    *triCounter,
                    *fpsCounter;

  public:
    Terrain          terrain;
    //NSCamera            camera;
    Camera3		camera;
    void DrawAtlas();
    TextureID           waterTexture, skyTexture,
                      waterDepth;
	TextureID normalmap;

    ShaderID           waterShader,
                      skyShader;

    VertexFormatID           waterVF,
                      skyVF;
    bool             renderWater, renderSky, renderTextures;
  //private:

    D3DXFROMWINEVECTOR4          skyInfo;

    D3DXFROMWINEVECTOR2          mouseLockedPosition;

    //TransformGroup   waterGroup, skyDome;
                    

	#ifdef USEGROUPS
    WaterMesh*       m_pWaterMesh;
    SkyMesh*         m_pSkyMesh;
	#endif
    Frustum          frustum;
    D3DXFROMWINEMATRIX         waterMatrix;

    bool             paused,
                     active;
};
extern SceneFrame* g_SceneFrame;
#include "Terrain/SkyMesh.h"
#include "Terrain/WaterMesh.h"
#endif

