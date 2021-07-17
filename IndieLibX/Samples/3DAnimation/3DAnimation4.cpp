/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define defaultshader eSimpleShader
//define alternativeshader eAmbientLighting
#define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
//define alternativeshader ePhongLighting2

float AngleX = 0.0f;
float AngleY = 0.0f;

const char* filename[] =
{
"/dx9sdk/tiger.x",
"/DXJune2010/Dwarf/Dwarf.x",
//"/DXJune2010/Dwarf/DwarfWithEffectInstance.x",
};

tShader shaders[] =
{
defaultshader,
defaultshader,
defaultshader
};

float angles[] =
{
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f
};

class _3DAnimation : public STXGUI {
public:
	_3DAnimation()
	{
		m_i=0;
		shdname = defaultshader;
	}
	virtual ~_3DAnimation(){}
	D3DXFROMWINEVECTOR3 T, R, S;
	D3DXFROMWINEMATRIX Tm, Rm, Sm;
	unsigned int m_i;
	std::string shdname;
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
		//LOG_PRINT("callbackString=%s\n", callbackString.data);
		if(callbackString == "dino_videogame") 
		{
			m_i=0;
		}
		else if(callbackString == "dino_videogame2") 
		{
			m_i=1;
		}
		else if(callbackString == "skeleton") 
		{
			m_i=2;
		}
		else if(callbackString == "test") 
		{
			m_i=3;
		}
		else if(callbackString == "dwarf") 
		{
			m_i=4;
		}
		else if(callbackString == "backdrop") 
		{
			m_i=5;
		}
		else if(callbackString == "chopper") 
		{
			m_i=6;
		}
		else if(callbackString == "Rotor") 
		{
			m_i=7;
		}
		else if(callbackString == "Shadow") 
		{
			m_i=8;
		}
		else if(callbackString == "cube") 
		{
			m_i=9;
		}
		else if(callbackString == "sphere") 
		{
			m_i=10;
		}
		else if(callbackString == "sphere2") 
		{
			m_i=11;
		}
		else if(callbackString == "teapot") 
		{
			m_i=12;
		}
		else if(callbackString == "tiger") 
		{
			m_i=13;
		}
		else if(callbackString == "tiny") 
		{
			m_i=14;
		}
		else if(callbackString == "Dwarf") 
		{
			m_i=15;
		}
		else if(callbackString == "Orcs") 
		{
			m_i=16;
		}
		else if(callbackString == "demon") 
		{
			m_i=17;
		}
		else if(callbackString == "castle2") 
		{
			m_i=18;
		}
		else if(callbackString == "watermesh") 
		{
			m_i=19;
		}
		else if(callbackString == "gargoyle") 
		{
			m_i=20;
		} /*
		else if(callbackString == "untitled") 
		{
			m_i=21;
		}
		else if(callbackString == "flat") 
		{
			m_i=22;
		} */
		else if(callbackString == "blender_cube") 
		{
			m_i=21;
		}
		else if(callbackString == "blender_cup") 
		{
			m_i=22;
		}
		else if(callbackString == "blender_demon") 
		{
			m_i=23;
		}
	}
	}
}
void LuaRead()
{
#if 0
		LuaScript script;
		script.Load("/Orcs/Bonfire_wOrcs.lua");
		T.x = script.get<float>("x");
		T.y = script.get<float>("y");
		T.z = script.get<float>("z");
		S.x = script.get<float>("sx");
		S.y = script.get<float>("sy");
		S.z = script.get<float>("sz");
		R.x = script.get<float>("rx");
		R.y = script.get<float>("ry");
		R.z = script.get<float>("rz");
#else
	std::string fn=stx_convertpath("/Orcs/Bonfire_wOrcs.xml");
	LOG_PRINT("fn.c_str()=%s\n", fn.c_str());

	TiXmlDocument document(fn.c_str());
	document.LoadFile();
	TiXmlHandle docHandle( &document );
	TiXmlHandle BodyHandle = docHandle.FirstChild("Body");
	TiXmlElement* BodyElement = BodyHandle.Element();
	if(BodyElement) {LOG_PRINT("!BodyElement\n");stx_exit(0);}

	TiXmlElement* CameraElement = BodyHandle.FirstChild("Camera").Element();
		T.x=atof(CameraElement->Attribute("x"));
		T.y=atof(CameraElement->Attribute("y"));
		T.z=atof(CameraElement->Attribute("z"));
		S.x=atof(CameraElement->Attribute("sx"));
		S.y=atof(CameraElement->Attribute("sy"));
		S.z=atof(CameraElement->Attribute("sz"));
		R.x=atof(CameraElement->Attribute("rx"));
		R.y=atof(CameraElement->Attribute("ry"));
		R.z=atof(CameraElement->Attribute("rz"));
#endif

		D3DXFROMWINEMatrixTranslation(&Tm, T.x, T.y, T.z); 
		D3DXFROMWINEMatrixScaling(&Sm, S.x, S.y, S.z);
		D3DXFROMWINEMatrixRotationYawPitchRoll(&Rm, R.x, R.y, R.z); 

}
	virtual void init(const char* aTitle="")
	{			
		STXGUI::init("/3DAnimation/GUILayout.xml");
		m_i=0;
		//LuaRead();
		for(unsigned int i=0;i<elementsOf(filename);i++)
		{
			STX_PRINT("3DAnimation2[%d]=%s\n", i, filename[i]);
#if 0
			FW3::GetInstance()->GetMesh(m_Mesh[i], filename[i], shaders[i]);
#else
			m_Mesh[i].Load(filename[i], shaders[i]);
#endif
			if(1)
			{
				m_Mesh[i].SetPlay(false);
				m_Mesh[i].SetAnimIndex(0);
				m_Mesh[i].SetPlay(true);
				if(shaders[i]!=eSimpleShader)
				{
					D3DXFROMWINEVECTOR4 a(0.1f, 0.0f, 0.0f, 1.0f);
					D3DXFROMWINEVECTOR4 d(1.0f, 0.0f, 0.0f, 1.0f);
					D3DXFROMWINEVECTOR4 l(0.0f, 1.0f, 0.0f, 1.0f);
					D3DXFROMWINEVECTOR4 e(0.0f, 0.0f, 1.0f, 1.0f);
					m_Mesh[i].SetAmbientColor(a);
					m_Mesh[i].SetDiffuseColor(d);
					m_Mesh[i].SetLightDir(l);
					m_Mesh[i].SetEyeDir(e);
				}
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
		if(m_i==16)
		{
	stx_GetAngles(mAngleX, mAngleY, 50.0f);
{
    D3DXFROMWINEMATRIX mView, mProj, mWorld, R, S, T;
    float aspectRatio = 1.0f*IRenderer::GetRendererInstance()->GetViewportWidth()/IRenderer::GetRendererInstance()->GetViewportHeight();
    //g_Camera.SetProjParams(D3DX_PI / 3, aspectRatio, 1.0f, 1000.0f);
	//D3DXFROMWINEMatrixPerspectiveLH(&mProj, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight(), 1.0f, 1000.0f);
	D3DXFROMWINEMatrixPerspectiveFovLH(&mProj, D3DXFROMWINE_PI / 3, aspectRatio, 1.0f, 1000.0f);

    // Setup camera
    D3DXFROMWINEVECTOR3 vEye3(0.0f, 0.1f, -15.0f);
    D3DXFROMWINEVECTOR3 vAt(0.0f, 0.1f, 0.0f);
	D3DXFROMWINEVECTOR3 vup(0.0f, 1.0f, 0.0f);
	D3DXFROMWINEMatrixLookAtLH(&mView, &vEye3, &vAt, &vup);

    AngleX = mAngleX+angles[2*m_i+0];
    AngleY = mAngleY+angles[2*m_i+1];
    float s=1.0f;
    D3DXFROMWINEVECTOR3 t(0.0f, 0.0f, -7.5f);
#if 0
#if 0
    LuaScript script;
    script.Load("/Orcs/Bonfire_wOrcs.lua");
    AngleX = script.get<float>("Orcs.aX");
    AngleY = script.get<float>("Orcs.aY");
    s = script.get<float>("Orcs.S");
    t.x = script.get<float>("Orcs.Tx");
    t.y = script.get<float>("Orcs.Ty");
    t.z = script.get<float>("Orcs.Tz");
#else
	TiXmlDocument* document;
	TiXmlHandle* docHandle;
	TiXmlHandle* BodyHandle;
	std::string fn=stx_convertpath("/Orcs/Bonfire_wOrcs.xml");
	LOG_PRINT("fn.c_str()=%s\n", fn.c_str());
	document=new TiXmlDocument(fn.c_str());
	document->LoadFile();
	docHandle=new TiXmlHandle( document );
	BodyHandle = &docHandle->FirstChild("Body");
	TiXmlElement* CameraElement = BodyHandle->FirstChild("Camera").Element();
		t.x=atof(CameraElement->Attribute("x"));
		t.y=atof(CameraElement->Attribute("y"));
		t.z=atof(CameraElement->Attribute("z"));
		s=atof(CameraElement->Attribute("sx"));
		AngleX=atof(CameraElement->Attribute("rx"));
		AngleY=atof(CameraElement->Attribute("ry"));
#endif
#endif
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(AngleX),
		                            D3DXFROMWINEToRadian(AngleY),
		                            0.0f );
		D3DXFROMWINEMatrixScaling( &S, s, s, s); 
		D3DXFROMWINEMatrixTranslation( &T, t.x, t.y, t.z); 
    mvp= R*S*T;
}
		} else {
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
		m_Mesh[m_i].render(mvp);
#else
	m_Mesh[m_i].BeginDraw(mvp);//, -1, g_pCurrentTechnique,g_pCurrentLayout);

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
	IRenderer* r=IRenderer::GetRendererInstance("3DAnimation2");		
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

