/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

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

// Macros
#define RELEASE(x)		if (x) {(x)->Release(); (x) = 0; }
#define FRAND(x)		(((float)stx_rand()/RAND_MAX) * (x))
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
class App 
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
	App()
		: OVERLAY_SIZE(256)
		, nFrame(0)
		, m_pRenderTargetTexture(0)
		, bReinitializeParticles(false)
		, bStepTime(false)
		, bIncreaseTerrain(false)
		, bDecreaseTerrain(false)
	{
		// void
	}

	// Default destructor.
	~App() {}

	//---------------------------------------------------------------------------
	// Overriden from framework
	//---------------------------------------------------------------------------
	char *getTitleString() { return "R2VB-Collision"; }
	char *getHomeDirectory() { return "../DirectX 9.0/R2VB-Collision"; }

	void setup() {
		width  = 1024;
		height = 768;
	}
private:
	const uint32 OVERLAY_SIZE;                  // quad overlay size (for displaying the terrain texture contents)

	// Terrain map dimensions
	uint32 dwTerrainMapWidth;                   // Terrain map dimensions
	uint32 dwTerrainMapLength;

	// Data required for mouse picking
	D3DFROMWINEVIEWPORT9	MainViewport;
	D3DXFROMWINEVECTOR4		MouseRayCast[2];

	// Frame variables
	float			fTimeElapsed;
	int				nFrame;

	// Bools
	bool			bReinitializeParticles;
	bool			bStepTime;
	bool			bStep;
	bool			bIncreaseTerrain, bDecreaseTerrain;

	// Terrain texture and surface
	TextureID 				m_pRenderTargetTexture;
	SurfaceID 				m_pRenderTargetSurface;
	SurfaceID 				m_pDummyDepthSurface;

	// Particle textures and surfaces (2 each for ping-ponging)
	TextureID				m_pParticlesPosition[2];
	TextureID				m_pParticlesVelocity[2];
	SurfaceID				m_pParticlesPositionSurface[2];
	SurfaceID				m_pParticlesVelocitySurface[2];

	// Ray intersection texture and surface
	TextureID				m_pRayIntersectionTexture;
	SurfaceID				m_pRayIntersectionSurface;

	// Vertex and index data: note that the terrain does not use a vertex buffer (since the VB in this sample is a texture)
	IndexBufferID			m_pTerrainIndexBuffer;
	VertexFormatID	m_pPositionVertexDeclaration;
	//VertexFormatID	m_pPositionTVertexDeclaration;
	VertexFormatID	m_pPositionTexcoordVertexDeclaration;
	ShaderID	m_pPositionVertexShader;
	//ShaderID	m_pPositionTVertexShader;
	ShaderID	m_pPositionTexcoordVertexShader;
	VertexBufferID			m_pDummyVB;
	//VertexFormatID					skyboxVF;

	// Shaders
	ShaderID	m_sceneShader, m_scenewireframeShader, m_overlayShader, m_particlesmathShader, m_particlesrenderShader, m_rayintersectionShader;//, m_skyboxShader; 
	VertexFormatID	m_sceneVF, m_scenewireframeVF, m_overlayVF, m_particlesmathVF, m_particlesrenderVF, m_rayintersectionVF;

	// Textures
	TextureID						TerrainTexture, RenderTargetTexture;
	TextureID						TerrainDiffuseTexture;
	TextureID						HillSprite, SnowFlakeTexture;
	TextureID						NoiseTexture, SkyboxTexture;
	Menu							*DemoMenu;
	MenuItemID						Reset, Display2DView, ToggleWireframe;
	Slider							*BounceFactor, *SnowFlakeLife;
#ifdef R2VB_GUI
	// GUI stuff
	Label							*Label_IncreaseElevation, *Label_DecreaseElevation, *Label_BounceFactor, *Label_SnowFlakeLife;
#endif

	// Matrices
	D3DXFROMWINEMATRIX						matView, matProj;
	D3DXFROMWINEMATRIX							m_modelMvp;
    float width;
    float height;
    D3DXFROMWINEVECTOR3 camPos;
    float wx;
    float wy;
    float speed;
    bool debugRuntime;
	TextureID env;
	stx_SkyBox sb;
public:
#ifdef R2VB_GUI
//---------------------------------------------------------------------------
// Handle resizing messages
//
//---------------------------------------------------------------------------
void onSize(const int w, const int h)
{
	Label_IncreaseElevation->setPosition(5.0f, h - 48.0f);
	Label_DecreaseElevation->setPosition(5.0f, h - 23.0f);

	Label_BounceFactor->setPosition(w - 124.0f, h - 58.0f);
	BounceFactor->setPosition(w - 124.0f, h - 38.0f);

	Label_SnowFlakeLife->setPosition(w - 274.0f, h - 58.0f);
	SnowFlakeLife->setPosition(w - 274.0f, h - 38.0f);

	//D3D9App::onSize(w, h);
}
//-----------------------------------------------------------------------------
// Name: initGUI()
// Desc: init GUI
//-----------------------------------------------------------------------------
void initGUI()
{
	BaseApp::addWidget(Label_IncreaseElevation = new Label(5, 720, BaseApp::getMainFont(), "Right Mouse Button: Increase elevation", 18, 20));
	BaseApp::addWidget(Label_DecreaseElevation = new Label(5, 745, BaseApp::getMainFont(), "Middle Mouse Button: Decrease elevation", 18, 20));

	BaseApp::addWidget(Label_BounceFactor = new Label(900, 710, BaseApp::getMainFont(), "Bounce Factor", 18, 20));
	BaseApp::addWidget(BounceFactor  = new Slider(900, 730, 100, 25, true, 0.0f, 0.7f, 0.35f));

	BaseApp::addWidget(Label_SnowFlakeLife = new Label(750, 710, BaseApp::getMainFont(), "Snowflake Life", 18, 20));
	BaseApp::addWidget(SnowFlakeLife  = new Slider(750, 730, 100, 25, true, 0.0f, 5.0f, 0.5f));

	DemoMenu = BaseApp::getMenu()->addMenu("Demo");
	Display2DView =		DemoMenu->addItem("Terrain 2D View (V)", MENU_CHECKABLE | MENU_CHECKED);
	ToggleWireframe =	DemoMenu->addItem("Wireframe (W)", MENU_CHECKABLE);
	Reset =				DemoMenu->addItem("Particles Reset (R)", 0);
	
	BaseApp::initGUI();
}
#endif
//---------------------------------------------------------------------------
// Reset camera
//
//---------------------------------------------------------------------------
void resetCamera(){
	// Define initial camera view
	camPos = D3DXFROMWINEVECTOR3(0, 150, -600);
	wx = D3DXFROMWINE_PI/8.0f;
	wy = 0;
}

//---------------------------------------------------------------------------
// App load.
//
//---------------------------------------------------------------------------
bool load()
{
	int hres;

	//
	// Load shaders
	//
	m_sceneShader				= IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Collision/Scene.hlsl", "main", "main");
	m_scenewireframeShader		= IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Collision/Scene_wireframe.hlsl", "main", "main");
	m_overlayShader			= IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Collision/Overlay.hlsl", "main", "main");
	m_particlesmathShader		= IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Collision/ParticlesMath.hlsl", "main", "main");
	m_particlesrenderShader	= IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Collision/ParticlesRender.hlsl", "main", "main");
	m_rayintersectionShader	= IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Collision/RayIntersection.hlsl", "main", "main");
	//m_skyboxShader				= IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Collision/skybox.hlsl", "main", "main");
	
	m_pPositionVertexShader = m_sceneShader; // ???
	m_pPositionTexcoordVertexShader = m_sceneShader; // ???

	//
	// Create all vertex declarations we'll need
	//

	// Pre-transformed position data in float16 format
	const FormatDesc position16fVertexAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_HALF, 4
	};
	// Pre-transformed position data and 1 pair of texture coordinates
	const FormatDesc positionTexcoordVertexAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,		
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
	};

	// Create vertex declarations
	m_pPositionVertexDeclaration=IRenderer::GetRendererInstance()->addVertexFormat(position16fVertexAttribs, elementsOf(position16fVertexAttribs), m_sceneShader);
	m_pPositionTexcoordVertexDeclaration=IRenderer::GetRendererInstance()->addVertexFormat(positionTexcoordVertexAttribs, elementsOf(positionTexcoordVertexAttribs), m_sceneShader /* ??? */);
	
	m_sceneVF	    = IRenderer::GetRendererInstance()->addVertexFormat(position16fVertexAttribs, elementsOf(position16fVertexAttribs), m_sceneShader);
	m_scenewireframeVF  = IRenderer::GetRendererInstance()->addVertexFormat(position16fVertexAttribs, elementsOf(position16fVertexAttribs), m_scenewireframeShader);
	m_overlayVF	    = IRenderer::GetRendererInstance()->addVertexFormat(position16fVertexAttribs, elementsOf(position16fVertexAttribs), m_overlayShader);
	m_particlesmathVF   = IRenderer::GetRendererInstance()->addVertexFormat(position16fVertexAttribs, elementsOf(position16fVertexAttribs), m_particlesmathShader);
	m_particlesrenderVF = IRenderer::GetRendererInstance()->addVertexFormat(position16fVertexAttribs, elementsOf(position16fVertexAttribs), m_particlesrenderShader);
	m_rayintersectionVF = IRenderer::GetRendererInstance()->addVertexFormat(position16fVertexAttribs, elementsOf(position16fVertexAttribs), m_rayintersectionShader);
/*
	// Create skybox vertex format
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3
	};
	skyboxVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);
*/
	//
	// Load textures
	//

	// Terrain texture: contains reference height data for terrain (FORMAT_RGBA32F format)
	// Use point sampling since this data contains vertex buffer data (i.e. we don't want it filtered)
	// Note that we won't render from this reference texture directly: a FORMAT_RGBA16F render target will be used instead
	TerrainTexture = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/land_DIS_512.hdr", false, IRenderer::GetRendererInstance()->GetnearestClamp());

	// Terrain diffuse and illumination textures
	TerrainDiffuseTexture = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/land_DL.dds", false, IRenderer::GetRendererInstance()->Getbilinear());

	// Load snow texture
	SnowFlakeTexture = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/particle.dds", false, IRenderer::GetRendererInstance()->Getbilinear());

	// Texture used to create bump or craters in terrain data
	Image3 img;
	ubyte *dest = img.create(FORMAT_I8, 512, 512, 1, 1);
	for (int y = 0; y < 512; y++){
		float fy = (y / 511.0f) * 2 - 1;
		for (int x = 0; x < 512; x++){
			float fx = (x / 511.0f) * 2 - 1;
			*dest++ = (ubyte) (255 * max(1.0f - fx * fx - fy * fy, 0.0f));
		}
	}
	HillSprite = IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());

	// Noise texture
	NoiseTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/noise2D.tga", false, IRenderer::GetRendererInstance()->Getbilinear());

	// Background cube map
	#if 0
	env = IRenderer::GetRendererInstance()->addCubemap("Olympus", "jpg");
	#else
    	sb.init("/SkyBox/cubemaps/x/OlympusGL.dds");
	#endif

	// Save terrain map dimensions
	dwTerrainMapWidth = IRenderer::GetRendererInstance()->GetTextureWidth(TerrainTexture);
	dwTerrainMapLength = IRenderer::GetRendererInstance()->GetTextureHeight(TerrainTexture);

	//
	// Reference terrain texture contains height data only (Y), need to add X, Z and W data as well
	//

	// Retrieve surface attached to terrain texture
	TextureID m_pTerrainTexture = (TerrainTexture);
	int pitch=0;
	BYTE* pBits = IRenderer::GetRendererInstance()->LockTexture(m_pTerrainTexture, 0, pitch);
	// Fill data with XZW data, leaving Y (height) untouched
	for (uint32 i=0; i<dwTerrainMapLength; i++) {
		float *pfGrid = (float *)((char *)pBits + i*pitch);
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
	IRenderer::GetRendererInstance()->UnlockTexture(m_pTerrainTexture, 0);
	//RELEASE(m_pTerrainSurface);

	// Call onReset to create application-managed surfaces
	if (!onReset()) return false;

	//
	// Create index buffer for terrain
	//
	std::vector<__DWORD__> vi((dwTerrainMapWidth-1)*(dwTerrainMapLength-1)*6);
#if 0 // ???
	// Lock index buffer
	// Fill index buffer
	for (__DWORD__ i=0; i<(dwTerrainMapLength-1); i++) {
		for (__DWORD__ j=0; j<(dwTerrainMapWidth-1); j++) {
			// 1st triangle
			vi[6*i] = i*dwTerrainMapLength + j;
			vi[6*i+1] = (i+1)*dwTerrainMapLength + j;
			vi[6*i+2] = i*dwTerrainMapLength + j + 1;
			// 2nd triangle
			vi[6*i+3] = i*dwTerrainMapLength + j + 1;
			vi[6*i+4] = (i+1)*dwTerrainMapLength + j;
			vi[6*i+5] = (i+1)*dwTerrainMapLength + j + 1;
		}
	}
#endif
	m_pTerrainIndexBuffer = IRenderer::GetRendererInstance()->addIndexBuffer(vi.size()*sizeof(__DWORD__), 4, STATIC, &vi[0]);

	// Create dummy vertex buffer (needed for R2VB API overload)
	// The dummy vertex buffer needs to be as large as either the terrain or particle map size (since it's used for both)
	m_pDummyVB=IRenderer::GetRendererInstance()->addVertexBuffer(MAX(dwTerrainMapWidth, PARTICLES_TEXTURE_SIZE)*MAX(dwTerrainMapLength, PARTICLES_TEXTURE_SIZE)*2*sizeof(half), STATIC);

	return true;
}


//---------------------------------------------------------------------------
// App reset.
//
//---------------------------------------------------------------------------
bool onReset()
{
	int hres;
	
	// Put all application-managed resources here (e.g. D3DPOOL_DEFAULT resources)

	// Create terrain render target texture that we will render terrain geometry from
	// Create ID for this render target
	RenderTargetTexture = IRenderer::GetRendererInstance()->addRenderTarget(dwTerrainMapWidth, dwTerrainMapLength, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());

	// Retrieve surface attached to render target texture

	// Create particles render target textures (position and velocity, double-buffered for ping-ponging)
	// that we will render particles geometry from
	m_pParticlesPosition[0] = IRenderer::GetRendererInstance()->addRenderTarget(PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, FORMAT_RGBA16F, IRenderer::GetRendererInstance()->GetnearestClamp());
	m_pParticlesPosition[1] = IRenderer::GetRendererInstance()->addRenderTarget(PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, FORMAT_RGBA16F, IRenderer::GetRendererInstance()->GetnearestClamp());
	m_pParticlesVelocity[0] = IRenderer::GetRendererInstance()->addRenderTarget(PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, FORMAT_RGBA16F, IRenderer::GetRendererInstance()->GetnearestClamp());
	m_pParticlesVelocity[1] = IRenderer::GetRendererInstance()->addRenderTarget(PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, FORMAT_RGBA16F, IRenderer::GetRendererInstance()->GetnearestClamp());

	// Retrieve surfaces associated with textures

	// Create ray intersection render target (1x1) that is used to calculate mouse ray intersection with terrain
	m_pRayIntersectionTexture = IRenderer::GetRendererInstance()->addRenderTarget(1, 1, FORMAT_RGBA16f);

	// Create dummy depth buffer
	// The dummy depth buffer needs to be as large as either the terrain or particle map size (since it's used for both)
	m_pDummyDepthSurface=IRenderer::GetRendererInstance()->CreateDepthStencilSurface(MAX(dwTerrainMapWidth, PARTICLES_TEXTURE_SIZE), MAX(dwTerrainMapLength, PARTICLES_TEXTURE_SIZE), FORMAT_D16);

	// Reset frame number
	nFrame = 0;
	
	// Create projection matrix (constant for the whole demo)
	D3DXFROMWINEMatrixPerspectiveFovLH(&matProj, D3DXFROMWINE_PI / 3, (float)width/height, FRONTCLIP, FARCLIP);

	return true;
}


//---------------------------------------------------------------------------
// App pre reset.
//
//---------------------------------------------------------------------------
bool onPreReset()
{
	// Destroy all application-managed resources here (e.g. D3DPOOL_DEFAULT resources)
	//RELEASE(m_pDummyDepthSurface);
	//RELEASE(m_pRayIntersectionSurface);
	//RELEASE(m_pRayIntersectionTexture);
	//RELEASE(m_pParticlesVelocitySurface[1]);
	//RELEASE(m_pParticlesVelocitySurface[0]);
	//RELEASE(m_pParticlesPositionSurface[1]);
	//RELEASE(m_pParticlesPositionSurface[0]);
	//RELEASE(m_pParticlesVelocity[1]);
	//RELEASE(m_pParticlesVelocity[0]);
	//RELEASE(m_pParticlesPosition[1]);
	//RELEASE(m_pParticlesPosition[0]);
	//RELEASE(m_pRenderTargetSurface);

	return true;
}

//---------------------------------------------------------------------------
// App unload.
//
//---------------------------------------------------------------------------
bool unload()
{
	// Release memory allocated in Reset()
	onPreReset();

	// Since we are managing these Direct3D reference pointers ourselves,
	// we must be careful to release them when the app quits.
	//RELEASE(m_pTerrainIndexBuffer);
	//RELEASE(m_pPositionTexcoordVertexDeclaration);
	//RELEASE(m_pPositionTVertexDeclaration);
	//RELEASE(m_pPositionVertexDeclaration);
	//RELEASE(m_pDummyVB);

    return true;
}


//---------------------------------------------------------------------------
// App update time ('tick').
//
//---------------------------------------------------------------------------
void updateTime( const float fTime )
{
	updateTime( fTime );
	updateTransforms();
}


//---------------------------------------------------------------------------
// App draw frame.
//
//---------------------------------------------------------------------------
void drawFrame()
{
	int			hres;
	D3DFROMWINEVIEWPORT9	RenderTargetViewport;

	// Get a copy of the current viewport
	IRenderer::GetRendererInstance()->GetViewport( &MainViewport );

	// Tell the sample framework that we'll be managing the render target changes ourselves
	//IRenderer::GetRendererInstance()->changeRenderTarget( RT_USERDEFINED, RT_USERDEFINED );

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
		IRenderer::GetRendererInstance()->changeRenderTarget( m_pRenderTargetSurface, m_pDummyDepthSurface);
		//IRenderer::GetRendererInstance()->SetDepthStencilSurface(m_pDummyDepthSurface);
		IRenderer::GetRendererInstance()->setViewport( &RenderTargetViewport );

		// Turn Z testing off
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, FALSE);

		// Render terrain onto FORMAT_RGBA16F render target
		IRenderer::GetRendererInstance()->setTexture("(unsigned int)0", TerrainTexture);
		//IRenderer::GetRendererInstance()->setShader(SHADER_NONE);
		
		//IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		//IRenderer::GetRendererInstance()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		//IRenderer::GetRendererInstance()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		// Draw quad filling the whole render target
		IRenderer::GetRendererInstance()->setShader(m_pPositionTexcoordVertexShader);IRenderer::GetRendererInstance()->setVertexFormat(m_pPositionTexcoordVertexDeclaration);DrawScreenAlignedQuad( (float)dwTerrainMapWidth, (float)dwTerrainMapLength, 0.0f);
	}

	// Reinitialize particles if required, i.e. first frame or key pressed
	if (bReinitializeParticles || nFrame==0) {
		
		InitializeParticlesTextures();
		bReinitializeParticles = false;
		nFrame = 0; // Reset frame number
	}

	// Modify terrain render target based on mouse interaction (i.e. adding hills or craters)
	if (bIncreaseTerrain || bDecreaseTerrain) {

		// First we need to determine the intersection of the mouse ray with the terrain
		// This is done by running a shader on a 1x1 render target; the result of the render is the coordinates of the intersection point

		// Set render target
		IRenderer::GetRendererInstance()->changeRenderTarget( m_pRayIntersectionSurface, m_pDummyDepthSurface );
		//IRenderer::GetRendererInstance()->SetDepthStencilSurface(m_pDummyDepthSurface);
		
		// Set up the viewport for our 1x1 render target
		D3DFROMWINEVIEWPORT9 RayIntersectionViewport;
		RayIntersectionViewport.X = 0;
		RayIntersectionViewport.Y = 0;
		RayIntersectionViewport.Width = 1;
		RayIntersectionViewport.Height = 1;
		RayIntersectionViewport.MinZ = 0.0f;
		RayIntersectionViewport.MaxZ = 1.0f;
		IRenderer::GetRendererInstance()->setViewport( &RayIntersectionViewport );

		// Set shader and constants
		IRenderer::GetRendererInstance()->setShader( m_rayintersectionShader );
		// Set vertex declaration
		IRenderer::GetRendererInstance()->setVertexFormat(m_rayintersectionVF);
		IRenderer::GetRendererInstance()->setCullFace( eBACK );
		
		IRenderer::GetRendererInstance()->setShaderConstant4f( "RayStartingPoint", (const D3DXFROMWINEVECTOR4 &)MouseRayCast[0]);
		D3DXFROMWINEVECTOR4 DirectionVector4 = MouseRayCast[1] - MouseRayCast[0];
		D3DXFROMWINEVECTOR3 DirectionVector(DirectionVector4.x, DirectionVector4.y, DirectionVector4.z);
		D3DXFROMWINEVec3Normalize(&DirectionVector, &DirectionVector);
		// Scale with unit length (unit corresponding to one pixel gap)
		float fMaxRadius = min(fTotalTerrainWidth/dwTerrainMapWidth, fTotalTerrainLength/dwTerrainMapLength);
		DirectionVector *= fMaxRadius / sqrt(DirectionVector.x*DirectionVector.x + DirectionVector.z*DirectionVector.z);
		IRenderer::GetRendererInstance()->setShaderConstant3f( "RayDirectionVector", (const D3DXFROMWINEVECTOR3 &)DirectionVector);
		IRenderer::GetRendererInstance()->setShaderConstant2f( "RcpTotalTerrainDimensions", D3DXFROMWINEVECTOR2(1.0f/fTotalTerrainWidth, 1.0f/fTotalTerrainLength));
		IRenderer::GetRendererInstance()->setShaderConstant2f( "HeightMapSize", D3DXFROMWINEVECTOR2((float)dwTerrainMapWidth, (float)dwTerrainMapLength));
		IRenderer::GetRendererInstance()->setShaderConstant2f( "TexelSize", D3DXFROMWINEVECTOR2(1.0f/dwTerrainMapWidth, 1.0f/dwTerrainMapLength));
		IRenderer::GetRendererInstance()->setShaderConstant2f( "TerrainUnitSize", D3DXFROMWINEVECTOR2(fTotalTerrainWidth/dwTerrainMapWidth, fTotalTerrainLength/dwTerrainMapLength));

		// Z off
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, FALSE);

		// Blending off
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		// Set terrain as texture input
		IRenderer::GetRendererInstance()->setTexture("Sampler0", m_pRenderTargetTexture);

		// Render a single pixel
		IRenderer::GetRendererInstance()->setShader(m_pPositionTexcoordVertexShader);IRenderer::GetRendererInstance()->setVertexFormat(m_pPositionTexcoordVertexDeclaration);
		if (!DrawQuad( 0.0f, 0.0f, 0.0f, 1.0f, 1.0f)) {	
			LOG_PRINT("Failed to render single pixel\n");
		}

		// At this stage the coordinates of the mouse pick intersection with terrain geometry
		// is contained in the 1x1 m_pRayIntersectionTexture texture

		// Set render target to terrain surface
		IRenderer::GetRendererInstance()->changeRenderTarget( m_pRenderTargetSurface, m_pDummyDepthSurface );
		//IRenderer::GetRendererInstance()->SetDepthStencilSurface(m_pDummyDepthSurface);

		// Restore viewport
		IRenderer::GetRendererInstance()->setViewport( &RenderTargetViewport );

		// Use fixed-function vertex processing
		//IRenderer::GetRendererInstance()->SetVertexShader(0);

		// Only write to Green channel (height data in render target) leaving other channels (X, Z and W position) untouched
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_GREEN);

		// Set sprite texture
		IRenderer::GetRendererInstance()->setTexture(0, (HillSprite));

		// Enable alpha blending
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// Use fixed-function pixel processing
		//IRenderer::GetRendererInstance()->SetPixelShader(0);

		// Change to additive or subtractive blending mode based on user action
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_BLENDOP, bDecreaseTerrain ? D3DBLENDOP_REVSUBTRACT : D3DBLENDOP_ADD);

		// Enable point sprites
		float fSize = 20.0f * (dwTerrainMapWidth / 256.0f);
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_POINTSIZE, *(__DWORD__ *)&fSize);

		// Set stream source to dummy VB (used to specify offset and stride: offset = 0, stride = 4*sizeof(unsigned short))
		IRenderer::GetRendererInstance()->setVertexBuffer(0, m_pDummyVB);

		// Bind the texture to the DMAP sampler
		IRenderer::GetRendererInstance()->setTexture("DMAP", m_pRayIntersectionTexture);

		// Render single particle (this particle will be rendered at the terrain intersection with the mouse ray)
		IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_POINTS, 0, 1);

		// Restore color writes
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

		// Disable alpha blending
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		// Restore Z testing
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, TRUE);
	}

	// Revert to default render target
	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
	IRenderer::GetRendererInstance()->setViewport( &MainViewport );

	// Clear render target
	IRenderer::GetRendererInstance()->Clear(true, true, 0xFFB0B0B0);

	//
	// Main terrain view
	//

	// Set shader
	IRenderer::GetRendererInstance()->setShader( m_sceneShader );
	// Set vertex declaration
	IRenderer::GetRendererInstance()->setVertexFormat(m_sceneVF);
	IRenderer::GetRendererInstance()->setTexture("TerrainDiffuseTexture", TerrainDiffuseTexture);
	IRenderer::GetRendererInstance()->setCullFace( eBACK );
	IRenderer::GetRendererInstance()->setDepthFunc( eLEQUAL );
	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f( "WorldTransformMatrix", m_modelMvp );

	// Disable alpha blending
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// Override filtering modes set by framework
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	//IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// Set stream source to dummy VB (used to specify offset and stride)
	IRenderer::GetRendererInstance()->setVertexBuffer(0, m_pDummyVB);

	// Set index buffer
	IRenderer::GetRendererInstance()->setIndexBuffer(m_pTerrainIndexBuffer);

	// Bind the render target to the DMAP sampler
	IRenderer::GetRendererInstance()->setTexture("DMAP", m_pRenderTargetTexture);

	// Turn Z testing on
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, TRUE);

	// Draw the terrain
	IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, (dwTerrainMapWidth)*(dwTerrainMapLength), 0, (dwTerrainMapWidth-1)*(dwTerrainMapLength-1)*2);

	// Draw the terrain in wireframe if requested
	if (DemoMenu && DemoMenu->isItemChecked(ToggleWireframe)) {
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		IRenderer::GetRendererInstance()->setShader( m_scenewireframeShader );
	// Set vertex declaration
		IRenderer::GetRendererInstance()->setVertexFormat(m_scenewireframeVF);
		IRenderer::GetRendererInstance()->setDepthFunc( eLEQUAL );
		
		IRenderer::GetRendererInstance()->setShaderConstant4x4f( "WorldTransformMatrix", m_modelMvp );
		IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, (dwTerrainMapWidth)*(dwTerrainMapLength), 0, (dwTerrainMapWidth-1)*(dwTerrainMapLength-1)*2);
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	//
	// Render sky-box (after terrain for best performance)
	//
	DrawSkybox();

	//
	// Calculate new particles position
	//

	// Set render targets
	//IRenderer::GetRendererInstance()->changeRenderTarget( RT_USERDEFINED, RT_USERDEFINED );
	IRenderer::GetRendererInstance()->changeRenderTarget( /* 0, */ m_pParticlesPositionSurface[(nFrame+1)%2], m_pDummyDepthSurface );
	IRenderer::GetRendererInstance()->changeRenderTarget( /* 1, */ m_pParticlesVelocitySurface[(nFrame+1)%2], m_pDummyDepthSurface );
	//IRenderer::GetRendererInstance()->SetDepthStencilSurface(m_pDummyDepthSurface);

	// Set up the viewport for our particles render
	RenderTargetViewport.X = 0;
	RenderTargetViewport.Y = 0;
	RenderTargetViewport.Width = PARTICLES_TEXTURE_SIZE;
	RenderTargetViewport.Height = PARTICLES_TEXTURE_SIZE;
	RenderTargetViewport.MinZ = 0.0f;
	RenderTargetViewport.MaxZ = 1.0f;
	IRenderer::GetRendererInstance()->setViewport( &RenderTargetViewport );

	// Turn Z testing off
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, FALSE);

	// Set shader
	IRenderer::GetRendererInstance()->setShader( m_particlesmathShader);
	IRenderer::GetRendererInstance()->setCullFace( eBACK );
	
	IRenderer::GetRendererInstance()->setShaderConstant4f( "ClampedTerrainDimensions", D3DXFROMWINEVECTOR4(0.9f*fTotalTerrainWidth, 0.9f*fTotalTerrainLength, fTotalTerrainWidth/2.0f, fTotalTerrainLength/2.0f));
	IRenderer::GetRendererInstance()->setShaderConstant2f( "RcpTotalTerrainDimensions", D3DXFROMWINEVECTOR2(1.0f/fTotalTerrainWidth, 1.0f/fTotalTerrainLength));
	IRenderer::GetRendererInstance()->setShaderConstant2f( "HeightMapSize", D3DXFROMWINEVECTOR2((float)dwTerrainMapWidth, (float)dwTerrainMapLength));
	IRenderer::GetRendererInstance()->setShaderConstant2f( "RcpParticlesMapSize", D3DXFROMWINEVECTOR2(1.0f/(PARTICLES_TEXTURE_SIZE-1.0f), 1.0f/(PARTICLES_TEXTURE_SIZE-1.0f)));
	IRenderer::GetRendererInstance()->setShaderConstant2f( "TexelSize", D3DXFROMWINEVECTOR2(1.0f/dwTerrainMapWidth, 1.0f/dwTerrainMapLength));
	IRenderer::GetRendererInstance()->setShaderConstant2f( "TerrainUnitSize", D3DXFROMWINEVECTOR2(fTotalTerrainWidth/dwTerrainMapWidth, fTotalTerrainLength/dwTerrainMapLength));
	if(SnowFlakeLife)
		IRenderer::GetRendererInstance()->setShaderConstant1f( "SnowFlakeLife", FRAND(2.0f) + SnowFlakeLife->getValue());
	if(BounceFactor)
		IRenderer::GetRendererInstance()->setShaderConstant1f( "BounceFactor", BounceFactor->getValue());

	if (bStepTime) {
		if (bStep) fTimeElapsed = 1.0f/60.0f;
			else fTimeElapsed = 0.0f;
		bStep = false;
	}
	else {
		fTimeElapsed = 1000.0f*timeGetTime();
	}
	IRenderer::GetRendererInstance()->setShaderConstant1f( "fTimeElapsed", fTimeElapsed);

	// Set texture inputs
	IRenderer::GetRendererInstance()->setTexture("Sampler0", m_pParticlesPosition[(nFrame%2)]);
	IRenderer::GetRendererInstance()->setTexture("Sampler1", m_pParticlesVelocity[(nFrame%2)]);
	IRenderer::GetRendererInstance()->setTexture("Sampler2", m_pRenderTargetTexture);
	IRenderer::GetRendererInstance()->setTexture("Sampler3", (NoiseTexture));

	// Override filtering modes set by framework
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	//IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	//IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//IRenderer::GetRendererInstance()->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	//IRenderer::GetRendererInstance()->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	//IRenderer::GetRendererInstance()->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//IRenderer::GetRendererInstance()->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	//IRenderer::GetRendererInstance()->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	//IRenderer::GetRendererInstance()->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	//IRenderer::GetRendererInstance()->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//IRenderer::GetRendererInstance()->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	//IRenderer::GetRendererInstance()->SetSamplerState( 3, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	// Draw quad
	IRenderer::GetRendererInstance()->setShader(m_pPositionTexcoordVertexShader);IRenderer::GetRendererInstance()->setVertexFormat(m_pPositionTexcoordVertexDeclaration);DrawScreenAlignedQuad( PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, 0.0f);

	//
	// Render particles
	//

	// Revert to default render target
	//IRenderer::GetRendererInstance()->changeRenderTarget( 1, 0 );
	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
	IRenderer::GetRendererInstance()->setViewport( &MainViewport );

	// Set shader
	IRenderer::GetRendererInstance()->setShader( m_particlesrenderShader );
	// Set vertex declaration
	IRenderer::GetRendererInstance()->setVertexFormat(m_particlesrenderVF);
	IRenderer::GetRendererInstance()->setTexture("SnowFlakeTexture", SnowFlakeTexture);
	IRenderer::GetRendererInstance()->setCullFace( eBACK );
	IRenderer::GetRendererInstance()->setDepthFunc( eLEQUAL );
	IRenderer::GetRendererInstance()->setBlending(eSRC_ALPHA, eONE);
	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f( "WorldTransformMatrix", m_modelMvp * translate(0, 1.4f, 0));
	IRenderer::GetRendererInstance()->setShaderConstant3f( "CameraPosition", camPos);
	IRenderer::GetRendererInstance()->setShaderConstant1f( "PointSizeFactor", height * 4.3f);

	// Override filtering modes set by framework
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	// Set stream source to dummy VB (used to specify offset and stride)
	IRenderer::GetRendererInstance()->setVertexBuffer(0, m_pDummyVB);

	// Bind the texture VB to the DMAP sampler
	IRenderer::GetRendererInstance()->setTexture("DMAP", m_pParticlesPosition[nFrame%2]);

	// Enable blending and turn Z-writes off
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, TRUE);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// Draw the particles
	IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_POINTS, 0, NUMBER_OF_PARTICLES);

	// Disable point sprites
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);


	// Render overlay
	if (DemoMenu && DemoMenu->isItemChecked(Display2DView)) {

		// Set shader and texture
		IRenderer::GetRendererInstance()->setShader( m_overlayShader );
		// Set vertex declaration
		IRenderer::GetRendererInstance()->setVertexFormat(m_overlayVF);
		IRenderer::GetRendererInstance()->setCullFace( eCULL_NONE );
		IRenderer::GetRendererInstance()->setTexture("Sampler0", RenderTargetTexture);
		

		// Override filtering modes set by framework
		//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		//IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

		// Set blending
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		// Create an overlay quad model
		const float overlayData[] = {
			4.5f,                4.5f + OVERLAY_SIZE, 0.0f, 1.0f, 0.0f, 0.0f,
			4.5f + OVERLAY_SIZE, 4.5f + OVERLAY_SIZE, 0.0f, 1.0f, 1.0f, 0.0f,
			4.5f,                4.5f,                0.0f, 1.0f, 0.0f, 1.0f,
			4.5f + OVERLAY_SIZE, 4.5f,                0.0f, 1.0f, 1.0f, 1.0f
		};
		
		// Cull back faces
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

		// Use fixed-function vertex processing
		// ??? IRenderer::GetRendererInstance()->SetVertexShader(0);

		// Draw quad
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, overlayData, overlayData, 6*sizeof(float));
	}

	// Increase frame number
	nFrame++;
}

//---------------------------------------------------------------------------
// App update transforms
//
//---------------------------------------------------------------------------
void updateTransforms()
{
	// Create view matrix and convert it to column-major
	D3DXFROMWINEMATRIX modelview = rotateXY4(-wx, -wy);
	modelview.translate(-camPos);
	matView = (D3DXFROMWINEMATRIX)modelview;
	D3DXFROMWINEMatrixTranspose(&matView, &matView);

	// Create MVP matrix
	D3DXFROMWINEMatrixMultiply((D3DXFROMWINEMATRIX*)&m_modelMvp, &matView, &matProj);
	D3DXFROMWINEMatrixTranspose((D3DXFROMWINEMATRIX*)&m_modelMvp, (D3DXFROMWINEMATRIX*)&m_modelMvp);
}


//-----------------------------------------------------------------------------
// Name: DrawScreenAlignedQuad()
// Desc: Simple function to render a screen-aligned quad
//-----------------------------------------------------------------------------
bool DrawScreenAlignedQuad(float fWidth, float fHeight, float fZ)
{
	int	hres;

	// Create screen-aligned quad geometry
	float quadVertices[] = {
		- 0.5f,				-0.5f,				fZ, 1.0f, 0.0f, 0.0f,  // x y z w tu tv
		- 0.5f + fWidth,	-0.5f,				fZ, 1.0f, 1.0f, 0.0f,
		- 0.5f,				-0.5f + fHeight,	fZ, 1.0f, 0.0f, 1.0f,
		- 0.5f + fWidth,	-0.5f + fHeight,	fZ, 1.0f, 1.0f, 1.0f
	};
	// Set vertex declaration
	

	// Cull back faces
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	IRenderer::GetRendererInstance()->setCullFace(eCULL_CCW);

	// Use fixed-function vertex processing
	// ??? IRenderer::GetRendererInstance()->SetVertexShader(0);

	// Draw quad
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, quadVertices, quadVertices, 6*sizeof(float));

	return 0;
}


//-----------------------------------------------------------------------------
// Name: DrawQuad()
// Desc: Simple function to render a quad in screen coordinates
//       fX, fX are the coordinates of the quad's CENTER
//-----------------------------------------------------------------------------
bool DrawQuad(float fX, float fY, float fZ, float fWidth, float fHeight)
{
	int	hres;

	// Create fullscreen quad geometry (displace 
	float quadVertices[] = {
		fX - fWidth/2.0f, fY - fHeight/2.0f, fZ, 1.0f, 0.0f, 0.0f,  // x y z w tu tv
		fX + fWidth/2.0f, fY - fHeight/2.0f, fZ, 1.0f, 1.0f, 0.0f,
		fX - fWidth/2.0f, fY + fHeight/2.0f, fZ, 1.0f, 0.0f, 1.0f,
		fX + fWidth/2.0f, fY + fHeight/2.0f, fZ, 1.0f, 1.0f, 1.0f
	};
	// Set vertex declaration
	
	
	// Cull back faces
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	IRenderer::GetRendererInstance()->setCullFace(eCULL_CCW);

	// Use fixed-function vertex processing
	// ??? IRenderer::GetRendererInstance()->SetVertexShader(0);

	// Draw quad
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, quadVertices, quadVertices, 6*sizeof(float));

	return 0;
}

//-----------------------------------------------------------------------------
// Name: InitializeParticlesTextures()
// Desc: Initialize the first particle texture with startup data
//-----------------------------------------------------------------------------
void InitializeParticlesTextures()
{
	TextureID m_pTempTexture;
	D3DFROMWINEVIEWPORT9		RenderTargetViewport;
	int				hres;

	// The only way to initialize a render target with contents is to render into it
	// Hence we create a temporary Lock()able texture so that we can render its contents onto our render targets
	Image3 img;
	img.create(FORMAT_RGBA16f, PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, 1, 0);
	m_pTempTexture=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());

	// Set render states
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, FALSE);
	//IRenderer::GetRendererInstance()->SetPixelShader(0);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// Prepare the viewport for our particles render
	RenderTargetViewport.X = 0;
	RenderTargetViewport.Y = 0;
	RenderTargetViewport.Width = PARTICLES_TEXTURE_SIZE;
	RenderTargetViewport.Height = PARTICLES_TEXTURE_SIZE;
	RenderTargetViewport.MinZ = 0.0f;
	RenderTargetViewport.MaxZ = 1.0f;

	// Set texture input
	IRenderer::GetRendererInstance()->setTexture(0, m_pTempTexture);

	//
	// Position texture
	//
	BYTE* pBits=0;
        int Pitch=0;
#if 0 // ???
	// Lock texture
        pBits=IRenderer::GetRendererInstance()->LockTexture(m_pTempTexture, 0, Pitch);
	// Fill texture with data
	for (int i=0; i<PARTICLES_TEXTURE_SIZE; i++) {
		unsigned short *pusHalf = (unsigned short *)((char *)pBits + i*Pitch);
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
	IRenderer::GetRendererInstance()->UnlockTexture(m_pTempTexture, 0);
#endif
	// Then we manually set the render target and depth-stencil surface
	IRenderer::GetRendererInstance()->changeRenderTarget( m_pParticlesPositionSurface[0], m_pDummyDepthSurface );
	//IRenderer::GetRendererInstance()->SetDepthStencilSurface(m_pDummyDepthSurface);
	IRenderer::GetRendererInstance()->setViewport( &RenderTargetViewport );

	// Draw quad
	IRenderer::GetRendererInstance()->setShader(m_pPositionTexcoordVertexShader);IRenderer::GetRendererInstance()->setVertexFormat(m_pPositionTexcoordVertexDeclaration);DrawScreenAlignedQuad( PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, 0.0f);

	//
	// Velocity texture
	//
#if 0 // ???
	// Lock texture
        pBits=IRenderer::GetRendererInstance()->LockTexture(m_pTempTexture, 0, Pitch);
	// Fill texture with data (initial velocity is 0)
	for (int i=0; i<PARTICLES_TEXTURE_SIZE; i++) {
		unsigned short *pusHalf = (unsigned short *)((char *)pBits + i*Pitch);
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
	IRenderer::GetRendererInstance()->UnlockTexture(m_pTempTexture, 0);
#endif
	// Then we manually set the render target and depth-stencil surface
	IRenderer::GetRendererInstance()->changeRenderTarget( m_pParticlesVelocitySurface[0], m_pDummyDepthSurface );
	//IRenderer::GetRendererInstance()->SetDepthStencilSurface(m_pDummyDepthSurface);
	IRenderer::GetRendererInstance()->setViewport( &RenderTargetViewport );

	// Draw quad
	//IRenderer::GetRendererInstance()->SetPixelShader(0);
	IRenderer::GetRendererInstance()->setShader(m_pPositionTexcoordVertexShader);IRenderer::GetRendererInstance()->setVertexFormat(m_pPositionTexcoordVertexDeclaration);DrawScreenAlignedQuad( PARTICLES_TEXTURE_SIZE, PARTICLES_TEXTURE_SIZE, 0.0f);

	// Turn Z testing back on
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ZENABLE, TRUE);

	// Release temp texture
	//m_pTempTexture->Release();
}


//-----------------------------------------------------------------------------
// Name: DrawSkybox()
// Desc: Draw a skybox as a cube centered around the camera position
//-----------------------------------------------------------------------------
void DrawSkybox()
{
	#if 0
	static const float verts[] = { -1,-1,-1, -1,-1,1, -1,1,-1, -1,1,1, 1,-1,-1, 1,-1,1, 1,1,-1, 1,1,1 };
	static const ushort inds[] = { 2,3,0, 0,3,1, 3,7,1, 1,7,5, 7,6,5, 5,6,4, 6,2,4, 4,2,0, 6,7,2, 2,7,3, 0,1,4, 4,1,5 };

	IRenderer::GetRendererInstance()->setShader(m_skyboxShader);
	IRenderer::GetRendererInstance()->setVertexFormat(skyboxVF);
	IRenderer::GetRendererInstance()->setTexture("Skybox", SkyboxTexture);
	IRenderer::GetRendererInstance()->setCullFace( eBACK );
	IRenderer::GetRendererInstance()->setMask(eCOLOR);
	

	// Get mouse rotation and convert it into a column-major matrix
	D3DXFROMWINEMATRIX	matSkybox;
	D3DXFROMWINEMATRIX		modelview = rotateXY4(-wx, -wy);
	D3DXFROMWINEMatrixTranspose(&matSkybox, (D3DXFROMWINEMATRIX *)&modelview);

	// Multiply with projection matrix and send as constant
	D3DXFROMWINEMatrixMultiplyTranspose(&matSkybox, &matSkybox, &matProj);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", pegToFarPlane((D3DXFROMWINEMATRIX &) matSkybox));

	// Draw skybox
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, 8, 12, inds, inds, CONSTANT_INDEX2, verts, verts, 3 * sizeof(float));
	#else
	sb.render();
	#endif
}
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
	IRenderer::GetRendererInstance()->BeginScene();
	app.drawFrame();
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present();
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("R2VB-Collision");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

