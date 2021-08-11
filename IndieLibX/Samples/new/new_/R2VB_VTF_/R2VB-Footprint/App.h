#ifndef _APP_H_
#define _APP_H_

#include "../../Framework/Direct3D/D3D9App.h"
#include "../../Framework/Util/ATIModel.h"
#include "../../Framework/Util/R2VBAnimation.h"
#include "SnowSystem.h"

#define RT_SIZE 512

class Player : public R2VBAnimation {
public:
	ShaderID footprintShader;
	mat4 Trans;
	float Deg;
	bool bBounce;
	bool bInSafeZone;
	TextureID heightMap;

	bool load(char *modelname, char *diffuse, DWORD maxtexsize, LPDIRECT3DDEVICE9 lpdev, Renderer *lprenderer, bool debugruntime);	
	virtual void psCalculateAnimation(float t, float interp);
};


typedef struct {
	VertexFormatID VF;
	TextureID R2VB;
	VertexBufferID R2VB_DummyVBO, texCoordBuffer;
	IndexBufferID IB;

	TextureID heightMap, heightMapAccumulated, deformedHeightmap, terrainBuffer, snowDiffuse, snowBump, deformMap, deformMapDepth, noiseFillMap;
	TextureID blurDeformMap[2];

	VertexFormatID QuadVF;
	ShaderID generateHeightShd, accumulateHeightShd, deformShd, blurShd;
}Terrain;

class App : public D3D9App {
public:
	App() {bSnowing = false; currentBlurMap = 0; accu = 0.0f; };
	char *getTitleString(){ return "R2VB Footprint"; }
	char *getHomeDirectory(){ return "../DirectX 9.0/R2VB-Footprint"; }
	void setup(char *args[], int nArguments);
	void resetCamera();

	bool load();

	bool onReset();

	void resetTerrain();
	bool createTerrain(char *heightmap); 
	void accumulateTerrain(TextureID &accumulatedHeightMap, float speed);
	void renderDeformMap();
	void deformTerrain();
	void blurDeformMap(int times);
	void generateTerrain();
	void drawTerrain(const float4x4 &mvp);

	void AILoop();
	bool createPlayer(char *model, char *animation);
	void playPlayer();
	void drawPlayer(const float4x4 &mvp);

	void drawSkybox(const float4x4 &mvp);
	
	void drawFrame();

private:
	Terrain	terrain;
	Player	player;
	SnowSystem snowSys;
	bool bSnowing;
	int currentBlurMap;
	float snowingTime;
	float step, accu;

	VertexFormatID terrainVF, skyboxVF, terrainGenVF;
	VertexBufferID terrainVB;
	IndexBufferID terrainIB;

	ShaderID skybox, terrainGen, terrainSH, dumpShd;
	TextureID env, rt0, rt1;
};

#endif
