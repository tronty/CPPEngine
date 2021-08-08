/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define defaultshader eSimpleShader
//define alternativeshader eAmbientLighting
//define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
#define alternativeshader ePhongLighting2

float AngleX = 0.0f;
float AngleY = 0.0f;

const char* filename[] =
{
"/3DS_OBJ_File_Loader/3DS_File_Loader/face.3DS",
"/3DS_OBJ_File_Loader/OBJ_File_Loader/Foot.obj",
"/blender_3ds/untitled.3ds",
"/blender_3ds/untitled.obj",
"/NatureScene/Water/watermesh.3ds",
"/blender_3ds/cup/cup.obj",
"/blender_3ds/cube.3ds",
"/Smoke/Gargoyle/gargoyle.dae",
"/dx9sdk/cube.x",
"/dx9sdk/sphere2.x",
"/dx9sdk/teapot.x",
//"/Sparkles/cat.x",
#if 0
"/DXJune2010/Dwarf/dwarf.sdkmesh",
"/DXJune2010/trees/tree.sdkmesh",
#endif
};

tShader shaders[] =
{
defaultshader,
defaultshader,
defaultshader,
defaultshader,
defaultshader,
defaultshader,
defaultshader,
defaultshader,
alternativeshader,
defaultshader,
alternativeshader,
};

float angles[] =
{
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
};

class _3DAnimation : public STXGUI {
public:
	_3DAnimation()
	{
		m_i=5;
		shdname = defaultshader;
	}
	virtual ~_3DAnimation(){}
	D3DXFROMWINEVECTOR3 T, R, S;
	D3DXFROMWINEMATRIX Tm, Rm, Sm;
	unsigned int m_i;
	std::string shdname;
	MeshRenderer2 m_Mesh[elementsOf(filename)];
	TextureID texID, texID2;

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
		//LOG_PRINT("callbackString=%s\n", callbackString.data);
		if(callbackString == "face") 
		{
			m_i=0;
		}
                else if(callbackString == "Foot") 
		{
			m_i=1;
		}
                else if(callbackString == "untitled3ds") 
		{
			m_i=2;
		}
                else if(callbackString == "untitledobj") 
		{
			m_i=3;
		}
                else if(callbackString == "watermesh") 
		{
			m_i=4;
		}
                else if(callbackString == "cup") 
		{
			m_i=5;
		}
#if 0
                else if(callbackString == "dwarf") 
		{
			m_i=6;
		}
                else if(callbackString == "tree") 
		{
			m_i=7;
		}
#endif
                else if(callbackString == "cube3ds") 
		{
			m_i=6;
		}
                else if(callbackString == "gargoyle") 
		{
			m_i=7;
		}
                else if(callbackString == "cube") 
		{
			m_i=8;
		}
                else if(callbackString == "sphere") 
		{
			m_i=9;
		}
                else if(callbackString == "teapot") 
		{
			m_i=10;
		}
                else if(callbackString == "cat") 
		{
			m_i=11;
		}
	}
	}
}
	virtual void init(const char* aTitle="")
	{			
		STXGUI::init("/3DAnimation/GUILayout5.xml");
		m_i=5;
		SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
		texID2=IRenderer::GetRendererInstance()->addImageLibTexture("/3DS_OBJ_File_Loader/OBJ_File_Loader/bone.bmp", false, ss);
		for(unsigned int i=0;i<elementsOf(filename);i++)
		{
			if(i==9)
			{
				texID=IRenderer::GetRendererInstance()->addImageLibTexture("/dx9sdk/earth.bmp", false, ss);
				m_Mesh[i].CreateSphere(1.0f, eSimpleShader);
				continue;
			}
			LOG_PRINT("3DAnimation5[%d]=%s\n", i, filename[i]);
			m_Mesh[i].Load(filename[i], shaders[i], true);
			
				if(shaders[i]!=eSimpleShader)
				{
					D3DXFROMWINEVECTOR4 a(0.1f, 0.0f, 0.0f, 1.0f);
					D3DXFROMWINEVECTOR4 d(1.0f, 0.0f, 0.0f, 1.0f);
					D3DXFROMWINEVECTOR4 l(0.0f, 1.0f, -1.0f, 1.0f);
					D3DXFROMWINEVECTOR4 e(0.0f, 0.0f, -1.0f, 1.0f);
					D3DXFROMWINEVECTOR4 gold=D3DXFROMWINEVECTOR4(133.0f, 109.0f, 74.0f, 255.0f)/255.0f;
					D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
					m_Mesh[i].SetAmbientColor(gold*0.1f);
					m_Mesh[i].SetDiffuseColor(gold);
					m_Mesh[i].SetLightDir(l);
					m_Mesh[i].SetEyeDir(e);
				}
		}
		//m_Mesh[elementsOf(filename)-1]=m_Mesh[elementsOf(filename)-2];
	}
	virtual void render(void)
	{
		IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
		IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=0.0f;
	float mAngleY=0.0f;

		D3DXFROMWINEMATRIX mvp;
		{
	stx_GetAngles(mAngleX, mAngleY, 0.5f);

    AngleX = mAngleX+angles[2*m_i+0];
    AngleY = mAngleY+angles[2*m_i+1];
		D3DXFROMWINEMatrixRotationYawPitchRoll( &mvp,
		                            D3DXFROMWINEToRadian(AngleX),
		                            D3DXFROMWINEToRadian(AngleY),
		                            0.0f );
		}
	if(1)
	{
#if 1	
		if(m_i==9)
			m_Mesh[m_i].Draw(&mvp, texID);
		else if(m_i==1)
			m_Mesh[m_i].Draw(&mvp, texID2);
		else
			m_Mesh[m_i].render(&mvp);	
#else
	m_Mesh[m_i].BeginDraw(&mvp);//, -1, g_pCurrentTechnique,g_pCurrentLayout);

    IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProj", mWorldViewProj );
    IRenderer::GetRendererInstance()->setShaderConstant4f("EyePos", vEye );
    IRenderer::GetRendererInstance()->setShaderConstant1f("LightIntensity", rnd );

	m_Mesh[m_i].EndDraw();
#endif
	}
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
		stx_snprintf(txt2, 128, "m_i=%d\n", m_i);
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
	}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("3DAnimation5");		
	IInput*    i=STX_Service::GetInputInstance();
	{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

		const char* txt = "Initializing. Please wait a moment.";
		IRenderer::GetRendererInstance()->drawText(txt, 250, 250, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
	}
	_3DAnimation app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}

