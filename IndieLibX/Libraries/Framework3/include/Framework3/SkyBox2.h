#ifndef __SkyBox2__
#define __SkyBox2__
/*
  Copyright (c) 2018 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>
#include <GUI/GUIUtils.h>

const bool mipmaps=false;
#define CUBEMAPS 1

#if defined(CUBEMAPS)
const char* GUILayout="/SkyBox/GUILayout2.xml";
const char* filename[] =
{
"/SkyBox/cubemaps/x/DragonFireDX.dds",
"/SkyBox/cubemaps/x/MountainsDX.dds",
"/SkyBox/cubemaps/x/OlympusDX.dds",
"/SkyBox/cubemaps/x/DragonFireGL.dds",
"/SkyBox/cubemaps/x/MountainsGL.dds",
"/SkyBox/cubemaps/x/OlympusGL.dds",
"/SkyBox/cubemaps/CloudyHillsCubemap2.dds",
"/SkyBox/cubemaps/IncloudsCubemap.dds",
"/SkyBox/cubemaps/nvlobby_cube_mipmap.dds",
"/SkyBox/cubemaps/StPauls_CubeMap.dds",
"/SkyBox/cubemaps/StPauls_CubeMap_Compressed.dds"
};
const char* filename_[] =
{
"DragonFireDX",
"MountainsDX",
"OlympusDX",
"DragonFireGL",
"MountainsGL",
"OlympusGL",
"CloudyHillsCubemap2",
"IncloudsCubemap",
"nvlobby_cube_mipmap",
"StPauls_CubeMap",
"StPauls_CubeMap_Compressed"
};
#else
//const unsigned int elementsOf(filename)=10;
const char* GUILayout="/SkyBox/GUILayout.xml";
const char* filename[] =
{
"cube",
"dots",       
"Mountains", 
"Olympus",   
"Terrain",
"Dragonfire",
"nvlobby",
//"Terrain_dark",
"Heightmap",
"StPauls",
//"skybox",
"Terrain2",
//"DX2010",
0
};
const char* ext[] =
{
"png",
"png",       
"jpg", 
"jpg",   
"png",
"jpg",
"png",
//"png",
"bmp",
"bmp",
//"jpg",
"png",
//"JPG",
0
};
#endif

class SkyBox2
{
D3DXFROMWINEVECTOR4 fogColor;
TextureID env;
ShaderID skyBox;
VertexFormatID skyboxVF;
D3DXFROMWINEVECTOR3 camPos;
D3DXFROMWINEMATRIX mvp;
unsigned int m_i;
public:
int init(const char* aTitle)
{
	m_i=0;
	//mvp=(1.5f, width, height, 0.001f, 2)
	D3DXFROMWINEMatrixPerspectiveFovLH(&mvp, D3DXFROMWINE_PI / 2.0f, float(IRenderer::GetRendererInstance()->GetViewportWidth())/float(IRenderer::GetRendererInstance()->GetViewportHeight()), 0.001f, 1200000.0f);//2.0f);

	camPos=D3DXFROMWINEVECTOR3(0.0f, 0.8f, 0.0f);
	fogColor = D3DXFROMWINEVECTOR4(1.0f, 0.5f, 0.1f, 1.0f);
	std::string res="/";	
	unsigned int j=0;

#if !defined(CUBEMAPS)	
	//while(1)
	{
		//LOG_FNLN;		
		//if(j==elementsOf(filename))
			//break;
		
		env = IRenderer::GetRendererInstance()->addCubemap(filename[j], ext[j]);

		//LOG_FNLN;
		//j++;
	}
#else
	char *fileName=new char[128];
	//while(1)
	{		
		//if(filename[j]==0)
			//break;
	
		stx_snprintf(fileName, 128, "%s%s", res.c_str(), filename[j]);
		env = RendererHLSLCg::addTexture(fileName, true, IRenderer::GetRendererInstance()->Gettrilinear());
		//j++;
	}

#endif	
	skyBox = IRenderer::GetRendererInstance()->addShaderFromFile("/SkyBox/skybox2.hlsl", "main", "main");
	
	FormatDesc skyboxFmt[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	
	skyboxVF = IRenderer::GetRendererInstance()->addVertexFormat(skyboxFmt, elementsOf(skyboxFmt), skyBox);
	return 0;
}

void render( )
{
	float vertices[] = { 0, 2, 0, 0, -2, -1, -1, -2, 1, 1, -2, 1 };
	ushort indices[] = { 0, 1, 2, 3, 0, 1 };

	IRenderer::GetRendererInstance()->setShader(skyBox);
	IRenderer::GetRendererInstance()->setVertexFormat(skyboxVF);

#if defined(CUBEMAPS)
	IRenderer::GetRendererInstance()->setTexture("Env", env, IRenderer::GetRendererInstance()->Gettrilinear());
#else
	IRenderer::GetRendererInstance()->setTexture("Env", env, IRenderer::GetRendererInstance()->GetbilinearClamp());
#endif
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthWrite());
	#if 0
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", I);
	#else
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	#endif
	IRenderer::GetRendererInstance()->setShaderConstant4f("fogColor", fogColor);
	IRenderer::GetRendererInstance()->setShaderConstant2f("scaleBias", D3DXFROMWINEVECTOR2(2 + 0.3f * camPos.y, camPos.y * 1.3f - 1.2f));

	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLE_STRIP, 0, 4, 4, indices, indices, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR3));
}
};
#endif

