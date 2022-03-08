/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <GUI/GUIUtils.h>

#include <STX/PerlinNoise.h>

const char* filename[] =
{
"/new/R2VB_VTF/R2VB-Sort/SmokeAnimation.dds",
"/new/R2VB_VTF/R2VB-Sort/FireNoise.dds",
0
};

// The number of passes for sorting is log(COUNT) * (log(COUNT) + 1) / 2.
#define SIZE_X 64
#define SIZE_Y 64
#define COUNT (SIZE_X * SIZE_Y)
float tc[5*4*COUNT];

ShaderID shd=-1;
ShaderID shd31=-1;
ShaderID shd32=-1;
TextureID SmokeAnimation=-1;
//TextureID noise=-1;
//TextureID noise_=-1;
TextureID firePalette=-1;
TextureID texture=-1;
VertexFormatID vf=-1;
VertexFormatID vf31=-1;
VertexFormatID vf32=-1;
VertexBufferID texCoords=-1;

class DDSAnimation : public STXGUI {
public:
	float time;
	__DWORD__  prev, curr;
	D3DXFROMWINEMATRIX mvp;
	unsigned int i,j;
	MeshRenderer2 g_pMesh[2];
	DDSAnimation()
	{
		time=0.0f;
		curr=timeGetTime();
		i=1;
		j=0;
	}
	virtual ~DDSAnimation(){}
	virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
	GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
	if(radioButton->isPressed() && radioButton->isChecked())
	{
		if(callbackString == "SmokeAnimation") 
			i=0;
		else if(callbackString == "FireNoise") 
			i=1;
		else if(callbackString == "Sphere") 
			j=0;
		else if(callbackString == "Box") 
			j=1;
	}
  }
}
	virtual void init(const char* aTitle="")
	{
#if 1
#if 1
	firePalette = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB-Sort/FirePalette.png", true, IRenderer::GetRendererInstance()->Getlinear());
	SmokeAnimation=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB-Sort/SmokeAnimation.dds", true, IRenderer::GetRendererInstance()->Getlinear());
#elif 0
	noise = IRenderer::GetRendererInstance()->addTexture(&img, true, IRenderer::GetRendererInstance()->Getlinear());
	firePalette = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB-Sort/FirePalette.png", true, IRenderer::GetRendererInstance()->Getlinear());
	SmokeAnimation=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB-Sort/SmokeAnimation.dds", true, IRenderer::GetRendererInstance()->Gettrilinear());
#else
	noise = IRenderer::GetRendererInstance()->addTexture(&img, true, IRenderer::GetRendererInstance()->Getlinear());
	firePalette = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB-Sort/FirePalette.png", true, IRenderer::GetRendererInstance()->GetlinearClamp());
	SmokeAnimation=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB-Sort/SmokeAnimation.dds", true, IRenderer::GetRendererInstance()->GettrilinearClamp());
#endif
#else
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", true, ss);
#endif
#if 0
		shd = IRenderer::GetRendererInstance()->addShaderFromFile("/DDSAnimation/DDSAnimation.hlsl", "main", "main");
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
		};
		vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
#endif
		shd31 = IRenderer::GetRendererInstance()->addShaderFromFile("/DDSAnimation/DDSAnimation.hlsl", "main3", "main1");
		FormatDesc format3[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
			0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
			0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
		};
		vf31 = IRenderer::GetRendererInstance()->addVertexFormat(format3, elementsOf(format3), shd31);

		shd32 = IRenderer::GetRendererInstance()->addShaderFromFile("/DDSAnimation/DDSAnimation.hlsl", "main3", "main2");
		vf32 = IRenderer::GetRendererInstance()->addVertexFormat(format3, elementsOf(format3), shd32);
		shd=shd32;
		vf=vf32;
		//noise_=noise;
	#if 1
	LOG_FNLN;
	LOG_PRINT("shd31:%x\n", shd31);
	LOG_PRINT("vf31:%x\n", vf31);
	LOG_PRINT("shd32:%x\n", shd32);
	LOG_PRINT("vf32:%x\n", vf32);
	#endif
	g_pMesh[0].CreateSphere(1.0f, eShaderNone);
	g_pMesh[1].CreateTexturedBox(1.0f, 1.0f, 1.0f, eShaderNone);
	

	// Base texture coordinates for the particles
	for (int i = 0; i < COUNT; i++){
		tc[5*4 * i + 0] = 0;
		tc[5*4 * i + 1] = 0;
		tc[5*4 * i + 2] = 0;
		tc[5*4 * i + 3] = 0;
		tc[5*4 * i + 4] = 0;
		tc[5*4 * i + 5] = 1;
		tc[5*4 * i + 6] = 0;
		tc[5*4 * i + 7] = 0;
		tc[5*4 * i + 8] = 1;
		tc[5*4 * i + 9] = 0;
		tc[5*4 * i + 10] = 1;
		tc[5*4 * i + 11] = 1;
		tc[5*4 * i + 12] = 0;
		tc[5*4 * i + 13] = 1;
		tc[5*4 * i + 14] = 1;
		tc[5*4 * i + 15] = 0;
		tc[5*4 * i + 16] = 1;
		tc[5*4 * i + 17] = 0;
		tc[5*4 * i + 18] = 0;
		tc[5*4 * i + 19] = 1;
	}
	STXGUI::init("/DDSAnimation/GUILayout.xml");
	}

	void setShaderConstants( D3DXFROMWINEMATRIX aM)
	{
	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", aM);
#if 0
	IRenderer::GetRendererInstance()->setTexture("Base", texture);
#else
	IRenderer::GetRendererInstance()->setTexture("FirePalette", firePalette);
	IRenderer::GetRendererInstance()->setShaderConstant1f("time", time);
#endif
	}
	virtual void render()
	{
	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMatrixRotationYawPitchRoll( &mvp,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );

	IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

#if 0
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	setShaderConstants();
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 4*COUNT-2, &tc[0], &tc[0], 5*sizeof(float));
#else
	

	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1))
	{
		shd=shd31;
		vf=vf31;
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2))
	{
		shd=shd32;
		vf=vf32;
	}
/*
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F3))
	{
		noise_=noise;
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F4))
	{
		noise_=SmokeAnimation;
	}
*/
	LOG_FNLN;
	LOG_PRINT("x:shd=%x\n", shd);
	LOG_PRINT("x:vf=%x\n", vf);
	g_pMesh[j].BeginDraw(&mvp, -1, shd, vf);
	setShaderConstants(mvp);
	//IRenderer::GetRendererInstance()->setTexture("Noise", noise_);
	g_pMesh[j].EndDraw();
#endif

	STXGUI::update();
		const char* txt = "F1-F4 key";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 10, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present();

	prev = curr;
	curr=timeGetTime();
	time+=0.001f*(curr-prev);
	}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("DDSAnimation");
	IInput*    i=STX_Service::GetInputInstance();
	DDSAnimation app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();	
		app.render();
	}
	return 0;
}

