//-----------------------------------------------------------------------------
// File: App.h
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------


#ifndef _APP_H_
#define _APP_H_

#include "../../Framework/Direct3D/D3D9App.h"

#define RT_SIZE 128
#define WATER_POS_Y 10.0f

class App : public D3D9App {
public:
	char *getTitleString(){ return "R2VB Water"; }
	char *getHomeDirectory(){ return "../DirectX 9.0/R2VB-Water"; }
	void setup(char *args[], int nArguments);
	void resetCamera();
	void moveCamera(const vec3 &dir);

	void initGUI();
	bool load();

	void onSize(const int w, const int h);
	bool onReset();

	void drawDrop(const float2 &xy, const float size, const float strength, const float intp);

	void updateWater(const float4x4 &mvp, const float frameTime);
	void drawSkybox(const float4x4 &mvp);
	void drawPool(const float4x4 &mvp);
	void drawMainScene(const float4x4 &mvpSB, const float4x4 &mvp, const float *clipPlane = NULL);
	void drawFrame();

private:
	VertexFormatID waterVF, skyboxVF, waveVF, poolVF;
	VertexBufferID waterVB;
	IndexBufferID waterIB;

	ShaderID wave, drop, skybox, water, pool;
	TextureID env, rt0, rt1, currRT, otherRT, brick, stone, reflection, refraction, depth;

	Slider *dropRate;

	bool needInit;
};

#endif
