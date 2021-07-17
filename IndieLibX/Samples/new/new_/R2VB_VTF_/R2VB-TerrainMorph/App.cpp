//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//
// This sample demostrates how to use R2VB to morph a height-map based terrain.  
// It's a quite simple demo using a pixel shader to interpolate the terrain from
// one height-map to another.
//-----------------------------------------------------------------------------


#include "App.h"
#include "../../Framework/Direct3D/atir2vb.h"

BaseApp *app = new App();
//-----------------------------------------------------------------------------------------
// Set default parameters
//-----------------------------------------------------------------------------------------
void App::setup(char *args[], int nArguments)
{
	width  = 800;
	height = 600;
}
//-----------------------------------------------------------------------------------------
// Reset position and viewing angles
//-----------------------------------------------------------------------------------------
void App::resetCamera()
{
	camPos = float3(1500, 1000, 0);
	wx = 0.7f;
	wy = PI / 2;
}
//-----------------------------------------------------------------------------------------
// Load all the resources the sample needs
//-----------------------------------------------------------------------------------------
bool App::load()
{
	/*
		Create render targets for the final terrain.
		Since this render target will be used as vertex buffers we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	LPDIRECT3DTEXTURE9 d3drt;
	if (dev->CreateTexture(RT_SIZE, RT_SIZE, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_R32F, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((R2VB_VBO = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_LINEAR, CLAMP)) == TEXTURE_NONE) return false;

	// Load shaders
	if ((skybox = renderer->addShader("skybox.shd")) == SHADER_NONE) return false;
	if ((terrain = renderer->addShader("terrain.shd")) == SHADER_NONE) return false;
	if ((terrainGen = renderer->addShader("terrainGen.shd")) == SHADER_NONE) return false;

	// Load textures
	if ((env = renderer->addCubemap(
		TEX_PATH "Mountains/posx.jpg", TEX_PATH "Mountains/negx.jpg",
		TEX_PATH "Mountains/posy.jpg", TEX_PATH "Mountains/negy.jpg",
		TEX_PATH "Mountains/posz.jpg", TEX_PATH "Mountains/negz.jpg", FILTER_TRILINEAR)) == TEXTURE_NONE) return false;

	// Load height maps
	if ((heightMap[0] = renderer->addTexture(TEX_PATH "height1024.tga",   FILTER_LINEAR)) == TEXTURE_NONE) return false;
	if ((heightMap[1] = renderer->addTexture(TEX_PATH "height1024_1.tga", FILTER_LINEAR)) == TEXTURE_NONE) return false;
	if ((heightMap[2] = renderer->addTexture(TEX_PATH "height1024_4.tga", FILTER_LINEAR)) == TEXTURE_NONE) return false;
	if ((heightMap[3] = renderer->addTexture(TEX_PATH "height1024_5.tga", FILTER_LINEAR)) == TEXTURE_NONE) return false;

	// Load diffuse maps
	if ((terrainMap[0] = renderer->addTexture(TEX_PATH "terrain.dds",   FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;
	if ((terrainMap[1] = renderer->addTexture(TEX_PATH "terrain_3.dds", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	// Create the main terrain vertex buffer. It's used for texture coordinates.
	if ((terrainVB = renderer->addVertexBuffer(RT_SIZE * RT_SIZE * sizeof(float2))) == VB_NONE) return false;
	float2 *dest = (float2 *) renderer->lockVertexBuffer(terrainVB);
		for (int z = 0; z < RT_SIZE; z++){
			float fz = float(2 * z) / (RT_SIZE - 1) - 1.0f;
			for (int x = 0; x < RT_SIZE; x++){
				float fx = float(2 * x) / (RT_SIZE - 1) - 1.0f;
				*dest++ = float2(1024 * fx, 1024 * fz);
			}
		}
	renderer->unlockVertexBuffer(terrainVB);

	// Create the terrain index buffer
	if ((terrainIB = renderer->addIndexBuffer(6 * (RT_SIZE - 1) * (RT_SIZE - 1), sizeof(uint))) == IB_NONE) return false;
	uint *iDest = (uint *) renderer->lockIndexBuffer(terrainIB);
		for (int z = 0; z < RT_SIZE - 1; z++){
			for (int x = 0; x < RT_SIZE - 1; x++){
				*iDest++ = (z * RT_SIZE + x);
				*iDest++ = (z * RT_SIZE + x + 1);
				*iDest++ = ((z + 1) * RT_SIZE + x);
				*iDest++ = ((z + 1) * RT_SIZE + x);
				*iDest++ = (z * RT_SIZE + x + 1);
				*iDest++ = ((z + 1) * RT_SIZE + x + 1);
			}
		}
	renderer->unlockIndexBuffer(terrainIB);

	// Create vertex declaration for the terrain vertex buffer
	StreamVertexAttribute waterAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		1, TYPE_TEXCOORD, FORMAT_FLOAT, 1,
	};
	if ((terrainVF = renderer->addVertexFormat(waterAttribs, elementsOf(waterAttribs))) == VF_NONE) return false;

	// Create vertex declaration for the terrain generation shader
	VertexAttribute terrainAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((terrainGenVF = renderer->addVertexFormat(terrainAttribs, elementsOf(terrainAttribs))) == VF_NONE) return false;

	// Create vertex declaration for the skybox
	VertexAttribute skyboxAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 3 };
	if ((skyboxVF = renderer->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs))) == VF_NONE) return false;

	onReset();
	return true;
}
//-----------------------------------------------------------------------------------------
// A fullscreen quad
//-----------------------------------------------------------------------------------------
static float2 vertices[] = {
	float2(-1, -1),
	float2( 1, -1),
	float2( 1,  1),
	float2(-1,  1),
};
//-----------------------------------------------------------------------------------------
// Generate terrain vertex buffer by rendering height map data into an R2VB texture.
// Resulting height values and the texture coordinates form the final vertices.
//-----------------------------------------------------------------------------------------
void App::generateTerrain()
{
	// Smooth interpolation
	float t = 0.3f * time;

	int i = int(t);
	t -= i;
	i = (i % 3) + 1;

	fracTime = 4 * t * (1 - t);
	fracTime *= fracTime;

	// Change render target to R2VB texture
	renderer->changeRenderTarget(R2VB_VBO);

		renderer->setVertexFormat(terrainGenVF);
		renderer->setShader(terrainGen);
		renderer->setTexture("Tex", heightMap[0]);
		renderer->setTexture("Tex2", heightMap[i]);
		renderer->setShaderConstant1f("time", fracTime);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

	renderer->resetRenderTarget();
}
//-----------------------------------------------------------------------------------------
// Render the skybox
//-----------------------------------------------------------------------------------------
void App::drawSkybox(const float4x4 &mvp)
{
	static const float verts[] = { -1,-1,-1, -1,-1,1, -1,1,-1, -1,1,1, 1,-1,-1, 1,-1,1, 1,1,-1, 1,1,1 };
	static const unsigned short inds[] = { 2,3,0, 0,3,1, 3,7,1, 1,7,5, 7,6,5, 5,6,4, 6,2,4, 4,2,0, 6,7,2, 2,7,3, 0,1,4, 4,1,5 };

	renderer->setShader(skybox);
	renderer->setShaderConstant4x4f("mvp", mvp);
	renderer->setVertexFormat(skyboxVF);
	renderer->setTexture("Skybox", env);
	renderer->setMask(COLOR);
	renderer->apply();

	dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 12, inds, D3DFMT_INDEX16, verts, 3 * sizeof(float));
}
//-----------------------------------------------------------------------------------------
// Draw the whole scene
//-----------------------------------------------------------------------------------------
void App::drawFrame()
{
	float4x4 projection = perspectiveMatrixX(PI / 2, width, height, 1, 10000, true);
	float4x4 modelview = rotateXY4(-wx, -wy);
	float4x4 mvpSB = pegToFarPlane(projection * modelview);
	modelview.translate(-camPos);
	float4x4 mvp = projection * modelview;

	// Generate final terrain by interpolating two height maps
	generateTerrain();

	dev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	
	// Enable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));
	// Bind the render target to the DMAP sampler
	dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(R2VB_VBO));
	// Tell the driver that stream 1 is to be fetched from the DMAP texture
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DMAP));

	// Using R2VB texture as a vertex buffer and use height values
	// as the vertex height position in the vertex shader.
	renderer->setShader(terrain);
	renderer->setVertexFormat(terrainVF);
	/*
		Even though we get the vertices from the render target we still need to call SetStreamSource()
		to set up the stride and offset. For this we need a valid dummy vertex buffer to make the call.
		If the debug runtime is used the buffer must be large enough to be valid for the draw call.
		This is only to make the runtime validation happy. For the release runtime the size does not
		matter since the buffer is ignored anyway.
		For this sample, the terrainVB vertex buffer can simply be used again for the R2VB stream.
		It will work with the debug runtime too since it's bigger than the amount of data that will
		be fetched from the render target. For cases where this is not true, one may have to create
		a separate dummy vertex buffer if the debug runtime is detected.
	*/
	renderer->setVertexBuffer(terrainVB, 0);
	renderer->setVertexBuffer(terrainVB, 1); // Dummy vertex buffer. Only the stride and offset data will be used.
	renderer->setIndexBuffer(terrainIB);
	renderer->setTexture("base", terrainMap[0]);
	renderer->setTexture("base2", terrainMap[1]);
	renderer->setShaderConstant4x4f("mvp", mvp);
	// Control the maximum height value
	renderer->setShaderConstant1f("HeightFactor", 512.0f);
	// Control the number of texture tiles
	renderer->setShaderConstant1f("TexFactor", 1024.0f/2.0f);
	renderer->setShaderConstant1f("time", fracTime);
	renderer->apply();

	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, RT_SIZE * RT_SIZE, 0, 2 * (RT_SIZE - 1) * (RT_SIZE - 1));

	// Stream 1 restored to regular vertex buffer mode
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
	// Unbind the DMAP texture
	dev->SetTexture(D3DDMAPSAMPLER, NULL);
	// Disable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));

	drawSkybox(mvpSB);
}
