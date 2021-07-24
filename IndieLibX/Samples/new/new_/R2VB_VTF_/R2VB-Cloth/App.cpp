//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#include "App.h"

BaseApp *app = new App();

void App::setup(char *args[], int nArguments){
	width  = 960;
	height = 720;
	speed = 3;
}

void App::resetCamera(){
	camPos = float3(0, 0, -2);
	wx = 0;
	wy = 0;
}

void App::initGUI(){
	// Set up menu items
	clothMenu = mainMenu->addMenu("Cloth");
	curtain = clothMenu->addItem("Curtain", MENU_CHECKABLE);
	sail = clothMenu->addItem("Sail", MENU_CHECKABLE | MENU_CHECKED);
	safetyNet = clothMenu->addItem("Safety net", MENU_CHECKABLE);
	roundTable = clothMenu->addItem("Round table", MENU_CHECKABLE);
	squareTable = clothMenu->addItem("Square table", MENU_CHECKABLE);
	squareTable2 = clothMenu->addItem("Square table 2", MENU_CHECKABLE);

	clothMenu->setRadioButtonGroup(curtain, squareTable2, 1);

	multipass = clothMenu->addItem("Multipass", MENU_CHECKABLE);
	separateStreams = clothMenu->addItem("Separate R2VB streams", MENU_CHECKABLE | MENU_CHECKED);
	clothMenu->addSeparatorBefore(multipass);

	// Add sliders for wind control
	addWidget(xLabel = new Label(13, 583, &mainFont, "X", 14, 18, 0, 0, HA_CENTER));
	addWidget(yLabel = new Label(13, 613, &mainFont, "Y", 14, 18, 0, 0, HA_CENTER));
	addWidget(zLabel = new Label(13, 643, &mainFont, "Z", 14, 18, 0, 0, HA_CENTER));
	addWidget(xWind = new Slider(26, 580, 170, 22, true, -10, 10, 4));
	addWidget(yWind = new Slider(26, 610, 170, 22, true, -10, 10, 3));
	addWidget(zWind = new Slider(26, 640, 170, 22, true, -10, 10, -2));

	addWidget(xnLabel = new Label(13, 683, &mainFont, "Nx", 14, 18, 0, 0, HA_CENTER));
	addWidget(ynLabel = new Label(13, 713, &mainFont, "Ny", 14, 18, 0, 0, HA_CENTER));
	addWidget(znLabel = new Label(13, 743, &mainFont, "Nz", 14, 18, 0, 0, HA_CENTER));
	addWidget(xnWind = new Slider(26, 680, 170, 22, true, 0, 15, 5));
	addWidget(ynWind = new Slider(26, 710, 170, 22, true, 0, 15, 5));
	addWidget(znWind = new Slider(26, 740, 170, 22, true, 0, 15, 5));

	D3D9App::initGUI();
}

void App::onMenuItemPress(Menu *menu, MenuItemID item){
	// Handle the presets
	if (menu == clothMenu){
		if (item != separateStreams){
			if (item == curtain){
				xWind->setValue(0);
				yWind->setValue(0);
				zWind->setValue(0);
				xnWind->setValue(10);
				ynWind->setValue(2);
				znWind->setValue(10);
			} else if (item == sail){
				xWind->setValue(4);
				yWind->setValue(3);
				zWind->setValue(-2);
				xnWind->setValue(5);
				ynWind->setValue(5);
				znWind->setValue(5);
			} else if (item == safetyNet){
				xWind->setValue(0);
				yWind->setValue(3);
				zWind->setValue(0);
				xnWind->setValue(5);
				ynWind->setValue(15);
				znWind->setValue(5);
			} else if (item == roundTable){
				xWind->setValue(4);
				yWind->setValue(0);
				zWind->setValue(0);
				xnWind->setValue(8);
				ynWind->setValue(3);
				znWind->setValue(8);
			} else if (item == squareTable){
				xWind->setValue(0);
				yWind->setValue(2);
				zWind->setValue(0);
				xnWind->setValue(8);
				ynWind->setValue(3);
				znWind->setValue(8);
			} else if (item == squareTable2){
				xWind->setValue(0);
				yWind->setValue(2);
				zWind->setValue(0);
				xnWind->setValue(8);
				ynWind->setValue(3);
				znWind->setValue(8);
			}

			needsInit = true;
		}
	} else {
		D3D9App::onMenuItemPress(menu, item);
	}
}

void App::onSize(const int w, const int h){
	// Move the widgets in place
	xLabel->setPosition(13, h - 185.0f);
	yLabel->setPosition(13, h - 155.0f);
	zLabel->setPosition(13, h - 125.0f);

	xWind->setPosition(26, h - 188.0f);
	yWind->setPosition(26, h - 158.0f);
	zWind->setPosition(26, h - 128.0f);

	xnLabel->setPosition(13, h - 85.0f);
	ynLabel->setPosition(13, h - 55.0f);
	znLabel->setPosition(13, h - 25.0f);

	xnWind->setPosition(26, h - 88.0f);
	ynWind->setPosition(26, h - 58.0f);
	znWind->setPosition(26, h - 28.0f);

	D3D9App::onSize(w, h);
}

bool App::load(){
	// Check for R2VB support
	if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, (D3DFORMAT) FOURCC('R','2','V','B')) != D3D_OK){
		ERRORMSG("No R2VB support");
		return false;
	}

	if (caps.PixelShaderVersion < D3DPS_VERSION(2,0)){
		ERRORMSG("No ps2.0 support");
		return false;
	}

	// Check for VS3.0 support.
	if (caps.VertexShaderVersion < D3DVS_VERSION(3,0)){
		clothMenu->checkItem(separateStreams, false);
		clothMenu->enableItem(separateStreams, false);

		if (caps.PS20Caps.NumInstructionSlots < 512){
			clothMenu->checkItem(multipass, true);
			clothMenu->enableItem(multipass, false);
		}
	}

	// Check whether half floats are supported in vertex declarations (R300 doesn't support this).
	bool supportsHalf = (caps.DeclTypes & D3DDTCAPS_FLOAT16_4) != 0;
	D3DFORMAT d3dFormat = supportsHalf? D3DFMT_A16B16G16R16F : D3DFMT_A32B32G32R32F;
	AttribFormat attribFormat = supportsHalf? FORMAT_HALF : FORMAT_FLOAT;


	// Render targets for tracking the cloth state
	for (int i = 0; i < 2; i++){
		LPDIRECT3DTEXTURE9 d3drt0, d3drt1;
		if (dev->CreateTexture(CLOTH_SIZE, CLOTH_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &d3drt0, NULL) != D3D_OK){
			ERRORMSG("Couldn't create render target");
			return false;
		}
		if (dev->CreateTexture(CLOTH_SIZE, CLOTH_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &d3drt1, NULL) != D3D_OK){
			ERRORMSG("Couldn't create render target");
			return false;
		}
		if ((pos[i] = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt0, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
		if ((dir[i] = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt1, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	}

	// Render targets for the tangent space
	LPDIRECT3DTEXTURE9 d3drt;
	if (dev->CreateTexture(CLOTH_SIZE, CLOTH_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, d3dFormat, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((tangent = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	if (dev->CreateTexture(CLOTH_SIZE, CLOTH_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, d3dFormat, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((binormal = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	if (dev->CreateTexture(CLOTH_SIZE, CLOTH_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, d3dFormat, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((normal = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

    // Create a render target for interleaving the streams
	if (dev->CreateTexture(4 * CLOTH_SIZE, CLOTH_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, d3dFormat, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((rtInterleaved = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;


	// Render targets used to figure out where on the cloth the user clicked
	if ((rtHit = renderer->addRenderTarget(1, 1, FORMAT_RGBA16, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	if ((rtHitDepth = renderer->addRenderDepthStencil(1, 1, 16, 0)) == TEXTURE_NONE) return false;


	// Initialization shaders for the presets
	if ((initCloth[0] = renderer->addShader("initCloth.shd", "#define CURTAIN\n")) == SHADER_NONE) return false;
	if ((initCloth[1] = renderer->addShader("initCloth.shd", "#define SAIL\n")) == SHADER_NONE) return false;
	if ((initCloth[2] = renderer->addShader("initCloth.shd", "#define SAFETYNET\n")) == SHADER_NONE) return false;
	if ((initCloth[3] = renderer->addShader("initCloth.shd", "#define ROUNDTABLE\n")) == SHADER_NONE) return false;
	if ((initCloth[4] = renderer->addShader("initCloth.shd", "#define SQUARETABLE\n")) == SHADER_NONE) return false;
	if ((initCloth[5] = renderer->addShader("initCloth.shd", "#define SQUARETABLE2\n")) == SHADER_NONE) return false;

    // The physics shaders
	if (caps.PS20Caps.NumInstructionSlots >= 512){
		if ((physics = renderer->addShader("physics.shd", "#define NODE_COUNT 20\n#define FIRST_PASS\n#define LAST_PASS\n")) == SHADER_NONE) return false;
	}

	if ((physicsMP[0] = renderer->addShader("physics.shd", "#define NODE_COUNT 6\n#define FIRST_PASS\n")) == SHADER_NONE) return false;
	if ((physicsMP[1] = renderer->addShader("physics.shd", "#define NODE_COUNT 5\n")) == SHADER_NONE) return false;
	if ((physicsMP[2] = renderer->addShader("physics.shd", "#define NODE_COUNT 4\n#define LAST_PASS\n")) == SHADER_NONE) return false;

    // The rest of the needed shaders
	if ((lighting = renderer->addShader("lighting.shd")) == SHADER_NONE) return false;
	if ((tangentSpace = renderer->addShader("tangentSpace.shd")) == SHADER_NONE) return false;
	if ((interleave = renderer->addShader("interleave.shd")) == SHADER_NONE) return false;
	if ((hit = renderer->addShader("hit.shd")) == SHADER_NONE) return false;
	if ((setNode = renderer->addShader("setNode.shd")) == SHADER_NONE) return false;

	// The cloth texture
	if ((base = renderer->addTexture(TEX_PATH "wallpaper.dds", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	// Load a bumpmap for the cloth. We use UV5L6 format for improved performance.
	Image img;
	if (!img.loadPNG(TEX_PATH "wallpaperBump.png")) return false;
	img.generateMipMaps();
	img.toNormalMap(FORMAT_UV5L6, 0.4f);
	if ((bump = renderer->addTexture(img, FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

    // Load vertex attributes
	VertexAttribute quadAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((quadVF = renderer->addVertexFormat(quadAttribs, elementsOf(quadAttribs))) == TEXTURE_NONE) return false;

	StreamVertexAttribute clothAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT,  4,
		1, TYPE_TANGENT,  attribFormat,  4,
		2, TYPE_BINORMAL, attribFormat,  4,
		3, TYPE_NORMAL,   attribFormat,  4,
		4, TYPE_TEXCOORD, FORMAT_USHORT, 2,
	};
	if ((clothVF = renderer->addVertexFormat(clothAttribs, elementsOf(clothAttribs))) == TEXTURE_NONE) return false;

	StreamVertexAttribute clothILAttribs[] = {
		0, TYPE_VERTEX,   attribFormat,  4,
		0, TYPE_TANGENT,  attribFormat,  4,
		0, TYPE_BINORMAL, attribFormat,  4,
		0, TYPE_NORMAL,   attribFormat,  4,
		1, TYPE_TEXCOORD, FORMAT_USHORT, 2,
	};
	if ((clothInterleavedVF = renderer->addVertexFormat(clothILAttribs, elementsOf(clothILAttribs))) == TEXTURE_NONE) return false;

	StreamVertexAttribute hitAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT,  4,
		1, TYPE_TEXCOORD, FORMAT_USHORT, 2,
	};
	if ((hitVF = renderer->addVertexFormat(hitAttribs, elementsOf(hitAttribs))) == TEXTURE_NONE) return false;

	// Create an index buffer for the cloth.
	if ((clothIB = renderer->addIndexBuffer(getRectangleIndexCount(CLOTH_SIZE, CLOTH_SIZE), sizeof(INDEX_TYPE))) == IB_NONE) return false;
	INDEX_TYPE *iDest = (INDEX_TYPE *) renderer->lockIndexBuffer(clothIB);
		fillRectangleIB(iDest, CLOTH_SIZE, CLOTH_SIZE);
	renderer->unlockIndexBuffer(clothIB);

	// Create a vertex buffer for the cloth. It only contains the texture coordinates.
	if ((clothVB = renderer->addVertexBuffer(CLOTH_SIZE * CLOTH_SIZE * sizeof(float2))) == VB_NONE) return false;
	ushort *vDest = (ushort *) renderer->lockVertexBuffer(clothVB);
		for (int y = 0; y < CLOTH_SIZE; y++){
			for (int x = 0; x < CLOTH_SIZE; x++){
				*vDest++ = (65535 * x) / (CLOTH_SIZE - 1);
				*vDest++ = (65535 * y) / (CLOTH_SIZE - 1);
			}
		}
	renderer->unlockVertexBuffer(clothVB);

	/*
		Create a dummy vertex buffer.
		For the debug runtime we need to make it large enough to be valid for the draw call.
		The release runtime doesn't have this validation, and since we're really sampling
		from the texture we don't want to waste memory, so we create a minimal vertex buffer
		in this case.
	*/
	if ((dummy = renderer->addVertexBuffer(debugRuntime? 4 * CLOTH_SIZE * CLOTH_SIZE * (supportsHalf? sizeof(half4) : sizeof(float4)) : 1)) == VB_NONE) return false;

	currRT  = 0;
	otherRT = 1;
	needsInit = true;

	grabCoord = float2(0, 0);
	grabPos = float3(0, 0, 0);
	grabbed = false;

	if (dev->CreateOffscreenPlainSurface(1, 1, D3DFMT_A16B16G16R16, D3DPOOL_SYSTEMMEM, &grabCoordSurface, NULL) != D3D_OK) return false;
	if (dev->CreateOffscreenPlainSurface(CLOTH_SIZE, CLOTH_SIZE, D3DFMT_A32B32G32R32F, D3DPOOL_SYSTEMMEM, &grabPosSurface, NULL) != D3D_OK) return false;

	initPerlin();

	return true;
}

bool App::unload(){
	grabCoordSurface->Release();
	grabPosSurface->Release();
	return true;
}

bool App::onReset(){
	needsInit = true;
	return true;
}

void App::drawFrame(){
	float4x4 projection = perspectiveMatrixY(1.3f, width, height, 0.02f, 40, true);

	float4x4 modelview = rotateXY4(-wx, -wy);
	modelview.translate(-camPos);
	float4x4 mvp = projection * modelview;

	float3 lightPos = float3(3, 1, -1);

	// Protect against framerate fluctuation.
	static float fTime = 0;
	fTime *= 0.85f;
	fTime += min(frameTime, 0.007f);
	float pfTime = 0.15f * fTime;


	// Compute wind direction
	float3 windDir;
	windDir.x = xWind->getValue();
	windDir.y = yWind->getValue();
	windDir.z = zWind->getValue();
	windDir.x += xnWind->getValue() * noise1(-0.5f * time);
	windDir.y += ynWind->getValue() * noise1(0.5f * time + 19.291f);
	windDir.z += znWind->getValue() * noise1(0.5f * time);



	const float vertices[] = { -1, -1,   1, -1,   1, 1,   -1, 1 };

	if (keys[KEY_R]){
		needsInit = true;
	}


	TextureID rtPN[] = { pos[currRT], dir[currRT] };
	renderer->changeRenderTargets(elementsOf(rtPN), rtPN);

		if (needsInit){
			// Initialize the cloth
			float scale = CLOTH_SIZE / (CLOTH_SIZE - 1.0f);
			float bias = 1.0f / (CLOTH_SIZE - 1.0f);

			renderer->setShader(initCloth[clothMenu->getCheckedRadioButton(1)]);
			renderer->setShaderConstant2f("scale", float2(scale, -scale));
			renderer->setShaderConstant1f("bias", bias);
			renderer->setVertexFormat(quadVF);
			renderer->setDepthFunc(DEPTH_NONE);
			renderer->apply();

			dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

			// Also reset the normals to a reasonable starting point
			// as they are used in the simulation.
			renderer->changeRenderTarget(normal);
			dev->Clear(0, NULL, D3DCLEAR_TARGET, 0x0000FF00, 1.0f, 0);

			needsInit = false;
		} else {
			/*
				The physics pass
			*/
			// Each node have springs connecting to neighboring nodes in the following pattern
			float nodeDistance = 2.0f / (CLOTH_SIZE - 1);
			float sq2 = sqrtf(2);

			float3 springs[] = {
				float3( 1.0f / CLOTH_SIZE,  0.0f,              nodeDistance),
				float3( 0.0f,               1.0f / CLOTH_SIZE, nodeDistance),
				float3(-1.0f / CLOTH_SIZE,  0.0f,              nodeDistance),
				float3( 0.0f,              -1.0f / CLOTH_SIZE, nodeDistance),

				float3( 1.0f / CLOTH_SIZE,  1.0f / CLOTH_SIZE, nodeDistance * sq2),
				float3(-1.0f / CLOTH_SIZE,  1.0f / CLOTH_SIZE, nodeDistance * sq2),
				float3(-1.0f / CLOTH_SIZE, -1.0f / CLOTH_SIZE, nodeDistance * sq2),
				float3( 1.0f / CLOTH_SIZE, -1.0f / CLOTH_SIZE, nodeDistance * sq2),

				float3( 2.0f / CLOTH_SIZE,  2.0f,              nodeDistance * 2 * sq2),
				float3(-2.0f,               2.0f / CLOTH_SIZE, nodeDistance * 2 * sq2),
				float3(-2.0f / CLOTH_SIZE, -2.0f,              nodeDistance * 2 * sq2),
				float3( 2.0f,              -2.0f / CLOTH_SIZE, nodeDistance * 2 * sq2),

				float3( 3.0f / CLOTH_SIZE,  0.0f / CLOTH_SIZE, nodeDistance * 3),
				float3( 0.0f / CLOTH_SIZE,  3.0f / CLOTH_SIZE, nodeDistance * 3),
				float3(-3.0f / CLOTH_SIZE,  0.0f / CLOTH_SIZE, nodeDistance * 3),
				float3( 0.0f / CLOTH_SIZE, -3.0f / CLOTH_SIZE, nodeDistance * 3),

				float3( 4.0f / CLOTH_SIZE,  4.0f,              nodeDistance * 4 * sq2),
				float3(-4.0f,               4.0f / CLOTH_SIZE, nodeDistance * 4 * sq2),
				float3(-4.0f / CLOTH_SIZE, -4.0f,              nodeDistance * 4 * sq2),
				float3( 4.0f,              -4.0f / CLOTH_SIZE, nodeDistance * 4 * sq2),
			};

			float damp;
			if (keys[KEY_END]){
				// To manually dampen the cloth simulation to near halt.
				damp = powf(1e-20f, pfTime);
			} else {
				// Compute a dampen factor to avoid energy build up in the system.
				damp = powf(0.005f, pfTime);
			}
			// Wind direction with frameTime pre-multiplied into the vector to simplify the math in the shader
			float3 ftWindDir = sqrtf(pfTime) * windDir;
			// Frame-time pre-multiplied with a set of numbers to simplify the shader
			float4 frameTimePM = pfTime * float4(1, 5, 10, 3500);


			if (clothMenu->isItemChecked(multipass)){
				/*
					Multipass physics (for ps2_0 cards)
				*/

				// First pass
					renderer->setShader(physicsMP[0]);
					renderer->setShaderConstant2f("halfPixel", float2(0.5f / CLOTH_SIZE, 0.5f / CLOTH_SIZE));
					renderer->setShaderConstant4f("frameTime", frameTimePM);
					renderer->setShaderConstant1f("damp", damp);
					renderer->setShaderConstantArray3f("springs", springs, 6);
					renderer->setTexture("Pos", pos[otherRT]);
					renderer->setTexture("Dir", dir[otherRT]);
					renderer->setVertexFormat(quadVF);
					renderer->setDepthFunc(DEPTH_NONE);
					renderer->apply();

					dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

				// Second pass
				renderer->changeRenderTarget(dir[otherRT]);

					renderer->setShader(physicsMP[1]);
					renderer->setShaderConstant2f("halfPixel", float2(0.5f / CLOTH_SIZE, 0.5f / CLOTH_SIZE));
					renderer->setShaderConstant4f("frameTime", frameTimePM);
					renderer->setShaderConstantArray3f("springs", springs + 6, 5);
					renderer->setTexture("Pos", pos[currRT]);
					renderer->setTexture("Dir", dir[currRT]);
					renderer->setVertexFormat(quadVF);
					renderer->setDepthFunc(DEPTH_NONE);
					renderer->apply();

					dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

				// Third pass
				renderer->changeRenderTarget(dir[currRT]);

					renderer->setShader(physicsMP[1]);
					renderer->setShaderConstant2f("halfPixel", float2(0.5f / CLOTH_SIZE, 0.5f / CLOTH_SIZE));
					renderer->setShaderConstant4f("frameTime", frameTimePM);
					renderer->setShaderConstantArray3f("springs", springs + 11, 5);
					renderer->setTexture("Pos", pos[otherRT]);
					renderer->setTexture("Dir", dir[otherRT]);
					renderer->setVertexFormat(quadVF);
					renderer->setDepthFunc(DEPTH_NONE);
					renderer->apply();

					dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

				// Fourth and final pass
				TextureID rtOtherPN[] = { pos[otherRT], dir[otherRT] };
				renderer->changeRenderTargets(elementsOf(rtOtherPN), rtOtherPN);

					renderer->setShader(physicsMP[2]);
					renderer->setShaderConstant2f("halfPixel", float2(0.5f / CLOTH_SIZE, 0.5f / CLOTH_SIZE));
					renderer->setShaderConstant4f("frameTime", frameTimePM);
					renderer->setShaderConstant3f("windDir", ftWindDir);
					renderer->setShaderConstantArray3f("springs", springs + 16, 4);
					renderer->setTexture("Pos", pos[currRT]);
					renderer->setTexture("Dir", dir[currRT]);
					renderer->setTexture("Normal", normal);
					renderer->setVertexFormat(quadVF);
					renderer->setDepthFunc(DEPTH_NONE);
					renderer->apply();

					dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

				currRT = otherRT;
				otherRT = 1 - currRT;

			} else {
				/*
					Single pass physics (for ps_2_b and up)
				*/
				renderer->setShader(physics);
				renderer->setShaderConstant2f("halfPixel", float2(0.5f / CLOTH_SIZE, 0.5f / CLOTH_SIZE));
				renderer->setShaderConstant4f("frameTime", frameTimePM);
				renderer->setShaderConstant1f("damp", damp);
				renderer->setShaderConstant3f("windDir", ftWindDir);
				renderer->setShaderConstantArray3f("springs", springs, elementsOf(springs));
				renderer->setTexture("Pos", pos[otherRT]);
				renderer->setTexture("Dir", dir[otherRT]);
				renderer->setTexture("Normal", normal);
				renderer->setVertexFormat(quadVF);
				renderer->setDepthFunc(DEPTH_NONE);
				renderer->apply();

				dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));
			}

			if (grabbed){
				// Position in clip space. We don't know z, so we set it arbitrarily to 0.
				float4 cPos(2 * float(mouseX) / float(width) - 1, 1 - 2 * float(mouseY) / float(height), 0, 1);
				// Reverse transform to unproject it back to world space.
				// This gives a point that would map to the clicked pixel.
				float4 unProj = !mvp * cPos;

				// Compute direction from camera to this position
				float3 dir = normalize((unProj.xyz() / unProj.w) - camPos);

				// Intersect with the plane
				float d = dot(camPos, grabNormal) + grabOffset;
				vec3 pos = camPos - (d / dot(grabNormal, dir)) * dir;

				renderer->setShader(setNode);
				renderer->setShaderConstant3f("pos", pos);
				renderer->setShaderConstant1f("free", grabFree);
				renderer->setVertexFormat(quadVF);
				renderer->setDepthFunc(DEPTH_NONE);
				renderer->apply();

				float ps = 5;
				dev->SetRenderState(D3DRS_POINTSIZE, *(DWORD *) &ps);

				dev->DrawPrimitiveUP(D3DPT_POINTLIST, 1, grabCoord, sizeof(float2));

			}

		}

	TextureID rtTS[] = { tangent, binormal, normal };
	renderer->changeRenderTargets(elementsOf(rtTS), rtTS);

		/*
			Compute the tangent space
		*/
		renderer->setShader(tangentSpace);
		renderer->setShaderConstant2f("halfPixel", float2(0.5f / CLOTH_SIZE, 0.5f / CLOTH_SIZE));
		renderer->setShaderConstant1f("pixel", 1.0f / CLOTH_SIZE);
		renderer->setTexture("Pos", pos[currRT]);
		renderer->setVertexFormat(quadVF);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

		/*
			Produce an interleaved vertex buffer.
			This is neccesary for cards that don't support VS3.0 since they can't
			use the vertex samplers and thus can only use one R2VB vertex stream.
		*/
		if (!clothMenu->isItemChecked(separateStreams)){

			renderer->changeRenderTarget(rtInterleaved);

			renderer->setShader(interleave);
			renderer->setTexture("Position", pos[currRT]);
			renderer->setTexture("Tangent",  tangent);
			renderer->setTexture("Binormal", binormal);
			renderer->setTexture("Normal",   normal);
			renderer->setShaderConstant2f("halfPixel", float2(0.5f / (4 * CLOTH_SIZE), 0.5f / CLOTH_SIZE));
			renderer->setShaderConstant1f("width", CLOTH_SIZE);
			renderer->setVertexFormat(quadVF);
			renderer->setDepthFunc(DEPTH_NONE);
			renderer->apply();

			dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));
		}

	renderer->resetRenderTarget();





	dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF224466, 1.0f, 0);

	/*
		The lighting pass
	*/

	// Enable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));
	if (clothMenu->isItemChecked(separateStreams)){
		renderer->setShader(lighting);
		renderer->setTexture("Base", base);
		renderer->setTexture("Bump", bump);
		renderer->setShaderConstant4x4f("mvp", mvp);
		renderer->setShaderConstant3f("lightPos", lightPos);
		renderer->setShaderConstant3f("camPos", camPos);
		renderer->setVertexFormat(clothVF);
		renderer->setVertexBuffer(dummy, 0);
		renderer->setVertexBuffer(dummy, 1);
		renderer->setVertexBuffer(dummy, 2);
		renderer->setVertexBuffer(dummy, 3);
		renderer->setVertexBuffer(clothVB, 4);
		renderer->setIndexBuffer(clothIB);
		renderer->apply();

		// Bind the R2VB render targets to the vertex texture samplers
		dev->SetTexture(D3DVERTEXTEXTURESAMPLER0, ((D3D9Renderer *) renderer)->getD3DTexture(pos[currRT]));
		dev->SetTexture(D3DVERTEXTEXTURESAMPLER1, ((D3D9Renderer *) renderer)->getD3DTexture(tangent));
		dev->SetTexture(D3DVERTEXTEXTURESAMPLER2, ((D3D9Renderer *) renderer)->getD3DTexture(binormal));
		dev->SetTexture(D3DVERTEXTEXTURESAMPLER3, ((D3D9Renderer *) renderer)->getD3DTexture(normal));

		// Tell the driver that the vertex streams are to be fetched from the vertex textures
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_VTX0));
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_VTX1));
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(2, R2VB_VSMP_OVR_VTX2));
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(3, R2VB_VSMP_OVR_VTX3));

		dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, CLOTH_SIZE * CLOTH_SIZE, 0, getRectangleIndexCount(CLOTH_SIZE, CLOTH_SIZE) - 2);

		// Streams restored to regular vertex buffer mode
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DIS));
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(2, R2VB_VSMP_OVR_DIS));
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(3, R2VB_VSMP_OVR_DIS));

		// Unbind the vertex textures
		dev->SetTexture(D3DVERTEXTEXTURESAMPLER0, NULL);
		dev->SetTexture(D3DVERTEXTEXTURESAMPLER1, NULL);
		dev->SetTexture(D3DVERTEXTEXTURESAMPLER2, NULL);
		dev->SetTexture(D3DVERTEXTEXTURESAMPLER3, NULL);
	} else {
		renderer->setShader(lighting);
		renderer->setTexture("Base", base);
		renderer->setTexture("Bump", bump);
		renderer->setShaderConstant4x4f("mvp", mvp);
		renderer->setShaderConstant3f("lightPos", lightPos);
		renderer->setShaderConstant3f("camPos", camPos);
		renderer->setVertexFormat(clothInterleavedVF);
		renderer->setVertexBuffer(dummy, 0);
		renderer->setVertexBuffer(clothVB, 1);
		renderer->setIndexBuffer(clothIB);
		renderer->apply();

		// Bind the interleaved R2VB render target to the DMAP sampler
		dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(rtInterleaved));
		// Tell the driver that vertex stream 0 is to be fetched from the DMAP sampler
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DMAP));

		dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, CLOTH_SIZE * CLOTH_SIZE, 0, getRectangleIndexCount(CLOTH_SIZE, CLOTH_SIZE) - 2);

		// Stream 0 restored to regular vertex buffer mode
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DIS));
		// Unbind the vertex textures
		dev->SetTexture(D3DDMAPSAMPLER, NULL);
	}

	/*
		Handle mouse clicks
	*/
	static bool lastMouse = false;
	if (mMouseButton || rMouseButton){
		if (!lastMouse){
			// First render to a 1x1 texture.
			// We scale projection so that the clicked pixel maps right into this render target.
			renderer->changeRenderTarget(rtHit, rtHitDepth);

				dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF000000, 1.0f, 0);

				float x = (float(mouseX) / width ) * 2 - 1;
				float y = 1 - (float(mouseY) / height) * 2;

				float4x4 sMvp = scale4(width / 2.0f, height / 2.0f, 1) * translate(-x, -y, 0) * mvp;

				renderer->setShader(hit);
				renderer->setShaderConstant4x4f("mvp", sMvp);
				renderer->setVertexFormat(hitVF);
				renderer->setVertexBuffer(dummy, 0);
				renderer->setVertexBuffer(clothVB, 1);
				renderer->setIndexBuffer(clothIB);
				renderer->apply();


				// Bind the render target to the DMAP sampler
				dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(pos[currRT]));
				// Tell the driver that stream 0 is to be fetched from a vertex texture
				dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DMAP));

				dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, CLOTH_SIZE * CLOTH_SIZE, 0, getRectangleIndexCount(CLOTH_SIZE, CLOTH_SIZE) - 2);

				// Streams restored to regular vertex buffer mode
				dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DIS));
				// Unbind the vertex texture
				dev->SetTexture(D3DDMAPSAMPLER, NULL);

			renderer->resetRenderTarget();

			// Read back the pixel
			if (dev->GetRenderTargetData(((D3D9Renderer *) renderer)->getRenderTargetSurface(rtHit), grabCoordSurface) == D3D_OK){
				D3DLOCKED_RECT lockedRect;
				if (grabCoordSurface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY) == D3D_OK){
					ushort *src = (ushort *) lockedRect.pBits;

					// If src[3] != 0, we're clicking somewhere outside the cloth
					if (src[3] == 0){
						// Compute the texture coordinates for the point we clicked
						grabCoord.x = (src[0] / 65535.0f) * 2 - 1;
						grabCoord.y = 1 - (src[1] / 65535.0f) * 2;

						// Read back the position of the closest node
						if (dev->GetRenderTargetData(((D3D9Renderer *) renderer)->getRenderTargetSurface(pos[currRT]), grabPosSurface) == D3D_OK){

							int s = CLOTH_SIZE * src[0] / 65536;
							int t = CLOTH_SIZE * src[1] / 65536;

							D3DLOCKED_RECT lockedRect;
							if (grabPosSurface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY) == D3D_OK){
                                // Get the position
								grabPos = *(float3 *) (((ubyte *) lockedRect.pBits) + t * lockedRect.Pitch + s * sizeof(float4));

								// Compute a plane parallel with the view plane intersection the node.
								// This is the plane we'll move the point in when we drag the mouse.
								grabNormal = modelview.getForwardVec();
								grabOffset = -dot(grabPos, grabNormal);

								grabFree = rMouseButton? 1.0f : 0.0f;
								grabbed = true;

								grabPosSurface->UnlockRect();
							}
						}
					}

					grabCoordSurface->UnlockRect();
				}
			}
		}
	} else {
		grabbed = false;
	}
	lastMouse = (mMouseButton || rMouseButton);


	// Disable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));


	currRT = otherRT;
	otherRT = 1 - currRT;
}
