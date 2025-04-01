/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

#define LOG_FNLN STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "common.h"
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
    D3DXFROMWINEVECTOR4 p;
};

// stream 1 texture coordinate data
struct WATER_TEXCOORD
{
    D3DXFROMWINEVECTOR2 uv;
};

FormatDesc WaterDecl[] = {
	0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
	1, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
};

//---------------------------------------------------------------------------
/*
    Welcome to the R2VB-OceanWater sample source!  This sample demonstrates
    the use of Render to Vertex Buffer and an example of general purpose 
    use of a graphics processor. In this case, ocean water wave synthesis
    in the frequency domain and an inverse FFT.

    Functions of interest:
        - DrawFrame() this is where the render to vertex buffer texture
            is set as a vertex buffer.
        - onReset(), onPreReset() these functions are responsible
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

#include <Util/Model.h>
#include "physics.h"

//-----------------------------------------------------------------------------
// Class App
// App is the main application class. It is derived from the D3D9App class,
// which is in turn derived from the BaseApp class.
//-----------------------------------------------------------------------------

class App {
public:
	int antiAliasSamples;
    int width;
    int height;
    D3DXFROMWINEVECTOR3 camPos;
    float wx;
    float wy;
    float speed;
    bool debugRuntime;
#ifdef R2VB_GUI
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
#endif
#ifdef R2VB_GUI
//---------------------------------------------------------------------------
// App::initGUI
//
//  GUI setup
//---------------------------------------------------------------------------
void initGUI()
{
    char *str = new char[128];

    menu = BaseApp::getMenu()->addMenu("R2VB Ocean Water");
    m_useR2VBMenuID = BaseApp::getMenu()->addItem("Use Render to Vertex Buffer", MENU_CHECKABLE);
    m_useGPUforWavesMenuID = BaseApp::getMenu()->addItem("Use GPU for wave synthesis", MENU_CHECKABLE);
    m_wireFrameMenuID = BaseApp::getMenu()->addItem("Wireframe", MENU_CHECKABLE);

    // initial window width and height
    width  = 800;
    height = 600;

    // initial wind velocity
    m_windSpeed = 10;
    m_windDirection = 90;

    // create the wind direction slider
    BaseApp::addWidget(m_windDirSlider = new Slider(SLIDER_X_OFFSET, height - SLIDER_Y_OFFSET, 
        SLIDER_WIDTH, 20.0f, true, 0.0f, 360.0f, m_windDirection, 10.0));
    sprintf(str, "Wind Direction = %d", (int)m_windDirection);
    BaseApp::addWidget(m_windDirLabel = new Label(SLIDER_X_OFFSET, height - LABEL_Y_OFFSET, /* BaseApp::getMainFont() ??? */ 0, str, 12.0f, 18.0f)); 

    // create the wind speed slider
    BaseApp::addWidget(m_windSpeedSlider = new Slider(SLIDER_X_OFFSET + SLIDER_WIDTH + SLIDER_X_SEPERATOR, 
        height - SLIDER_Y_OFFSET, SLIDER_WIDTH, 20.0f, true, 0.0f, 15.0f, m_windSpeed, 1.0f));
    sprintf(str, "Wind Speed = %d", (int)m_windSpeed);
    BaseApp::addWidget(m_windSpeedLabel = new Label(SLIDER_WIDTH + SLIDER_X_SEPERATOR, height - LABEL_Y_OFFSET, /* BaseApp::getMainFont() ??? */ 0, str, 12.0f, 18.0f)); 
    delete []str;

    BaseApp::initGUI();
}
#endif

//---------------------------------------------------------------------------
// App setup
//
// This is called from the sample's framework prior to creating any D3D
// objects.
//---------------------------------------------------------------------------
void setup()
{
    antiAliasSamples = 4;
    m_r2VB_Initialized = false;
    m_useGPUforWaves = true;
    m_useR2VB = true;
    m_wireFrame = false;
    m_GPU_Physics = 0;
    m_CPU_Physics = 0;

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
// resetCamera
//---------------------------------------------------------------------------
void resetCamera()
{
    camPos = D3DXFROMWINEVECTOR3(0, 5, 0);
    wx = 0;
    wy = D3DXFROMWINE_PI / 2;
    speed = 0;      // disable camera movement
}

//---------------------------------------------------------------------------
// App onReset()
//
// This is called by the sample framework upon a device reset.
//---------------------------------------------------------------------------
bool onReset()
{
    int hr;

    // Initialize the physics objects
    m_GPU_Physics = new GPU_Physics();
    m_CPU_Physics = new CPU_Physics();

   // initialize render to vb
   if (!m_r2VB_Initialized)
   {
        // ??? ( r2vbInit( WATER_GEOM_RESOLUTION * WATER_GEOM_RESOLUTION * sizeof(WATER_VERTEX)) );
        m_r2VB_Initialized = true;
   }

    // Fill in utility non-renderable textures and CPU-side buffers for FFTW computation
    m_CPU_Physics->FillWaterUtilityBuffers(m_windSpeed, m_windDirection);

    // Fill in the textures used by the GPU to simulate the wave physics
    m_GPU_Physics->FillWaterUtilityBuffers(m_windSpeed, m_windDirection);

    // Create a texture for use when render to VB is disabled
	Image3 tex;
	tex.create(FORMAT_RGBA32F, WATER_GEOM_RESOLUTION, WATER_GEOM_RESOLUTION, 1, 0);
	m_pSysMemLowSpatialWaterTex = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());

    return true;
}

//---------------------------------------------------------------------------
// App onPreReset()
//
// This is called by the sample framework before a device reset. We use this
// function to delete all of the non-managed objects before re-creating them
// during the reset.
//---------------------------------------------------------------------------
bool onPreReset()
{
    if (m_GPU_Physics)
    {
        delete(m_GPU_Physics);
        m_GPU_Physics = 0;
    }

    if (m_CPU_Physics)
    {
        delete(m_CPU_Physics);
        m_CPU_Physics = 0;
    }

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
bool load()
{

    onReset();
   if(BaseApp::getMenu()){
    // set some default values for the menu items
    BaseApp::getMenu()->enableItem(m_useR2VBMenuID, true);
    BaseApp::getMenu()->checkItem(m_useR2VBMenuID, m_useR2VB);
    BaseApp::getMenu()->enableItem(m_useGPUforWavesMenuID, true);
    BaseApp::getMenu()->checkItem(m_useGPUforWavesMenuID, m_useGPUforWaves);
    BaseApp::getMenu()->enableItem(m_wireFrameMenuID, true);
    BaseApp::getMenu()->checkItem(m_wireFrameMenuID, m_wireFrame);
	}

    // set the viewport
    m_viewport.Y = 0;
    m_viewport.X = 0;
    m_viewport.Width = width;
    m_viewport.Height = height;
    m_viewport.MinZ = 0.0f;
    m_viewport.MaxZ = 1.0f;
    IRenderer::GetRendererInstance()->setViewport(&m_viewport);

    // Load shaders
    m_waterRenderShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-OceanWater/WaterRender.hlsl", "main", "main");
    FormatDesc waterRenderAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 4, 0, TYPE_TEXCOORD, FORMAT_FLOAT, 2 };
    m_waterRenderVertexFormat = IRenderer::GetRendererInstance()->addVertexFormat(waterRenderAttribs, elementsOf(waterRenderAttribs), m_waterRenderShader);
    //m_skyboxShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-OceanWater/skybox.hlsl", "main", "main");

    // Load Textures
    m_skyboxTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/MarlinCube.dds", false, IRenderer::GetRendererInstance()->Gettrilinear());
    sb.init("/new/R2VB_VTF/R2VB/MarlinCube.dds");

    // skybox vertex format
    //FormatDesc skyboxAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
    //m_skyboxVertexFormat = IRenderer::GetRendererInstance()->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs), -1);

    // Create the ocean water vertex buffers
    m_waterVertexFormat  = IRenderer::GetRendererInstance()->addVertexFormat( WaterDecl, elementsOf(WaterDecl), m_waterRenderShader);
    std::vector<WATER_VERTEX> vv(WATER_GEOM_RESOLUTION * WATER_GEOM_RESOLUTION);
    std::vector<WATER_VERTEX> uv(WATER_GEOM_RESOLUTION * WATER_GEOM_RESOLUTION);

    // Create the ocean water index buffer
    std::vector<__WORD__> vi((WATER_GEOM_RESOLUTION-1) * (WATER_GEOM_RESOLUTION-1) * 6);

    // Fill the VBs and IBs with data
    FillWaterGeometry((WATER_VERTEX*)&vv[0], (WATER_TEXCOORD*)&uv[0], (__WORD__*)&vi[0]);

	m_waterPosVB = IRenderer::GetRendererInstance()->addVertexBuffer(vv.size(), DYNAMIC, &vv[0]);
	m_waterTexCoordVB = IRenderer::GetRendererInstance()->addVertexBuffer(uv.size(), STATIC, &uv[0]);
	m_waterIB = IRenderer::GetRendererInstance()->addIndexBuffer(vi.size(), 2, STATIC, &vi[0]);


    return true;
}

//---------------------------------------------------------------------------
// unload
//---------------------------------------------------------------------------
bool unload()
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
int FillWaterGeometry(WATER_VERTEX* pWVm, WATER_TEXCOORD* pWTexCd, __WORD__* pIndices)
{
   int hr = 0;
   int x, z, i, j;
   __DWORD__ curVertex = 0;

   //
   // Lock and fill the main water grid vertex buffer
   // Water grid is an x,z plane with y up
   //
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

        pWVm[curVertex].p  = D3DXFROMWINEVECTOR4(fX, fY, fZ, 1);
        pWTexCd[curVertex].uv = D3DXFROMWINEVECTOR2(fU, fV);

        curVertex++;
      }
   }

   //
   // Lock and fill the main water grid index buffer
   //
   for( j=0; j < (WATER_GEOM_RESOLUTION-1); j++ )
   {
      int baseIndex = j*WATER_GEOM_RESOLUTION;

      for( i=0; i < (WATER_GEOM_RESOLUTION-1); i++ )
      {
            *pIndices++ = (__WORD__)( baseIndex + i );
            *pIndices++ = (__WORD__)( baseIndex + i + WATER_GEOM_RESOLUTION );
            *pIndices++ = (__WORD__)( baseIndex + i + 1 );

            *pIndices++ = (__WORD__)( baseIndex + i + 1 );
            *pIndices++ = (__WORD__)( baseIndex + i + WATER_GEOM_RESOLUTION );
            *pIndices++ = (__WORD__)( baseIndex + i + WATER_GEOM_RESOLUTION + 1 );
         }
      }

   return hr;
}

//-----------------------------------------------------------------------------
//
// UpdateWaterGeometryCPU()
//
// Updates the water geometry based on the height field
//-----------------------------------------------------------------------------
int UpdateWaterGeometryCPU()
{
   int hr = 0;
   int x, z;
   __DWORD__ curVertex = 0;
   complex_number *pHeightFieldRow, *pHeightField;


   pHeightField = (complex_number *)m_CPU_Physics->GetDisplacementMap();

   //
   // Lock and update the main water grid vertex buffer
   // Water grid is an x,z plane with y up
   //
   WATER_VERTEX* pWV;
   pWV = (WATER_VERTEX *)IRenderer::GetRendererInstance()->lockVertexBuffer(m_waterPosVB);

   for (z = 0; z < WATER_GEOM_RESOLUTION; z++)
   {
      pHeightFieldRow = pHeightField + ((z % WATER_GRID_RESOLUTION) * WATER_GRID_RESOLUTION);
      for (x = 0; x < WATER_GEOM_RESOLUTION; x++)
      {
         // Position xy of grid in -1..+1 range at z=0
         float fX = (((float)x) / ((float)(WATER_GEOM_RESOLUTION-2))) * 2.0f - 1.0f;
         float fY = fabs(c_re(pHeightFieldRow[x % WATER_GRID_RESOLUTION])* (HEIGHT_SCALE));
         float fZ = (((float)z) / ((float)(WATER_GEOM_RESOLUTION-2))) * 2.0f - 1.0f;
         pWV[curVertex].p  = D3DXFROMWINEVECTOR4(fX, fY, fZ, 1);
         curVertex++;
      }
   }

   IRenderer::GetRendererInstance()->unlockVertexBuffer(m_waterPosVB);

   return hr;
}

//-----------------------------------------------------------------------------
//
// UpdateWaterGeometryGPU()
//
// Updates the water geometry based on the height field which was generated on
// the GPU and stored in a render target.
//-----------------------------------------------------------------------------
int UpdateWaterGeometryGPU(TextureID displacementMap)
{
    int hr = 0;
    int x, z;
    __DWORD__ curVertex = 0;
    //SurfaceID pHeightField;
    //SurfaceID pRenderTargetSurface;

    // Get the height field data

    struct Tex32RGBA {float x,y,z,w;};int pitch=0;
    Tex32RGBA* pRGBAData = (Tex32RGBA*)IRenderer::GetRendererInstance()->
				LockTexture(m_pSysMemLowSpatialWaterTex, 0, pitch);
    //
    // Lock and update the main water grid vertex buffer
    // Water grid is an x,z plane with y up
   //
    WATER_VERTEX* pWV;
    pWV = (WATER_VERTEX *)IRenderer::GetRendererInstance()->lockVertexBuffer(m_waterPosVB);

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
            pWV[curVertex].p  = D3DXFROMWINEVECTOR4(fX, fY, fZ, 1);
            curVertex++;
        }
    }

    IRenderer::GetRendererInstance()->UnlockTexture(m_pSysMemLowSpatialWaterTex, 0);
    IRenderer::GetRendererInstance()->unlockVertexBuffer(m_waterPosVB);

    return hr;
}

//---------------------------------------------------------------------------
// drawSkybox
//---------------------------------------------------------------------------
void drawSkybox(const D3DXFROMWINEMATRIX &mvp){
	#if 0
    static const float verts[] = { -8,-8,-8, -8,-8,8, -8,8,-8, -8,8,8, 8,-8,-8, 8,-8,8, 8,8,-8, 8,8,8 };
    static const unsigned short inds[] = { 2,3,0, 0,3,1, 3,7,1, 1,7,5, 7,6,5, 5,6,4, 6,2,4, 4,2,0, 6,7,2, 2,7,3, 0,1,4, 4,1,5 };

    IRenderer::GetRendererInstance()->setShader(m_skyboxShader);
    IRenderer::GetRendererInstance()->setVertexFormat(m_skyboxVertexFormat);
    IRenderer::GetRendererInstance()->setTexture("Skybox", m_skyboxTexture);
    IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
    

    IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);

    IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, 8, 12, inds, inds, CONSTANT_INDEX2, verts, verts, 3 * sizeof(float));
	#else
        sb.render();
	#endif
}

//---------------------------------------------------------------------------
// App drawFrame
//
//---------------------------------------------------------------------------
void drawFrame()
{

    int hr;
    int tile;
    float fTime;
    TextureID normalMap;
    TextureID displacementMap;
    float lastWindSpeed = m_windSpeed;
    float lastWindDirection = m_windDirection;

    // initialize booleans from the menu
	if(BaseApp::getMenu()){
    m_wireFrame = BaseApp::getMenu()->isItemChecked(m_wireFrameMenuID);
    m_useGPUforWaves = BaseApp::getMenu()->isItemChecked(m_useGPUforWavesMenuID);}
    if (!m_useGPUforWaves)
    {
	if(BaseApp::getMenu())
        	BaseApp::getMenu()->enableItem(m_useR2VBMenuID, false);
        m_useR2VB = false;
    }
    else if(BaseApp::getMenu())
    {
        BaseApp::getMenu()->enableItem(m_useR2VBMenuID, true);
        m_useR2VB = BaseApp::getMenu()->isItemChecked(m_useR2VBMenuID);
    }

    // initialize the wind velocity from the sliders
    if(m_windDirSlider)
        m_windDirection = m_windDirSlider->getValue();
    if(m_windSpeed)
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
    D3DXFROMWINEMATRIX projection = perspectiveMatrixX(D3DXFROMWINE_PI / 2, width, height, 1, 2000, true);
    D3DXFROMWINEMATRIX modelview = rotateXY4(-wx , -wy);
    D3DXFROMWINEMATRIX transMtx = translate(0, -0.125f, 0);
    D3DXFROMWINEMATRIX mvpSB = projection * modelview * transMtx;
    modelview.translate(-camPos);
    D3DXFROMWINEMATRIX mvp = projection * modelview;

    IRenderer::GetRendererInstance()->setViewport(&m_viewport);

    // clear the render target
    IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

    // Draw the skybox
    drawSkybox(mvpSB);

    // Compute the waves
    fTime = timeGetTime();
    if (m_useGPUforWaves)
    {
        // GPU Wave Generation
        m_GPU_Physics->ComputeWaves(m_windSpeed, m_windDirection, fTime);
        displacementMap = (TextureID)m_GPU_Physics->GetDisplacementMap();
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
        displacementMap = 0;
    }

    //
    // Draw water surface tiles
    //
    //normalMap->AddDirtyRect(0);
    // ??? normalMap->GenerateMipSubLevels();

    //IRenderer::GetRendererInstance()->defaults();
    IRenderer::GetRendererInstance()->setShader(m_waterRenderShader);
    IRenderer::GetRendererInstance()->setTexture("SkySampler", m_skyboxTexture);

    // Choose appropriate fill mode
    if (menu && BaseApp::getMenu()->isItemChecked(m_wireFrameMenuID))
    {
        // ( IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) );
        IRenderer::GetRendererInstance()->setShaderConstant1f("g_useConstantColor",  1.0f);
    }
    else
    {
        // ( IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID) );
        IRenderer::GetRendererInstance()->setShaderConstant1f("g_useConstantColor",  -1.0f);
    }

    // set the vertex format
    IRenderer::GetRendererInstance()->setVertexFormat(m_waterVertexFormat);

    if (!m_useGPUforWaves || !m_useR2VB)
    {
        // use the position vertex buffer
        IRenderer::GetRendererInstance()->setVertexBuffer(0, m_waterPosVB);
    }

    // set the texture coordinate in stream 1
    IRenderer::GetRendererInstance()->setVertexBuffer(1, m_waterTexCoordVB);

    // set the index buffer
    IRenderer::GetRendererInstance()->setIndexBuffer(m_waterIB);

    // world space eye position
    D3DXFROMWINEVECTOR4 eyePos = D3DXFROMWINEVECTOR4(camPos, 1.0f);
    IRenderer::GetRendererInstance()->setShaderConstant4f("g_vWSEyePos",  eyePos);

    // render state
    //IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    //IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    //IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    //IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
    //IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    // set the normal map
    //int index = IRenderer::GetRendererInstance()->getSamplerIndex(m_waterRenderShader, "HighFreqWaterNormalSampler");
    ////IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    ////IRenderer::GetRendererInstance()->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    IRenderer::GetRendererInstance()->setTexture("HighFreqWaterNormalSampler", normalMap); // High frequency normal map

    if (m_useGPUforWaves && m_useR2VB)
    {
        // set the displacement map as stream source
        IRenderer::GetRendererInstance()->setTexture("DMAP", displacementMap);
    }

    D3DXFROMWINEMATRIX scaleMtx = scale4(100, 100, 100);
    unsigned int numVerts = WATER_GEOM_RESOLUTION*WATER_GEOM_RESOLUTION;
    unsigned int numTris  = (WATER_GEOM_RESOLUTION-1) * (WATER_GEOM_RESOLUTION-1) * 2;

    for (tile=0; tile < NUM_TILES; tile++)
    {
        // composite matrix
        D3DXFROMWINEMATRIX scaledTile = scaleMtx * m_matTile[tile];
        D3DXFROMWINEMATRIX wvp = mvp * scaledTile;

        IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorldViewProjection", wvp);
        IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorld", scaledTile);

        ( IRenderer::GetRendererInstance()->DrawIndexedPrimitive( PRIM_TRIANGLES, 0, 0, numVerts, 0, numTris) );
    }

}

    int setTexture(ShaderID shader, char* name, TextureID  texture)
    {
        int index = IRenderer::GetRendererInstance()->getSamplerIndex(shader, name);
        IRenderer::GetRendererInstance()->setTexture(name, texture);
	return index;
    }

    Physics *m_GPU_Physics;
    Physics *m_CPU_Physics;
    bool m_r2VB_Initialized;            // true if R2VB library is initialized
    bool m_useR2VB;                     // if true, use R2VB for rendering
    bool m_useGPUforWaves;              // if true, use the GPU for physics
    bool m_wireFrame;                   // enables wireframe rendering
    float m_windSpeed;                  // wind speed for waves
    float m_windDirection;              // direction for waves
    D3DXFROMWINEMATRIX m_matTile[9];                  // Matrices for tiling the water surface
    D3DFROMWINEVIEWPORT9 m_viewport;            // application viewport

    // SDK Framework IDs
    ShaderID m_waterRenderShader;       // shader for rendering the waves
    VertexFormatID m_waterRenderVertexFormat;
    VertexFormatID m_waterVertexFormat;
    VertexBufferID m_waterPosVB;
    VertexBufferID m_waterTexCoordVB;
    IndexBufferID m_waterIB;
    ShaderID m_skyboxShader;
    //VertexFormatID m_skyboxVertexFormat;
    TextureID m_skyboxTexture;
	stx_SkyBox sb;

    // System memory texture for the non-render-to-VB case
    TextureID       m_pSysMemLowSpatialWaterTex;
	#if 0
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
	#endif
};
App app;
int init(const char* aTitle)
{
	app.setup();
	app.resetCamera();
	app.load();
#ifdef R2VB_GUI
	app.initGUI();
#endif	
	return 0;
}

void render()
{
	app.drawFrame();
	IRenderer::GetRendererInstance()->Present();
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("R2VB-OceanWater");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	STX_PRINT("Function init finished!\n");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

