/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

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

//include "Island11.h"
#include "terrain.h"

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

float g_BackBufferWidth         = 1280.0f;
float g_BackBufferHeight        = 720.0f;
float g_LightPosition[3]        = {-10000.0f,6500.0f,10000.0f};

// Statistics
int g_CurrentFrame              = 0;
float g_FrameTime				= 0;
float g_TotalTime				= 0;

// Control variables
bool g_RenderWireframe          = false;
bool g_QueryStats               = false;
bool g_RenderHUD                = true;
bool g_UseDynamicLOD			= true;
bool g_RenderCaustics			= true;
bool g_FrustumCullInHS			= true;
bool g_CycleViewPoints			= false;


float g_DynamicTessellationFactor = 50.0f;
float g_StaticTessellationFactor = 12.0f;


D3DXFROMWINEVECTOR3 g_EyePoints[6]=  {D3DXFROMWINEVECTOR3(365.0f,  3.0f, 166.0f),
						D3DXFROMWINEVECTOR3(478.0f, 15.0f, 248.0f),
						D3DXFROMWINEVECTOR3(430.0f,  3.0f, 249.0f),
						D3DXFROMWINEVECTOR3(513.0f, 10.0f, 277.0f),
						D3DXFROMWINEVECTOR3(303.0f,  3.0f, 459.0f),
						D3DXFROMWINEVECTOR3(20.0f,  12.0f, 477.0f),

						};

D3DXFROMWINEVECTOR3 g_LookAtPoints[6]={D3DXFROMWINEVECTOR3(330.0f,-11.0f, 259.0f),
						  D3DXFROMWINEVECTOR3(388.0f,-16.0f, 278.0f),
						  D3DXFROMWINEVECTOR3(357.0f,-59.0f, 278.0f),
						  D3DXFROMWINEVECTOR3(438.0f,-12.0f, 289.0f),
						  D3DXFROMWINEVECTOR3(209.0f,-20.0f, 432.0f),
						  D3DXFROMWINEVECTOR3(90.0f,  -7.0f, 408.0f),
						  };


int g_NumViews=6;

D3DXFROMWINEVECTOR3 g_EyePoint = g_EyePoints[0];
D3DXFROMWINEVECTOR3 g_LookAtPoint = g_LookAtPoints[0];
D3DXFROMWINEVECTOR3 g_EyePointspeed = D3DXFROMWINEVECTOR3(0,0,0);
D3DXFROMWINEVECTOR3 g_LookAtPointspeed = D3DXFROMWINEVECTOR3(0,0,0);

D3DXFROMWINEMATRIX                  g_ProjMatrix;
D3DXFROMWINEMATRIX                  g_ViewMatrix;
CTerrain			g_Terrain;
stx_Effect*      g_pEffect       = 0;

struct App
{
	App(){}
	virtual ~App(){}
//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
int  OnCreateDevice()
{
    int hr;

    static bool isFirstTime = true;
    
    g_pEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/new/Island11/Island11.fx");

	// Initialize terrain 
	g_Terrain.Initialize(g_pEffect);
	g_Terrain.LoadTextures();
	g_Terrain.BackbufferWidth=g_BackBufferWidth;
	g_Terrain.BackbufferHeight=g_BackBufferHeight;
	g_Terrain.ReCreateBuffers();

    // Set initial camera view
    if(isFirstTime)
    {
	D3DXFROMWINEVECTOR3 vecUp (0.0f, 1.0f, 0.0f);
	D3DXFROMWINEMatrixLookAtLH(&g_ViewMatrix, &g_EyePoints[0], &g_LookAtPoints[0], &vecUp);
    }
    isFirstTime = false;

    return 0;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
int  OnResizedSwapChain()
{
    int hr;

    float fAspectRatio = IRenderer::GetRendererInstance()->GetAspectRatio();
    D3DXFROMWINEMatrixPerspectiveFovLH(&g_ProjMatrix, camera_fov * D3DXFROMWINE_PI / 360.0f, fAspectRatio, scene_z_near, scene_z_far);

    g_BackBufferWidth = (float)STX_Service::GetWindowInstance()->GetWidth();
    g_BackBufferHeight = (float)STX_Service::GetWindowInstance()->GetHeight();
	g_Terrain.BackbufferWidth = g_BackBufferWidth;
	g_Terrain.BackbufferHeight = g_BackBufferHeight;
	g_Terrain.ReCreateBuffers();

    return 0;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void  OnFrameMove( double fTime, float fElapsedTime )
{
    if(!g_CycleViewPoints)
	{
		//g_Camera.FrameMove(fElapsedTime);
	}
}




//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void  OnFrameRender( double fTime, float fElapsedTime )
{
	g_TotalTime+=fElapsedTime;
	g_FrameTime=fElapsedTime;

	float viewpoints_slide_speed_factor=0.01f;
	float viewpoints_slide_speed_damp=0.97f;
	float scaled_time=(1.0f+g_TotalTime/25.0f);
	int viewpoint_index = (int)(scaled_time)%6;
	D3DXFROMWINEVECTOR3 predicted_camera_position;
	float dh;
	
	if(g_CycleViewPoints)
	{
		g_EyePointspeed+=(g_EyePoints[viewpoint_index]-g_EyePoint)*viewpoints_slide_speed_factor;
		g_LookAtPointspeed+=(g_LookAtPoints[viewpoint_index]-g_LookAtPoint)*viewpoints_slide_speed_factor;

		predicted_camera_position=(g_EyePoint+g_EyePointspeed*fElapsedTime*15.0f)/terrain_geometry_scale;

		dh=predicted_camera_position.y-g_Terrain.height[((int)predicted_camera_position.x)%terrain_gridpoints]
													   [((int)(terrain_gridpoints-predicted_camera_position.z))%terrain_gridpoints]-3.0f;
		if(dh<0)
		{
			g_EyePointspeed.y-=dh;
		}
		
		dh=predicted_camera_position.y-3.0f;
		if(dh<0)
		{
			g_EyePointspeed.y-=dh;
		}

		g_EyePointspeed*=viewpoints_slide_speed_damp;
		g_LookAtPointspeed*=viewpoints_slide_speed_damp;

		g_EyePoint+=g_EyePointspeed*fElapsedTime;
		g_LookAtPoint+=g_LookAtPointspeed*fElapsedTime;


		if(g_EyePoint.x<0)g_EyePoint.x=0;
		if(g_EyePoint.y<0)g_EyePoint.y=0;
		if(g_EyePoint.z<0)g_EyePoint.z=0;

		if(g_EyePoint.x>terrain_gridpoints*terrain_geometry_scale)g_EyePoint.x=terrain_gridpoints*terrain_geometry_scale;
		if(g_EyePoint.y>terrain_gridpoints*terrain_geometry_scale)g_EyePoint.y=terrain_gridpoints*terrain_geometry_scale;
		if(g_EyePoint.z>terrain_gridpoints*terrain_geometry_scale)g_EyePoint.z=terrain_gridpoints*terrain_geometry_scale;

		D3DXFROMWINEVECTOR3 vecUp (0.0f, 1.0f, 0.0f);
		D3DXFROMWINEMatrixLookAtLH(&g_ViewMatrix, &g_EyePoint, &g_LookAtPoint, &vecUp);
	}
	else
	{
		g_EyePointspeed=D3DXFROMWINEVECTOR3(0,0,0);
		g_LookAtPointspeed=D3DXFROMWINEVECTOR3(0,0,0);
	}

    D3DFROMWINEVIEWPORT9 defaultViewport=IRenderer::GetRendererInstance()->GetViewport();

	D3DXFROMWINEVECTOR2 WaterTexcoordShift(g_TotalTime*1.5f,g_TotalTime*0.75f);
	D3DXFROMWINEVECTOR2 ScreenSizeInv(1.0f/(g_BackBufferWidth*main_buffer_size_multiplier),1.0f/(g_BackBufferHeight*main_buffer_size_multiplier));

	g_pEffect->SetFloat("g_ZNear", scene_z_near);
	g_pEffect->SetFloat("g_ZFar", scene_z_far);
	g_pEffect->SetFloat("g_DynamicTessFactor", g_DynamicTessellationFactor);
	g_pEffect->SetFloat("g_StaticTessFactor", g_StaticTessellationFactor);
	g_pEffect->SetFloat("g_UseDynamicLOD", g_UseDynamicLOD?1.0f:0.0f);
	g_pEffect->SetFloat("g_RenderCaustics", g_RenderCaustics?1.0f:0.0f);
	g_pEffect->SetFloat("g_FrustumCullInHS", g_FrustumCullInHS?1.0f:0.0f);
        g_pEffect->SetFloat("g_LightPosition", &g_LightPosition[0], 3);
	g_pEffect->SetFloat("g_WaterBumpTexcoordShift", &WaterTexcoordShift.x, 2);
	g_pEffect->SetFloat("g_ScreenSizeInv", &ScreenSizeInv.x, 2);

#if 0
	g_Terrain.Render(g_ProjMatrix, g_ViewMatrix);
#else
    stx_CFirstPersonCamera cam;
	cam.SetProjMatrix(g_ProjMatrix);
	cam.SetViewMatrix(g_ViewMatrix);
	g_Terrain.Render(&cam);
#endif

    g_CurrentFrame++;
}
};
App app;
int init(const char* aTitle)
{
    	app.OnCreateDevice();	
	app.OnResizedSwapChain();
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
	IRenderer* r=IRenderer::GetRendererInstance("Island11");
	//if(std::string(r->getString())!="D3D11") stx_exit(11);
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	STX_PRINT("Function init finished!\n");
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

