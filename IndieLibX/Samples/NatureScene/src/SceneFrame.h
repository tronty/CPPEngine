#ifndef SCENEFRAME_H
#define SCENEFRAME_H

//include "../Renderer/FrameBufferObject.h"
//include "../GUI/InputEventListener.h"
#include <GUI/GUIEventListener.h>
#include <Nodes/TransformGroup.h>
//include "../Renderer/Renderer.h"
//include "../Tools/Benchmark.h"
//include "../Window/Window.h"

#include <GUI/GUIUtils.h>
#include <Terrain/Terrain.h>
#include <Viewer/Camera.h>

#define __ALL__ 1

class SceneFrame : public STXGUI, public IOXMLObject, public GUIEventListener//, public InputEventListener
{
  int windowEventsProcessor();

  public:

    SceneFrame(int instance = 0);
   ~SceneFrame();

    bool initialize();
    void destroy();
    void run();
    void run2();

    void setActive(bool act){ active = act;  }
    bool isActive()         { return active; }

    virtual bool loadXMLSettings(XMLElement *element);
    virtual bool exportXMLSettings(ofstream &xmlFile){return true; }

    const TextureID &getGrassTexture(){return grass;}
    const TextureID &getGrassPackTexture(){return grassPack;}

#if 0
    virtual void windowPositionChanged(){setPerspective(evt);};
    virtual void windowSizeChanged    (){setPerspective(evt);};
    virtual void windowMaximized      (){setPerspective(evt);};
    virtual void windowMinimized      (){setPerspective(evt);};
    virtual void windowClosing        (){};
#endif
    virtual void mouseDoubleClicked();
    virtual void mouseScrolled     (){};
    virtual void mouseReleased     ();
    virtual void mouseClicked      ();
    virtual void mouseDragged      ();
    virtual void mouseMoved        ();
    virtual void mouseExit         (){};

    virtual void keyReleased();
    virtual void keyPressed ();

    virtual void actionPerformed(GUIEvent &evt);

  private:
#ifdef __ALL__
    int    renderWaterSurface(float timer);
    void   updateWaterSurface(float timer);
#endif
    void   setPerspective(int evtx, int evty, int evtWidth, int evtHeight);
    void   updateClipPlanes(float nearClip, float farClip);
    int    renderSkyDome();

    //Benchmark        benchmark;
    GUIFrame         guiFrame;

    GUIPanel        *userControls,
                    *statistics;

    GUILabel        *cellCounter, 
                    *triCounter,
                    *fpsCounter;

    TextureID       waterDepth, randommap, m_pTexture;

    TextureID grass;
    TextureID grassPack;
    TextureID noise;
    ShaderID skyShader;
  public:
    const ShaderID &getSkyShader() const {return  skyShader;}
#ifdef __ALL__
    const ShaderID &getWaterShader() const {return  waterShader;}
#endif
  private:

#ifdef __ALL__
    ShaderID           waterShader;
    TextureID           waterTexture;
	TransformGroup   waterGroup;
	    D3DXFROMWINEMATRIX         waterMatrix;
#endif

    D3DXFROMWINEVECTOR4 skyInfo,terrainInfo,translation;

    Tuple2i          mouseLockedPosition;

    TransformGroup   skyDome;
    Terrain*          terrain;
  public:                    
    Terrain* GetTerrain(){return terrain;}

    bool  getWFlag() const {return bW;}
    bool  getVPFlag() const {return bVP;}
    bool  getPVFlag() const {return bPV;}
    bool  getInverseFlag() const {return bInverse;}
    bool  getTransposeFlag() const {return bTranspose;}
    bool  getIdentityFlag() const {return bIdentity;}
    void  setDrawSkyFlag(bool  dSky){drawSky=dSky;}
    const bool  getDrawSkyFlag() const {return drawSky;}
    void  setDrawWaterFlag(bool  dWater){renderWater=dWater;}
    const bool  getDrawWaterFlag() const {return renderWater;}
    void  setWFlag(bool a){bW=a;}
    void  setPVFlag(bool a){bPV=a;}
    void  setVPFlag(bool a){bVP=a;}
    void  setInverseFlag(bool a){bInverse=a;}
    void  setTransposeFlag(bool a){bTranspose=a;}
    void  setIdentityFlag(bool a){bIdentity=a;}
    NSCamera GetCamera() const {return camera;}
    D3DXFROMWINEMATRIX GetProj() const {return matProj;}
    static D3DXFROMWINEMATRIX matProj;
    bool getSkyWireFrameFlag(){return bSkyWireFrameFlag;}
  private:
    bool bSkyWireFrameFlag;
    D3DXFROMWINEVECTOR3 eye, look, up;
    bool         bTranspose, bIdentity, bInverse;
    bool         bVP,bPV,bW;
    bool         drawSky, renderWater;
    Frustum          frustum;
    NSCamera           camera;

    //Window           window;

    bool             mouseLocked,
                     paused,
                     active;
    unsigned int waterTextureWidth, waterTextureHeight;
};
extern SceneFrame* g_SceneFrame;
#endif
