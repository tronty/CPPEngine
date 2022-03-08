/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//
// This sample demostrates how to use R2VB to morph a height-map based terrain.  
// It's a quite simple demo using a pixel shader to interpolate the terrain from
// one height-map to another.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// A fullscreen quad
//-----------------------------------------------------------------------------------------
#if 1 // ???
static D3DXFROMWINEVECTOR2 vertices[] = {
	D3DXFROMWINEVECTOR2(-1, -1),
	D3DXFROMWINEVECTOR2( 1, -1),
	D3DXFROMWINEVECTOR2( 1,  1),
	D3DXFROMWINEVECTOR2(-1,  1)
};
#else
	float vertices[] = { -1, -1 , 1, -1, 1, 1, -1, 1};
#endif

#define RT_SIZE 512

class App {
public:
  unsigned int width, height;
  D3DXFROMWINEVECTOR3 camPos;
  float wx, wy;
//-----------------------------------------------------------------------------------------
// Set default parameters
//-----------------------------------------------------------------------------------------
void setup()
{
	width  = 800;
	height = 600;
}
//-----------------------------------------------------------------------------------------
// Reset position and viewing angles
//-----------------------------------------------------------------------------------------
void resetCamera()
{
	camPos = D3DXFROMWINEVECTOR3(1500, 1000, 0);
	wx = 0.7f;
	wy = D3DXFROMWINE_PI / 2;
}
//-----------------------------------------------------------------------------------------
// Load all the resources the sample needs
//-----------------------------------------------------------------------------------------
bool load()
{

	// Load shaders
	skybox = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-TerrainMorph/skybox.hlsl", "main", "main");
	terrain = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-TerrainMorph/terrain.hlsl", "main", "main");
	terrainGen = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-TerrainMorph/terrainGen.hlsl", "main", "main");
	/*
		Create render targets for the final terrain.
		Since this render target will be used as vertex buffers we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/

	R2VB_VBO = IRenderer::GetRendererInstance()->addRenderTarget(RT_SIZE, RT_SIZE, FORMAT_R32f, IRenderer::GetRendererInstance()->GetlinearClamp());

	// Load textures
	#if 0
	env = IRenderer::GetRendererInstance()->addCubemap("Mountains", "jpg");
	#else
    	sb.init("/SkyBox/cubemaps/x/MountainsGL.dds");
	#endif

	// Load height maps
	heightMap[0]=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/height1024.tga", false, IRenderer::GetRendererInstance()->Getlinear());
	heightMap[1]=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/height1024_1.tga", false, IRenderer::GetRendererInstance()->Getlinear());
	heightMap[2]=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/height1024_4.tga", false, IRenderer::GetRendererInstance()->Getlinear());
	heightMap[3]=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/height1024_5.tga", false, IRenderer::GetRendererInstance()->Getlinear());

	// Load diffuse maps
	terrainMap[0]=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/terrain.dds", false, IRenderer::GetRendererInstance()->GetlinearAniso());
	terrainMap[1]=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/terrain_3.dds", false, IRenderer::GetRendererInstance()->GetlinearAniso());

	// Create the main terrain vertex buffer. It's used for texture coordinates.
	std::vector<D3DXFROMWINEVECTOR2> vv(RT_SIZE * RT_SIZE);
	D3DXFROMWINEVECTOR2 *dest = &vv[0];
		for (int z = 0; z < RT_SIZE; z++){
			float fz = float(2 * z) / (RT_SIZE - 1) - 1.0f;
			for (int x = 0; x < RT_SIZE; x++){
				float fx = float(2 * x) / (RT_SIZE - 1) - 1.0f;
				*dest++ = D3DXFROMWINEVECTOR2(1024 * fx, 1024 * fz);
			}
		}
	terrainVB = IRenderer::GetRendererInstance()->addVertexBuffer(vv.size()*sizeof(D3DXFROMWINEVECTOR2), STATIC, &vv[0]);

	// Create the terrain index buffer
	std::vector<uint> vi(6 * (RT_SIZE - 1) * (RT_SIZE - 1));
	uint *iDest = &vi[0];
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
	terrainIB = IRenderer::GetRendererInstance()->addIndexBuffer(6 * (RT_SIZE - 1) * (RT_SIZE - 1), 2, STATIC, &vi[0]);

	// Create vertex declaration for the terrain vertex buffer
	FormatDesc waterAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		1, TYPE_TEXCOORD, FORMAT_FLOAT, 1,
	};
	if ((terrainVF = IRenderer::GetRendererInstance()->addVertexFormat(waterAttribs, elementsOf(waterAttribs), terrain)) == VF_NONE) return false;

	// Create vertex declaration for the terrain generation shader
	FormatDesc terrainAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((terrainGenVF = IRenderer::GetRendererInstance()->addVertexFormat(terrainAttribs, elementsOf(terrainAttribs), terrainGen)) == VF_NONE) return false;

	// Create vertex declaration for the skybox
	//FormatDesc skyboxAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	//if ((skyboxVF = IRenderer::GetRendererInstance()->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs), -1)) == VF_NONE) return false;

	//onReset();
	return true;
}

//-----------------------------------------------------------------------------------------
// Generate terrain vertex buffer by rendering height map data into an R2VB texture.
// Resulting height values and the texture coordinates form the final vertices.
//-----------------------------------------------------------------------------------------
void generateTerrain()
{
	// Smooth interpolation
	float t = 0.3f * 1000.0f * timeGetTime();

	int i = int(t);
	t -= i;
	i = (i % 3) + 1;

	fracTime = 4 * t * (1 - t);
	fracTime *= fracTime;

	// Change render target to R2VB texture
	IRenderer::GetRendererInstance()->changeRenderTarget(R2VB_VBO);

		IRenderer::GetRendererInstance()->setShader(terrainGen);
		IRenderer::GetRendererInstance()->setVertexFormat(terrainGenVF);
		IRenderer::GetRendererInstance()->setTexture("Tex", heightMap[0]);
		IRenderer::GetRendererInstance()->setTexture("Tex2", heightMap[i]);
		IRenderer::GetRendererInstance()->setShaderConstant1f("time", fracTime);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
}
//-----------------------------------------------------------------------------------------
// Render the skybox
//-----------------------------------------------------------------------------------------
void drawSkybox(const D3DXFROMWINEMATRIX &mvp)
{
	#if 0
	static const float verts[] = { -1,-1,-1, -1,-1,1, -1,1,-1, -1,1,1, 1,-1,-1, 1,-1,1, 1,1,-1, 1,1,1 };
	static const unsigned short inds[] = { 2,3,0, 0,3,1, 3,7,1, 1,7,5, 7,6,5, 5,6,4, 6,2,4, 4,2,0, 6,7,2, 2,7,3, 0,1,4, 4,1,5 };

	IRenderer::GetRendererInstance()->setShader(skybox);
	IRenderer::GetRendererInstance()->setVertexFormat(skyboxVF);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	IRenderer::GetRendererInstance()->setTexture("Skybox", env);
	IRenderer::GetRendererInstance()->setMask(eCOLOR);

	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, 8, 12, inds, inds, CONSTANT_INDEX2, verts, verts, 3 * sizeof(float));
	#else
        sb.render();
	#endif
}
//-----------------------------------------------------------------------------------------
// Draw the whole scene
//-----------------------------------------------------------------------------------------
void drawFrame()
{
	D3DXFROMWINEMATRIX projection = perspectiveMatrixX(D3DXFROMWINE_PI / 2, width, height, 1, 10000, true);
	D3DXFROMWINEMATRIX modelview = rotateXY4(-wx, -wy);
	D3DXFROMWINEMATRIX mvpSB = pegToFarPlane(projection * modelview);
	modelview.translate(-camPos);
	D3DXFROMWINEMATRIX mvp = projection * modelview;

	// Generate final terrain by interpolating two height maps
	generateTerrain();

	IRenderer::GetRendererInstance()->Clear(false, true, 0);

	// Using R2VB texture as a vertex buffer and use height values
	// as the vertex height position in the vertex shader.
	IRenderer::GetRendererInstance()->setShader(terrain);
	IRenderer::GetRendererInstance()->setVertexFormat(terrainVF);

	// Bind the render target to the DMAP sampler
	IRenderer::GetRendererInstance()->setTexture("DMAP", R2VB_VBO);
	/*
		Even though we get the vertices from the render target we still need to call setVertexBuffer()
		to set up the stride and offset. For this we need a valid dummy vertex buffer to make the call.
		If the debug runtime is used the buffer must be large enough to be valid for the draw call.
		This is only to make the runtime validation happy. For the release runtime the size does not
		matter since the buffer is ignored anyway.
		For this sample, the terrainVB vertex buffer can simply be used again for the R2VB stream.
		It will work with the debug runtime too since it's bigger than the amount of data that will
		be fetched from the render target. For cases where this is not true, one may have to create
		a separate dummy vertex buffer if the debug runtime is detected.
	*/
	IRenderer::GetRendererInstance()->setVertexBuffer(0, terrainVB);
	//IRenderer::GetRendererInstance()->setVertexBuffer(1, terrainVB); // Dummy vertex buffer. Only the stride and offset data will be used.
	IRenderer::GetRendererInstance()->setIndexBuffer(terrainIB);
	IRenderer::GetRendererInstance()->setTexture("base", terrainMap[0]);
	IRenderer::GetRendererInstance()->setTexture("base2", terrainMap[1]);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	// Control the maximum height value
	IRenderer::GetRendererInstance()->setShaderConstant1f("HeightFactor", 512.0f);
	// Control the number of texture tiles
	IRenderer::GetRendererInstance()->setShaderConstant1f("TexFactor", 1024.0f/2.0f);
	IRenderer::GetRendererInstance()->setShaderConstant1f("time", fracTime);
	

	IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, RT_SIZE * RT_SIZE, 0, 2 * (RT_SIZE - 1) * (RT_SIZE - 1));

	drawSkybox(mvpSB);
}

private:
	VertexFormatID terrainVF, skyboxVF, terrainGenVF;
	VertexBufferID terrainVB;
	IndexBufferID terrainIB;

	ShaderID skybox, terrainGen, terrain;
	TextureID env, R2VB_VBO, heightMap[4], terrainMap[2];
	stx_SkyBox sb;

	float fracTime;
};
App app;
int init(const char* aTitle)
{
	app.setup();
	app.resetCamera();
	app.load();
#ifdef R2VB_GUI
	BaseApp::initGUI();
#endif
	return 0;
}

void render()
{
	IRenderer::GetRendererInstance()->BeginScene();
	app.drawFrame();
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present();
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("R2VB-TerrainMorph");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

