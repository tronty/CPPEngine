/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

class App: public STXGUI
{
	ShaderID shd=-1;
	VertexFormatID vf=-1;
	TextureID texture=-1;
	unsigned int n_i;
public:
App(){}
virtual ~App(){}
int init(const char* aTitle)
{
	n_i=0;
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/noise/noise2D.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texture=IRenderer::GetRendererInstance()->addImageLibTexture("/VertexDisplacement/explosion.png",false, IRenderer::GetRendererInstance()->Getlinear());

	STXGUI::init("/noise/GUILayout2D.xml");
	
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->setTexture("Base", texture);
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setShaderConstant1f("time", timeGetTime()/1000.0f);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

    //                 	   x     y     tx    ty
	float v[24] =  { -1.0f,-1.0f, 0.0f, 1.0f,
			  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f};
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, 2, &v[0], &v[0], 4*sizeof(float));
	
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
		if(callbackString == "fBm")
      		{
			n_i = 0;
		}
		if(callbackString == "turbulence")
      		{
			n_i = 1;
		}
	}
	}
}
};
App shape3D;
int init(const char* aTitle)
{
	shape3D.init("");
	return 0;
}

void render()
{
	shape3D.render();
}
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("noise2D");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

