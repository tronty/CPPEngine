/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <Math/Perlin.h>
#include <Util/ProgrammerArt.h>

#define ROWS_PER_DIP 64
#define MAX_WIDTH 1024

#if (ROWS_PER_DIP * MAX_WIDTH <= 65536)
#define INDEX_TYPE ushort
#else
#define INDEX_TYPE uint
#endif

class App {
public:
    float width;
    float height;
    D3DXFROMWINEVECTOR3 camPos;
    float wx;
    float wy;
    float speed;
    bool debugRuntime;

void setup(){
	width  = 800;
	height = 600;

	initPerlin();
}

void resetCamera(){
	camPos = D3DXFROMWINEVECTOR3(0, 50, -100);
	wx = 0;
	wy = 0;
}
#ifdef R2VB_GUI
void initGUI(){
	// Add widgets to control particle count and size
	BaseApp::addWidget(scLabel = new Label(709, 8, BaseApp::getMainFont(), "Size Count", 19, 20));
	BaseApp::addWidget(particleSize  = new Slider(710, 30, 30, 200, false, 0, 1, 0.2f));
	BaseApp::addWidget(particleCount = new Slider(760, 30, 30, 200, false, 0, 1, 0.5f));

	BaseApp::addWidget(xzLabel = new Label(20, 524, BaseApp::getMainFont(), "XZ", 20, 26, 0, 0, HA_CENTER));
	BaseApp::addWidget(yLabel  = new Label(20, 564, BaseApp::getMainFont(), "Y",  20, 26, 0, 0, HA_CENTER));
	BaseApp::addWidget(xzPower = new Slider(40, 520, 200, 30, true, 0.0f, 1.5f, 1));
	BaseApp::addWidget(yPower  = new Slider(40, 560, 200, 30, true, 0.0f, 1.5f, 1));

	BaseApp::addWidget(countLabel = new Label(790, 564, BaseApp::getMainFont(), "", 20, 26, 0, 0, HA_RIGHT));

	BaseApp::initGUI();
}
#endif
bool load(){

	// Check whether half floats are supported in vertex declarations (R300 doesn't support this).
	// Since we don't need full float precision we'll use half float for performance on boards that support it.
	bool supportsHalf = false;// IRenderer::GetRendererInstance()->supportsHalf();

	// Load shaders
	particleShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ParticleSystem/particle.hlsl", "main", "main");
	physics        = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ParticleSystem/physics.hlsl", "main", "main");
	initPhysics    = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ParticleSystem/initPhysics.hlsl", "main", "main");
	//skybox         = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ParticleSystem/skybox.hlsl", "main", "main");
	
	floorShader    = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ParticleSystem/floor.hlsl", "main", "main");
	makeQuads      = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ParticleSystem/makeQuads.hlsl", "main", "main");

	// Load vertex attributes
	FormatDesc particleAttribs[] = {
		0, TYPE_VERTEX,   supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 4,
		1, TYPE_TEXCOORD, supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 2,
	};
	if ((particleVF = IRenderer::GetRendererInstance()->addVertexFormat(particleAttribs, elementsOf(particleAttribs), particleShader)) == TEXTURE_NONE) return false;
	FormatDesc physicsAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((physicsVF = IRenderer::GetRendererInstance()->addVertexFormat(physicsAttribs, elementsOf(physicsAttribs), physics)) == TEXTURE_NONE) return false;
	if ((initPhysicsVF = IRenderer::GetRendererInstance()->addVertexFormat(physicsAttribs, elementsOf(physicsAttribs), physics)) == TEXTURE_NONE) return false;
	if ((makeQuadsVF = IRenderer::GetRendererInstance()->addVertexFormat(physicsAttribs, elementsOf(physicsAttribs), makeQuads)) == TEXTURE_NONE) return false;

	//FormatDesc skyboxAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	//if ((skyboxVF = IRenderer::GetRendererInstance()->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs), -1)) == VF_NONE) return false;

	// Create a texture that can hold a million particles.
	// We need another render target with 4x the width, so make sure we can fit it within the GPU's maximum texture size.
	rtWidth  = min(int(IRenderer::GetRendererInstance()->GetMaxTextureWidth()) / 4, MAX_WIDTH);
	rtHeight = min(1048576 / rtWidth, int(IRenderer::GetRendererInstance()->GetMaxTextureHeight()));

	/*
		Create render targets for position and direction.
		We need two of each so we can ping-pong between a current state and the new one.
	*/
	rtPos[0] = IRenderer::GetRendererInstance()->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());
	rtPos[1] = IRenderer::GetRendererInstance()->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());
	rtDir[0] = IRenderer::GetRendererInstance()->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());
	rtDir[1] = IRenderer::GetRendererInstance()->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());

	/*
		Since this render target will be used as a vertex buffer we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	r2vbRT = IRenderer::GetRendererInstance()->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16F, IRenderer::GetRendererInstance()->GetnearestClamp());

	// Load textures
	#if 0
	env = IRenderer::GetRendererInstance()->addCubemap("Mountains", "jpg");
	#else
    	sb.init("/SkyBox/cubemaps/x/MountainsGL.dds");
	#endif

	floorTex=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/bump_normal/fieldstone.dds", false, IRenderer::GetRendererInstance()->GettrilinearAniso());
	particle=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/particle.dds", false, IRenderer::GetRendererInstance()->Getbilinear());
	palette=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/sparkPalette.dds", false, IRenderer::GetRendererInstance()->Gettrilinear());

	// Create an index buffer for the particles. We're using quads.
	std::vector<INDEX_TYPE> indices_(6 * rtWidth * ROWS_PER_DIP);
	INDEX_TYPE *indices = &indices_[0];
	for (int i = 0; i < rtWidth * ROWS_PER_DIP; i++){
		indices[6 * i    ] = 4 * i;
		indices[6 * i + 1] = 4 * i + 1;
		indices[6 * i + 2] = 4 * i + 2;
		indices[6 * i + 3] = 4 * i;
		indices[6 * i + 4] = 4 * i + 2;
		indices[6 * i + 5] = 4 * i + 3;
	}
	particleIndices = IRenderer::GetRendererInstance()->addIndexBuffer(indices_.size(), 2, STATIC, &indices_[0]);

	/*
		We need a dummy vertex buffer when we specify stride and offset when calling setVertexBuffer()
		for R2VB. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
	int size = rtWidth * rtHeight * 6;
	std::vector<float> quadCoord(size);
	for (int i = 0; i < rtWidth * rtHeight; i++){
		quadCoord[8 * i + 0] = 0;
		quadCoord[8 * i + 1] = 0;
		quadCoord[8 * i + 2] = 65535;
		quadCoord[8 * i + 3] = 0;
		quadCoord[8 * i + 4] = 65535;
		quadCoord[8 * i + 5] = 65535;
		quadCoord[8 * i + 6] = 0;
		quadCoord[8 * i + 7] = 65535;
	}
	quadCoordVB = IRenderer::GetRendererInstance()->addVertexBuffer(size, STATIC, &quadCoord[0]);

    // Create a simple disc as our floor
	floor = createDisc(32, 1, 400, XZ_PLANE);
	if (!floor->makeDrawable()) return false;


	// RAND_MAX = 32767 in MSVC
#define rand16() (stx_rand() ^ (stx_rand() << 1))

	// Create a texture containing random positions within a unit sphere
	Image3 img;
	ushort *pixels = (ushort *) img.create(FORMAT_RGBA16, 128, 128, 1, 1);
	for (int i = 0; i < 128 * 128; i++){
		ushort x, y, z;
		float d;
		do {
			x = rand16();
			y = rand16();
			z = rand16();

			float dx = (x / 65535.0f) * 2 - 1;
			float dy = (y / 65535.0f) * 2 - 1;
			float dz = (z / 65535.0f) * 2 - 1;

			d = dx * dx + dy * dy + dz * dz;
		} while (d > 1.0f);

		pixels[4 * i + 0] = x;
		pixels[4 * i + 1] = y;
		pixels[4 * i + 2] = z;
		pixels[4 * i + 3] = rand16();
	}

	randTex = IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->GetnearestClamp());

	//onReset();

	return true;
}

bool unload(){
	delete floor;
	return true;
}

bool onReset(){
	IRenderer::GetRendererInstance()->BeginScene();

	IRenderer::GetRendererInstance()->setShader(initPhysics);
	IRenderer::GetRendererInstance()->setVertexFormat(initPhysicsVF);
	IRenderer::GetRendererInstance()->setTexture("Rand", randTex);
	IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / rtWidth, 0.5f / rtHeight));
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	

	const D3DXFROMWINEVECTOR2 vertices[] = { D3DXFROMWINEVECTOR2(-1, -1), D3DXFROMWINEVECTOR2( 1, -1), D3DXFROMWINEVECTOR2( 1,  1), D3DXFROMWINEVECTOR2(-1,  1) };

	// Initialize the render targets
	TextureID rt0[] = { rtPos[0], rtDir[0] };
	IRenderer::GetRendererInstance()->changeRenderTargets(rt0, elementsOf(rt0));
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

	TextureID rt1[] = { rtPos[1], rtDir[1] };
	IRenderer::GetRendererInstance()->changeRenderTargets(rt1, elementsOf(rt1));
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();

	currRT  = 0;
	otherRT = 1;

	IRenderer::GetRendererInstance()->EndScene();

	return true;
}

void drawSkybox(const D3DXFROMWINEMATRIX &mvp){
	#if 0
	static const float verts[] = { -1,-1,-1, -1,-1,1, -1,1,-1, -1,1,1, 1,-1,-1, 1,-1,1, 1,1,-1, 1,1,1 };
	static const ushort inds[] = { 2,3,0, 0,3,1, 3,7,1, 1,7,5, 7,6,5, 5,6,4, 6,2,4, 4,2,0, 6,7,2, 2,7,3, 0,1,4, 4,1,5 };

	IRenderer::GetRendererInstance()->setShader(skybox);
	IRenderer::GetRendererInstance()->setVertexFormat(skyboxVF);
	IRenderer::GetRendererInstance()->setTexture("Skybox", env);
	IRenderer::GetRendererInstance()->setMask(eCOLOR);
	

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);

	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, 8, 12, inds, inds, CONSTANT_INDEX2, verts, verts, 3 * sizeof(float));
	#else
        sb.render();
	#endif
}

void drawFrame(){
	D3DXFROMWINEMATRIX projection = perspectiveMatrixX(D3DXFROMWINE_PI / 2, width, height, 1, 2000, true);

	D3DXFROMWINEMATRIX modelview = rotateXY4(-wx, -wy);
	D3DXFROMWINEMATRIX mvpSB = pegToFarPlane(projection * modelview);
	modelview.translate(-camPos);
	D3DXFROMWINEMATRIX mvp = projection * modelview;

	// Compute how many rows in the render target we need depending on the particle count the user selects.
#if 0
	int activeRows = (int) (particleCount->getValue() * rtHeight + 0.5f);
#else
	int activeRows = (int) (0.5f * rtHeight + 0.5f);
#endif
	float b = 1.0f - 2.0f * activeRows / rtHeight;

	const D3DXFROMWINEVECTOR2 vertices[] = { D3DXFROMWINEVECTOR2(-1, b), D3DXFROMWINEVECTOR2(1, b), D3DXFROMWINEVECTOR2( 1,  1), D3DXFROMWINEVECTOR2(-1,  1) };

	TextureID rt[] = { rtPos[currRT], rtDir[currRT] };
	IRenderer::GetRendererInstance()->changeRenderTargets(rt, elementsOf(rt));
		/*
			Main particle physics pass
		*/

		static float t = -2;
		float ft = min(1000.0f*timeGetTime(), 0.05f);
		t += 2.0f * ft;
		// Use noise to find the direction to sprinkle particle in
		D3DXFROMWINEVECTOR3 sprinkleDir;
		if(xzPower && yPower)
			sprinkleDir = 450 * D3DXFROMWINEVECTOR3(xzPower->getValue() * noise1(t), yPower->getValue() * (noise1(t + 10.42523f) * 0.5f + 0.5f), xzPower->getValue() * noise1(-t));

		IRenderer::GetRendererInstance()->setShader(physics);
		IRenderer::GetRendererInstance()->setVertexFormat(physicsVF);
		IRenderer::GetRendererInstance()->setTexture("Pos", rtPos[otherRT]);
		IRenderer::GetRendererInstance()->setTexture("Dir", rtDir[otherRT]);
		IRenderer::GetRendererInstance()->setTexture("Rand", randTex);
		IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / rtWidth, 0.5f / rtHeight));
		IRenderer::GetRendererInstance()->setShaderConstant3f("sprinkleDir", sprinkleDir);
		IRenderer::GetRendererInstance()->setShaderConstant1f("time", ft);
		

		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

	IRenderer::GetRendererInstance()->changeRenderTarget(r2vbRT);

		/*
			Turn the particles into quads
		*/
		IRenderer::GetRendererInstance()->setShader(makeQuads);
		IRenderer::GetRendererInstance()->setVertexFormat(makeQuadsVF);
		IRenderer::GetRendererInstance()->setTexture("Pos", rtPos[currRT]);
		IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / (4 * rtWidth), 0.5f / rtHeight));
		

		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();




	IRenderer::GetRendererInstance()->Clear(false, true, stx_GetRGBA(0x00000000));

	// Draw the floor
	IRenderer::GetRendererInstance()->setShader(floorShader);
	IRenderer::GetRendererInstance()->setTexture("Base", floorTex);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	

	floor->draw(PRIM_TRIANGLE_STRIP);


	/*
		The skybox and the particle system need not be antialiased, so we
		disable it here. This substancially improves performance for rendering
		the particle system since it contains loads of very tiny quads.
	*/
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);


	// Draw the skybox
	drawSkybox(mvpSB);

	/*
		Draw the particles
	*/
	IRenderer::GetRendererInstance()->setShader(particleShader);
	IRenderer::GetRendererInstance()->setVertexFormat(particleVF);
	IRenderer::GetRendererInstance()->setTexture("Particle", particle);
	IRenderer::GetRendererInstance()->setTexture("Palette", palette);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	if(particleSize)
		IRenderer::GetRendererInstance()->setShaderConstant3f("dx", particleSize->getValue() * modelview.getRightVec());
	if(particleSize)
		IRenderer::GetRendererInstance()->setShaderConstant3f("dy", particleSize->getValue() * modelview.getUpVec());
	IRenderer::GetRendererInstance()->setIndexBuffer(particleIndices);
	IRenderer::GetRendererInstance()->setVertexBuffer(0, quadCoordVB); // Dummy vertex buffer
	//IRenderer::GetRendererInstance()->setVertexBuffer(1, quadCoordVB);
	IRenderer::GetRendererInstance()->setBlending(eONE, eONE);
	IRenderer::GetRendererInstance()->setMask(eCOLOR);

	// Bind the render target to the DMAP sampler
	IRenderer::GetRendererInstance()->setTexture("DMAP", r2vbRT);

	/*
		We could make just one big DIP call, but some cards have a lower primitive count limitation.
		So we split it into a number of draw calls to be friendly with those cards; we're not going
		to be limited by drawcalls anyway. This also allows us to use a smaller index buffer.
	*/
	for (int i = 0; i < activeRows; i += ROWS_PER_DIP){
		int nRows = min(activeRows - i, ROWS_PER_DIP);
		int nQuads = nRows * rtWidth;

		IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 4 * i * rtWidth, 0, 4 * nQuads, 0, 2 * nQuads);
	}

	// Flip render targets
	currRT = otherRT;
	otherRT = 1 - currRT;

	if(countLabel){
	char str[256];
#if 0
	str.appendIntNatural(activeRows * rtWidth);
	str.append(" particles", 10);
#else
	stx_snprintf(str, 256, "%d particles", activeRows * rtWidth);
#endif
	countLabel->setText(str);
	}
	// Enable multisampling again for the GUI
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
}

private:
	VertexFormatID skyboxVF;

	ShaderID skybox, floorShader, particleShader, physics, initPhysics, makeQuads;
	TextureID env, randTex, floorTex, particle, palette;
	stx_SkyBox sb;

	VertexFormatID particleVF, physicsVF, initPhysicsVF, makeQuadsVF;

	VertexBufferID quadCoordVB;
	IndexBufferID particleIndices;

	TextureID rtPos[2], rtDir[2], r2vbRT;
	int currRT, otherRT;
	int rtWidth, rtHeight;

	#if 1//def R2VB_GUI
	Model *floor;
	Slider *particleCount, *particleSize, *yPower, *xzPower;
	Label *countLabel, *xzLabel, *yLabel, *scLabel;
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
	IRenderer::GetRendererInstance()->BeginScene();
	app.drawFrame();
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present();
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("R2VB-ParticleSystem");
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

