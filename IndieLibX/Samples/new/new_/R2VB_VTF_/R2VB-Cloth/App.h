//-----------------------------------------------------------------------------
// File: App.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _APP_H_
#define _APP_H_

#include "../../Framework/Direct3D/D3D9App.h"
#include "../../Framework/Direct3D/atir2vb.h"
#include "../../Framework/Util/ProgrammerArt.h"
#include "../../Framework/Math/Perlin.h"

#define CLOTH_SIZE 64

#if (CLOTH_SIZE * CLOTH_SIZE <= 65536)
typedef ushort INDEX_TYPE;
#else
typedef uint INDEX_TYPE;
#endif

class App : public D3D9App {
public:
	char *getTitleString(){ return "R2VB Cloth"; }
	char *getHomeDirectory(){ return "../DirectX 9.0/R2VB-Cloth"; }

	void setup(char *args[], int nArguments);
	void resetCamera();

	void initGUI();
	void onMenuItemPress(Menu *menu, MenuItemID item);
	void onSize(const int w, const int h);

	bool load();
	bool unload();

	bool onReset();

	void drawFrame();

private:
	TextureID base, bump;
	ShaderID initCloth[6], physics, physicsMP[3], lighting, tangentSpace, hit, setNode, interleave;

	VertexFormatID quadVF, clothVF, clothInterleavedVF, hitVF;
	VertexBufferID clothVB;
	IndexBufferID clothIB;

	VertexBufferID dummy;

	LPDIRECT3DSURFACE9 grabCoordSurface;
	LPDIRECT3DSURFACE9 grabPosSurface;

	vec2 grabCoord;
	vec3 grabPos;
	float grabFree;
	vec3 grabNormal;
	float grabOffset;
	bool grabbed;

	TextureID pos[2], dir[2], tangent, binormal, normal, rtInterleaved, rtHit, rtHitDepth;
	int currRT, otherRT;
	bool needsInit;

	Menu *clothMenu;
	MenuItemID curtain, sail, safetyNet, roundTable, squareTable, squareTable2, multipass, separateStreams;
	Slider *xWind, *yWind, *zWind;
	Label *xLabel, *yLabel, *zLabel;
	Slider *xnWind, *ynWind, *znWind;
	Label *xnLabel, *ynLabel, *znLabel;
};

#endif
