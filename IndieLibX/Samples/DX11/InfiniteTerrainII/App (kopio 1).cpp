/***********      .---.         .-"-.      *******************\
* -------- *     /   ._.       / Ž ` \     * ---------------- *
* Author's *     \_  (__\      \_°v°_/     * humus@rogers.com *
*   note   *     //   \\       //   \\     * ICQ #47010716    *
* -------- *    ((     ))     ((     ))    * ---------------- *
*          ****--""---""-------""---""--****                  ********\
* This file is a part of the work done by Humus. You are free to use  *
* the code in any way you like, modified, unmodified or copy'n'pasted *
* into your own work. However, I expect you to respect these points:  *
*  @ If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  @ For use in anything commercial, please request my approval.      *
*  @ Share your work and ideas too as much as you can.                *
\*********************************************************************/
/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
#include <STX/Noise.h>

#define TERRAIN_SIZE 512
#define INDEX_COUNT ((TERRAIN_SIZE - 1) * (2 * TERRAIN_SIZE + 1) - 1)

#define NOISE_SIZE 128

class App {
public:
	char *getTitle() const { return "Infinite Terrain II"; }
void resetCamera(){
	camPos = D3DXFROMWINEVECTOR3(0, 0.8f, 0);
	wx = 0;
	wy = 0;
	speed = 0.3f;
}

bool onKey(const uint key, const bool pressed){
	// ??? if (STX_Service::GetInputInstance()->OnKeyPress (key)) return true;

	if (STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F5)){
		showInfo = !showInfo;
		return true;
	}

	return false;
}

void onSize(const int w, const int h){
	STX_Service::GetWindowInstance()->ReSize(w, h, 32);

	lastX = INT_MAX;
	lastZ = INT_MAX;
}


int init(const char* aTitle)
{
	load();
	
	lightDir = normalize(D3DXFROMWINEVECTOR3(0, 0.75f, 1));
	fogColor = D3DXFROMWINEVECTOR4(1, 0.5f, 0.1f, 1);
	showInfo = false;

	// Generate a noise texture
	stx_srand(1);
	initNoise();
	Image3 noiseImage;
	float* nDest = (float*)noiseImage.create(FORMAT_R32F, NOISE_SIZE, NOISE_SIZE, 1, 1);
#if 1
	for (int z = 0; z < NOISE_SIZE; z++){
		for (int x = 0; x < NOISE_SIZE; x++){
			*nDest++ = tileableNoise2(x * 0.143f, z * 0.143f, 0.143f * NOISE_SIZE, 0.143f * NOISE_SIZE);
		}
	}
#elif 0
	std::string fn=stx_convertpath("/test.bmp");
    noiseImage.loadImage(fn.c_str());
#endif
	//noiseImage.normalize();
	//noiseImage.convert(FORMAT_I16);
	noise = IRenderer::GetRendererInstance()->addTexture(&noiseImage, true, IRenderer::GetRendererInstance()->Getlinear());
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
#if 1
	bool supportsHalf = false;// IRenderer::GetRendererInstance()->supportsHalf();

	lightDir = normalize(D3DXFROMWINEVECTOR3(0, 0.75f, 1));
	fogColor = D3DXFROMWINEVECTOR4(1, 0.5f, 0.1f, 1);
	showInfo = false;
#if 0
	// Generate a noise texture
	stx_srand(1);
	initNoise();

	Image3 noiseImage;
	float* nDest = (float*)noiseImage.create(FORMAT_R32F, NOISE_SIZE, NOISE_SIZE, 1, 1);
	for (int z = 0; z < NOISE_SIZE; z++) {
		for (int x = 0; x < NOISE_SIZE; x++) {
			*nDest++ = tileableNoise2(x * 0.143f, z * 0.143f, 0.143f * NOISE_SIZE, 0.143f * NOISE_SIZE);
		}
	}
	//noiseImage.normalize();
	//noiseImage.convert(FORMAT_I16);
	noise = IRenderer::GetRendererInstance()->addTexture(&noiseImage, true, IRenderer::GetRendererInstance()->Getlinear());
#endif

	// Shaders
	if ((genTerrain = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/InfiniteTerrainII/generateTerrain.hlsl", "main", "main")) == SHADER_NONE) return false;

	if ((tangentSpace = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/InfiniteTerrainII/tangentSpace.hlsl", "main", "main")) == SHADER_NONE) return false;

	if ((terrain = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/InfiniteTerrainII/terrain.hlsl", "main", "main")) == SHADER_NONE) return false;


	// Samplerstates
	if ((linear = IRenderer::GetRendererInstance()->addSamplerState(LINEAR, WRAP, WRAP, WRAP)) == SS_NONE) return false;
	if ((nearestClamp = IRenderer::GetRendererInstance()->addSamplerState(NEAREST, CLAMP, CLAMP, CLAMP)) == SS_NONE) return false;
	if ((bilinearClamp = IRenderer::GetRendererInstance()->addSamplerState(BILINEAR, CLAMP, CLAMP, CLAMP)) == SS_NONE) return false;
	if ((trilinearAniso = IRenderer::GetRendererInstance()->addSamplerState(TRILINEAR, WRAP, WRAP, WRAP)) == SS_NONE) return false;

	// Height render target
	heightRT = IRenderer::GetRendererInstance()->addRenderTarget(TERRAIN_SIZE, TERRAIN_SIZE, FORMAT_R32F, IRenderer::GetRendererInstance()->GetnearestClamp());
	// R2VB render target
	terrainRT = IRenderer::GetRendererInstance()->addRenderTarget(TERRAIN_SIZE, TERRAIN_SIZE, FORMAT_RGBA16F, IRenderer::GetRendererInstance()->GetnearestClamp());

    	sb.init("/SkyBox/cubemaps/x/DragonFireGL.dds");

	skyBox = -1;
	env = -1;
	ground = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/InfiniteTerrainII/plast02.png", true, IRenderer::GetRendererInstance()->GettrilinearAniso());

	// Vertex declaration
	FormatDesc terrainFmt[] = {
		0, TYPE_VERTEX, FORMAT_FLOAT, 2
		,0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
	};
	if ((terrainVF = IRenderer::GetRendererInstance()->addVertexFormat(terrainFmt, elementsOf(terrainFmt), terrain)) == VF_NONE) return false;

	FormatDesc quadFmt[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((genTerrainVF = IRenderer::GetRendererInstance()->addVertexFormat(quadFmt, elementsOf(quadFmt), genTerrain)) == VF_NONE) return false;
	if ((tangentSpaceVF = IRenderer::GetRendererInstance()->addVertexFormat(quadFmt, elementsOf(quadFmt), tangentSpace)) == VF_NONE) return false;

	//FormatDesc skyboxFmt[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	//if ((skyboxVF = IRenderer::GetRendererInstance()->addVertexFormat(skyboxFmt, elementsOf(skyboxFmt), -1)) == VF_NONE) return false;




	// Generate a vertex buffer for the terrain
	int vbSize = 4 * TERRAIN_SIZE * TERRAIN_SIZE;

		float *vertices = new float[vbSize];
		float *vDest = vertices+0;
		for (int z = 0; z < TERRAIN_SIZE; z++){
			for (int x = 0; x < TERRAIN_SIZE; x++){
				//*(vDest+0) = float(x) / TERRAIN_SIZE - 0.5f;
				//*(vDest+1) = float(z) / TERRAIN_SIZE - 0.5f;
				*(vDest+0) = 2.0f*float(x) / TERRAIN_SIZE - 1.0f;
				*(vDest+1) = 2.0f*float(z) / TERRAIN_SIZE - 1.0f;
				*(vDest+2) = float(x) / TERRAIN_SIZE;
				*(vDest+3) = float(z) / TERRAIN_SIZE;
				vDest+=4;
			}
		}
		if ((terrainVB = IRenderer::GetRendererInstance()->addVertexBuffer(vbSize * sizeof(float), STATIC, vertices)) == VB_NONE) return false;
		delete vertices;


	// Generate an index buffer for the terrain
	uint *indices = new uint[INDEX_COUNT];
	fillTerrainIB(indices, true);
	if ((terrainIBs[0] = IRenderer::GetRendererInstance()->addIndexBuffer(INDEX_COUNT, sizeof(uint), STATIC, indices)) == IB_NONE) return false;
	fillTerrainIB(indices, false);
	if ((terrainIBs[1] = IRenderer::GetRendererInstance()->addIndexBuffer(INDEX_COUNT, sizeof(uint), STATIC, indices)) == IB_NONE) return false;
	delete indices;

	nUpdates = 0;

#endif
	return true;
}

void drawEnvironment(const D3DXFROMWINEMATRIX &mvp){
#if 1
	sb.render();
#elif 0
	static float vertices[] = { 0, 2, 0,  0, -2, -1,  -1, -2, 1,  1, -2, 1 };
	static ushort indices[] = { 0, 1, 2, 3, 0, 1 };
	
	IRenderer::GetRendererInstance()->setShader(skyBox);
	IRenderer::GetRendererInstance()->setVertexFormat(skyboxVF);
	IRenderer::GetRendererInstance()->setTexture("Env", env, IRenderer::GetRendererInstance()->GetbilinearClamp());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthWrite());
	if(0) stx_Dump4x4f(mvp, "mvp");
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	if(0) stx_Dump4f(fogColor, "fogColor");
	IRenderer::GetRendererInstance()->setShaderConstant4f("fogColor", fogColor);
	D3DXFROMWINEVECTOR2 sB(2 + 0.3f * camPos.y, camPos.y * 1.3f - 1.2f);
	if(0) stx_Dump2f(sB, "scaleBias");
	IRenderer::GetRendererInstance()->setShaderConstant2f("scaleBias", sB);

	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLE_STRIP, 0, 4, 4, indices, indices, CONSTANT_INDEX2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR3));
#endif
}

void drawTerrain(const D3DXFROMWINEMATRIX &mvp){
	int x = round(camPos.x);
	int z = round(camPos.z);

	if (1){//(x != lastX || z != lastZ){

		const float triVertices[]  = { 0, 3,  3, -1,  -3, -1 };
	const float quadVertices[] =  {  1.0f,-1.0f,
			           1.0f, 1.0f,
			          -1.0f,-1.0f,
			          -1.0f, 1.0f };

#if 1
		IRenderer::GetRendererInstance()->changeRenderTarget(heightRT);

			IRenderer::GetRendererInstance()->setShader(genTerrain);
			IRenderer::GetRendererInstance()->setVertexFormat(genTerrainVF);
			IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
			IRenderer::GetRendererInstance()->setTexture("Noise", noise, IRenderer::GetRendererInstance()->Getlinear());
			D3DXFROMWINEVECTOR2 off(0.5f * x, -0.5f * z);
			if(0) stx_Dump2f(off, "offset");
	
			IRenderer::GetRendererInstance()->setShaderConstant2f("offset", off);
	        //IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
#if 0
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, quadVertices, quadVertices, 4*sizeof(float));
#elif 1
			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, 1, triVertices, triVertices, sizeof(D3DXFROMWINEVECTOR2));
#endif
			//return;
#endif
#if 1
		IRenderer::GetRendererInstance()->changeRenderTarget(terrainRT);
			
			IRenderer::GetRendererInstance()->setShader(tangentSpace);
			IRenderer::GetRendererInstance()->setVertexFormat(tangentSpaceVF);
			IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
			IRenderer::GetRendererInstance()->setTexture("Height", heightRT, IRenderer::GetRendererInstance()->GetnearestClamp());
			if(0) stx_Dump1f(1.0f / TERRAIN_SIZE, "pixel");
			if(0) stx_Dump1f(0.5f / TERRAIN_SIZE, "halfPixel");
			IRenderer::GetRendererInstance()->setShaderConstant1f("pixel", 1.0f / TERRAIN_SIZE);
			IRenderer::GetRendererInstance()->setShaderConstant1f("halfPixel", 0.5f / TERRAIN_SIZE);
#if 0
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, quadVertices, quadVertices, 2*sizeof(float));
#elif 1
			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, 1, triVertices, triVertices, sizeof(D3DXFROMWINEVECTOR2));
#endif
			//return;

		IRenderer::GetRendererInstance()->changeToMainFramebuffer();
#endif

		nUpdates++;
		lastX = x;
		lastZ = z;
	}

#if 1	
	IRenderer::GetRendererInstance()->setShader(terrain);
	IRenderer::GetRendererInstance()->setVertexFormat(terrainVF);
	IRenderer::GetRendererInstance()->setVertexBuffer(0, terrainVB);
	// Select an optimal index buffer for HiZ depending on which way we're looking.
	IRenderer::GetRendererInstance()->setIndexBuffer(terrainIBs[int(cosf(wy) < 0)]);
	IRenderer::GetRendererInstance()->setTexture("Ground", ground, IRenderer::GetRendererInstance()->GettrilinearAniso());
	IRenderer::GetRendererInstance()->setTexture("Terrain", terrainRT, IRenderer::GetRendererInstance()->GetnearestClamp());
	IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullBack());
	D3DXFROMWINEVECTOR3 offset(float(x), 0, float(z));
	
	D3DXFROMWINEMATRIX T; 
	D3DXFROMWINEMatrixTranslation(&T, offset.x, offset.y, offset.z);
	D3DXFROMWINEMATRIX _mvp_=mvp * T;
	if(0) stx_Dump4x4f( _mvp_, "_mvp_");
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", _mvp_);
	D3DXFROMWINEVECTOR3 v3=camPos - offset;
	if(0) stx_Dump3f(camPos, "camPos");
	IRenderer::GetRendererInstance()->setShaderConstant3f("camPos", v3);
	if(0) stx_Dump3f(lightDir, "lightDir");
	IRenderer::GetRendererInstance()->setShaderConstant3f("lightDir", lightDir);
	if(0) stx_Dump4f(fogColor, "fogColor");
	IRenderer::GetRendererInstance()->setShaderConstant4f("fogColor", fogColor);

	IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLE_STRIP, 0, 0, TERRAIN_SIZE * TERRAIN_SIZE, 0, INDEX_COUNT - 2);
#endif
}

void drawFrame(){
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	D3DXFROMWINEMATRIX mvp;
#if 1
	D3DXFROMWINEMATRIX projection, T, S; 
	D3DXFROMWINEMatrixPerspectiveFovLH(&projection, 1.5f, 1.0f*IRenderer::GetRendererInstance()->GetViewportWidth()/IRenderer::GetRendererInstance()->GetViewportHeight(), 0.001f, 2);
	D3DXFROMWINEMATRIX modelview; 
	D3DXFROMWINEMatrixRotationYawPitchRoll(&modelview, -wx, -wy, 0.0f);

	// Force depth to 1 for environment to put it behind everything
	D3DXFROMWINEMatrixTranslation(&T, 0, 0, 1);
	D3DXFROMWINEMatrixScaling(&S, 1, 1, 0);
	D3DXFROMWINEMATRIX envMvp = T * S * projection * modelview;

	D3DXFROMWINEMatrixTranslation(&modelview, -camPos.x, -camPos.y, -camPos.z );
	mvp = projection * modelview;
#endif

#if 1
	if(0)D3DXFROMWINEMatrixIdentity(&mvp);
	drawTerrain(mvp);
	if(0)drawEnvironment(envMvp);
	//stx_exit(0);
#elif 0
	drawTexture(red, D3DXFROMWINEVECTOR2(0.0f, 0.0f));
	drawTexture(blue, D3DXFROMWINEVECTOR2(0.5f, 0.5f));
#endif
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
protected:
	D3DXFROMWINEVECTOR3 lightDir;
	D3DXFROMWINEVECTOR4 fogColor;

	TextureID heightRT, terrainRT;
	ShaderID genTerrain, tangentSpace;

	TextureID noise, red, blue;

	VertexBufferID terrainVB;
	IndexBufferID  terrainIBs[2];
	VertexFormatID terrainVF, skyboxVF, genTerrainVF, tangentSpaceVF;//quadVF;

	TextureID ground, env;
	ShaderID terrain, skyBox;
	stx_SkyBox sb;

	SamplerStateID linear, nearestClamp, bilinearClamp, trilinearAniso;

	int lastX, lastZ;
	int nUpdates;
	bool showInfo;

	D3DXFROMWINEVECTOR3 camPos;
	float wx,wy,speed;
};

