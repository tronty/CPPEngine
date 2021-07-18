//-----------------------------------------------------------------------------
// File: App.h
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _APP_H_
#define _APP_H_

#include "../../Framework/Direct3D/D3D9App.h"
#include "../../Framework/Util/ProgrammerArt.h"
#include "../../Framework/Direct3D/atir2vb.h"

#define ROWS_PER_DIP 64
#define MAX_WIDTH 1024

#if (ROWS_PER_DIP * MAX_WIDTH <= 65536)
#define INDEX_TYPE ushort
#else
#define INDEX_TYPE uint
#endif


class App : public D3D9App {
public:
	char *getTitleString(){ return "R2VB Particle system"; }
	char *getHomeDirectory(){ return "../DirectX 9.0/R2VB-ParticleSystem"; }
	void setup(char *args[], int nArguments);
	void resetCamera();

	void initGUI();
	void onSize(const int w, const int h);

	bool load();
	bool unload();

	bool onReset();

	void drawSkybox(const float4x4 &mvp);
	void drawFrame();

private:
	VertexFormatID skyboxVF;

	ShaderID skybox, floorShader, particleShader, physics, initPhysics, makeQuads;
	TextureID env, randTex, floorTex, particle, palette;

	VertexFormatID particleVF, physicsVF;
	VertexBufferID quadCoordVB;
	IndexBufferID particleIndices;

	TextureID rtPos[2], rtDir[2], r2vbRT;
	int currRT, otherRT;
	int rtWidth, rtHeight;

	Model *floor;

	Slider *particleCount, *particleSize, *yPower, *xzPower;
	Label *countLabel, *xzLabel, *yLabel, *scLabel;
};

#endif
