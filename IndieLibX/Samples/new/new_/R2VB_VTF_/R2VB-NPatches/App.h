//-----------------------------------------------------------------------------
// File: App.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _APP_H_
#define _APP_H_

#include "../../Framework/Direct3D/D3D9App.h"
#include "../../Framework/Direct3D/atir2vb.h"

class App : public D3D9App {
public:
	char *getTitleString(){ return "R2VB N-Patches"; }
	char *getHomeDirectory(){ return "../DirectX 9.0/R2VB-NPatches"; }
	void setup(char *args[], int nArguments);
	void resetCamera();

	void initGUI();
	void onMenuItemPress(Menu *menu, MenuItemID item);

	void changeNPatchLevel(const int level);

	bool load();
	bool unload();

	void drawFrame();

private:
	ShaderID nPatch, lighting, nPatchLighting, wireframe;
	TextureID base;

	VertexFormatID quadVF, nPatchVF;
	VertexBufferID dummy;
	IndexBufferID nPatchIndices[8];

	TextureID weights[8];

	TextureID modelVertices[3], modelNormals[3];
	int texWidth, texHeight;
	int nFaces;

	TextureID rtVertex, rtNormal;

	LPD3DXMESH tiger;

	Menu *nPatchMenu;
	MenuItemID noNPatches, updateEveryFrame, drawWireframe;

	bool needsUpdate;
};

#endif
