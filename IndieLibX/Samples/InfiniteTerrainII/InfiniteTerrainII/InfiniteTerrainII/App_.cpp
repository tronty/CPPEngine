#include "App_.h"

#define R2VB 0

#if 1
void failedAssert(char *file, int line, char *statement){}
void ErrorMsg(const char *string){}
void WarningMsg(const char *string){}
void InfoMsg(const char *string){}
void outputDebugString(const char *str){}
#endif

BaseApp *app = new App();

void App::resetCamera(){
	camPos = vec3(0, 0.8f, 0);
	wx = 0;
	wy = 0;
	speed = 0.3f;
}

bool App::onKey(const uint key, const bool pressed){
	if (OpenGLApp::onKey(key, pressed)) return true;

	if (pressed && key == KEY_F5){
		showInfo = !showInfo;
		return true;
	}

	return false;
}

void App::onSize(const int w, const int h){
	OpenGLApp::onSize(w, h);

	lastX = INT_MAX;
	lastZ = INT_MAX;
}

bool App::init(){
	lightDir = normalize(vec3(0, 0.75f, 1));
	fogColor = float4(1, 0.5f, 0.1f, 1);
	showInfo = false;

	// Generate a noise texture
	srand(1);
	initNoise();

	float *nDest = (float *) noiseImage.create(FORMAT_R32F, NOISE_SIZE, NOISE_SIZE, 1, 1);
	for (int z = 0; z < NOISE_SIZE; z++){
		for (int x = 0; x < NOISE_SIZE; x++){
			*nDest++ = tileableNoise2(x * 0.143f, z * 0.143f, 0.143f * NOISE_SIZE, 0.143f * NOISE_SIZE);
		}
	}
	noiseImage.normalize();
	noiseImage.convert(FORMAT_I16);

	return true;
}

void App::fillTerrainIB(uint *iDest, const bool zDir){
	for (int zt = 0; zt < TERRAIN_SIZE - 1; zt++){
		int z = zDir? zt : TERRAIN_SIZE - 2 - zt;
		if (z & 1){
			if (zt) *iDest++ = z * TERRAIN_SIZE + TERRAIN_SIZE - 1;
			for (int x = TERRAIN_SIZE - 1; x >= 0; x--){
				*iDest++ = (z + 0) * TERRAIN_SIZE + x;
				*iDest++ = (z + 1) * TERRAIN_SIZE + x;
			}
		} else {
			if (zt) *iDest++ = z * TERRAIN_SIZE;
			for (int x = 0; x < TERRAIN_SIZE; x++){
				*iDest++ = (z + 0) * TERRAIN_SIZE + x;
				*iDest++ = (z + 1) * TERRAIN_SIZE + x;
			}
		}
	}
}

bool App::load(){
	bool supportsHalf = true;
	bool debugRuntime = true;

	// Samplerstates
	if ((linear = renderer->addSamplerState(LINEAR, WRAP, WRAP, WRAP)) == SS_NONE) return false;
	if ((nearestClamp = renderer->addSamplerState(NEAREST, CLAMP, CLAMP, CLAMP)) == SS_NONE) return false;
	if ((bilinearClamp = renderer->addSamplerState(BILINEAR, CLAMP, CLAMP, CLAMP)) == SS_NONE) return false;
	if ((trilinearAniso = renderer->addSamplerState(TRILINEAR, WRAP, WRAP, WRAP)) == SS_NONE) return false;




	// Height render target
	if ((heightRT = renderer->addRenderTarget(TERRAIN_SIZE, TERRAIN_SIZE, FORMAT_R32F, nearestClamp)) == TEXTURE_NONE) return false;
	// R2VB render target
	if (useR2VB){
		if ((terrainRT = renderer->addRenderTarget(TERRAIN_SIZE, TERRAIN_SIZE, supportsHalf? FORMAT_RGBA16F : FORMAT_RGBA32F, nearestClamp, R2VB)) == TEXTURE_NONE) return false;
	} else {
		if ((terrainRT = renderer->addRenderTarget(TERRAIN_SIZE, TERRAIN_SIZE, FORMAT_RGBA32F, nearestClamp)) == TEXTURE_NONE) return false;
	}


	// Shaders
	if ((terrain = renderer->addShader("terrain.shd", useR2VB? "#define R2VB\n" : "#define VTF\n")) == SHADER_NONE) return false;
	if ((genTerrain = renderer->addShader("generateTerrain.shd")) == SHADER_NONE) return false;
	if ((tangentSpace = renderer->addShader("tangentSpace.shd")) == SHADER_NONE) return false;
	if ((skyBox = renderer->addShader("skybox.shd")) == SHADER_NONE) return false;

	// Cubemap
	const char *files[] = {
		"../Textures/CubeMaps/Dragonfire/posx.jpg", "../Textures/CubeMaps/Dragonfire/negx.jpg",
		"../Textures/CubeMaps/Dragonfire/posy.jpg", "../Textures/CubeMaps/Dragonfire/negy.jpg",
		"../Textures/CubeMaps/Dragonfire/posz.jpg", "../Textures/CubeMaps/Dragonfire/negz.jpg",
	};
	if ((env = renderer->addCubemap(files, true, bilinearClamp)) == TEXTURE_NONE) return false;

	if ((ground = renderer->addTexture("../Textures/plast02.dds", true, trilinearAniso)) == TEXTURE_NONE) return false;

	// Vertex declaration
	FormatDesc terrainFmt[] = {
		0, TYPE_VERTEX, supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 2,
		1, TYPE_VERTEX, supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 4,
	};
	if ((terrainVF = renderer->addVertexFormat(terrainFmt, elementsOf(terrainFmt))) == VF_NONE) return false;

	FormatDesc quadFmt[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((quadVF = renderer->addVertexFormat(quadFmt, elementsOf(quadFmt))) == VF_NONE) return false;
	
	FormatDesc skyboxFmt[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	if ((skyboxVF = renderer->addVertexFormat(skyboxFmt, elementsOf(skyboxFmt))) == VF_NONE) return false;


	if ((noise = renderer->addTexture(noiseImage, linear)) == TEXTURE_NONE) return false;



	// Generate a vertex buffer for the terrain
	int vbSize = 2 * TERRAIN_SIZE * TERRAIN_SIZE;
	if (debugRuntime) vbSize *= 2; // To make it large enough for debug runtime validation when used as a dummy for an RGBA16F or RGBA32F R2VB vertex strean

	if (supportsHalf){
		half *vertices = new half[vbSize];
		half *vDest = vertices;
		for (int z = 0; z < TERRAIN_SIZE; z++){
			for (int x = 0; x < TERRAIN_SIZE; x++){
				*vDest++ = float(x) / TERRAIN_SIZE - 0.5f;
				*vDest++ = float(z) / TERRAIN_SIZE - 0.5f;
			}
		}
		if ((terrainVB = renderer->addVertexBuffer(vbSize * sizeof(half), STATIC, vertices)) == VB_NONE) return false;
		delete vertices;
	} else {
		float *vertices = new float[vbSize];
		float *vDest = vertices;
		for (int z = 0; z < TERRAIN_SIZE; z++){
			for (int x = 0; x < TERRAIN_SIZE; x++){
				*vDest++ = float(x) / TERRAIN_SIZE - 0.5f;
				*vDest++ = float(z) / TERRAIN_SIZE - 0.5f;
			}
		}
		if ((terrainVB = renderer->addVertexBuffer(vbSize * sizeof(float), STATIC, vertices)) == VB_NONE) return false;
		delete vertices;
	}


	// Generate an index buffer for the terrain
	uint *indices = new uint[INDEX_COUNT];
	fillTerrainIB(indices, true);
	if ((terrainIBs[0] = renderer->addIndexBuffer(INDEX_COUNT, sizeof(uint), STATIC, indices)) == IB_NONE) return false;
	fillTerrainIB(indices, false);
	if ((terrainIBs[1] = renderer->addIndexBuffer(INDEX_COUNT, sizeof(uint), STATIC, indices)) == IB_NONE) return false;
	delete indices;

	nUpdates = 0;
	return true;
}

void App::drawEnvironment(const float4x4 &mvp){
	static float vertices[] = { 0, 2, 0,  0, -2, -1,  -1, -2, 1,  1, -2, 1 };
	static ushort indices[] = { 0, 1, 2, 3, 0, 1 };

	renderer->reset();
	renderer->setShader(skyBox);
	renderer->setVertexFormat(skyboxVF);
	renderer->setTexture("Env", env, bilinearClamp);
	renderer->setDepthState(noDepthWrite);
	renderer->setShaderConstant4x4f("mvp", mvp);
	renderer->setShaderConstant4f("fogColor", fogColor);
	renderer->setShaderConstant2f("scaleBias", float2(2 + 0.3f * camPos.y, camPos.y * 1.3f - 1.2f));
	renderer->apply();

	//dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, 4, 4, indices, D3DFMT_INDEX16, vertices, sizeof(float3));
}

void App::drawTerrain(const float4x4 &mvp){
	int x = round(camPos.x);
	int z = round(camPos.z);

	if (x != lastX || z != lastZ){

		const float triVertices[]  = { 0, 3,  3, -1,  -3, -1 };

		renderer->changeRenderTarget(heightRT);

			renderer->reset();
			renderer->setShader(genTerrain);
			renderer->setVertexFormat(quadVF);
			renderer->setDepthState(noDepthTest);
			renderer->setTexture("Noise", noise, linear);
			renderer->setShaderConstant2f("offset", float2(0.5f * x, -0.5f * z));
			renderer->apply();

			//dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, triVertices, sizeof(float2));

		renderer->changeRenderTarget(terrainRT);

			renderer->reset();
			renderer->setShader(tangentSpace);
			renderer->setVertexFormat(quadVF);
			renderer->setDepthState(noDepthTest);
			renderer->setTexture("Height", heightRT, nearestClamp);
			renderer->setShaderConstant1f("pixel",     1.0f / TERRAIN_SIZE);
			renderer->setShaderConstant1f("halfPixel", 0.5f / TERRAIN_SIZE);
			renderer->apply();

			//dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, triVertices, sizeof(float2));

		renderer->changeToMainFramebuffer();

		nUpdates++;
		lastX = x;
		lastZ = z;
	}


	renderer->reset();
	renderer->setShader(terrain);
	renderer->setVertexFormat(terrainVF);
	renderer->setVertexBuffer(0, terrainVB);
	if (useR2VB){
		// Dummy vertex buffer, but R2VB needs the stride and offset.
		renderer->setVertexBuffer(1, terrainVB);
	}
	// Select an optimal index buffer for HiZ depending on which way we're looking.
	renderer->setIndexBuffer(terrainIBs[int(cosf(wy) < 0)]);
	renderer->setTexture("Ground", ground, trilinearAniso);
	renderer->setRasterizerState(cullBack);
	float3 offset(float(x), 0, float(z));
	renderer->setShaderConstant4x4f("mvp", mvp * translate(offset));
	renderer->setShaderConstant3f("camPos", camPos - offset);
	renderer->setShaderConstant3f("lightDir", lightDir);
	renderer->setShaderConstant4f("fogColor", fogColor);
	renderer->apply();

	if (useR2VB){
	 	//dev->SetRenderState(D3DRS_POINTSIZE, R2VB_ENABLE);
		//dev->SetRenderState(D3DRS_POINTSIZE, R2VB_MAP_STREAM(1, R2VB_DMAP));
		//dev->SetTexture(D3DDMAPSAMPLER, ((Direct3DRenderer *) renderer)->getD3DTexture(terrainRT));
	} else {
		//dev->SetTexture(D3DVERTEXTEXTURESAMPLER0, ((Direct3DRenderer *) renderer)->getD3DTexture(terrainRT));
	}

	//dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, TERRAIN_SIZE * TERRAIN_SIZE, 0, INDEX_COUNT - 2);

	if (useR2VB){
		//dev->SetTexture(D3DDMAPSAMPLER, NULL);
		//dev->SetRenderState(D3DRS_POINTSIZE, R2VB_UNMAP_STREAM(1));
		//dev->SetRenderState(D3DRS_POINTSIZE, R2VB_DISABLE);
	} else {
		//dev->SetTexture(D3DVERTEXTEXTURESAMPLER0, NULL);
	}
}

void App::drawFrame(){
	float4x4 projection = toD3DProjection(perspectiveMatrixX(1.5f, width, height, 0.001f, 2));
	float4x4 modelview = rotateXY(-wx, -wy);

	// Force depth to 1 for environment to put it behind everything
	float4x4 envMvp = translate(0, 0, 1) * scale(1, 1, 0) * projection * modelview;

	modelview.translate(-camPos);
	float4x4 mvp = projection * modelview;

	//dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	drawTerrain(mvp);
	drawEnvironment(envMvp);


	if (showInfo){
		renderer->setup2DMode(0, (float) width, 0, (float) height);

		char str[256];
		sprintf(str, "%d MT/s\nUpdates: %d", int(((INDEX_COUNT - 2) * 1e-6f) / frameTime + 0.5f), nUpdates);
		renderer->drawText(str, 10, height - 82.0f, 30, 36, defaultFont, linearClamp, blendSrcAlpha, noDepthTest);
	}
}
