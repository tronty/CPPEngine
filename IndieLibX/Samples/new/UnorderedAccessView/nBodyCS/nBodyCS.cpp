/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
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

//--------------------------------------------------------------------------------------
// This program performs a parallel simulation of an "N-body" system of stars.  That is,
// it simulates the evolution over time of the positions and velocities of a number of
// massive bodies, such as stars, that all influence each other via gravitational
// attraction.  Because all bodies interact with all others, there are O(N^2) interactions
// to compute at each time step (each frame).  This is a lot of computation, but it 
// is highly parallel and is thus amenable to GPU implementation using DirectCompute
// shaders.

#include "nBodyCS.h"

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

D3DXFROMWINEMATRIX g_CameraEyePt;
D3DXFROMWINEMATRIX g_CameraProj;
D3DXFROMWINEMATRIX g_CameraView;
D3DXFROMWINEMATRIX g_CameraWorld;
                // A model viewing camera

bool                          g_bShowHelp          = false;  
bool                          g_bPaused            = false;

float                         g_pointSize          = DEFAULT_POINT_SIZE;
float                         g_fFrameTime         = 0.0f;
int                           g_iNumBodies         = 1;

float                         g_clusterScale       = 1.54f;
float                         g_velocityScale      = 8.0f;

BodyData                      g_BodyData;

NBodySystemCS*                g_nBodySystem        = 0;  

unsigned int g_iSimSizes[] = {1024, 4096, 8192, 14336, 16384, 28672, 30720, 32768, 57344, 61440, 65536 };

struct App
{
    App(){}
    virtual ~App(){}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{ 
	g_nBodySystem = new NBodySystemCS();
}

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// created, which will happen during application initialization and windowed/full screen 
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these 
// resources need to be reloaded whenever the device is destroyed. Resources created  
// here should be released in the OnDestroyDevice callback. 
//--------------------------------------------------------------------------------------
int OnCreateDevice( )
{
    int hr;

	D3DXFROMWINEVECTOR3 vecEye(0.0f, 0.0f, -50.0f);
    D3DXFROMWINEVECTOR3 vecAt (0.0f, 0.0f,  0.0f);
    g_Camera.SetViewParams( &vecEye, &vecAt );

	// Init the Body system
	g_nBodySystem->onCreateDevice( IRenderer::GetRendererInstance());
	initializeBodies( g_iNumBodies );
	g_nBodySystem->resetBodies( g_BodyData ); 
	g_nBodySystem->setPointSize   ( g_pointSize );

	return 0;
}

//--------------------------------------------------------------------------------------
// Main render function
//--------------------------------------------------------------------------------------
void OnFrameRender( )
{
    IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));

	// Update bodies
	if (!g_bPaused)
    {
        g_nBodySystem->updateBodies(fElapsedTime);
    }

	// Render bodies
	g_nBodySystem->renderBodies( g_Camera.GetWorldMatrix() , g_Camera.GetViewMatrix() , g_Camera.GetProjMatrix() );
}


//--------------------------------------------------------------------------------------
// Handle resize
//--------------------------------------------------------------------------------------
int OnResizedSwapChain( )
{
    int hr;

	// Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams ( D3DXFROMWINE_PI/4, fAspectRatio, 0.1f, 1000.0f );

	g_nBodySystem->onResizedSwapChain( pBackBufferSurfaceDesc);

	return 0;
}

//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void OnFrameMove( double fTime, float fElapsedTime)
{
    static int tickCounter = 0;
	
	g_Camera.FrameMove(fElapsedTime);
    
	// Update the global time
	g_fFrameTime = fElapsedTime; 
}

//--------------------------------------------------------------------------------------
// Helper function to compute performance statistics for n-body system
//--------------------------------------------------------------------------------------
void computePerfStats(double &interactionsPerSecond, double &gflops, 
                      float seconds, int iterations)
{
    const int flopsPerInteraction = 20;
    interactionsPerSecond = (float)g_iNumBodies * (float)g_iNumBodies;
    interactionsPerSecond *= 1e-9 * iterations / seconds;
    gflops = interactionsPerSecond * (float)flopsPerInteraction;
}

//--------------------------------------------------------------------------------------
// Initialize bodies
//--------------------------------------------------------------------------------------
void initializeBodies(unsigned int numBodies)
{
    if( g_BodyData.nBodies == numBodies )
    {
        return;
    }

    // Free previous data
    delete [] g_BodyData.position;
    delete [] g_BodyData.velocity;

    // Allocate new data
    g_BodyData.position = new float[numBodies * 3];
    g_BodyData.velocity = new float[numBodies * 3];

    g_BodyData.nBodies = numBodies;

    float scale = g_clusterScale;
    float vscale = scale * g_velocityScale;
    float inner = 2.5f * scale;
    float outer = 4.0f * scale;

    int p = 0, v=0;
    unsigned int i = 0;
    while (i < numBodies)
    {
        float x, y, z;
        x = stx_rand() / (float) RAND_MAX * 2 - 1;
        y = stx_rand() / (float) RAND_MAX * 2 - 1;
        z = stx_rand() / (float) RAND_MAX * 2 - 1;

        D3DXFROMWINEVECTOR3 point(x, y, z);
        float len = D3DXFROMWINEVec3Length(&point);
        D3DXFROMWINEVec3Normalize(&point, &point);
        if (len > 1)
            continue;

        g_BodyData.position[p++] = point.x * (inner + (outer - inner) * stx_rand() / (float) RAND_MAX);
        g_BodyData.position[p++] = point.y * (inner + (outer - inner) * stx_rand() / (float) RAND_MAX);
        g_BodyData.position[p++] = point.z * (inner + (outer - inner) * stx_rand() / (float) RAND_MAX);
        //g_BodyData.position[p++] = 1.0f;

        x = 0.0f; // * (stx_rand() / (float) RAND_MAX * 2 - 1);
        y = 0.0f; // * (stx_rand() / (float) RAND_MAX * 2 - 1);
        z = 1.0f; // * (stx_rand() / (float) RAND_MAX * 2 - 1);
        D3DXFROMWINEVECTOR3 axis(x, y, z);
        D3DXFROMWINEVec3Normalize(&axis, &axis);

        if (1 - D3DXFROMWINEVec3Dot(&point, &axis) < 1e-6)
        {
            axis.x = point.y;
            axis.y = point.x;
            D3DXFROMWINEVec3Normalize(&axis, &axis);
        }
        //if (point.y < 0) axis = scalevec(axis, -1);
        D3DXFROMWINEVECTOR3 vv(g_BodyData.position[3*i], g_BodyData.position[3*i+1], g_BodyData.position[3*i+2]);
        D3DXFROMWINEVec3Cross(&vv, &vv, &axis);
        g_BodyData.velocity[v++] = vv.x * vscale;
        g_BodyData.velocity[v++] = vv.y * vscale;
        g_BodyData.velocity[v++] = vv.z * vscale;
        //g_BodyData.velocity[v++] = 1.0f;

        i++;
    }
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
	IRenderer* r=IRenderer::GetRendererInstance("nBodyCS");
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

