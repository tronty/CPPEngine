/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

class App: public STXGUI
{
public:
	App(){}
	virtual ~App(){}
public:

	ShaderID shd=-1;
	VertexFormatID vf=-1;
	unsigned int t_i;

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/Effect2.shd", "main1", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);

	STXGUI::init("/MeshRenderer2/GUILayout4.xml");
	t_i=1;
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->setShaderConstant1i("iI", t_i);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	//IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

	//		   x	 y     tx    ty
	float v[20] =  {  1.0f,-1.0f, 0.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 5*sizeof(float));
	
	STXGUI::update();

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
		if(callbackString == "Texture1")
      		{
			t_i = 1;
		}
		if(callbackString == "Texture2")
      		{
			t_i = 2;
		}
		if(callbackString == "Texture3")
      		{
			t_i = 3;
		}
		if(callbackString == "Texture4")
      		{
			t_i = 4;
		}
		if(callbackString == "Texture5")
      		{
			t_i = 5;
		}
		if(callbackString == "Texture6")
      		{
			t_i = 6;
		}
		if(callbackString == "Texture7")
      		{
			t_i = 7;
		}
	}
	}
}
};

App app;
int init(const char* aTitle)
{
	app.init(aTitle);
	return 0;
}

void render()
{
	app.render();
	return;
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Effect2");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

