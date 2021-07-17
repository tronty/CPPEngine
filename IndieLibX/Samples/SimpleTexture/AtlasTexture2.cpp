/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

const char* filename[] =
{
	//"/Stars/SolarSystemDisks/Sun.jpg",
	"/Stars/SolarSystemDisks/Moon.jpg",
	//"/Stars/SolarSystemDisks/Star.jpg",
	"/Stars/SolarSystemDisks/mercury_mariner10.jpg",
	"/Stars/SolarSystemDisks/venus_galileo.jpg",
	"/Stars/SolarSystemDisks/mars_mgs.jpg",
	"/Stars/SolarSystemDisks/jupiter_cassini.jpg",
	"/Stars/SolarSystemDisks/saturn_cassini.jpg",
	"/Stars/SolarSystemDisks/uranus_voyager.jpg",
	"/Stars/SolarSystemDisks/neptune_voyager.jpg",
	"/Stars/SolarSystemDisks/pluto_HST.jpg",
};

ShaderID m_pShader=-1;
TextureID m_pDecl=-1;
ShaderID shd=-1;
TextureID texID=-1;
VertexFormatID vf=-1;

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.shd", "mainVS", "mainPS");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);

        SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	if(!stx_fileExists("/atlas2.png"))
	{
	std::vector<std::string> av;
	for(unsigned int i=0;i<elementsOf(filename);i++)
		av.push_back(filename[i]);

	Image3 atlas;
	stx_CreateAtlas(atlas, av);
	
	texID=IRenderer::GetRendererInstance()->addTexture(&atlas, false, ss, 0);
	if(0)
	{
		std::string fn=stx_convertpath("/atlas2.png");
		//atlas.compress();
		atlas.saveImageLibImage(fn.c_str(), false);
	}
	}
	else
		texID=IRenderer::GetRendererInstance()->addImageLibTexture("/atlas2.png", false, ss);

	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setTexture("Base", texID);
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	//		   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("AtlasTexture2");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

