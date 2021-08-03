//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "App.h"
#include "common.h"
#include "R2VB.h"
#include "physics.h"
#include "CPUphysics.h"
#include "GPUphysics.h"

// GUI slider positions
#define SLIDER_X_OFFSET 40.0f
#define SLIDER_Y_OFFSET 40.0f
#define SLIDER_WIDTH 200.0f
#define SLIDER_X_SEPERATOR 20.0f
#define LABEL_Y_OFFSET 60.0f

// stream 0 position data
struct WATER_VERTEX
{
    D3DXVECTOR4 p;
};

// stream 1 texture coordinate data
struct WATER_TEXCOORD
{
    D3DXVECTOR2 uv;
};

D3DVERTEXELEMENT9 WaterDecl[] = 
{
    {0, 0,  D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, 
                                      D3DDECLUSAGE_POSITION, 0},
    {1, 0,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, 
                                      D3DDECLUSAGE_TEXCOORD, 0},
    D3DDECL_END()
};



//---------------------------------------------------------------------------
/*
    Welcome to the R2VB-OceanWater sample source!  This sample demonstrates
    the use of Render to Vertex Buffer and an example of general purpose 
    use of a graphics processor. In this case, ocean water wave synthesis
    in the frequency domain and an inverse FFT.

    Functions of interest:
        - App::DrawFrame() this is where the render to vertex buffer texture
            is set as a vertex buffer.
        - App::onReset(), App::onPreReset() these functions are responsible
            for initializing and releasing render to vertex buffer data.

    The render to vertex buffer technique is implemented in this sample using
    the following helper functions defined in r2vb.cpp:
        - r2vbCheckCap() checks to see if this device supports render to vb
        - r2vbInit() initializes render to vb
        - r2vbTerm() releases render to vb data
        - r2vbSetStreamTexSource() sets the vertex buffer from a texture
        - r2vbDisTexSource() disables the previously set r2vb stream
*/
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Create an instance of the app.
//---------------------------------------------------------------------------
BaseApp *app = new App();

//---------------------------------------------------------------------------
// App::initGUI
//
//  GUI setup
//---------------------------------------------------------------------------
void App::initGUI()
{
    char *str = new char[128];

    menu = mainMenu->addMenu("R2VB Ocean Water");
    m_useR2VBMenuID = menu->addItem("Use Render to Vertex Buffer", MENU_CHECKABLE);
    m_useGPUforWavesMenuID = menu->addItem("Use GPU for wave synthesis", MENU_CHECKABLE);
    m_wireFrameMenuID = menu->addItem("Wireframe", MENU_CHECKABLE);

    // initial window width and height
    width  = 800;
    height = 600;

    // initial wind velocity
    m_windSpeed = 10;
    m_windDirection = 90;

    // create the wind direction slider
    addWidget(m_windDirSlider = new Slider(SLIDER_X_OFFSET, height - SLIDER_Y_OFFSET, 
        SLIDER_WIDTH, 20.0f, true, 0.0f, 360.0f, m_windDirection, 10.0));
    sprintf(str, "Wind Direction = %d", (int)m_windDirection);
    addWidget(m_windDirLabel = new Label(SLIDER_X_OFFSET, height - LABEL_Y_OFFSET, &mainFont, str, 12.0f, 18.0f)); 

    // create the wind speed slider
    addWidget(m_windSpeedSlider = new Slider(SLIDER_X_OFFSET + SLIDER_WIDTH + SLIDER_X_SEPERATOR, 
        height - SLIDER_Y_OFFSET, SLIDER_WIDTH, 20.0f, true, 0.0f, 15.0f, m_windSpeed, 1.0f));
    sprintf(str, "Wind Speed = %d", (int)m_windSpeed);
    addWidget(m_windSpeedLabel = new Label(SLIDER_WIDTH + SLIDER_X_SEPERATOR, height - LABEL_Y_OFFSET, &mainFont, str, 12.0f, 18.0f)); 
    delete []str;

    D3D9App::initGUI();
}

//---------------------------------------------------------------------------
// App setup
//
// This is called from the sample's framework prior to creating any D3D
// objects.
//---------------------------------------------------------------------------
void App::setup(char *args[], int nArguments)
{
    antiAliasSamples = 4;
    m_r2VB_Initialized = false;
    m_hR2VB = NULL;
    m_useGPUforWaves = true;
    m_useR2VB = true;
    m_wireFrame = false;
    m_GPU_Physics = NULL;
    m_CPU_Physics = NULL;

    // ocean water surface tile translations
    m_matTile[0] = translate( 0.0f,  0.0f,  0.0f);
    m_matTile[1] = translate(-2.0f,  0.0f, -2.0f);
    m_matTile[2] = translate(-2.0f,  0.0f,  0.0f);
    m_matTile[3] = translate(-2.0f,  0.0f,  2.0f);
    m_matTile[4] = translate( 0.0f,  0.0f, -2.0f);
    m_matTile[5] = translate( 0.0f,  0.0f,  2.0f);
    m_matTile[6] = translate( 2.0f,  0.0f, -2.0f);
    m_matTile[7] = translate( 2.0f,  0.0f,  0.0f);
    m_matTile[8] = translate( 2.0f,  0.0f,  2.0f);

    width  = 800;
    height = 600;

}

//---------------------------------------------------------------------------
// App keyboard handler
//
// This is called from the sample's framework to intercept keyboard signals.
//---------------------------------------------------------------------------
bool App::onKeyPress(const unsigned int key, const bool pressed)
{
    if (pressed){
        if (key >= '1' && key <= '3'){
            menu->toggleItemCheck(key - '1');
            return true;
        }
    }
    return D3D9App::onKeyPress(key, pressed);
}

//---------------------------------------------------------------------------
// App onSize()
//
// This is called by the sample framework when the window is resized
//---------------------------------------------------------------------------
void App::onSize(const int w, const int h)
{
    D3D9App::onSize(w, h);

    // Adjust the slider positions
    m_windDirSlider->setPosition(SLIDER_X_OFFSET, h - SLIDER_Y_OFFSET);
    m_windDirLabel->setPosition(SLIDER_X_OFFSET, h - LABEL_Y_OFFSET);
    m_windSpeedSlider->setPosition(SLIDER_X_OFFSET + SLIDER_WIDTH + SLIDER_X_SEPERATOR, h - SLIDER_Y_OFFSET);
    m_windSpeedLabel->setPosition(SLIDER_X_OFFSET + SLIDER_WIDTH + SLIDER_X_SEPERATOR, h - LABEL_Y_OFFSET);

    if (dev)
    {
        // change the viewport
        m_viewport.Y = 0;
        m_viewport.X = 0;
        m_viewport.Width = w;
        m_viewport.Height = h;
        m_viewport.MinZ = 0.0f;
        m_viewport.MaxZ = 1.0f;
        dev->SetViewport(&m_viewport);
    }
}

//---------------------------------------------------------------------------
// resetCamera
//---------------------------------------------------------------------------
void App::resetCamera()
{
    camPos = float3(0, 5, 0);
    wx = 0;
    wy = PI / 2;
    speed = 0;      // disable camera movement
}

//---------------------------------------------------------------------------
// App onReset()
//
// This is called by the sample framework upon a device reset.
//---------------------------------------------------------------------------
bool App::onReset()
{
    HRESULT hr;

    // Initialize the physics objects
    m_GPU_Physics = new GPU_Physics(dev, renderer);
    m_CPU_Physics = new CPU_Physics(dev, renderer);

   // initialize render to vb
   if (!m_r2VB_Initialized)
   {
        V( r2vbInit(dev, WATER_GEOM_RESOLUTION * WATER_GEOM_RESOLUTION * sizeof(WATER_VERTEX), &m_hR2VB) );
        m_r2VB_Initialized = true;
   }

    // Fill in utility non-renderable textures and CPU-side buffers for FFTW computation
    m_CPU_Physics->FillWaterUtilityBuffers(m_windSpeed, m_windDirection);

    // Fill in the textures used by the GPU to simulate the wave physics
    m_GPU_Physics->FillWaterUtilityBuffers(m_windSpeed, m_windDirection);

    // Create a texture for use when render to VB is disabled
    V( D3DXCreateTexture (dev, WATER_GEOM_RESOLUTION, WATER_GEOM_RESOLUTION, 1,
                            0, D3DFMT_A32B32G32R32F, D3DPOOL_SYSTEMMEM,
                            &m_pSysMemLowSpatialWaterTex ) );

    return true;
}

//---------------------------------------------------------------------------
// App onPreReset()
//
// This is called by the sample framework before a device reset. We use this
// function to delete all of the non-managed objects before re-creating them
// during the reset.
//---------------------------------------------------------------------------
bool App::onPreReset()
{
    if (m_GPU_Physics)
    {
        delete(m_GPU_Physics);
        m_GPU_Physics = NULL;
    }

    if (m_CPU_Physics)
    {
        delete(m_CPU_Physics);
        m_CPU_Physics = NULL;
    }

    if (m_r2VB_Initialized)
    {
        r2vbDisTexSource(m_hR2VB, 0);
        r2vbTerm(m_hR2VB);
        m_r2VB_Initialized = false;
    }

    SAFE_RELEASE(m_pSysMemLowSpatialWaterTex);

    return true;
}

//---------------------------------------------------------------------------
// App load
//
// This is called from the sample's framework after creating a D3D object and
// a Direct3DDevice9 object.  We commonly use this to load any geometry and
// textures the sample needs.
//
//---------------------------------------------------------------------------
bool App::load()
{
    if (!onReset()) return false;

    // set some default values for the menu items
    menu->enableItem(m_useR2VBMenuID, true);
    menu->checkItem(m_useR2VBMenuID, m_useR2VB);
    menu->enableItem(m_useGPUforWavesMenuID, true);
    menu->checkItem(m_useGPUforWavesMenuID, m_useGPUforWaves);
    menu->enableItem(m_wireFrameMenuID, true);
    menu->checkItem(m_wireFrameMenuID, m_wireFrame);

    // set the viewport
    m_viewport.Y = 0;
    m_viewport.X = 0;
    m_viewport.Width = width;
    m_viewport.Height = height;
    m_viewport.MinZ = 0.0f;
    m_viewport.MaxZ = 1.0f;
    dev->SetViewport(&m_viewport);

    // Check for Render to Vertex Buffer support
    LPDIRECT3D9 pD3D;
    dev->GetDirect3D(&pD3D);
    if FAILED(r2vbCheckCap(pD3D))
    {
        ERRORMSG("This sample will not work because render to vertex buffer is not supported on this device.");
        return false;
    }
    pD3D->Release();

    // Load shaders
    if ((m_waterRenderShader = renderer->addShader("WaterRender.shd")) == SHADER_NONE) 
        return false;
    if ((m_skyboxShader = renderer->addShader("skybox.shd")) == SHADER_NONE) 
        return false;

    // Load Textures
    if ((m_skyboxTexture = renderer->addTexture(TEX_PATH "MarlinCube.dds", FILTER_TRILINEAR, CUBEMAP)) == TEXTURE_NONE) 
        return false;

    // skybox vertex format
    VertexAttribute skyboxAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 3 };
    if ((m_skyboxVertexFormat = renderer->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs))) == VF_NONE) 
        return false;

    // Create the ocean water vertex buffers
    m_waterVertexFormat = ((D3D9Renderer*)renderer)->addVertexFormat( WaterDecl );
    m_waterPosVB = renderer->addVertexBuffer( WATER_GEOM_RESOLUTION * WATER_GEOM_RESOLUTION * sizeof(WATER_VERTEX),
                   NULL, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC );
    m_waterTexCoordVB = renderer->addVertexBuffer( WATER_GEOM_RESOLUTION * WATER_GEOM_RESOLUTION * sizeof(WATER_TEXCOORD),
                   NULL, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC );

    // Create the ocean water index buffer
    m_waterIB = renderer->addIndexBuffer((WATER_GEOM_RESOLUTION-1) * (WATER_GEOM_RESOLUTION-1) * 6,
                sizeof(WORD), NULL, D3DUSAGE_WRITEONLY);

    // Fill the VBs and IBs with data
    FillWaterGeometry();

    return true;
}

//---------------------------------------------------------------------------
// unload
//---------------------------------------------------------------------------
bool App::unload()
{
    // do the same clean up as before a reset
    return onPreReset();
}


//---------------------------------------------------------------------------
// FillWaterGeometry
//
// Fills the vertex buffers used for the water geometry
//
//---------------------------------------------------------------------------
HRESULT App::FillWaterGeometry()
{
   HRESULT hr = S_OK;
   int x, z, i, j;
   DWORD curVertex = 0;

   //
   // Lock and fill the main water grid vertex buffer
   // Water grid is an x,z plane with y up
   //
   WATER_VERTEX* pWV;
   WATER_TEXCOORD* pWTexCd; 
   pWV = (WATER_VERTEX *)renderer->lockVertexBuffer(m_waterPosVB);
   pWTexCd = (WATER_TEXCOORD *)renderer->lockVertexBuffer(m_waterTexCoordVB);

   for (x = 0; x < WATER_GEOM_RESOLUTION; x++)
   {
      for (z = 0; z < WATER_GEOM_RESOLUTION; z++)
      {
        // Position xy of grid in -1..+1 range at z=0
        float fX = (((float)x) / ((float)(WATER_GEOM_RESOLUTION-2))) * 2.0f - 1.0f;
        float fY = 0.0f;
        float fZ = (((float)z) / ((float)(WATER_GEOM_RESOLUTION-2))) * 2.0f - 1.0f;

        float fU, fV;

        // Texture coordinates
        fU = REPEAT_RATE * ((float)z / ((float)(WATER_GEOM_RESOLUTION)));
        fV = REPEAT_RATE * ((float)x / ((float)(WATER_GEOM_RESOLUTION)));

        pWV[curVertex].p  = D3DXVECTOR4(fX, fY, fZ, 1);
        pWTexCd[curVertex].uv = D3DXVECTOR2(fU, fV);

        curVertex++;
      }
   }

   renderer->unlockVertexBuffer(m_waterPosVB);
   renderer->unlockVertexBuffer(m_waterTexCoordVB);

   //
   // Lock and fill the main water grid index buffer
   //
   WORD* pIndices;
   pIndices = (WORD *)renderer->lockIndexBuffer(m_waterIB);

   for( j=0; j < (WATER_GEOM_RESOLUTION-1); j++ )
   {
      int baseIndex = j*WATER_GEOM_RESOLUTION;

      for( i=0; i < (WATER_GEOM_RESOLUTION-1); i++ )
      {
            *pIndices++ = (WORD)( baseIndex + i );
            *pIndices++ = (WORD)( baseIndex + i + WATER_GEOM_RESOLUTION );
            *pIndices++ = (WORD)( baseIndex + i + 1 );

            *pIndices++ = (WORD)( baseIndex + i + 1 );
            *pIndices++ = (WORD)( baseIndex + i + WATER_GEOM_RESOLUTION );
            *pIndices++ = (WORD)( baseIndex + i + WATER_GEOM_RESOLUTION + 1 );
         }
      }

   renderer->unlockIndexBuffer(m_waterIB);

   return hr;
}

//-----------------------------------------------------------------------------
//
// UpdateWaterGeometryCPU()
//
// Updates the water geometry based on the height field
//-----------------------------------------------------------------------------
HRESULT App::UpdateWaterGeometryCPU()
{
   HRESULT hr = S_OK;
   int x, z;
   DWORD curVertex = 0;
   complex_number *pHeightFieldRow, *pHeightField;


   pHeightField = (complex_number *)m_CPU_Physics->GetDisplacementMap();

   //
   // Lock and update the main water grid vertex buffer
   // Water grid is an x,z plane with y up
   //
   WATER_VERTEX* pWV;
   pWV = (WATER_VERTEX *)renderer->lockVertexBuffer(m_waterPosVB);

   for (z = 0; z < WATER_GEOM_RESOLUTION; z++)
   {
      pHeightFieldRow = pHeightField + ((z % WATER_GRID_RESOLUTION) * WATER_GRID_RESOLUTION);
      for (x = 0; x < WATER_GEOM_RESOLUTION; x++)
      {
         // Position xy of grid in -1..+1 range at z=0
         float fX = (((float)x) / ((float)(WATER_GEOM_RESOLUTION-2))) * 2.0f - 1.0f;
         float fY = fabs(c_re(pHeightFieldRow[x % WATER_GRID_RESOLUTION])* (HEIGHT_SCALE));
         float fZ = (((float)z) / ((float)(WATER_GEOM_RESOLUTION-2))) * 2.0f - 1.0f;
         pWV[curVertex].p  = D3DXVECTOR4(fX, fY, fZ, 1);
         curVertex++;
      }
   }

   renderer->unlockVertexBuffer(m_waterPosVB);

   return hr;
}

//-----------------------------------------------------------------------------
//
// UpdateWaterGeometryGPU()
//
// Updates the water geometry based on the height field which was generated on
// the GPU and stored in a render target.
//-----------------------------------------------------------------------------
HRESULT App::UpdateWaterGeometryGPU(IDirect3DTexture9 *displacementMap)
{
    HRESULT hr = S_OK;
    int x, z;
    DWORD curVertex = 0;
    IDirect3DSurface9 *pHeightField;
    IDirect3DSurface9 *pRenderTargetSurface;

    displacementMap->GetSurfaceLevel(0, &pRenderTargetSurface);

    m_pSysMemLowSpatialWaterTex->GetSurfaceLevel(0, &pHeightField);

    // Get the height field data
    V_RETURN(dev->GetRenderTargetData(pRenderTargetSurface, pHeightField));
    D3DLOCKED_RECT lockedRect;
    V_RETURN(pHeightField->LockRect(&lockedRect, NULL, 0));
    struct Tex32RGBA {float x,y,z,w;};
    Tex32RGBA *pRGBAData = NULL;

     pRGBAData = (Tex32RGBA *)lockedRect.pBits;

    //
    // Lock and update the main water grid vertex buffer
    // Water grid is an x,z plane with y up
   //
    WATER_VERTEX* pWV;
    pWV = (WATER_VERTEX *)renderer->lockVertexBuffer(m_waterPosVB);

    for (z = 0; z < WATER_GEOM_RESOLUTION; z++)
    {
        for (x = 0; x < WATER_GEOM_RESOLUTION; x++)
        {
            // Position xy of grid in -1..+1 range at z=0
            float fX = -((((float)x) / ((float)(WATER_GEOM_RESOLUTION-2))) * 2.0f - 1.0f);
            float fY;
            if (pRGBAData)
            {
                fY = pRGBAData->y;
                pRGBAData++;
            }
            else
            {
                fY = 0;
            }
            float fZ = ((((float)z) / ((float)(WATER_GEOM_RESOLUTION-2))) * 2.0f - 1.0f);
            pWV[curVertex].p  = D3DXVECTOR4(fX, fY, fZ, 1);
            curVertex++;
        }
    }

    V_RETURN(pHeightField->UnlockRect();)
    pHeightField->Release();
    pRenderTargetSurface->Release();
    renderer->unlockVertexBuffer(m_waterPosVB);

    return hr;
}

//---------------------------------------------------------------------------
// drawSkybox
//---------------------------------------------------------------------------
void App::drawSkybox(const float4x4 &mvp){
    static const float verts[] = { -8,-8,-8, -8,-8,8, -8,8,-8, -8,8,8, 8,-8,-8, 8,-8,8, 8,8,-8, 8,8,8 };
    static const unsigned short inds[] = { 2,3,0, 0,3,1, 3,7,1, 1,7,5, 7,6,5, 5,6,4, 6,2,4, 4,2,0, 6,7,2, 2,7,3, 0,1,4, 4,1,5 };

    renderer->setShader(m_skyboxShader);
    renderer->setVertexFormat(m_skyboxVertexFormat);
    renderer->setTexture("Skybox", m_skyboxTexture);
    renderer->setDepthFunc(DEPTH_NONE);
    renderer->apply();

    renderer->changeShaderConstant4x4f("mvp", mvp);

    dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 12, inds, D3DFMT_INDEX16, verts, 3 * sizeof(float));
}

//---------------------------------------------------------------------------
// App drawFrame
//
//---------------------------------------------------------------------------
void App::drawFrame()
{
    HRESULT hr;
    int tile;
    float fTime;
    IDirect3DTexture9 *normalMap;
    IDirect3DTexture9 *displacementMap;
    float lastWindSpeed = m_windSpeed;
    float lastWindDirection = m_windDirection;

    // initialize booleans from the menu
    m_wireFrame = menu->isItemChecked(m_wireFrameMenuID);
    m_useGPUforWaves = menu->isItemChecked(m_useGPUforWavesMenuID);
    if (!m_useGPUforWaves)
    {
        menu->enableItem(m_useR2VBMenuID, false);
        m_useR2VB = false;
    }
    else
    {
        menu->enableItem(m_useR2VBMenuID, true);
        m_useR2VB = menu->isItemChecked(m_useR2VBMenuID);
    }

    // initialize the wind velocity from the sliders
    m_windDirection = m_windDirSlider->getValue();
    m_windSpeed = m_windSpeedSlider->getValue();
    if (m_windSpeed != lastWindSpeed || m_windDirection != lastWindDirection)
    {
        // update the labels
        char *str = new char[128];
        sprintf(str, "Wind Direction = %d", (int)m_windDirection);
        m_windDirLabel->setText(str);
        sprintf(str, "Wind Speed = %d", (int)m_windSpeed);
        m_windSpeedLabel->setText(str);
        delete []str;

        // re-compute the wave input buffers based on the new information
        m_CPU_Physics->FillWaterUtilityBuffers(m_windSpeed, m_windDirection);
        m_GPU_Physics->FillWaterUtilityBuffers(m_windSpeed, m_windDirection);
    }

    // set up the transformation matrices
    float4x4 projection = perspectiveMatrixX(PI / 2, width, height, 1, 2000, true);
    float4x4 modelview = rotateXY4(-wx , -wy);
    float4x4 transMtx = translate(0, -0.125f, 0);
    float4x4 mvpSB = projection * modelview * transMtx;
    modelview.translate(-camPos);
    float4x4 mvp = projection * modelview;

    dev->SetViewport(&m_viewport);

    // clear the render target
    dev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );

    // Draw the skybox
    drawSkybox(mvpSB);

    // Compute the waves
    fTime = getTime();
    if (m_useGPUforWaves)
    {
        // GPU Wave Generation
        m_GPU_Physics->ComputeWaves(m_windSpeed, m_windDirection, fTime);
        displacementMap = (IDirect3DTexture9 *)m_GPU_Physics->GetDisplacementMap();
        if (!m_useR2VB) 
            UpdateWaterGeometryGPU(displacementMap); // without render to VB
        normalMap = m_GPU_Physics->GetNormalMap();
    }
    else
    {
        // CPU Wave Generation
        m_CPU_Physics->ComputeWaves(m_windSpeed, m_windDirection, fTime);
        UpdateWaterGeometryCPU();
        normalMap = m_CPU_Physics->GetNormalMap();
        displacementMap = NULL;
    }

    //
    // Draw water surface tiles
    //
    normalMap->AddDirtyRect(NULL);
    normalMap->GenerateMipSubLevels();

    ((D3D9Renderer *)renderer)->defaults();
    renderer->changeShader(m_waterRenderShader);
    renderer->changeTexture("SkySampler", m_skyboxTexture);

    // Choose appropriate fill mode
    if (menu->isItemChecked(m_wireFrameMenuID))
    {
        V( dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) );
        renderer->changeShaderConstant1f("g_useConstantColor",  1.0f);
    }
    else
    {
        V( dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID) );
        renderer->changeShaderConstant1f("g_useConstantColor",  -1.0f);
    }

    // set the vertex format
    renderer->changeVertexFormat(m_waterVertexFormat);

    if (!m_useGPUforWaves || !m_useR2VB)
    {
        // use the position vertex buffer
        renderer->changeVertexBuffer(m_waterPosVB, 0);
    }

    // set the texture coordinate in stream 1
    renderer->changeVertexBuffer(m_waterTexCoordVB, 1);

    // set the index buffer
    renderer->changeIndexBuffer(m_waterIB);

    // world space eye position
    vec4 eyePos = vec4(camPos, 1.0f);
    renderer->changeShaderConstant4f("g_vWSEyePos",  eyePos);

    // render state
    dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    dev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    dev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    dev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    // set the normal map
    int index = renderer->getSamplerIndex(m_waterRenderShader, "HighFreqWaterNormalSampler");
    dev->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    dev->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    dev->SetTexture(index, normalMap); // High frequency normal map

    if (m_useGPUforWaves && m_useR2VB)
    {
        // set the displacement map as stream source
        V( r2vbSetStreamTexSource(m_hR2VB, 0, 0, displacementMap, 0, sizeof(WATER_VERTEX)) );
    }

    mat4 scaleMtx = scale4(100, 100, 100);
    UINT numVerts = WATER_GEOM_RESOLUTION*WATER_GEOM_RESOLUTION;
    UINT numTris  = (WATER_GEOM_RESOLUTION-1) * (WATER_GEOM_RESOLUTION-1) * 2;

    for (tile=0; tile < NUM_TILES; tile++)
    {
        // composite matrix
        mat4 scaledTile = scaleMtx * m_matTile[tile];
        mat4 wvp = mvp * scaledTile;

        renderer->changeShaderConstant4x4f("g_mWorldViewProjection", wvp);
        renderer->changeShaderConstant4x4f("g_mWorld", scaledTile);

        V( dev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, numVerts, 0, numTris) );
    }

    if (menu->isItemChecked(m_wireFrameMenuID))
    {
        // restore solid fill mode
        V( dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID) );
    }

    if (m_useGPUforWaves && m_useR2VB)
    {
        // disable render to vb
        V( r2vbDisTexSource(m_hR2VB, 0));
    }

    // disable texturing for the UI
    dev->SetTexture(0, NULL);
    dev->SetTexture(1, NULL);
}
