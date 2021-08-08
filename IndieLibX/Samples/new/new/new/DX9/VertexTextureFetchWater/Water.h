/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#pragma once

#include "Grid.h"
#include "HeightMap.h"
#include "Vertex.h"
#include "Viewer.h"

#include <string>
#include <vector>
#include <windows.h>


#define CHECK_RETURN_CODE(text, hr) \
if(FAILED((hr)))\
{\
  return(DXTRACE_ERR((text), (hr)));\
}\


////////////////////////////////////////////////////////////////////////////////
//
//                         Water class declaration
//
////////////////////////////////////////////////////////////////////////////////
class Water
{
public:
                          Water(void);
  virtual                ~Water();

  static VertexBufferID m_quadVertexBuffer;

protected:
  enum RenderSelection
    {
      RENDER_LAND        = 0x00000001,
      RENDER_BOAT        = 0x00000002,
      RENDER_SUNKEN_BOAT = 0x00000004,
      RENDER_ALL         = 0xFFFFFFFF,
    };

  static Water*           m_Water;

private:
  BOOL                     m_loadingApp;           // TRUE, if the app is loading
  int                      m_deviceWidth;
  int                      m_deviceHeight;

  D3DXFROMWINEMATRIX               m_projectionMatrix;
  D3DXFROMWINEMATRIX               m_reflectionProjectionMatrix;
  D3DXFROMWINEMATRIX               m_refractionProjectionMatrix;
  D3DXFROMWINEMATRIX               m_worldViewProjectionInverseTransposeMatrix;

  D3DXFROMWINEVECTOR3              m_lightPositionWorld;

  TextureID       m_waveSimulationTexture[3];
  SurfaceID       m_waveSimulationSurface[3];
  TextureID       m_waveSimulationVertexShaderTexture;
  SurfaceID       m_waveSimulationVertexShaderSurface;
  TextureID       m_waveSimulationDampeningTexture;
  Grid                     m_waveSimulationGrid;
  VertexBufferID  m_waveSimulationVertexBuffer;
  IndexBufferID   m_waveSimulationIndexBuffer;
  ShaderID  m_waveSimulationVertexShader;
  ShaderID   m_waveSimulationPixelShader;
  ShaderID  m_waveSimulationPerturbationVertexShader;
  ShaderID   m_waveSimulationPerturbationPixelShader;
  int                      m_waveSimulationIndex;
  int                      m_waveSimulationWidth;
  int                      m_waveSimulationHeight;

  int                      m_reflectionTextureWidth;
  int                      m_reflectionTextureHeight;
  int                      m_refractionTextureWidth;
  int                      m_refractionTextureHeight;
  TextureID       m_waveReflectionTexture;
  SurfaceID       m_waveReflectionSurface;
  TextureID       m_waveRefractionTexture;
  SurfaceID       m_waveRefractionSurface;
  TextureID       m_waveReflectionNearTexture;
  SurfaceID       m_waveReflectionNearSurface;
  TextureID       m_waveRefractionNearTexture;
  SurfaceID       m_waveRefractionNearSurface;
  ShaderID   m_waveRenderingPixelShader;
  ShaderID   m_waveRenderingRefractionPixelShader;
  ShaderID   m_waveRenderingReflectionPixelShader;
  ShaderID   m_waveRenderingFresnelPixelShader;
  ShaderID   m_waveRenderingNormalsPixelShader;
  ShaderID  m_waveRenderingVertexShader;
  TextureID       m_fadeTexture;


  SurfaceID       m_backBufferRenderTarget;

  TextureID       m_underwaterWorldRenderTargetTexture;
  SurfaceID       m_underwaterWorldRenderTargetSurface;
  ShaderID   m_underwaterPixelShader;

  ShaderID  m_causticVertexShader;
  ShaderID   m_causticPixelShader;

  ShaderID  m_diffuseVertexShader;
  ShaderID   m_diffusePixelShader;

  ShaderID  m_singleTextureVertexShader;
  ShaderID   m_singleTexturePixelShader;
  
  TextureID       m_skyBoxTextures[6];

  TextureID       m_wakeTexture;

  LPD3DXMATRIXSTACK        m_worldMatrixStack;
  LPD3DXMATRIXSTACK        m_viewMatrixStack;
  LPD3DXMATRIXSTACK        m_projectionMatrixStack;
  bool                     m_reflectViewMatrix;
  
  HeightMap*               m_land;
  TextureID       m_landTexture;
  float                    m_landScale[3];

  Mesh                     m_boatMesh;
  Mesh                     m_sunkenBoatMesh;
  TextureID       m_boatTexture;

  Viewer                   m_viewer;
  bool                     m_walkOnGround;
  float                    m_time;
  float                    m_tLastFrame;
  float                    m_dt; 
  float                    m_fieldOfView;
  float                    m_height;
  D3DXFROMWINEPLANE                m_refractionClipPlaneAboveWater;
  D3DXFROMWINEPLANE                m_reflectionClipPlaneAboveWater;
  D3DXFROMWINEPLANE                m_refractionClipPlaneBelowWater;
  D3DXFROMWINEPLANE                m_reflectionClipPlaneBelowWater;

  float                    m_aspectRatio;
  float                    m_refractionMapOverdraw;
  float                    m_reflectionMapOverdraw;
  bool                     m_underwater;
  bool                     m_mouseIsClipped;
  RECT                     m_mouseClipRect;
  int                      m_textureOverlayIndex;
  bool                     m_displayHelp;
  bool                     m_freezeSimulation;
  bool                     m_stopPertubingWater;
  bool                     m_enableWaterGhost;
  bool                     m_renderRefractionOnly;
  bool                     m_renderReflectionOnly;
  bool                     m_renderFresnelOnly;
  bool                     m_renderNormalsOnly;
  bool                     m_renderWireframeWater;
  bool                     m_renderText;
  bool                     m_renderGUI;
  bool                     m_renderSunkenBoat;
  D3DCOLOR                 m_fontColor;
  POINT                    m_fontPosition;
  std::vector<std::string> m_path;

  enum GUI_IDS
    {
      IDC_TOGGLE_FULLSCREEN,
      IDC_TOGGLE_REF,
      IDC_CHANGE_DEVICE,
      IDC_RENDER_MODE,
      IDC_RENDER_MODE_STATIC,
      IDC_INTERMEDIATE_RENDER_TARGET,
      IDC_INTERMEDIATE_RENDER_TARGET_STATIC,
      IDC_ENABLE_SIMULATION,
      IDC_ENABLE_PERTURBATION,
      IDC_ENABLE_WATER_GHOST,
      IDC_WIREFRAME_WATER,
      IDC_WALK_ON_GROUND
    };
};



//
// Global function prototypes
//
int      dprintf(const char* formatString, ...);
void     dprintf(const D3DXFROMWINEMATRIX& matrix);
void     dprintf(const char* description, const D3DXFROMWINEMATRIX& matrix);
int  drawQuad( device, const D3DXFROMWINEVECTOR3& position0, D3DCOLOR color0, const D3DXFROMWINEVECTOR2& texCoord0,
                  const D3DXFROMWINEVECTOR3& position1, D3DCOLOR color1, const D3DXFROMWINEVECTOR2& texCoord1,
                  const D3DXFROMWINEVECTOR3& position2, D3DCOLOR color2, const D3DXFROMWINEVECTOR2& texCoord2,
                  const D3DXFROMWINEVECTOR3& position3, D3DCOLOR color3, const D3DXFROMWINEVECTOR2& texCoord3);
void     fatalError(const char* formatString, ...);
D3DCOLOR randomColor(void);
char*   toUnicode(const char* string);
char*    fromUnicode(const char* string);
