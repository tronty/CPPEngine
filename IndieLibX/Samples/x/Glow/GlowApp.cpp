/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#include "GlowApp.h"
#include "GlowDemo.h"

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

    // Set up our view matrix. A view matrix can be defined given an eye point and
    // a point to lookat. Here, we set the eye five units back along the z-axis and 
	// up three units and look at the origin.
    D3DXFROMWINEVECTOR3 vFromPt   = D3DXFROMWINEVECTOR3(0.0f, 0.0f, -5.0f);
    D3DXFROMWINEVECTOR3 vLookatPt = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
	g_Camera.SetViewParams( &vFromPt, &vLookatPt);
	return 0;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// reset, which will happen after a lost device scenario. This is the best location to 
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever 
// the device is lost. Resources created here should be released in the OnLostDevice 
// callback. 
//--------------------------------------------------------------------------------------
int OnResetDevice( )
{
	int hr;

    SAFE_DELETE( g_pDemo );
	g_pDemo = new GlowDemo;
	FAIL_IF_NULL( g_pDemo );
	g_pDemo->Initialize( IRenderer::GetRendererInstance() );

    IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, TRUE);
    return 0;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void OnFrameMove( IDirect3DDevice9* IRenderer::GetRendererInstance(), double fTime, float fElapsedTime )
{
    // TODO: update world
    g_Camera.FrameMove( fElapsedTime );
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, the sample framework will call 
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void OnFrameRender( double fTime, float fElapsedTime )
{

    int hr;

    // Set the world matrix
    IRenderer::GetRendererInstance()->SetTransform(D3DTS_WORLD, g_Camera.GetWorldMatrix());
    // Set the projection matrix
    IRenderer::GetRendererInstance()->SetTransform(D3DTS_PROJECTION, g_Camera.GetProjMatrix());
	// Set the view matrix
	IRenderer::GetRendererInstance()->SetTransform(D3DTS_VIEW, g_Camera.GetViewMatrix());

	// Begin the scene
    if (SUCCEEDED(IRenderer::GetRendererInstance()->BeginScene()))
    {
		if( g_pDemo != 0 )
		{
			g_pDemo->Render();
		}

        // End the scene.
        ( IRenderer::GetRendererInstance()->EndScene() );
    }
}

