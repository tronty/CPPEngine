//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------


#include "App.h"
#include "../../Framework/Direct3D/atir2vb.h"

BaseApp *app = new App();

void App::setup(char *args[], int nArguments){
	width  = 800;
	height = 600;
	speed = 500;
}

void App::resetCamera(){
	camPos = float3(150, 100, 0);
	wx = 0.7f;
	wy = PI / 2;
}

void App::moveCamera(const vec3 &dir){
	float3 pos = camPos + dir * (speed * frameTime);
	float3 cPos;

	float3 minLim(-160, -104, -160);
	float3 maxLim(160, 24, 160);

	float d = 0;
	for (int i = 0; i < 3; i++){
		if (pos[i] < minLim[i]){
			float s = pos[i] - minLim[i];
			d += s * s;
			cPos[i] = minLim[i];
		} else if (pos[i] > maxLim[i]){
			float s = pos[i] - maxLim[i];
			d += s * s;
			cPos[i] = maxLim[i];
		} else {
			cPos[i] = pos[i];
		}
	}

	if (d < 30 * 30){
		float3 dir = pos - cPos;
		camPos = cPos + 30 * normalize(dir);
	} else {
		camPos = pos;
	}
}

void App::initGUI(){
	addWidget(dropRate = new Slider(770, 340, 26, 256, false, 0, 1, 0.4f));

	D3D9App::initGUI();
}

bool App::load(){
	/*
		Create render targets for the water dynamics.
		Since these render targets will be used as vertex buffers we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	LPDIRECT3DTEXTURE9 d3drt0, d3drt1;
	if (dev->CreateTexture(RT_SIZE, RT_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A16B16G16R16, D3DPOOL_DEFAULT, &d3drt0, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if (dev->CreateTexture(RT_SIZE, RT_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A16B16G16R16, D3DPOOL_DEFAULT, &d3drt1, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((rt0 = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt0, FILTER_LINEAR, CLAMP)) == TEXTURE_NONE) return false;
	if ((rt1 = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt1, FILTER_LINEAR, CLAMP)) == TEXTURE_NONE) return false;

	// Create screen-sized render targets for reflection and refraction
	if ((reflection = renderer->addRenderTarget(width, height, FORMAT_RGBA8, FILTER_LINEAR, CLAMP | TRACK_WINDOW_SIZE)) == TEXTURE_NONE) return false;
	if ((refraction = renderer->addRenderTarget(width, height, FORMAT_RGBA8, FILTER_LINEAR, CLAMP | TRACK_WINDOW_SIZE)) == TEXTURE_NONE) return false;
	if ((depth = renderer->addRenderDepthStencil(width, height, 16, 0, TRACK_WINDOW_SIZE)) == TEXTURE_NONE) return false;

	currRT  = rt0;
	otherRT = rt1;

	// Load shaders
	if ((wave   = renderer->addShader("wave.shd"  )) == SHADER_NONE) return false;
	if ((drop   = renderer->addShader("drop.shd"  )) == SHADER_NONE) return false;
	if ((skybox = renderer->addShader("skybox.shd")) == SHADER_NONE) return false;
	if ((water  = renderer->addShader("water.shd" )) == SHADER_NONE) return false;
	if ((pool   = renderer->addShader("pool.shd"  )) == SHADER_NONE) return false;

	// Load textures
	if ((env = renderer->addCubemap(
		TEX_PATH "Mountains/posx.jpg", TEX_PATH "Mountains/negx.jpg",
		TEX_PATH "Mountains/posy.jpg", TEX_PATH "Mountains/negy.jpg",
		TEX_PATH "Mountains/posz.jpg", TEX_PATH "Mountains/negz.jpg", FILTER_TRILINEAR)) == TEXTURE_NONE) return false;

	if ((brick = renderer->addTexture(TEX_PATH "parqfloor2.dds", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;
	if ((stone = renderer->addTexture(TEX_PATH "StoneWall2.dds", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	// Create the main water vertex buffer
	if ((waterVB = renderer->addVertexBuffer(RT_SIZE * RT_SIZE * sizeof(float3))) == VB_NONE) return false;
	float3 *dest = (float3 *) renderer->lockVertexBuffer(waterVB);
		for (int z = 0; z < RT_SIZE; z++){
			float fz = float(2 * z) / (RT_SIZE - 1) - 1.0f;
			for (int x = 0; x < RT_SIZE; x++){
				float fx = float(2 * x) / (RT_SIZE - 1) - 1.0f;
				*dest++ = float3(128 * fx, 0, 128 * fz);
			}
		}
	renderer->unlockVertexBuffer(waterVB);

	// Create the water index buffer
	if ((waterIB = renderer->addIndexBuffer(6 * (RT_SIZE - 1) * (RT_SIZE - 1), sizeof(uint))) == IB_NONE) return false;
	uint *iDest = (uint *) renderer->lockIndexBuffer(waterIB);
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
	renderer->unlockIndexBuffer(waterIB);

	// Create vertex declarations
	StreamVertexAttribute waterAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT,  3,
		1, TYPE_TEXCOORD, FORMAT_USHORT, 4,
	};
	if ((waterVF = renderer->addVertexFormat(waterAttribs, elementsOf(waterAttribs))) == VF_NONE) return false;

	VertexAttribute waveAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((waveVF = renderer->addVertexFormat(waveAttribs, elementsOf(waveAttribs))) == VF_NONE) return false;

	VertexAttribute skyboxAttribs[] = {
		TYPE_VERTEX,   FORMAT_FLOAT, 4,
		TYPE_TEXCOORD, FORMAT_FLOAT, 4,
	};
	if ((skyboxVF = renderer->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs))) == VF_NONE) return false;

	VertexAttribute poolAttribs[] = {
		TYPE_VERTEX,   FORMAT_FLOAT, 3,
		TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		TYPE_TEXCOORD, FORMAT_FLOAT, 1,
	};
	if ((poolVF = renderer->addVertexFormat(poolAttribs, elementsOf(poolAttribs))) == VF_NONE) return false;

	onReset();
	return true;
}

void App::onSize(const int w, const int h){
	D3D9App::onSize(w, h);

	dropRate->setPosition(w - 30.0f, h - 260.0f);
}

bool App::onReset(){
	// Initialize the render targets
	renderer->changeRenderTarget(rt0);
	dev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0x80, 0x80, 0x80, 0x80), 1.0f, 0);

	renderer->changeRenderTarget(rt1);
	dev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0x80, 0x80, 0x80, 0x80), 1.0f, 0);

	renderer->resetRenderTarget();

	needInit = true;

	return true;
}

static float2 vertices[] = {
	float2(-1, -1),
	float2( 1, -1),
	float2( 1,  1),
	float2(-1,  1),
};

void App::drawDrop(const float2 &xy, const float size, const float strength, const float intp){

	static float2 dVertices[] = {
		float2(-0.707f, -0.707f),
		float2( 0, -1),
		float2( 0.707f, -0.707f),
		float2( 1,  0),
		float2( 0.707f,  0.707f),
		float2( 0,  1),
		float2(-0.707f,  0.707f),
		float2(-1,  0),
	};

	/*
		Drop a drop on the current render target.
		Emulate floating point blending by reading the other render target.
	*/
	renderer->setShader(drop);
	renderer->setVertexFormat(waveVF);
	renderer->setTexture("Tex", otherRT);
	renderer->setDepthFunc(DEPTH_NONE);
	renderer->apply();

	renderer->changeShaderConstant1f("off", 0.5f / RT_SIZE);

	renderer->changeShaderConstant2f("xy", xy);
	renderer->changeShaderConstant1f("size", size);
	renderer->changeShaderConstant1f("strength", strength);
	renderer->changeShaderConstant2f("intp", float2(intp, 1.0f - intp));

	dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 6, dVertices, sizeof(float2));
}

void App::updateWater(const float4x4 &mvp, const float frameTime){
	renderer->changeRenderTarget(currRT);

		renderer->setVertexFormat(waveVF);
		renderer->setShader(wave);
		renderer->setTexture("Tex", otherRT);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();

		renderer->changeShaderConstant1f("off", 0.5f / RT_SIZE);
		renderer->changeShaderConstant1f("acceleration", 17 * frameTime);
		renderer->changeShaderConstant1f("velocity", 60 * frameTime);
		renderer->changeShaderConstant1f("damp", pow(0.5f, frameTime));

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

		if (needInit){
			/*
				When the water RTs are first created and cleared they need a bit of noise in them to behave
				naturally. Otherwise the fact that 0.0 is right between 32677 and 32768 when the texture is
				mapped to signed range and thus isn't exactly represented it makes the entire surface either
				lift or sink depending on if you clear to 0x7F or 0x80 since all heights will be equal after
				the clear and therefore the force computed for each pixel will be zero, maintaining the slight
				movement. Initially drawing a drop over the entire surface with minimal strength adds enough
				differences between neighboring texels to make this effect go away.
			*/
			drawDrop(float2(0, 0), 1.414f, 0.0001f, 0.5f);
		}

		/*
			Interact with the surface when the right mouse button is pressed
		*/
		if (mMouseButton || rMouseButton){
			// Position in clip space. We don't know z, so we set it arbitrarily to 0.
			float4 cPos(2 * float(mouseX) / float(width) - 1, 1 - 2 * float(mouseY) / float(height), 0, 1);
			// Reverse transform to unproject it back to world space.
			// This gives a point that would map to the clicked pixel.
			float4 unProj = !mvp * cPos;

			// Compute direction from camera to this position
			float3 dir = normalize((unProj.xyz() / unProj.w) - camPos);
			float3 pos = camPos - ((camPos.y/* + WATER_POS_Y*/) / dir.y) * dir;

			// Compute where this vector would hit the water plane
			float2 xy(floorf(pos.x + 0.5f) / 128.0f, floorf(-pos.z + 0.5f) / 128.0f);

			drawDrop(xy, 0.125f, rMouseButton? 0.005f : -0.005f, 0.95f);
		}

		float dr = dropRate->getValue();
		if (dr > 0){
			static float2 xy;
			static uint c = 0;
			static float t = 0;
			t += frameTime;

			dr = 0.2f / dr;

			if (t > dr){
				xy = float2(float(rand()) / RAND_MAX * 2 - 1, float(rand()) / RAND_MAX * 2 - 1);
				t -= dr;
				c = 0;
			}
			if (c < 10){
				drawDrop(xy, 0.125f, 0.008f, 0.95f);
				c++;
			}
		}

	renderer->resetRenderTarget();
}

void App::drawSkybox(const float4x4 &mvp){
	renderer->setShader(skybox);
	renderer->setVertexFormat(skyboxVF);
	renderer->setTexture("Skybox", env);
	renderer->setMask(COLOR);
	renderer->apply();

	/*
		Render the skybox in clip-space. We use a big triangle spanning outside
		the screen to avoid polygon edges within the screen for a tiny performance
		boost. Z is set to constant 1 to put it behind everything.

		The texture coordinates for the cubemap is computed by projecting the
		clip-space position back into world space with the inverse MVP matrix.
	*/
	float4x4 iMvp = !mvp;

	float4 v0( 0,  2, 1, 1);
	float4 v1( 3, -1, 1, 1);
	float4 v2(-3, -1, 1, 1);

	struct Vertex {
		float4 clipPos;
		float4 cubeCoord;
	};

	Vertex vertices[3] = {
		v0, iMvp * v0,
		v1, iMvp * v1,
		v2, iMvp * v2,
	};

	dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vertices, sizeof(Vertex));
}

void App::drawPool(const float4x4 &mvp){
	struct Vertex {
		float3 vertex;
		float2 texCoord;
		float  color;
	};

	static const Vertex vertices[] = {
		float3(-128, -104,  128), float2(0, 1), 0.3f, float3(-128, 24,  128), float2(0, 0), 0.3f,
		float3( 128, -104,  128), float2(2, 1), 0.7f, float3( 128, 24,  128), float2(2, 0), 0.7f,
		float3( 128, -104, -128), float2(4, 1), 0.7f, float3( 128, 24, -128), float2(4, 0), 0.7f,
		float3(-128, -104, -128), float2(2, 1), 0.3f, float3(-128, 24, -128), float2(2, 0), 0.3f,

		float3(-160, -104,  160), float2(0, 1), 1.0f, float3(-160, 24,  160), float2(0, 0), 1.0f,
		float3( 160, -104,  160), float2(2, 1), 0.5f, float3( 160, 24,  160), float2(2, 0), 0.5f,
		float3( 160, -104, -160), float2(4, 1), 0.5f, float3( 160, 24, -160), float2(4, 0), 0.5f,
		float3(-160, -104, -160), float2(2, 1), 1.0f, float3(-160, 24, -160), float2(2, 0), 1.0f,

		float3(-160, 24,  160), float2(-0.25f, -0.25f), 0.8f, float3(-128, 24,  128), float2(0, 0), 0.8f,
		float3( 160, 24,  160), float2( 2.25f, -0.25f), 0.8f, float3( 128, 24,  128), float2(2, 0), 0.8f,
		float3( 160, 24, -160), float2( 2.25f,  2.25f), 0.8f, float3( 128, 24, -128), float2(2, 2), 0.8f,
		float3(-160, 24, -160), float2(-0.25f,  2.25f), 0.8f, float3(-128, 24, -128), float2(0, 2), 0.8f,

		float3(-128, -104,  128), float2(0, 0), 0.3f, float3(128, -104,  128), float2(2, 0), 1.0f,
		float3(-128, -104, -128), float2(0, 2), 0.3f, float3(128, -104, -128), float2(2, 2), 1.0f,
	};

	static const ushort indices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1 };

	renderer->setVertexFormat(poolVF);
	renderer->setShader(pool);
	renderer->setTexture("Base", brick);
	renderer->apply();

	renderer->changeShaderConstant4x4f("mvp", mvp);

	dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, 8, 8, indices, D3DFMT_INDEX16, vertices,      sizeof(Vertex));
	dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, 8, 8, indices, D3DFMT_INDEX16, vertices + 8,  sizeof(Vertex));
	dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, 8, 8, indices, D3DFMT_INDEX16, vertices + 16, sizeof(Vertex));

	renderer->changeTexture("Base", stone);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices + 24, sizeof(Vertex));
}

void App::drawMainScene(const float4x4 &mvpSB, const float4x4 &mvp, const float *clipPlane){
	dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	if (clipPlane){
		dev->SetClipPlane(0, clipPlane);
		dev->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
	}
	drawPool(mvp);
	if (clipPlane){
		dev->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
	}
	drawSkybox(mvpSB);
}

void App::drawFrame(){
	float4x4 projection = perspectiveMatrixX(PI / 2, width, height, 1, 2000, true);

	float4x4 modelview = rotateXY4(-wx, -wy);
	float4x4 mvpSB = projection * modelview;
	float4x4 mvp = mvpSB * translate(-camPos);


	/*
		Only update reflection and refraction textures if the view changed. Boosts performance quite a bit.
	*/
	static float3 lastPos;
	static float lastWx, lastWy;
	if (needInit || lastPos != camPos || lastWx != wx || lastWy != wy){
		renderer->changeRenderTarget(reflection, depth);
			float4x4 mirrorMVP = mvp * mirrorMatrix(float3(0, 1, 0), WATER_POS_Y);
			float4 clipPlane = (!transpose(mirrorMVP)) * float4(0, 1, 0, WATER_POS_Y + 10);
			drawMainScene(mvpSB * scale4(1, -1, 1), mirrorMVP, clipPlane);

		renderer->changeRenderTarget(refraction, depth);
			clipPlane = (!transpose(mvp)) * float4(0, -1, 0, -WATER_POS_Y + 10);
			drawMainScene(mvpSB, mvp, clipPlane);

		renderer->resetRenderTarget();

		lastPos = camPos;
		lastWx = wx;
		lastWy = wy;
	}

	// Draw scene, excluding water
	drawMainScene(mvpSB, mvp);


	/*
		Draw the water using the R2VB extension. This extension allows us to bind a texture as a
		vertex buffer, allowing for interesting effects by rendering to vertex buffer, in this case
		letting a pixel shader do physics on a water surface and pass that to the vertex shader for
		a truely geometric water effect done entirely on the VPU.
	*/

	// Enable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));

	renderer->setShader(water);
	renderer->setVertexFormat(waterVF);
	renderer->setVertexBuffer(waterVB, 0);
	renderer->setVertexBuffer(waterVB, 1); // Dummy vertex buffer. Only the stride and offset data will be used.
	renderer->setIndexBuffer(waterIB);
	renderer->setTexture("Reflection", reflection);
	renderer->setTexture("Refraction", refraction);
	renderer->apply();

	renderer->changeShaderConstant4x4f("mvp", mvp);
	renderer->changeShaderConstant4x4f("mvpProj", translate(0.5f, 0.5f, 0) * scale4(0.5f, -0.5f, 1) * mvp);
	renderer->changeShaderConstant3f("camPos", camPos);


	// Bind the render target to the DMAP sampler
	dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(currRT));
	// Tell the driver that stream 1 is to be fetched from the DMAP texture
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, 0));

	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, RT_SIZE * RT_SIZE, 0, 2 * (RT_SIZE - 1) * (RT_SIZE - 1));

	// Stream 1 restored to regular vertex buffer mode
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
	// Unbind the DMAP texture
	dev->SetTexture(D3DDMAPSAMPLER, NULL);

	// Disable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));


	/*
		The water simulation can become unstable if the framerate fluctuates too much.
		Here we try to keep the update frequency within a certain range to avoid that.
	*/
#define MIN_FPS 50
#define MAX_FPS 100
#define MIN_TIME (1.0f / MAX_FPS)
#define MAX_TIME (1.0f / MIN_FPS)

	// Main water dynamics pass
	static float timeCredit = MIN_TIME;
	timeCredit += frameTime;

	if (timeCredit > MIN_TIME){
		float ft = min(floor(timeCredit * MAX_FPS) * MIN_TIME, MAX_TIME);

		updateWater(mvp, ft);

		timeCredit -= ft;

		// Flip render targets
		TextureID temp = currRT;
		currRT = otherRT;
		otherRT = temp;
	}

	needInit = false;
}
