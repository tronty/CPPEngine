/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
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

class SkyBox: public STXGUI
{
D3DXFROMWINEVECTOR4 fogColor;
TextureID env[elementsOf(filename)];
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

#if !defined(CUBEMAPS)		
	unsigned int j=0;
	while(1)
	{
		//LOG_FNLN;		
		if(j==elementsOf(filename))
			break;
		
		env[j] = IRenderer::GetRendererInstance()->addCubemap(filename[j], ext[j]);

		//LOG_FNLN;
		j++;
	}
#else
	unsigned int j=0;
	char *fileName=new char[128];
	while(1)
	{		
		if(filename[j]==0)
			break;
	
		stx_snprintf(fileName, 128, "%s%s", res.c_str(), filename[j]);
		env[j] = RendererHLSLCg::addTexture(fileName, true, IRenderer::GetRendererInstance()->Gettrilinear());
		j++;
	}

#endif	
	skyBox = IRenderer::GetRendererInstance()->addShaderFromFile("/SkyBox/skybox2.shd", "main", "main");
	
	FormatDesc skyboxFmt[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	
	skyboxVF = IRenderer::GetRendererInstance()->addVertexFormat(skyboxFmt, elementsOf(skyboxFmt), skyBox);
	
	STXGUI::init(GUILayout);
	return 0;
}

void render( )
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMATRIX matRot;
		D3DXFROMWINEMatrixRotationYawPitchRoll( &matRot,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );

	float vertices[] = { 0, 2, 0, 0, -2, -1, -1, -2, 1, 1, -2, 1 };
	ushort indices[] = { 0, 1, 2, 3, 0, 1 };

	IRenderer::GetRendererInstance()->setShader(skyBox);
	IRenderer::GetRendererInstance()->setVertexFormat(skyboxVF);

#if defined(CUBEMAPS)
	IRenderer::GetRendererInstance()->setTexture("Env", env[m_i], IRenderer::GetRendererInstance()->Gettrilinear());
#else
	IRenderer::GetRendererInstance()->setTexture("Env", env[m_i], IRenderer::GetRendererInstance()->GetbilinearClamp());
#endif
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthWrite());
	#if 0
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", I);
	#else
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", matRot*mvp);
	#endif
	IRenderer::GetRendererInstance()->setShaderConstant4f("fogColor", fogColor);
	IRenderer::GetRendererInstance()->setShaderConstant2f("scaleBias", D3DXFROMWINEVECTOR2(2 + 0.3f * camPos.y, camPos.y * 1.3f - 1.2f));

	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLE_STRIP, 0, 4, 4, indices, indices, CONSTANT_INDEX2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR3));
	
	STXGUI::update();

		const char* txt = "Use mouse buttons to rotate the skybox.";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 10, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		txt = "CTRL-r resets the scene.";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 30, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		char txt2[128];
		stx_snprintf(txt2, 128, "m_i=%d\n", m_i);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 50, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
    GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
    if(radioButton->isPressed() && radioButton->isChecked())
    {
	unsigned int i=0;
	m_i = 0;
	while(1)
	{
		if(filename_[i]==0)
			break;
		
      		if(callbackString == filename_[i])
      		{
			m_i = i;
			break;
		}
		i++;
		}
	}
	}
}
};
int ApplicationLogic()
{	
	//LOG_PRINT("sizeof(ushort)=%d\n", sizeof(ushort));
	IRenderer* r=IRenderer::GetRendererInstance("SkyBox2");		
	IInput*    i=STX_Service::GetInputInstance();
	SkyBox sb;
	sb.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		sb.render();
	}
	return 0;
}

