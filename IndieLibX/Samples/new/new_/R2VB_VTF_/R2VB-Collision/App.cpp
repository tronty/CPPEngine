//-----------------------------------------------------------------------------
// File: Samples\DirectX 9.0\R2VB-Collision\App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------


//---------------------------------------------------------------------------
/*
	Sample showing render to vertex buffer used for ray picking and collision
	detection.

	Functions on of interest:
	    - load()
	    - drawFrame()
	    - InitializeParticlesTextures()
*/
//---------------------------------------------------------------------------

#include "../../Framework/Direct3D/D3D9App.h"
#include "../../Framework/Direct3D/atir2vb.h"

// Macros
#define RELEASE(x)		if (x) {(x)->Release(); (x) = NULL; }
#define FRAND(x)		(((float)rand()/RAND_MAX) * (x))
#define RAND(x)			((int)floor(FRAND(x)+0.5f))
#define MAX(x,y)		( (x) > (y) ? (x) : (y) )

// Defines
#define PARTICLES_TEXTURE_SIZE  128
#define NUMBER_OF_PARTICLES     (PARTICLES_TEXTURE_SIZE*PARTICLES_TEXTURE_SIZE)
#define FRONTCLIP               1.0f
#define FARCLIP                 10000.0f

// Constants
const float fTotalTerrainWidth = 1000.f;
const float fTotalTerrainLength = 1000.f;
const float fTotalTerrainHeight = 100.f;	// This should ideally be determined from the height map's maximum height


//---------------------------------------------------------------------------
// This is our main sample application class.
//---------------------------------------------------------------------------
class SampleApp : public D3D9App
{
public:
	// A basic model structure to contain all the info necessary to draw a model
	struct Model
	{
		VertexFormatID vertexFormat;
		VertexBufferID vertexBuffer;
		uint32         vertexCount;
		IndexBufferID  indexBuffer;
		uint32         indexCount;
	};

	// Default constructor.
	SampleApp()
		: OVERLAY_SIZE(256)
		, nFrame(0)
		, m_pRenderTargetTexture(NULL)
		, bReinitializeParticles(false)
		, bStepTime(false)
		, bIncreaseTerrain(false)
		, bDecreaseTerrain(false)
	{
		// void
	}

	// Default destructor.
	~SampleApp() {}

	//---------------------------------------------------------------------------
	// Overriden from framework
	//---------------------------------------------------------------------------
	char *getTitleString() { return "R2VB-Collision"; }
	char *getHomeDirectory() { return "../DirectX 9.0/R2VB-Collision"; }

	void setup(char *args[], int nArguments) {
		width  = 1024;
		height = 768;
	}

	bool load();
	bool unload();

	void updateTime(const float fTime);
	void drawFrame();
	void initGUI();
	

protected:
	// Utility function to update all the necessary transforms
	void updateTransforms();
	void resetCamera();
	bool onReset();
	bool onPreReset();
	void onSize(const int w, const int h);
	bool onMouseMove(const int x, const int y, const bool lButton, const bool mButton, const bool rButton);
	bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed);
	void onMenuItemPress( Menu *menu, MenuItemID item );
	bool onKeyPress(const unsigned int key, const bool pressed);

	void InitializeParticlesTextures(LPDIRECT3DDEVICE9 pd3dDevice);
	bool DrawScreenAlignedQuad(LPDIRECT3DDEVICE9 pd3dDevice, float fWidth, float fHeight, float fZ);
	bool DrawQuad(LPDIRECT3DDEVICE9 pd3dDevice, float fX, float fY, float fZ, float fWidth, float fHeight);

	void DrawSkybox();

private:
	const uint32 OVERLAY_SIZE;                  // quad overlay size (for displaying the terrain texture contents)

	// Terrain map dimensions
	uint32 dwTerrainMapWidth;                   // Terrain map dimensions
	uint32 dwTerrainMapLength;

	// Data required for mouse picking
	D3DVIEWPORT9	MainViewport;
	D3DXVECTOR4		MouseRayCast[2];

	// Frame variables
	float			fTimeElapsed;
	int				nFrame;

	// Bools
	bool			bReinitializeParticles;
	bool			bStepTime;
	bool			bStep;
	bool			bIncreaseTerrain, bDecreaseTerrain;

	// Terrain texture and surface
	IDirect3DTexture9* 				m_pRenderTargetTexture;
	IDirect3DSurface9* 				m_pRenderTargetSurface;
	IDirect3DSurface9* 				m_pDummyDepthSurface;

	// Particle textures and surfaces (2 each for ping-ponging)
	IDirect3DTexture9*				m_pParticlesPosition[2];
	IDirect3DTexture9*				m_pParticlesVelocity[2];
	IDirect3DSurface9*				m_pParticlesPositionSurface[2];
	IDirect3DSurface9*				m_pParticlesVelocitySurface[2];

	// Ray intersection texture and surface
	IDirect3DTexture9*				m_pRayIntersectionTexture;
	IDirect3DSurface9*				m_pRayIntersectionSurface;

	// Vertex and index data: note that the terrain does not use a vertex buffer (since the VB in this sample is a texture)
	IDirect3DIndexBuffer9*			m_pTerrainIndexBuffer;
	IDirect3DVertexDeclaration9*	m_pPositionVertexDeclaration;
	IDirect3DVertexDeclaration9*	m_pPositionTVertexDeclaration;
	IDirect3DVertexDeclaration9*	m_pPositionTTexcoordVertexDeclaration;
	IDirect3DVertexBuffer9*			m_pDummyVB;
	VertexFormatID					skyboxVF;

	// Shaders
	ShaderID						m_sceneShader;
	ShaderID						m_scenewireframeShader;
	ShaderID						m_overlayShader;
	ShaderID						m_particlesmathShader;
	ShaderID						m_particlesrenderShader;
	ShaderID						m_rayintersectionShader;
	ShaderID						m_skyboxShader; 

	// Textures
	TextureID						TerrainTexture, RenderTargetTexture;
	TextureID						TerrainDiffuseTexture;
	TextureID						HillSprite, SnowFlakeTexture;
	TextureID						NoiseTexture, SkyboxTexture;
	
	// GUI stuff
	Slider							*BounceFactor, *SnowFlakeLife;
	Menu							*DemoMenu;
	MenuItemID						Reset, Display2DView, ToggleWireframe;
	Label							*Label_IncreaseElevation, *Label_DecreaseElevation, *Label_BounceFactor, *Label_SnowFlakeLife;

	// Matrices
	D3DXMATRIX						matView, matProj;
	mat4							m_modelMvp;
};

//---------------------------------------------------------------------------
// Create an instance of the application.
//---------------------------------------------------------------------------
BaseApp *app = new SampleApp();


//---------------------------------------------------------------------------
// Handle resizing messages
//
//---------------------------------------------------------------------------
void SampleApp::onSize(const int w, const int h)
{
	Label_IncreaseElevation->setPosition(5.0f, h - 48.0f);
	Label_DecreaseElevation->setPosition(5.0f, h - 23.0f);

	Label_BounceFactor->setPosition(w - 124.0f, h - 58.0f);
	BounceFactor->setPosition(w - 124.0f, h - 38.0f);

	Label_SnowFlakeLife->setPosition(w - 274.0f, h - 58.0f);
	SnowFlakeLife->setPosition(w - 274.0f, h - 38.0f);

	D3D9App::onSize(w, h);
}

//---------------------------------------------------------------------------
// Handle key presses
//
//---------------------------------------------------------------------------
bool SampleApp::onKeyPress(const unsigned int key, const bool pressed)
{
	// 'R': Re-initialize all particles
	if (pressed && key==KEY_R) {
		if (nFrame>0) bReinitializeParticles = true;
	}

	// 'W': Toggle wireframe mode
	if (pressed && key==KEY_W) {
		DemoMenu->toggleItemCheck(ToggleWireframe);
	}

	// 'S': Toggle step mode (pause)
	if (pressed && key==KEY_S) {
		bStepTime = !bStepTime;
	}

	// F: steps forward one time unit
	if (pressed && key==KEY_F) {
		bStep = TRUE;
	}

	// F: steps forward one time unit
	if (pressed && key==KEY_V) {
		DemoMenu->toggleItemCheck(Display2DView);
	}

	// Call normal key handler
	return BaseApp::onKeyPress(key, pressed);
}


//---------------------------------------------------------------------------
// Reset camera
//
//---------------------------------------------------------------------------
void SampleApp::resetCamera(){
	// Define initial camera view
	camPos = float3(0, 150, -600);
	wx = PI/8.0f;
	wy = 0;
}

//---------------------------------------------------------------------------
// App load.
//
//---------------------------------------------------------------------------
bool SampleApp::load()
{
	HRESULT hres;
	
	// Check that R2VB is supported
	if (d3d->CheckDeviceFormat(m_deviceSettings.adapterOrdinal, m_deviceSettings.deviceType, m_deviceSettings.adapterFormat,
								0, D3DRTYPE_SURFACE, (D3DFORMAT)R2VB_FOURCC_R2VB)!=D3D_OK) {
		MessageBox(getWindow(), "This demo requires R2VB support.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Check that Shader Model 3 is supported
	if (caps.PixelShaderVersion<D3DPS_VERSION(3, 0) || caps.VertexShaderVersion<D3DVS_VERSION(3, 0)) {
		MessageBox(getWindow(), "This demo requires Shader Model 3.0 support.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	//
	// Load textures
	//

	// Terrain texture: contains reference height data for terrain (D3DFMT_A32B32G32R32F format)
	// Use point sampling since this data contains vertex buffer data (i.e. we don't want it filtered)
	// Note that we won't render from this reference texture directly: a D3DFMT_A16B16G16R16F render target will be used instead
	if ((TerrainTexture = renderer->addTexture(TEX_PATH "land_DIS_512.hdr", FILTER_POINT)) == TEXTURE_NONE) return false;


	// Terrain diffuse and illumination textures
	if ((TerrainDiffuseTexture = renderer->addTexture(TEX_PATH "land_DL.dds", FILTER_BILINEAR)) == TEXTURE_NONE) return false;

	// Load snow texture
	if ((SnowFlakeTexture = renderer->addTexture(TEX_PATH "Particle.dds", FILTER_BILINEAR)) == TEXTURE_NONE) return false;

	// Texture used to create bump or craters in terrain data
	Image img;
	ubyte *dest = img.create(FORMAT_I8, 512, 512, 1, 1);
	for (int y = 0; y < 512; y++){
		float fy = (y / 511.0f) * 2 - 1;
		for (int x = 0; x < 512; x++){
			float fx = (x / 511.0f) * 2 - 1;
			*dest++ = (ubyte) (255 * max(1.0f - fx * fx - fy * fy, 0.0f));
		}
	}
	if ((HillSprite = renderer->addTexture(img, FILTER_LINEAR)) == TEXTURE_NONE) return false;

	// Noise texture
	if ((NoiseTexture = renderer->addTexture(TEX_PATH "noise2D.tga", FILTER_BILINEAR)) == TEXTURE_NONE) return false;

	// Background cube map
	if ((SkyboxTexture = renderer->addCubemap(
		TEX_PATH "Olympus/posx.jpg", TEX_PATH "Olympus/negx.jpg",
		TEX_PATH "Olympus/posy.jpg", TEX_PATH "Olympus/negy.jpg",
		TEX_PATH "Olympus/posz.jpg", TEX_PATH "Olympus/negz.jpg", FILTER_TRILINEAR)) == TEXTURE_NONE) return false;

	// Save terrain map dimensions
	dwTerrainMapWidth = renderer->getTextureWidth(TerrainTexture);
	dwTerrainMapLength = renderer->getTextureHeight(TerrainTexture);

	//
	// Reference terrain texture contains height data only (Y), need to add X, Z and W data as well
	//

	// Retrieve surface attached to terrain texture
	IDirect3DBaseTexture9 *m_pTerrainTexture = ((D3D9Renderer*)renderer)->getD3DTexture(TerrainTexture);
	IDirect3DSurface9 *m_pTerrainSurface;
	((IDirect3DTexture9 *)m_pTerrainTexture)->GetSurfaceLevel( 0, &m_pTerrainSurface);

	// Lock surface
	D3DLOCKED_RECT LockedRect;
	if (m_pTerrainSurface->LockRect(&LockedRect, NULL, 0)!=D3D_OK) {
		ERRORMSG("Failed to lock surface");
		return false;
	}

	// Fill data with XZW data, leaving Y (height) untouched
	for (uint32 i=0; i<dwTerrainMapLength; i++) {
		float *pfGrid = (float *)((char *)LockedRect.pBits + i*LockedRect.Pitch);
		for (uint32 j=0; j<dwTerrainMapWidth; j++) {

			// X
			*pfGrid++ = ((float)j/(dwTerrainMapWidth-1)) * fTotalTerrainWidth - fTotalTerrainWidth/2.0f;

			// Y (untouched)
			pfGrid++;

			// Z
			*pfGrid++ = ((float)i/(dwTerrainMapLength-1)) * fTotalTerrainLength - fTotalTerrainLength/2.0f;

			// W
			*pfGrid++ = 1.0f;
		}
	}

	// Unlock surface
	m_pTerrainSurface->UnlockRect();
	RELEASE(m_pTerrainSurface);

	// Call onReset to create application-managed surfaces
	if (!onReset()) return false;


	//
	// Create all vertex declarations we'll need
	//

	// Position data in float16 format
	const D3DVERTEXELEMENT9 position16fVertexAttribs[] = {
		{ 0,  0, D3DDECLTYPE_FLOAT16_4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END()
	};

	// Pre-transformed position data in float16 format
	const D3DVERTEXELEMENT9 positiont16fVertexAttribs[] = {
		{ 0,  0, D3DDECLTYPE_FLOAT16_4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
		D3DDECL_END()
	};

	// Pre-transformed position data and 1 pair of texture coordinates
	const D3DVERTEXELEMENT9 positiontTexcoordVertexAttribs[] = {
		{ 0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
		{ 0,  16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	// Create vertex declarations
	hres =	dev->CreateVertexDeclaration(position16fVertexAttribs, &m_pPositionVertexDeclaration);
	hres |= dev->CreateVertexDeclaration(positiont16fVertexAttribs, &m_pPositionTVertexDeclaration);
	hres |= dev->CreateVertexDeclaration(positiontTexcoordVertexAttribs, &m_pPositionTTexcoordVertexDeclaration);
	if (hres!=D3D_OK) {
		ERRORMSG("Couldn't create terrain vertex declarations");
		return false;
	}

	//
	// Create index buffer for terrain
	//
	if (dev->CreateIndexBuffer((dwTerrainMapWidth-1)*(dwTerrainMapLength-1)*2*3*sizeof(DWORD), 
							0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pTerrainIndexBuffer, NULL)!=D3D_OK) {
		ERRORMSG("Failed to create index buffer");
		return false;
	}

	// Lock index buffer
	DWORD *pIndex;
	m_pTerrainIndexBuffer->Lock(0, 0, (void **)&pIndex, 0);

	// Fill index buffer
	for (DWORD i=0; i<(dwTerrainMapLength-1); i++) {
		for (DWORD j=0; j<(dwTerrainMapWidth-1); j++) {
			// 1st triangle
			*pIndex++ = i*dwTerrainMapLength + j;
			*pIndex++ = (i+1)*dwTerrainMapLength + j;
			*pIndex++ = i*dwTerrainMapLength + j + 1;
			// 2nd triangle
			*pIndex++ = i*dwTerrainMapLength + j + 1;
			*pIndex++ = (i+1)*dwTerrainMapLength + j;
			*pIndex++ = (i+1)*dwTerrainMapLength + j + 1;
		}
	}

	// Unlock index buffer
	m_pTerrainIndexBuffer->Unlock();

	// Create dummy vertex buffer (needed for R2VB API overload)
	// The dummy vertex buffer needs to be as large as either the terrain or particle map size (since it's used for both)
	if (dev->CreateVertexBuffer(MAX(dwTerrainMapWidth, PARTICLES_TEXTURE_SIZE)*MAX(dwTerrainMapLength, PARTICLES_TEXTURE_SIZE)*4*sizeof(unsigned short), 0, 0, D3DPOOL_MANAGED, &m_pDummyVB, NULL)!=D3D_OK) {
		ERRORMSG("Failed to create dummy vertex buffer");
		return false;
	}

	// Create skybox vertex format
	VertexAttribute skyboxAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 3 };
	if ((skyboxVF = renderer->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs))) == VF_NONE) return false;

	//
	// Load shaders
	//
	if ((m_sceneShader				= renderer->addShader( "Scene.shd"            ))		== SHADER_NONE) return false;
	if ((m_scenewireframeShader		= renderer->addShader( "Scene_wireframe.shd"  ))		== SHADER_NONE) return false;
	if ((m_overlayShader			= renderer->addShader( "Overlay.shd"          ))		== SHADER_NONE) return false;
	if ((m_particlesmathShader		= renderer->addShader( "ParticlesMath.shd"    ))		== SHADER_NONE) return false;
	if ((m_particlesrenderShader	= renderer->addShader( "ParticlesRender.shd"  ))		== SHADER_NONE) return false;
	if ((m_rayintersectionShader	= renderer->addShader( "RayIntersection.shd"  ))		== SHADER_NONE) return false;
	if ((m_skyboxShader				= renderer->addShader( "skybox.shd"           ))		== SHADER_NONE) return false;

	return true;
}


//---------------------------------------------------------------------------
// App reset.
//
//---------------------------------------------------------------------------
bool SampleApp::onReset()
{
	HRESULT hres;
	
	// Put all application-managed resources here (e.g. D3DPOOL_DEFAULT resources)

	// Create terrain render target texture that we will render terrain geometry from
	if (dev->CreateTexture(dwTerrainMapWidth, dwTerrainMapLength, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &m_pRenderTargetTexture, NULL) != D3D_OK){
		ERRORMSG("Couldn't create terrain render target texture");
		return false;
	}

	// Create ID for this render target
	RenderTargetTexture = ((D3D9Renderer *)renderer)->addRenderTarget(m_pRenderTargetTexture, FILTER_POINT);

	// Retrieve surface attached to render target texture
	m_pRenderTargetTexture->GetSurfaceLevel( 0, &m_pRenderTargetSurface);

	// Create particles render target textures (position and velocity, double-buffered for ping-ponging)
	// that we will render particles geometry from
	hres =  dev->CreateTexture(PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &m_pParticlesPosition[0], NULL);
	hres |= dev->CreateTexture(PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &m_pParticlesPosition[1], NULL);
	hres |= dev->CreateTexture(PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &m_pParticlesVelocity[0], NULL);
	hres |= dev->CreateTexture(PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &m_pParticlesVelocity[1], NULL);
	if (hres!=D3D_OK) {
		ERRORMSG("Couldn't create particles render target textures");
		return false;
	}

	// Retrieve surfaces associated with textures
	m_pParticlesPosition[0]->GetSurfaceLevel(0, &m_pParticlesPositionSurface[0]);
	m_pParticlesPosition[1]->GetSurfaceLevel(0, &m_pParticlesPositionSurface[1]);
	m_pParticlesVelocity[0]->GetSurfaceLevel(0, &m_pParticlesVelocitySurface[0]);
	m_pParticlesVelocity[1]->GetSurfaceLevel(0, &m_pParticlesVelocitySurface[1]);

	// Create ray intersection render target (1x1) that is used to calculate mouse ray intersection with terrain
	if (dev->CreateTexture(1, 1, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &m_pRayIntersectionTexture, NULL) != D3D_OK){
		ERRORMSG("Couldn't create ray intersection render target texture");
		return false;
	}
	m_pRayIntersectionTexture->GetSurfaceLevel(0, &m_pRayIntersectionSurface);
	

	// Create dummy depth buffer
	// The dummy depth buffer needs to be as large as either the terrain or particle map size (since it's used for both)
	if (dev->CreateDepthStencilSurface(MAX(dwTerrainMapWidth, PARTICLES_TEXTURE_SIZE), MAX(dwTerrainMapLength, PARTICLES_TEXTURE_SIZE), D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pDummyDepthSurface, NULL)!=D3D_OK) {
		ERRORMSG("Failed to create dummy depth buffer");
		return false;
	}

	// Reset frame number
	nFrame = 0;
	
	// Create projection matrix (constant for the whole demo)
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 3, (float)width/height, FRONTCLIP, FARCLIP);

	return true;
}


//---------------------------------------------------------------------------
// App pre reset.
//
//---------------------------------------------------------------------------
bool SampleApp::onPreReset()
{
	// Destroy all application-managed resources here (e.g. D3DPOOL_DEFAULT resources)
	RELEASE(m_pDummyDepthSurface);
	RELEASE(m_pRayIntersectionSurface);
	RELEASE(m_pRayIntersectionTexture);
	RELEASE(m_pParticlesVelocitySurface[1]);
	RELEASE(m_pParticlesVelocitySurface[0]);
	RELEASE(m_pParticlesPositionSurface[1]);
	RELEASE(m_pParticlesPositionSurface[0]);
	RELEASE(m_pParticlesVelocity[1]);
	RELEASE(m_pParticlesVelocity[0]);
	RELEASE(m_pParticlesPosition[1]);
	RELEASE(m_pParticlesPosition[0]);
	RELEASE(m_pRenderTargetSurface);

	return true;
}

//---------------------------------------------------------------------------
// App unload.
//
//---------------------------------------------------------------------------
bool SampleApp::unload()
{
	// Release memory allocated in Reset()
	onPreReset();

	// Since we are managing these Direct3D reference pointers ourselves,
	// we must be careful to release them when the app quits.
	RELEASE(m_pTerrainIndexBuffer);
	RELEASE(m_pPositionTTexcoordVertexDeclaration);
	RELEASE(m_pPositionTVertexDeclaration);
	RELEASE(m_pPositionVertexDeclaration);
	RELEASE(m_pDummyVB);

    return true;
}


//---------------------------------------------------------------------------
// App update time ('tick').
//
//---------------------------------------------------------------------------
void SampleApp::updateTime( const float fTime )
{
	BaseApp::updateTime( fTime );
	updateTransforms();
}


//---------------------------------------------------------------------------
// App draw frame.
//
//---------------------------------------------------------------------------
void SampleApp::drawFrame()
{
	HRESULT			hres;
	D3DVIEWPORT9	RenderTargetViewport;

	// Get a copy of the current viewport
	dev->GetViewport( &MainViewport );

	// Tell the sample framework that we'll be managing the render target changes ourselves
	renderer->changeRenderTarget( RT_USERDEFINED, RT_USERDEFINED );

	// Set up the viewport for terrain rendering
	RenderTargetViewport.X = 0;
	RenderTargetViewport.Y = 0;
	RenderTargetViewport.Width = dwTerrainMapWidth;
	RenderTargetViewport.Height = dwTerrainMapLength;
	RenderTargetViewport.MinZ = 0.0f;
	RenderTargetViewport.MaxZ = 1.0f;

	// Re-initialize terrain contents at first frame
	if (nFrame==0) {

		// Set render target to terrain render target
		dev->SetRenderTarget( 0, m_pRenderTargetSurface );
		dev->SetDepthStencilSurface(m_pDummyDepthSurface);
		dev->SetViewport( &RenderTargetViewport );

		// Turn Z testing off
		dev->SetRenderState(D3DRS_ZENABLE, FALSE);

		// Render terrain onto D3DFMT_A16B16G16R16F render target
		renderer->setTexture((unsigned int)0, TerrainTexture);
		renderer->changeShader(SHADER_NONE);
		renderer->apply();
		dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		dev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		// Draw quad filling the whole render target
		DrawScreenAlignedQuad(dev, (float)dwTerrainMapWidth, (float)dwTerrainMapLength, 0.0f);
	}

	// Reinitialize particles if required, i.e. first frame or key pressed
	if (bReinitializeParticles || nFrame==0) {
		
		InitializeParticlesTextures(dev);
		bReinitializeParticles = false;
		nFrame = 0; // Reset frame number
	}

	// Modify terrain render target based on mouse interaction (i.e. adding hills or craters)
	if (bIncreaseTerrain || bDecreaseTerrain) {

		// First we need to determine the intersection of the mouse ray with the terrain
		// This is done by running a shader on a 1x1 render target; the result of the render is the coordinates of the intersection point

		// Set render target
		dev->SetRenderTarget( 0, m_pRayIntersectionSurface );
		dev->SetDepthStencilSurface(m_pDummyDepthSurface);
		
		// Set up the viewport for our 1x1 render target
		D3DVIEWPORT9 RayIntersectionViewport;
		RayIntersectionViewport.X = 0;
		RayIntersectionViewport.Y = 0;
		RayIntersectionViewport.Width = 1;
		RayIntersectionViewport.Height = 1;
		RayIntersectionViewport.MinZ = 0.0f;
		RayIntersectionViewport.MaxZ = 1.0f;
		dev->SetViewport( &RayIntersectionViewport );

		// Set shader and constants
		renderer->setShader( m_rayintersectionShader );
		renderer->setCullFace( BACK );
		renderer->apply();
		renderer->changeShaderConstant4f( "RayStartingPoint", (const vec4 &)MouseRayCast[0]);
		D3DXVECTOR3 DirectionVector = MouseRayCast[1] - MouseRayCast[0];
		D3DXVec3Normalize(&DirectionVector, &DirectionVector);
		// Scale with unit length (unit corresponding to one pixel gap)
		float fMaxRadius = min(fTotalTerrainWidth/dwTerrainMapWidth, fTotalTerrainLength/dwTerrainMapLength);
		DirectionVector *= fMaxRadius / sqrt(DirectionVector.x*DirectionVector.x + DirectionVector.z*DirectionVector.z);
		renderer->changeShaderConstant3f( "RayDirectionVector", (const vec3 &)DirectionVector);
		renderer->changeShaderConstant2f( "RcpTotalTerrainDimensions", vec2(1.0f/fTotalTerrainWidth, 1.0f/fTotalTerrainLength));
		renderer->changeShaderConstant2f( "HeightMapSize", vec2((float)dwTerrainMapWidth, (float)dwTerrainMapLength));
		renderer->changeShaderConstant2f( "TexelSize", vec2(1.0f/dwTerrainMapWidth, 1.0f/dwTerrainMapLength));
		renderer->changeShaderConstant2f( "TerrainUnitSize", vec2(fTotalTerrainWidth/dwTerrainMapWidth, fTotalTerrainLength/dwTerrainMapLength));

		// Z off
		dev->SetRenderState(D3DRS_ZENABLE, FALSE);

		// Blending off
		dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		// Set terrain as texture input
		dev->SetTexture(0, m_pRenderTargetTexture);

		// Render a single pixel
		if (!DrawQuad(dev, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f)) {	
			OutputDebugString("Failed to render single pixel\n");
		}

		// At this stage the coordinates of the mouse pick intersection with terrain geometry
		// is contained in the 1x1 m_pRayIntersectionTexture texture

		// Set render target to terrain surface
		dev->SetRenderTarget( 0, m_pRenderTargetSurface );
		dev->SetDepthStencilSurface(m_pDummyDepthSurface);

		// Restore viewport
		dev->SetViewport( &RenderTargetViewport );

		// Set vertex declaration
		dev->SetVertexDeclaration(m_pPositionTVertexDeclaration);

		// Use fixed-function vertex processing
		dev->SetVertexShader(NULL);

		// Only write to Green channel (height data in render target) leaving other channels (X, Z and W position) untouched
		dev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_GREEN);

		// Set sprite texture
		dev->SetTexture(0, ((D3D9Renderer*)renderer)->getD3DTexture(HillSprite));

		// Enable alpha blending
		dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// Use fixed-function pixel processing
		dev->SetPixelShader(NULL);

		// Change to additive or subtractive blending mode based on user action
		dev->SetRenderState(D3DRS_BLENDOP, bDecreaseTerrain ? D3DBLENDOP_REVSUBTRACT : D3DBLENDOP_ADD);

		// Enable point sprites
		float fSize = 20.0f * (dwTerrainMapWidth / 256.0f);
		dev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
		dev->SetRenderState(D3DRS_POINTSIZE, *(DWORD *)&fSize);

		// Enable render to vertex buffer extension
		hres = dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));

		// Set stream source to dummy VB (used to specify offset and stride: offset = 0, stride = 4*sizeof(unsigned short))
		hres = dev->SetStreamSource(0, m_pDummyVB, 0, 4*sizeof(unsigned short));

		// Bind the texture to the DMAP sampler
		dev->SetTexture(D3DDMAPSAMPLER, m_pRayIntersectionTexture);

		// Tell the driver that stream 0 is to be fetched from the DMAP texture
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DMAP));

		// Render single particle (this particle will be rendered at the terrain intersection with the mouse ray)
		if (dev->DrawPrimitive(D3DPT_POINTLIST, 0, 1)!=D3D_OK) {
			OutputDebugString("Failed to render single particle on terrain\n");
		}

		// Stream 0 restored to regular vertex buffer mode
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DIS));

		// Unbind the DMAP texture
		dev->SetTexture(D3DDMAPSAMPLER, NULL);

		// Disable render to vertex buffer extension
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));

		// Disable point sprites
		dev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);

		// Restore color writes
		dev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

		// Disable alpha blending
		dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		// Restore Z testing
		dev->SetRenderState(D3DRS_ZENABLE, TRUE);
	}

	// Revert to default render target
	renderer->resetRenderTarget();
	dev->SetViewport( &MainViewport );

	// Clear render target
	dev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFB0B0B0, 1.0f, 0L );

	//
	// Main terrain view
	//

	// Set shader
	renderer->setShader( m_sceneShader );
	renderer->setTexture("TerrainDiffuseTexture", TerrainDiffuseTexture);
	renderer->setCullFace( BACK );
	renderer->setDepthFunc( LEQUAL );
	renderer->apply();
	renderer->changeShaderConstant4x4f( "WorldTransformMatrix", m_modelMvp );

	// Disable alpha blending
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// Override filtering modes set by framework
	dev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	dev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	dev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	dev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	dev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	dev->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	dev->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	dev->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// Enable render to vertex buffer extension
	hres = dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));

	// Set vertex declaration
	hres = dev->SetVertexDeclaration(m_pPositionVertexDeclaration);

	// Set stream source to dummy VB (used to specify offset and stride)
	hres = dev->SetStreamSource(0, m_pDummyVB, 0, 4*sizeof(unsigned short));

	// Set index buffer
	hres = dev->SetIndices(m_pTerrainIndexBuffer);

	// Bind the render target to the DMAP sampler
	dev->SetTexture(D3DDMAPSAMPLER, m_pRenderTargetTexture);

	// Tell the driver that stream 0 is to be fetched from the DMAP texture
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DMAP));

	// Turn Z testing on
	dev->SetRenderState(D3DRS_ZENABLE, TRUE);

	// Draw the terrain
	if (dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (dwTerrainMapWidth)*(dwTerrainMapLength), 0, (dwTerrainMapWidth-1)*(dwTerrainMapLength-1)*2)!=D3D_OK) {
		OutputDebugString("Failed to render terrain");
	}

	// Draw the terrain in wireframe if requested
	if (DemoMenu->isItemChecked(ToggleWireframe)) {
		dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		renderer->setShader( m_scenewireframeShader );
		renderer->setDepthFunc( LEQUAL );
		renderer->apply();
		renderer->changeShaderConstant4x4f( "WorldTransformMatrix", m_modelMvp );
		if (dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (dwTerrainMapWidth)*(dwTerrainMapLength), 0, (dwTerrainMapWidth-1)*(dwTerrainMapLength-1)*2)!=D3D_OK) {
			OutputDebugString("Failed to render terrain");
		}
		dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	// Unbind the DMAP texture
	dev->SetTexture(D3DDMAPSAMPLER, NULL);

	// Disable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));

	//
	// Render sky-box (after terrain for best performance)
	//
	DrawSkybox();

	//
	// Calculate new particles position
	//

	// Set render targets
	renderer->changeRenderTarget( RT_USERDEFINED, RT_USERDEFINED );
	hres = dev->SetRenderTarget( 0, m_pParticlesPositionSurface[(nFrame+1)%2] );
	hres = dev->SetRenderTarget( 1, m_pParticlesVelocitySurface[(nFrame+1)%2] );
	hres = dev->SetDepthStencilSurface(m_pDummyDepthSurface);

	// Set up the viewport for our particles render
	RenderTargetViewport.X = 0;
	RenderTargetViewport.Y = 0;
	RenderTargetViewport.Width = PARTICLES_TEXTURE_SIZE;
	RenderTargetViewport.Height = PARTICLES_TEXTURE_SIZE;
	RenderTargetViewport.MinZ = 0.0f;
	RenderTargetViewport.MaxZ = 1.0f;
	dev->SetViewport( &RenderTargetViewport );

	// Turn Z testing off
	dev->SetRenderState(D3DRS_ZENABLE, FALSE);

	// Set shader
	renderer->setShader( m_particlesmathShader);
	renderer->setCullFace( BACK );
	renderer->apply();
	renderer->changeShaderConstant4f( "ClampedTerrainDimensions", vec4(0.9f*fTotalTerrainWidth, 0.9f*fTotalTerrainLength, fTotalTerrainWidth/2.0f, fTotalTerrainLength/2.0f));
	renderer->changeShaderConstant2f( "RcpTotalTerrainDimensions", vec2(1.0f/fTotalTerrainWidth, 1.0f/fTotalTerrainLength));
	renderer->changeShaderConstant2f( "HeightMapSize", vec2((float)dwTerrainMapWidth, (float)dwTerrainMapLength));
	renderer->changeShaderConstant2f( "RcpParticlesMapSize", vec2(1.0f/(PARTICLES_TEXTURE_SIZE-1.0f), 1.0f/(PARTICLES_TEXTURE_SIZE-1.0f)));
	renderer->changeShaderConstant2f( "TexelSize", vec2(1.0f/dwTerrainMapWidth, 1.0f/dwTerrainMapLength));
	renderer->changeShaderConstant2f( "TerrainUnitSize", vec2(fTotalTerrainWidth/dwTerrainMapWidth, fTotalTerrainLength/dwTerrainMapLength));
	renderer->changeShaderConstant1f( "SnowFlakeLife", FRAND(2.0f) + SnowFlakeLife->getValue());
	renderer->changeShaderConstant1f( "BounceFactor", BounceFactor->getValue());

	if (bStepTime) {
		if (bStep) fTimeElapsed = 1.0f/60.0f;
			else fTimeElapsed = 0.0f;
		bStep = false;
	}
	else {
		fTimeElapsed = frameTime;
	}
	renderer->changeShaderConstant1f( "fTimeElapsed", fTimeElapsed);

	// Set texture inputs
	dev->SetTexture(0, m_pParticlesPosition[(nFrame%2)]);
	dev->SetTexture(1, m_pParticlesVelocity[(nFrame%2)]);
	dev->SetTexture(2, m_pRenderTargetTexture);
	dev->SetTexture(3, ((D3D9Renderer*)renderer)->getD3DTexture(NoiseTexture));

	// Override filtering modes set by framework
	dev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	dev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	dev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	dev->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	dev->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	dev->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	dev->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	dev->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	dev->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	dev->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	dev->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	dev->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	dev->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	dev->SetSamplerState( 3, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	// Draw quad
	DrawScreenAlignedQuad(dev, PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, 0.0f);

	//
	// Render particles
	//

	// Revert to default render target
	hres = dev->SetRenderTarget( 1, NULL );
	renderer->resetRenderTarget();
	dev->SetViewport( &MainViewport );

	// Set shader
	renderer->setShader( m_particlesrenderShader );
	renderer->setTexture("SnowFlakeTexture", SnowFlakeTexture);
	renderer->setCullFace( BACK );
	renderer->setDepthFunc( LEQUAL );
	renderer->setBlending(SRC_ALPHA, ONE);
	renderer->apply();
	renderer->changeShaderConstant4x4f( "WorldTransformMatrix", m_modelMvp * translate(0, 1.4f, 0));
	renderer->changeShaderConstant3f( "CameraPosition", camPos);
	renderer->changeShaderConstant1f( "PointSizeFactor", height * 4.3f);

	// Override filtering modes set by framework
	dev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	dev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	dev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	// Enable point sprites
	dev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);

	// Enable render to vertex buffer extension
	hres = dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));

	// Set vertex declaration
	hres = dev->SetVertexDeclaration(m_pPositionVertexDeclaration);

	// Set stream source to dummy VB (used to specify offset and stride)
	hres = dev->SetStreamSource(0, m_pDummyVB, 0, 4*sizeof(unsigned short));

	// Bind the texture VB to the DMAP sampler
	dev->SetTexture(D3DDMAPSAMPLER, m_pParticlesPosition[nFrame%2]);

	// Tell the driver that stream 0 is to be fetched from the DMAP texture
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DMAP));

	// Enable blending and turn Z-writes off
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_ZENABLE, TRUE);
	dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// Draw the particles
	if (dev->DrawPrimitive(D3DPT_POINTLIST, 0, NUMBER_OF_PARTICLES)!=D3D_OK) {
		OutputDebugString("Failed to render particles");
	}

	// Stream 0 restored to regular vertex buffer mode
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DIS));

	// Unbind the DMAP texture
	dev->SetTexture(D3DDMAPSAMPLER, NULL);

	// Disable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));

	// Disable point sprites
	dev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	hres = dev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);


	// Render overlay
	if (DemoMenu->isItemChecked(Display2DView)) {

		// Set shader and texture
		renderer->setShader( m_overlayShader );
		renderer->setCullFace( CULL_NONE );
		renderer->setTexture((unsigned int)0, RenderTargetTexture);
		renderer->apply();

		// Override filtering modes set by framework
		dev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		dev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		dev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

		// Set blending
		dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		// Create an overlay quad model
		const float overlayData[] = {
			4.5f,                4.5f + OVERLAY_SIZE, 0.0f, 1.0f, 0.0f, 0.0f,
			4.5f + OVERLAY_SIZE, 4.5f + OVERLAY_SIZE, 0.0f, 1.0f, 1.0f, 0.0f,
			4.5f,                4.5f,                0.0f, 1.0f, 0.0f, 1.0f,
			4.5f + OVERLAY_SIZE, 4.5f,                0.0f, 1.0f, 1.0f, 1.0f
		};

		// Set vertex declaration
		dev->SetVertexDeclaration(m_pPositionTTexcoordVertexDeclaration);
		
		// Cull back faces
		dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

		// Use fixed-function vertex processing
		dev->SetVertexShader(NULL);

		// Draw quad
		hres=dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, overlayData, 6*sizeof(float));
	}

	// Increase frame number
	nFrame++;
}

//---------------------------------------------------------------------------
// App update transforms
//
//---------------------------------------------------------------------------
void SampleApp::updateTransforms()
{
	// Create view matrix and convert it to column-major
	float4x4 modelview = rotateXY4(-wx, -wy);
	modelview.translate(-camPos);
	matView = (D3DXMATRIX)modelview;
	D3DXMatrixTranspose(&matView, &matView);

	// Create MVP matrix
	D3DXMatrixMultiply((D3DXMATRIX*)&m_modelMvp, &matView, &matProj);
	D3DXMatrixTranspose((D3DXMATRIX*)&m_modelMvp, (D3DXMATRIX*)&m_modelMvp);
}


//-----------------------------------------------------------------------------
// Name: DrawScreenAlignedQuad()
// Desc: Simple function to render a screen-aligned quad
//-----------------------------------------------------------------------------
bool SampleApp::DrawScreenAlignedQuad(LPDIRECT3DDEVICE9 pd3dDevice, float fWidth, float fHeight, float fZ)
{
	HRESULT	hres;

	// Create screen-aligned quad geometry
	float quadVertices[] = {
		- 0.5f,				-0.5f,				fZ, 1.0f, 0.0f, 0.0f,  // x y z w tu tv
		- 0.5f + fWidth,	-0.5f,				fZ, 1.0f, 1.0f, 0.0f,
		- 0.5f,				-0.5f + fHeight,	fZ, 1.0f, 0.0f, 1.0f,
		- 0.5f + fWidth,	-0.5f + fHeight,	fZ, 1.0f, 1.0f, 1.0f
	};

	// Set vertex declaration
	pd3dDevice->SetVertexDeclaration(m_pPositionTTexcoordVertexDeclaration);

	// Cull back faces
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Use fixed-function vertex processing
	pd3dDevice->SetVertexShader(NULL);

	// Draw quad
	hres = pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quadVertices, 6*sizeof(float));

	return (hres==D3D_OK);
}


//-----------------------------------------------------------------------------
// Name: DrawQuad()
// Desc: Simple function to render a quad in screen coordinates
//       fX, fX are the coordinates of the quad's CENTER
//-----------------------------------------------------------------------------
bool SampleApp::DrawQuad(LPDIRECT3DDEVICE9 pd3dDevice, float fX, float fY, float fZ, float fWidth, float fHeight)
{
	HRESULT	hres;

	// Create fullscreen quad geometry (displace 
	float quadVertices[] = {
		fX - fWidth/2.0f, fY - fHeight/2.0f, fZ, 1.0f, 0.0f, 0.0f,  // x y z w tu tv
		fX + fWidth/2.0f, fY - fHeight/2.0f, fZ, 1.0f, 1.0f, 0.0f,
		fX - fWidth/2.0f, fY + fHeight/2.0f, fZ, 1.0f, 0.0f, 1.0f,
		fX + fWidth/2.0f, fY + fHeight/2.0f, fZ, 1.0f, 1.0f, 1.0f
	};

	// Set vertex declaration
	pd3dDevice->SetVertexDeclaration(m_pPositionTTexcoordVertexDeclaration);
	
	// Cull back faces
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Use fixed-function vertex processing
	pd3dDevice->SetVertexShader(NULL);

	// Draw quad
	hres=pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quadVertices, 6*sizeof(float));

	return (hres==D3D_OK);
}


//-----------------------------------------------------------------------------
// Name: onMouseMove()
// Desc: Handle mouse movement input
//-----------------------------------------------------------------------------
bool SampleApp::onMouseMove(const int x, const int y, const bool lButton, const bool mButton, const bool rButton)
{
	D3DXMATRIX		Identity;
	D3DXVECTOR3		MouseFrontClip, MouseFarClip;
	D3DXVECTOR3		UnprojectedMouseFrontClip, UnprojectedMouseFarClip;
	
	// Set identity matrix
	D3DXMatrixIdentity(&Identity);

	// Only perform back-projection if initialization has been done
	if (nFrame) {

		// Back-transform front clip plane
		MouseFrontClip.x = (float)x;
		MouseFrontClip.y = (float)y;
		MouseFrontClip.z = 0.0f;
		D3DXVec3Unproject(&UnprojectedMouseFrontClip, &MouseFrontClip, &MainViewport, &matProj, &matView, &Identity);

		// Back-transform far clip plane
		MouseFarClip.x = (float)x;
		MouseFarClip.y = (float)y;
		MouseFarClip.z = 1.0f;
		D3DXVec3Unproject(&UnprojectedMouseFarClip, &MouseFarClip, &MainViewport, &matProj, &matView, &Identity);

		// Set line coordinates
		MouseRayCast[0].x = UnprojectedMouseFrontClip.x;
		MouseRayCast[0].y = UnprojectedMouseFrontClip.y;
		MouseRayCast[0].z = UnprojectedMouseFrontClip.z;
		MouseRayCast[0].w = 1.0f;
		MouseRayCast[1].x = UnprojectedMouseFarClip.x;
		MouseRayCast[1].y = UnprojectedMouseFarClip.y;
		MouseRayCast[1].z = UnprojectedMouseFarClip.z;
		MouseRayCast[1].w = 1.0f;
	}

	// Call normal mouse move handler
	return BaseApp::onMouseMove(x, y, lButton, mButton, rButton);;
}

//-----------------------------------------------------------------------------
// Name: onMouseClick()
// Desc: Handle mouse click input
//-----------------------------------------------------------------------------
bool SampleApp::onMouseClick(const int x, const int y, const unsigned int button, const bool pressed)
{
	bIncreaseTerrain = ((button == RIGHT_BUTTON) && pressed);
	bDecreaseTerrain = ((button == MIDDLE_BUTTON) && pressed);

	// Call normal mouse click handler
	return BaseApp::onMouseClick(x, y, button, pressed);
}


//-----------------------------------------------------------------------------
// Name: InitializeParticlesTextures()
// Desc: Initialize the first particle texture with startup data
//-----------------------------------------------------------------------------
void SampleApp::InitializeParticlesTextures(LPDIRECT3DDEVICE9 pd3dDevice)
{
	IDirect3DTexture9	*m_pTempTexture;
	D3DLOCKED_RECT		LockedRect;
	D3DVIEWPORT9		RenderTargetViewport;
	HRESULT				hres;

	// The only way to initialize a render target with contents is to render into it
	// Hence we create a temporary Lock()able texture so that we can render its contents onto our render targets
	if (pd3dDevice->CreateTexture(PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, 1, 0, D3DFMT_A16B16G16R16F, D3DPOOL_MANAGED, &m_pTempTexture, NULL) != D3D_OK){
		ERRORMSG("Couldn't create temp texture");
		return;
	}

	// Set render states
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pd3dDevice->SetPixelShader(NULL);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// Prepare the viewport for our particles render
	RenderTargetViewport.X = 0;
	RenderTargetViewport.Y = 0;
	RenderTargetViewport.Width = PARTICLES_TEXTURE_SIZE;
	RenderTargetViewport.Height = PARTICLES_TEXTURE_SIZE;
	RenderTargetViewport.MinZ = 0.0f;
	RenderTargetViewport.MaxZ = 1.0f;

	// Set texture input
	pd3dDevice->SetTexture(0, m_pTempTexture);

	//
	// Position texture
	//

	// Lock texture
	hres = m_pTempTexture->LockRect(0, &LockedRect, NULL, 0);

	// Fill texture with data
	for (int i=0; i<PARTICLES_TEXTURE_SIZE; i++) {
		unsigned short *pusHalf = (unsigned short *)((char *)LockedRect.pBits + i*LockedRect.Pitch);
		for (int j=0; j<PARTICLES_TEXTURE_SIZE; j++) {

			// Red = X
			*pusHalf++ = half(FRAND(fTotalTerrainWidth*0.9f) - (fTotalTerrainWidth*0.9f)/2.0f).sh;

			// Green = Y (force some particles to spawn under terrain so that their life starts decreasing immediately,
			// ensuring a continuous flow of snow flakes)
			*pusHalf++ = half(FRAND(250.0f)+50.0f).sh;

			// Blue = Z
			*pusHalf++ = half(FRAND(fTotalTerrainLength*0.9f) - (fTotalTerrainLength*0.9f)/2.0f).sh;

			// Alpha = W = particle's life length after landing, in seconds
			*pusHalf++ = half(FRAND(2.0f) + SnowFlakeLife->getValue()).sh;
		}
	}

	// Unlock texture
	m_pTempTexture->UnlockRect(0);

	// Then we manually set the render target and depth-stencil surface
	pd3dDevice->SetRenderTarget( 0, m_pParticlesPositionSurface[0] );
	pd3dDevice->SetDepthStencilSurface(m_pDummyDepthSurface);
	pd3dDevice->SetViewport( &RenderTargetViewport );

	// Draw quad
	DrawScreenAlignedQuad(pd3dDevice, PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, 0.0f);

	//
	// Velocity texture
	//

	// Lock texture
	hres = m_pTempTexture->LockRect(0, &LockedRect, NULL, 0);

	// Fill texture with data (initial velocity is 0)
	for (int i=0; i<PARTICLES_TEXTURE_SIZE; i++) {
		unsigned short *pusHalf = (unsigned short *)((char *)LockedRect.pBits + i*LockedRect.Pitch);
		for (int j=0; j<PARTICLES_TEXTURE_SIZE; j++) {
			
			// Red = X 
			*pusHalf++ = half(0.0f).sh;

			// Green = Y
			*pusHalf++ = half(0.0f).sh;

			// Blue = Z
			*pusHalf++ = half(0.0f).sh;

			// Alpha = W
			*pusHalf++ = half(0.0f).sh;
		}
	}

	// Unlock texture
	m_pTempTexture->UnlockRect(0);

	// Then we manually set the render target and depth-stencil surface
	pd3dDevice->SetRenderTarget( 0, m_pParticlesVelocitySurface[0] );
	pd3dDevice->SetDepthStencilSurface(m_pDummyDepthSurface);
	pd3dDevice->SetViewport( &RenderTargetViewport );

	// Draw quad
	pd3dDevice->SetPixelShader(NULL);
	DrawScreenAlignedQuad(pd3dDevice, PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, 0.0f);

	// Turn Z testing back on
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	// Release temp texture
	m_pTempTexture->Release();
}


//-----------------------------------------------------------------------------
// Name: DrawSkybox()
// Desc: Draw a skybox as a cube centered around the camera position
//-----------------------------------------------------------------------------
void SampleApp::DrawSkybox()
{
	static const float verts[] = { -1,-1,-1, -1,-1,1, -1,1,-1, -1,1,1, 1,-1,-1, 1,-1,1, 1,1,-1, 1,1,1 };
	static const ushort inds[] = { 2,3,0, 0,3,1, 3,7,1, 1,7,5, 7,6,5, 5,6,4, 6,2,4, 4,2,0, 6,7,2, 2,7,3, 0,1,4, 4,1,5 };

	renderer->setShader(m_skyboxShader);
	renderer->setVertexFormat(skyboxVF);
	renderer->setTexture("Skybox", SkyboxTexture);
	renderer->setCullFace( BACK );
	renderer->setMask(COLOR);
	renderer->apply();

	// Get mouse rotation and convert it into a column-major matrix
	D3DXMATRIX	matSkybox;
	mat4		modelview = rotateXY4(-wx, -wy);
	D3DXMatrixTranspose(&matSkybox, (D3DXMATRIX *)&modelview);

	// Multiply with projection matrix and send as constant
	D3DXMatrixMultiplyTranspose(&matSkybox, &matSkybox, &matProj);
	renderer->changeShaderConstant4x4f("mvp", pegToFarPlane((mat4 &) matSkybox));

	// Draw skybox
	dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 12, inds, D3DFMT_INDEX16, verts, 3 * sizeof(float));
}

//-----------------------------------------------------------------------------
// Name: initGUI()
// Desc: init GUI
//-----------------------------------------------------------------------------
void SampleApp::initGUI()
{
	addWidget(Label_IncreaseElevation = new Label(5, 720, &mainFont, "Right Mouse Button: Increase elevation", 18, 20));
	addWidget(Label_DecreaseElevation = new Label(5, 745, &mainFont, "Middle Mouse Button: Decrease elevation", 18, 20));

	addWidget(Label_BounceFactor = new Label(900, 710, &mainFont, "Bounce Factor", 18, 20));
	addWidget(BounceFactor  = new Slider(900, 730, 100, 25, true, 0.0f, 0.7f, 0.35f));

	addWidget(Label_SnowFlakeLife = new Label(750, 710, &mainFont, "Snowflake Life", 18, 20));
	addWidget(SnowFlakeLife  = new Slider(750, 730, 100, 25, true, 0.0f, 5.0f, 0.5f));

	DemoMenu = mainMenu->addMenu("Demo");
	Display2DView =		DemoMenu->addItem("Terrain 2D View (V)", MENU_CHECKABLE | MENU_CHECKED);
	ToggleWireframe =	DemoMenu->addItem("Wireframe (W)", MENU_CHECKABLE);
	Reset =				DemoMenu->addItem("Particles Reset (R)", 0);
	
	D3D9App::initGUI();
}


//-----------------------------------------------------------------------------
// Name: onMenuItemPress()
// Desc: handle menu messages
//-----------------------------------------------------------------------------
void SampleApp::onMenuItemPress( Menu *menu, MenuItemID item )
{
	if (menu==DemoMenu) {
		if (item==Reset) {
			// Reset particles
			if (nFrame>0) bReinitializeParticles = true;
		}
	}

	BaseApp::onMenuItemPress( menu, item );
}
