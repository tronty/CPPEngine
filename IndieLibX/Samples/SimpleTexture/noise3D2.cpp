/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

class App: public STXGUI
{
	ShaderID shaders[2];
	VertexFormatID vfs[2];
	MeshRenderer2 shape3D[2];
	TextureID texture[7];
	unsigned int n_i;
	unsigned int s_i;
	unsigned int t_i;
public:
App(){}
virtual ~App(){}
int init(const char* aTitle)
{
	n_i=0;
	s_i=1;
	t_i=0;
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_BINORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TANGENT,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2 
	};{
	shaders[0]=IRenderer::GetRendererInstance()->addShaderFromFile("/noise/noise3D2.shd", "main", "main");
	vfs[0] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[0]);}{
	shaders[1]=IRenderer::GetRendererInstance()->addShaderFromFile("/vnoise/vnoise.shd", "main", "main");
	vfs[1] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[1]);}
        shape3D[0].CreateBox(1.0f, 1.0f, 1.0f, eShaderNone);
	shape3D[1].CreateSphere(1.0f, eShaderNone);

	texture[0]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/explosion.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[1]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/fire_gradient3.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[2]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FireGrade.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[3]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/grad3.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[4]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FireTransferFunction.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[5]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FirePalette.png",false, IRenderer::GetRendererInstance()->Getlinear());
	//texture[6]=IRenderer::GetRendererInstance()->addImageLibTexture("/12_Explosion_Particle_Effect/explosion.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[6]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/Spectrum.png",false, IRenderer::GetRendererInstance()->Getlinear());

	STXGUI::init("/noise/GUILayout3D.xml");
	
	return 0;
}

void render()
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


		D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
		D3DXFROMWINEVECTOR4 AmbientColor=silver;
		D3DXFROMWINEVECTOR4 DiffuseColor=0.1f*silver;
		D3DXFROMWINEVECTOR4 LightDir(0.0f, 0.0f, -1.0f, 1.0f);
		D3DXFROMWINEVECTOR4 EyeDir(0.0f, 0.0f, 1.0f, 1.0f);

	//D3DXFROMWINEMatrixIdentity(&matRot);
        shape3D[s_i].BeginDraw(&matRot, -1, shaders[0], vfs[0], AmbientColor, DiffuseColor, LightDir, EyeDir);
	if(0)// ??? (s_i==3)
	{
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matRot );
	float displacement=1.0; 
    IRenderer::GetRendererInstance()->setShaderConstant1f("Displacement", displacement );
	IRenderer::GetRendererInstance()->setTexture("tColor", texture[t_i]);
	}
	else
	{
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matRot);
	IRenderer::GetRendererInstance()->setShaderConstant1f("time", timeGetTime()/1000.0f);
	IRenderer::GetRendererInstance()->setShaderConstant1i("iI", n_i);
	IRenderer::GetRendererInstance()->setTexture("tExplosion", texture[t_i]);
	}
	shape3D[s_i].EndDraw();
	
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
		if(callbackString == "sphere")
      		{
			s_i = 1;
		}
		if(callbackString == "box")
      		{
			s_i = 0;
		}
		if(callbackString == "explosion")
      		{
			t_i = 0;
		}
		if(callbackString == "fireBase")
      		{
			t_i = 1;
		}
		if(callbackString == "FireTransferFunction")
      		{
			t_i = 2;
		}
		if(callbackString == "FirePalette")
      		{
			t_i = 3;
		}
		if(callbackString == "fire_gradient3")
      		{
			t_i = 4;
		}
		if(callbackString == "FireGrade")
      		{
			t_i = 5;
		}
		if(callbackString == "Spectrum")
      		{
			t_i = 6;
		}
		#if 0
         	if(callbackString == "vnoise")
      		{
			 = 3;
		}
		#endif
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
	IRenderer* r=IRenderer::GetRendererInstance("noise3D2");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

