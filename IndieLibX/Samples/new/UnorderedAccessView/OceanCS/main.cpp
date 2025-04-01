// Copyright (c) 2011 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, NONINFRINGEMENT,IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA 
// OR ITS SUPPLIERS BE  LIABLE  FOR  ANY  DIRECT, SPECIAL,  INCIDENTAL,  INDIRECT,  OR  
// CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS 
// OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY 
// OTHER PECUNIARY LOSS) ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, 
// EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// Please direct any bugs or questions to SDKFeedback@nvidia.com
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
#include "ocean_simulator.h"

void renderWireframe(const stx_CFirstPersonCamera& camera, ShaderResourceViewID displacemnet_map, float time);
void renderShaded(const stx_CFirstPersonCamera& camera, ShaderResourceViewID displacemnet_map, ShaderResourceViewID gradient_map, float time);

struct App
{
	App(){}
	virtual ~App(){}

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
stx_CFirstPersonCamera          g_Camera;                   // A model viewing camera

// Ocean simulation variables
OceanSimulator* g_pOceanSimulator = 0;

bool g_RenderWireframe = false;
bool g_PauseSimulation = false;
int g_BufferType = 0;

// Skybox
TextureID g_pSkyCubeMap = 0;
ShaderResourceViewID g_pSRV_SkyCube = 0;

stx_SkyBox g_Skybox;

//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void  OnFrameMove( double fTime, float fElapsedTime )
{
    // Update the camera's position based on user input 
    g_Camera.FrameMove( fElapsedTime );

	// Update simulation
	static double app_time = 0;
	if (g_PauseSimulation == false)
	{
		app_time += (double)fElapsedTime;
		g_pOceanSimulator->updateDisplacementMap((float)app_time);
	}
}

//--------------------------------------------------------------------------------------
// Create an OceanSimulator object at startup time
//--------------------------------------------------------------------------------------
void CreateOceanSimAndRender()
{
	// Create ocean simulating object
	// Ocean object
	OceanParameter ocean_param;

	// The size of displacement map. In this sample, it's fixed to 512.
	ocean_param.dmap_dim			= 512;
	// The side length (world space) of square patch
	ocean_param.patch_length		= 2000.0f;
	// Adjust this parameter to control the simulation speed
	ocean_param.time_scale			= 0.8f;
	// A scale to control the amplitude. Not the world space height
	ocean_param.wave_amplitude		= 0.35f;
	// 2D wind direction. No need to be normalized
	ocean_param.wind_dir			= D3DXFROMWINEVECTOR2(0.8f, 0.6f);
	// The bigger the wind speed, the larger scale of wave crest.
	// But the wave scale can be no larger than patch_length
	ocean_param.wind_speed			= 600.0f;
	// Damp out the components opposite to wind direction.
	// The smaller the value, the higher wind dependency
	ocean_param.wind_dependency		= 0.07f;
	// Control the scale of horizontal movement. Higher value creates
	// pointy crests.
	ocean_param.choppy_scale		= 1.3f;

	g_pOceanSimulator = new OceanSimulator(ocean_param);

	// Update the simulation for the first time.
	g_pOceanSimulator->updateDisplacementMap(0);


	// Init D3D11 resources for rendering
	//initRenderResource(ocean_param);
}

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// created, which will happen during application initialization and windowed/full screen 
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these 
// resources need to be reloaded whenever the device is destroyed. Resources created  
// here should be released in the OnDestroyDevice callback. 
//--------------------------------------------------------------------------------------
int  OnCreateDevice()
{
    int hr;

	// Setup the camera's view parameters
	D3DXFROMWINEVECTOR3 vecEye(1562.24f, 854.291f, -1224.99f);
	D3DXFROMWINEVECTOR3 vecAt (1562.91f, 854.113f, -1225.71f);
	g_Camera.SetViewParams(&vecEye, &vecAt);

	// Create an OceanSimulator object and D3D11 rendering resources
	CreateOceanSimAndRender();

	// Sky box
	g_pSRV_SkyCube="Media/OceanCS/sky_cube.dds";

    return 0;
}

int  OnResizedSwapChain()
{
    int hr;

    // Setup the camera's projection parameters
	float fAspectRatio = STX_Service::GetWindowInstance()->GetWidth() / (FLOAT)STX_Service::GetWindowInstance()->GetHeight();
	g_Camera.SetProjParams(D3DXFROMWINE_PI/4, fAspectRatio, 100.0f, 200000.0f);

	// Sky box
	//g_Skybox.OnResizedSwapChain();

    return 0;
}

void  OnFrameRender( double fTime,
                                 float fElapsedTime )
{
    IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.1f,0.2f,0.4f,0.0f));

	// Sky box rendering
    D3DXFROMWINEMATRIX mView = D3DXFROMWINEMATRIX(1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1) * *g_Camera.GetViewMatrix();

	//D3DXFROMWINEMATRIX GetProjMatrix();
    D3DXFROMWINEMATRIX mProj = g_Camera.GetProjMatrix();
    D3DXFROMWINEMATRIX mWorldViewProjection = mView * mProj;

	if (!g_RenderWireframe)
	    g_Skybox.render( &mWorldViewProjection);

	// Time
	static double app_time = fTime;
	static double app_prev_time = fTime;
	if (g_PauseSimulation == false)
		app_time += fTime - app_prev_time;
	app_prev_time = fTime;

	// Ocean rendering
	ShaderResourceViewID tex_displacement = g_pOceanSimulator->getD3D11DisplacementMap();
	ShaderResourceViewID tex_gradient = g_pOceanSimulator->getD3D11GradientMap();
	if (g_RenderWireframe)
		renderWireframe(g_Camera, tex_displacement, (float)app_time);
	else
		renderShaded(g_Camera, tex_displacement, tex_gradient, (float)app_time);
}
};
App app;
int init(const char* aTitle)
{
    	app.OnCreateDevice();	
	return 0;
}

void render()
{
   double fTime=0;
   float fElapsedTime=0.0f;
   app.OnFrameRender(fTime, fElapsedTime); 
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("OceanCS");
	//if(std::string(r->getString())!="D3D11") stx_exit(11);
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		IRenderer::GetRendererInstance()->BeginScene();
		render();
		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present( );
	}
	return 0;
}

