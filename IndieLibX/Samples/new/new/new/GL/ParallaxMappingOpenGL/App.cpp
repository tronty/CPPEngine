
//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>




#include "App.h"

BaseApp *app = new App();

void App::initGUI(){
	parallaxMenu = mainMenu->addMenu("Parallax mapping");
	noParallax           = parallaxMenu->addItem("No parallax", MENU_CHECKABLE);
	parallaxMapping      = parallaxMenu->addItem("Parallax mapping", MENU_CHECKABLE);
	parallaxRefined      = parallaxMenu->addItem("Parallax with refinement", MENU_CHECKABLE | MENU_CHECKED);
	parallaxDistance     = parallaxMenu->addItem("Parallax with distance function", MENU_CHECKABLE);
	parallaxDistanceSS   = parallaxMenu->addItem("Parallax with distance function supersampled", MENU_CHECKABLE);
	parallaxOccMapping   = parallaxMenu->addItem("Parallax occlusion mapping", MENU_CHECKABLE);
	parallaxOccMappingSS = parallaxMenu->addItem("Parallax occlusion mapping supersampled", MENU_CHECKABLE);
	normalMapping        = parallaxMenu->addItem("Normal mapping", MENU_CHECKABLE | MENU_CHECKED);
	staticShadow         = parallaxMenu->addItem("Static shadow",  MENU_CHECKABLE | MENU_CHECKED);

	parallaxMenu->setRadioButtonGroup(noParallax, parallaxOccMappingSS, 1);

	addWidget(quality = new Slider(770, 340, 26, 256, false, 0, 1, 0.7f));

	GLApp::initGUI();
}

void App::onSize(const int w, const int h){
	GLApp::onSize(w, h);

	quality->setPosition(w - 30.0f, h - 260.0f);
}

void App::setup(char *args[], int nArguments){
	width  = 800;
	height = 600;

	lightPos = D3DXFROMWINEVECTOR3(-3000, 2050, 200);
}

void App::resetCamera(){
	camPos = D3DXFROMWINEVECTOR3(-600, -200, -550);
	wx = 0.03f;
	wy = -1.4f;
}

void App::moveCamera(const D3DXFROMWINEVECTOR3 &dir){
	D3DXFROMWINEVECTOR3 newPos = camPos + dir * (speed * frameTime);

	// Collision detection
	D3DXFROMWINEVECTOR3 point;
	const BTri *tri;
	if (bsp.intersects(camPos, newPos, &point, &tri)){
		newPos = point + tri->normal;
	}
	bsp.pushSphere(newPos, 40);

	camPos = newPos;
}

bool App::load(){
	if (!GLSL_Flag){
		ERRORMSG("No GLSL support");
		return false;
	}

	int maxShader;
	if (GL_ATI_shader_texture_lod_Flag){
		parallaxMenu->toggleItemCheck(parallaxDistance);
		maxShader = 28;
	} else {
		parallaxMenu->enableItem(parallaxDistance,   false);
		parallaxMenu->enableItem(parallaxDistanceSS, false);
		parallaxMenu->enableItem(parallaxOccMapping,   false);
		parallaxMenu->enableItem(parallaxOccMappingSS, false);
		maxShader = 12;
	}

	// Load shaders
	char str[1024];
	for (int i = 0; i < maxShader; i++){
		str[0] = '\0';
		if (i &  1) strcat(str, "#define BUMP_MAPPING\n");
		if (i &  2) strcat(str, "#define STATIC_SHADOW\n");
		if (i >= 4) strcat(str, "#define PARALLAX_MAPPING\n");
		if (i >= 8){
			if (i < 12){
				strcat(str, "#define PARALLAX_REFINEMENT\n");
			} else if (i < 20){
				strcat(str, "#define PARALLAX_DISTANCE\n");
				if (i >= 16) strcat(str, "#define SUPERSAMPLE\n");
			} else {
				strcat(str, "#define PARALLAX_OCCLUSION_MAPPING\n");
				if (i >= 24) strcat(str, "#define SUPERSAMPLE\n");
			}
		}

		static const char *attribs[] = { 0, "textureCoord", "tangent", "binormal", "normal", "lightmapCoord" };
		if ((shaders[i] = renderer->addShader("parallax.hlsl", attribs, elementsOf(attribs), str)) == SHADER_NONE) return false;
	}
	if ((envShader = renderer->addShader("env.hlsl")) == SHADER_NONE) return false;

	// Load textures
	if ((stone = renderer->addTexture(TEX_PATH "StoneWall.dds",  FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;
	if ((wood  = renderer->addTexture("/new/R2VB_VTF/R2VB/parqfloor2.dds", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;
	if ((brick = renderer->addTexture(TEX_PATH "brick.dds", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	/*
		Load bumpmaps and distance maps.

		A distance map is a volume texture that at each pixel stores the distance to the closest point
		on the surface. This is useful for ray-tracing the bumpmap as it gives us the length we can
		safely offset our texture coordinates without missing any bumps.
	*/
	Image bump, dist;
	if (!bump.loadPNG(TEX_PATH "StoneWallBump.png")) return false;
	stoneDistSB = computeDistanceMap(TEX_PATH "StoneWallDist", dist, bump, 64, 2, true);
	if ((stoneBump = renderer->addTexture(bump, FILTER_TRILINEAR_ANISO, NORMALMAP)) == TEXTURE_NONE) return false;
	if ((stoneDist = renderer->addTexture(dist, FILTER_LINEAR, CLAMP_R)) == TEXTURE_NONE) return false;

	if (!bump.loadPNG(TEX_PATH "parqfloor2Bump.png")) return false;
	woodDistSB = computeDistanceMap(TEX_PATH "parqfloor2Dist", dist, bump, 64, 2, true);
	if ((woodBump = renderer->addTexture(bump, FILTER_TRILINEAR_ANISO, NORMALMAP)) == TEXTURE_NONE) return false;
	if ((woodDist = renderer->addTexture(dist, FILTER_LINEAR, CLAMP_R)) == TEXTURE_NONE) return false;

	if (!bump.loadPNG(TEX_PATH "brickBump.png")) return false;
	brickDistSB = computeDistanceMap(TEX_PATH "brickDist", dist, bump, 64, 2, true);
	if ((brickBump = renderer->addTexture(bump, FILTER_TRILINEAR_ANISO, NORMALMAP)) == TEXTURE_NONE) return false;
	if ((brickDist = renderer->addTexture(dist, FILTER_LINEAR, CLAMP_R)) == TEXTURE_NONE) return false;

	if ((ceiling = renderer->addTexture(TEX_PATH "hex.png", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	VertexAttribute attribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 3 };
	if ((envVF = renderer->addVertexFormat(attribs, elementsOf(attribs))) == VF_NONE) return false;

	// Load scene geometry
	room = new Model();
	room->addFormat(TYPE_VERTEX,  3); // Vertex_
	room->addFormat(TYPE_GENERIC, 2); // TexCoord
	room->addFormat(TYPE_GENERIC, 3); // Tangent
	room->addFormat(TYPE_GENERIC, 3); // Binormal
	room->addFormat(TYPE_GENERIC, 3); // Normal
	room->addFormat(TYPE_GENERIC, 2); // Lightmap texcoord
	if (!room->load(MODEL_PATH "SimpleTemple/Temple.model")) return false;
	if (!room->makeDrawable(renderer)) return false;

	// Load BSP for collision detection
	if (!bsp.load(MODEL_PATH "SimpleTemple/Temple.bsp")) return false;

	if ((lightMap = renderer->addTexture(MODEL_PATH "SimpleTemple/lightmap.png", FILTER_LINEAR, CLAMP)) == TEXTURE_NONE) return false;

	if ((env = renderer->addCubemap(
		TEX_PATH "Mountains/posx.jpg",
		TEX_PATH "Mountains/negx.jpg",
		TEX_PATH "Mountains/posy.jpg",
		TEX_PATH "Mountains/negy.jpg",
		TEX_PATH "Mountains/posz.jpg",
		TEX_PATH "Mountains/negz.jpg", FILTER_LINEAR)) == TEXTURE_NONE) return false;

	return true;
}

bool App::unload(){
	delete room;

	return true;
}

void App::drawEnvironment(){
	static const float vertices[] = { 0, 1, 0,  0, -1, -1,  1, -1, 1,  -1, -1, 1 };
	static const ushort indices[] = { 0, 1, 2, 3, 0, 1 };

	renderer->setVertexFormat(envVF);
	renderer->setVertexBuffer(VF_NONE, 0, (intptr) vertices);
	renderer->setShader(envShader);
	renderer->setTexture("Env", env);
	renderer->setMask(COLOR);
	renderer->apply();

	glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, indices);
}

void App::drawLighting(const D3DXFROMWINEVECTOR3 &lightPos){
	uint parallaxMethod = parallaxMenu->getCheckedRadioButton(1);
	// The quality value is only relevant for parallax occlusion mapping and parallax with distance function
	quality->setVisible(parallaxMethod >= parallaxDistance);

	// Compute which shader to use depending on what options are selected
	int shaderIndex = (parallaxMethod << 2)
		| (int(parallaxMenu->isItemChecked(staticShadow)) << 1)
		| (int(parallaxMenu->isItemChecked(normalMapping)));
	bool bumpmap = (parallaxMenu->isItemChecked(normalMapping) || parallaxMethod == parallaxMapping ||
		parallaxMethod == parallaxRefined || parallaxMethod == parallaxOccMapping || parallaxMethod == parallaxOccMappingSS);

	// Set shader, constants and textures as needed
	renderer->setShader(shaders[shaderIndex]);
	renderer->setShaderConstant3f("lightPos", lightPos);
	renderer->setShaderConstant3f("camPos", camPos);
	renderer->setShaderConstant1f("ambient", 0.12f);
	renderer->setTexture("Base", wood);
	if (bumpmap) renderer->setTexture("Bump", woodBump);
	if (parallaxMethod >= parallaxDistance && parallaxMethod <= parallaxDistanceSS){
		renderer->setTexture("DistanceMap", woodDist);
		renderer->setShaderConstant2f("scaleBias", woodDistSB);
		float val = 1.0f - quality->getValue();
		renderer->setShaderConstant1f("threshold", 0.0005f * powf(128, val));
	} else if (parallaxMethod >= parallaxOccMapping && parallaxMethod <= parallaxOccMappingSS){
		renderer->setShaderConstant1f("maxStepCount", 8 + 120 * quality->getValue());
	}
	if (parallaxMenu->isItemChecked(staticShadow)){
		renderer->setTexture("LightMap", lightMap);
	}
	renderer->setCullFace(BACK);
	renderer->apply();

	// Walls
	room->drawBatch(renderer, 0, TRIANGLES);
	room->drawBatch(renderer, 1, TRIANGLES);

	// Pillars
	renderer->changeTexture("Base", brick);
	if (bumpmap) renderer->changeTexture("Bump", brickBump);
	if (parallaxMethod >= parallaxDistance && parallaxMethod <= parallaxDistanceSS){
		renderer->changeTexture("DistanceMap", brickDist);
		renderer->changeShaderConstant2f("scaleBias", brickDistSB);
	}
	for (uint i = 2; i < 10; i++){
		room->drawBatch(renderer, i, TRIANGLE_STRIP);
	}

	// Arcs
	renderer->changeTexture("Base", stone);
	if (bumpmap) renderer->changeTexture("Bump", stoneBump);
	if (parallaxMethod >= parallaxDistance && parallaxMethod <= parallaxDistanceSS){
		renderer->changeTexture("DistanceMap", stoneDist);
		renderer->changeShaderConstant2f("scaleBias", stoneDistSB);
	}
	for (uint i = 10; i < room->getBatchCount(); i++){
		room->drawBatch(renderer, i, TRIANGLE_STRIP);
	}
}

void App::drawCeiling(){
	// Blend the glass ceiling over the image
	renderer->setTexture(TMU0, ceiling);
	renderer->setBlending(ZERO, SRC_COLOR);
	renderer->setMask(COLOR);
	renderer->apply();

	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-1536, 512,  512);
		glTexCoord2f(3, 0);
		glVertex3f( 1536, 512,  512);
		glTexCoord2f(3, 1);
		glVertex3f( 1536, 512, -512);
		glTexCoord2f(0, 1);
		glVertex3f(-1536, 512, -512);
	glEnd();
}

void App::drawFrame(){
	// Compute matrixes
	D3DXFROMWINEMATRIX projection = perspectiveMatrixX(1.5f, width, height, 10, 5000, false);
	D3DXFROMWINEMATRIX modelview = rotateXY4(-wx, -wy);
	D3DXFROMWINEMATRIX envMV = modelview;
	modelview.translate(-camPos);
	// Force depth to 1 for environment
	D3DXFROMWINEMATRIX envProj = pegToFarPlane(projection);


	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadTransposeMatrixfARB(projection);
	glMatrixMode(GL_MODELVIEW);
	glLoadTransposeMatrixfARB(modelview);

	// Depth only pass to speed things up. It's only worth it for the pillars so we skip it for the rest.
	renderer->setMask(DEPTH);
	renderer->apply();

	for (unsigned int i = 2; i < 10; i++){
		room->drawBatch(renderer, i, GL_TRIANGLE_STRIP);
	}

	// Draw lighting
	drawLighting(lightPos);


	// Draw environment
	glMatrixMode(GL_PROJECTION);
	glLoadTransposeMatrixfARB(envProj);
	glMatrixMode(GL_MODELVIEW);
	glLoadTransposeMatrixfARB(envMV);

	drawEnvironment();


	// Draw ceiling
	glMatrixMode(GL_PROJECTION);
	glLoadTransposeMatrixfARB(projection);
	glMatrixMode(GL_MODELVIEW);
	glLoadTransposeMatrixfARB(modelview);

	drawCeiling();
}
};
App app;
int init(const char* aTitle)
{
    //app.OnCreateDevice();	
	return 0;
}

void render()
{
   double fTime=0;
   float fElapsedTime=0.0f;
   //app.OnFrameRender(fTime, fElapsedTime); 
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("ParallaxMappingOpenGL");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		IRenderer::GetRendererInstance()->BeginScene();
		render();
		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present( );
	}
	return 0;
}

