/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
//----------------------------------------------------------------------------------
// File:   Main.cpp
// Author: Sarah Tariq and Ignacio Llamas
// Email:  sdkfeedback@nvidia.com
// 
// Copyright (c) 2007 NVIDIA_Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
// BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
//----------------------------------------------------------------------------------
#include <Framework3/IRenderer.h>

#include "resource.h"

#include "nvutPCH.h"
#include "Smoke.h"

#include "NVUTSkybox.h"

#include "Fluid.h"
#include "Voxelizer.h"
#include "SkinnedMesh.h"

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

#define MAX_SO_BUFFERS 3
int                         g_dstSOBuffer = 0;
int                         g_srcSOBuffer = -1;
unsigned int                g_numWrittenSOBuffers = 0;
unsigned int                g_numWrittenSOBuffersSincePause = 0;

float                       g_Fovy = D3DXFROMWINE_PI * 0.25f;

// Window Dimensions
int                         g_Width  = 1024;
int                         g_Height = 768; 

// Misc options
bool                        g_bPause = false;
bool                        g_bRenderFluid = true;
bool                        g_bRenderMesh = true;
int                         g_currentRenderChoice = 0;
int                         g_gridWidth = 70;
int                         g_gridHeight = 70;
int                         g_gridDepth = 100;
int                         g_numJacobi = 6; 
float                       g_fTimestep = 2.0f;
bool                        g_bReinitializeFluid = false;
bool                        g_bPauseAnimation = false;
bool                        g_bRenderGlow = true;
Fluid::FLUID_TYPE           g_eFluidType = Fluid::FT_SMOKE;
bool                        g_bVoxelizeSkinnedMesh = true;
bool                        g_bStreamoutSkinnedMesh = true;

float                       g_fModelRotation = 0.0;
float                       g_fRotSpeed = 4.0;

//variables for the fire
float g_glowContribution     = 0.81f;
float g_finalIntensityScale  = 28.0f; 
float g_finalAlphaScale      = 0.95f;
float g_smokeColorMultiplier = 2.0f;   
float g_smokeAlphaMultiplier = 0.05f; 
int   g_RednessFactor        = 5; 


char* g_tstr_renderModeLabels[] = 
{ 
    ("Scene"), 
    ("Phi as density"),
    ("Phi as level set"),
    ("Gradient of phi"),
    ("Velocity Field"),
    ("Pressure Field"),
    ("Voxelized Obstacles"),
    0
};

char* g_tstr_fluidTypeLabels[] = 
{
    ("Smoke"),
    ("Fire"),
    ("Water"),
    0
};


// Fluid Simulation state
Fluid*                      g_fluid = 0;
Voxelizer*                  g_voxelizer = 0;
VolumeRenderer*             g_renderer = 0;

// Camera state
D3DXFROMWINEMATRIX g_CameraEyePt;
D3DXFROMWINEMATRIX g_CameraProj;
D3DXFROMWINEMATRIX g_CameraView;
D3DXFROMWINEMATRIX g_CameraWorld;

float                       g_zNear = 1.0f;
float                       g_zFar = 1000.0f;
D3DXFROMWINEVECTOR3                 g_Eye = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, -20.0f );
D3DXFROMWINEVECTOR3                 g_At = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );
D3DXFROMWINEVECTOR3                 g_Up = D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f );
D3DXFROMWINEMATRIX                  g_View;
D3DXFROMWINEMATRIX                  g_Projection;

// Scene state
//   World transforms
D3DXFROMWINEMATRIX                  g_gridWorld;
D3DXFROMWINEMATRIX                  g_meshWorld;
D3DXFROMWINEVECTOR3                 g_obstaclePos = D3DXFROMWINEVECTOR3(0.5, 0.5, 0.5);
SkinnedMesh*                g_skinnedMesh = 0;
float                       g_meshCenter[3];
float                       g_meshRadius;
SkinnedMesh::stdStringVector    g_voxelization_includeNodes;
SkinnedMesh::stdStringVector    g_rendering_excludeNodes;

NVUTSkybox                  g_Skybox;
// Environment map
TextureID *           g_pEnvMap       = 0;                    
TextureID *  g_pEnvMapSRV    = 0;

// Additional depth buffer (to render the scene into) 
//   so that we can sample from it in VolumeRenderer
TextureID*            g_pSceneDepthTex2D      = 0;
TextureID*            g_pSceneDepthTex2DNonMS = 0;
TextureID*   g_pSceneDepthSRV        = 0;
TextureID*     g_pSceneDepthRTV        = 0;


// Any macros to be passed to the shaders used in this sample (for conditional compilation)
D3D10_SHADER_MACRO g_shadersMacros[] = 
{
    0
};
D3D10_SHADER_MACRO *g_pShadersMacros = g_shadersMacros;


// Mouse State for user-interaction with the simulation
bool                        g_bEnableMouseInteraction = false;
bool                        leftIsPressed = false;
int                         mouse_x = g_gridWidth/2;
int                         mouse_y = g_gridHeight/2;
int                         mouse_z = g_gridDepth/2;
int                         last_x  = g_gridWidth/2;
int                         last_y  = g_gridHeight/2;
int                         last_z  = g_gridDepth/2;
bool                        last_mouse = false;
bool                        mouse_active = false;
bool                        justClicked = false;

bool                        g_bShowHelp = false;    // show help menu
bool                        g_bDrawUI = true;       // show UI

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
enum {
    IDC_TOGGLEFULLSCREEN,
    IDC_TOGGLEREF,
    IDC_CHANGEDEVICE,
    IDC_CHANGEFLUIDTYPE,
    IDC_CHANGERENDER,

    IDC_PAUSEANIMATION,
    IDC_PAUSESIMULATION,
    IDC_RENDERFLUID,
    IDC_RENDERMODEL,
    IDC_DO_INOUT_VOXELIZATION,
    IDC_DO_VEL_VOXELIZATION,

    IDC_USEMACCORMACK,
    IDC_GRAVITY,
    IDC_WATER_STREAM,

    IDC_GRIDWIDTH_STATIC,
    IDC_GRIDWIDTH_SCALE,
    IDC_GRIDHEIGHT_STATIC,
    IDC_GRIDHEIGHT_SCALE,
    IDC_GRIDDEPTH_STATIC,
    IDC_GRIDDEPTH_SCALE,
    IDC_GRIDAPPLY,

    IDC_NUMJACOBI_STATIC,
    IDC_NUMJACOBI_SCALE,
    
    IDC_TOGGLEPRESET1,
    
    IDC_RENDERGLOW,
    IDC_GLOWCONTRIBUTION_STATIC,
    IDC_GLOWCONTRIBUTION_SCALE,

    IDC_FINALINTENSITY_STATIC,
    IDC_FINALINTENSITY_SCALE,
    IDC_FINALALPHA_STATIC,
    IDC_FINALALPHA_SCALE,
    IDC_SMOKECOLOR_STATIC,
    IDC_SMOKECOLOR_SCALE,
    IDC_SMOKEALPHA_STATIC,
    IDC_SMOKEALPHA_SCALE,
    IDC_REDNESS_STATIC,
    IDC_REDNESS_SCALE,

    IDC_XYVELOCITY_STATIC,
    IDC_XYVELOCITY_SCALE,
    IDC_ZVELOCITY_STATIC,
    IDC_ZVELOCITY_SCALE,

};

class App: public STXGUI
{
public:
    App(){}
    virtual ~App(){}
    int init(const char* aTitle){STXGUI::init("/GUILayout.xml");}
    void render( ){STXGUI::update();}

//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
int OnCreateDevice( const DXGI_SURFACE_DESC *pBufferSurfaceDesc,  )
{
    int hr(0);

    // Initialize skybox and environment map
    char str[MAX_PATH];
    stx_Resource pRes = 0;
    ( NVUTFindDXSDKMediaFileCchT( str, MAX_PATH, "..\\..\\Media\\VolumetricWater\\cubemap.dds" ) );
    ( D3DXFROMWINE10CreateTextureFromFile( IRenderer::GetRendererInstance(), str, 0, 0, &pRes, 0 ) );
    if(pRes)
    {
        ( pRes->QueryInterface( __uuidof( TextureID ), (LPVOID*)&g_pEnvMap ));
        if (g_pEnvMap)
        {
            SURFACE_DESC desc;
            g_pEnvMap->GetDesc( &desc );
            D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
            ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
            SRVDesc.Format = desc.Format;
            SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
            SRVDesc.TextureCube.MipLevels = desc.MipLevels;
            ( IRenderer::GetRendererInstance()->CreateShaderResourceView( g_pEnvMap, &SRVDesc, &g_pEnvMapSRV ));
        }
        //////////////SAFE_RELEASE(pRes);
    }
    ( g_Skybox.OnCreateDevice( IRenderer::GetRendererInstance() ) );
    g_Skybox.SetTexture( "..\\..\\Media\\VolumetricWater\\cubemap.dds" );

    (CreateMesh());

    (InitializeFluidState());

    // Initialize the view matrix
    g_Camera.SetViewParams( &g_Eye, &g_At );
    g_Camera.SetEnablePositionMovement(true);
    g_Camera.SetScalers(0.004f, 20.0f);
    g_View = *g_Camera.GetViewMatrix();
    g_Projection = *g_Camera.GetProjMatrix();

    resetVariablesToPreset1();
    resetGUIControls();

    return 0;
}

//--------------------------------------------------------------------------------------
// Load a mesh to put inside the fluid
//--------------------------------------------------------------------------------------
int CreateMesh
{
    int hr(0);

    // try loading skinned mesh
    char fullMeshPath[MAX_PATH];

#if 1
    (NVUTFindDXSDKMediaFileCchT( fullMeshPath, MAX_PATH, ("..\\..\\Media\\Gargoyle\\gargoyle.dae") ));
    g_voxelization_includeNodes.push_back("GargoyleCollision");
    g_rendering_excludeNodes.push_back("GargoyleCollision");
#else
    //(NVUTFindDXSDKMediaFileCchT( fullMeshPath, MAX_PATH, ("..\\..\\Media\\bunny.dae") ));
    //(NVUTFindDXSDKMediaFileCchT( fullMeshPath, MAX_PATH, ("..\\..\\Media\\dragon.dae") ));
    //(NVUTFindDXSDKMediaFileCchT( fullMeshPath, MAX_PATH, ("..\\..\\Media\\happy.dae") ));
#endif
    g_skinnedMesh = SkinnedMesh::CreateD3D10FromColladaFile(fullMeshPath, IRenderer::GetRendererInstance(), MAX_SO_BUFFERS);
    if(!g_skinnedMesh)
        return 1;

    g_skinnedMesh->GetBoundingSphere(g_meshCenter, &g_meshRadius);

    return 0;
}


//--------------------------------------------------------------------------------------
// Initialize fluid and voxelizer states
//--------------------------------------------------------------------------------------
int InitializeFluidState
{
    int hr(0);

    SAFE_DELETE(g_fluid);
    SAFE_DELETE(g_voxelizer);
	SAFE_DELETE(g_renderer);


    // Initialize fluid state
    g_fluid = new Fluid();
    if( !g_fluid ) 
        return E_OUTOFMEMORY;
	g_renderer = new VolumeRenderer( IRenderer::GetRendererInstance() );
    if( !g_renderer ) 
	{        
		SAFE_DELETE(g_renderer);
		return E_OUTOFMEMORY;
	}

    (g_fluid->Initialize(g_gridWidth, g_gridHeight, g_gridDepth, g_eFluidType));

	VolumeRenderer::RENDER_MODE renderMode(VolumeRenderer::RM_SMOKE);
    switch( g_eFluidType )
    {
        default:
		case Fluid::FT_SMOKE:
            renderMode = VolumeRenderer::RM_SMOKE;
            break;
		case Fluid::FT_FIRE:
            renderMode = VolumeRenderer::RM_FIRE;
            break;
		case Fluid::FT_LIQUID:
            renderMode = VolumeRenderer::RM_LEVELSET;
            break;
    }
	
	(g_renderer->Initialize( g_fluid->GetDimX(), g_fluid->GetDimY(), g_fluid->GetDimZ(), renderMode));
    (g_renderer->SetScreenSize(g_Width, g_Height));

    if( g_SampleUI.GetCheckBox( IDC_USEMACCORMACK ) )
        g_fluid->SetUseMACCORMACK(g_SampleUI.GetCheckBox( IDC_USEMACCORMACK )->GetChecked());

    // Initialize voxelizer state
    g_voxelizer = new Voxelizer();
    if( !g_voxelizer )
        return E_OUTOFMEMORY;

    if( g_SampleUI.GetCheckBox( IDC_DO_INOUT_VOXELIZATION ) )
        g_voxelizer->SetEnableInOutVoxelize(g_SampleUI.GetCheckBox( IDC_DO_INOUT_VOXELIZATION )->GetChecked());
    if( g_SampleUI.GetCheckBox( IDC_DO_VEL_VOXELIZATION ) )
        g_voxelizer->SetEnableVelocityVoxelize(g_SampleUI.GetCheckBox( IDC_DO_VEL_VOXELIZATION )->GetChecked());

    (g_voxelizer->SetDestination(IRenderer::GetRendererInstance(), 
        g_fluid->Get3DTexture(Fluid::RENDER_TARGET_OBSTACLES),
        g_fluid->Get3DTexture(Fluid::RENDER_TARGET_OBSTVELOCITY)));

    if( g_eFluidType == Fluid::FT_FIRE )
        g_voxelizer->SetVelocityMultiplier(0.2f);
  

    g_bStreamoutSkinnedMesh = !g_bPauseAnimation;


    // Re-initialize the UI controls
    resetGUIControls();

    g_bReinitializeFluid = false;

    return hr;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
int OnResizedSwapChain( IDXGISwapChain *pSwapChain, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,  )
{
    int hr(0);
    
    IRenderer::GetRendererInstance()->OMSetRenderTargets( 0, 0, 0 );

    g_Width = pBufferSurfaceDesc->Width;
    g_Height = pBufferSurfaceDesc->Height;

    // Setup the projection parameters again
    float fAspect = static_cast<float>( pBufferSurfaceDesc->Width )/static_cast<float>( pBufferSurfaceDesc->Height );
    g_Camera.SetProjParams( g_Fovy , fAspect, g_zNear, g_zFar);

    ( ReinitWindowSizeDependentRenderTargets());
    if( g_renderer ) 
       ( g_renderer->SetScreenSize(g_Width, g_Height) );

    g_Skybox.OnResizedSwapChain(IRenderer::GetRendererInstance(), pBufferSurfaceDesc);

    resetGUIControls();

    return 0;
}

//--------------------------------------------------------------------------------------
// Initialize any textures that must match the window size
//--------------------------------------------------------------------------------------
int ReinitWindowSizeDependentRenderTargets
{
    int hr(0);

    // Create resources to enable writing the scene depth using MRT, as well as to 
    //  enable reading as a shader resource
    TextureID *pRTV = DXUTGetD3D10RenderTargetView();
    stx_ResourcepRTVResource;
    pRTV->GetResource(&pRTVResource);
    TextureID *pRTVTex2D = static_cast<TextureID*>(pRTVResource);
    assert(pRTVTex2D);
    SURFACE_DESC pRTVTex2DDesc;
    pRTVTex2D->GetDesc(&pRTVTex2DDesc);
    pRTVResource->Release();    

    //////////////SAFE_RELEASE(g_pSceneDepthTex2DNonMS);
    //////////////SAFE_RELEASE(g_pSceneDepthTex2D);
    //////////////SAFE_RELEASE(g_pSceneDepthRTV);
    //////////////SAFE_RELEASE(g_pSceneDepthSRV);

    SURFACE_DESC desc;
    desc.ArraySize = 1;
    desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
    desc.CPUAccessFlags = 0;
    desc.MipLevels = 1;
    desc.MiscFlags = 0;
    desc.SampleDesc = pRTVTex2DDesc.SampleDesc;
    desc.Usage = D3D10_USAGE_DEFAULT;
    desc.Width = g_Width;
    desc.Height = g_Height;
    desc.Format = DXGI_FORMAT_R32_FLOAT;
    (IRenderer::GetRendererInstance()->CreateTexture2D(&desc,0,&g_pSceneDepthTex2D));

    // We need a Non-Multisampled texture2D resource of the same dimensions to read from in shaders
    if(pRTVTex2DDesc.SampleDesc.Count > 1)
    {
        desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        (IRenderer::GetRendererInstance()->CreateTexture2D(&desc,0,&g_pSceneDepthTex2DNonMS));
    }
    else
    {
        g_pSceneDepthTex2DNonMS = g_pSceneDepthTex2D;
        g_pSceneDepthTex2DNonMS->AddRef();
    }

    // Create the render target view for the potentially Multisampled texture2D resource
    SURFACE_DESC descRTV;
    descRTV.Format = DXGI_FORMAT_R32_FLOAT;
    if( pRTVTex2DDesc.SampleDesc.Count <= 1 )
    {
        descRTV.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
        descRTV.Texture2D.MipSlice = 0;
    }
    else
    {
        descRTV.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DMS;
    }
    ( IRenderer::GetRendererInstance()->CreateRenderTargetView( g_pSceneDepthTex2D, &descRTV, &g_pSceneDepthRTV ) );

    // Create a shader resource view for a Non-MS texture
    D3D10_SHADER_RESOURCE_VIEW_DESC descSRV;
    descSRV.Format = DXGI_FORMAT_R32_FLOAT;
    descSRV.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    descSRV.Texture2D.MipLevels = 1;
    descSRV.Texture2D.MostDetailedMip = 0;
    ( IRenderer::GetRendererInstance()->CreateShaderResourceView(g_pSceneDepthTex2DNonMS, &descSRV, &g_pSceneDepthSRV) );

    return 0;

}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void OnFrameRender( double fTime, float fElapsedTime,  )
{
    if( !g_bPauseAnimation )
    {
        // animation moves at the same rate as the fluid simulation
        float fTimestep = g_fTimestep * 0.025f;

        g_fModelRotation += fTimestep * g_fRotSpeed/10; 

        if( g_skinnedMesh )
            g_skinnedMesh->Update(fTimestep);
    }


    // Get the render target and depth buffer, and clear them
    TextureID *pRTV = DXUTGetD3D10RenderTargetView();
    TextureID *pDSV = DXUTGetD3D10DepthStencilView();
    
    if(g_eFluidType == Fluid::FT_FIRE)
    {
        float color[4] = {40.0f/255.0f, 40.0f/255.0f, 40.0f/255.0f, 0 };
        IRenderer::GetRendererInstance()->ClearRenderTargetView( pRTV, color );
    }
    else
    {
        float color[4] = {0, 0, 0, 0 };
        IRenderer::GetRendererInstance()->ClearRenderTargetView( pRTV, color );
    }
    float colorZ[4] = { g_zFar,0,0,0};
    IRenderer::GetRendererInstance()->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH , 1.0, 0 );
    IRenderer::GetRendererInstance()->ClearRenderTargetView( g_pSceneDepthRTV, colorZ );
    
    // Create a viewport to match the screen size
    D3DFROMWINEVIEWPORT9 rtViewport;
    rtViewport.TopLeftX = 0;
    rtViewport.TopLeftY = 0;
    rtViewport.MinDepth = 0;
    rtViewport.MaxDepth = 1;
    rtViewport.Width = g_Width;
    rtViewport.Height = g_Height;

    // If the grid size changed we need to reinitialize the fluid
    if(g_bReinitializeFluid)
    {
        int hr(0);
        (InitializeFluidState());
    }

    if(g_fluid == 0)
    {
        IRenderer::GetRendererInstance()->OMSetRenderTargets( 0, 0, 0 );
        return;
    }
    
    
    // Update the simulation one time step
    if(!g_bPause)
    {
        g_fluid->SetNumJacobiIterations(g_numJacobi);
        g_fluid->Update( g_fTimestep);
    }

    // Initialize the world matrices for the simulation Grid and the obstacle Mesh
    //   Grid
    D3DXFROMWINEMATRIX gridScale, gridRotate;
    D3DXFROMWINEMatrixScaling(&gridScale, 20.0f, 20.0f, 20.0f );
    D3DXFROMWINEMatrixRotationX(&gridRotate, 3.0f*3.1416f/2.0f);
    g_gridWorld = gridScale * gridRotate;
    //   Mesh
    D3DXFROMWINEMATRIX meshScale, meshRotate, meshTranslate;
    D3DXFROMWINEVECTOR3 meshT(0,0,0);
    float meshS(1);
    if(g_eFluidType == Fluid::FT_FIRE) 
    {   
        meshS = 10.6f / g_meshRadius;
        meshT = D3DXFROMWINEVECTOR3(-g_meshCenter[0]*meshS, -g_meshCenter[1]*meshS -4.0f, -g_meshCenter[2]*meshS);
    }
    else
    {
        meshS = 10.0f / g_meshRadius;
        meshT = D3DXFROMWINEVECTOR3(-g_meshCenter[0]*meshS, -g_meshCenter[1]*meshS, -g_meshCenter[2]*meshS);
    }
    D3DXFROMWINEMatrixTranslation(&meshTranslate, meshT .x, meshT.y, meshT.z );
    D3DXFROMWINEMatrixScaling(&meshScale, meshS, meshS, meshS);
    D3DXFROMWINEMatrixRotationY(&meshRotate, 3.146f);
    g_meshWorld = meshScale * meshRotate * meshTranslate;
    

    if(g_voxelizer && (g_voxelizer->GetEnableInOutVoxelize() || g_voxelizer->GetEnableVelocityVoxelize()) )
    {
        if( g_bStreamoutSkinnedMesh )
        {
            // Streamout the skinned mesh, it's needed for voxelization only
            D3DXFROMWINEMATRIX identity;
            D3DXFROMWINEMatrixIdentity(&identity);
            g_skinnedMesh->SetRenderFilterExclude(0);
            g_skinnedMesh->SetRenderFilterInclude(0);
            g_skinnedMesh->RenderToSO((float*)&identity, g_dstSOBuffer);

            // Advance the streamout buffer counters
            g_numWrittenSOBuffers++;
            
            // handle potential overflow
            if(g_numWrittenSOBuffers == 0)
                g_numWrittenSOBuffers = 2;
            
            g_srcSOBuffer = g_dstSOBuffer;
            g_dstSOBuffer++;
            g_dstSOBuffer %= MAX_SO_BUFFERS;

            // stop streamout after streaming out 3 times since paused
            if( g_bPauseAnimation )
            {
                g_numWrittenSOBuffersSincePause++;
                if( g_numWrittenSOBuffersSincePause > 2)
                    g_bStreamoutSkinnedMesh = false;
            }
        }

        // Voxelize the mesh if needed
        if(g_numWrittenSOBuffers > 1)
        {
            // Compute mesh-to-grid xform
            D3DXFROMWINEMATRIX gridWorldInv;
            D3DXFROMWINEMatrixInverse(&gridWorldInv, 0, &g_gridWorld);
            D3DXFROMWINEMATRIX meshToGridXForm = g_meshWorld * gridWorldInv;

            int prevSrcSOBuffer = g_srcSOBuffer-1;
            if(prevSrcSOBuffer < 0) prevSrcSOBuffer = MAX_SO_BUFFERS-1;

            if( g_voxelization_includeNodes.size() > 0 )
                g_skinnedMesh->SetRenderFilterInclude(&g_voxelization_includeNodes);
            //if( g_rendering_excludeNodes.size() > 0 )
            //    g_skinnedMesh->SetRenderFilterExclude(&g_rendering_excludeNodes);
            

            g_voxelizer->Voxelize(meshToGridXForm, g_skinnedMesh, 
                g_srcSOBuffer, prevSrcSOBuffer, g_fTimestep);

            g_skinnedMesh->SetRenderFilterInclude(0);
            g_skinnedMesh->SetRenderFilterExclude(0);
        }

    }

    if(g_currentRenderChoice > 0) 
    {
        // Draw one of the simulation textures
        g_fluid->Draw( g_currentRenderChoice );
    } 
    else
    {
        TextureID *pCnDRTs[2] = { pRTV, g_pSceneDepthRTV };
        IRenderer::GetRendererInstance()->RSSetViewports(1,&rtViewport);
        IRenderer::GetRendererInstance()->OMSetRenderTargets( 2, pCnDRTs, pDSV );

        // Compute and set modelview matrix
        g_Projection = *(g_Camera.GetProjMatrix());
        g_View = *(g_Camera.GetViewMatrix());

        // Render the skybox if needed
        if( g_eFluidType == Fluid::FT_LIQUID )
        {
            g_Skybox.OnFrameRender(g_View * g_Projection);
        }

        // Render the mesh
        if(g_bRenderMesh)
        {
            if( g_rendering_excludeNodes.size() > 0 )
                g_skinnedMesh->SetRenderFilterExclude(&g_rendering_excludeNodes);

            // Render the scene color and depth (so that we can sample from it in VolumeRenderer)
            g_skinnedMesh->Render((float*)&g_meshWorld, (float*)&g_View, (float*)&g_Projection);

            g_skinnedMesh->SetRenderFilterExclude(0);
        }

        // Resolve the MS depth buffer into a single-sampled one if needed
        if(g_pSceneDepthTex2DNonMS != g_pSceneDepthTex2D)
        {
            IRenderer::GetRendererInstance()->ResolveSubresource(g_pSceneDepthTex2DNonMS, 0, 
                g_pSceneDepthTex2D, 0, DXGI_FORMAT_R32_FLOAT);
        }

        // Render the fluid
        if(g_bRenderFluid)
            g_renderer->Draw( g_fluid->getCurrentShaderResourceView());

    }
}

//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnCreateDevice 
//--------------------------------------------------------------------------------------
void OnDestroyDevice(  )
{
    //////////////SAFE_RELEASE(g_pEnvMapSRV);
    //////////////SAFE_RELEASE(g_pEnvMap);

    //////////////SAFE_RELEASE( g_pFont );
    //////////////SAFE_RELEASE( g_pSprite );
    SAFE_DELETE( g_pTxtHelper );


    SAFE_DELETE(g_skinnedMesh);

    //////////////SAFE_RELEASE(g_pSceneDepthTex2DNonMS);
    //////////////SAFE_RELEASE(g_pSceneDepthTex2D);
    //////////////SAFE_RELEASE(g_pSceneDepthRTV);
    //////////////SAFE_RELEASE(g_pSceneDepthSRV);


    SAFE_DELETE(g_fluid);
    SAFE_DELETE(g_voxelizer);
	SAFE_DELETE(g_renderer);
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void OnFrameMove( double fTime, float fElapsedTime,  )
{
    if(!g_bEnableMouseInteraction) 
        g_Camera.FrameMove( fElapsedTime );
}

void resetVariablesToPreset1()
{
    g_glowContribution     = 0.81f;
    g_finalIntensityScale  = 28.0f;
    g_finalAlphaScale      = 0.95f;
    g_smokeColorMultiplier = 2.0f;
    g_smokeAlphaMultiplier = 0.05f;
    g_RednessFactor        = 5;
    g_zVelocityScale       = 4.8f;
    g_xyVelocityScale      = 4.0f;

}

virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
    GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
    if(radioButton->isPressed() && radioButton->isChecked())
    {
		if(callbackString == "")
      	{
		}
	}
	}
}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Fluid Simulation Sample");
	IInput*    i=STX_Service::GetInputInstance();
	App app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}

