/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//-----------------------------------------------------------------------------
// File: App.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _APP_H_
#define _APP_H_

#include "../../Framework/OpenGL/GLApp.h"
#include "../../Framework/Util/Model.h"
#include "../../Framework/Util/BSP.h"
#include "../../Framework/Imaging/ImageTools.h"

class App : public GLApp {
public:
	char *getTitleString(){ return "Parallax mapping"; }
	char *getHomeDirectory(){ return "../OpenGL/ParallaxMapping"; }

	void initGUI();
	void onSize(const int w, const int h);

	void setup(char *args[], int nArguments);
	void resetCamera();
	void moveCamera(const D3DXFROMWINEVECTOR3 &dir);

	bool load();
	bool unload();

	void drawEnvironment();
	void drawLighting(const D3DXFROMWINEVECTOR3 &lightPos);
	void drawCeiling();
	void drawFrame();

private:
	TextureID wood, woodBump, stone, stoneBump, brick, brickBump, env, lightMap, ceiling;
	ShaderID shaders[28], envShader;

	TextureID stoneDist, woodDist, brickDist;
	D3DXFROMWINEVECTOR2 stoneDistSB, woodDistSB, brickDistSB;

	Menu *parallaxMenu;
	MenuItemID noParallax, parallaxMapping, parallaxRefined, parallaxDistance, parallaxDistanceSS, parallaxOccMapping, parallaxOccMappingSS, normalMapping, staticShadow;
	Slider *quality;

	VertexFormatID envVF;

	Model *room;
	BSP bsp;

	D3DXFROMWINEVECTOR3 lightPos;
};

#endif // _APP_H_
