/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

class _Shape3D: public STXGUI
{
	TextureID texture[11];
	ShaderID shaders[4];
	VertexFormatID vfs[4];
	MeshRenderer2 shape3D;
	unsigned int iI;
	unsigned int s_i;
	D3DXFROMWINEMATRIX I;
public:
_Shape3D(){}
virtual ~_Shape3D(){}
int init(const char* aTitle)
{
	D3DXFROMWINEMatrixIdentity(&I);
	iI=6;
	s_i=2;
	texture[0]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/explosion.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[1]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/fire_gradient3.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[2]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FireGrade.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[3]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/grad3.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[4]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FireTransferFunction.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[5]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FirePalette.png",false, IRenderer::GetRendererInstance()->Getlinear());
	//texture[6]=IRenderer::GetRendererInstance()->addImageLibTexture("/12_Explosion_Particle_Effect/explosion.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[6]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/Spectrum.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[7]=IRenderer::GetRendererInstance()->addImageLibTexture("/PerlinNoiseGPU/grasscolorscale.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[8]=IRenderer::GetRendererInstance()->addImageLibTexture("/PerlinNoiseGPU/sandcolorscale.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[9]=IRenderer::GetRendererInstance()->addImageLibTexture("/PerlinNoiseGPU/rockscolorscale.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[10]=IRenderer::GetRendererInstance()->addImageLibTexture("/PerlinNoiseGPU/snowcolorscale.png",false, IRenderer::GetRendererInstance()->Getlinear());


        shape3D.CreateSphere(1.0f, eShaderNone);

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
	{
	unsigned int i=0;
	shaders[i]=IRenderer::GetRendererInstance()->addShaderFromFile("/VertexDisplacement/hlsl1.shd", "main", "main");
	vfs[i] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[i]);
	}
	if(1)
	{
	unsigned int i=1;
	shaders[i]=IRenderer::GetRendererInstance()->addShaderFromFile("/VertexDisplacement/hlsl2.shd", "main", "main");
	vfs[i] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[i]);
	}
	if(1)
	{
	unsigned int i=2;
	shaders[i]=IRenderer::GetRendererInstance()->addShaderFromFile("/VertexDisplacement/hlsl3.shd", "main", "main");
	vfs[i] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[i]);
	}
	if(1)
	{
	unsigned int i=3;
	shaders[i]=IRenderer::GetRendererInstance()->addShaderFromFile("/vnoise/vnoise.shd", "main", "main");
	vfs[i] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[i]);
	}
	return 0;
}

void render( )
{
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1) && (iI>0))
	{
		iI--;
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2))
	{
		iI++;
		if(iI>11)
			iI=0;
	}
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
        shape3D.BeginDraw(&W, -1, shaders[s_i], vfs[s_i], AmbientColor, DiffuseColor, LightDir, EyeDir);

	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
		// Javascript: = .00025 * ( Date.now() - start );
		static float start=timeGetTime();
		float time=.00025 * (timeGetTime() - start );
	if(s_i==2)
	{
		IRenderer::GetRendererInstance()->setTexture("tExplosion", texture[iI]);
		IRenderer::GetRendererInstance()->setShaderConstant1f("time", time);
	}
	else if(s_i==3)
	{
	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMATRIX matRot;
		D3DXFROMWINEMatrixRotationYawPitchRoll( &matRot,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matRot );
	float displacement=1.0; 
    IRenderer::GetRendererInstance()->setShaderConstant1f("Displacement", displacement );
		IRenderer::GetRendererInstance()->setTexture("tColor", texture[iI]);
		IRenderer::GetRendererInstance()->setShaderConstant1f("time", time);
	}
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
		if(callbackString == "Shader1")
      		{
			s_i = 0;
		}
		if(callbackString == "Shader2")
      		{
			s_i = 1;
		}
		if(callbackString == "Shader3")
      		{
			s_i = 2;
		}
		if(callbackString == "Shader4")
      		{
			s_i = 3;
		}

		if(callbackString == "Texture1")
      		{
			iI = 0;
		}
		if(callbackString == "Texture2")
      		{
			iI = 1;
		}
		if(callbackString == "Texture3")
      		{
			iI = 2;
		}
		if(callbackString == "Texture4")
      		{
			iI = 3;
		}
		if(callbackString == "Texture5")
      		{
			iI = 4;
		}
		if(callbackString == "Texture6")
      		{
			iI = 5;
		}
		if(callbackString == "Texture7")
      		{
			iI = 6;
		}
		if(callbackString == "Texture8")
      		{
			iI = 8;
		}
		if(callbackString == "Texture9")
      		{
			iI = 9;
		}
		if(callbackString == "Texture10")
      		{
			iI = 10;
		}
		if(callbackString == "Texture11")
      		{
			iI = 11;
		}
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
	IRenderer* r=IRenderer::GetRendererInstance("VertexDisplacement");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

