/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define defaultshader eShaderNone
#define alternativeshader eShaderNone
//define alternativeshader eSimpleShader
//define alternativeshader eAmbientLighting
//define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
//define alternativeshader ePhongLighting2

//define NSIZE 64
//define SCALE 0.1482f

class _Shape3D: public STXGUI
{
	TextureID atlasID, texture[7];
	ShaderID shaders[4];
	VertexFormatID vfs[4];
	MeshRenderer2 shape3D[2];
	unsigned int s_i;
	unsigned int m_i;
	unsigned int t_i;
	D3DXFROMWINEMATRIX I;
public:
_Shape3D(){}
virtual ~_Shape3D(){}
int init(const char* aTitle)
{
	SamplerStateID ss2=IRenderer::GetRendererInstance()->Getlinear();
	#ifdef __DrawAtlas__
const char* filename[] =
{
	"/Smoke/fireBase.png",
	"/Smoke/FireTransferFunction.png",
	//"/textures/NVIDIA_Corporation/2d/flame.png",
	//"/textures/NVIDIA_Corporation/1D/fire_gradient3.png",
	//"/textures/NVIDIA_Corporation/1D/FireGrade.png",
	"/new/R2VB_VTF/R2VB-Sort/FirePalette.png",
};
	std::vector<std::string> av;
	for(unsigned int i=0;i<elementsOf(filename);i++)
		av.push_back(filename[i]);

	Image3 atlas;
	stx_CreateAtlas(atlas, av);
	atlasID=IRenderer::GetRendererInstance()->addTexture(&atlas, false, ss2, 0);
	#endif
	D3DXFROMWINEMatrixIdentity(&I);

	s_i = 3;
	m_i=1;
	t_i = 0;
        shape3D[0].CreateBox(1.0f, 1.0f, 1.0f, eShaderNone);
	shape3D[1].CreateSphere(1.0f, eShaderNone);

	STXGUI::init("/MeshRenderer2/GUILayout2.xml");

	texture[0]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/explosion.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[1]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/fire_gradient3.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[2]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FireGrade.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[3]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/grad3.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[4]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FireTransferFunction.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[5]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FirePalette.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[6]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/Spectrum.png",false, IRenderer::GetRendererInstance()->Getlinear());

	//texture[6]=IRenderer::GetRendererInstance()->addImageLibTexture("/12_Explosion_Particle_Effect/explosion.png",false, IRenderer::GetRendererInstance()->Getlinear());
	
	shaders[0]=IRenderer::GetRendererInstance()->addShaderFromFile("/Shape3D/fireball.fx", "RayMarchVS", "RayMarchPS");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_BINORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TANGENT,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2 
	};
	{vfs[0] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[0]);
	}
	shaders[1]=IRenderer::GetRendererInstance()->addShaderFromFile("/Shape3D/vbomb.fx", "mainVS", "hotPS");
	{
	vfs[1] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[1]);
	}
	shaders[2]=IRenderer::GetRendererInstance()->addShaderFromFile("/Shape3D/Flame.fx", "flameVS", "flamePS");
	{
	vfs[2] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[2]);
	}
	shaders[3]=IRenderer::GetRendererInstance()->addShaderFromFile("/vnoise/vnoise.shd", "main", "main");
	vfs[3] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[1]);

	return 0;
}

void render( )
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	#ifdef __DrawAtlas__
	if(1)
		DrawAtlas();
	else
	#endif
	{

	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1))
	{
		s_i=0;
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2))
	{
		s_i=1;
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F3))
	{
		s_i=2;
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F4))
	{
		s_i=3;
	}

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

	D3DXFROMWINEMatrixIdentity(&matRot);
	//LOG_FNLN;
	//LOG_PRINT("x:shaders[%d]=%x\n", s_i, shaders[s_i]);
	//LOG_PRINT("x:vfs[%d]=%x\n", s_i, vfs[s_i]);
        shape3D[m_i].BeginDraw(&matRot, -1, shaders[s_i], vfs[s_i], AmbientColor, DiffuseColor, LightDir, EyeDir);

	float time=timeGetTime()/1000.0f;
	float rnd = (float)stx_rand() / RAND_MAX * 0.5f + 0.5f;
    	D3DXFROMWINEVECTOR4 lightPos( 0.25f * (rnd - 0.5f), 5.7f, 1.0f * (rnd - 0.5f), 1.0f );
	D3DXFROMWINEVECTOR3 vEye(0, 0, 0);
	IRenderer::GetRendererInstance()->setTexture("tExplosion", texture[t_i]);
	if(s_i==0)
	{
		//LOG_PRINT("fireball\n");
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("viewInv", I);
		D3DXFROMWINEVECTOR2 viewport(IRenderer::GetRendererInstance()->GetViewportWidth(),IRenderer::GetRendererInstance()->GetViewportHeight());
		IRenderer::GetRendererInstance()->setShaderConstant2f("viewport", viewport);
		IRenderer::GetRendererInstance()->setShaderConstant1f("time", time);
	}
	else if(s_i==1)
	{
		//LOG_PRINT("vbomb\n");
		/* ???
		D3DXFROMWINEVECTOR4 pg[FULLSIZE];
		IRenderer::GetRendererInstance()->setShaderConstantArray4f("pg", pg, FULLSIZE);
		*/
		IRenderer::GetRendererInstance()->setShaderConstant1f("timer", time);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProj", I);
	}
	else if(s_i==2)
	{
		//LOG_PRINT("flame\n");
		IRenderer::GetRendererInstance()->setShaderConstant1f("ticks", time);
		IRenderer::GetRendererInstance()->setShaderConstant1f("timeScale", 1.0f);//???
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProj", I);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("World", I);
	}
	else if(s_i==3)
	{
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matRot );
	float displacement=1.0; 
    IRenderer::GetRendererInstance()->setShaderConstant1f("Displacement", displacement );
	IRenderer::GetRendererInstance()->setTexture("tColor", texture[t_i]);
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
		txt = "F1-F3 key";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 50, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		char txt2[128];
		stx_snprintf(txt2, 128, "m_i=%d\n", m_i);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 70, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		stx_snprintf(txt2, 128, "s_i=%d\n", s_i);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 90, 
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
		if(callbackString == "FireBall")
      		{
			s_i = 0;
		}
		if(callbackString == "Vbomb")
      		{
			s_i = 1;
		}
		if(callbackString == "Flame")
      		{
			s_i = 2;
		}
         	if(callbackString == "vnoise")
      		{
			s_i = 3;
		}

		#if 0
		if(callbackString == "PerlinFire3D")
      		{
			s_i = 3;
		}
		if(callbackString == "PerlinFire3DFlow")
      		{
			s_i = 4;
		}
 		if(callbackString == "PerlinFire4D")
      		{
			s_i = 5;
		}
		if(callbackString == "ParticlesDeferred")
      		{
			s_i = 6;
		}
		if(callbackString == "Particles")
      		{
			s_i = 7;
		}
		if(callbackString == "CompositeParticles")
      		{
			s_i = 8;
		}
		#endif
         	if(callbackString == "Box")
      		{
			m_i = 0;
		}
         	if(callbackString == "Sphere")
      		{
			m_i = 1;
		}
	}
	}
}

	#ifdef __DrawAtlas__
void DrawAtlas()
{
	static ShaderID shd=-1;
	static VertexFormatID vf=-1;
	if(shd==-1)
	{
		shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.shd", "mainVS", "mainPS");
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
		};
		vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	}
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setTexture("Base", atlasID);
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
}
#endif
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Shape3D3");
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

