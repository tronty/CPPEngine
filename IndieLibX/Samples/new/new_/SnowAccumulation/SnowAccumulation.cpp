/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// Copyright NVIDIA Corporation 2004
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED 
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS 
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL 
// NVIDIA OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR 
// CONSEQUENTIAL DAMAGES WHATSOEVER INCLUDING, WITHOUT LIMITATION, DAMAGES FOR 
// LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS 
// INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR 
// INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGES.
// 
// File: SnowAccumultation.cpp
// Desc: This sample snows the use of a orthographic rendering into a RT to obtain 
//			the closest distance to the sky, and then use that value to create a snow coverage of the scene.
//		This method is being done in real time in this sample, but it is easily convceivable to preprocess a snow map,
//		and then jsut use this, as long as occluders don't move
//-----------------------------------------------------------------------------

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders

//include <list>
//pragma warning( disable : 4311)	// we cast a void* to an int intentionally

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
bool                    g_bShowHelp = true;     // If true, it renders the UI control text
D3DXFROMWINEMATRIX g_ProjMatrix;
D3DXFROMWINEMATRIX g_ViewMatrix;
bool                    g_bEnablePreshader;     // if TRUE, then D3DXFROMWINESHADER_NO_PRESHADER is used when compiling the shader

// NV Vars
#define WORLD_SIZE		1200.0f
int						g_iExposureMapResolution = 512;
stx_SkyBox				g_skybox;
stx_Effect*            g_pEffect = 0;       // D3DXFROMWINE effect interface
#if 0
stx_Effect::stx_Technique* RenderScene_NoSnow_Technique=0;
stx_Effect::stx_Technique* RenderScene_SM20_Technique=0;
stx_Effect::stx_Technique* RenderScene_SM30_Technique=0;
stx_Effect::stx_Technique* RenderDepthPass_Technique=0;
#endif

SurfaceID			g_pBBSurface = 0;	// original swap chain surface
SurfaceID			g_pBBDepthSurface = 0;	// original swap chain depth surface
TextureID			g_pExposureTexture = 0;
TextureID			g_pExposureTextureRGBA = 0;
TextureID			g_pExposureTextureR32 = 0;
SurfaceID			g_pExposureSurface = 0;
TextureID			g_pExposureDepthTexture = 0;
SurfaceID			g_pExposureDepthSurface = 0;	// original swap chain depth surface
TextureID	g_pNoiseTexture = 0;

stx_Effect*				g_pExposureCalcEffect = 0;
D3DXFROMWINEMATRIX					g_pExposureView;
D3DXFROMWINEMATRIX					g_pExposureOrthoProj;

bool bSM30Support;
bool bExposureChanged=true;

/*
	Simple wrapper class that allows me to easily place a couple of meshes around the world, and be able to
	control whether they accumulate snow or are just occluders
*/
class PlacedMesh
{
public:
	MeshRenderer2				Mesh;			// The mesh to draw
    TextureID               mTex;
	D3DXFROMWINEMATRIX      mWorld;			// Where it is
	bool					bAccumSnow;		// does this object accumulate snow?
	bool					bOccludeSnow;	// does this object block snow from faling on object below it?
	float					fMaxOffset;		// if object accumulates, what is the max vertex offset to use?  Not needed, but makes effect look better.

	PlacedMesh(std::string file, D3DXFROMWINEMATRIX&world, bool accum, bool occlude,float maxOffset, TextureID aTex=-1)
	{
        Mesh.Load(file.c_str(), eSimpleShader, false, false);
        mTex=aTex;
		mWorld = world;
		bAccumSnow = accum;
		bOccludeSnow = occlude;
		fMaxOffset = maxOffset;
	}

	PlacedMesh(std::string file, D3DXFROMWINEMATRIX&world, bool accum, bool occlude,float maxOffset, std::string aTexFile)
	{
        Mesh.Load(file.c_str(), eSimpleShader, false, false);
        mTex = IRenderer::GetRendererInstance()->addImageLibTexture(aTexFile.c_str(), false, IRenderer::GetRendererInstance()->Gettrilinear());
		mWorld = world;
		bAccumSnow = accum;
		bOccludeSnow = occlude;
		fMaxOffset = maxOffset;
	}
	virtual ~PlacedMesh(){}
	void InvalidateDeviceObjects(){}
	void RestoreDeviceObjects(){}
};

std::list<PlacedMesh> g_SceneMeshList;
PlacedMesh *g_pShip = 0;

D3DXFROMWINEVECTOR3 g_LightControl;
float                g_fLightScale;
int                  g_nNumActiveLights;
int                  g_nActiveLight;


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_OFFSETAMOUNT		5
#define IDC_SAMPLEDISTANCE		6
#define IDC_SNOWON				7
#define IDC_BASESNOWPCT			9
#define IDC_NORMALDISTORT		10
#define IDC_DOTNORMDISTORT		11
#define IDC_SHOWEXPMAP			12
#define IDC_SNOWBIAS			13

enum E_RENDERMODE
{
	RM_NOSNOW=0,
	RM_SM20=1,
	RM_SM30=2,
};

#define IDC_STATIC_BASE			100

struct App
{
    App(){}
    virtual ~App(){}
//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_bEnablePreshader = true;

    g_LightControl=D3DXFROMWINEVECTOR3( 0.5f, 0.8f, 0.5f );

    g_nActiveLight = 0;
    g_nNumActiveLights = 1;
    g_fLightScale = 1.0f;
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

    D3DXFROMWINEVECTOR3 vCenter = D3DXFROMWINEVECTOR3(0,0,0);
	float fObjectRadius = 1.0;
    //g_LightControl.SetRadius( fObjectRadius );

    g_pEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/new/SnowAccumulation/Snow.fx");

	// Seperate fx file for the exposure map calculation, and set its projection matrix
	g_pExposureCalcEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/new/SnowAccumulation/OrthoDepth.fx");

#if 0
	RenderScene_NoSnow_Technique=g_pEffect->GetTechniqueByName( "RenderScene_NoSnow");
	RenderScene_SM20_Technique=g_pEffect->GetTechniqueByName( "RenderScene_SM20");
	RenderScene_SM30_Technique=g_pEffect->GetTechniqueByName( "RenderScene_SM30";
	RenderDepthPass_Technique=g_pExposureCalcEffect->GetTechniqueByName("RenderDepthPass");
#endif

	// make our view proj matrix for the "snow"'s view.
	// $$ These values control the range for the encoded depths values, so fitting it close to the 
	//		actual range of the possible values will results in better precision utilization.
	// Be careful if your occluder objects fall outside this clipping range, the they will not occlude at all.
	D3DXFROMWINEMatrixOrthoLH(&g_pExposureOrthoProj,WORLD_SIZE,WORLD_SIZE,1.f,1000.f);
	D3DXFROMWINEMatrixLookAtLH(&g_pExposureView,&D3DXFROMWINEVECTOR3(0,540,0),&D3DXFROMWINEVECTOR3(0,0,0),&D3DXFROMWINEVECTOR3(0,0,1));

	D3DXFROMWINEMATRIX mWorld;
	D3DXFROMWINEMATRIX mTrans,mRot,mRot2,mScale;
	TextureID pTexture;

	// Load some trees or other interesting objects, and then the terrain.  This is all hard coded since it is just a sample.

	// ------------------
#if 1
	// A rock, gets and occludes snow
	D3DXFROMWINEMatrixTranslation(&mTrans,100,-30,30);
	D3DXFROMWINEMatrixRotationZ(&mRot,-3.141592654f/2);
	D3DXFROMWINEMatrixScaling(&mScale,50,50,50);
	mWorld = mScale*mRot*mTrans;

	{PlacedMesh Mesh("/new/SnowAccumulation/aster1.x",mWorld,true,true,15.f, "/new/SnowAccumulation/terrainRock.bmp");
	 g_SceneMeshList.push_back(Mesh);}
#endif

	// ------------------
#if 1
	// A Tree, gets and occludes
	D3DXFROMWINEMatrixTranslation(&mTrans,80,-50,90);
	//	D3DXFROMWINEMatrixRotationZ(&mRot,-3.141592654f/2);
	D3DXFROMWINEMatrixScaling(&mScale,25,15,25);
	mWorld = mScale*mTrans;
	{PlacedMesh Mesh("/new/SnowAccumulation/snowAccumulation_tree.x",mWorld,true,true,8.f,"/new/SnowAccumulation/bark.tga");
	 g_SceneMeshList.push_back(Mesh);}
#endif

	// ------------------
#if 1
	// A Tree, gets and occludes
	D3DXFROMWINEMatrixTranslation(&mTrans,230,10,-185);
	D3DXFROMWINEMatrixScaling(&mScale,35,35,35);
	mWorld = mScale*mTrans;
	{PlacedMesh Mesh("/new/SnowAccumulation/tree_0.x",mWorld,true,true,4.f);
	g_SceneMeshList.push_back(Mesh);}

	D3DXFROMWINEMatrixTranslation(&mTrans,150,20,-200);
	D3DXFROMWINEMatrixScaling(&mScale,35,40,35);
	mWorld = mScale*mTrans;
	{PlacedMesh Mesh("/new/SnowAccumulation/tree_0.x",mWorld,true,true,4.f);
	g_SceneMeshList.push_back(Mesh);}

	D3DXFROMWINEMatrixTranslation(&mTrans,100,20,-280);
	D3DXFROMWINEMatrixScaling(&mScale,45,45,45);
	mWorld = mScale*mTrans;
	{PlacedMesh Mesh("/new/SnowAccumulation/tree_0.x",mWorld,true,true,4.f);
	g_SceneMeshList.push_back(Mesh);}

	D3DXFROMWINEMatrixTranslation(&mTrans,0,35,-190);
	D3DXFROMWINEMatrixScaling(&mScale,55,35,55);
	mWorld = mScale*mTrans;
	{PlacedMesh Mesh("/new/SnowAccumulation/tree_0.x",mWorld,true,true,4.f);
	g_SceneMeshList.push_back(Mesh);}

#endif

	// ------------------
	// Snowman!!
#if 1
	D3DXFROMWINEMatrixTranslation(&mTrans,-200,-55,150);
	D3DXFROMWINEMatrixRotationZ(&mRot,3.141592654f/20);
	D3DXFROMWINEMatrixRotationY(&mRot2,3.141592654f*1.3f);
	D3DXFROMWINEMatrixScaling(&mScale,20,20,20);
	mWorld = mScale*mRot2*mRot*mTrans;
	{PlacedMesh Mesh("/new/SnowAccumulation/snowman.x",mWorld,false,true,0.f);
	g_SceneMeshList.push_back(Mesh);}
#endif


	// ---------------
	// Our ground mesh, loaded last since it'll generally be behind everything, so this is "rough front to back sorting"
	D3DXFROMWINEMatrixTranslation( &mTrans, 0.0f, -10.f, 0.f );
	//D3DXFROMWINEMatrixRotationX(&mRot,3.141592654f/6);
	D3DXFROMWINEMatrixScaling(&mScale,5.f,5.f,5.f);
	mWorld = mScale*mTrans;

#if 1
    {PlacedMesh Mesh("/new/SnowAccumulation/snowAccumulation_terrain.x",mWorld,false,true,0.f);
	g_SceneMeshList.push_back(Mesh);}
#else
	{PlacedMesh Mesh("/new/SnowAccumulation/snowAccumulation_terrain.x",mWorld,true,true,25.f);

	// $$$$ This is ugly, but i cant edit x files, and these meshes dont have smothed normals and it looks horrible.
	//  so set the alpha for all non smooth objects to 0, so they dont draw...
	for(unsigned int i=0;i<pPlacedMesh->pMesh->m_dwNumMaterials;i++)
	{
		if(i==0) continue;
		pPlacedMesh->pMesh->m_pMaterials[i].Diffuse.a = 0.0f;
	}
	g_SceneMeshList.push_back(Mesh);
#endif

    return 0;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// reset, which will happen after a lost device scenario. This is the best location to
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever
// the device is lost. Resources created here should be released in the OnLostDevice
// callback.
//--------------------------------------------------------------------------------------
int  OnResetDevice()
{
    int hr;

    char str[MAX_PATH];

	// Setup the camera's projection parameters
	float fAspectRatio = IRenderer::GetRendererInstance()->GetAspectRatio();
	D3DXFROMWINEMatrixPerspectiveFovLH(&g_ProjMatrix, D3DXFROMWINE_PI/4, fAspectRatio, 1.0f, 5000.0f );
	//g_Camera.SetRotateButtons(true,false,false);

	// Setup the camera's view parameters
	D3DXFROMWINEVECTOR3 vecEye(-15.0f, 30.0f, 440.0f);
	D3DXFROMWINEVECTOR3 vecAt (0.0f, -20.0f, 0.0f);
	D3DXFROMWINEVECTOR3 vecUp (0.0f, 1.0f, 0.0f);
	D3DXFROMWINEMatrixLookAtLH(&g_ViewMatrix, &vecEye, &vecAt, &vecUp);
	//g_Camera.SetScalers(0.01f,95.f);

	bExposureChanged = true;

	//---------------------------------------------------------------------------
	// initilize our sky box and it's cube map

	TextureID cubeTex;
	g_skybox.init("/new/SnowAccumulation/CloudyHillsCubemap.dds");

#if 0 // ???
	// Store off BB
	IRenderer::GetRendererInstance()->GetRenderTarget(0,&g_pBBSurface);
	IRenderer::GetRendererInstance()->GetDepthStencilSurface(&g_pBBDepthSurface);
#endif

	// Make an RGBA RT possibly to store the exposure values
    g_pExposureTextureRGBA = IRenderer::GetRendererInstance()->addRenderTarget(g_iExposureMapResolution,g_iExposureMapResolution, FORMAT_RGBA8, IRenderer::GetRendererInstance()->GetnearestClamp());

	// Need to have both surfaces since can't StretchRect R32 to backbuffer for visualization.  In normal situation, only need one.
	if(bSM30Support)
	{
		// Make a 32bit float RT for storing the exposure depth
        g_pExposureTextureR32 = IRenderer::GetRendererInstance()->addRenderTarget(g_iExposureMapResolution,g_iExposureMapResolution, FORMAT_R32F, IRenderer::GetRendererInstance()->GetnearestClamp());
		g_pExposureTexture = g_pExposureTextureR32;
	}
	else
	{
		g_pExposureTexture = g_pExposureTextureRGBA;
	}

    // D3DUSAGE_DEPTHSTENCIL,D3DFMT_D24X8,D3DPOOL_DEFAULT ???
    g_pExposureDepthTexture = IRenderer::GetRendererInstance()->addDepthStencilSurface(g_iExposureMapResolution,g_iExposureMapResolution, FORMAT_D24, IRenderer::GetRendererInstance()->GetnearestClamp());

	// This is our noise volume texture, just has some random values.  Combined in shader using a perlin's method
    g_pNoiseTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/new/SnowAccumulation/noiseL8_32x32x32.dds", false);

    return 0;
}

/*
	Renders out a R32F RT which contains the closest depth value to the falling snow.  Note that near and far have been
	pre set to tightly fit our scene.  This ensures more precision in our depth values.
*/
void RenderOrthoDepth()
{
	int hr;
	unsigned int iPass, cPasses;
	// Render out the closest depth to the sky...
	( IRenderer::GetRendererInstance()->changeRenderTarget(0,g_pExposureSurface));
	( IRenderer::GetRendererInstance()->SetDepthStencilSurface(g_pExposureDepthSurface));

	// $ Note when clearing we don't clear to black, but this is ok, we clear to 1 for the red component which is enough
	//		the color is handy for visualization in debug mode
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(1.0f,0.25f,0.25f,0.55f));

	( g_pExposureCalcEffect->SetTechnique("RenderDepthPass") );
	( g_pExposureCalcEffect->SetMatrix("mView",&g_pExposureView));
	( g_pExposureCalcEffect->SetMatrix("mProj",&g_pExposureOrthoProj));
	( g_pExposureCalcEffect->SetMatrix("mViewProj",&(g_pExposureView*g_pExposureOrthoProj)));
	( g_pExposureCalcEffect->Begin(&cPasses, 0) );
	for (iPass = 0; iPass < cPasses; iPass++)
	{
		( g_pExposureCalcEffect->BeginPass(iPass) );
		for(std::list<PlacedMesh>::iterator i=g_SceneMeshList.begin();i!= g_SceneMeshList.end();i++)
		{
			if((*i).bOccludeSnow)
			{
				D3DXFROMWINEMATRIX mWorld = (*i).mWorld;
				( g_pExposureCalcEffect->SetMatrix("mWorldViewProjection",&(mWorld*g_pExposureView*g_pExposureOrthoProj)));
				( g_pExposureCalcEffect->SetMatrix("mWorld",&mWorld));
				//( g_pExposureCalcEffect->CommitChanges());
				(*i).Mesh.render(&mWorld);
			}
		}
		( g_pExposureCalcEffect->EndPass() );
	}
	( g_pExposureCalcEffect->End() );

	(IRenderer::GetRendererInstance()->changeRenderTarget(0,g_pBBSurface));
	(IRenderer::GetRendererInstance()->SetDepthStencilSurface((g_pBBDepthSurface)));
}

void RenderScene( bool accum)
{
	int hr;
	unsigned int iPass, cPasses;	
	D3DXFROMWINEMATRIX mView;
	D3DXFROMWINEMATRIX mProj;

	// Get the projection & view matrix from the camera class
	mProj = g_ProjMatrix;
	mView = g_ViewMatrix;

	// Used when we offset in the exposure map
    float f=STX_Service::GetWindowInstance()->GetWidth();
	( g_pEffect->SetFloat("SceneWidth", &f));
    f=STX_Service::GetWindowInstance()->GetHeight();
	( g_pEffect->SetFloat("SceneHeight", &f));

	( g_pEffect->SetValue( "g_LightDir", &g_LightControl, sizeof(D3DXFROMWINEVECTOR3) ) );
	( g_pEffect->SetValue( "g_LightDiffuse", &(g_fLightScale * D3DXFROMWINECOLOR(1,1,1,1)), sizeof(D3DXFROMWINEVECTOR4) ) );
	( g_pEffect->SetMatrix( "g_mProjection", &mProj ) );
	( g_pEffect->SetMatrix( "g_mViewProj", &(mView*mProj)) );
	D3DXFROMWINEMATRIX mViewInv;
	D3DXFROMWINEMatrixInverse(&mViewInv,0,&mView);
	( g_pEffect->SetMatrix( "g_mViewInverse", &mViewInv) );
	
	( g_pEffect->SetTexture("g_ExposureDepthMapTexture",g_pExposureTexture));
	( g_pEffect->SetTexture("g_Noise3D",g_pNoiseTexture));

	D3DXFROMWINECOLOR vWhite = D3DXFROMWINECOLOR(1,1,1,1);
	( g_pEffect->SetValue("g_MaterialDiffuseColor", &vWhite, sizeof(D3DXFROMWINECOLOR) ) );
	( g_pEffect->SetValue("g_SnowColor",&D3DXFROMWINECOLOR(0.90f,0.95f,1.0f,1.0f),sizeof(D3DXFROMWINECOLOR)));

    f=IDC_SAMPLEDISTANCE/100.f;
	( g_pEffect->SetFloat( "SampleDistance", &f));
    f=300.f;
	( g_pEffect->SetFloat( "SpecExpon", &f));
    f=IDC_BASESNOWPCT/100.f;
	( g_pEffect->SetFloat( "BaseSnowPct", &f));
    f=IDC_SNOWBIAS/10000.f;
	( g_pEffect->SetFloat( "SnowBias", &f));
    f=IDC_NORMALDISTORT/1000.f;
	( g_pEffect->SetFloat( "normalDistortionAmount", &f));
    f=IDC_DOTNORMDISTORT/100.f;
	( g_pEffect->SetFloat( "dotNormalDistortionAmount", &f));
		
	( g_pEffect->SetVector("g_LightAmbient",&D3DXFROMWINEVECTOR4(0.3f,0.3f,0.3f,0.0)));

	// Apply the technique contained in the effect
	( g_pEffect->Begin(&cPasses, 0) );
	for (iPass = 0; iPass < cPasses; iPass++)
	{
		( g_pEffect->BeginPass(iPass) );
		for(std::list<PlacedMesh>::iterator i=g_SceneMeshList.begin();i!= g_SceneMeshList.end();i++)
		{
			D3DXFROMWINEMATRIX mWorld = (*i).mWorld;

			if(accum == (*i).bAccumSnow)
			{
				D3DXFROMWINEMATRIX mInvWorld;
				D3DXFROMWINEMatrixInverse(&mInvWorld,0,&mWorld);
				D3DXFROMWINEVECTOR4 mUpObjectSpace;
				D3DXFROMWINEVec3Transform(&mUpObjectSpace,&D3DXFROMWINEVECTOR3(0,1,0),&mInvWorld);
				( g_pEffect->SetVector("g_upObjectSpace",&mUpObjectSpace));
				( g_pEffect->SetMatrix( "g_mWorldViewProjection", &(mWorld*mView * mProj)));
				( g_pEffect->SetMatrix( "g_mWorld", &mWorld ) );
				D3DXFROMWINEMATRIX mWorldIT;
				D3DXFROMWINEMatrixTranspose(&mWorldIT,D3DXFROMWINEMatrixInverse(&mWorldIT,0,&mWorld));
				( g_pEffect->SetMatrix( "g_mWorldIT", &mWorldIT ) );
				( g_pEffect->SetMatrix( "g_mWorldView", &(mWorld*mView) ) );
				( g_pEffect->SetMatrix( "g_mExposureWorldViewOrthoProj",&(mWorld*g_pExposureView*g_pExposureOrthoProj)));
                    float f=(*i).fMaxOffset * ((float)IDC_OFFSETAMOUNT/100.f );
				  g_pEffect->SetFloat( "OffsetAmount", &f);
				//( g_pEffect->CommitChanges() );
				(*i).Mesh.render(&mWorld);
			}
		}
		
		( g_pEffect->EndPass() );
	}
	( g_pEffect->End() );
}

//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not
// intended to contain actual rendering calls, which should instead be placed in the
// OnFrameRender callback.
//--------------------------------------------------------------------------------------
void  OnFrameMove(  double fTime, float fElapsedTime)
{
    // Update the camera's position based on user input
    //g_Camera.FrameMove( fElapsedTime );

	// if something moves
	// bExposureChanged = true;

#if 0
	D3DXFROMWINEMATRIX mWorld;
	D3DXFROMWINEMATRIX mTrans,mRot,mScale;
	D3DXFROMWINEMatrixTranslation(&mTrans,50*(float)sin((double)fTime*3.141592654/4),10,0);
	D3DXFROMWINEMatrixRotationZ(&mRot,3.141592654f/2);
	D3DXFROMWINEMatrixScaling(&mScale,9,9,9);
	mWorld = mScale*mRot*mTrans;
	g_pShip->mWorld = mWorld;
#endif
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the
// rendering calls for the scene, and it will also be called if the window needs to be
// repainted. After this function has returned, the sample framework will call
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void  OnFrameRender(  double fTime, float fElapsedTime)
{
    int hr;
    D3DXFROMWINEMATRIX mView;

    // Clear the render target and the zbuffer
    IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.0f,0.25f,0.25f,0.55f));

    // Render the scene
    IRenderer::GetRendererInstance()->BeginScene();
    {
		int renderMode = 0;
		// Renders all possible snow occluding objects and generates a distance from the sky surface, used in exposure calculation
		if(bExposureChanged && (renderMode == RM_SM20 || renderMode == RM_SM30))
		{
			RenderOrthoDepth();
			bExposureChanged = false;
		}

		// Draw us a nice sky box
		mView = g_ViewMatrix;	// using the view makes the box move around when we move.
		g_skybox.render();//(mView);

		// $$$ Need to have skybox not render Z, or something...
		IRenderer::GetRendererInstance()->Clear(false, true, D3DXFROMWINEVECTOR4(0.0f,0.25f,0.25f,0.55f));

		// render all non snowed surfaces first.
		( g_pEffect->SetTechnique( "RenderScene_NoSnow" ) );
		RenderScene(false);

		// Render all snow accumulated surfaces
		switch(renderMode)
		{
			case RM_NOSNOW:
			default:
				( g_pEffect->SetTechnique( "RenderScene_NoSnow" ) );
				break;
			case RM_SM20:
				( g_pEffect->SetTechnique( "RenderScene_SM20" ) );
				break;
			case RM_SM30:
				( g_pEffect->SetTechnique( "RenderScene_SM30" ) );
				break;
		}
		RenderScene(true);
#if 0
		// $$ This is just for the visualization of the map, not needed really.
		if(g_SampleUI.GetCheckBox(IDC_SHOWEXPMAP)->GetChecked())
		{
			// visualize the depth surface, only works with RGBA depth surface, not R32F
			RECT destRect;
			D3DSURFACE_DESC desc;
			g_pBBSurface->GetDesc(&desc);
			destRect.left = 0;
			destRect.right = g_iExposureMapResolution/2;
			destRect.top = desc.Height-(g_iExposureMapResolution/2);
			destRect.bottom = desc.Height;

			if(g_pExposureTextureR32 != 0)
			{
				g_pExposureSurface->Release();
				g_pExposureTextureRGBA->GetSurfaceLevel(0,&g_pExposureSurface);
				RenderOrthoDepth();
				(IRenderer::GetRendererInstance()->StretchRect(g_pExposureSurface,0,g_pBBSurface,&destRect,D3DTEXF_NONE));
				g_pExposureSurface->Release();
				g_pExposureTextureR32->GetSurfaceLevel(0,&g_pExposureSurface);
			}
			else
			{
				(IRenderer::GetRendererInstance()->StretchRect(g_pExposureSurface,0,g_pBBSurface,&destRect,D3DTEXF_NONE));
			}
		}
#endif
        ( IRenderer::GetRendererInstance()->EndScene() );
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
	IRenderer* r=IRenderer::GetRendererInstance("SnowAccumulation");
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

