/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

class _Shape3D: public STXGUI
{
	TextureID texture[7], env;
	ShaderID shaders[7];
	VertexFormatID vfs[7];
	MeshRenderer2 shape3D[3];
	unsigned int s_i;
	unsigned int m_i;
	unsigned int t_i;
	D3DXFROMWINEMATRIX I, viewInv;
public:
_Shape3D(){}
virtual ~_Shape3D(){}
int init(const char* aTitle)
{
	D3DXFROMWINEMatrixIdentity(&I);

	s_i = 3;
	m_i=2;
	t_i = 0;
        shape3D[0].CreateTexturedQuad(2.0f, 2.0f, eShaderNone);
        shape3D[1].CreateBox(1.0f, 1.0f, 1.0f, eShaderNone);
	shape3D[2].CreateSphere(1.0f, eShaderNone);

	STXGUI::init("/RenderMonkey/GUILayout.xml");

	texture[0]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/explosion.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[1]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/fire_gradient3.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[2]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FireGrade.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[3]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/grad3.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[4]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FireTransferFunction.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[5]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FirePalette.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[6]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/Spectrum.png",false, IRenderer::GetRendererInstance()->Getlinear());
	env = RendererHLSLCg::addTexture("/SkyBox/cubemaps/x/DragonFireGL.dds", true, IRenderer::GetRendererInstance()->Gettrilinear());

	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
		0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2 
	};
	shaders[0]=IRenderer::GetRendererInstance()->addShaderFromFile("/RenderMonkey/BlueSky.shd", "main", "main");
	{
	vfs[0] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[0]);
	}
	#if 0
	shaders[1]=-1;
	vfs[1]=-1;
	shaders[2]=-1;
	vfs[2]=-1;
	#else
	shaders[1]=IRenderer::GetRendererInstance()->addShaderFromFile("/RenderMonkey/Explosion.shd", "main", "main");
	{
	vfs[1] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[1]);
	}
	shaders[2]=IRenderer::GetRendererInstance()->addShaderFromFile("/RenderMonkey/Inferno.shd", "main", "main");
	{
	vfs[2] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[2]);
	}
	#endif
	shaders[3]=IRenderer::GetRendererInstance()->addShaderFromFile("/RenderMonkey/Ocean.shd", "main", "main");
	{
	vfs[3] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[3]);
	}

	shaders[4]=IRenderer::GetRendererInstance()->addShaderFromFile("/RenderMonkey/DarkSky.shd", "main", "main");
	{
	vfs[4] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[4]);
	}

	#if 0
	shaders[5]=-1;
	vfs[5]=-1;
	shaders[6]=-1;
	vfs[6]=-1;
	#else
	shaders[5]=-1;//IRenderer::GetRendererInstance()->addShaderFromFile("/RenderMonkey/FireBall.shd", "main", "main");
	{
	vfs[5] = -1;//IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[5]);
	}
	shaders[6]=IRenderer::GetRendererInstance()->addShaderFromFile("/RenderMonkey/Lava.shd", "main", "main");
	{
	vfs[6] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[6]);
	}
	#endif

	return 0;
}

void render( )
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	{
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
	float d=0;
	D3DXFROMWINEMatrixInverse(&viewInv, &d, &matRot);
        shape3D[m_i].BeginDraw(&matRot, -1, shaders[s_i], vfs[s_i], AmbientColor, DiffuseColor, LightDir, EyeDir);

	float time=timeGetTime()/1000.0f;
	float rnd = (float)stx_rand() / RAND_MAX * 0.5f + 0.5f;
    	D3DXFROMWINEVECTOR4 lightPos( 0.25f * (rnd - 0.5f), 5.7f, 1.0f * (rnd - 0.5f), 1.0f );
	D3DXFROMWINEVECTOR3 vEye(0, 0, 0);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matRot);
	IRenderer::GetRendererInstance()->setShaderConstant1f("time_0_X", time);
	IRenderer::GetRendererInstance()->setTexture("Flame", texture[t_i]);
	if(s_i==3)
	{
		IRenderer::GetRendererInstance()->setTexture("skyBox", env);
	}

	shape3D[m_i].EndDraw();
	
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
	}
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
		if(callbackString == "BlueSkyRM")
      		{
			s_i = 0;
		}

		if(callbackString == "ExplosionRM")
      		{
			s_i = 1;
		}
		if(callbackString == "InfernoRM")
      		{
			s_i = 2;
		}
		if(callbackString == "OceanRM")
      		{
			s_i = 3;
		}
		if(callbackString == "DarkSkyRM")
      		{
			s_i = 4;
		}
		if(callbackString == "FireBallRM")
      		{
			s_i = 5;
		}
		if(callbackString == "LavaRM")
      		{
			s_i = 6;
		}
         	if(callbackString == "Quad")
      		{
			m_i = 0;
		}
         	if(callbackString == "Box")
      		{
			m_i = 1;
		}
         	if(callbackString == "Sphere")
      		{
			m_i = 2;
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
	}
	}
}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("RenderMonkey");
	IInput*    i=STX_Service::GetInputInstance();
	_Shape3D shape3D;
	shape3D.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		shape3D.render();
	}
	return 0;
}

