//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#include "App.h"

BaseApp *app = new App();

void App::setup(char *args[], int nArguments){
	width  = 800;
	height = 600;
	speed = 0.3f;
}

void App::resetCamera(){
	camPos = float3(0.086f, 0.05f, -0.43f);
	wx = 0.069f;
	wy = 2.1f;
}

// The height of the terrain
float terrainY(const float x, const float z){
	return 0.2f * turbulence2(x, z, 4);
}

// Function to compute the normal of the terrain
float3 terrainNormal(const float x, const float z){
	return normalize(cross(float3(0, terrainY(x, z + 0.005f) - terrainY(x, z - 0.005f), 0.01f), float3(0.01f, terrainY(x + 0.005f, z) - terrainY(x - 0.005f, z), 0)));
}

void App::moveCamera(const float3 &dir){
	camPos += dir * (speed * frameTime);

	// Compute the tangent plane of the terrain
	float ty0 = terrainY(camPos.x, camPos.z);
	float ty1 = terrainY(camPos.x + 0.01f, camPos.z);
	float ty2 = terrainY(camPos.x, camPos.z + 0.01f);

	float3 normal = normalize(cross(float3(0, ty2 - ty0, 0.01f), float3(0.01f, ty1 - ty0, 0)));
	float offset = -dot(float3(camPos.x, ty0, camPos.z), normal);

	// Do collision detection against the tangent plane
	float d = dot(camPos, normal) + offset;
	if (d < 0.02f){
		camPos += (0.02f - d) * normal;
	}
}

bool App::onReset(){
	needsInit = true;
	return true;
}

bool App::load(){
	// Check for R2VB support
	if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, (D3DFORMAT) FOURCC('R','2','V','B')) != D3D_OK){
		ERRORMSG("No R2VB support");
		return false;
	}

	// Check whether certain features are supported.
	bool supportsPS3 = (caps.PixelShaderVersion >= D3DPS_VERSION(3,0));
	bool supportsHalf = (caps.DeclTypes & D3DDTCAPS_FLOAT16_4) != 0;
	bool supportsATI1N = (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, (D3DFORMAT) MAKEFOURCC('A', 'T', 'I', '1')) == D3D_OK);

	stx_srand(16);
	initPerlin();

	for (int i = 0; i < 2; i++){
		// Render targets for sorting
		if ((posRT[i]  = renderer->addRenderTarget(SIZE_X, SIZE_Y, FORMAT_RGBA16f, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
		if ((distRT[i] = renderer->addRenderTarget(SIZE_X, SIZE_Y, FORMAT_RG32f,   FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

		// Render targets for smoke animation
		if ((smokePosRT[i] = renderer->addRenderTarget(SMOKE_X, SMOKE_Y, FORMAT_RGBA16f, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
		if ((smokeDirRT[i] = renderer->addRenderTarget(SMOKE_X, SMOKE_Y, FORMAT_RGBA16f, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	}

	// R2VB render target for expanding the particles to quads
	LPDIRECT3DTEXTURE9 d3drt;
	if (dev->CreateTexture(4 * SIZE_X, SIZE_Y, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, supportsHalf? D3DFMT_A16B16G16R16F : D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((particlesRT = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;


	// Index buffer for the particles
	if ((particleIB = renderer->addIndexBuffer(6 * COUNT, sizeof(int))) == TEXTURE_NONE) return false;
	uint *indices = (uint *) renderer->lockIndexBuffer(particleIB);
	for (int i = 0; i < COUNT; i++){
		indices[6 * i + 0] = 4 * i;
		indices[6 * i + 1] = 4 * i + 1;
		indices[6 * i + 2] = 4 * i + 2;
		indices[6 * i + 3] = 4 * i;
		indices[6 * i + 4] = 4 * i + 2;
		indices[6 * i + 5] = 4 * i + 3;
	}
	renderer->unlockIndexBuffer(particleIB);

	// Base texture coordinates for the particles
	if ((texCoords = renderer->addVertexBuffer(4 * COUNT * sizeof(float2))) == TEXTURE_NONE) return false;
	float2 *tc = (float2 *) renderer->lockVertexBuffer(texCoords);
	for (int i = 0; i < COUNT; i++){
		tc[4 * i + 0] = float2(0, 0);
		tc[4 * i + 1] = float2(1, 0);
		tc[4 * i + 2] = float2(1, 1);
		tc[4 * i + 3] = float2(0, 1);
	}
	renderer->unlockVertexBuffer(texCoords);


	// Ground texture
	if ((terrainTex = renderer->addTexture(TEX_PATH "Ground_12-1.dds", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	// Vertex declaration for the terrain
	VertexAttribute terrainAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 3 };
	if ((terrainVF = renderer->addVertexFormat(terrainAttribs, elementsOf(terrainAttribs))) == TEXTURE_NONE) return false;

	// Vertex buffer for the terrain
	if ((terrainVB = renderer->addVertexBuffer(TERRAIN_SIZE * TERRAIN_SIZE * sizeof(float3))) == TEXTURE_NONE) return false;
	float3 *dest = (float3 *) renderer->lockVertexBuffer(terrainVB);
	for (int z = 0; z < TERRAIN_SIZE; z++){
		for (int x = 0; x < TERRAIN_SIZE; x++){
			float tx = float(x) / (TERRAIN_SIZE - 1) * 2 - 1;
			float tz = 1 - 2 * float(z) / (TERRAIN_SIZE - 1);
			*dest++ = float3(tx, terrainY(tx, tz), tz);
		}
	}
	renderer->unlockVertexBuffer(terrainVB);

	// Index buffer for terrain
	if ((terrainIB = renderer->addIndexBuffer(getRectangleIndexCount(TERRAIN_SIZE, TERRAIN_SIZE), sizeof(int))) == TEXTURE_NONE) return false;
	indices = (uint *) renderer->lockIndexBuffer(terrainIB);
		fillRectangleIB(indices, TERRAIN_SIZE, TERRAIN_SIZE);
	renderer->unlockIndexBuffer(terrainIB);





	// Create ground and lighting texture
	Image gImg, tImg;
	ubyte *gDest = (ubyte *) gImg.create(FORMAT_I8, TERRAIN_TEX_SIZE, TERRAIN_TEX_SIZE, 1, 1);
	ubyte *tDest = (ubyte *) tImg.create(FORMAT_I8, TERRAIN_TEX_SIZE, TERRAIN_TEX_SIZE, 1, 1);

	float3 lightDir = normalize(float3(-1, 1, 0));
	for (int z = 0; z < TERRAIN_TEX_SIZE; z++){
		for (int x = 0; x < TERRAIN_TEX_SIZE; x++){
			float tx = float(x) / (TERRAIN_TEX_SIZE - 1) * 2 - 1;
			float tz = float(z) / (TERRAIN_TEX_SIZE - 1) * 2 - 1;

			float y = terrainY(tx, tz);
			float3 normal = terrainNormal(tx, tz);

			*gDest++ = (ubyte) (255.0f * (y / 0.2f * 0.5f + 0.5f));
			*tDest++ = (ubyte) (255.0f * saturate(dot(normal, lightDir)));
		}
	}
	tImg.generateMipMaps();
	// Compress the textures. Use ATI1N if available to get better quality.
	gImg.compress(supportsATI1N? FORMAT_ATI1N : FORMAT_DXT1);
	tImg.compress(supportsATI1N? FORMAT_ATI1N : FORMAT_DXT1);

	if ((ground = renderer->addTexture(gImg, FILTER_LINEAR, CLAMP)) == TEXTURE_NONE) return false;
	if ((terrainLight = renderer->addTexture(tImg, FILTER_BILINEAR, CLAMP)) == TEXTURE_NONE) return false;




	// Load the textures for the foliage and create a texture atlas.
	Image atlas, grass0, grass1, leaves;
	if (!grass0.loadDDS(TEX_PATH "grass001.dds")) return false;
	if (!grass1.loadDDS(TEX_PATH "grass002.dds")) return false;
	if (!leaves.loadDDS(TEX_PATH "MapleLeaves.dds")) return false;

	// Much easier to deal with uncompressed ...
	grass0.decompress();
	grass1.decompress();
	leaves.decompress();

	ubyte *fDest = atlas.create(FORMAT_RGBA8, 1024, 1024, 1, 1);
	ubyte *gSrc0 = grass0.getPixels();
	ubyte *gSrc1 = grass1.getPixels();
	ubyte *lSrc  = leaves.getPixels();
	for (int y = 0; y < 512; y++){
		stx_memcpy(fDest + 1024 * 4 * y,           gSrc0 + 512 * 4 * y, 512 * 4);
		stx_memcpy(fDest + 1024 * 4 * y + 512 * 4, lSrc  + 512 * 4 * y, 512 * 4);
	}
	for (int y = 0; y < 128; y++){
		stx_memcpy(fDest + 1024 * 4 * (512 + y), gSrc1 + 128 * 4 * y, 128 * 4);
	}
	if ((foliage = renderer->addTexture(atlas, FILTER_TRILINEAR_ANISO, CLAMP)) == TEXTURE_NONE) return false;

	// Load the smoke animation texture
	if ((smokeTex = renderer->addTexture(TEX_PATH "SmokeAnimation.dds", FILTER_TRILINEAR, CLAMP)) == TEXTURE_NONE) return false;

	/*
		Distribute foliage over the terrain. To make the distribution fairly even a KD-tree is used
		to check whether an incoming location is too close to any existing location. This way all
		pieces of foliage are guaranteed to be at least maxDist distance apart.
	*/
	KDTree <float, 2> tree;
	float maxDist = 1.5f / sqrtf(COUNT);

	Image foliageImg;
	half *foliageDest = (half *) foliageImg.create(FORMAT_RGBA16f, FOLIAGE_X, FOLIAGE_Y, 1, 1);
	for (int i = 0; i < FOLIAGE_X * FOLIAGE_Y; i++){
		float tx, tz;

		do {
			tx = float(stx_rand()) / RAND_MAX * 2 - 1;
			tz = float(stx_rand()) / RAND_MAX * 2 - 1;
		} while (tree.hasPointWithin(float2(tx, tz), maxDist));
		tree.add(float2(tx, tz));

		foliageDest[4 * i + 0] = tx;
		foliageDest[4 * i + 1] = min(min(min(min(terrainY(tx, tz), terrainY(tx - 0.02f, tz)), terrainY(tx + 0.02f, tz)), terrainY(tx, tz - 0.02f)), terrainY(tx, tz + 0.02f));
		foliageDest[4 * i + 2] = tz;
		foliageDest[4 * i + 3] = -float(stx_rand() + 1) / (RAND_MAX + 2);
	}


	// Add fires and smoke sources
	Image smokeImg;
	half *smokeDest = (half *) smokeImg.create(FORMAT_RGBA16f, SMOKE_X, SMOKE_Y, 1, 1);

	float px, pz;
	int n = 0x7FFFFFFE;
	int fireIndex = 0;
	for (int i = 0; i < SMOKE_X * SMOKE_Y; i++){
		n++;
		if (n >= SMOKE_X * SMOKE_Y / SMOKE_EMITTERS && fireIndex < SMOKE_EMITTERS){
			px = foliageDest[4 * fireIndex + 0];
			pz = foliageDest[4 * fireIndex + 2];
			foliageDest[4 * fireIndex + 3] = -2;
			fireIndex++;

			n = 0;
		}

		float tx, tz;
		tx = px + 0.03f * (float(stx_rand()) / RAND_MAX * 2 - 1);
		tz = pz + 0.03f * (float(stx_rand()) / RAND_MAX * 2 - 1);

		smokeDest[4 * i + 0] = tx;
		smokeDest[4 * i + 1] = terrainY(tx, tz);
		smokeDest[4 * i + 2] = tz;
		smokeDest[4 * i + 3] = 5.0f * float(stx_rand()) / RAND_MAX;
	}

	if ((foliageSource = renderer->addTexture(foliageImg, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	if ((smokeSource = renderer->addTexture(smokeImg, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;


	// Create a noise texture made specially for fire and store a cached copy to disc
#define NSIZE 64
#define SCALE 0.1482f
	Image img;
	if (!img.loadDDS("FireNoise.dds") || ((img.getFormat() == FORMAT_ATI1N) != supportsATI1N)){
		ubyte *nDest = img.create(FORMAT_I8, NSIZE, NSIZE, NSIZE, 1);
		for (int z = 0; z < NSIZE; z++){
			for (int y = 0; y < NSIZE; y++){
				float wobble = tileableNoise1(y * 0.152f, 0.152f * NSIZE);
				for (int x = 0; x < NSIZE; x++){
					float n = tileableTurbulence3(SCALE * x + wobble, SCALE * y, SCALE * z, NSIZE * SCALE, NSIZE * SCALE, NSIZE * SCALE, 16);

					float fx = saturate(float(x) / (NSIZE - 1) * 1.1f - 0.05f + 0.2f * wobble);
					fx *= 4 * (1 - fx);

					n = fx * (n * 0.5f + 0.5f);

					*nDest++ = (ubyte) (255 * n);
				}
			}
		}
		img.compress(supportsATI1N? FORMAT_ATI1N : FORMAT_DXT1);
		img.saveDDS("FireNoise.dds");
	}
	if ((noise = renderer->addTexture(img, FILTER_LINEAR)) == TEXTURE_NONE) return false;
	if ((firePalette = renderer->addTexture(TEX_PATH "FirePalette.dds", FILTER_LINEAR, CLAMP)) == TEXTURE_NONE) return false;

	// Load a skybox
	if ((env = renderer->addCubemap(
		TEX_PATH "Mountains/posx.jpg", TEX_PATH "Mountains/negx.jpg",
		TEX_PATH "Mountains/posy.jpg", TEX_PATH "Mountains/negy.jpg",
		TEX_PATH "Mountains/posz.jpg", TEX_PATH "Mountains/negz.jpg", FILTER_TRILINEAR)) == TEXTURE_NONE) return false;

	// Load all the shaders
	if ((bitonicSort = renderer->addShader("bitonicSort.hlsl", supportsPS3? "#define SM3\n" : NULL)) == SHADER_NONE) return false;
	if ((particles   = renderer->addShader("particles.hlsl", supportsPS3? "#define SM3\n" : NULL)) == SHADER_NONE) return false;
	if ((distance    = renderer->addShader("distance.hlsl")) == SHADER_NONE) return false;
	if ((makeQuads   = renderer->addShader("makeQuads.hlsl")) == SHADER_NONE) return false;
	if ((terrain     = renderer->addShader("terrain.hlsl")) == SHADER_NONE) return false;
	if ((skybox      = renderer->addShader("skybox.hlsl")) == SHADER_NONE) return false;
	if ((initSmoke   = renderer->addShader("initSmoke.hlsl")) == SHADER_NONE) return false;
	if ((smoke       = renderer->addShader("smoke.hlsl", supportsPS3? "#define SM3\n" : NULL)) == SHADER_NONE) return false;

	// Load the vertex declarations
	VertexAttribute skyboxAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 3 };
	if ((skyboxVF = renderer->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs))) == VF_NONE) return false;

	VertexAttribute quadAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((quadVF = renderer->addVertexFormat(quadAttribs, elementsOf(quadAttribs))) == TEXTURE_NONE) return false;

	// Create a minimal vertex buffer for a quad. We'll use it frequently in the sort passes.
	float vertices[]  = { -1, -1,  1, -1,  -1, 1,  1, 1 };
	if ((quadVB = renderer->addVertexBuffer(sizeof(vertices), vertices)) == VB_NONE) return false;

	StreamVertexAttribute particleAttribs[] = {
		0, TYPE_VERTEX,   supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 4,
		1, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
	};
	if ((particleVF = renderer->addVertexFormat(particleAttribs, elementsOf(particleAttribs))) == TEXTURE_NONE) return false;

	// Create a dummy vertex buffer for R2VB. Use a minimal one for the release runtime in order to not waste any memory.
	if ((dummy = renderer->addVertexBuffer(debugRuntime? 4 * COUNT * (supportsHalf? sizeof(half4) : sizeof(float4)) : 1)) == VB_NONE) return false;


	currSmoke = 0;
	needsInit = true;

	return true;
}

bool App::unload(){
	return true;
}

void App::drawSkybox(const float4x4 &mvp){
	static const float vertices[] = { 0, 1, 0,  0, -1, -1,  1, -1, 1,  -1, -1, 1 };
	static const ushort indices[] = { 0, 1, 2, 3, 0, 1 };

	renderer->setShader(skybox);
	renderer->setVertexFormat(skyboxVF);
	renderer->setTexture("Skybox", env);
	renderer->setMask(COLOR);
	renderer->apply();

	renderer->changeShaderConstant4x4f("mvp", mvp);

	dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, 4, 4, indices, D3DFMT_INDEX16, vertices, 3 * sizeof(float));
}

void App::animateSmoke(const float3 &windDir){
	TextureID rt[] = { smokePosRT[currSmoke], smokeDirRT[currSmoke]};
	renderer->changeRenderTargets(elementsOf(rt), rt);

	if (needsInit){
		renderer->setShader(initSmoke);
		needsInit = false;
	} else {
		renderer->setShader(smoke);
		renderer->setTexture("Ground", ground);
		renderer->setTexture("Pos", smokePosRT[1 - currSmoke]);
		renderer->setTexture("Dir", smokeDirRT[1 - currSmoke]);
		renderer->setShaderConstant1f("frameTime", frameTime);
		// Wind and gravity combined, and a friction constant
		renderer->setShaderConstant4f("force", float4(frameTime * (windDir - float3(0, 0.1f, 0)), powf(0.85f, frameTime)));
	}
	renderer->setTexture("InitialPos", smokeSource);

	renderer->setShaderConstant2f("halfPixel", float2(0.5f / SIZE_X, 0.5f / SIZE_Y));
	renderer->setVertexFormat(quadVF);
	renderer->setVertexBuffer(quadVB);
	renderer->setDepthFunc(DEPTH_NONE);
	renderer->apply();

	dev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void App::assignDistance(){
	/*
		This function computes the distance from the camera
		to each particle. This is used in the sorting pass.
	*/
	renderer->setShader(distance);
	renderer->setShaderConstant3f("camPos", camPos);

	renderer->setShaderConstant2f("halfPixel", float2(0.5f / SIZE_X, 0.5f / SIZE_Y));
	renderer->setVertexFormat(quadVF);
	renderer->setVertexBuffer(quadVB);
	renderer->setDepthFunc(DEPTH_NONE);
	renderer->apply();


	TextureID rt[] = { posRT[0], distRT[0]};
	renderer->changeRenderTargets(elementsOf(rt), rt);

		float ratio = float(SMOKE_Y) / SIZE_Y;

		// Compute for the smoke particles into the upper part of the render target
		renderer->changeTexture("Tex", smokePosRT[currSmoke]);
		renderer->changeShaderConstant2f("scaleBias", float2(ratio, ratio - 1));

		dev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		// Compute for the foliage and fire into the lower part of the render target
		renderer->changeTexture("Tex", foliageSource);
		renderer->changeShaderConstant2f("scaleBias", float2(1 - ratio, ratio));

		dev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


	// Set up the initial source and destination for the sort pass that follows
	currSrc = 0;
	currDst = 1;
}

// This function does a bitonic merge operation in horizontal direction
void App::merge(const int start, const int end){
	renderer->changeShaderConstant1f("step1", 0.5f * start);

	for (int s = start; s <= end; s *= 2){
		TextureID rt[] = { posRT[currDst], distRT[currDst]};
		renderer->changeRenderTargets(elementsOf(rt), rt);

		renderer->changeTexture("Tex", posRT[currSrc]);
		renderer->changeTexture("Dist", distRT[currSrc]);
		renderer->changeShaderConstant4f("offset", float4(0.5f / s, 0, 0, 0));
		renderer->changeShaderConstant1f("step0", float(s));

		dev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		// Toggle source and destination
		currSrc = currDst;
		currDst = 1 - currSrc;
	}
}

// This function does a bitonic merge operation in vertical direction
void App::mergeRows(const int start, const int end){
	renderer->changeShaderConstant1f("step1", 0.5f * start);

	int s = start;
	for (int s = start; s <= end; s *= 2){
		TextureID rt[] = { posRT[currDst], distRT[currDst]};
		renderer->changeRenderTargets(elementsOf(rt), rt);

		renderer->changeTexture("Tex", posRT[currSrc]);
		renderer->changeTexture("Dist", distRT[currSrc]);
		renderer->changeShaderConstant4f("offset", float4(0, 0.5f / s, 0, 0));
		renderer->changeShaderConstant1f("step0", float(s));

		dev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		// Toggle source and destination
		currSrc = currDst;
		currDst = 1 - currSrc;
	}
}

void App::sort(){
	renderer->setShader(bitonicSort);
	renderer->setShaderConstant2f("halfPixel", float2(0.5f / SIZE_X, 0.5f / SIZE_Y));
	renderer->setVertexFormat(quadVF);
	renderer->setVertexBuffer(quadVB);
	renderer->setDepthFunc(DEPTH_NONE);
	renderer->apply();

	renderer->changeShaderConstant1f("step2", 0.5f * SIZE_Y);
	renderer->changeShaderConstant2f("select", float2(1, 0));

	// A full sort in horizontal direction
	int colStart = SIZE_X / 2;
	int colEnd   = SIZE_X / 2;
	while (colStart >= 1){
		merge(colStart, colEnd);
		colStart /= 2;
	}

	int rowStart = SIZE_Y / 2;
	int rowEnd   = SIZE_Y / 2;
	while (rowStart >= 1){
		// Do a vertical bitonic merge
		renderer->changeShaderConstant1f("step2", 0);
		renderer->changeShaderConstant2f("select", float2(0, 1));
		mergeRows(rowStart, rowEnd);
		rowStart /= 2;

		// Do the horizontal bitonic merge again
		renderer->changeShaderConstant1f("step2", (float) rowStart);
		renderer->changeShaderConstant2f("select", float2(1, 0));
		int colEnd = SIZE_X / 2;
		merge(1, colEnd);
	}
}

void App::quadify(const float3 &dx, const float3 &dy){
	renderer->changeRenderTarget(particlesRT);

	renderer->setShader(makeQuads);
	renderer->setTexture("Pos", posRT[currSrc]);
	renderer->setTexture("Dist", distRT[currSrc]);
	renderer->setShaderConstant2f("halfPixel", float2(0.5f / (4 * SIZE_X), 0.5f / SIZE_Y));
	renderer->setShaderConstant1f("width", SIZE_X);
	renderer->setShaderConstant3f("dx", 0.035f * dx);
	renderer->setShaderConstant3f("dy", 0.035f * dy);
	renderer->setVertexFormat(quadVF);
	renderer->setVertexBuffer(quadVB);
	renderer->setDepthFunc(DEPTH_NONE);
	renderer->apply();

	dev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void App::drawFrame(){
	float4x4 projection = perspectiveMatrixX(PI / 2, width, height, 0.001f, 10, true);
	float4x4 modelview = rotateXY4(-wx, -wy);

	mat4 skyboxMvp = pegToFarPlane(projection * modelview);

	modelview.translate(-camPos);
	float4x4 mvp = projection * modelview;

	// Compute a wind direction
	float3 windDir;
	windDir.x = 0.20f * noise1(-0.125f * time);
	windDir.y = 0.15f * noise1( 0.500f * time + 19.291f);
	windDir.z = 0.20f * noise1( 0.125f * time);


	/*
		Animate, sort and quadify all the particles.
	*/
	animateSmoke(windDir);
	assignDistance();
	sort();
	quadify(modelview.getRightVec(), modelview.getUpVec());

	renderer->resetRenderTarget();




	dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	/*
		Render terrain
	*/
	renderer->setCullFace(BACK);
	renderer->setShader(terrain);
	renderer->setTexture("Texture", terrainTex);
	renderer->setTexture("Light", terrainLight);
	renderer->setShaderConstant4x4f("mvp", mvp);
	renderer->setVertexFormat(terrainVF);
	renderer->setVertexBuffer(terrainVB);
	renderer->setIndexBuffer(terrainIB);
	renderer->apply();

	dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, TERRAIN_SIZE * TERRAIN_SIZE, 0, getRectangleIndexCount(TERRAIN_SIZE, TERRAIN_SIZE) - 2);


	// Render the skybox
	drawSkybox(skyboxMvp);


	/*
		Render the particles
	*/
	renderer->setShader(particles);
	renderer->setTexture("Foliage", foliage);
	renderer->setTexture("Light", terrainLight);
	renderer->setTexture("Smoke", smokeTex);
	renderer->setTexture("Noise", noise);
	renderer->setTexture("FirePalette", firePalette);
	renderer->setShaderConstant4x4f("mvp", mvp);
	renderer->setShaderConstant3f("dx", 0.04f * modelview.getRightVec());
	renderer->setShaderConstant1f("time", 0.1f * time);
	renderer->setVertexFormat(particleVF);
	renderer->setVertexBuffer(dummy, 0);
	renderer->setVertexBuffer(texCoords, 1);
	renderer->setIndexBuffer(particleIB);
	renderer->setMask(COLOR);
	renderer->setBlending(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	renderer->apply();


	// Enable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));

	// Bind the render target to the DMAP sampler
	dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(particlesRT));
	// Tell the driver that stream 0 is to be fetched from the DMAP texture
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DMAP));

	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4 * COUNT, 0, COUNT * 2);

	// Stream restored to regular vertex buffer mode
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DIS));
	// Unbind the DMAP texture
	dev->SetTexture(D3DDMAPSAMPLER, NULL);

	// Disable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));


	currSmoke = 1 - currSmoke;
}
