//-----------------------------------------------------------------------------
// File: App.h
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _APP_H_
#define _APP_H_

#include "../../Framework/Direct3D/D3D9App.h"
#include "../../Framework/Direct3D/atir2vb.h"

#include "../../Framework/Math/Perlin.h"
#include "../../Framework/Util/KDTree.h"
#include "../../Framework/Util/ProgrammerArt.h"

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

class App : public D3D9App {
public:
	char *getTitleString(){ return "R2VB Transparent particles sorting"; }
	char *getHomeDirectory(){ return "../DirectX 9.0/R2VB-Sort"; }
	void setup(char *args[], int nArguments);
	void resetCamera();
	void moveCamera(const float3 &dir);

	void drawSkybox(const float4x4 &mvp);

	void animateSmoke(const float3 &windDir);
	void assignDistance();
	void merge(const int start, const int end);
	void mergeRows(const int start, const int end);
	void sort();
	void quadify(const float3 &dx, const float3 &dy);

	bool onReset();

	bool load();
	bool unload();

	void drawFrame();

private:
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

	int currSrc, currDst;

	ShaderID bitonicSort, distance, makeQuads, initSmoke, smoke;

	TextureID posRT[2], distRT[2], particlesRT, smokePosRT[2], smokeDirRT[2];
	TextureID foliageSource, smokeSource;
	int currSmoke;

	ShaderID particles;
	TextureID particleVF;
	VertexBufferID dummy;
	VertexBufferID texCoords;
	IndexBufferID particleIB;

	VertexFormatID quadVF;
	VertexBufferID quadVB;

	bool needsInit;
};

#endif
