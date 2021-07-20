/***********      .---.         .-"-.      *******************\
* -------- *     /   ._.       / ´ ` \     * ---------------- *
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

#include "App.h"

BaseApp *app = new App();

void App::resetCamera(){
	camPos = vec3(-0.8f, 26.5f, 39.0f);
	wx = 0.0;
	wy = 2.0;
}


bool App::init(){
	speed = 100;

	return true;
}

void App::exit(){
}

bool App::load(){
	// Shaders
	if ((particleShader = renderer->addShader("particles.shd")) == SHADER_NONE) return false;
	if ((terrainShader = renderer->addShader("terrain.shd")) == SHADER_NONE) return false;
	if ((skyboxShader = renderer->addShader("skybox.shd")) == SHADER_NONE) return false;

	// Samplerstates
	if ((trilinearClamp = renderer->addSamplerState(TRILINEAR, CLAMP, CLAMP, CLAMP)) == SS_NONE) return false;
	if ((trilinearAniso = renderer->addSamplerState(TRILINEAR_ANISO, WRAP, WRAP, WRAP)) == SS_NONE) return false;

	// Textures
	if ((particle = renderer->addTexture("../Textures/Volumes/explosion3.dds", false, linearClamp)) == TEXTURE_NONE) return false;
	if ((terrain = renderer->addTexture("Terrain.dds", false, linearClamp)) == TEXTURE_NONE) return false;
	if ((lava = renderer->addTexture("../Textures/Lava.jpg", true, trilinearAniso)) == TEXTURE_NONE) return false;

	const char *files[] = {
		"../Textures/CubeMaps/Mercury/posx.jpg", "../Textures/CubeMaps/Mercury/negx.jpg",
		"../Textures/CubeMaps/Mercury/posy.jpg", "../Textures/CubeMaps/Mercury/negy.jpg",
		"../Textures/CubeMaps/Mercury/posz.jpg", "../Textures/CubeMaps/Mercury/negz.jpg",
	};
	if ((env = renderer->addCubemap(files, true, trilinearClamp)) == TEXTURE_NONE) return false;

	// Blendstates
	if ((blendAdd = renderer->addBlendState(ONE, ONE)) == BS_NONE) return false;

	// Rasterizerstates
	if ((noMSAA = renderer->addRasterizerState(CULL_NONE, SOLID, false, false)) == BS_NONE) return false;

	return true;
}

void App::unload(){

}

void App::drawFrame(){
//	device->ClearRenderTargetView(backBufferRTV, float4(0, 0, 0, 0));
	device->ClearDepthStencilView(depthBufferDSV, D3D10_CLEAR_DEPTH, 1.0f, 0);


	float4x4 projection = toD3DProjection(perspectiveMatrixX(1.5f, width, height, 0.1f, 500));
	float4x4 modelview = rotateXY(-wx, -wy);
	float4x4 invMvpEnv = !(projection * modelview);
	modelview.translate(-camPos);
	float4x4 mvp = projection * modelview;

	/*
		Terrain
	*/
	renderer->reset();
	renderer->setRasterizerState(cullBack);
	renderer->setShader(terrainShader);
	renderer->setShaderConstant4x4f("mvp", mvp);
	renderer->setTexture("terrain", terrain);
	renderer->setTexture("ground", lava);
	renderer->setSamplerState("filter", trilinearAniso);
	renderer->apply();

	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	device->DrawInstanced(256, 255, 0, 0);


	/*
		Skybox
	*/
	renderer->reset();
	renderer->setRasterizerState(noMSAA);
	renderer->setShader(skyboxShader);
	renderer->setShaderConstant4x4f("invMvp", invMvpEnv);
	renderer->setTexture("env", env);
	renderer->setSamplerState("filter", trilinearClamp);
	renderer->setDepthState(noDepthWrite);
	renderer->apply();

	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device->Draw(3, 0);


	/*
		Particle systems
	*/
	renderer->reset();
	renderer->setRasterizerState(noMSAA);
	renderer->setShader(particleShader);
	renderer->setShaderConstant4x4f("mvp", mvp);
	renderer->setShaderConstant2f("size", 2.6f * float2(1.0f, float(width) / float(height)));
	renderer->setShaderConstant1f("time", time);
	renderer->setTexture("tex", particle);
	renderer->setTexture("terrain", terrain);
	renderer->setSamplerState("filter", linearClamp);
	renderer->setBlendState(blendAdd);
	renderer->setDepthState(noDepthWrite);
	renderer->apply();

	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	device->DrawInstanced(32, 1024, 0, 0);
}
