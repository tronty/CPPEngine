#ifndef __CRENDERER_HPP__
#define __CRENDERER_HPP__

/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

#define TEXTUREWIDTH 512

extern float rRotationX;
extern float rRotationY;
extern float CamZ, CamY, CamX;
extern float Weather;

struct TTexParam {
    stx_Effect::stx_Variable* tmp[20];
    int Count;
    TTexParam() { Count = 0;}
    stx_Effect::stx_Variable* Add(stx_Effect::stx_Variable* p, int TexID = 0) { 
      if (TexID != 0) cgGLSetTextureParameter(p, TexID);
      tmp[Count++] = p;
    }
    void Enable(int bEnable) {
      for (int k = 0; k < Count; k++)
        if (bEnable) cgGLEnableTextureParameter(tmp[k]);
                else cgGLDisableTextureParameter(tmp[k]);
        
    }
    
};


class CRenderer
{
public:
  CRenderer() {};

  void Initialize();
  void Render(int bReflection);
  unsigned int GetReflID() {return WaterReflID;}

/*
  void Update();
  void Reset();
  void Shutdown();
*/

private:  
  stx_Effect::stx_Technique* program;
  struct {
    //stx_Effect::stx_Variable* environmentMap;
    //stx_Effect::stx_Variable* NMap, NMap1, WRefl, Freshel;
    stx_Effect::stx_Variable* c[10];
    stx_Effect::stx_Variable* dXYMap, dXYMap1;
    stx_Effect::stx_Variable* EnvCMap, FoamMap;
  } fpVars;

  struct {
    stx_Effect::stx_Variable* VOfs, CPos;
    stx_Effect::stx_Variable* Gabarites;
    stx_Effect::stx_Variable* HMap0, HMap1;
  } vpVars;

  void RenderSky();
  void RenderSea();
  void RenderIsland();
  


  void CreateNoiseTexture();
  float PRNGenerator(int x);
  unsigned int m_TextureID, Cube0ID, NMapID[16], LandCID, WaterReflID, FreshelID, NoiseID;
  unsigned float m_pfNoiseTextureMap[TEXTUREWIDTH][TEXTUREWIDTH];
  unsigned int FoamID, WaterReflDMID[128], WaterRefldXYID[128];
  
  float m_LightPos[3];
  float m_specExp;
};

#endif // __CRENDERER_HPP__

