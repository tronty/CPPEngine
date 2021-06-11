/*
  Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>
#include <Framework3/RendererHLSLD3D11.h>
#include <STX/Noise.h>
//include "../Framework3/Math/Frustum.h"

#define TERRAIN_SIZE 512
#define INDEX_COUNT ((TERRAIN_SIZE - 1) * (2 * TERRAIN_SIZE + 1) - 1)

class App {
protected:
	float3 lightDir;
	float4 fogColor;

	TextureID heightRT, terrainRT;
	ShaderID genTerrain, tangentSpace;

	Image3 noiseImage;
	TextureID noise;

	VertexBufferID terrainVB;
	IndexBufferID  terrainIBs[2];
	VertexFormatID terrainVF, skyboxVF, quadVF;

	TextureID ground, env;
	ShaderID terrain, skyBox;

	SamplerStateID linear, nearestClamp, bilinearClamp, trilinearAniso;

	int lastX, lastZ;
	int nUpdates;
	bool useR2VB;
	bool showInfo;

	D3DXFROMWINEVECTOR3 camPos;
	float wx,wy,speed;
public:

void resetCamera(){
	camPos = D3DXFROMWINEVECTOR3(0, 0.8f, 0);
	wx = 0;
	wy = 0;
	speed = 0.3f;
}

bool init(){
	lightDir = normalize(D3DXFROMWINEVECTOR3(0, 0.75f, 1));
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

void fillTerrainIB(uint *iDest, const bool zDir){
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

bool load(){
	bool supportsHalf = false;
	bool debugRuntime = false;

	// Samplerstates
	if ((linear = g_pRenderer11->addSamplerState(LINEAR, WRAP, WRAP, WRAP)) == SS_NONE) return false;
	if ((nearestClamp = g_pRenderer11->addSamplerState(NEAREST, CLAMP, CLAMP, CLAMP)) == SS_NONE) return false;
	if ((bilinearClamp = g_pRenderer11->addSamplerState(BILINEAR, CLAMP, CLAMP, CLAMP)) == SS_NONE) return false;
	if ((trilinearAniso = g_pRenderer11->addSamplerState(TRILINEAR, WRAP, WRAP, WRAP)) == SS_NONE) return false;

	// Height render target
	if ((heightRT = g_pRenderer11->addRenderTarget(TERRAIN_SIZE, TERRAIN_SIZE, FORMAT_R32F, nearestClamp)) == TEXTURE_NONE) return false;
	// R2VB render target
	#if 0
	if (useR2VB){
		if ((terrainRT = g_pRenderer11->addRenderTarget(TERRAIN_SIZE, TERRAIN_SIZE, supportsHalf? FORMAT_RGBA16F : FORMAT_RGBA32F, nearestClamp, R2VB)) == TEXTURE_NONE) return false;
	} else {
		if ((terrainRT = g_pRenderer11->addRenderTarget(TERRAIN_SIZE, TERRAIN_SIZE, FORMAT_RGBA32F, nearestClamp)) == TEXTURE_NONE) return false;
	}
	#else
	if ((terrainRT = g_pRenderer11->addRenderTarget(TERRAIN_SIZE, TERRAIN_SIZE, FORMAT_RGBA16F, nearestClamp)) == TEXTURE_NONE) return false;
	#endif

	// Shaders
	if ((terrain = g_pRenderer11->addShader("terrain.shd", useR2VB? "#define R2VB\n" : "#define VTF\n")) == SHADER_NONE) return false;
	if ((genTerrain = g_pRenderer11->addShader("generateTerrain.shd")) == SHADER_NONE) return false;
	if ((tangentSpace = g_pRenderer11->addShader("tangentSpace.shd")) == SHADER_NONE) return false;
	if ((skyBox = g_pRenderer11->addShader("skybox.shd")) == SHADER_NONE) return false;

	// Cubemap
	const char *files[] = {
		"../Textures/CubeMaps/Dragonfire/posx.jpg", "../Textures/CubeMaps/Dragonfire/negx.jpg",
		"../Textures/CubeMaps/Dragonfire/posy.jpg", "../Textures/CubeMaps/Dragonfire/negy.jpg",
		"../Textures/CubeMaps/Dragonfire/posz.jpg", "../Textures/CubeMaps/Dragonfire/negz.jpg",
	};
	if ((env = g_pRenderer11->addCubemap(files, true, bilinearClamp)) == TEXTURE_NONE) return false;

	if ((ground = g_pRenderer11->addTexture("../Textures/plast02.dds", true, trilinearAniso)) == TEXTURE_NONE) return false;

	// Vertex declaration
	FormatDesc terrainFmt[] = {
		0, TYPE_VERTEX, supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 2,
		1, TYPE_VERTEX, supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 4,
	};
	if ((terrainVF = g_pRenderer11->addVertexFormat(terrainFmt, elementsOf(terrainFmt))) == VF_NONE) return false;

	FormatDesc quadFmt[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((quadVF = g_pRenderer11->addVertexFormat(quadFmt, elementsOf(quadFmt))) == VF_NONE) return false;
	
	FormatDesc skyboxFmt[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	if ((skyboxVF = g_pRenderer11->addVertexFormat(skyboxFmt, elementsOf(skyboxFmt))) == VF_NONE) return false;

	if ((noise = g_pRenderer11->addTexture(noiseImage, linear)) == TEXTURE_NONE) return false;

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
		if ((terrainVB = g_pRenderer11->addVertexBuffer(vbSize * sizeof(half), STATIC, vertices)) == VB_NONE) return false;
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
		if ((terrainVB = g_pRenderer11->addVertexBuffer(vbSize * sizeof(float), STATIC, vertices)) == VB_NONE) return false;
		delete vertices;
	}

	// Generate an index buffer for the terrain
	uint *indices = new uint[INDEX_COUNT];
	fillTerrainIB(indices, true);
	if ((terrainIBs[0] = g_pRenderer11->addIndexBuffer(INDEX_COUNT, sizeof(uint), STATIC, indices)) == IB_NONE) return false;
	fillTerrainIB(indices, false);
	if ((terrainIBs[1] = g_pRenderer11->addIndexBuffer(INDEX_COUNT, sizeof(uint), STATIC, indices)) == IB_NONE) return false;
	delete indices;

	nUpdates = 0;
	return true;
}

void drawEnvironment(const float4x4 &mvp){
	static float vertices[] = { 0, 2, 0,  0, -2, -1,  -1, -2, 1,  1, -2, 1 };
	static ushort indices[] = { 0, 1, 2, 3, 0, 1 };

	g_pRenderer11->setShader(skyBox);
	g_pRenderer11->setVertexFormat(skyboxVF);
	g_pRenderer11->setTexture("Env", env, bilinearClamp);
	g_pRenderer11->setDepthState(noDepthWrite);
	g_pRenderer11->setShaderConstant4x4f("mvp", mvp);
	g_pRenderer11->setShaderConstant4f("fogColor", fogColor);
	g_pRenderer11->setShaderConstant2f("scaleBias", float2(2 + 0.3f * camPos.y, camPos.y * 1.3f - 1.2f));

	g_pRenderer11->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, 4, 4, indices, D3DFMT_INDEX16, vertices, sizeof(float3));
}

void drawTerrain(const float4x4 &mvp){
	int x = round(camPos.x);
	int z = round(camPos.z);

	if (x != lastX || z != lastZ){

		const float triVertices[]  = { 0, 3,  3, -1,  -3, -1 };

		g_pRenderer11->changeRenderTarget(heightRT);
		
			g_pRenderer11->setShader(genTerrain);
			g_pRenderer11->setVertexFormat(quadVF);
			g_pRenderer11->setDepthState(noDepthTest);
			g_pRenderer11->setTexture("Noise", noise, linear);
			g_pRenderer11->setShaderConstant2f("offset", float2(0.5f * x, -0.5f * z));		

			g_pRenderer11->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, triVertices, sizeof(float2));

		g_pRenderer11->changeRenderTarget(terrainRT);

			g_pRenderer11->setShader(tangentSpace);
			g_pRenderer11->setVertexFormat(quadVF);
			g_pRenderer11->setDepthState(noDepthTest);
			g_pRenderer11->setTexture("Height", heightRT, nearestClamp);
			g_pRenderer11->setShaderConstant1f("pixel",     1.0f / TERRAIN_SIZE);
			g_pRenderer11->setShaderConstant1f("halfPixel", 0.5f / TERRAIN_SIZE);		

			g_pRenderer11->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, triVertices, sizeof(float2));

		g_pRenderer11->changeToMainFramebuffer();

		nUpdates++;
		lastX = x;
		lastZ = z;
	}

	g_pRenderer11->setShader(terrain);
	g_pRenderer11->setVertexFormat(terrainVF);
	g_pRenderer11->setVertexBuffer(0, terrainVB);

	// Select an optimal index buffer for HiZ depending on which way we're looking.
	g_pRenderer11->setIndexBuffer(terrainIBs[int(cosf(wy) < 0)]);
	g_pRenderer11->setTexture("Ground", ground, trilinearAniso);
	g_pRenderer11->setRasterizerState(cullBack);
	float3 offset(float(x), 0, float(z));
	g_pRenderer11->setShaderConstant4x4f("mvp", mvp * translate(offset));
	g_pRenderer11->setShaderConstant3f("camPos", camPos - offset);
	g_pRenderer11->setShaderConstant3f("lightDir", lightDir);
	g_pRenderer11->setShaderConstant4f("fogColor", fogColor);
	g_pRenderer11->SetTexture(D3DVERTEXTEXTURESAMPLER0, ((Direct3DRenderer *) g_pRenderer11)->getD3DTexture(terrainRT));

	g_pRenderer11->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, TERRAIN_SIZE * TERRAIN_SIZE, 0, INDEX_COUNT - 2);
}

void drawFrame(){
	float4x4 projection = toD3DProjection(perspectiveMatrixX(1.5f, width, height, 0.001f, 2));
	float4x4 modelview = rotateXY(-wx, -wy);

	// Force depth to 1 for environment to put it behind everything
	float4x4 envMvp = translate(0, 0, 1) * scale(1, 1, 0) * projection * modelview;

	modelview.translate(-camPos);
	float4x4 mvp = projection * modelview;

	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));

	drawTerrain(mvp);
	drawEnvironment(envMvp);
}
};
App app;
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("InfiniteTerrainII_DX11");
	g_pRenderer11 = (RendererHLSLD3D11*)r;
	IInput*    i=STX_Service::GetInputInstance();
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}

