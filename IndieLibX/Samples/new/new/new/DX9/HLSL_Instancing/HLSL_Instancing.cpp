/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
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
// File: HLSL_Instancing.cpp
// Desc: This sample shows the use of D3D's Instancing API.  You should see the whitepaper for
//			the full details.  The gist is that we have two instanced meshes.  Space ships and 
//			asteroids.  The big motherships are just garnish.  The instanced mesh data
//			is in on VB stream, and world matrices are stored in the second stream.  See OnRender()
//			for the guts of this.
//-----------------------------------------------------------------------------
#include "resource.h"

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 

FormatDesc g_aCloneMeshVertDecl[] =
{
	0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
	0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
	0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
	0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
	0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
};

struct MeshVertex
{
	MeshVertex(D3DXFROMWINEVECTOR3 p,D3DXFROMWINEVECTOR3 n,D3DXFROMWINEVECTOR2 u): pos(p),normal(n),uv(u)
	{}
	D3DXFROMWINEVECTOR3 pos;
	D3DXFROMWINEVECTOR3 normal;
	D3DXFROMWINEVECTOR2 uv;
};

FormatDesc g_aMeshVertDecl[] =
{
	0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
	0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
	0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
	0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
	0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
	0, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
	0, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
	0, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
	0, TYPE_TEXCOORD, FORMAT_FLOAT, 4 // COLOR
};

struct MatrixVertex
{
public:
	MatrixVertex();
	MatrixVertex(D3DXFROMWINEVECTOR4& i1,D3DXFROMWINEVECTOR4& i2,D3DXFROMWINEVECTOR4&i3,D3DXFROMWINEVECTOR4&i4,D3DXFROMWINECOLOR&c)
		: r1(i1),r2(i2),r3(i3),c1(c) {}
	D3DXFROMWINEVECTOR4 r1;     // row 1
	D3DXFROMWINEVECTOR4 r2;     // row 2
	D3DXFROMWINEVECTOR4 r3;		// row 3
	D3DXFROMWINEVECTOR4 c1;
};

//include "nv_skybox.h"
#include "AsteroidManager.h"
#include "MothershipManager.h"
#include "SpaceShipManager.h"
#include "nv_d3d9FullScreenQuad.h"

D3DXFROMWINECOLOR vWhite = D3DXFROMWINECOLOR(1,1,1,1);
const static int INSTANCE_INIT_SRAND=1425236;

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
bool                    g_bShowHelp = true;     // If true, it renders the UI control text
stx_CFirstPersonCamera      g_Camera;               // A model viewing camera
stx_Effect*             g_pEffect = -1;       // D3DXFROMWINE effect interface
bool                    g_bEnablePreshader;     // if TRUE, then D3DXFROMWINESHADER_NO_PRESHADER is used when compiling the shader

// Configs
bool	g_bUseInstancing = true;
bool	g_bShowStats = true;
int		g_numShipInstances = 500;
int		g_numRockInstances = 4000;
int		g_numDrawCalls;


// dirty bits
bool g_bReAllocateMatrixVB = true;		// if true the VBs need to be re-created due to size changes

VertexFormatID	g_pMeshVertexDecl;				// Full 2 stream decl
VertexFormatID	g_pCloneMeshVertexDecl;			// 1st stream used to conform VB

// ------------------- Space ships instancing data ---------------
int		g_numShipPolys;
int		g_numShipVerts;
int		g_numShipIndices;
D3DXFROMWINEMATRIX *					g_shipInstanceMatrices=0;
D3DXFROMWINECOLOR *						g_shipInstanceColors=0;
VertexBufferID			g_shipVB=0;						// VB of a single model verts
IndexBufferID			g_shipIB=0;						// IB will contain indices
VertexBufferID			g_shipMatrixVB=0;				// VB of model world matrices
TextureID				g_pShipTexture = 0;			// Mesh texture
TextureID				g_pShipBumpTexture = 0;		// Mesh texture

// ------------------- Asteroid Rock instancing data ---------------
int		g_numRockPolys;
int		g_numRockVerts;
int		g_numRockIndices;
D3DXFROMWINEMATRIX *					g_rockInstanceMatrices=0;
VertexBufferID			g_rockVB=0;						// VB of a single model verts
IndexBufferID			g_rockIB=0;						// IB will contain indices
VertexBufferID			g_rockMatrixVB=0;				// VB of model world matrices
TextureID				g_pRockTexture = 0;			// Mesh texture
TextureID				g_pRockBumpTexture = 0;		// Mesh texture


SurfaceID				g_pSharedDepth = 0;
TextureID				g_pRenderTargetTexture = 0;  // For scene texture
SurfaceID				g_pRenderTargetSurface = 0;

TextureID				g_pDownsampleTargetTexture = 0;  // downsampled scen
SurfaceID				g_pDownsampleTargetSurface = 0;

TextureID				g_pBlurHTargetTexture = 0;  // downsampled and h blurred
SurfaceID				g_pBlurHTargetSurface = 0;

TextureID				g_pBlurVTargetTexture = 0;  // downsampled and h & v blurred
SurfaceID				g_pBlurVTargetSurface = 0;

SurfaceID				g_pBBSurface = 0;	// original swap chain surface
SurfaceID				g_pBBDepthSurface = 0;	// original swap chain depth surface

// Misc classes to control "AI" of ships and placements, and non-instancing stuff
stx_SkyBox* 				g_skybox = 0;
SpaceShipManager*		g_shipManager = new SpaceShipManager();
AsteroidManager*		g_asteroidManager = new AsteroidManager();
MothershipManager *		g_mothershipManager = new MothershipManager();
//nv_D3D9FullScreenQuad*  g_fullScreenQuad = new nv_D3D9FullScreenQuad();

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_VIEWWHITEPAPER      2
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_INSTANCECOUNT		5
#define IDC_INSTANCECOUNT_STATIC 6
#define IDC_ROCKCOUNT			7
#define IDC_ROCKCOUNT_STATIC	8
#define IDC_USEINSTANCING		9

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// created, which will happen during application initialization and windowed/full screen 
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these 
// resources need to be reloaded whenever the device is destroyed. Resources created  
// here should be released in the OnDestroyDevice callback. 
//--------------------------------------------------------------------------------------
int OnCreateDevice()
{
	// -------------------------------------------------------------------------------------------
	// Create our VBs and IBs and stuff
	g_pMeshVertexDecl = IRenderer::GetRendererInstance()->addVertexFormat(g_aMeshVertDecl, elementsOf(g_aMeshVertDecl), -1);
	g_pCloneMeshVertexDecl = IRenderer::GetRendererInstance()->addVertexFormat(g_aCloneMeshVertDecl, elementsOf(g_aCloneMeshVertDecl), -1);

    return 0;
}

//--------------------------------------------------------------------------------------
// This function loads a model from a .X file and optimizes it in place, and marshals it
//	into a VB following the vertex format passed in.
//--------------------------------------------------------------------------------------
int LoadMesh( const char * strFileName, 
				 const FormatDesc* aMeshDecl,
				 VertexBufferID  ppVB,IndexBufferID  ppIB, 
				 int *pNumPolys, int *pNumVerts, int *pNumIndices )
{
	MeshRenderer2 m_Mesh;
	m_Mesh.Load(strFileName, eSimpleShader, true, true);

	// Propagate back to caller
	*pNumIndices = m_Mesh.GetNumIndices();
	*pNumPolys = 0; // ???
	*pNumVerts = m_Mesh.GetNumVertices();

    return 0;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// reset, which will happen after a lost device scenario. This is the best location to 
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever 
// the device is lost. Resources created here should be released in the OnLostDevice 
// callback. 
//--------------------------------------------------------------------------------------
int OnResetDevice()
{
    int hr;

    // Setup the camera's projection parameters
    float fAspectRatio = STX_Service::GetWindowInstance()->GetWidth() / (FLOAT)STX_Service::GetWindowInstance()->GetHeight();
    g_Camera.SetProjParams( D3DXFROMWINE_PI/4, fAspectRatio, 1.0f, 5000.0f );

	// -------------------------------------------------------------------------------------------
	// Load ship model and create VB/IB and instance matrix VB
	( LoadMesh ( "models/SpaceShip/missile1.x", (FormatDesc*)g_aCloneMeshVertDecl, 
		&g_shipVB, &g_shipIB, &g_numShipPolys,&g_numShipVerts, &g_numShipIndices ));

	// Create our matrix VB
	g_shipMatrixVB=IRenderer::GetRendererInstance()->addVertexBuffer(g_numShipInstances*sizeof(MatrixVertex), DYNAMIC);

	// -------------------------------------------------------------------------------------------
	// Load rock model and create VB/IB and instance matrix VB
	( LoadMesh ( "models/Asteroid/aster1.x", (FormatDesc*)g_aCloneMeshVertDecl, 
		&g_rockVB, &g_rockIB, &g_numRockPolys, &g_numRockVerts, &g_numRockIndices ));

	// Create our matrix VB
	g_rockMatrixVB=IRenderer::GetRendererInstance()->addVertexBuffer(g_numRockInstances*sizeof(MatrixVertex), DYNAMIC);

	//---------------------------------------------------------------------------
	// Read the D3DXFROMWINE effect file
	g_pEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/programs/HLSL_Instancing/HLSL_Instancing.fx");
	g_pShipTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/models/SpaceShip/missile1-Color.dds", false, IRenderer::GetRendererInstance()->Getlinear());
	g_pShipBumpTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/models/SpaceShip/missile1-normal.dds", false, IRenderer::GetRendererInstance()->Getlinear());
	g_pRockTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/models/Asteroid/aster1.dds", false, IRenderer::GetRendererInstance()->Getlinear());
	g_pRockBumpTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/models/Asteroid/aster1-normal.dds", false, IRenderer::GetRendererInstance()->Getlinear());

	// Set effect variables as needed
	D3DXFROMWINECOLOR colorMtrlAmbient(0.15f, 0.15f, 0.15f, 0);
	( g_pEffect->SetValue("g_MaterialAmbientColor", &colorMtrlAmbient, sizeof(D3DXFROMWINECOLOR) ) );

	// Setup the camera's view parameters
	D3DXFROMWINEVECTOR3 vecEye(0.0f, 30.0f, 30.0f);
	D3DXFROMWINEVECTOR3 vecAt (0.0f, 20.0f, 0.0f);
	g_Camera.SetViewParams( &vecEye, &vecAt );

	//---------------------------------------------------------------------------
	// initilize our sky box and it's cube map
	TextureID cubeTex=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/cubemaps/biggerStars.dds", false, IRenderer::GetRendererInstance()->Getlinear());
	g_skybox->OnCreateDevice(0, cubeTex, "/programs/HLSL_Instancing/HLSL_Instancing.fx" );

	//---------------------------------------------------------------------------
	D3DXFROMWINEVECTOR3 v = D3DXFROMWINEVECTOR3(300.f,300.f,300.0f);
	stx_srand(INSTANCE_INIT_SRAND);
	g_shipManager->SetWorldExtents(-v,v);
	g_asteroidManager->SetWorldExtents(-v,v);
	g_mothershipManager->SetWorldExtents(-v,v);
	g_shipManager->Init(g_numShipInstances);	
	g_asteroidManager->Init(g_numRockInstances);	
	g_mothershipManager->Init(10);

	g_mothershipManager->Create();

	// Create our scene RT texs and surfaces
	(D3DXFROMWINECreateTexture(
		STX_Service::GetWindowInstance()->GetWidth(),STX_Service::GetWindowInstance()->GetHeight(),
		0,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,
		&g_pRenderTargetTexture));

	g_pRenderTargetTexture->GetSurfaceLevel(0,&g_pRenderTargetSurface);

	(D3DXFROMWINECreateTexture(
		STX_Service::GetWindowInstance()->GetWidth(),STX_Service::GetWindowInstance()->GetHeight(),
		0,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,
		&g_pDownsampleTargetTexture));

	g_pDownsampleTargetTexture->GetSurfaceLevel(0,&g_pDownsampleTargetSurface);

	(D3DXFROMWINECreateTexture(
		STX_Service::GetWindowInstance()->GetWidth(),STX_Service::GetWindowInstance()->GetHeight(),
		0,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,
		&g_pBlurHTargetTexture));

	g_pBlurHTargetTexture->GetSurfaceLevel(0,&g_pBlurHTargetSurface);

	(D3DXFROMWINECreateTexture(
		STX_Service::GetWindowInstance()->GetWidth(),STX_Service::GetWindowInstance()->GetHeight(),
		0,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,
		&g_pBlurVTargetTexture));

	g_pBlurVTargetTexture->GetSurfaceLevel(0,&g_pBlurVTargetSurface);

	// Need to pull out and use the created RT sizes, since create texture might bump them
	//		 up to pow2 textures without telling us.
	D3DSURFACE_DESC desc;
	g_pRenderTargetSurface->GetDesc(&desc);
	(IRenderer::GetRendererInstance()->CreateDepthStencilSurface(desc.Width,desc.Height,		
		D3DFMT_D24X8,D3DMULTISAMPLE_NONE,0,true,&g_pSharedDepth,0));
#if 0
	// Setup full screen rendering
	g_fullScreenQuad->RestoreDeviceObjects();
	IRenderer::GetRendererInstance()->GetRenderTarget(0,&g_pBBSurface);
	IRenderer::GetRendererInstance()->GetDepthStencilSurface(&g_pBBDepthSurface);
	g_fullScreenQuad->SetUpForRenderTargetSurface(g_pBBSurface);
#endif

    return 0;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void OnFrameMove( double fTime, float fElapsedTime )
{
	int hr;
    // Update the camera's position based on user input 
    g_Camera.FrameMove( fElapsedTime );

	// number of instances have been changes, so update our matrix vb to be the right size
	if(g_bReAllocateMatrixVB)
	{
		////////////////SAFE_RELEASE(g_shipMatrixVB);
		(IRenderer::GetRendererInstance()->CreateVertexBuffer(g_numShipInstances*sizeof(MatrixVertex),
			D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0,D3DPOOL_DEFAULT, &g_shipMatrixVB, 0));

		if(g_shipInstanceMatrices) delete[] g_shipInstanceMatrices;
		g_shipInstanceMatrices = new D3DXFROMWINEMATRIX[g_numShipInstances];

		if(g_shipInstanceColors) delete[] g_shipInstanceColors;
		g_shipInstanceColors = new D3DXFROMWINECOLOR[g_numShipInstances];

		////////////////SAFE_RELEASE(g_rockMatrixVB);
		(IRenderer::GetRendererInstance()->CreateVertexBuffer(g_numRockInstances*sizeof(MatrixVertex),
			D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0,D3DPOOL_DEFAULT, &g_rockMatrixVB, 0));

		if(g_rockInstanceMatrices) delete[] g_rockInstanceMatrices;
		g_rockInstanceMatrices = new D3DXFROMWINEMATRIX[g_numRockInstances];

		// reinit our managers to contain the right amount of simulated objects
		g_shipManager->Init(g_numShipInstances);
		g_asteroidManager->Init(g_numRockInstances);

		g_bReAllocateMatrixVB = false;
	}

	// update our little ships, rocks and motherships
	g_asteroidManager->Update(fElapsedTime);
	g_shipManager->Update(fElapsedTime,g_asteroidManager,g_mothershipManager);
	g_mothershipManager->Update(fElapsedTime);

	// Update our per instance matrices
	for(int i=0;i<g_numShipInstances;i++)
	{	
		D3DFROMWINEMATRIX matrix = g_shipManager->MakeMatrixFor(i);
		g_shipInstanceMatrices[i] = matrix;
		g_shipInstanceColors[i] = g_shipManager->GetColorFor(i);
	}
	for(int i=0;i<g_numRockInstances;i++)
	{	
		D3DFROMWINEMATRIX matrix = g_asteroidManager->MakeMatrixFor(i);
		g_rockInstanceMatrices[i] = matrix;
	}

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
    D3DXFROMWINEMATRIXA16 mViewProjection;
    D3DXFROMWINEVECTOR3 vLightDir;
    unsigned int iPass, cPasses;
    D3DXFROMWINEMATRIXA16 mView;
    D3DXFROMWINEMATRIXA16 mProj;
	D3DXFROMWINEMATRIXA16 mViewInv;
	D3DXFROMWINEMATRIXA16 mIdentity;

	D3DXFROMWINEMatrixIdentity(&mIdentity);

	// reset our draw count
	g_numDrawCalls = 0;
    // Clear the back buffer
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (0.0f,0.25f,0.25f,0.55f));

	// Get the projection & view matrix from the camera class
	mProj = *g_Camera.GetProjMatrix();
	mView = *g_Camera.GetViewMatrix();
	D3DXFROMWINEMatrixInverse(&mViewInv,0,&mView);
	mViewProjection = mView * mProj;

	// Calc light 
	
	D3DXFROMWINEVec3TransformNormal(&vLightDir,&D3DXFROMWINEVECTOR3( -0.81675f,0.1867f,0.5456f ),&mViewProjection);
	D3DXFROMWINEVec3Normalize(&vLightDir,&vLightDir);

	( g_pEffect->SetValue( "g_LightDir", &vLightDir, sizeof(D3DXFROMWINEVECTOR3) ) );
	( g_pEffect->SetValue( "g_LightColor", &D3DXFROMWINECOLOR(1.0f,1.0f,1.0f,1.0f), sizeof(D3DXFROMWINECOLOR) ) );
	( g_pEffect->SetMatrix( "g_mViewProj", &mViewProjection ) );
	( g_pEffect->SetMatrix( "g_mViewInverse", &mViewInv ) );

    // Render the scene
    IRenderer::GetRendererInstance()->BeginScene();
    {
		// Draw a simple skybox
		D3DXFROMWINEMATRIX I;
		D3DXFROMWINEMatrixIdentity(&I);		
		g_skybox->render(&I);//(mView);
		g_numDrawCalls++;

		if(g_bUseInstancing)
		{
			// Update the instance streams with the per instance data.
			MatrixVertex* pVertices=new MatrixVertex[g_numShipInstances];
			for(int i=0;i<g_numShipInstances;i++)
			{	
				D3DFROMWINEMATRIX matrix = g_shipInstanceMatrices[i];
				// since no projection, encode the 4x3 part in a 3x4(shader will decode)
				pVertices[i].r1 = D3DXFROMWINEVECTOR4(matrix._11,matrix._21,matrix._31,matrix._41);
				pVertices[i].r2 = D3DXFROMWINEVECTOR4(matrix._12,matrix._22,matrix._32,matrix._42);
				pVertices[i].r3 = D3DXFROMWINEVECTOR4(matrix._13,matrix._23,matrix._33,matrix._43);
				pVertices[i].c1 = g_shipInstanceColors[i];
			}
			g_shipMatrixVB=IRenderer::GetRendererInstance()->addVertexBuffer(g_numShipInstances*sizeof(MatrixVertex), STATIC, pVertices, g_shipMatrixVB);

			for(int i=0;i<g_numRockInstances;i++)
			{	
				D3DFROMWINEMATRIX matrix = g_rockInstanceMatrices[i];
				// since no projection, encode the 4x3 part in a 3x4(shader will decode)
				pVertices[i].r1 = D3DXFROMWINEVECTOR4(matrix._11,matrix._21,matrix._31,matrix._41);
				pVertices[i].r2 = D3DXFROMWINEVECTOR4(matrix._12,matrix._22,matrix._32,matrix._42);
				pVertices[i].r3 = D3DXFROMWINEVECTOR4(matrix._13,matrix._23,matrix._33,matrix._43);
				pVertices[i].c1 = vWhite;
			}
			g_rockMatrixVB=IRenderer::GetRendererInstance()->addVertexBuffer(g_numRockInstances*sizeof(MatrixVertex), STATIC, pVertices, g_rockMatrixVB);
			delete[] MatrixVertex;

			(IRenderer::GetRendererInstance()->setVertexFormat(g_pMeshVertexDecl));

			// Set the first stream to be the indexed data and render N instances
			//(IRenderer::GetRendererInstance()->SetStreamSourceFreq(0,(D3DSTREAMSOURCE_INDEXEDDATA | g_numShipInstances)));

			// Set the scond stream to be per instance data and iterate once per instance
			//(IRenderer::GetRendererInstance()->SetStreamSourceFreq(1,(D3DSTREAMSOURCE_INSTANCEDATA | 1)));

			// -------------------------------------------------------------------------------
			// SPACESHIPS
			( g_pEffect->SetTechnique( "RenderSceneInstance" ) );
			(IRenderer::GetRendererInstance()->setVertexBuffer(0, g_shipVB));//, 0, D3DXFROMWINEGetDeclVertexSize( g_aMeshVertDecl, 0 )));
			(IRenderer::GetRendererInstance()->setIndexBuffer(g_shipIB));
			(IRenderer::GetRendererInstance()->setVertexBuffer(1, g_shipMatrixVB));//, 0, D3DXFROMWINEGetDeclVertexSize( g_aMeshVertDecl, 1 )));

			// Configure the lighting constants
			( g_pEffect->SetFloat("SpecExpon",120.0f));
			( g_pEffect->SetFloat("Ks",0.3f));
			( g_pEffect->SetFloat("Bumpy",1.5f));
			( g_pEffect->SetFloat("Kd",1.0f));
			( g_pEffect->SetTexture( "g_MeshTexture", g_pShipTexture) );
			( g_pEffect->SetTexture( "g_MeshBumpTexture", g_pShipBumpTexture) );

			// Draw with our effect 
			( g_pEffect->Begin(&cPasses, 0) );
			for (iPass = 0; iPass < cPasses; iPass++)
			{
				( g_pEffect->BeginPass(iPass) );
				g_numDrawCalls++;
				(IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES,0,0,g_numShipVerts,0,g_numShipIndices/3));
				( g_pEffect->EndPass() );
			}
			( g_pEffect->End() );

			// -------------------------------------------------------------------------------
			// ROCKS
			( g_pEffect->SetTechnique( "RenderRockInstance" ) );
			//(IRenderer::GetRendererInstance()->SetStreamSourceFreq(0,(D3DSTREAMSOURCE_INDEXEDDATA | g_numRockInstances)));
			//(IRenderer::GetRendererInstance()->SetStreamSourceFreq(1,(D3DSTREAMSOURCE_INSTANCEDATA | 1)));
			(IRenderer::GetRendererInstance()->setIndexBuffer(g_rockIB));
			IRenderer::GetRendererInstance()->setVertexBuffer(0, g_rockVB);//, 0, D3DXFROMWINEGetDeclVertexSize( g_aMeshVertDecl, 0 )));
			IRenderer::GetRendererInstance()->setVertexBuffer(1, g_rockMatrixVB);//, 0, D3DXFROMWINEGetDeclVertexSize( g_aMeshVertDecl, 1 )));

			// Configure the lighting constants
			( g_pEffect->SetFloat("SpecExpon",12.0f));
			( g_pEffect->SetFloat("Ks",0.2f));
			( g_pEffect->SetFloat("Bumpy",0.9f));
			( g_pEffect->SetFloat("Kd",1.0f));
			( g_pEffect->SetTexture( "g_MeshTexture", g_pRockTexture) );
			( g_pEffect->SetTexture( "g_MeshBumpTexture", g_pRockBumpTexture) );

			// Draw with our effect 
			( g_pEffect->Begin(&cPasses, 0) );
			for (iPass = 0; iPass < cPasses; iPass++)
			{
				( g_pEffect->BeginPass(iPass) );
				g_numDrawCalls++;
				(IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES,0,0,g_numRockVerts,0,g_numRockIndices/3));
				( g_pEffect->EndPass() );
			}
			( g_pEffect->End() );

			// -------------------------------------------------------------------------------
			// Reset the freq for streams or D3D will complain
			//(IRenderer::GetRendererInstance()->SetStreamSourceFreq(0,1));
			//(IRenderer::GetRendererInstance()->SetStreamSourceFreq(1,1));
		}
		else
		{
			( g_pEffect->SetTechnique( "RenderSceneNormal" ) );
			(IRenderer::GetRendererInstance()->setVertexFormat(g_pCloneMeshVertexDecl));
			(IRenderer::GetRendererInstance()->setIndexBuffer(g_shipIB));
			IRenderer::GetRendererInstance()->setVertexBuffer(0, g_shipVB);//, 0, D3DXFROMWINEGetDeclVertexSize( g_aMeshVertDecl, 0 )));

			for(int i=0;i<g_numShipInstances;i++)
			{
				D3DXFROMWINEMATRIXA16 matrix = g_shipInstanceMatrices[i];
				D3DXFROMWINEVECTOR4 diffuseC(g_shipInstanceColors[i].r,g_shipInstanceColors[i].g,g_shipInstanceColors[i].b,1);

				( g_pEffect->SetMatrix( "g_mWorld", &(matrix) ));
				( g_pEffect->SetFloat("SpecExpon",120.0f));
				( g_pEffect->SetFloat("Ks",0.3f));
				( g_pEffect->SetFloat("Bumpy",1.5f));
				( g_pEffect->SetFloat("Kd",1.0f));
				g_pEffect->SetVector("g_MaterialDiffuseColor",&diffuseC);

				( g_pEffect->SetTexture( "g_MeshTexture", g_pShipTexture) );
				( g_pEffect->SetTexture( "g_MeshBumpTexture", g_pShipBumpTexture) );

				//( g_pEffect->CommitChanges() );
				( g_pEffect->Begin(&cPasses, 0) );
				for (iPass = 0; iPass < cPasses; iPass++)
				{
					( g_pEffect->BeginPass(iPass) );
					g_numDrawCalls++;
					(IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES,0,0,g_numShipVerts,0,g_numShipIndices/3));
					( g_pEffect->EndPass() );
				}
				( g_pEffect->End() );
			}

			( g_pEffect->SetTechnique( "RenderRockNormal" ) );
			(IRenderer::GetRendererInstance()->setIndexBuffer(g_rockIB));
			IRenderer::GetRendererInstance()->setVertexBuffer(0, g_rockVB);//, 0, D3DXFROMWINEGetDeclVertexSize( g_aMeshVertDecl, 0 )));

			for(int i=0;i<g_numRockInstances;i++)
			{
				D3DXFROMWINEMATRIXA16 matrix = g_rockInstanceMatrices[i];

				( g_pEffect->SetMatrix( "g_mWorld", &(matrix) ));
				( g_pEffect->SetValue("g_MaterialDiffuseColor", &vWhite, sizeof(D3DXFROMWINECOLOR) ) );
				( g_pEffect->SetFloat("SpecExpon",12.0f));
				( g_pEffect->SetFloat("Ks",0.2f));
				( g_pEffect->SetFloat("Bumpy",0.9f));
				( g_pEffect->SetFloat("Kd",1.0f));

				( g_pEffect->SetTexture( "g_MeshTexture", g_pRockTexture) );
				( g_pEffect->SetTexture( "g_MeshBumpTexture", g_pRockBumpTexture) );

				//( g_pEffect->CommitChanges() );
				( g_pEffect->Begin(&cPasses, 0) );
				for (iPass = 0; iPass < cPasses; iPass++)
				{
					( g_pEffect->BeginPass(iPass) );
					g_numDrawCalls++;
					(IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES,0,0,g_numRockVerts,0,g_numRockIndices/3));
					( g_pEffect->EndPass() );
				}
				( g_pEffect->End() );
			}
		}
       
        ( IRenderer::GetRendererInstance()->EndScene() );
    }

	// some mother ships for garnish
	g_numDrawCalls += g_mothershipManager->Render(g_pEffect);
}

void UpdateMatrixVB()
{
	g_numShipInstances = max(1,g_numShipInstances);
	g_bReAllocateMatrixVB = true;
}

