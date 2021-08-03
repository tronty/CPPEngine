//-----------------------------------------------------------------------------
// File: App.h
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------


#ifndef _APP_H_
#define _APP_H_

#include "../../Framework/Direct3D/D3D9App.h"

#define RT_SIZE 512

class App : public D3D9App {
public:
	char *getTitleString(){ return "R2VB Terrain Morph"; }
	char *getHomeDirectory(){ return "../DirectX 9.0/R2VB-TerrainMorph"; }
	void setup(char *args[], int nArguments);
	void resetCamera();

	bool load();

	void generateTerrain();
	void drawSkybox(const float4x4 &mvp);
	void drawFrame();

private:
	VertexFormatID terrainVF, skyboxVF, terrainGenVF;
	VertexBufferID terrainVB;
	IndexBufferID terrainIB;

	ShaderID skybox, terrainGen, terrain;
	TextureID env, R2VB_VBO, heightMap[4], terrainMap[2];

	float fracTime;
};

#endif
