/*
  Copyright (c) 2024 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

class _Shape3D: public STXGUI
{
	TextureID texture;
	ShaderID shader;
	VertexFormatID vf;
	MeshRenderer2 shape3D;
	D3DXFROMWINEMATRIX I;
public:
_Shape3D(){}
virtual ~_Shape3D(){}
int init(const char* aTitle)
{
	D3DXFROMWINEMatrixIdentity(&I);
	texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp",false, IRenderer::GetRendererInstance()->Getlinear());

	#if 1
        shape3D.CreateTexturedBox(1.0f, 1.0f, 1.0f, eShaderNone);
        #else
        shape3D.CreateSphere(1.0f, eShaderNone);
        #endif

	STXGUI::init("/MeshRenderer2/GUILayout3.xml");

	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_BINORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TANGENT,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2 
	};
	shader=IRenderer::GetRendererInstance()->addShaderFromFile("/VertexDisplacement/glsl3.glsl", "main", "main");
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shader);
	
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

		D3DXFROMWINEMATRIX W,R,S;
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );


		D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
		D3DXFROMWINEVECTOR4 AmbientColor=silver;
		D3DXFROMWINEVECTOR4 DiffuseColor=0.1f*silver;
		D3DXFROMWINEVECTOR4 LightDir(0.0f, 0.0f, -1.0f, 1.0f);
		D3DXFROMWINEVECTOR4 EyeDir(0.0f, 0.0f, 1.0f, 1.0f);

	//D3DXFROMWINEMatrixIdentity(&R);
	float s=0.25;
	D3DXFROMWINEMatrixScaling(&S, s, s, s);
	W=R*S;
        shape3D.BeginDraw(&W, -1, shader, vf, AmbientColor, DiffuseColor, LightDir, EyeDir);

	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
		// Javascript: = .00025 * ( Date.now() - start );
		static float start=timeGetTime();
		float time=.00025 * (timeGetTime() - start );

		IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texture);
		IRenderer::GetRendererInstance()->setShaderConstant1f("time", time);
	
	shape3D.EndDraw();
	
	STXGUI::update();

		const char* txt = "Use mouse buttons to rotate the model.";
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
	}
	}
}
};
_Shape3D shape3D;
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
	IRenderer* r=IRenderer::GetRendererInstance("VertexDisplacement2");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

