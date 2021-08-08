//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//
// This sample uses the R2VB technique on many fields. In this sample, we use R2VB
// for terrain deformation, heightmap based collision detection, skinning animation
// and the particle system. We do all calculation in the GPU except AI.
// We use one 512x512 R2VB texture for terrain generation, and another 512x152
// render target for real time deformation map. This deformation map is used
// to deform the original terrain mesh. We also implement the accumulation of snow,
// so the footprint will be filled up after snowing.
//-----------------------------------------------------------------------------

#include "App.h"
#include "../../Framework/Direct3D/atir2vb.h"

#define PLAYER_SCALE					90.0f
#define D2R								(PI/180.0f)
#define TERRAIN_SCALE					(1024.0f*8.0f)
#define DEPTH_IN_SNOW					3.0f*PLAYER_SCALE

BaseApp *app = new App();

//-----------------------------------------------------------------------------------------
// Make a rotation matrix with respect to Y axis.
//-----------------------------------------------------------------------------------------
void yawMatrix(mat4& m, float deg)
{
	float Rad = deg*D2R;
	float sn = sinf(Rad)*PLAYER_SCALE;
	float cs = cosf(Rad)*PLAYER_SCALE;

	m.elem[0][0] = cs;
	m.elem[2][0] = sn;

	m.elem[0][2] = -sn;
	m.elem[2][2] = cs;
}
//-----------------------------------------------------------------------------------------
// Load a model animation into R2VB animation class. We override this function because
// we need to load an extra shader for rendering footprint.
//-----------------------------------------------------------------------------------------
bool Player::load(char *modelname, char *diffuse, DWORD maxtexsize, LPDIRECT3DDEVICE9 lpdev, Renderer *lprenderer, bool debugruntime)
{
	R2VBAnimation::load(modelname, diffuse, maxtexsize, lpdev, lprenderer, debugruntime);
	if ((footprintShader = renderer->addShader("footprint.shd")) == SHADER_NONE) return false;	

	setRange(51.0f, 76.0f);
	setTime(51.0f);

	Deg = 0.0f;

	transformMatrix.elem[0][0] *= PLAYER_SCALE;
	transformMatrix.elem[1][1] *= PLAYER_SCALE;
	transformMatrix.elem[2][2] *= PLAYER_SCALE;

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
	renderer->changeRenderTarget(boneMatrix);

		renderer->setVertexFormat(QuadVF);
		renderer->setVertexBuffer(QuadVB, 0);renderer->setShader(animationSH);
		renderer->setTexture("boneAnimation", boneAnimation);
		renderer->setTexture("heightmap", heightMap);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();

		renderer->changeShaderConstant4f("time_interp", float4(t-1, t, interp, 0.0f));
		renderer->changeShaderConstant1f("XBias", 0.1f/(numBones*4.0f));
		renderer->changeShaderConstant1f("iBoneAnimationHeight", 1.0f/numFrames);
		renderer->changeShaderConstant4x4f("TransM", transformMatrix);
		renderer->changeShaderConstant4f("Param", float4(TERRAIN_SCALE, DEPTH_IN_SNOW, 512.0f*3.0f, 0));

		dev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::setup(char *args[], int nArguments)
{
	width  = 800;
	height = 600;
	speed= 10000;
}
//-----------------------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------------------
void App::resetCamera()
{
	camPos = float3(1500*3, 1000*3, 0);
	wx = 0.7f;
	wy = PI / 2;
}
//-----------------------------------------------------------------------------------------
// We render a heightmap texture into R2VB texture to generate terrain vertex buffer.
//-----------------------------------------------------------------------------------------
void App::resetTerrain()
{
	static float4 vertices[] = {
		float4(-1, -1, 0, 0),
		float4( 1, -1, 1, 0),
		float4( 1,  1, 1, 1),
		float4(-1,  1, 0, 1),
	};

	if (dev)
	{
		dev->BeginScene();
		// Generate R2VB vertex buffer from heightmap
		renderer->changeRenderTarget(terrain.deformedHeightmap);

			renderer->setVertexFormat(terrain.QuadVF);
			renderer->setShader(terrain.generateHeightShd);
			renderer->setTexture("heightmap", terrain.heightMap);
			renderer->setDepthFunc(DEPTH_NONE);
			renderer->apply();

			dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));

		renderer->resetRenderTarget();
		dev->EndScene();
	}
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool App::createTerrain(char *heightmap)
{
	char			Buffer[256];

	sprintf(Buffer, "%s%s", TEX_PATH, heightmap);
	if ((terrain.heightMap = renderer->addTexture(Buffer, FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	sprintf(Buffer, "%s%s", TEX_PATH, "height1024_7.tga");
	if ((terrain.heightMapAccumulated = renderer->addTexture(Buffer, FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	sprintf(Buffer, "%s%s", TEX_PATH, "noiseFillMap.png");
	if ((terrain.noiseFillMap = renderer->addTexture(Buffer, FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;
	
	
	StreamVertexAttribute vertexAttribs[] = {
		0, TYPE_TEXCOORD,	FORMAT_FLOAT, 2,
		1, TYPE_VERTEX,		FORMAT_FLOAT, 1,
	};

	if ((terrain.VF = renderer->addVertexFormat(vertexAttribs, elementsOf(vertexAttribs))) == VF_NONE) return false;

	// generate texture coordinate for terrain mesh
	if ((terrain.texCoordBuffer = renderer->addVertexBuffer(RT_SIZE * RT_SIZE * sizeof(float2))) == VB_NONE) return false;
	float2 *dest = (float2 *) renderer->lockVertexBuffer(terrain.texCoordBuffer);
		for (int z = 0; z < RT_SIZE; z++){
			float fz = float(2 * z) / (RT_SIZE - 1) - 1.0f;
			for (int x = 0; x < RT_SIZE; x++){
				float fx = float(2 * x) / (RT_SIZE - 1) - 1.0f;
				*dest++ = float2((TERRAIN_SCALE/2.0f) * fx, (TERRAIN_SCALE/2.0f) * fz);
			}
		}
	renderer->unlockVertexBuffer(terrain.texCoordBuffer);

	// generate index buffer for terrain mesh
	if ((terrain.IB = renderer->addIndexBuffer(6 * (RT_SIZE - 1) * (RT_SIZE - 1), sizeof(uint))) == IB_NONE) return false;
	uint *iDest = (uint *) renderer->lockIndexBuffer(terrain.IB);
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
	renderer->unlockIndexBuffer(terrain.IB);

	LPDIRECT3DTEXTURE9 d3drt;
	// Create terrain R2VB texture. Since the render target will be used as vertex buffer
	// we need to signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	if (dev->CreateTexture(RT_SIZE, RT_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_R32F, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((terrain.R2VB = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_LINEAR, CLAMP)) == TEXTURE_NONE) return false;

	/*
		We need a dummy vertex buffer when we specify stride and offset when calling SetStreamSource()
		for R2VB. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
	int size;
	if (debugRuntime)
		size = (RT_SIZE) * RT_SIZE * 4 * sizeof(float);
	else
		size = 2;
	if ((terrain.R2VB_DummyVBO = renderer->addVertexBuffer(size)) == VB_NONE) return false;

	// create a render target for storing deformed terrain data
	if (dev->CreateTexture(RT_SIZE, RT_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((terrain.deformedHeightmap = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	// create a render target for buffering
	if (dev->CreateTexture(RT_SIZE, RT_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((terrain.terrainBuffer = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	VertexAttribute quadAttribs[] = { TYPE_VERTEX,   FORMAT_FLOAT, 2,
	                                  TYPE_TEXCOORD, FORMAT_FLOAT, 2};
	if ((terrain.QuadVF = renderer->addVertexFormat(quadAttribs, elementsOf(quadAttribs))) == VF_NONE) return false;
	// load shaders
	if ((terrain.generateHeightShd = renderer->addShader("generateHeight.shd")) == SHADER_NONE) return false;
	if ((terrain.accumulateHeightShd = renderer->addShader("accumulateHeight.shd")) == SHADER_NONE) return false;
	if ((terrain.deformShd = renderer->addShader("deformShd.shd")) == SHADER_NONE) return false;
	if ((terrain.blurShd = renderer->addShader("blur.shd")) == SHADER_NONE) return false;

	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool App::load()
{
	if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, (D3DFORMAT) FOURCC('R','2','V','B')) != D3D_OK)
	{
		ERRORMSG("No R2VB support");
		return false;
	}

	// Load shaders
	if ((skybox = renderer->addShader("skybox.shd")) == SHADER_NONE) return false;
	if ((terrainSH  = renderer->addShader("terrain.shd" )) == SHADER_NONE) return false;
	if ((dumpShd = renderer->addShader("DumpTexture.shd")) == SHADER_NONE) return false;
	

	// Load skybox
	if ((env = renderer->addCubemap(
		TEX_PATH "Olympus/posx.jpg", TEX_PATH "Olympus/negx.jpg",
		TEX_PATH "Olympus/posy.jpg", TEX_PATH "Olympus/negy.jpg",
		TEX_PATH "Olympus/posz.jpg", TEX_PATH "Olympus/negz.jpg", FILTER_TRILINEAR)) == TEXTURE_NONE) return false;

	StreamVertexAttribute skyboxAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	if ((skyboxVF = renderer->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs))) == VF_NONE) return false;

	if (!createTerrain("height1024_5.tga")) return false;

	if (!player.load("boy03/boy03", "boy03.dds", caps.MaxTextureWidth, dev, renderer, debugRuntime)) return false;

	LPDIRECT3DTEXTURE9 d3drt;
	// create render target and depth buffer for rendering footprint as real-time deformation map
	if (dev->CreateTexture(RT_SIZE, RT_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((terrain.deformMap = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	if ((terrain.deformMapDepth  = renderer->addRenderDepthStencil(RT_SIZE, RT_SIZE, 16, 0)) == TEXTURE_NONE) return false;

	// create 2 textures for ping-pong bluring texture
	if (dev->CreateTexture(RT_SIZE, RT_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((terrain.blurDeformMap[0] = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	if (dev->CreateTexture(RT_SIZE, RT_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((terrain.blurDeformMap[1] = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	// load particle system
	snowSys.load(dev, renderer, caps.MaxTextureWidth, caps.MaxTextureHeight, debugRuntime, (caps.DeclTypes & D3DDTCAPS_FLOAT16_4) != 0);

	onReset();
	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool App::onReset()
{
	resetTerrain();
	snowSys.reset();
	snowingTime = 0;

	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::drawSkybox(const float4x4 &mvp)
{
	static const float verts[] = { -1,-1,-1, -1,-1,1, -1,1,-1, -1,1,1, 1,-1,-1, 1,-1,1, 1,1,-1, 1,1,1 };
	static const unsigned short inds[] = { 2,3,0, 0,3,1, 3,7,1, 1,7,5, 7,6,5, 5,6,4, 6,2,4, 4,2,0, 6,7,2, 2,7,3, 0,1,4, 4,1,5 };

	renderer->setShader(skybox);
	renderer->setVertexFormat(skyboxVF);
	renderer->setTexture("Skybox", env);
	renderer->setMask(COLOR);
	renderer->apply();

	renderer->changeShaderConstant4x4f("mvp", mvp);

	dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 12, inds, D3DFMT_INDEX16, verts, 3 * sizeof(float));
}
//-----------------------------------------------------------------------------------------
// Rendering deformation map to current terrain map to add deformation on terrain.
//-----------------------------------------------------------------------------------------
void App::deformTerrain()
{
	static float4 vertices[] = {
		float4(-1, -1, 0, 0),
		float4( 1, -1, 1, 0),
		float4( 1,  1, 1, 1),
		float4(-1,  1, 0, 1),
	};

	renderer->changeRenderTarget(terrain.deformedHeightmap);
		renderer->setVertexFormat(terrain.QuadVF);
		renderer->setShader(terrain.deformShd);
		renderer->setTexture("deformMap", terrain.blurDeformMap[!currentBlurMap]);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->setBlending(ONE, ONE, BM_REVERSE_SUBTRACT);
		renderer->apply();
		renderer->changeShaderConstant1f("depth", 0.45f*step);
		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));
}
//-----------------------------------------------------------------------------------------
// Using a noise texture and random texture coordinate for accumulating snow. 
// We render current terrain map to a buffering texture, then using buffering texture
// as input and add noise texture to form result terrain map.
//-----------------------------------------------------------------------------------------
void App::accumulateTerrain(TextureID &accumulatedHeightMap, float speed)
{
	static float4 vertices[] = {
		float4(-1, -1, 0, 0),
		float4( 1, -1, 1, 0),
		float4( 1,  1, 1, 1),
		float4(-1,  1, 0, 1),
	};

	renderer->changeRenderTarget(terrain.terrainBuffer);
		renderer->setVertexFormat(terrain.QuadVF);
		renderer->setShader(dumpShd);
		renderer->setTexture("DumpTex", terrain.deformedHeightmap);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();
		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));

	renderer->changeRenderTarget(terrain.deformedHeightmap);
		renderer->setVertexFormat(terrain.QuadVF);
		renderer->setShader(terrain.accumulateHeightShd);
		renderer->setTexture("currentMap", terrain.terrainBuffer);
		renderer->setTexture("accumulatedHeightMap", accumulatedHeightMap);
		renderer->setTexture("fillMap", terrain.noiseFillMap);
		
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();
		renderer->changeShaderConstant1f("accumulateSpeed", speed);
		renderer->changeShaderConstant2f("random", float2((rand()%137)/100.0f, (rand()%531)/100.0f));
		
		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));	
}
//-----------------------------------------------------------------------------------------
// Generate R2VB vertex buffer from heightmap
//-----------------------------------------------------------------------------------------
void App::generateTerrain()
{
	static float4 vertices[] = {
		float4(-1, -1, 0, 0),
		float4( 1, -1, 1, 0),
		float4( 1,  1, 1, 1),
		float4(-1,  1, 0, 1),
	};

	renderer->changeRenderTarget(terrain.R2VB);

		renderer->setVertexFormat(terrain.QuadVF);
		renderer->setShader(terrain.generateHeightShd);
		renderer->setTexture("heightmap", terrain.deformedHeightmap);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();
		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));	
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::drawTerrain(const float4x4 &mvp)
{
	// Enable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));
	// Bind the render target to the DMAP sampler
	dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(terrain.R2VB));
	// Tell the driver that stream 1 is to be fetched from the DMAP texture
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DMAP));

	renderer->setVertexFormat(terrain.VF);
	renderer->setVertexBuffer(terrain.texCoordBuffer, 0);
	renderer->setVertexBuffer(terrain.R2VB_DummyVBO, 1); // Dummy vertex buffer. Only the stride and offset data will be used.
	renderer->setIndexBuffer(terrain.IB);
	renderer->setShader(terrainSH);
	renderer->apply();

	renderer->changeShaderConstant4x4f("mvp", mvp);
	renderer->changeShaderConstant1f("HeightFactor", 512.0f*3);
	
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, RT_SIZE * RT_SIZE, 0, 2 * (RT_SIZE - 1) * (RT_SIZE - 1));

	// Stream 1 restored to regular vertex buffer mode
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
	// Unbind the DMAP texture
	dev->SetTexture(D3DDMAPSAMPLER, NULL);
	// Disable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::AILoop()
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
		float3    v1, v2;
		float     d;
		v1 = float3(player.Trans.elem[3][0], 0.0f, player.Trans.elem[3][2]);
		v2 = float3(player.Trans.elem[2][0], 0.0f, player.Trans.elem[2][2]);
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
	player.Trans.elem[3][0] -= player.Trans.elem[2][0]*step*70.0f/PLAYER_SCALE;
	player.Trans.elem[3][1] -= player.Trans.elem[2][1]*step*70.0f/PLAYER_SCALE;
	player.Trans.elem[3][2] -= player.Trans.elem[2][2]*step*70.0f/PLAYER_SCALE;
	
	if (player.bInSafeZone)
	{
		if (player.Trans.elem[3][0] > 3500.0f || player.Trans.elem[3][0] < -3500.0f ||
			player.Trans.elem[3][1] > 3500.0f || player.Trans.elem[3][1] < -3500.0f ||
			player.Trans.elem[3][2] > 3500.0f || player.Trans.elem[3][2] < -3500.0f)
		{
			player.bBounce = true;
			player.bInSafeZone = false;
		}
		else if (player.Trans.elem[3][0] < 2000.0f && player.Trans.elem[3][0] > -2000.0f &&
			player.Trans.elem[3][1] < 2000.0f && player.Trans.elem[3][1] > -2000.0f &&
			player.Trans.elem[3][2] < 2000.0f && player.Trans.elem[3][2] > -2000.0f)
		{
			player.bBounce = false;
			player.bInSafeZone = false;
		}
	}
	else
	{
		if (player.Trans.elem[3][0] < 3000.0f && player.Trans.elem[3][0] > -3000.0f &&
			player.Trans.elem[3][1] < 3000.0f && player.Trans.elem[3][1] > -3000.0f &&
			player.Trans.elem[3][2] < 3000.0f && player.Trans.elem[3][2] > -3000.0f)
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
void App::blurDeformMap(int times)
{
	static float4 vertices[] = {
		float4(-1, -1, 0, 0),
		float4( 1, -1, 1, 0),
		float4( 1,  1, 1, 1),
		float4(-1,  1, 0, 1),
	};

	for (int i=0; i<times; i++)
	{		
		renderer->changeRenderTarget(terrain.blurDeformMap[currentBlurMap]);
			renderer->setVertexFormat(terrain.QuadVF);
			renderer->setShader(terrain.blurShd);
			if (i == 0)
				renderer->setTexture("blur", terrain.deformMap);
			else
				renderer->setTexture("blur", terrain.blurDeformMap[!currentBlurMap]);
			renderer->setDepthFunc(DEPTH_NONE);
			renderer->apply();
			dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));

		currentBlurMap = !currentBlurMap;
	}
}
//-----------------------------------------------------------------------------------------
// We put a camera under the character and use parallel projection to render character to
// texture. This texture will be used as deformation map after blurring.
//-----------------------------------------------------------------------------------------
void App::renderDeformMap()
{
	float4x4 orth = orthoMatrix(-TERRAIN_SCALE, TERRAIN_SCALE, TERRAIN_SCALE, -TERRAIN_SCALE, TERRAIN_SCALE, -TERRAIN_SCALE, true);
	float4x4 modelview = rotateXY4(PI/2, 0);
	modelview.translate(float3(-0, -5000, -0));
	float4x4 mvp = orth * modelview;

	vec3 pos;
	player.getPosition(pos);

	renderer->changeRenderTarget(terrain.deformMap, terrain.deformMapDepth);
	dev->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, D3DCOLOR_RGBA(0x00, 0x00, 0x00, 0x00), 1.0f, 0);

	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));
	// Tell the driver that stream 1 is to be fetched from the DMAP texture
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DMAP));
	// Bind the render target to the DMAP sampler
	dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(player.getR2VB_VBO()));

	renderer->setShader(player.footprintShader);
	player.setupVertexSource();
	renderer->setDepthFunc(LESS);
	renderer->setTexture("heightmap", terrain.deformedHeightmap);
	renderer->apply();
	renderer->changeShaderConstant4x4f("mvp", mvp);
	renderer->changeShaderConstant1f("depthFactor", 0.1f);
	renderer->changeShaderConstant4f("Param", float4(TERRAIN_SCALE, DEPTH_IN_SNOW, 512.0f*3.0f, 0));

	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, player.getNumVertices(), 0, player.getNumTriangles());

	// Stream 1 restored to regular vertex buffer mode
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
	// Unbind the DMAP texture
	dev->SetTexture(D3DDMAPSAMPLER, NULL);
	// Disable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::drawFrame()
{
	float3 pos;

	step = min(frameTime, 0.1f) * 60.0f;

	player.heightMap = terrain.heightMap;
	player.play(step);
	AILoop();

	renderDeformMap();
	blurDeformMap(4);
	
	player.getPosition(pos);
	deformTerrain();
	if (snowingTime > 20.0f)
	{
		accu += frameTime*0.5f;
		if (accu >= 0.03f)
		{
			accumulateTerrain(terrain.heightMapAccumulated, accu);
			accu = 0.0f;
		}
	}
	generateTerrain();

	float4x4 projection = perspectiveMatrixX(PI / 2, width, height, 10, 30000, true);
	float4x4 modelview = rotateXY4(-wx, -wy);
	float4x4 mvpSB = pegToFarPlane(projection * modelview);
	modelview.translate(-camPos);
	float4x4 mvp = projection * modelview;

	renderer->resetRenderTarget();
	dev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	drawTerrain(mvp);
	player.draw(mvp);
	drawSkybox(mvpSB);
	snowSys.draw(mvp, modelview, frameTime);
	
	snowingTime += frameTime;
}
