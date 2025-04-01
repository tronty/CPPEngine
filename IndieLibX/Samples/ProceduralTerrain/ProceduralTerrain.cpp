/*
  Copyright (c) 2016 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <Framework3/ProceduralTerrain.h>
#include <STX/PerlinNoise.h>
#include <stdio.h>

class MainApp {
protected:
	TextureID env;
D3DXFROMWINEVECTOR4 fogColor;
D3DXFROMWINEVECTOR3 position;
float Wx, Wy, Wz, speed;
ShaderID skyBox;
VertexFormatID skyboxVF;
D3DXFROMWINEVECTOR3 camPos;
D3DXFROMWINEMATRIX mvp, modelView, projection;

	ProceduralTerrain *terrain;

public:
void drawEnvironment(const D3DXFROMWINEMATRIX &mvp);
int init(const char* aTitle)
{
	load();
	return 0;
}
void render()
{
	drawFrame();
}
	bool init_();
	bool load();
	bool unload();
	void drawFrame();
	
	void resetCamera();
	//void selectPixelFormat(PixelFormat &pixelFormat);
};

void MainApp::resetCamera(){
	position = D3DXFROMWINEVECTOR3(0,100,0);
	Wx = 0;
	Wy = 1.0f;
	Wz = 0;
}
/*
void MainApp::selectPixelFormat(PixelFormat &pixelFormat){
	pixelFormat.redBits   = 8;
	pixelFormat.greenBits = 8;
	pixelFormat.blueBits  = 8;
	pixelFormat.alphaBits = 0;

	pixelFormat.depthBits = 24;
}
*/
float terrainFunc(const float x, const float z){
	//return 256 * sinf(x * 0.00734f) * cosf(z * 0.00234f);
	return 256 * noise2(0.0031212312f * x, 0.004613212f * z) + 32 * noise2(0.017412412f * x, 0.01752341f * z);
}

bool MainApp::init_(){
	initPerlin();

	speed = 256;
	return true;
}

#if 1
bool loadSlicedImage(Image3& img, const char **fileNames, const int nImages, const int nArraySlices = 1, uint flags = 0){
	int maxImage = nImages? nImages : 6;
		//LOG_FNLN;
		//LOG_PRINT("maxImage=%d\n", maxImage);
		//LOG_PRINT("nArraySlices=%d\n", nArraySlices);

	Image3 *images = new Image3[maxImage * nArraySlices];

	for (int i = 0; i < maxImage * nArraySlices; i++){
		//LOG_FNLN;
		//LOG_PRINT("fileNames[%d]=%s\n", i, fileNames[i]);

		//if (!
		images[i].loadImage(fileNames[i], flags);
		if(0){
		//LOG_FNLN;
			delete [] images;
			return false;
		}
		//LOG_FNLN;
	}
		//LOG_FNLN;

	uint nMipMaps = images[0].getMipMapCount();
	ubyte *dest = img.create(images[0].getFormat(), images[0].getWidth(), images[0].getHeight(), nImages, nMipMaps, nArraySlices);

	for (int arraySlice = 0; arraySlice < nArraySlices; arraySlice++){
		int base = arraySlice * maxImage;

		for (uint level = 0; level < nMipMaps; level++){
			int size = images[0].getMipMappedSize(level, 1);
			for (int i = 0; i < maxImage; i++){
				stx_memcpy(dest, images[base + i].getPixels(level), size);
				dest += size;
			}
		}
	}

	delete [] images;
		//LOG_FNLN;

	return true;
}

TextureID addCubemap(const char **fileNames, const bool useMipMaps, const SamplerStateID samplerState = SS_NONE, const int nArraySlices = 1, uint flags = 0)
{
		//LOG_FNLN;
	Image3 img;
		//LOG_FNLN;
	if (loadSlicedImage(img, fileNames, 0, nArraySlices))
	{
		//LOG_FNLN;
		//if (img.getFormat() == FORMAT_RGBE8) img.unpackImage();
		if (useMipMaps && img.getMipMapCount() <= 1) img.createMipMaps();
		//LOG_FNLN;
		TextureID id=IRenderer::GetRendererInstance()->addTexture(&img,true, samplerState, flags);
		//LOG_FNLN;
		return id;
	}
	else
	{
		//LOG_FNLN;
		char str[1024];
        int n = stx_snprintf(str,1024, "Couldn't open cubemap:\n");
		//LOG_FNLN;
		for (int i = 0; i < 6 * nArraySlices; i++)
		{
            n += stx_snprintf(str + n,1024, "%s\n", fileNames[i]);
		}
		//LOG_FNLN;
		//LOG_PRINT(str);
		return TEXTURE_NONE;
	}
		//LOG_FNLN;
}
#endif

bool MainApp::load(){
	skyBox = IRenderer::GetRendererInstance()->addShaderFromFile("/InfiniteTerrain/InfiniteTerrain.hlsl", "main", "main");
	
	FormatDesc skyboxFmt[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	
	skyboxVF = IRenderer::GetRendererInstance()->addVertexFormat(skyboxFmt, elementsOf(skyboxFmt), skyBox /* ??? */ );

	#if 1	
	// Cubemap
	const char *files[] = {
		"/SkyBox/Dragonfire/posx.jpg", "/SkyBox/Dragonfire/negx.jpg",
		"/SkyBox/Dragonfire/posy.jpg", "/SkyBox/Dragonfire/negy.jpg",
		"/SkyBox/Dragonfire/posz.jpg", "/SkyBox/Dragonfire/negz.jpg",
	};
	if ((env = /* IRenderer::GetRendererInstance()-> */ addCubemap(files, true, IRenderer::GetRendererInstance()->GetbilinearClamp())) == TEXTURE_NONE) return false;
	#endif

	/*
	if ((ground = IRenderer::GetRendererInstance()->addTexture(	"/new/R2VB_VTF/InfiniteTerrainII/plast02.png",
									//"/Dragonfire/plast02.dds", 
					true, trilinearAniso)) == TEXTURE_NONE) return false;
	*/

	D3DXFROMWINEMatrixPerspectiveFovLH(&projection, D3DXFROMWINEToRadian( 90.0f ),
                                		float(IRenderer::GetRendererInstance()->GetViewportWidth())/IRenderer::GetRendererInstance()->GetViewportHeight(), 1.0f, 4000.0f );

	camPos=D3DXFROMWINEVECTOR3(0.0f, 0.8f, 0.0f);
	fogColor = D3DXFROMWINEVECTOR4(1.0f, 0.5f, 0.1f, 1.0f);

	terrain = new ProceduralTerrain(terrainFunc, 1024.0f, 128, 4, 140);

	return true;
}

bool MainApp::unload(){
	delete terrain;

	return true;
}

void MainApp::drawEnvironment(const D3DXFROMWINEMATRIX &mvp){
	static float vertices[] = { 0, 2, 0,  0, -2, -1,  -1, -2, 1,  1, -2, 1 };
	static ushort indices[] = { 0, 1, 2, 3, 0, 1 };

	
	IRenderer::GetRendererInstance()->setShader(skyBox);
	IRenderer::GetRendererInstance()->setVertexFormat(skyboxVF);
	IRenderer::GetRendererInstance()->setTexture("Env", env, IRenderer::GetRendererInstance()->GetbilinearClamp());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthWrite());
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	IRenderer::GetRendererInstance()->setShaderConstant4f("fogColor", fogColor);
	IRenderer::GetRendererInstance()->setShaderConstant2f("scaleBias", float2(2 + 0.3f * camPos.y, camPos.y * 1.3f - 1.2f));

	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLE_STRIP, 0, 4, 4, indices, indices, CONSTANT_INDEX2, vertices, vertices, sizeof(float3));
}

void MainApp::drawFrame(){
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
		D3DXFROMWINEMatrixRotationYawPitchRoll( &modelView,
		                            D3DXFROMWINEToRadian(-Wx),
		                            D3DXFROMWINEToRadian(-Wy),
		                            D3DXFROMWINEToRadian(-Wz) );
	
	D3DXFROMWINEMATRIX mvp=modelView*projection;
	drawEnvironment(mvp);

	D3DXFROMWINEMatrixTranslation(&modelView, -position.x, -position.y, -position.z);

	//IRenderer::GetRendererInstance()->setTextures(ground);
	//IRenderer::GetRendererInstance()->setTexGen(TEXGEN_OBJECT_LINEAR);
	
	/*IRenderer::GetRendererInstance()->changeTextureUnit(0);
	glMatrixMode(GL_TEXTURE);
	float mat[16] = {
		0.005f, 0,      0, 0,
		0,      0,      0, 0,
		0,      0.005f, 0, 0,
		0,      0,      0, 1
	};
	glLoadMatrixf(mat);
	*/

	#if 0
	int nVertices = terrain->draw(position, 4000, 0.0013f);
	#endif

	//glLoadIdentity();
/*
	char str[64];
	sprintf(str, "%.1f Mt/s", (nVertices / time) / 1000000);
	setDebugString(str);
*/
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("InfiniteTerrain");	
	IInput*    i=STX_Service::GetInputInstance();
	MainApp *app = new MainApp();
	app->init("");
	while (!STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app->render();
	}
	delete app;
	return 0;
}

