/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

////////////////////////////////////////////////////////////////////////////////
//
//                              Questions?
//  Author: Jeremy Zelsnack
//  Contact sdkfeedback@nvidia.com
//
////////////////////////////////////////////////////////////////////////////////

// The Dec 2004 and Feb 2005 DXSDK's seem to have a bug that prevents the
// vertex shader from compiling.
#define FXC_WORK_AROUND 1

#include "dxstdafx.h"

#include "Mesh.h"
#include "registers.h"
#include "Vertex.h"
#include "Viewer.h"
#include "Water.h"

// Static Water variables
VertexBufferID Water::m_quadVertexBuffer = 0;
Water* Water::m_Water = 0;

//-----------------------------------------------------------------------------
// Name: Water()
// Desc: Application constructor.   Paired with ~Water()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D IRenderer::GetRendererInstance() 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
Water::Water(void)
{
  // Record this to a static variable. Only 1 instance of Water can exist.
  assert(m_Water == 0);
  m_Water = this;

  // Initialize with default values
  m_loadingApp                        = TRUE;
  m_deviceWidth                       = 1;
  m_deviceHeight                      = 1;
  m_land                              = 0;
  m_height                            = 5.0f;
  m_walkOnGround                      = false;
  m_underwater                        = false;
  m_waveSimulationWidth               = 128;
  m_waveSimulationHeight              = 128;
  m_waveSimulationIndex               = 0;
  m_mouseIsClipped                    = false;
  m_textureOverlayIndex               = 0;
  m_displayHelp                       = false;
  m_freezeSimulation                  = false;
  m_stopPertubingWater                = false;
  m_enableWaterGhost                  = true;
  m_renderRefractionOnly              = false;
  m_renderReflectionOnly              = false;
  m_renderFresnelOnly                 = false;
  m_renderNormalsOnly                 = false;
  m_renderWireframeWater              = false;
  m_renderText                        = true;
  m_renderGUI                         = true;
  m_renderSunkenBoat                  = true;
  m_refractionMapOverdraw             = 1.25f;
  m_reflectionMapOverdraw             = 1.5f;
  m_reflectionTextureWidth            = 512;
  m_reflectionTextureHeight           = 512;
  m_refractionTextureWidth            = 512;
  m_refractionTextureHeight           = 512;
  m_fontColor                         = D3DCOLOR_ARGB(255, 255, 255, 127);
  m_fontPosition.x                    = 0;
  m_fontPosition.y                    = 0;
  m_backBufferRenderTarget            = 0;
  m_aspectRatio                       = 1.0f;
  m_fieldOfView                       = 60.0f * D3DXFROMWINE_PI / 180.0f; 
  m_time                              = 0.0f;
  m_tLastFrame                        = -1.0f;
  m_dt                                = 0.0f;


  // Initialize clip planes
  m_refractionClipPlaneAboveWater = D3DXFROMWINEPLANE(0.0f, -1.0f, 0.0f, 0.5f);
  m_reflectionClipPlaneAboveWater = D3DXFROMWINEPLANE(0.0f,  1.0f, 0.0f, 0.5f);
  m_refractionClipPlaneBelowWater = D3DXFROMWINEPLANE(0.0f,  1.0f, 0.0f, 0.5f);
  m_reflectionClipPlaneBelowWater = D3DXFROMWINEPLANE(0.0f, -1.0f, 0.0f, 0.0f);


  // Set up the path
  m_path.push_back(std::string("./"));
  m_path.push_back(std::string("src/VertexTextureFetchWater/"));
  m_path.push_back(std::string("DEMOS/Direct3D9/src/VertexTextureFetchWater/"));
  m_path.push_back(std::string("MEDIA/programs/VertexTextureFetchWater/"));
  m_path.push_back(std::string("MEDIA/models/"));
  m_path.push_back(std::string("MEDIA/textures/2D/"));

  // Set up the GUI
  InitGUI();
}

//-----------------------------------------------------------------------------
// Name: ~Water()
// Desc: Application destructor.  Paired with Water()
//-----------------------------------------------------------------------------
Water::~Water()
{
  OnLostDevice();
  OnDestroyDevice();
}





////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//
//               Callbacks for 9.0c sample framework
//
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//
//                       Water's own methods
//
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
// 
// Copies the simulation texture from fp16 texture to fp32 texture. This is
// necessary because vertex texture fetch only works on fp32 formats.
//        
//-----------------------------------------------------------------------------
void Water::CopySimulationTexture(TextureID simulationSourceTexture, SurfaceID currentRenderTarget)
{
  D3DXFROMWINEMATRIX identity;
  int    hr;

  D3DXFROMWINEMatrixIdentity(&identity);

  // Copy the simulation from the D3DFMT_A16B16G16R16F texture to D3DFMT_A32B32G32R32F texture
  hr = IRenderer::GetRendererInstance()->SetRenderTarget(0, m_waveSimulationVertexShaderSurface);
  assert(hr == D3D_OK);
  
  // Set up state
  IRenderer::GetRendererInstance()->SetTexture(0, simulationSourceTexture);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
  IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
  IRenderer::GetRendererInstance()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
  IRenderer::GetRendererInstance()->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, FALSE);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
  IRenderer::GetRendererInstance()->SetTransform(D3DTS_WORLD, &identity);
  IRenderer::GetRendererInstance()->SetTransform(D3DTS_VIEW, &identity);
  IRenderer::GetRendererInstance()->SetTransform(D3DTS_PROJECTION, &identity);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

  // Draw the quad to do the simulation
  drawQuad( 
           D3DXFROMWINEVECTOR3(-1.0f, -1.0f,  0.0f), randomColor(), D3DXFROMWINEVECTOR2(0.0f, 0.0f),
           D3DXFROMWINEVECTOR3( 1.0f, -1.0f,  0.0f), randomColor(), D3DXFROMWINEVECTOR2(1.0f, 0.0f),
           D3DXFROMWINEVECTOR3( 1.0f,  1.0f,  0.0f), randomColor(), D3DXFROMWINEVECTOR2(1.0f, 1.0f),
           D3DXFROMWINEVECTOR3(-1.0f,  1.0f,  0.0f), randomColor(), D3DXFROMWINEVECTOR2(0.0f, 1.0f));

  // Reset state
  IRenderer::GetRendererInstance()->SetTexture(0, 0);
  IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
  IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, TRUE);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

  // Reset the matrix state
  SetMatrices();


  hr = IRenderer::GetRendererInstance()->SetRenderTarget(0, currentRenderTarget);
  assert(hr == D3D_OK);  
}

//-----------------------------------------------------------------------------
// 
// Initialize all of the vertex and pixel shaders
//        
//-----------------------------------------------------------------------------
bool Water::InitShaders(void)
{
  assert(IRenderer::GetRendererInstance() != 0);

  D3DXFROMWINEMACRO renderRefractionIncludes[] = {{"RETURN_REFRACTION", "1"}, 0};
  D3DXFROMWINEMACRO renderReflectionIncludes[] = {{"RETURN_REFLECTION", "1"}, 0};
  D3DXFROMWINEMACRO renderFresnelIncludes[] = {{"RETURN_FRESNEL", "1"}, 0};
  D3DXFROMWINEMACRO renderNormalsIncludes[] = {{"RETURN_NORMALS", "1"}, 0};

  CreateVertexShader( "simulation.hlsl", "vertexShader", "vs_1_1", m_waveSimulationVertexShader);
  CreatePixelShader( "simulation.hlsl", 0, "pixelShader", "ps_2_0", m_waveSimulationPixelShader);
  CreateVertexShader( "perturbSimulation.hlsl", "vertexShader", "vs_2_0", m_waveSimulationPerturbationVertexShader);
  CreatePixelShader( "perturbSimulation.hlsl", 0, "pixelShader", "ps_2_0", m_waveSimulationPerturbationPixelShader);
#ifdef FXC_WORK_AROUND
  CreateVertexShader( "renderVertexShader.vsh", m_waveRenderingVertexShader);
#else
  CreateVertexShader( "render.hlsl", "vertexShader", "vs_3_0", m_waveRenderingVertexShader);
#endif
  CreatePixelShader( "render.hlsl", 0, "pixelShader", "ps_3_0", m_waveRenderingPixelShader);
  CreatePixelShader( "render.hlsl", renderRefractionIncludes, "pixelShader", "ps_3_0", m_waveRenderingRefractionPixelShader);
  CreatePixelShader( "render.hlsl", renderReflectionIncludes, "pixelShader", "ps_3_0", m_waveRenderingReflectionPixelShader);
  CreatePixelShader( "render.hlsl", renderFresnelIncludes, "pixelShader", "ps_3_0", m_waveRenderingFresnelPixelShader);
  CreatePixelShader( "render.hlsl", renderNormalsIncludes, "pixelShader", "ps_3_0", m_waveRenderingNormalsPixelShader);
  CreatePixelShader( "underwater.hlsl", 0, "pixelShader", "ps_2_0", m_underwaterPixelShader);
  CreateVertexShader( "caustic.hlsl", "vertexShader", "vs_1_1", m_causticVertexShader);
  CreatePixelShader( "caustic.hlsl", 0, "pixelShader", "ps_2_0", m_causticPixelShader);
  CreateVertexShader( "diffuse.hlsl", "vertexShader", "vs_1_1", m_diffuseVertexShader);
  CreatePixelShader( "diffuse.hlsl", 0, "pixelShader", "ps_1_1", m_diffusePixelShader);
  CreateVertexShader( "singleTexture.hlsl", "vertexShader", "vs_1_1", m_singleTextureVertexShader);
  CreatePixelShader( "singleTexture.hlsl", 0, "pixelShader", "ps_1_1", m_singleTexturePixelShader);
  
  return(true);
}

//-----------------------------------------------------------------------------
// 
// Render the normal geometry to the backbuffer. If we're underwater, apply
// a full-screen wavy effect.
//
//-----------------------------------------------------------------------------
void Water::RenderBackbuffer(void)
{
  int hr;

  if(m_underwater)
    {
      // Set up fog
      float fogDensity = 0.02f;

      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FOGENABLE, 1);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_RGBA(0, 40, 120, 255));
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FOGDENSITY, (*(DWORD*)(&fogDensity)));
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP2);


      // If we're under water, we actually render to texture. This way we can 
      // apply a full-screen wave effect on it.
      hr = IRenderer::GetRendererInstance()->SetRenderTarget(0, m_underwaterWorldRenderTargetSurface);
      assert(hr == D3D_OK);
    }
  else
    {
      hr = IRenderer::GetRendererInstance()->SetRenderTarget(0, m_backBufferRenderTarget);
      assert(hr == D3D_OK);          
    }

  // Do the actual rendering of the world
  IRenderer::GetRendererInstance()->Clear(0L, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0L);
  RenderScene();
  RenderSkybox();
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FOGENABLE, 0);




  if(m_underwater)
    {
      //
      // Render caustics on top off all the world geometry
      // This is blended into what is already in the render target
      //
      IRenderer::GetRendererInstance()->SetVertexShader(m_causticVertexShader);
      IRenderer::GetRendererInstance()->SetPixelShader(m_causticPixelShader);

      IRenderer::GetRendererInstance()->SetTexture(0, m_waveSimulationTexture[(m_waveSimulationIndex + 0) % 3]);

      assert(hr == D3D_OK);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
      RenderScene(RENDER_LAND | RENDER_SUNKEN_BOAT, true);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

      IRenderer::GetRendererInstance()->SetVertexShader(0);
      IRenderer::GetRendererInstance()->SetPixelShader(0);
      
      IRenderer::GetRendererInstance()->SetTexture(0, 0);
          

      //
      //
      //  Render the full-screen wavy effect.
      //
      //

      //
      // We were rendering to texture so we could do the full-screen wavy effect.
      // Now, really use the backbuffer as the render target
      //
      IRenderer::GetRendererInstance()->SetRenderTarget(0, m_backBufferRenderTarget);

      m_worldMatrixStack->Push();
      m_worldMatrixStack->LoadIdentity();
          
      m_viewMatrixStack->Push();
      m_viewMatrixStack->LoadIdentity();
          
      m_projectionMatrixStack->Push();
      m_projectionMatrixStack->LoadIdentity();

      SetMatrices();
          
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, false);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZWRITEENABLE, false);
      IRenderer::GetRendererInstance()->SetPixelShader(m_underwaterPixelShader);
      IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_TIME, (const float*)&D3DXFROMWINEVECTOR4(m_time, 0.0f, 0.0f, 0.0f), 1);
      IRenderer::GetRendererInstance()->SetTexture(0, m_underwaterWorldRenderTargetTexture);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

      drawQuad( 
               D3DXFROMWINEVECTOR3(-1.0f, -1.0f,  0.0f), randomColor(), D3DXFROMWINEVECTOR2(0.0f, 1.0f),
               D3DXFROMWINEVECTOR3(-1.0f,  1.0f,  0.0f), randomColor(), D3DXFROMWINEVECTOR2(0.0f, 0.0f),
               D3DXFROMWINEVECTOR3( 1.0f,  1.0f,  0.0f), randomColor(), D3DXFROMWINEVECTOR2(1.0f, 0.0f),
               D3DXFROMWINEVECTOR3( 1.0f, -1.0f,  0.0f), randomColor(), D3DXFROMWINEVECTOR2(1.0f, 1.0f));
      IRenderer::GetRendererInstance()->SetPixelShader(0);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, true);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZWRITEENABLE, true);
      IRenderer::GetRendererInstance()->SetTexture(0, 0);

      m_worldMatrixStack->Pop();
      m_viewMatrixStack->Pop();
      m_projectionMatrixStack->Pop();
      SetMatrices();
    }
}

//-----------------------------------------------------------------------------
// 
// Render the informational text.
//
//-----------------------------------------------------------------------------
void Water::RenderInformation(void)
{  
  // Set up the transform state for text
  m_worldMatrixStack->Push();
  m_worldMatrixStack->LoadIdentity();
      
  m_viewMatrixStack->Push();
  m_viewMatrixStack->LoadIdentity();
      
  m_projectionMatrixStack->Push();
  m_projectionMatrixStack->LoadIdentity();

  SetMatrices();


  // Render various render targets if desired
  if(m_textureOverlayIndex > 0)
    {
      TextureID texture;
      char*              description0;
      char*              description1;

      switch(m_textureOverlayIndex)
        {
        case 1:
          {
            texture = m_waveRefractionTexture;
            description0 = "refraction texture";
            description1 = "";
            break;
          }
        case 2:
          {
            texture = m_waveReflectionTexture;
            description0 = "reflection texture";
            description1 = "";
            break;
          }
        case 3:
          {
            texture = m_waveRefractionNearTexture;
            description0 = "reflection texture for";
            description1 = "objects penetrating the water";
            break;
          }
        case 4:
          {
            texture = m_waveReflectionNearTexture;
            description0 = "refraction texture for";
            description1 = "objects penetrating the water";
            break;
          }
        case 5:
          {            
            texture = m_waveSimulationTexture[(m_waveSimulationIndex + 0) % 3];
            description0 = "wave equation simulation";
            description1 = "height = r,   normal = gba";
            break;
          }
        default:
          {
            assert("Shouldn't be here" == 0);
            description0 = "";
            description1 = "";
            break;
          }
        }



      // Draw the texture
      IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
      IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
      IRenderer::GetRendererInstance()->SetTexture(0, texture);
      drawQuad( 
               D3DXFROMWINEVECTOR3( 0.00f,  0.00f, 0.0f), 0, D3DXFROMWINEVECTOR2(0.0f, 0.0f),
               D3DXFROMWINEVECTOR3( 1.00f,  0.00f, 0.0f), 0, D3DXFROMWINEVECTOR2(1.0f, 0.0f),
               D3DXFROMWINEVECTOR3( 1.00f, -1.00f, 0.0f), 0, D3DXFROMWINEVECTOR2(1.0f, 1.0f),
               D3DXFROMWINEVECTOR3( 0.00f, -1.00f, 0.0f), 0, D3DXFROMWINEVECTOR2(0.0f, 1.0f));
      IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
      IRenderer::GetRendererInstance()->SetTexture(0, 0);

      // Render the text description
      m_fontPosition.x = m_deviceWidth / 2;
      m_fontPosition.y = m_deviceHeight / 2;
      Printf(description0);
      Printf(description1);
    }

  // Render info about rendering
  m_fontPosition.x = 10;
  m_fontPosition.y = 10;
  Printf(fromUnicode(DXUTGetFrameStats()));
  Printf(fromUnicode(DXUTGetDeviceStats()));

  // Render info about current state
  m_fontPosition.x = m_deviceWidth / 2;
  m_fontPosition.y = 20;  

  // Render help blurb and help
  m_fontPosition.x = 10 + (int)(50.0f + 50.0f * sin(m_time));
  m_fontPosition.y = m_deviceHeight - 30;
  Printf("Press 'F1' to toggle help");

  if(m_displayHelp)
    {
      m_fontPosition.x = 10;
      m_fontPosition.y = 100;

      Printf("Drag with left button down to rotate camera");
      Printf("'M' to toggle mouse camera control");      
      Printf("WASD to move");
      Printf("'E' to move eye up");
      Printf("'Q' to move eye down");
      Printf("'Esc' to quit");
    }

  // Reset the transform state
  m_worldMatrixStack->Pop();
  m_viewMatrixStack->Pop();
  m_projectionMatrixStack->Pop();
  SetMatrices();
}


//-----------------------------------------------------------------------------
// 
// Render the scene into the reflection texture
//
//-----------------------------------------------------------------------------
void Water::RenderReflectionTexture(void)
{
  // Render the water using the vertex shader with texture fetch
  D3DXFROMWINEPLANE clipPlane;
  D3DXFROMWINEPLANE transformedClipPlane;
  float     reflectionMapFieldOfView;

  // Render to the reflection map
  IRenderer::GetRendererInstance()->SetRenderTarget(0, m_waveReflectionSurface);

  // Compute the field of view and use it
  reflectionMapFieldOfView = 2.0f * atan((tan(m_fieldOfView / 2.0f)) * m_reflectionMapOverdraw);
  D3DXFROMWINEMatrixPerspectiveFovRH(&m_reflectionProjectionMatrix, reflectionMapFieldOfView, m_aspectRatio, 1.0f, 100.0f);

  m_projectionMatrixStack->Push();
  m_projectionMatrixStack->LoadMatrix(&m_reflectionProjectionMatrix);
  m_reflectViewMatrix = true;
  SetMatrices();

  // Set up the clip plane to draw the correct geometry
  if(m_underwater)
    {
      clipPlane = m_reflectionClipPlaneBelowWater;
    }
  else
    {
      clipPlane = m_reflectionClipPlaneAboveWater;
    }
  D3DXFROMWINEPlaneTransform(&transformedClipPlane, &clipPlane, &m_worldViewProjectionInverseTransposeMatrix);
  IRenderer::GetRendererInstance()->SetClipPlane(0, transformedClipPlane);


  // Clear
  IRenderer::GetRendererInstance()->Clear(0L, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0L);

  // Render the actual scene
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CLIPPLANEENABLE, 1);
  RenderScene(RENDER_LAND | RENDER_SUNKEN_BOAT);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
  RenderSkybox();
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CLIPPLANEENABLE, 1);

#ifdef CODE_IS_NOT_USED   
  // Render the reflection map for objects penetrating the water
  // This is not used because it looks ugly
  IRenderer::GetRendererInstance()->SetRenderTarget(0, m_waveReflectionNearSurface);
  IRenderer::GetRendererInstance()->Clear(0L, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0L);
  RenderScene(RENDER_BOAT);
#endif


  // Reste state
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
  m_projectionMatrixStack->Pop();
  m_reflectViewMatrix = false;

  SetMatrices();
}


//-----------------------------------------------------------------------------
// 
// Render the scene into the refraction texture
//
//-----------------------------------------------------------------------------
void Water::RenderRefractionTexture(void)
{
  D3DXFROMWINEPLANE clipPlane;
  D3DXFROMWINEPLANE transformedClipPlane;
  float     refractionMapFieldOfView;

  IRenderer::GetRendererInstance()->SetRenderTarget(0, m_waveRefractionSurface);

  refractionMapFieldOfView = 2.0f * atan((tan(m_fieldOfView / 2.0f)) * m_refractionMapOverdraw);
  D3DXFROMWINEMatrixPerspectiveFovRH(&m_refractionProjectionMatrix, refractionMapFieldOfView, m_aspectRatio, 1.0f, 100.0f);

  m_worldMatrixStack->LoadIdentity();
  m_projectionMatrixStack->Push();
  m_projectionMatrixStack->LoadMatrix(&m_refractionProjectionMatrix);
  SetMatrices();


  if(m_underwater)
    {
      clipPlane = m_refractionClipPlaneBelowWater;
    }
  else
    {
      clipPlane = m_refractionClipPlaneAboveWater;
    }
  D3DXFROMWINEPlaneTransform(&transformedClipPlane, &clipPlane, &m_worldViewProjectionInverseTransposeMatrix);
  IRenderer::GetRendererInstance()->SetClipPlane(0, transformedClipPlane);

  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CLIPPLANEENABLE, 1);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

  IRenderer::GetRendererInstance()->Clear(0L, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0L);

  RenderScene(RENDER_LAND | RENDER_SUNKEN_BOAT);

  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);

  RenderSkybox();

  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CLIPPLANEENABLE, 1);

  IRenderer::GetRendererInstance()->SetRenderTarget(0, m_waveRefractionNearSurface);
  IRenderer::GetRendererInstance()->Clear(0L, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0L);
  RenderScene(RENDER_BOAT);

  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

  // Use the default matrix setup
  m_projectionMatrixStack->Pop();
  SetMatrices();
}


//-----------------------------------------------------------------------------
// 
// Render the scene
// Selectively renders bit and pices depending on what the scene is
// being rendered for
//        
//-----------------------------------------------------------------------------
void Water::RenderScene(unsigned int objectSelection, bool disableStateSetup)
{
  if(objectSelection & RENDER_LAND)
    {
      RenderSceneLand(disableStateSetup);
    }

  if(objectSelection & RENDER_BOAT)
    {
      RenderSceneBoat(disableStateSetup);
    }

  if(objectSelection & RENDER_SUNKEN_BOAT)
    {
      RenderSceneSunkenBoat(disableStateSetup);
    }
}


//-----------------------------------------------------------------------------
// 
// Renders the boat in the scene
//        
//-----------------------------------------------------------------------------
void Water::RenderSceneBoat(bool disableStateSetup)
{
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

  m_worldMatrixStack->Push();
  m_worldMatrixStack->LoadMatrix(&m_boatMesh.getWorldMatrix());
  SetMatrices();
  if(disableStateSetup == false)
    {
      IRenderer::GetRendererInstance()->SetVertexShader(m_singleTextureVertexShader);
      IRenderer::GetRendererInstance()->SetPixelShader(m_singleTexturePixelShader);
      IRenderer::GetRendererInstance()->SetTexture(0, m_boatTexture);
    }
  m_boatMesh.render(disableStateSetup);
  if(disableStateSetup == false)
    {
      IRenderer::GetRendererInstance()->SetVertexShader(0);
      IRenderer::GetRendererInstance()->SetPixelShader(0);
    }

  m_worldMatrixStack->Pop();
  SetMatrices();
}


//-----------------------------------------------------------------------------
// 
// Renders the land in the scene
//        
//-----------------------------------------------------------------------------
void Water::RenderSceneLand(bool disableStateSetup)
{
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

  m_worldMatrixStack->Push();
  m_worldMatrixStack->LoadIdentity();
  SetMatrices();

  if(disableStateSetup == false)
    {
      IRenderer::GetRendererInstance()->SetVertexShader(m_singleTextureVertexShader);
      IRenderer::GetRendererInstance()->SetPixelShader(m_singleTexturePixelShader);
    }

  m_land->render( disableStateSetup);

  if(disableStateSetup == false)
    {
      IRenderer::GetRendererInstance()->SetVertexShader(0);
      IRenderer::GetRendererInstance()->SetPixelShader(0);
    }
      
  m_worldMatrixStack->Pop();
  SetMatrices();

  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


//-----------------------------------------------------------------------------
// 
// Renders the sunken boat in the scene
//        
//-----------------------------------------------------------------------------
void Water::RenderSceneSunkenBoat(bool disableStateSetup)
{
  // Render the boat
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
      
  m_worldMatrixStack->Push();
  m_worldMatrixStack->LoadMatrix(&m_sunkenBoatMesh.getWorldMatrix());
  SetMatrices();
  if(disableStateSetup == false)
    {
      IRenderer::GetRendererInstance()->SetVertexShader(m_diffuseVertexShader);
      IRenderer::GetRendererInstance()->SetPixelShader(m_diffusePixelShader);
    }

  if(m_renderSunkenBoat)
    {
      m_sunkenBoatMesh.render(disableStateSetup);
    }

  if(disableStateSetup == false)
    {
      IRenderer::GetRendererInstance()->SetVertexShader(0);
      IRenderer::GetRendererInstance()->SetPixelShader(0);
    }

  m_worldMatrixStack->Pop();
  SetMatrices();

  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


//-----------------------------------------------------------------------------
// 
// Render the skybox
//        
//-----------------------------------------------------------------------------
void Water::RenderSkybox(void)
{
  D3DXFROMWINEMATRIX worldMatrix;

  // Don't need culling for skybox
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

  // Scale and translate the unit cube
  m_worldMatrixStack->Push();
  m_worldMatrixStack->Translate(1.0f, 0.0f, 1.0f);
  m_worldMatrixStack->Scale(25.0f, 25.0f, 25.0f);
  SetMatrices();

  // Set up rendering state
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
  IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);

  // Draw the skybox
  IRenderer::GetRendererInstance()->SetTexture(0, m_skyBoxTextures[0]);
  drawQuad( 
           D3DXFROMWINEVECTOR3(-1.0f, -1.0f,  1.0f), 0, D3DXFROMWINEVECTOR2(0.0f, 0.0f),
           D3DXFROMWINEVECTOR3( 1.0f, -1.0f,  1.0f), 0, D3DXFROMWINEVECTOR2(1.0f, 0.0f),
           D3DXFROMWINEVECTOR3( 1.0f,  1.0f,  1.0f), 0, D3DXFROMWINEVECTOR2(1.0f, 1.0f),
           D3DXFROMWINEVECTOR3(-1.0f,  1.0f,  1.0f), 0, D3DXFROMWINEVECTOR2(0.0f, 1.0f));
  IRenderer::GetRendererInstance()->SetTexture(0, m_skyBoxTextures[1]);
  drawQuad( 
           D3DXFROMWINEVECTOR3( 1.0f, -1.0f,  1.0f), 0, D3DXFROMWINEVECTOR2(0.0f, 0.0f),
           D3DXFROMWINEVECTOR3( 1.0f, -1.0f, -1.0f), 0, D3DXFROMWINEVECTOR2(1.0f, 0.0f),
           D3DXFROMWINEVECTOR3( 1.0f,  1.0f, -1.0f), 0, D3DXFROMWINEVECTOR2(1.0f, 1.0f),
           D3DXFROMWINEVECTOR3( 1.0f,  1.0f,  1.0f), 0, D3DXFROMWINEVECTOR2(0.0f, 1.0f));
  IRenderer::GetRendererInstance()->SetTexture(0, m_skyBoxTextures[2]);
  drawQuad( 
           D3DXFROMWINEVECTOR3( 1.0f, -1.0f, -1.0f), 0, D3DXFROMWINEVECTOR2(0.0f, 0.0f),
           D3DXFROMWINEVECTOR3(-1.0f, -1.0f, -1.0f), 0, D3DXFROMWINEVECTOR2(1.0f, 0.0f),
           D3DXFROMWINEVECTOR3(-1.0f,  1.0f, -1.0f), 0, D3DXFROMWINEVECTOR2(1.0f, 1.0f),
           D3DXFROMWINEVECTOR3( 1.0f,  1.0f, -1.0f), 0, D3DXFROMWINEVECTOR2(0.0f, 1.0f));
  IRenderer::GetRendererInstance()->SetTexture(0, m_skyBoxTextures[3]);
  drawQuad( 
           D3DXFROMWINEVECTOR3(-1.0f, -1.0f, -1.0f), 0, D3DXFROMWINEVECTOR2(0.0f, 0.0f),
           D3DXFROMWINEVECTOR3(-1.0f, -1.0f,  1.0f), 0, D3DXFROMWINEVECTOR2(1.0f, 0.0f),
           D3DXFROMWINEVECTOR3(-1.0f,  1.0f,  1.0f), 0, D3DXFROMWINEVECTOR2(1.0f, 1.0f),
           D3DXFROMWINEVECTOR3(-1.0f,  1.0f, -1.0f), 0, D3DXFROMWINEVECTOR2(0.0f, 1.0f));
  IRenderer::GetRendererInstance()->SetTexture(0, m_skyBoxTextures[4]);
  drawQuad( 
           D3DXFROMWINEVECTOR3( 1.0f,  1.0f, -1.0f), 0, D3DXFROMWINEVECTOR2(0.0f, 0.0f),
           D3DXFROMWINEVECTOR3(-1.0f,  1.0f, -1.0f), 0, D3DXFROMWINEVECTOR2(1.0f, 0.0f),
           D3DXFROMWINEVECTOR3(-1.0f,  1.0f,  1.0f), 0, D3DXFROMWINEVECTOR2(1.0f, 1.0f),
           D3DXFROMWINEVECTOR3( 1.0f,  1.0f,  1.0f), 0, D3DXFROMWINEVECTOR2(0.0f, 1.0f));
  IRenderer::GetRendererInstance()->SetTexture(0, m_skyBoxTextures[5]);


  // Reset the rendering state
  IRenderer::GetRendererInstance()->SetTexture(0, 0);
  IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
  m_worldMatrixStack->Pop();

  // Reset the world transform
  SetMatrices();
}




//-----------------------------------------------------------------------------
// 
// Render the surface of the water. This uses the refraction and reflection
// textures that have already been rendered for this frame.
//
//-----------------------------------------------------------------------------
void Water::RenderWater(void)
{
  int hr;

  const float etaWater = 1.33f;    // Index of refraction of water
  const float etaAir   = 1.00f;    // Index of refraction of air
  float etaRatio;                  // Ratio of indices of refraction of two media

  // Set up the vertex shader texture
  CopySimulationTexture(m_waveSimulationTexture[(m_waveSimulationIndex + 0) % 3], m_backBufferRenderTarget);

  hr = IRenderer::GetRendererInstance()->SetTexture(D3DVERTEXTEXTURESAMPLER0, m_waveSimulationVertexShaderTexture);
  IRenderer::GetRendererInstance()->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  IRenderer::GetRendererInstance()->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  IRenderer::GetRendererInstance()->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
  IRenderer::GetRendererInstance()->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);


  // Set up the vertex shader
  IRenderer::GetRendererInstance()->SetVertexShader(m_waveRenderingVertexShader);
  SetMatrices();

  // Set up the pixel shader
  if(m_renderRefractionOnly)
    {
      IRenderer::GetRendererInstance()->SetPixelShader(m_waveRenderingRefractionPixelShader);
    }
  else if(m_renderReflectionOnly)
    {
      IRenderer::GetRendererInstance()->SetPixelShader(m_waveRenderingReflectionPixelShader);
    }
  else if(m_renderFresnelOnly)
    {
      IRenderer::GetRendererInstance()->SetPixelShader(m_waveRenderingFresnelPixelShader);
    }
  else if(m_renderNormalsOnly)
    {
      IRenderer::GetRendererInstance()->SetPixelShader(m_waveRenderingNormalsPixelShader);
    }
  else
    {
      IRenderer::GetRendererInstance()->SetPixelShader(m_waveRenderingPixelShader);
    }

  // Set up coefficients that depend on whether the viewer is above or below the water
  if(m_underwater)
    {
      etaRatio = etaWater / etaAir;

      IRenderer::GetRendererInstance()->SetVertexShaderConstantF(VS_ETA_RATIO, (const float*)&D3DXFROMWINEVECTOR4(etaRatio, etaRatio * etaRatio, 0.0f, 0.0f), 1);
      IRenderer::GetRendererInstance()->SetVertexShaderConstantF(VS_NORMAL_SIGN, (const float*)&D3DXFROMWINEVECTOR4(-1.0f, 0.0f, 0.0f, 0.0f), 1);
      IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_FRESNEL_R0, (const float*)&D3DXFROMWINEVECTOR4(0.20f, 0.0f, 0.0f, 0.0f), 1);
      IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_UNDER_WATER, (const float*)&D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 1);
      IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_TOTAL_INTERNAL_REFLECTION_SLOPE_BIAS, (const float*)&D3DXFROMWINEVECTOR4(5.0f, 0.65f, 0.65f, 0.0f), 1);
    }
  else
    {
      etaRatio = etaAir / etaWater;

      IRenderer::GetRendererInstance()->SetVertexShaderConstantF(VS_ETA_RATIO, (const float*)&D3DXFROMWINEVECTOR4(etaRatio, etaRatio * etaRatio, 0.0f, 0.0f), 1);
      IRenderer::GetRendererInstance()->SetVertexShaderConstantF(VS_NORMAL_SIGN, (const float*)&D3DXFROMWINEVECTOR4(1.0f, 0.0f, 0.0f, 0.0f), 1);
      IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_FRESNEL_R0, (const float*)&D3DXFROMWINEVECTOR4(0.0977f, 0.0f, 0.0f, 0.0f), 1);
      IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_UNDER_WATER, (const float*)&D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);
      IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_TOTAL_INTERNAL_REFLECTION_SLOPE_BIAS, (const float*)&D3DXFROMWINEVECTOR4(1.0f, 0.0f, 1.0f, 0.0f), 1);
    }

  // Set the eye position in world space
  IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_EYE_POSITION_WORLD, (const float*)(&(m_viewer.getPosition())), 1);

  // Set the refraction map overdraw
  IRenderer::GetRendererInstance()->SetVertexShaderConstantF(VS_REFRACTION_TEXCOORD_SCALE_OFFSET, 
                                         (const float*)&D3DXFROMWINEVECTOR4((1.0f /  m_refractionMapOverdraw), (1.0f / m_refractionMapOverdraw),
                                                                    .5f * (1.0f - (1.0f / m_refractionMapOverdraw)),
                                                                    .5f * (1.0f - (1.0f / m_refractionMapOverdraw))), 1);
  IRenderer::GetRendererInstance()->SetVertexShaderConstantF(VS_REFLECTION_TEXCOORD_SCALE_OFFSET, 
                                         (const float*)&D3DXFROMWINEVECTOR4((1.0f /  m_reflectionMapOverdraw), 
                                                                    (1.0f / m_reflectionMapOverdraw),
                                                                    .5f * (1.0f - (1.0f / m_reflectionMapOverdraw)),
                                                                    .5f * (1.0f - (1.0f / m_reflectionMapOverdraw))), 1);



  // Set up the pixel shader textures
  IRenderer::GetRendererInstance()->SetTexture(0, m_waveRefractionTexture);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetTexture(1, m_waveReflectionTexture);
  IRenderer::GetRendererInstance()->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  IRenderer::GetRendererInstance()->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  IRenderer::GetRendererInstance()->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
  IRenderer::GetRendererInstance()->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
  IRenderer::GetRendererInstance()->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetTexture(2, m_waveRefractionNearTexture);
  IRenderer::GetRendererInstance()->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  IRenderer::GetRendererInstance()->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  IRenderer::GetRendererInstance()->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
  IRenderer::GetRendererInstance()->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
  IRenderer::GetRendererInstance()->SetTexture(3, m_waveReflectionNearTexture);
  IRenderer::GetRendererInstance()->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  IRenderer::GetRendererInstance()->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  IRenderer::GetRendererInstance()->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
  IRenderer::GetRendererInstance()->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
  hr = IRenderer::GetRendererInstance()->SetTexture(4, m_fadeTexture);
  assert(hr == D3D_OK);
  IRenderer::GetRendererInstance()->SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  IRenderer::GetRendererInstance()->SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  IRenderer::GetRendererInstance()->SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

  if(m_renderWireframeWater)
    {
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    }

  // Set renderstates
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

  // Set up vertices, indices and render
  IRenderer::GetRendererInstance()->SetStreamSource(0, m_waveSimulationVertexBuffer, 0, sizeof(Vertex));
  IRenderer::GetRendererInstance()->SetIndices(m_waveSimulationIndexBuffer);
  IRenderer::GetRendererInstance()->SetFVF(Vertex::FVF);
  IRenderer::GetRendererInstance()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_waveSimulationGrid.size(), 0, m_waveSimulationGrid.primitiveCount());

  // Reset state
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
  IRenderer::GetRendererInstance()->SetTexture(0, 0);
  IRenderer::GetRendererInstance()->SetTexture(1, 0);
  IRenderer::GetRendererInstance()->SetTexture(2, 0);
  IRenderer::GetRendererInstance()->SetTexture(3, 0);
  IRenderer::GetRendererInstance()->SetTexture(4, 0);
  IRenderer::GetRendererInstance()->SetVertexShader(0);
  IRenderer::GetRendererInstance()->SetPixelShader(0);
  IRenderer::GetRendererInstance()->SetTexture(D3DVERTEXTEXTURESAMPLER0, 0);
  if(m_renderWireframeWater)
    {
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    }
}


//-----------------------------------------------------------------------------
// 
// Restore all of the objects related to rendering the water
// This is called by RestoreDeviceObjects()
//
//-----------------------------------------------------------------------------
int Water::RestoreWaterRenderingObjects(void)
{
  int hr;
  
  // Create the wave reflection and refraction textures for objects
  // that do not penetrate the surface of the water.
  hr = IRenderer::GetRendererInstance()->CreateTexture(min((int)m_deviceWidth, m_reflectionTextureWidth), 
                                   min((int)m_deviceHeight, m_reflectionTextureHeight), 
                                   1, D3DUSAGE_RENDERTARGET, 
                                   D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_waveReflectionTexture, 0);
  CHECK_RETURN_CODE("CreateTexture m_waveReflectionTexture", hr);  

  hr = m_waveReflectionTexture->GetSurfaceLevel(0, &m_waveReflectionSurface);
  CHECK_RETURN_CODE("GetSurfaceLevel m_waveReflectionSurface", hr);
  
  hr = IRenderer::GetRendererInstance()->CreateTexture(min((int)m_deviceWidth, m_refractionTextureWidth), 
                                   min((int)m_deviceHeight, m_refractionTextureHeight), 
                                   1, D3DUSAGE_RENDERTARGET, 
                                   D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_waveRefractionTexture, 0);
  CHECK_RETURN_CODE("CreateTexture m_waveRefractionTexture", hr);

  hr = m_waveRefractionTexture->GetSurfaceLevel(0, &m_waveRefractionSurface);
  CHECK_RETURN_CODE("GetSurfaceLevel m_waveRefractionSurface", hr);

  
  // Create the second set of refraction and reflection render targets for objects
  // that are penetrating the surface of the water.
  hr = IRenderer::GetRendererInstance()->CreateTexture(min((int)m_deviceWidth, m_reflectionTextureWidth), 
                                   min((int)m_deviceHeight, m_reflectionTextureHeight), 
                                   1, D3DUSAGE_RENDERTARGET, 
                                   D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_waveReflectionNearTexture, 0);
  CHECK_RETURN_CODE("CreateTexture m_waveReflectionNearTexture", hr);  

  hr = m_waveReflectionNearTexture->GetSurfaceLevel(0, &m_waveReflectionNearSurface);
  CHECK_RETURN_CODE("GetSurfaceLevel m_waveReflectionNearSurface", hr);
  
  hr = IRenderer::GetRendererInstance()->CreateTexture(min((int)m_deviceWidth, m_refractionTextureWidth), 
                                   min((int)m_deviceHeight, m_refractionTextureHeight), 
                                   1, D3DUSAGE_RENDERTARGET, 
                                   D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_waveRefractionNearTexture, 0);
  CHECK_RETURN_CODE("CreateTexture m_waveRefractionNearTexture", hr);

  hr = m_waveRefractionNearTexture->GetSurfaceLevel(0, &m_waveRefractionNearSurface);
  CHECK_RETURN_CODE("GetSurfaceLevel m_waveRefractionNearSurface", hr);

  return(D3D_OK);
}


//-----------------------------------------------------------------------------
// 
// Restore all of the objects related to simulating the water
// This is called by RestoreDeviceObjects()
//
//-----------------------------------------------------------------------------
int Water::RestoreWaterSimulationObjects(void)
{
  int         i;
  int     hr;
  std::string fullpath;


  // Create the texture/render targets where the wave simulation takes place
  for(i=0; i<3; i++)
    {
      // Create the texture
      hr = D3DXFROMWINECreateTexture( m_waveSimulationWidth, m_waveSimulationHeight, 1, D3DUSAGE_RENDERTARGET, 
                             D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &m_waveSimulationTexture[i]);
      assert(hr == D3D_OK);

      // Get the surface from the texture so we can use it as a render target
      hr = m_waveSimulationTexture[i]->GetSurfaceLevel(0, &m_waveSimulationSurface[i]);
      assert(hr == D3D_OK);

      // Clear the surface
      IRenderer::GetRendererInstance()->SetRenderTarget(0, m_waveSimulationSurface[i]);
      IRenderer::GetRendererInstance()->Clear(0L, 0, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0L);
      IRenderer::GetRendererInstance()->SetRenderTarget(0, m_backBufferRenderTarget);
    }


  // Create the wave simulation texture and surface for the vertex shader
  hr = D3DXFROMWINECreateTexture( m_waveSimulationWidth, m_waveSimulationHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, 
                         &m_waveSimulationVertexShaderTexture);
  assert(hr == D3D_OK);

  hr = m_waveSimulationVertexShaderTexture->GetSurfaceLevel(0, &m_waveSimulationVertexShaderSurface);
  assert(hr == D3D_OK);
  

  // Load the simulation dampening texture from a file
  FindFile("dampening.tga", fullpath);
  hr = D3DXFROMWINECreateTextureFromFileEx( toUnicode(fullpath.c_str()), m_waveSimulationWidth, m_waveSimulationHeight, 1, 0, 
                                   D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DXFROMWINE_FILTER_NONE, D3DXFROMWINE_DEFAULT, 0, 0, 0, &m_waveSimulationDampeningTexture);
  assert(hr == D3D_OK);

  return(D3D_OK);
}



//-----------------------------------------------------------------------------
// 
// Setup up the transforms for fixed function.
// Setup constants for vertex shaders.        
//
//-----------------------------------------------------------------------------
void Water::SetMatrices(void)
{
  D3DXFROMWINEMATRIX worldViewMatrix;
  D3DXFROMWINEMATRIX worldInverseMatrix;
  D3DXFROMWINEMATRIX worldInverseTransposeMatrix;
  D3DXFROMWINEMATRIX viewMatrix;
  D3DXFROMWINEMATRIX viewProjectionMatrix;
  D3DXFROMWINEMATRIX worldViewProjectionMatrix;

  // Reflect the view matrix if asked
  if(m_reflectViewMatrix)
    {
      D3DXFROMWINEMATRIX reflectionMatrix;

      D3DXFROMWINEMatrixReflect(&reflectionMatrix, &D3DXFROMWINEPLANE(0.0f, 1.0f, 0.0f, 0.0f));

      viewMatrix = reflectionMatrix * (*m_viewMatrixStack->GetTop());
    }
  else
    {
      viewMatrix = (*m_viewMatrixStack->GetTop());
    }

  // Compute various matrices
  worldViewMatrix = (*m_worldMatrixStack->GetTop()) * viewMatrix;
  worldViewProjectionMatrix = (*m_worldMatrixStack->GetTop()) * viewMatrix * (*m_projectionMatrixStack->GetTop());
  viewProjectionMatrix = viewMatrix * (*m_projectionMatrixStack->GetTop());
  D3DXFROMWINEMatrixInverse(&worldInverseMatrix, 0, m_worldMatrixStack->GetTop());
  D3DXFROMWINEMatrixTranspose(&worldInverseTransposeMatrix, &worldInverseMatrix);

    
  // Set the matrices for fixed function
  IRenderer::GetRendererInstance()->SetTransform(D3DTS_WORLD, m_worldMatrixStack->GetTop());
  IRenderer::GetRendererInstance()->SetTransform(D3DTS_VIEW, &viewMatrix);
  IRenderer::GetRendererInstance()->SetTransform(D3DTS_PROJECTION, m_projectionMatrixStack->GetTop());


  // Set the vertex shader constants for the matrices
  SetMatrix(VS_WORLD_MATRIX, (*m_worldMatrixStack->GetTop()));
  SetMatrix(VS_WORLD_INVERSE_TRANSPOSE_MATRIX, worldInverseTransposeMatrix);
  SetMatrix(VS_WORLD_VIEW_MATRIX, worldViewMatrix);
  SetMatrix(VS_WORLD_VIEW_PROJECTION_MATRIX, worldViewProjectionMatrix);
  SetMatrix(VS_PROJECTION_MATRIX, (*m_projectionMatrixStack->GetTop()));
  SetMatrix(VS_VIEW_PROJECTION_MATRIX, viewProjectionMatrix); 

  // Compute _worldViewProjectionInverseTransposeMatrix so that we can transform clip planes for 
  // geometry that is rendered without the fixed function pipeline
  D3DXFROMWINEMatrixInverse(&m_worldViewProjectionInverseTransposeMatrix, 0, &worldViewProjectionMatrix);
  D3DXFROMWINEMatrixTranspose(&m_worldViewProjectionInverseTransposeMatrix, &m_worldViewProjectionInverseTransposeMatrix);

  // Set up some vertex shader constants
  IRenderer::GetRendererInstance()->SetVertexShaderConstantF(VS_LIGHT_POSITION_WORLD, (float*)&m_lightPositionWorld, 1);
  IRenderer::GetRendererInstance()->SetVertexShaderConstantF(VS_EYE_POSITION_WORLD, (float*)&m_viewer.getPosition(), 1);
  IRenderer::GetRendererInstance()->SetVertexShaderConstantF(VS_LIGHT_DIFFUSE, (float*)&D3DXFROMWINEVECTOR4(0.7f, 0.7f, 0.7f, 1.0f), 1);
  IRenderer::GetRendererInstance()->SetVertexShaderConstantF(VS_LIGHT_AMBIENT, (float*)&D3DXFROMWINEVECTOR4(0.7f, 0.7f, 0.7f, 1.0f), 1);
  
}




//-----------------------------------------------------------------------------
// 
// Setup the transpose of a matrix in the vertex shader constants
//
//-----------------------------------------------------------------------------
void Water::SetMatrix(int constantIndex, const D3DXFROMWINEMATRIX& matrix)
{
  assert(constantIndex >= 0);

  D3DXFROMWINEMATRIX transposeMatrix;

  D3DXFROMWINEMatrixTranspose(&transposeMatrix, &matrix);

  IRenderer::GetRendererInstance()->SetVertexShaderConstantF(constantIndex, (float*)&transposeMatrix, 4);
}




//-----------------------------------------------------------------------------
// 
// Do the simulation of the wave equation by rendering to an off-screen
// render target using the simulation pixel shader. Also, render pertubations
// in the simulation to account for things in the water.
//
//-----------------------------------------------------------------------------
void Water::SimulateWaveEquation(void)
{
  int hr;
  float   hackFactor;
  float   x, y;
  float   u[2];
  float   v[2];
  float   wakeTime;

  // Check to see if simulation is frozen
  if(m_freezeSimulation)
    {
      m_tLastFrame = -1.0f;
      return;
    }
      
  // Set up the samplers. We need to use the previous two simulation results
  // to compute the new simulation result.
  IRenderer::GetRendererInstance()->SetTexture(0, m_waveSimulationTexture[(m_waveSimulationIndex + 1) % 3]);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetTexture(1, m_waveSimulationTexture[(m_waveSimulationIndex + 2) % 3]);
  IRenderer::GetRendererInstance()->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  IRenderer::GetRendererInstance()->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  IRenderer::GetRendererInstance()->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

  // There is a dampening texture to allow an artist to control how much the
  // water is dampened in areas. This is mostly useful near the shore.
  IRenderer::GetRendererInstance()->SetTexture(2, m_waveSimulationDampeningTexture);
  IRenderer::GetRendererInstance()->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  IRenderer::GetRendererInstance()->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  IRenderer::GetRendererInstance()->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  IRenderer::GetRendererInstance()->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);


  // This is the simulation result we're going to compute
  IRenderer::GetRendererInstance()->SetRenderTarget(0, m_waveSimulationSurface[(m_waveSimulationIndex + 0) % 3]);
  IRenderer::GetRendererInstance()->SetVertexShader(m_waveSimulationVertexShader);
  IRenderer::GetRendererInstance()->SetPixelShader(m_waveSimulationPixelShader);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, FALSE);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

  // Set the constants
  IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_SIMULATION_TEX_COORD_DELTA_X0Y1, 
                                        (const float*)&D3DXFROMWINEVECTOR4(-1.0f / m_waveSimulationWidth,  0.0f / m_waveSimulationHeight, 0.0f, 0.0f), 1);
  IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_SIMULATION_TEX_COORD_DELTA_X2Y1, 
                                        (const float*)&D3DXFROMWINEVECTOR4( 1.0f / m_waveSimulationWidth,  0.0f / m_waveSimulationHeight, 0.0f, 0.0f), 1);
  IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_SIMULATION_TEX_COORD_DELTA_X1Y0, 
                                        (const float*)&D3DXFROMWINEVECTOR4( 0.0f / m_waveSimulationWidth, -1.0f / m_waveSimulationHeight, 0.0f, 0.0f), 1);
  IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_SIMULATION_TEX_COORD_DELTA_X1Y2, 
                                        (const float*)&D3DXFROMWINEVECTOR4( 0.0f / m_waveSimulationWidth,  1.0f / m_waveSimulationHeight, 0.0f, 0.0f), 1);
  IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_SIMULATION_POSITION_WEIGHTING, 
                                        (const float*)&D3DXFROMWINEVECTOR4(1.99f, 0.99f, 0.0f, 0.0f), 1);
  IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_SIMULATION_WAVE_SPEED_SQUARED, 
                                        (const float*)&D3DXFROMWINEVECTOR4(10.0f, 0.0f, 0.0f, 0.0f), 1);
  IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_SIMULATION_ONE_HALF_TIMES_DELTA_TIME_SQUARED, 
                                        (const float*)&D3DXFROMWINEVECTOR4(.01f, 0.0f, 0.0f, 0.0f), 1);

  // The hackFactor is just an empirically derived value that makes the water look
  // ok for the given average framerate. Currently, the simulation is not very
  // sophisticated and runs on a fixed timestep. If the framerate increases, the
  // simulation rate increases. This is clearly not the right thing to do, but
  // it is illustrative and simpler to understand. This factor was also selected for
  // stability.
  hackFactor = 15.0f;                      
  
  IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_SIMULATION_ONE_HALF_TIMES_DELTA_TIME_SQUARED, 
                                        (const float*)&D3DXFROMWINEVECTOR4(hackFactor * m_dt * m_dt, 0.0f, 0.0f, 0.0f), 1);
  IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_SIMULATION_ONE_HALF_TIMES_DELTA_TIME_SQUARED, 
                                        (const float*)&D3DXFROMWINEVECTOR4(0.001f, 0.0f, 0.0f, 0.0f), 1);
  IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_SIMULATION_GRID_SIZE, 
                                        (const float*)&D3DXFROMWINEVECTOR4(m_landScale[0] / m_waveSimulationWidth, 
                                                                   0.0f,
                                                                   m_landScale[1] / m_waveSimulationHeight, 
                                                                   0.0f), 1);


  // Draw a quad to do the simulation; this is where the work gets done
  srand(10);
  drawQuad( 
           D3DXFROMWINEVECTOR3(-1.0f,  1.0f, 0.0f), randomColor(), D3DXFROMWINEVECTOR2(0.0f, 0.0f),
           D3DXFROMWINEVECTOR3( 1.0f,  1.0f, 0.0f), randomColor(), D3DXFROMWINEVECTOR2(1.0f, 0.0f),
           D3DXFROMWINEVECTOR3( 1.0f, -1.0f, 0.0f), randomColor(), D3DXFROMWINEVECTOR2(1.0f, 1.0f),
           D3DXFROMWINEVECTOR3(-1.0f, -1.0f, 0.0f), randomColor(), D3DXFROMWINEVECTOR2(0.0f, 1.0f));

      
  // Be nice and reset state for the simulation
  IRenderer::GetRendererInstance()->SetVertexShader(0);
  IRenderer::GetRendererInstance()->SetPixelShader(0);
  IRenderer::GetRendererInstance()->SetTexture(0, 0);
  IRenderer::GetRendererInstance()->SetTexture(1, 0);
  IRenderer::GetRendererInstance()->SetTexture(2, 0);


  //
  // Render perturbations into the simulation texture
  // This is used to put the wake from the boat and the water ghost
  // into the water.
  //
  if(m_stopPertubingWater == false)
    {
      // Calculate some hackety hack hack animation
      wakeTime = m_time - .55f;

      x = 0.5f * sin(wakeTime);
      y = -0.5f * cos(wakeTime);
      v[0] = -0.1f * cos(wakeTime);
      v[1] = -0.1f * sin(wakeTime);
      u[0] = -0.1f * -sin(wakeTime);
      u[1] = -0.1f * cos(wakeTime);

      
      // Set up the texture for the wake
      IRenderer::GetRendererInstance()->SetTexture(0, m_wakeTexture);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
      IRenderer::GetRendererInstance()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

      // Set up to blend the wake directly into the floating point simulation texture
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      
      IRenderer::GetRendererInstance()->SetVertexShader(m_waveSimulationPerturbationVertexShader);
      IRenderer::GetRendererInstance()->SetPixelShader(m_waveSimulationPerturbationPixelShader);

      IRenderer::GetRendererInstance()->SetPixelShaderConstantF(PS_DELTA_TIME, (float*)&D3DXFROMWINEVECTOR4(m_dt, 0.0f, 0.0f, 0.0f), 1);

      // Draw the wake into the simulation texture
      drawQuad( 
               D3DXFROMWINEVECTOR3(x - u[0] - v[0], y - u[1] - v[1], 0.0f), D3DCOLOR_RGBA(1, 0, 0, 0), D3DXFROMWINEVECTOR2(0.0f, 0.0f),
               D3DXFROMWINEVECTOR3(x + u[0] - v[0], y + u[1] - v[1], 0.0f), D3DCOLOR_RGBA(1, 0, 0, 0), D3DXFROMWINEVECTOR2(1.0f, 0.0f),
               D3DXFROMWINEVECTOR3(x + u[0] + v[0], y + u[1] + v[1], 0.0f), D3DCOLOR_RGBA(1, 0, 0, 0), D3DXFROMWINEVECTOR2(1.0f, 1.0f),
               D3DXFROMWINEVECTOR3(x - u[0] + v[0], y - u[1] + v[1], 0.0f), D3DCOLOR_RGBA(1, 0, 0, 0), D3DXFROMWINEVECTOR2(0.0f, 1.0f));


      IRenderer::GetRendererInstance()->SetTexture(0, 0);
      IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
      IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
      IRenderer::GetRendererInstance()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

      IRenderer::GetRendererInstance()->SetPixelShader(0);


      // The water ghost is a goofy thing that bounces around the perturbing the water surface.
      // This is just done to make the water look more lively than a sheet of glass.
      if(m_enableWaterGhost)
        {
          float ghostTime;

          ghostTime = m_time * 1.5f;

          x = 0.5f * sin(ghostTime);
          y = -0.5f * cos(0.7f * ghostTime);
          v[0] = -0.05f * cos(ghostTime);
          v[1] = -0.05f * sin(ghostTime);
          u[0] = -0.1f * -sin(ghostTime);
          u[1] = -0.1f * cos(ghostTime);
          drawQuad( 
                   D3DXFROMWINEVECTOR3(x - u[0] - v[0], y - u[1] - v[1], 0.0f), D3DCOLOR_RGBA(128, 0, 255, 0), D3DXFROMWINEVECTOR2(0.0f, 0.0f),
                   D3DXFROMWINEVECTOR3(x + u[0] - v[0], y + u[1] - v[1], 0.0f), D3DCOLOR_RGBA(128, 0, 255, 0), D3DXFROMWINEVECTOR2(1.0f, 0.0f),
                   D3DXFROMWINEVECTOR3(x + u[0] + v[0], y + u[1] + v[1], 0.0f), D3DCOLOR_RGBA(128, 0, 255, 0), D3DXFROMWINEVECTOR2(1.0f, 1.0f),
                   D3DXFROMWINEVECTOR3(x - u[0] + v[0], y - u[1] + v[1], 0.0f), D3DCOLOR_RGBA(128, 0, 255, 0), D3DXFROMWINEVECTOR2(0.0f, 1.0f));
        }

      IRenderer::GetRendererInstance()->SetVertexShader(0);
    }

  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, TRUE);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
  hr = IRenderer::GetRendererInstance()->SetRenderTarget(0, m_backBufferRenderTarget);
}










////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//
//               9.0c sample framework methods
//
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D IRenderer::GetRendererInstance() has been 
// created, which will happen during application initialization and windowed/full screen 
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these 
// resources need to be reloaded whenever the IRenderer::GetRendererInstance() is destroyed. Resources created  
// here should be released in the OnDestroyDevice callback. 
//--------------------------------------------------------------------------------------
int Water::OnCreateDevice()
{
  int     hr;
  int         fontHeight;
  D3DXFROMWINEMATRIX  sunkenBoatMatrix;
  float       scale;
  D3DXFROMWINEVECTOR3 viewerPosition;
  std::string fullpath; 

  // Record the width and height
  m_deviceWidth = pBackBufferSurfaceDesc->Width;
  m_deviceHeight = pBackBufferSurfaceDesc->Height;

  // Load the heightmap
  m_landScale[0] = 50.0f;
  m_landScale[1] = 20.0f;
  m_landScale[2] = 50.0f;
  FindFile("ground.tga", fullpath);
  m_land = new HeightMap(fullpath.c_str(), D3DXFROMWINEVECTOR3(m_landScale[0], m_landScale[1], m_landScale[2]), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f));

  // Create the vertex buffer and index buffer for the heightmap
  m_land->createVertexBuffer(IRenderer::GetRendererInstance());
  m_land->createIndexBuffer(IRenderer::GetRendererInstance());

  // Create a texture out of the heightmap
  m_land->createHeightTexture( m_landTexture);
  m_land->setTexture(LoadTexture("groundColor.tga"));

  // Initialize the viewer
  viewerPosition = D3DXFROMWINEVECTOR3(5.0f, 5.0f, 5.0f);
  m_viewer.set(viewerPosition, 1.3f * D3DXFROMWINE_PI, 0.0f);

  // Initialize the font
  hDC = GetDC(0);
  fontHeight = -MulDiv(12, GetDeviceCaps(hDC, LOGPIXELSY), 72);
  ReleaseDC(0, hDC);
  if(FAILED(hr = D3DXFROMWINECreateFont( fontHeight, 0, FW_BOLD, 0, FALSE, 
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
                                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                                ("Arial"), &m_font)))
    {
      return(DXTRACE_ERR("D3DXFROMWINECreateFont", hr));
    }
  
  // Initialize all of the vertex and pixel shaders
  InitShaders();
                    
  // Set the light position
  m_lightPositionWorld = D3DXFROMWINEVECTOR3(25.0f, 25.0f, 25.0f);


  // Create the vertex buffer and index buffer for the wave simulation
  m_waveSimulationGrid.resize(m_waveSimulationWidth, m_waveSimulationHeight);
  m_waveSimulationGrid.clear(0.0f);

  assert(m_land != 0);
  m_waveSimulationGrid.createVertexAndIndexBuffers( m_waveSimulationVertexBuffer, m_waveSimulationIndexBuffer, 
                                                   D3DXFROMWINEVECTOR3(25.0f, 10.0f, 25.0f), D3DXFROMWINEVECTOR3(25.0f, 0.0f, 25.0f), m_land);


  // Load texture to perturb the wave simulation
  m_wakeTexture = LoadTexture("brush.tga");
    
  // Load the skybox textures
  m_skyBoxTextures[0] = LoadTexture("CloudyHills_negz.tga");
  m_skyBoxTextures[1] = LoadTexture("CloudyHills_negx.tga");
  m_skyBoxTextures[2] = LoadTexture("CloudyHills_posz.tga");
  m_skyBoxTextures[3] = LoadTexture("CloudyHills_posx.tga");
  m_skyBoxTextures[4] = LoadTexture("CloudyHills_posy.tga");
  m_skyBoxTextures[5] = LoadTexture("groundColor.tga");
  
  // Load the texture used for fading
  m_fadeTexture = LoadTexture("fade.tga");

  // Load the boat mesh
  FindFile("boat4.x", fullpath);
  m_boatMesh.initialize(fullpath.c_str(), IRenderer::GetRendererInstance());

  // Load the sunken boat mesh
  m_sunkenBoatMesh.initialize(fullpath.c_str(), IRenderer::GetRendererInstance());
  scale = 1.0f / 3.5f;
  sunkenBoatMatrix._11 = 0.5f * scale; sunkenBoatMatrix._12 = 0.3f  * scale; sunkenBoatMatrix._13 = 0.0f * scale; sunkenBoatMatrix._14 = 0.0f;
  sunkenBoatMatrix._21 = 0.3f * scale; sunkenBoatMatrix._22 = -0.5f * scale; sunkenBoatMatrix._23 = 0.0f * scale; sunkenBoatMatrix._24 = 0.0f;
  sunkenBoatMatrix._31 = 0.0f * scale; sunkenBoatMatrix._32 = 0.0f  * scale; sunkenBoatMatrix._33 = 0.5f * scale; sunkenBoatMatrix._34 = 0.0f;
  sunkenBoatMatrix._41 = 25.0f;        sunkenBoatMatrix._42 =-5.5f;          sunkenBoatMatrix._43 = 25.0f; sunkenBoatMatrix._44 = 1.0f;
  m_sunkenBoatMesh.setWorldMatrix(sunkenBoatMatrix);

  // Load the boat texture
  m_boatTexture = LoadTexture("boat4.dds");

  // Create matrix stacks
  D3DXFROMWINECreateMatrixStack(0, &m_worldMatrixStack);
  D3DXFROMWINECreateMatrixStack(0, &m_viewMatrixStack);
  D3DXFROMWINECreateMatrixStack(0, &m_projectionMatrixStack);

  // By default, we don't want to reflect the view transform
  m_reflectViewMatrix = false;

  return 0;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D IRenderer::GetRendererInstance() has been 
// reset, which will happen after a lost IRenderer::GetRendererInstance() scenario. This is the best location to 
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever 
// the IRenderer::GetRendererInstance() is lost. Resources created here should be released in the OnLostDevice 
// callback. 
//--------------------------------------------------------------------------------------
int Water::OnResetDevice(const D3DSURFACE_DESC* pBackBufferSurfaceDesc)
{
  int    hr;
  D3DXFROMWINEMATRIX viewMatrix;

  // Record the width and height
  m_deviceWidth = pBackBufferSurfaceDesc->Width;
  m_deviceHeight = pBackBufferSurfaceDesc->Height;

  // Reset some basic state
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, TRUE);
  IRenderer::GetRendererInstance()->SetRenderState(D3DRS_LIGHTING, FALSE);

  // Compute the aspect ratio
  m_aspectRatio = ((float)m_deviceWidth) / ((float)m_deviceHeight);

  // Load the world matrix with the identity matrix
  m_worldMatrixStack->LoadIdentity();

  // Set the view matrix
  m_viewer.computeViewMatrix((*m_viewMatrixStack->GetTop()));

  // Set the projection matrix
  D3DXFROMWINEMatrixPerspectiveFovRH(&m_projectionMatrix, m_fieldOfView, m_aspectRatio, 1.0f, 100.0f);
  D3DXFROMWINEMatrixPerspectiveFovRH(&m_reflectionProjectionMatrix, m_fieldOfView, m_aspectRatio, 1.0f, 100.0f);
  D3DXFROMWINEMatrixPerspectiveFovRH(&m_refractionProjectionMatrix, m_fieldOfView, m_aspectRatio, 1.0f, 100.0f);
  m_projectionMatrixStack->LoadMatrix(&m_projectionMatrix);
  
  // Set the fixed function transform and vertex shader constants
  SetMatrices();

  // Get a surface for the backbuffer
  hr = IRenderer::GetRendererInstance()->GetRenderTarget(0, &m_backBufferRenderTarget);
  assert(hr == D3D_OK);
  assert(m_backBufferRenderTarget != 0);

  // Create a render target for rendering the world when underwater
  hr = D3DXFROMWINECreateTexture( m_deviceWidth, m_deviceHeight, 1, 
                         D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_underwaterWorldRenderTargetTexture);
  assert(hr == D3D_OK);

  hr = m_underwaterWorldRenderTargetTexture->GetSurfaceLevel(0, &m_underwaterWorldRenderTargetSurface);
  assert(hr == D3D_OK);  

  // Reset the font
  if(m_font)
    {
      m_font->OnResetDevice();
    }

  // Create the vertex buffer for drawing quads
  hr = IRenderer::GetRendererInstance()->CreateVertexBuffer(4 * sizeof(Vertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
                                        Vertex::FVF, D3DPOOL_DEFAULT, &m_quadVertexBuffer, 0);

  // Restore all of the stuff related to the water simulation and rendering
  RestoreWaterSimulationObjects();
  RestoreWaterRenderingObjects();

  return(0);
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, the sample framework will call 
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void Water::OnFrameMove(double time, float elapsedTime)
{
  D3DXFROMWINEVECTOR3 position;
  float       height;
  D3DXFROMWINEMATRIX  boatMatrix;
  float       x[3], y[3], z[3];
  float       positionX, positionY, positionZ;
  float       boatScale[3] = {-0.5f / 3.5f, 0.8f / 3.5f, 0.5f / 3.5f};


  // Compute the delta time
  if(m_tLastFrame >= 0.0f)
    {
      m_dt = (float)(time - (double)m_tLastFrame);
      m_time += m_dt;
    }
  m_tLastFrame = (float)time;


  // Handle viewer movement
  if(KeyIsDown('W') || KeyIsDown(VK_UP))
    {
      m_viewer.moveForward(0.1f);        
    }
  if(KeyIsDown('S') || KeyIsDown(VK_DOWN))
    {
      m_viewer.moveForward(-0.1f);        
    }
  if(KeyIsDown('A') || KeyIsDown(VK_LEFT))
    {
      m_viewer.moveRight(-0.1f);
    }
  if(KeyIsDown('D') || KeyIsDown(VK_RIGHT))
    {
      m_viewer.moveRight(0.1f);
    }
  if(KeyIsDown('E'))
    {
      m_height += 0.1f;
    }
  if(KeyIsDown('Q'))
    {
      m_height -= 0.1f;
    }

  // Stand the viewer on top of the terrain
  if(m_walkOnGround)
    {
      position = m_viewer.getPosition();
      height = position.y = m_land->height(position.x, position.z) + 2.0f;
    }
  else
    {
      position = m_viewer.getPosition();
      height = position.y = m_height;
    }

  // Force the view to stay in the world
  if(position.x < 2.5f)
    {
      position.x = 2.5f;
    }
  else if(position.x >= 47.5f)
    {
      position.x = 47.5f;
    }
  if(position.z < 2.5f)
    {
      position.z = 2.5f;
    }
  else if(position.z >= 47.5f)
    {
      position.z = 47.5f;
    }
  m_viewer.setPosition(position);


  // Compute and setup transforms
  m_viewer.computeViewMatrix((*m_viewMatrixStack->GetTop()));
  SetMatrices();

  // Check to see if the viewer is under water
  if(height < 0.0f)
    {
      m_underwater = true;
    }
  else
    {
      m_underwater = false;
    }


  // Move the boat
  x[0] = boatScale[0] * cos(m_time + 1.57f);
  x[1] = boatScale[0] * 0.0f;
  x[2] = boatScale[0] * sin(m_time + 1.57f);
  y[0] = boatScale[1] * 0.0f;
  y[1] = boatScale[1];
  y[2] = boatScale[1] * 0.0f;
  z[0] = boatScale[2] * -sin(m_time + 1.57f);
  z[1] = boatScale[2] * 0.0f;
  z[2] = boatScale[2] * cos(m_time + 1.57f);
  positionX = 25.0f + 12.5f * cos(m_time - 1.57f);
  positionY = 0.3f + 0.2f * cos(3.0f * m_time);
  positionZ = 25.0f + 12.5f * sin(m_time - 1.57f);

  boatMatrix = D3DXFROMWINEMATRIX(x[0],  x[1],  x[2],  0.0f,
                          y[0],  y[1],  y[2],  0.0f,
                          z[0],  z[1],  z[2],  0.0f,
                          positionX, positionY, positionZ, 1.0f);

  m_boatMesh.setWorldMatrix(boatMatrix);
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, the sample framework will call 
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void Water::OnFrameRender(double time, float elapsedTime)
{
  // If the settings dialog is being shown, then
  // render it instead of rendering the app's scene
  if( m_SettingsDlg.IsActive() )
  {
    m_SettingsDlg.OnRender( elapsedTime );
    return;
  }

  // Begin the scene
  if (SUCCEEDED(IRenderer::GetRendererInstance()->BeginScene()))
    {
      // Do the wave equation simulation for the water
      SimulateWaveEquation();

      // Render the scene into a refraction texture
      RenderReflectionTexture();

      // Render the scene into a reflection texture
      RenderRefractionTexture();

      // Render the scene to the backbuffer
      RenderBackbuffer();

      // Render the surface of the water to the backbuffer
      RenderWater();

      // render informative junk
      RenderInformation();
      
      // Now that we're done, go home and chug a 40oz.
      IRenderer::GetRendererInstance()->EndScene();

      // Do some miscellaneous book-keeping
      if(m_freezeSimulation == false)
        {
          m_waveSimulationIndex++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//                           Global functions
//
////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// 
// Render a quad
//
//-----------------------------------------------------------------------------
int drawQuad( IRenderer::GetRendererInstance(), const D3DXFROMWINEVECTOR3& position0, D3DCOLOR color0, const D3DXFROMWINEVECTOR2& texCoord0,
                 const D3DXFROMWINEVECTOR3& position1, D3DCOLOR color1, const D3DXFROMWINEVECTOR2& texCoord1,
                 const D3DXFROMWINEVECTOR3& position2, D3DCOLOR color2, const D3DXFROMWINEVECTOR2& texCoord2,
                 const D3DXFROMWINEVECTOR3& position3, D3DCOLOR color3, const D3DXFROMWINEVECTOR2& texCoord3)
{
  Vertex  vertices[4];
  int hr;
  void*   buffer;

  hr = IRenderer::GetRendererInstance()->SetFVF(Vertex::FVF);
  assert(hr == D3D_OK);
  
  // Load the vertex data
  vertices[0].position = position0;
  vertices[0].diffuse = color0;
  vertices[0].tex0[0] = texCoord0.x;
  vertices[0].tex0[1] = texCoord0.y;

  vertices[1].position = position1;
  vertices[1].diffuse = color1;
  vertices[1].tex0[0] = texCoord1.x;
  vertices[1].tex0[1] = texCoord1.y;

  vertices[2].position = position2;
  vertices[2].diffuse = color2;
  vertices[2].tex0[0] = texCoord2.x;
  vertices[2].tex0[1] = texCoord2.y;

  vertices[3].position = position3;
  vertices[3].diffuse = color3;
  vertices[3].tex0[0] = texCoord3.x;
  vertices[3].tex0[1] = texCoord3.y;

  // Copy the vertex data into the vertex buffer
  hr = Water::m_quadVertexBuffer->Lock(0, 0, &buffer, D3DLOCK_DISCARD);
  CHECK_RETURN_CODE("drawQuad  Water::m_quadVertexBuffer->Lock()", hr);
  
  stx_memcpy(buffer, vertices, 4 * sizeof(Vertex));
  
  Water::m_quadVertexBuffer->Unlock();

  // Render the vertex buffer
  IRenderer::GetRendererInstance()->SetStreamSource(0, Water::m_quadVertexBuffer, 0, sizeof(Vertex));

  hr = IRenderer::GetRendererInstance()->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
  CHECK_RETURN_CODE("drawQuad  DrawPrimitive()", hr);  

  return(hr);
}

//-----------------------------------------------------------------------------
// 
// Compute a random color
//
//-----------------------------------------------------------------------------
D3DCOLOR randomColor(void)
{
  return(D3DCOLOR_RGBA(rand() & 0xFF, rand() & 0xFF, rand() & 0xFF, rand() & 0xFF));
}

