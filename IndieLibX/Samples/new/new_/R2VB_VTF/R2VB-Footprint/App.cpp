/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//
// This sample uses the R2VB_ technique on many fields. In this sample, we use R2VB_
// for terrain deformation, heightmap based collision detection, skinning animation
// and the particle system. We do all calculation in the GPU except AI.
// We use one 512x512 R2VB_ texture for terrain generation, and another 512x152
// render target for real time deformation map. This deformation map is used
// to deform the original terrain mesh. We also implement the accumulation of snow,
// so the footprint will be filled up after snowing.
//-----------------------------------------------------------------------------

#define PLAYER_SCALE					90.0f
#define D2R								(D3DXFROMWINE_PI/180.0f)
#define TERRAIN_SCALE					(1024.0f*8.0f)
#define DEPTH_IN_SNOW					3.0f*PLAYER_SCALE


#include <Util/ATIModel.h>
#include <Util/R2VBAnimation.h>
#include "SnowSystem.h"

#define RT_SIZE 512

class Player : public R2VBAnimation {
public:
	ShaderID footprintShader;
	VertexFormatID footprintVF;
	VertexBufferID footprintVB;
	D3DXFROMWINEMATRIX Trans;
	float Deg;
	bool bBounce;
	bool bInSafeZone;
	TextureID heightMap;

	bool load(char *modelname, char *diffuse, __DWORD__ maxtexsize, bool debugruntime);	
	virtual void psCalculateAnimation(float t, float interp);
};


typedef struct {
	VertexFormatID VF;
	TextureID R2VB_;
	//VertexBufferID R2VB_DummyVBO;
	VertexBufferID texCoordBuffer;
	IndexBufferID IB;

	TextureID heightMap, heightMapAccumulated, deformedHeightmap, terrainBuffer, snowDiffuse, snowBump, deformMap, deformMapDepth, noiseFillMap;
	TextureID blurDeformMap[2];

	VertexFormatID QuadVF, generateHeightVF, accumulateHeightVF, deformVF, blurVF;
	ShaderID generateHeightShd, accumulateHeightShd, deformShd, blurShd;
}Terrain;

//-----------------------------------------------------------------------------------------
// Load a model animation into R2VB_ animation class. We override this function because
// we need to load an extra shader for rendering footprint.
//-----------------------------------------------------------------------------------------
bool Player::load(char *modelname, char *diffuse, __DWORD__ maxtexsize, bool debugruntime)
{
	R2VBAnimation::load(modelname, diffuse, maxtexsize, debugruntime, "/new/R2VB_VTF/R2VB-Footprint");
	footprintShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Footprint/footprint.shd", "main", "main");
	
	bool supportsHalf = false;// IRenderer::GetRendererInstance()->supportsHalf();
	const FormatDesc fdesc[] = {
		0, TYPE_VERTEX,   supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 2,
	};
	footprintVF=IRenderer::GetRendererInstance()->addVertexFormat(fdesc, elementsOf(fdesc), footprintShader);
	unsigned int N=1;footprintVB=IRenderer::GetRendererInstance()->addVertexBuffer(N*3*sizeof(float), STATIC); // N=1 ???

	setRange(51.0f, 76.0f);
	setTime(51.0f);

	Deg = 0.0f;

	transformMatrix.m[0][0] *= PLAYER_SCALE;
	transformMatrix.m[1][1] *= PLAYER_SCALE;
	transformMatrix.m[2][2] *= PLAYER_SCALE;

	Trans = transformMatrix;

	bBounce = false;
	bInSafeZone = true;

	return true;
}
//-----------------------------------------------------------------------------------------
// Using pixel shader for animation calculation. We override this function because we need
// to do transform at this point. It will output transformed bone matrices to the texture.
//-----------------------------------------------------------------------------------------
void Player::psCalculateAnimation(float t, float interp)
{
	IRenderer::GetRendererInstance()->changeRenderTarget(boneMatrix);

		IRenderer::GetRendererInstance()->setShader(animationSH);
		IRenderer::GetRendererInstance()->setVertexFormat(QuadVF);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, QuadVB);
		IRenderer::GetRendererInstance()->setTexture("boneAnimation", boneAnimation);
		IRenderer::GetRendererInstance()->setTexture("heightmap", heightMap);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

		IRenderer::GetRendererInstance()->setShaderConstant4f("time_interp", D3DXFROMWINEVECTOR4(t-1, t, interp, 0.0f));
		IRenderer::GetRendererInstance()->setShaderConstant1f("XBias", 0.1f/(numBones*4.0f));
		IRenderer::GetRendererInstance()->setShaderConstant1f("iBoneAnimationHeight", 1.0f/numFrames);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("TransM", transformMatrix);
		IRenderer::GetRendererInstance()->setShaderConstant4f("Param", D3DXFROMWINEVECTOR4(TERRAIN_SCALE, DEPTH_IN_SNOW, 512.0f*3.0f, 0));

		IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_FAN, 0, 2);
}

class App {
public:
	App() {bSnowing = false; currentBlurMap = 0; accu = 0.0f; };

//-----------------------------------------------------------------------------------------
// Make a rotation matrix with respect to Y axis.
//-----------------------------------------------------------------------------------------
void yawMatrix(D3DXFROMWINEMATRIX& m, float deg)
{
	float Rad = deg*D2R;
	float sn = sinf(Rad)*PLAYER_SCALE;
	float cs = cosf(Rad)*PLAYER_SCALE;

	m.m[0][0] = cs;
	m.m[2][0] = sn;

	m.m[0][2] = -sn;
	m.m[2][2] = cs;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void setup()
{
	BaseApp::initGUI();
	width  = 800;
	height = 600;
	speed= 10000;
}
//-----------------------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------------------
void resetCamera()
{
	camPos = D3DXFROMWINEVECTOR3(1500*3, 1000*3, 0);
	wx = 0.7f;
	wy = D3DXFROMWINE_PI / 2;
}
//-----------------------------------------------------------------------------------------
// We render a heightmap texture into R2VB_ texture to generate terrain vertex buffer.
//-----------------------------------------------------------------------------------------
void resetTerrain()
{
	static D3DXFROMWINEVECTOR4 vertices[] = {
		D3DXFROMWINEVECTOR4(-1, -1, 0, 0),
		D3DXFROMWINEVECTOR4( 1, -1, 1, 0),
		D3DXFROMWINEVECTOR4( 1,  1, 1, 1),
		D3DXFROMWINEVECTOR4(-1,  1, 0, 1),
	};

	if (1)
	{
		IRenderer::GetRendererInstance()->BeginScene();
		// Generate R2VB_ vertex buffer from heightmap
		IRenderer::GetRendererInstance()->changeRenderTarget(terrain.deformedHeightmap);

			IRenderer::GetRendererInstance()->setShader(terrain.generateHeightShd);
			IRenderer::GetRendererInstance()->setVertexFormat(terrain.generateHeightVF);
			IRenderer::GetRendererInstance()->setTexture("heightmap", terrain.heightMap);
			IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
			

			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));

		IRenderer::GetRendererInstance()->changeToMainFramebuffer();
		IRenderer::GetRendererInstance()->EndScene();
	}
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool createTerrain(char *heightmap)
{
	// load shaders
	terrain.generateHeightShd = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Footprint/generateHeight.shd", "main", "main");
	terrain.accumulateHeightShd = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Footprint/accumulateHeight.shd", "main", "main");
	terrain.deformShd = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Footprint/deformShd.shd", "main", "main");
	terrain.blurShd = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Footprint/blur.shd", "main", "main");

	{FormatDesc vertexAttribs[] = {
		0, TYPE_VERTEX,		FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD,	FORMAT_FLOAT, 2,
	};
	
	terrain.generateHeightVF = IRenderer::GetRendererInstance()->addVertexFormat(vertexAttribs, elementsOf(vertexAttribs), terrain.generateHeightShd);
	terrain.accumulateHeightVF = IRenderer::GetRendererInstance()->addVertexFormat(vertexAttribs, elementsOf(vertexAttribs), terrain.accumulateHeightShd);
	terrain.deformVF = IRenderer::GetRendererInstance()->addVertexFormat(vertexAttribs, elementsOf(vertexAttribs), terrain.deformShd);}
	{FormatDesc vertexAttribs[] = {
		0, TYPE_VERTEX,		FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD,	FORMAT_FLOAT, 2,
	};
	terrain.blurVF = IRenderer::GetRendererInstance()->addVertexFormat(vertexAttribs, elementsOf(vertexAttribs), terrain.blurShd);}

	FormatDesc vertexAttribs_[] = {
		0, TYPE_TEXCOORD,	FORMAT_FLOAT, 2,
		1, TYPE_VERTEX,		FORMAT_FLOAT, 1,
	};

	if ((terrain.VF = IRenderer::GetRendererInstance()->addVertexFormat(vertexAttribs_, elementsOf(vertexAttribs_), terrainSH)) == VF_NONE) return false;

	FormatDesc quadAttribs[] = {	0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
	                                0, TYPE_TEXCOORD, FORMAT_FLOAT, 2};
	if ((terrain.QuadVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs))) == VF_NONE) return false;

	char			Buffer[256];

	stx_snprintf(Buffer, 256, "%s%s", TEX_PATH, "/new/R2VB_VTF/R2VB/height1024_5.tga");

	terrain.heightMap = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/height1024_5.tga", false, IRenderer::GetRendererInstance()->GettrilinearAniso());


	stx_snprintf(Buffer, 256, "%s%s", TEX_PATH, "/new/R2VB_VTF/R2VB/height1024_7.tga");
	terrain.heightMapAccumulated = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/height1024_7.tga", false, IRenderer::GetRendererInstance()->GettrilinearAniso());


	stx_snprintf(Buffer, 256, "%s%s", TEX_PATH, "/new/R2VB_VTF/R2VB/noiseFillMap.png");
	terrain.noiseFillMap = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/noiseFillMap.png", false, IRenderer::GetRendererInstance()->GettrilinearAniso());

	// generate texture coordinate for terrain mesh
	std::vector<float> uv(RT_SIZE * RT_SIZE*2);
	float *dest = &uv[0];
		for (int z = 0; z < RT_SIZE; z++){
			float fz = float(2 * z) / (RT_SIZE - 1) - 1.0f;
			for (int x = 0; x < RT_SIZE; x++){
				float fx = float(2 * x) / (RT_SIZE - 1) - 1.0f;
				*dest++ = (TERRAIN_SCALE/2.0f) * fx;
				*dest++ = (TERRAIN_SCALE/2.0f) * fz;
				//dest++;
			}
		}
	terrain.texCoordBuffer = IRenderer::GetRendererInstance()->addVertexBuffer(RT_SIZE * RT_SIZE * 2* sizeof(float), STATIC, &uv[0]);

	// generate index buffer for terrain mesh
	std::vector<uint> vi(6 * (RT_SIZE - 1) * (RT_SIZE - 1));
	uint *iDest = &vi[0];
		for (int z = 0; z < RT_SIZE - 1; z++){
			for (int x = 0; x < RT_SIZE - 1; x++){
				*iDest++ = (z * RT_SIZE + x);
				*iDest++ = (z * RT_SIZE + x + 1);
				*iDest++ = ((z + 1) * RT_SIZE + x);
				*iDest++ = ((z + 1) * RT_SIZE + x);
				*iDest++ = (z * RT_SIZE + x + 1);
				*iDest++ = ((z + 1) * RT_SIZE + x + 1);
			}
		}
	terrain.IB = IRenderer::GetRendererInstance()->addIndexBuffer(6 * (RT_SIZE - 1) * (RT_SIZE - 1), sizeof(uint), STATIC, &vi[0]);

	// Create terrain R2VB_ texture. Since the render target will be used as vertex buffer
	// we need to signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	terrain.R2VB_ = IRenderer::GetRendererInstance()->addRenderTarget(RT_SIZE, RT_SIZE, FORMAT_R32F, IRenderer::GetRendererInstance()->GetlinearClamp());

	/*
		We need a dummy vertex buffer when we specify stride and offset when calling setVertexBuffer()
		for R2VB_. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
	int size = (RT_SIZE) * RT_SIZE * 4 * sizeof(float);
	//if ((terrain.R2VB_DummyVBO = IRenderer::GetRendererInstance()->addVertexBuffer(size)) == VB_NONE) return false;

	// create a render target for storing deformed terrain data
	terrain.deformedHeightmap = IRenderer::GetRendererInstance()->addRenderTarget(RT_SIZE, RT_SIZE, FORMAT_RGBA8, IRenderer::GetRendererInstance()->GetlinearClamp());

	// create a render target for buffering
	terrain.terrainBuffer = IRenderer::GetRendererInstance()->addRenderTarget(RT_SIZE, RT_SIZE, FORMAT_RGBA8, IRenderer::GetRendererInstance()->GetnearestClamp());

	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool load()
{
	// Load shaders
	//skybox = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Footprint/skybox.shd", "main", "main");
	terrainSH  = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Footprint/terrain.shd", "main", "main");
	dumpShd = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Footprint/DumpTexture.shd", "main", "main");

	{FormatDesc vertexAttribs[] = {
		0, TYPE_VERTEX,		FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD,	FORMAT_FLOAT, 2,
	};

	if ((terrainVF = IRenderer::GetRendererInstance()->addVertexFormat(vertexAttribs, elementsOf(vertexAttribs), terrainSH)) == VF_NONE) return false;}

	{FormatDesc vertexAttribs[] = {
		0, TYPE_VERTEX,		FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD,	FORMAT_FLOAT, 2,
	};

	if ((dumpVF = IRenderer::GetRendererInstance()->addVertexFormat(vertexAttribs, elementsOf(vertexAttribs), dumpShd)) == VF_NONE) return false;}

	// Load skybox
	#if 0
	env = IRenderer::GetRendererInstance()->addCubemap("Olympus", "jpg");
	#else
    	sb.init("/SkyBox/cubemaps/x/OlympusGL.dds");
	#endif

	//FormatDesc skyboxAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	//if ((skyboxVF = IRenderer::GetRendererInstance()->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs), -1)) == VF_NONE) return false;


	if (!createTerrain("/new/R2VB_VTF/R2VB/height1024_5.tga")) return false;


	if (!player.load("Footprint", "boy03.dds", IRenderer::GetRendererInstance()->GetMaxTextureWidth(), debugRuntime)) return false;//, "/new/R2VB_VTF/R2VB-Footprint")) return false;


	TextureID d3drt;
	// create render target and depth buffer for rendering footprint as real-time deformation map
	terrain.deformMap = IRenderer::GetRendererInstance()->addRenderTarget(RT_SIZE, RT_SIZE, FORMAT_RGBA8, IRenderer::GetRendererInstance()->GetnearestClamp());


	terrain.deformMapDepth  = IRenderer::GetRendererInstance()->addRenderDepthStencil(RT_SIZE, RT_SIZE, 16, 0);


	// create 2 textures for ping-pong bluring texture
	terrain.blurDeformMap[0] = IRenderer::GetRendererInstance()->addRenderTarget(RT_SIZE, RT_SIZE, FORMAT_RGBA8, IRenderer::GetRendererInstance()->GetnearestClamp());


	terrain.blurDeformMap[1] = IRenderer::GetRendererInstance()->addRenderTarget(RT_SIZE, RT_SIZE, FORMAT_RGBA8, IRenderer::GetRendererInstance()->GetnearestClamp());


	// load particle system
	snowSys.load( IRenderer::GetRendererInstance()->GetMaxTextureWidth(), IRenderer::GetRendererInstance()->GetMaxTextureHeight(), debugRuntime, true); // , (caps.DeclTypes & D3DDTCAPS_FLOAT16_4) != 0);


	onReset();

	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool onReset()
{
	resetTerrain();
	snowSys.reset();
	snowingTime = 0;

	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void drawSkybox(const D3DXFROMWINEMATRIX &mvp)
{
	#if 0
	static const float verts[] = { -1,-1,-1, -1,-1,1, -1,1,-1, -1,1,1, 1,-1,-1, 1,-1,1, 1,1,-1, 1,1,1 };
	static const unsigned short inds[] = { 2,3,0, 0,3,1, 3,7,1, 1,7,5, 7,6,5, 5,6,4, 6,2,4, 4,2,0, 6,7,2, 2,7,3, 0,1,4, 4,1,5 };

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
//-----------------------------------------------------------------------------------------
// Rendering deformation map to current terrain map to add deformation on terrain.
//-----------------------------------------------------------------------------------------
void deformTerrain()
{
	static D3DXFROMWINEVECTOR4 vertices[] = {
		D3DXFROMWINEVECTOR4(-1, -1, 0, 0),
		D3DXFROMWINEVECTOR4( 1, -1, 1, 0),
		D3DXFROMWINEVECTOR4( 1,  1, 1, 1),
		D3DXFROMWINEVECTOR4(-1,  1, 0, 1),
	};

	IRenderer::GetRendererInstance()->changeRenderTarget(terrain.deformedHeightmap);
		IRenderer::GetRendererInstance()->setShader(terrain.deformShd);
		IRenderer::GetRendererInstance()->setVertexFormat(terrain.QuadVF);
		IRenderer::GetRendererInstance()->setTexture("deformMap", terrain.blurDeformMap[!currentBlurMap]);
		
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
		IRenderer::GetRendererInstance()->setBlending(eONE, eONE, eBM_REVERSE_SUBTRACT);
		
		IRenderer::GetRendererInstance()->setShaderConstant1f("depth", 0.45f*step);
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));
}
//-----------------------------------------------------------------------------------------
// Using a noise texture and random texture coordinate for accumulating snow. 
// We render current terrain map to a buffering texture, then using buffering texture
// as input and add noise texture to form result terrain map.
//-----------------------------------------------------------------------------------------
void accumulateTerrain(TextureID &accumulatedHeightMap, float speed)
{
	static D3DXFROMWINEVECTOR4 vertices[] = {
		D3DXFROMWINEVECTOR4(-1, -1, 0, 0),
		D3DXFROMWINEVECTOR4( 1, -1, 1, 0),
		D3DXFROMWINEVECTOR4( 1,  1, 1, 1),
		D3DXFROMWINEVECTOR4(-1,  1, 0, 1),
	};

	IRenderer::GetRendererInstance()->changeRenderTarget(terrain.terrainBuffer);
		IRenderer::GetRendererInstance()->setShader(dumpShd);
		IRenderer::GetRendererInstance()->setVertexFormat(dumpVF);
		IRenderer::GetRendererInstance()->setTexture("DumpTex", terrain.deformedHeightmap);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
		
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));

	IRenderer::GetRendererInstance()->changeRenderTarget(terrain.deformedHeightmap);
		IRenderer::GetRendererInstance()->setShader(terrain.accumulateHeightShd);
		IRenderer::GetRendererInstance()->setVertexFormat(terrain.QuadVF);
		IRenderer::GetRendererInstance()->setTexture("currentMap", terrain.terrainBuffer);
		IRenderer::GetRendererInstance()->setTexture("accumulatedHeightMap", accumulatedHeightMap);
		IRenderer::GetRendererInstance()->setTexture("fillMap", terrain.noiseFillMap);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
		
		IRenderer::GetRendererInstance()->setShaderConstant1f("accumulateSpeed", speed);
		IRenderer::GetRendererInstance()->setShaderConstant2f("random", D3DXFROMWINEVECTOR2((rand()%137)/100.0f, (rand()%531)/100.0f));
		
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));	
}
//-----------------------------------------------------------------------------------------
// Generate R2VB_ vertex buffer from heightmap
//-----------------------------------------------------------------------------------------
void generateTerrain()
{
	static D3DXFROMWINEVECTOR4 vertices[] = {
		D3DXFROMWINEVECTOR4(-1, -1, 0, 0),
		D3DXFROMWINEVECTOR4( 1, -1, 1, 0),
		D3DXFROMWINEVECTOR4( 1,  1, 1, 1),
		D3DXFROMWINEVECTOR4(-1,  1, 0, 1),
	};

	IRenderer::GetRendererInstance()->changeRenderTarget(terrain.R2VB_);

		IRenderer::GetRendererInstance()->setShader(terrain.generateHeightShd);
		IRenderer::GetRendererInstance()->setVertexFormat(terrain.QuadVF);
		IRenderer::GetRendererInstance()->setTexture("heightmap", terrain.deformedHeightmap);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
		
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));	
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void drawTerrain(const D3DXFROMWINEMATRIX &mvp)
{
	IRenderer::GetRendererInstance()->setShader(terrainSH);
	IRenderer::GetRendererInstance()->setVertexFormat(terrain.VF);
	// Bind the render target to the DMAP sampler
	IRenderer::GetRendererInstance()->setTexture("DMAP", terrain.R2VB_);

	IRenderer::GetRendererInstance()->setVertexBuffer(0, terrain.texCoordBuffer);
	//IRenderer::GetRendererInstance()->setVertexBuffer(1, terrain.R2VB_DummyVBO); // Dummy vertex buffer. Only the stride and offset data will be used.
	IRenderer::GetRendererInstance()->setIndexBuffer(terrain.IB);
	

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	IRenderer::GetRendererInstance()->setShaderConstant1f("HeightFactor", 512.0f*3);
	
	IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, RT_SIZE * RT_SIZE, 0, 2 * (RT_SIZE - 1) * (RT_SIZE - 1));
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void AILoop()
{
	if (player.Deg > 360.0f)
	{
		player.Deg -= 360.0f;
	}
	else if (player.Deg < -360.0f)
	{
		player.Deg += 360.0f;
	}

	if (player.bBounce)
	{
		player.Deg += 3.0f;
		D3DXFROMWINEVECTOR3    v1, v2;
		float     d;
		v1 = D3DXFROMWINEVECTOR3(player.Trans.m[3][0], 0.0f, player.Trans.m[3][2]);
		v2 = D3DXFROMWINEVECTOR3(player.Trans.m[2][0], 0.0f, player.Trans.m[2][2]);
		v1 = normalize(v1);
		v2 = normalize(v2);
		d = dot(v1, v2);
		if (d > 0.9f)
			player.bBounce = false;

		yawMatrix(player.Trans, player.Deg);	
	}
	else
	{
		if (!(rand()%5) && player.bInSafeZone)
		{
			player.Deg -= 1.0f + (2 - rand()%4);
			yawMatrix(player.Trans, player.Deg);	
		}
	}
	// move forward
	player.Trans.m[3][0] -= player.Trans.m[2][0]*step*70.0f/PLAYER_SCALE;
	player.Trans.m[3][1] -= player.Trans.m[2][1]*step*70.0f/PLAYER_SCALE;
	player.Trans.m[3][2] -= player.Trans.m[2][2]*step*70.0f/PLAYER_SCALE;
	
	if (player.bInSafeZone)
	{
		if (player.Trans.m[3][0] > 3500.0f || player.Trans.m[3][0] < -3500.0f ||
			player.Trans.m[3][1] > 3500.0f || player.Trans.m[3][1] < -3500.0f ||
			player.Trans.m[3][2] > 3500.0f || player.Trans.m[3][2] < -3500.0f)
		{
			player.bBounce = true;
			player.bInSafeZone = false;
		}
		else if (player.Trans.m[3][0] < 2000.0f && player.Trans.m[3][0] > -2000.0f &&
			player.Trans.m[3][1] < 2000.0f && player.Trans.m[3][1] > -2000.0f &&
			player.Trans.m[3][2] < 2000.0f && player.Trans.m[3][2] > -2000.0f)
		{
			player.bBounce = false;
			player.bInSafeZone = false;
		}
	}
	else
	{
		if (player.Trans.m[3][0] < 3000.0f && player.Trans.m[3][0] > -3000.0f &&
			player.Trans.m[3][1] < 3000.0f && player.Trans.m[3][1] > -3000.0f &&
			player.Trans.m[3][2] < 3000.0f && player.Trans.m[3][2] > -3000.0f)
		{
			player.bInSafeZone = true;
			player.bBounce = false;
		}
	}

	player.setTransformMatrix(player.Trans);
}
//-----------------------------------------------------------------------------------------
// Blur the deformation map to get more smooth deformation.
//-----------------------------------------------------------------------------------------
void blurDeformMap(int times)
{
	static D3DXFROMWINEVECTOR4 vertices[] = {
		D3DXFROMWINEVECTOR4(-1, -1, 0, 0),
		D3DXFROMWINEVECTOR4( 1, -1, 1, 0),
		D3DXFROMWINEVECTOR4( 1,  1, 1, 1),
		D3DXFROMWINEVECTOR4(-1,  1, 0, 1),
	};

	for (int i=0; i<times; i++)
	{		
		IRenderer::GetRendererInstance()->changeRenderTarget(terrain.blurDeformMap[currentBlurMap]);
			IRenderer::GetRendererInstance()->setShader(terrain.blurShd);
			IRenderer::GetRendererInstance()->setVertexFormat(terrain.QuadVF);
			if (i == 0)
				IRenderer::GetRendererInstance()->setTexture("blur", terrain.deformMap);
			else
				IRenderer::GetRendererInstance()->setTexture("blur", terrain.blurDeformMap[!currentBlurMap]);
			IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
			
			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));

		currentBlurMap = !currentBlurMap;
	}
}
//-----------------------------------------------------------------------------------------
// We put a camera under the character and use parallel projection to render character to
// texture. This texture will be used as deformation map after blurring.
//-----------------------------------------------------------------------------------------
void renderDeformMap()
{
	D3DXFROMWINEMATRIX orth = orthoMatrix(-TERRAIN_SCALE, TERRAIN_SCALE, TERRAIN_SCALE, -TERRAIN_SCALE, TERRAIN_SCALE, -TERRAIN_SCALE, true);
	D3DXFROMWINEMATRIX modelview = rotateXY4(D3DXFROMWINE_PI/2, 0);
	modelview.translate(D3DXFROMWINEVECTOR3(-0, -5000, -0));
	D3DXFROMWINEMATRIX mvp = orth * modelview;

	D3DXFROMWINEVECTOR3 pos;
	player.getPosition(pos);

	IRenderer::GetRendererInstance()->changeRenderTarget(terrain.deformMap, terrain.deformMapDepth);
	IRenderer::GetRendererInstance()->Clear(true, true, stx_GetRGBA(0x00000000));

	IRenderer::GetRendererInstance()->setShader(player.footprintShader);
    	IRenderer::GetRendererInstance()->setVertexFormat(player.footprintVF);
    	IRenderer::GetRendererInstance()->setVertexBuffer(0, player.footprintVB);
	// Bind the render target to the DMAP sampler
	IRenderer::GetRendererInstance()->setTexture("DMAP", player.getR2VB_VBO());

	player.setupVertexSource();
	IRenderer::GetRendererInstance()->setDepthFunc(eLESS);
	IRenderer::GetRendererInstance()->setTexture("heightmap", terrain.deformedHeightmap);
	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	IRenderer::GetRendererInstance()->setShaderConstant1f("depthFactor", 0.1f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("Param", D3DXFROMWINEVECTOR4(TERRAIN_SCALE, DEPTH_IN_SNOW, 512.0f*3.0f, 0));

	IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, player.getNumVertices(), 0, player.getNumTriangles());
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void drawFrame()
{
	D3DXFROMWINEVECTOR3 pos;

	step = min(1000.0f*timeGetTime(), 0.1f) * 60.0f;

	player.heightMap = terrain.heightMap;
	player.play(step);
	AILoop();

	renderDeformMap();
	blurDeformMap(4);
	
	player.getPosition(pos);
	deformTerrain();
	if (snowingTime > 20.0f)
	{
		accu += 1000.0f*timeGetTime()*0.5f;
		if (accu >= 0.03f)
		{
			accumulateTerrain(terrain.heightMapAccumulated, accu);
			accu = 0.0f;
		}
	}
	generateTerrain();

	D3DXFROMWINEMATRIX projection = perspectiveMatrixX(D3DXFROMWINE_PI / 2, width, height, 10, 30000, true);
	D3DXFROMWINEMATRIX modelview = rotateXY4(-wx, -wy);
	D3DXFROMWINEMATRIX mvpSB = pegToFarPlane(projection * modelview);
	modelview.translate(-camPos);
	D3DXFROMWINEMATRIX mvp = projection * modelview;

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
	IRenderer::GetRendererInstance()->Clear(true, true, 0);

	drawTerrain(mvp);
	player.draw(mvp);
	drawSkybox(mvpSB);
	snowSys.draw(mvp, modelview, 1000.0f*timeGetTime());
	
	snowingTime += 1000.0f*timeGetTime();
}

private:
	Terrain	terrain;
	Player	player;
	SnowSystem snowSys;
	bool bSnowing;
	int currentBlurMap;
	float snowingTime;
	float step, accu;

	VertexFormatID terrainVF, skyboxVF, terrainGenVF, dumpVF;
	VertexBufferID terrainVB;
	IndexBufferID terrainIB;

	ShaderID skybox, terrainGen, terrainSH, dumpShd;
	TextureID env, rt0, rt1;
	stx_SkyBox sb;
    float width;
    float height;
    D3DXFROMWINEVECTOR3 camPos;
    float wx;
    float wy;
    float speed;
    bool debugRuntime;
};
App app;
int init(const char* aTitle)
{
	app.setup();
	app.resetCamera();
	app.load();
	return 0;
}

void render()
{
	app.drawFrame();
	IRenderer::GetRendererInstance()->Present();
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("R2VB-FootPrint");
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

