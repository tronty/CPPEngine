//-----------------------------------------------------------------------------
// File: App.h
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _APP_H_
#define _APP_H_

#include "../../Framework/Direct3D/D3D9App.h"
#include "../../Framework/Util/Model.h"
#include "physics.h"
#include "r2VB.h"


//-----------------------------------------------------------------------------
// Class App
// App is the main application class. It is derived from the D3D9App class,
// which is in turn derived from the BaseApp class.
//-----------------------------------------------------------------------------

class App : public D3D9App {
public:
    char *getTitleString(){ return "R2VB Ocean Water"; }
    char *getHomeDirectory(){ return "../DirectX 9.0/R2VB-OceanWater"; }

    void setup(char *args[], int nArguments);
    void initGUI();

    bool onKeyPress(const unsigned int key, const bool pressed);

    bool onPreReset();
    bool onReset();
    void resetCamera();
    bool load();
    bool unload();
    void onSize(const int w, const int h);
    void drawFrame();

private:
    HRESULT FillWaterGeometry();        // fills the vertex buffers
    HRESULT UpdateWaterGeometryCPU();    // modifies positions based on a CPU generated height field
    HRESULT UpdateWaterGeometryGPU(IDirect3DTexture9 *displacementMap);    // modifies positions based on a GPU generated height field
    HRESULT SetTexture(ShaderID shader, char* name, IDirect3DTexture9* texture)
    {
        int index = renderer->getSamplerIndex(shader, name);
        return dev->SetTexture(index, texture);
    }
    void drawSkybox(const float4x4 &mvp);

    R2VBHANDLE m_hR2VB;                 // Render to VB handle
    Physics *m_GPU_Physics;
    Physics *m_CPU_Physics;
    bool m_r2VB_Initialized;            // true if R2VB library is initialized
    bool m_useR2VB;                     // if true, use R2VB for rendering
    bool m_useGPUforWaves;              // if true, use the GPU for physics
    bool m_wireFrame;                   // enables wireframe rendering
    float m_windSpeed;                  // wind speed for waves
    float m_windDirection;              // direction for waves
    mat4 m_matTile[9];                  // Matrices for tiling the water surface
    D3DVIEWPORT9 m_viewport;            // application viewport

    // SDK Framework IDs
    ShaderID m_waterRenderShader;       // shader for rendering the waves
    VertexFormatID m_waterVertexFormat;
    VertexBufferID m_waterPosVB;
    VertexBufferID m_waterTexCoordVB;
    IndexBufferID m_waterIB;
    ShaderID m_skyboxShader;
    VertexFormatID m_skyboxVertexFormat;
    TextureID m_skyboxTexture;

    // System memory texture for the non-render-to-VB case
    IDirect3DTexture9*      m_pSysMemLowSpatialWaterTex;

    // UI member variables
    Menu *menu;
    MenuItemID m_useR2VBMenuID;
    MenuItemID m_useGPUforWavesMenuID;
    MenuItemID m_windSpeedMenuID;
    MenuItemID m_windDirectionID;
    MenuItemID m_wireFrameMenuID;
    Slider *m_windDirSlider;
    Slider *m_windSpeedSlider;
    Label *m_windDirLabel;
    Label *m_windSpeedLabel;
};

#endif
