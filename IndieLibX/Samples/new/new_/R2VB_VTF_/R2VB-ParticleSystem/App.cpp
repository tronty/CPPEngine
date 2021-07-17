//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#include "App.h"
#include "../../Framework/Math/Perlin.h"

BaseApp *app = new App();

void App::setup(char *args[], int nArguments){
	width  = 800;
	height = 600;

	initPerlin();
}

void App::resetCamera(){
	camPos = float3(0, 50, -100);
	wx = 0;
	wy = 0;
}

void App::initGUI(){
	// Add widgets to control particle count and size
	addWidget(scLabel = new Label(709, 8, &mainFont, "Size Count", 19, 20));
	addWidget(particleSize  = new Slider(710, 30, 30, 200, false, 0, 1, 0.2f));
	addWidget(particleCount = new Slider(760, 30, 30, 200, false, 0, 1, 0.5f));

	addWidget(xzLabel = new Label(20, 524, &mainFont, "XZ", 20, 26, 0, 0, HA_CENTER));
	addWidget(yLabel  = new Label(20, 564, &mainFont, "Y",  20, 26, 0, 0, HA_CENTER));
	addWidget(xzPower = new Slider(40, 520, 200, 30, true, 0.0f, 1.5f, 1));
	addWidget(yPower  = new Slider(40, 560, 200, 30, true, 0.0f, 1.5f, 1));

	addWidget(countLabel = new Label(790, 564, &mainFont, "", 20, 26, 0, 0, HA_RIGHT));

	D3D9App::initGUI();
}

void App::onSize(const int w, const int h){
	// Move the widgets in place
	scLabel->setPosition(w - 91.0f, 8.0f);
	particleSize->setPosition(w - 90.0f, 30.0f);
	particleCount->setPosition(w - 40.0f, 30.0f);

	xzLabel->setPosition(20.0f, h - 76.0f);
	yLabel->setPosition(20.0f, h - 36.0f);
	xzPower->setPosition(40.0f, h - 80.0f);
	yPower->setPosition(40.0f, h - 40.0f);

	countLabel->setPosition(w - 10.0f, h - 36.0f);

	D3D9App::onSize(w, h);
}

bool App::load(){
	// Check for R2VB support
	if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, (D3DFORMAT) FOURCC('R','2','V','B')) != D3D_OK){
		ERRORMSG("No R2VB support");
		return false;
	}

	// Create a texture that can hold a million particles.
	// We need another render target with 4x the width, so make sure we can fit it within the GPU's maximum texture size.
	rtWidth  = min(int(caps.MaxTextureWidth) / 4, MAX_WIDTH);
	rtHeight = min(1048576 / rtWidth, int(caps.MaxTextureHeight));

	/*
		Create render targets for position and direction.
		We need two of each so we can ping-pong between a current state and the new one.
	*/
	if ((rtPos[0] = renderer->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	if ((rtDir[0] = renderer->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	if ((rtPos[1] = renderer->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	if ((rtDir[1] = renderer->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	// Check whether half floats are supported in vertex declarations (R300 doesn't support this).
	// Since we don't need full float precision we'll use half float for performance on boards that support it.
	bool supportsHalf = (caps.DeclTypes & D3DDTCAPS_FLOAT16_4) != 0;

	/*
		Since this render target will be used as a vertex buffer we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	LPDIRECT3DTEXTURE9 d3drt;
	if (dev->CreateTexture(4 * rtWidth, rtHeight, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, supportsHalf? D3DFMT_A16B16G16R16F : D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((r2vbRT = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;



	// Load shaders
	if ((skybox         = renderer->addShader("skybox.shd"     )) == SHADER_NONE) return false;
	if ((floorShader    = renderer->addShader("floor.shd"      )) == SHADER_NONE) return false;
	if ((particleShader = renderer->addShader("particle.shd"   )) == SHADER_NONE) return false;
	if ((physics        = renderer->addShader("physics.shd"    )) == SHADER_NONE) return false;
	if ((initPhysics    = renderer->addShader("initPhysics.shd")) == SHADER_NONE) return false;
	if ((makeQuads      = renderer->addShader("makeQuads.shd"  )) == SHADER_NONE) return false;


	// Load textures
	if ((env = renderer->addCubemap(
		TEX_PATH "Mountains/posx.jpg", TEX_PATH "Mountains/negx.jpg",
		TEX_PATH "Mountains/posy.jpg", TEX_PATH "Mountains/negy.jpg",
		TEX_PATH "Mountains/posz.jpg", TEX_PATH "Mountains/negz.jpg", FILTER_TRILINEAR)) == TEXTURE_NONE) return false;

	if ((floorTex = renderer->addTexture(TEX_PATH "Fieldstone.dds", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	if ((particle = renderer->addTexture(TEX_PATH "Particle.dds", FILTER_BILINEAR, CLAMP)) == TEXTURE_NONE) return false;
	if ((palette = renderer->addTexture(TEX_PATH "sparkPalette.dds", FILTER_LINEAR, CLAMP)) == TEXTURE_NONE) return false;

	// Load vertex attributes
	StreamVertexAttribute particleAttribs[] = {
		0, TYPE_VERTEX,   supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 4,
		1, TYPE_TEXCOORD, FORMAT_USHORT, 2,
	};
	if ((particleVF = renderer->addVertexFormat(particleAttribs, elementsOf(particleAttribs))) == TEXTURE_NONE) return false;

	VertexAttribute physicsAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((physicsVF = renderer->addVertexFormat(physicsAttribs, elementsOf(physicsAttribs))) == TEXTURE_NONE) return false;

	VertexAttribute skyboxAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 3 };
	if ((skyboxVF = renderer->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs))) == VF_NONE) return false;


	// Create an index buffer for the particles. We're using quads.
	if ((particleIndices = renderer->addIndexBuffer(6 * rtWidth * ROWS_PER_DIP, sizeof(INDEX_TYPE))) == IB_NONE) return false;
	INDEX_TYPE *indices = (INDEX_TYPE *) renderer->lockIndexBuffer(particleIndices);
	if (indices == NULL) return false;
	for (int i = 0; i < rtWidth * ROWS_PER_DIP; i++){
		indices[6 * i    ] = 4 * i;
		indices[6 * i + 1] = 4 * i + 1;
		indices[6 * i + 2] = 4 * i + 2;
		indices[6 * i + 3] = 4 * i;
		indices[6 * i + 4] = 4 * i + 2;
		indices[6 * i + 5] = 4 * i + 3;
	}
	renderer->unlockIndexBuffer(particleIndices);


	/*
		We need a dummy vertex buffer when we specify stride and offset when calling SetStreamSource()
		for R2VB. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
	int size = rtWidth * rtHeight * 4 * (debugRuntime? 4 * (supportsHalf? sizeof(half) : sizeof(float)) : sizeof(ushort) * 2);

	if ((quadCoordVB = renderer->addVertexBuffer(size)) == VB_NONE) return false;
	ushort *quadCoord = (ushort *) renderer->lockVertexBuffer(quadCoordVB);
	if (quadCoord == NULL) return false;
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
	renderer->unlockVertexBuffer(quadCoordVB);


    // Create a simple disc as our floor
	floor = createDisc(32, 1, 400, XZ_PLANE);
	if (!floor->makeDrawable(renderer)) return false;


	// RAND_MAX = 32767 in MSVC
#define rand16() (rand() ^ (rand() << 1))

	// Create a texture containing random positions within a unit sphere
	Image img;
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

	if ((randTex = renderer->addTexture(img, FILTER_POINT)) == TEXTURE_NONE) return false;

	onReset();

	return true;
}

bool App::unload(){
	delete floor;
	return true;
}

bool App::onReset(){
	dev->BeginScene();

	renderer->setShader(initPhysics);
	renderer->setTexture("Rand", randTex);
	renderer->setShaderConstant2f("halfPixel", float2(0.5f / rtWidth, 0.5f / rtHeight));
	renderer->setVertexFormat(physicsVF);
	renderer->setDepthFunc(DEPTH_NONE);
	renderer->apply();

	const float2 vertices[] = { float2(-1, -1), float2( 1, -1), float2( 1,  1), float2(-1,  1) };

	// Initialize the render targets
	TextureID rt0[] = { rtPos[0], rtDir[0] };
	renderer->changeRenderTargets(elementsOf(rt0), rt0);
		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

	TextureID rt1[] = { rtPos[1], rtDir[1] };
	renderer->changeRenderTargets(elementsOf(rt1), rt1);
		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

	renderer->resetRenderTarget();

	currRT  = 0;
	otherRT = 1;

	dev->EndScene();

	return true;
}

void App::drawSkybox(const float4x4 &mvp){
	static const float verts[] = { -1,-1,-1, -1,-1,1, -1,1,-1, -1,1,1, 1,-1,-1, 1,-1,1, 1,1,-1, 1,1,1 };
	static const ushort inds[] = { 2,3,0, 0,3,1, 3,7,1, 1,7,5, 7,6,5, 5,6,4, 6,2,4, 4,2,0, 6,7,2, 2,7,3, 0,1,4, 4,1,5 };

	renderer->setShader(skybox);
	renderer->setVertexFormat(skyboxVF);
	renderer->setTexture("Skybox", env);
	renderer->setMask(COLOR);
	renderer->apply();

	renderer->changeShaderConstant4x4f("mvp", mvp);

	dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 12, inds, D3DFMT_INDEX16, verts, 3 * sizeof(float));
}

void App::drawFrame(){
	float4x4 projection = perspectiveMatrixX(PI / 2, width, height, 1, 2000, true);

	float4x4 modelview = rotateXY4(-wx, -wy);
	float4x4 mvpSB = pegToFarPlane(projection * modelview);
	modelview.translate(-camPos);
	float4x4 mvp = projection * modelview;

	// Compute how many rows in the render target we need depending on the particle count the user selects.
	int activeRows = (int) (particleCount->getValue() * rtHeight + 0.5f);
	float b = 1.0f - 2.0f * activeRows / rtHeight;

	const float2 vertices[] = { float2(-1, b), float2(1, b), float2( 1,  1), float2(-1,  1) };

	TextureID rt[] = { rtPos[currRT], rtDir[currRT] };
	renderer->changeRenderTargets(elementsOf(rt), rt);
		/*
			Main particle physics pass
		*/

		static float t = -2;
		float ft = min(frameTime, 0.05f);
		t += 2.0f * ft;
		// Use noise to find the direction to sprinkle particle in
		float3 sprinkleDir = 450 * float3(xzPower->getValue() * noise1(t), yPower->getValue() * (noise1(t + 10.42523f) * 0.5f + 0.5f), xzPower->getValue() * noise1(-t));

		renderer->setShader(physics);
		renderer->setTexture("Pos", rtPos[otherRT]);
		renderer->setTexture("Dir", rtDir[otherRT]);
		renderer->setTexture("Rand", randTex);
		renderer->setShaderConstant2f("halfPixel", float2(0.5f / rtWidth, 0.5f / rtHeight));
		renderer->setShaderConstant3f("sprinkleDir", sprinkleDir);
		renderer->setShaderConstant1f("frameTime", ft);
		renderer->setVertexFormat(physicsVF);
		renderer->apply();

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

	renderer->changeRenderTarget(r2vbRT);

		/*
			Turn the particles into quads
		*/
		renderer->setShader(makeQuads);
		renderer->setTexture("Pos", rtPos[currRT]);
		renderer->setShaderConstant2f("halfPixel", float2(0.5f / (4 * rtWidth), 0.5f / rtHeight));
		renderer->setVertexFormat(physicsVF);
		renderer->apply();

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

	renderer->resetRenderTarget();




	dev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	// Draw the floor
	renderer->setShader(floorShader);
	renderer->setTexture("Base", floorTex);
	renderer->setShaderConstant4x4f("mvp", mvp);
	renderer->apply();

	floor->draw(renderer, TRIANGLE_STRIP);


	/*
		The skybox and the particle system need not be antialiased, so we
		disable it here. This substancially improves performance for rendering
		the particle system since it contains loads of very tiny quads.
	*/
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);


	// Draw the skybox
	drawSkybox(mvpSB);

	/*
		Draw the particles
	*/
	renderer->setShader(particleShader);
	renderer->setTexture("Particle", particle);
	renderer->setTexture("Palette", palette);
	renderer->setShaderConstant4x4f("mvp", mvp);
	renderer->setShaderConstant3f("dx", particleSize->getValue() * modelview.getRightVec());
	renderer->setShaderConstant3f("dy", particleSize->getValue() * modelview.getUpVec());
	renderer->setVertexFormat(particleVF);
	renderer->setIndexBuffer(particleIndices);
	renderer->setVertexBuffer(quadCoordVB, 0); // Dummy vertex buffer
	renderer->setVertexBuffer(quadCoordVB, 1);
	renderer->setBlending(ONE, ONE);
	renderer->setMask(COLOR);
	renderer->apply();


	// Enable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));

	// Bind the render target to the DMAP sampler
	dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(r2vbRT));
	// Tell the driver that stream 0 is to be fetched from the DMAP texture
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DMAP));

	/*
		We could make just one big DIP call, but some cards have a lower primitive count limitation.
		So we split it into a number of draw calls to be friendly with those cards; we're not going
		to be limited by drawcalls anyway. This also allows us to use a smaller index buffer.
	*/
	for (int i = 0; i < activeRows; i += ROWS_PER_DIP){
		int nRows = min(activeRows - i, ROWS_PER_DIP);
		int nQuads = nRows * rtWidth;

		dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 4 * i * rtWidth, 0, 4 * nQuads, 0, 2 * nQuads);
	}

	// Stream 0 restored to regular vertex buffer mode
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DIS));
	// Unbind the DMAP texture
	dev->SetTexture(D3DDMAPSAMPLER, NULL);

	// Disable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));

	// Flip render targets
	currRT = otherRT;
	otherRT = 1 - currRT;

	String str;
	str.appendIntNatural(activeRows * rtWidth);
	str.append(" particles", 10);
	countLabel->setText(str);

	// Enable multisampling again for the GUI
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
}
