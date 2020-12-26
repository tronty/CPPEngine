/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <STX/PerlinNoise.h>
#include <STX/KdTree.h>
#include <Util/ProgrammerArt.h>

#define TERRAIN_SIZE 128

#define TERRAIN_TEX_SIZE 128

// The number of passes for sorting is log(COUNT) * (log(COUNT) + 1) / 2.
#define SIZE_X 64
#define SIZE_Y 64
#define COUNT (SIZE_X * SIZE_Y)

// 25% of the particles are smoke
#define SMOKE_X SIZE_X
#define SMOKE_Y (SIZE_Y / 4)
#define SMOKE_EMITTERS 8

// The rest is foliage
#define FOLIAGE_X SIZE_X
#define FOLIAGE_Y (SIZE_Y - SMOKE_Y)

class App {
public:
#if 0
	TextureID smokeTex, noise, firePalette, ground, terrainLight;

	int currSrc, currDst;

	ShaderID bitonicSort, distance, makeQuads, initSmoke, smoke;

	TextureID posRT[2], distRT[2], particlesRT, smokePosRT[2], smokeDirRT[2];
	TextureID smokeSource, foliageSource, foliage;
	int currSmoke;

	ShaderID particles;
	TextureID particleVF;
	VertexBufferID texCoords;
	std::vector<__WORD__> particleIB_;
	IndexBufferID particleIB;

	VertexFormatID smokeVF, distanceVF, bitonicSortVF, makeQuadsVF, initSmokeVF;

	VertexBufferID quadVB;

	bool needsInit;
#else

	VertexFormatID smokeVF, distanceVF, bitonicSortVF, makeQuadsVF, initSmokeVF;

	TextureID foliage, smokeTex, noise, firePalette;
	TextureID ground;

	ShaderID terrain;
	TextureID terrainTex, terrainLight;
	VertexFormatID terrainVF;
	VertexBufferID terrainVB;
	IndexBufferID terrainIB;

	ShaderID skybox;
	VertexFormatID skyboxVF;
	TextureID env;
	stx_SkyBox sb;

	int currSrc, currDst;

	ShaderID bitonicSort, distance, makeQuads, initSmoke, smoke;

	TextureID posRT[2], distRT[2], particlesRT, smokePosRT[2], smokeDirRT[2];
	TextureID foliageSource, smokeSource;
	int currSmoke;

	ShaderID particles;
	TextureID particleVF;
	//VertexBufferID dummy;
	VertexBufferID texCoords;
	IndexBufferID particleIB;

	VertexFormatID quadVF;
	VertexBufferID quadVB;

	bool needsInit;
#endif
	float speed, wx,wy;
	D3DXFROMWINEVECTOR3 camPos;

void resetCamera(){
	camPos = D3DXFROMWINEVECTOR3(0.086f, 0.05f, -0.43f);
	wx = 0.069f;
	wy = 2.1f;
}

// The height of the terrain
float terrainY(const float x, const float z){
	return 0.2f * turbulence2(x, z, 4);
}

// Function to compute the normal of the terrain
D3DXFROMWINEVECTOR3 terrainNormal(const float x, const float z){
	return normalize(cross(D3DXFROMWINEVECTOR3(0, terrainY(x, z + 0.005f) - terrainY(x, z - 0.005f), 0.01f), D3DXFROMWINEVECTOR3(0.01f, terrainY(x + 0.005f, z) - terrainY(x - 0.005f, z), 0)));
}

void moveCamera(const D3DXFROMWINEVECTOR3 &dir){
	camPos += dir * (speed * timeGetTime());

	// Compute the tangent plane of the terrain
	float ty0 = terrainY(camPos.x, camPos.z);
	float ty1 = terrainY(camPos.x + 0.01f, camPos.z);
	float ty2 = terrainY(camPos.x, camPos.z + 0.01f);

	D3DXFROMWINEVECTOR3 normal = normalize(cross(D3DXFROMWINEVECTOR3(0, ty2 - ty0, 0.01f), D3DXFROMWINEVECTOR3(0.01f, ty1 - ty0, 0)));
	float offset = -dot(D3DXFROMWINEVECTOR3(camPos.x, ty0, camPos.z), normal);

	// Do collision detection against the tangent plane
	float d = dot(camPos, normal) + offset;
	if (d < 0.02f){
		camPos += (0.02f - d) * normal;
	}
}

int init(const char* aTitle)
{
	bool supportsHalf = false;// IRenderer::GetRendererInstance()->supportsHalf();
	bool debugRuntime=false;

	speed = 0.3f;
	stx_srand(16);
	initPerlin();

	// Load all the shaders
	terrain     = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Sort/terrain.shd", "main", "main");
	smoke       = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Sort/smoke.shd", "main", "main");
	distance    = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Sort/distance.shd", "main", "main");
	bitonicSort = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Sort/bitonicSort.shd", "main", "main");
	makeQuads   = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Sort/makeQuads.shd", "main", "main");		
	initSmoke   = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Sort/initSmoke.shd", "main", "main");
	particles   = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Sort/particles.shd", "main", "main");
	skybox      = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Sort/skybox.shd", "main", "main");

	for (int i = 0; i < 2; i++){
		// Render targets for sorting
		posRT[i]  = IRenderer::GetRendererInstance()->addRenderTarget(SIZE_X, SIZE_Y, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());
		distRT[i] = IRenderer::GetRendererInstance()->addRenderTarget(SIZE_X, SIZE_Y, FORMAT_RG32f,   IRenderer::GetRendererInstance()->GetnearestClamp());

		// Render targets for smoke animation
		smokePosRT[i] = IRenderer::GetRendererInstance()->addRenderTarget(SMOKE_X, SMOKE_Y, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());
		smokeDirRT[i] = IRenderer::GetRendererInstance()->addRenderTarget(SMOKE_X, SMOKE_Y, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());
	}

	// R2VB render target for expanding the particles to quads
	
    particlesRT = IRenderer::GetRendererInstance()->addRenderTarget(
		4 * SIZE_X, SIZE_Y,
		supportsHalf? FORMAT_RGBA16F : FORMAT_RGBA32F, 
		IRenderer::GetRendererInstance()->GetnearestClamp());

	// Index buffer for the particles
	uint *indices = new uint[6*COUNT];
	for (int i = 0; i < COUNT; i++){
		indices[6 * i + 0] = 4 * i;
		indices[6 * i + 1] = 4 * i + 1;
		indices[6 * i + 2] = 4 * i + 2;
		indices[6 * i + 3] = 4 * i;
		indices[6 * i + 4] = 4 * i + 2;
		indices[6 * i + 5] = 4 * i + 3;
	}
 
	particleIB = IRenderer::GetRendererInstance()->addIndexBuffer(6*COUNT, sizeof(uint), STATIC, indices);

	// Base texture coordinates for the particles
	D3DXFROMWINEVECTOR2 *tc = new D3DXFROMWINEVECTOR2[4 * COUNT];
	for (int i = 0; i < COUNT; i++){
		tc[4 * i + 0] = float2(0, 0);
		tc[4 * i + 1] = float2(1, 0);
		tc[4 * i + 2] = float2(1, 1);
		tc[4 * i + 3] = float2(0, 1);
	}
	texCoords = IRenderer::GetRendererInstance()->addVertexBuffer(4*COUNT*sizeof(D3DXFROMWINEVECTOR2), STATIC, tc);

	// Ground texture
	terrainTex = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB-Sort/Ground_12-1.dds", false, IRenderer::GetRendererInstance()->GettrilinearAniso());

	// Vertex declaration for the terrain
	FormatDesc terrainAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	terrainVF = IRenderer::GetRendererInstance()->addVertexFormat(terrainAttribs, elementsOf(terrainAttribs), terrain);

	// Vertex buffer for the terrain
	D3DXFROMWINEVECTOR3 *dest_ = new D3DXFROMWINEVECTOR3[TERRAIN_SIZE * TERRAIN_SIZE];
    D3DXFROMWINEVECTOR3 *dest = dest_;
	for (int z = 0; z < TERRAIN_SIZE; z++){
		for (int x = 0; x < TERRAIN_SIZE; x++){
			float tx = float(x) / (TERRAIN_SIZE - 1) * 2 - 1;
			float tz = 1 - 2 * float(z) / (TERRAIN_SIZE - 1);
			*dest++ = D3DXFROMWINEVECTOR3(tx, terrainY(tx, tz), tz);
		}
	}
	if ((terrainVB = IRenderer::GetRendererInstance()->addVertexBuffer(TERRAIN_SIZE * TERRAIN_SIZE * sizeof(D3DXFROMWINEVECTOR3), STATIC, dest_)) == TEXTURE_NONE) return false;

	// Index buffer for terrain
	{uint* indices = new uint[getRectangleIndexCount(TERRAIN_SIZE, TERRAIN_SIZE)];
		fillRectangleIB(indices, TERRAIN_SIZE, TERRAIN_SIZE);
	if ((terrainIB = IRenderer::GetRendererInstance()->addIndexBuffer(getRectangleIndexCount(TERRAIN_SIZE, TERRAIN_SIZE), sizeof(int), STATIC, indices)) == TEXTURE_NONE) return false;}

	// Create ground and lighting texture
	Image3 gImg, tImg;
	ubyte *gDest = (ubyte *) gImg.create(FORMAT_I8, TERRAIN_TEX_SIZE, TERRAIN_TEX_SIZE, 1, 1);
	ubyte *tDest = (ubyte *) tImg.create(FORMAT_I8, TERRAIN_TEX_SIZE, TERRAIN_TEX_SIZE, 1, 1);

	D3DXFROMWINEVECTOR3 lightDir = normalize(D3DXFROMWINEVECTOR3(-1, 1, 0));
	for (int z = 0; z < TERRAIN_TEX_SIZE; z++){
		for (int x = 0; x < TERRAIN_TEX_SIZE; x++){
			float tx = float(x) / (TERRAIN_TEX_SIZE - 1) * 2 - 1;
			float tz = float(z) / (TERRAIN_TEX_SIZE - 1) * 2 - 1;

			float y = terrainY(tx, tz);
			D3DXFROMWINEVECTOR3 normal = terrainNormal(tx, tz);

			*gDest++ = (ubyte) (255.0f * (y / 0.2f * 0.5f + 0.5f));
			*tDest++ = (ubyte) (255.0f * saturate(dot(normal, lightDir)));
		}
	}
	tImg.generateMipMaps();
#if 0
	// Compress the textures. Use ATI1N if available to get better quality.
	gImg.compress(supportsATI1N? FORMAT_ATI1N : FORMAT_DXT1);
	tImg.compress(supportsATI1N? FORMAT_ATI1N : FORMAT_DXT1);
#endif
	ground = IRenderer::GetRendererInstance()->addTexture(&gImg, false, IRenderer::GetRendererInstance()->GetlinearClamp());
	terrainLight = IRenderer::GetRendererInstance()->addTexture(&tImg, false, IRenderer::GetRendererInstance()->GetbilinearClamp());

	// Load the textures for the foliage and create a texture atlas.
	Image3 atlas, grass0, grass1, leaves;
	std::string fn="/new/R2VB_VTF/R2VB-Sort/grass001.png";
	grass0.loadImageLibImage(fn.c_str());
	fn="/new/R2VB_VTF/R2VB-Sort/grass002.png";
	grass1.loadImageLibImage(fn.c_str());
	fn="/new/R2VB_VTF/R2VB-Sort/MapleLeaves.png";	
	leaves.loadImageLibImage(fn.c_str());

#if 1
	// Much easier to deal with uncompressed ...
	grass0.decompress();
	grass1.decompress();
	leaves.decompress();
	ubyte *fDest = atlas.create(FORMAT_RGBA8, 1024, 1024, 1, 1);
#endif
#if 0 // ???
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
#endif

	/*
		Distribute foliage over the terrain. To make the distribution fairly even a KD-tree is used
		to check whether an incoming location is too close to any existing location. This way all
		pieces of foliage are guaranteed to be at least maxDist distance apart.
	*/
	KDTree <float, 2> tree;
	float maxDist = 1.5f / sqrtf(COUNT);

	Image3 foliageImg;
	half *foliageDest = (half *) foliageImg.create(FORMAT_RGBA16f, FOLIAGE_X, FOLIAGE_Y, 1, 1);
	for (int i = 0; i < FOLIAGE_X * FOLIAGE_Y; i++){
		float tx, tz;

		do {
			tx = float(stx_rand()) / RAND_MAX * 2 - 1;
			tz = float(stx_rand()) / RAND_MAX * 2 - 1;
		} while (tree.hasPointWithin(D3DXFROMWINEVECTOR2(tx, tz), maxDist));
		tree.add(D3DXFROMWINEVECTOR2(tx, tz));

		foliageDest[4 * i + 0] = tx;
		foliageDest[4 * i + 1] = min(min(min(min(terrainY(tx, tz), terrainY(tx - 0.02f, tz)), terrainY(tx + 0.02f, tz)), terrainY(tx, tz - 0.02f)), terrainY(tx, tz + 0.02f));
		foliageDest[4 * i + 2] = tz;
		foliageDest[4 * i + 3] = -float(stx_rand() + 1) / (RAND_MAX + 2);
	}


	// Add fires and smoke sources
	Image3 smokeImg;
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

	// Create a noise texture made specially for fire and store a cached copy to disc
#define NSIZE 64
#define SCALE 0.1482f
	Image3 img;
	//fn=stx_convertpath("/new/R2VB_VTF/R2VB-Sort/FireNoise.dds");
	//img.loadImageLibImage(fn.c_str());
	{
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
		//img.compress(supportsATI1N? FORMAT_ATI1N : FORMAT_DXT1);
		img.saveDDS("/new/R2VB_VTF/R2VB-Sort/FireNoise.dds");
	}

	// Load the vertex declarations
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2
	};
	smokeVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), smoke);
	distanceVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), distance);
	bitonicSortVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), bitonicSort);
	makeQuadsVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), makeQuads);
	initSmokeVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), initSmoke);

	// Create a minimal vertex buffer for a quad. We'll use it frequently in the sort passes.
	float vertices[]  = { -1, -1,  1, -1,  -1, 1,  1, 1 };
	if ((quadVB = IRenderer::GetRendererInstance()->addVertexBuffer(sizeof(vertices), STATIC, vertices)) == VB_NONE) return false;

	FormatDesc format2[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
		1, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
	};
	particleVF = IRenderer::GetRendererInstance()->addVertexFormat(format2, elementsOf(format2), particles);

	currSmoke = 0;
	needsInit = true;

#if 1
	foliage = IRenderer::GetRendererInstance()->addTexture(&atlas, false, IRenderer::GetRendererInstance()->GettrilinearAniso());

	// Load the smoke animation texture
	smokeTex=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB-Sort/SmokeAnimation.dds", false, IRenderer::GetRendererInstance()->GettrilinearClamp());

	foliageSource = IRenderer::GetRendererInstance()->addTexture(&foliageImg, false, IRenderer::GetRendererInstance()->GetnearestClamp());
	smokeSource = IRenderer::GetRendererInstance()->addTexture(&smokeImg, false, IRenderer::GetRendererInstance()->GetnearestClamp());

	noise = IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());
	firePalette = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB-Sort/FirePalette.png", false, IRenderer::GetRendererInstance()->GetlinearClamp());

#endif
    sb.init("Mountains", "jpg");
	return true;
}

void drawSkybox(const D3DXFROMWINEMATRIX &mvp){
    #if 0
	static const float vertices[] = { 0, 1, 0,  0, -1, -1,  1, -1, 1,  -1, -1, 1 };
	static const ushort indices[] = { 0, 1, 2, 3, 0, 1 };

	IRenderer::GetRendererInstance()->setShader(skybox);
	IRenderer::GetRendererInstance()->setVertexFormat(skyboxVF);
	IRenderer::GetRendererInstance()->setTexture("Skybox", env);
	IRenderer::GetRendererInstance()->setMask(eCOLOR);
	

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);

	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLE_STRIP, 0, 4, 4, indices, indices, CONSTANT_INDEX2, vertices, vertices, 3 * sizeof(float));
	#else
	sb.render();
	#endif
}

void animateSmoke(const D3DXFROMWINEVECTOR3 &windDir){
	TextureID rt[] = { smokePosRT[currSmoke], smokeDirRT[currSmoke]};
	IRenderer::GetRendererInstance()->changeRenderTargets(rt, 2);

	if (needsInit){
		IRenderer::GetRendererInstance()->setShader(initSmoke);
		IRenderer::GetRendererInstance()->setVertexFormat(initSmokeVF);
		needsInit = false;
	} else {
		IRenderer::GetRendererInstance()->setShader(smoke);
		IRenderer::GetRendererInstance()->setVertexFormat(smokeVF);
		IRenderer::GetRendererInstance()->setTexture("Ground", ground);
		IRenderer::GetRendererInstance()->setTexture("Pos", smokePosRT[1 - currSmoke]);
		IRenderer::GetRendererInstance()->setTexture("Dir", smokeDirRT[1 - currSmoke]);
		IRenderer::GetRendererInstance()->setShaderConstant1f("time", timeGetTime());
		// Wind and gravity combined, and a friction constant

		D3DXFROMWINEVECTOR3 v3(timeGetTime() * (windDir - D3DXFROMWINEVECTOR3(0, 0.1f, 0)));
		D3DXFROMWINEVECTOR4 v4(v3.x, v3.y, v3.z, powf(0.85f, timeGetTime()));

		IRenderer::GetRendererInstance()->setShaderConstant4f("force", v4);
	}
	IRenderer::GetRendererInstance()->setTexture("InitialPos", smokeSource);

	IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / SIZE_X, 0.5f / SIZE_Y));
	IRenderer::GetRendererInstance()->setVertexBuffer(0, quadVB);
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	
	IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2);
}

void assignDistance(){
	/*
		This function computes the distance from the camera
		to each particle. This is used in the sorting pass.
	*/
	IRenderer::GetRendererInstance()->setShader(distance);
	IRenderer::GetRendererInstance()->setVertexFormat(distanceVF);
	IRenderer::GetRendererInstance()->setShaderConstant3f("camPos", camPos);

	IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / SIZE_X, 0.5f / SIZE_Y));
	IRenderer::GetRendererInstance()->setVertexBuffer(0, quadVB);
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

	TextureID rt[] = { posRT[0], distRT[0]};
	IRenderer::GetRendererInstance()->changeRenderTargets(rt, 2);

		float ratio = float(SMOKE_Y) / SIZE_Y;

		// Compute for the smoke particles into the upper part of the render target
		IRenderer::GetRendererInstance()->setTexture("Tex", smokePosRT[currSmoke]);
		IRenderer::GetRendererInstance()->setShaderConstant2f("scaleBias", D3DXFROMWINEVECTOR2(ratio, ratio - 1));

		IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2);

		// Compute for the foliage and fire into the lower part of the render target
		IRenderer::GetRendererInstance()->setTexture("Tex", foliageSource);
		IRenderer::GetRendererInstance()->setShaderConstant2f("scaleBias", D3DXFROMWINEVECTOR2(1 - ratio, ratio));

		IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2);


	// Set up the initial source and destination for the sort pass that follows
	currSrc = 0;
	currDst = 1;
}

// This function does a bitonic merge operation in horizontal direction
void merge(const int start, const int end){
	IRenderer::GetRendererInstance()->setShaderConstant1f("step1", 0.5f * start);

	for (int s = start; s <= end; s *= 2){
		TextureID rt[] = { posRT[currDst], distRT[currDst]};
		IRenderer::GetRendererInstance()->changeRenderTargets(rt, 2);

		IRenderer::GetRendererInstance()->setTexture("Tex", posRT[currSrc]);
		IRenderer::GetRendererInstance()->setTexture("Dist", distRT[currSrc]);
		IRenderer::GetRendererInstance()->setShaderConstant4f("offset", D3DXFROMWINEVECTOR4(0.5f / s, 0, 0, 0));
		IRenderer::GetRendererInstance()->setShaderConstant1f("step0", float(s));

		IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2);

		// Toggle source and destination
		currSrc = currDst;
		currDst = 1 - currSrc;
	}
}

// This function does a bitonic merge operation in vertical direction
void mergeRows(const int start, const int end){
	IRenderer::GetRendererInstance()->setShaderConstant1f("step1", 0.5f * start);

	int s = start;
	for (int s = start; s <= end; s *= 2){
		TextureID rt[] = { posRT[currDst], distRT[currDst]};
		IRenderer::GetRendererInstance()->changeRenderTargets(rt, 2);

		IRenderer::GetRendererInstance()->setTexture("Tex", posRT[currSrc]);
		IRenderer::GetRendererInstance()->setTexture("Dist", distRT[currSrc]);
		IRenderer::GetRendererInstance()->setShaderConstant4f("offset", D3DXFROMWINEVECTOR4(0, 0.5f / s, 0, 0));
		IRenderer::GetRendererInstance()->setShaderConstant1f("step0", float(s));

		IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2);

		// Toggle source and destination
		currSrc = currDst;
		currDst = 1 - currSrc;
	}
}

void sort(){
	IRenderer::GetRendererInstance()->setShader(bitonicSort);
	IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / SIZE_X, 0.5f / SIZE_Y));
	IRenderer::GetRendererInstance()->setVertexFormat(bitonicSortVF);
	IRenderer::GetRendererInstance()->setVertexBuffer(0, quadVB);
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

	IRenderer::GetRendererInstance()->setShaderConstant1f("step2", 0.5f * SIZE_Y);
	IRenderer::GetRendererInstance()->setShaderConstant2f("select", D3DXFROMWINEVECTOR2(1, 0));

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
		IRenderer::GetRendererInstance()->setShaderConstant1f("step2", 0);
		IRenderer::GetRendererInstance()->setShaderConstant2f("select", D3DXFROMWINEVECTOR2(0, 1));
		mergeRows(rowStart, rowEnd);
		rowStart /= 2;

		// Do the horizontal bitonic merge again
		IRenderer::GetRendererInstance()->setShaderConstant1f("step2", (float) rowStart);
		IRenderer::GetRendererInstance()->setShaderConstant2f("select", D3DXFROMWINEVECTOR2(1, 0));
		int colEnd = SIZE_X / 2;
		merge(1, colEnd);
	}
}

void quadify(const D3DXFROMWINEVECTOR3 &dx, const D3DXFROMWINEVECTOR3 &dy){
	IRenderer::GetRendererInstance()->changeRenderTarget(particlesRT);

	IRenderer::GetRendererInstance()->setShader(makeQuads);
	IRenderer::GetRendererInstance()->setVertexFormat(makeQuadsVF);
	IRenderer::GetRendererInstance()->setTexture("Pos", posRT[currSrc]);
	IRenderer::GetRendererInstance()->setTexture("Dist", distRT[currSrc]);
	IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / (4 * SIZE_X), 0.5f / SIZE_Y));
	IRenderer::GetRendererInstance()->setShaderConstant1f("width", SIZE_X);
	IRenderer::GetRendererInstance()->setShaderConstant3f("dx", 0.035f * dx);
	IRenderer::GetRendererInstance()->setShaderConstant3f("dy", 0.035f * dy);
	IRenderer::GetRendererInstance()->setVertexBuffer(0, quadVB);
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

	IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2);
}

void render()
{
	D3DXFROMWINEMATRIX projection;
	D3DXFROMWINEMatrixPerspectiveFovLH(&projection, D3DXFROMWINE_PI / 2, IRenderer::GetRendererInstance()->GetViewportWidth()*1.0f/IRenderer::GetRendererInstance()->GetViewportHeight(), 0.001f, 10);
	D3DXFROMWINEMATRIX modelview, T;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&modelview, -wx, -wy, 0.0f);

	D3DXFROMWINEMATRIX skyboxMvp = projection * modelview;

	D3DXFROMWINEMatrixTranslation(&T, -camPos.x, -camPos.y, -camPos.z);
	D3DXFROMWINEMATRIX mvp = projection * modelview*T;

	// Compute a wind direction
	D3DXFROMWINEVECTOR3 windDir;
	windDir.x = 0.20f * noise1(-0.125f * 1.0f*timeGetTime());
	windDir.y = 0.15f * noise1( 0.500f * 1.0f*timeGetTime() + 19.291f);
	windDir.z = 0.20f * noise1( 0.125f * 1.0f*timeGetTime());


	/*
		Animate, sort and quadify all the particles.
	*/
	animateSmoke(windDir);
	assignDistance();
	sort();
	quadify(getRightVec(modelview), getUpVec(modelview));

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();

	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));

	/*
		Render terrain
	*/
	IRenderer::GetRendererInstance()->setShader(terrain);
	IRenderer::GetRendererInstance()->setVertexFormat(terrainVF);
	IRenderer::GetRendererInstance()->setCullFace(eBACK);
	IRenderer::GetRendererInstance()->setTexture("Texture", terrainTex);
	IRenderer::GetRendererInstance()->setTexture("Light", terrainLight);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	IRenderer::GetRendererInstance()->setVertexBuffer(0, terrainVB);
	IRenderer::GetRendererInstance()->setIndexBuffer(terrainIB);
	

	IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLE_STRIP, 0, 0, TERRAIN_SIZE * TERRAIN_SIZE, 0, getRectangleIndexCount(TERRAIN_SIZE, TERRAIN_SIZE) - 2);


	// Render the skybox
	drawSkybox(skyboxMvp);

	/*
		Render the particles
	*/
	IRenderer::GetRendererInstance()->setShader(particles);
	IRenderer::GetRendererInstance()->setVertexFormat(particleVF);
	IRenderer::GetRendererInstance()->setTexture("Foliage", foliage);
	IRenderer::GetRendererInstance()->setTexture("Light", terrainLight);
	IRenderer::GetRendererInstance()->setTexture("Smoke", smokeTex);
	IRenderer::GetRendererInstance()->setTexture("Noise", noise);
	IRenderer::GetRendererInstance()->setTexture("FirePalette", firePalette);
	IRenderer::GetRendererInstance()->setTexture("DMAP", particlesRT);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	IRenderer::GetRendererInstance()->setShaderConstant3f("dx", 0.04f * getRightVec(modelview));
	IRenderer::GetRendererInstance()->setShaderConstant1f("time", 0.1f * timeGetTime());
	IRenderer::GetRendererInstance()->setVertexBuffer(0, texCoords);
	IRenderer::GetRendererInstance()->setIndexBuffer(particleIB);
	IRenderer::GetRendererInstance()->setMask(eCOLOR);
	IRenderer::GetRendererInstance()->setBlending(eSRC_ALPHA, eONE_MINUS_SRC_ALPHA);

	// Bind the render target to the DMAP sampler
	//IRenderer::GetRendererInstance()->setTexture("DMAP"\, particlesRT));

	IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, 4 * COUNT, 0, COUNT * 2);
	currSmoke = 1 - currSmoke;
}
};
App app;
int init(const char* aTitle)
{
	app.init(aTitle);	
	return 0;
}

void render()
{
	IRenderer::GetRendererInstance()->BeginScene();
	app.render();
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present();
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("R2VB-Sort");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	STX_PRINT("Function init finished!\n");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

