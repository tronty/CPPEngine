/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define defaultshader eSimpleShader
//define alternativeshader eAmbientLighting
//define alternativeshader eDiffuseLighting
#define alternativeshader eSimpleShader
//define alternativeshader ePhongLighting2

ShaderID AmbientShader=-1;
ShaderID DiffuseShader=-1;
ShaderID PhongShader1=-1;
ShaderID PhongShader2=-1;
VertexFormatID AmbientVertexDeclaration=-1;
VertexFormatID DiffuseVertexDeclaration=-1;
VertexFormatID PhongVertexDeclaration1=-1;
VertexFormatID PhongVertexDeclaration2=-1;
unsigned int mShader_=0;

float AngleX = 0.0f;
float AngleY = 0.0f;

const char* filename[] =
{
"/dx9sdk/cube.x",
"/dx9sdk/teapot.x"
};

tShader shaders[] =
{
alternativeshader,
alternativeshader,
};

float angles[] =
{
0.0f, 0.0f,
0.0f, 0.0f,
};

class _3DAnimation : public STXGUI {
public:
	_3DAnimation(){}
	virtual ~_3DAnimation(){}
	unsigned int m_i;
	MeshRenderer2 m_Mesh[elementsOf(filename)];

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
		////LOG_PRINT("callbackString=%s\n", callbackString.data);
		if(callbackString == "cube")
		{
			m_i=0;
		}
		else if(callbackString == "teapot")
		{
			m_i=1;
		}
	}
	}
}
	virtual void init(const char* aTitle="")
	{
//LOG_FNLN;
		STXGUI::init("/3DAnimation/GUILayout6.xml");
		m_i=1;
		//LuaRead();
		for(unsigned int i=0;i<elementsOf(filename);i++)
		{
			LOG_PRINT("3DAnimation2[%d]=%s\n", i, filename[i]);
#if 0
			FW3::GetInstance()->GetMesh(m_Mesh[i], filename[i], shaders[i]);
#else
			if(0)//((i==13) && (i==15))
				m_Mesh[i].Load(filename[i], shaders[i], false, true);
			else
				m_Mesh[i].Load(filename[i], shaders[i], true, true);
			CompileShaders();
#endif
			if(1)
			{
				m_Mesh[i].SetPlay(false);
				m_Mesh[i].SetAnimIndex(0);
				m_Mesh[i].SetPlay(true);
				{
					D3DXFROMWINEVECTOR4 a(0.1f, 0.0f, 0.0f, 1.0f);
					D3DXFROMWINEVECTOR4 d(1.0f, 0.0f, 0.0f, 1.0f);
					D3DXFROMWINEVECTOR4 l(1.0f, 0.0f, 0.0f, 1.0f);
					D3DXFROMWINEVECTOR4 e(0.0f, 0.0f, -1.0f, 1.0f);
					D3DXFROMWINEVECTOR4 gold=D3DXFROMWINEVECTOR4(133.0f, 109.0f, 74.0f, 255.0f)/255.0f;
					D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
					m_Mesh[i].SetAmbientColor(silver*0.1f);
					m_Mesh[i].SetDiffuseColor(silver);
					m_Mesh[i].SetLightDir(l);
					m_Mesh[i].SetEyeDir(e);
				}
			}
		}
		//m_Mesh[elementsOf(filename)-1]=m_Mesh[elementsOf(filename)-2];
	}
	void CompileShaders()
	{
		//LOG_FNLN;
		FormatDesc format[] = 	{
						0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
						0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
						0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
						0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
					};
		//switch(mShader)
		{
			//case eAmbientLighting:
				if(AmbientShader==-1)
				{
				//LOG_PRINT("eAmbientLighting:\n");
					AmbientShader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer2/AmbientLighting.HLSL.shd", "main", "main");
					AmbientVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), AmbientShader);
				}
				//break;
			//case eDiffuseLighting:
				if(DiffuseShader==-1)
				{
				//LOG_PRINT("eDiffuseLighting:\n");
					DiffuseShader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer2/DiffuseLighting.HLSL.shd", "main", "main");
					DiffuseVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), DiffuseShader);
				}
				//break;
			//case ePhongLighting1:
				if(PhongShader1==-1)
				{
				//LOG_PRINT("ePhongLighting1:\n");
					////{////LOG_FNLN;}
					#if 0//def LINUX
					PhongShader1 = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer2/PhongLighting1.GLSL.shd", "main", "main");
					#else
					PhongShader1 = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer2/PhongLighting1.HLSL.shd", "main", "main");
					#endif
					PhongVertexDeclaration1 = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), PhongShader1);
				}
				//break;
			//case ePhongLighting2:
				if(PhongShader2==-1)
				{
				//LOG_PRINT("ePhongLighting2:\n");
					////{////LOG_FNLN;}
					#if 0//def LINUX
					PhongShader2 = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer2/PhongLighting2.GLSL.shd", "main", "main");
					#else
					PhongShader2 = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer2/PhongLighting2.HLSL.shd", "main", "main");
					#endif
					PhongVertexDeclaration2 = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), PhongShader2);
				}
				//break;
	}
//LOG_FNLN;
	return 0;
}
	void SetShaderConstants()
	{
//LOG_FNLN;
	float mAngleX=0.0f;
	float mAngleY=0.0f;
	static unsigned int mShader=0;

	stx_GetAngles(mAngleX, mAngleY, 0.5f);

    AngleX = mAngleX+angles[2*m_i+0];
    AngleY = mAngleY+angles[2*m_i+1];
		D3DXFROMWINEMatrixRotationYawPitchRoll( &w,
		                            D3DXFROMWINEToRadian(AngleX),
		                            D3DXFROMWINEToRadian(AngleY),
		                            0.0f );
					D3DXFROMWINEVECTOR4 AmbientColor(0.1f, 0.0f, 0.0f, 1.0f);
					D3DXFROMWINEVECTOR4 DiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
					D3DXFROMWINEVECTOR4 gold=D3DXFROMWINEVECTOR4(133.0f, 109.0f, 74.0f, 255.0f)/255.0f;
					D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
					AmbientColor=silver*0.1f;
					DiffuseColor=silver;
					LightDir=D3DXFROMWINEVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
					EyeDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, -1.0f, 1.0f);

	//static unsigned int mShader=0;
	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_F1))
	{
		mShader++;
		if(mShader>3)
			mShader=0;
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_F2))
	{
		if(mShader)
			mShader--;
		else
			mShader=3;
	}
	mShader_=mShader;
//LOG_FNLN;
//LOG_PRINT("mShader=%d\n", mShader);
		
			if(mShader==0)
			{
				//{LOG_PRINT("eAmbientLighting:\n");}
				IRenderer::GetRendererInstance()->setShader(AmbientShader);
				IRenderer::GetRendererInstance()->setVertexFormat(AmbientVertexDeclaration);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				////LOG_FNLN;
				//stx_Dump4f(AmbientColor, "AmbientColor");
				IRenderer::GetRendererInstance()->setShaderConstant4f("AmbientColor", AmbientColor);
			}
			else if(mShader==1)
			{
				//{LOG_PRINT("eDiffuseLighting:\n");}
				IRenderer::GetRendererInstance()->setShader(DiffuseShader);
				IRenderer::GetRendererInstance()->setVertexFormat(DiffuseVertexDeclaration);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("World", w);
				IRenderer::GetRendererInstance()->setShaderConstant4f("AmbientColor", AmbientColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("DiffuseColor", DiffuseColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("vecLightDir", LightDir);
				//IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
			}
			else if(mShader==2)
			{
				//{////LOG_FNLN;}
				//{LOG_PRINT("ePhongLighting1:\n");}
				IRenderer::GetRendererInstance()->setShader(PhongShader1);
				IRenderer::GetRendererInstance()->setVertexFormat(PhongVertexDeclaration1);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("World", w);
				////LOG_FNLN;
				//stx_Dump4f(AmbientColor, "AmbientColor");
				//stx_Dump4f(DiffuseColor, "DiffuseColor");
				IRenderer::GetRendererInstance()->setShaderConstant4f("AmbientColor", AmbientColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("DiffuseColor", DiffuseColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("vecLightDir", LightDir);
				IRenderer::GetRendererInstance()->setShaderConstant4f("vecEye", EyeDir);
				//IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
			}
			else if(mShader==3)
			{
				//{////LOG_FNLN;}
				//{LOG_PRINT("ePhongLighting2:\n");}
				IRenderer::GetRendererInstance()->setShader(PhongShader2);
				IRenderer::GetRendererInstance()->setVertexFormat(PhongVertexDeclaration2);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("World", w);
				////LOG_FNLN;
				//stx_Dump4f(AmbientColor, "AmbientColor");
				//stx_Dump4f(DiffuseColor, "DiffuseColor");
				IRenderer::GetRendererInstance()->setShaderConstant4f("AmbientColor", AmbientColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("DiffuseColor", DiffuseColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("vecLightDir", LightDir);
				IRenderer::GetRendererInstance()->setShaderConstant4f("vecEye", EyeDir);
				//IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
			}
	
	//IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
		//LOG_FNLN;
	return 0;
}
	virtual void render(void)
	{
		IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
		IRenderer::GetRendererInstance()->BeginScene();
#if 0
		m_Mesh[m_i].render(&mvp);
#else
		//LOG_FNLN;
		m_Mesh[m_i].BeginDraw(0, -1, -1, -1);
		SetShaderConstants();
		m_Mesh[m_i].EndDraw();
#endif
		//LOG_FNLN;
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

		char txt2[128];
		stx_snprintf(txt2, 128, "mShader=%d\n", mShader_);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 50,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		stx_snprintf(txt2, 128, "AngleX=%f\n", AngleX);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 70,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		stx_snprintf(txt2, 128, "AngleY=%f\n", AngleY);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 90,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present();
		//LOG_FNLN;
		//stx_exit(0);
	}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("TestLighting");
	IInput*    i=STX_Service::GetInputInstance();
	_3DAnimation app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}

