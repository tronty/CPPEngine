/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#undef _CREATE2SPHERES_

#if 1
#define defaultshader eSimpleShader
//define alternativeshader eAmbientLighting
//define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
#define alternativeshader ePhongLighting2
#else
#define defaultshader eCelShader
#define alternativeshader eCelShader
#endif

float AngleX = 0.0f;
float AngleY = 0.0f;

const char* filename[] =
{
"/trex_dx/dino_videogame.x",
"/trex_dx/dino_videogame2.x",
"/skeleton/skeleton.x",
"/assimp--1.0.412-sdk/test.x",
"/assimp--1.0.412-sdk/dwarf.x",
"/AdvAniCh12/backdrop.x",
"/AdvAniCh12/chopper.X",
"/AdvAniCh12/Rotor.X",
"/AdvAniCh12/Shadow.X",
"/dx9sdk/cube.x",
"/dx9sdk/sphere.x",
"/dx9sdk/sphere2.x",
"/dx9sdk/teapot.x",
"/dx9sdk/tiger.x",
"/dx9sdk/tiny.x",
//"/DXJune2010/Dwarf/Dwarf.x",
//"/DXJune2010/Dwarf/DwarfWithEffectInstance.x",
"/Orcs/Bonfire_wOrcs.x",
//"/GLEssentials/Assets/demon.x",
"/GLEssentials/Assets/demon.x",
"/ProceduralBuilding3_castle2.x",
"/NatureScene/Water/watermesh.3ds",
//"/GLEssentials/Assets/demon.x",
"/Smoke/Gargoyle/gargoyle.dae",
//"/blender_3ds/untitled.x",
//"/blender_3ds/flat.x",
"/blender_3ds/cube.3ds",
"/blender_3ds/cup/cup.obj"
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
defaultshader,
alternativeshader,
defaultshader,
defaultshader,
alternativeshader,
defaultshader,
defaultshader,
//defaultshader,
defaultshader,
defaultshader,
defaultshader,
defaultshader,
defaultshader,
//defaultshader,
//defaultshader,
defaultshader,
defaultshader
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
0.0f, 0.0f,
0.0f, 0.0f,
180.0f, -90.0f,
//0.0f, 0.0f,
18.37f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
0.0f, 0.0f,
//0.0f, 0.0f,
//0.0f, 0.0f,
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
	TextureID texID1, texID2;

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
		#if 0
		else if(callbackString == "Dwarf") 
		{
			m_i=15;
		}
		#endif
		else if(callbackString == "Orcs") 
		{
			m_i=15;
		}
		else if(callbackString == "demon") 
		{
			m_i=16;
		}
		else if(callbackString == "castle2") 
		{
			m_i=17;
		}
		else if(callbackString == "watermesh") 
		{
			m_i=18;
		}
		else if(callbackString == "gargoyle") 
		{
			m_i=19;
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
			m_i=20;
		}
		else if(callbackString == "blender_cup") 
		{
			m_i=21;
		}
		else if(callbackString == "blender_demon") 
		{
			m_i=22;
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
		#ifndef _CREATE2SPHERES_
		{
			SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
			texID1=IRenderer::GetRendererInstance()->addImageLibTexture("/dx9sdk/earth.bmp", false, ss);
			texID2=IRenderer::GetRendererInstance()->addImageLibTexture("/AccurateAtmosphericalScattering/earthmap1k.jpg", false, ss);
		}
		#endif
		for(unsigned int i=0;i<elementsOf(filename);i++)
		{
			LOG_PRINT("3DAnimation2[%d]=%s\n", i, filename[i]);
#if 0
			FW3::GetInstance()->GetMesh(m_Mesh[i], filename[i], shaders[i]);
#else
			if((i==10) || (i==11))
				//m_Mesh[i].Load(filename[i], shaders[i], false, true);
				m_Mesh[i].CreateSphere(1.0f, eSimpleShader);
			else
				m_Mesh[i].Load(filename[i], shaders[i], true, true);
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
		if(0)//(m_i==16)
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
		#ifndef _CREATE2SPHERES_
		if(m_i==10)
			m_Mesh[m_i].Draw(&mvp, texID1);
		else if(m_i==11)
			m_Mesh[m_i].Draw(&mvp, texID2);
		else
		#endif
			m_Mesh[m_i].render(&mvp);
#else
	m_Mesh[m_i].BeginDraw(&mvp);//, -1, g_pCurrentTechnique,g_pCurrentLayout);

    IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProj", mWorldViewProj );
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("World", mWorldViewProj );

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

_3DAnimation app;
int init(const char* aTitle)
{    
	app.init("");
	return 0;
}

void render()
{
		app.render();
}
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
#ifdef _CREATE2SPHERES_
if(0)
{
	MeshRenderer2 m_Mesh;
	m_Mesh.CreateSphere(1.0f, eShaderNone);
	std::vector<stx_VertexPositionNormalTexture> vertices;
	std::vector<__WORD__> indices;

	LOG_PRINT("m_Mesh.vertices.size()=%d\n", m_Mesh.vertices.size());
	LOG_PRINT("m_Mesh.indices.size()=%d\n", m_Mesh.indices.size());

	vertices=m_Mesh.vertices;
	indices=m_Mesh.indices;

	LOG_PRINT("vertices.size()=%d\n", vertices.size());
	LOG_PRINT("indices.size()=%d\n", indices.size());		
	
	std::vector<__WORD__> faces;
	for(unsigned int i=0;i<indices.size();i++)
	{
		if(0==((i%3)))
			faces.push_back(0);
	}
	{
		std::vector<std::string> textureNames;
		textureNames.push_back("/dx9sdk/earth.bmp");
		stx_writeXFile(	"/dx9sdk/sphere3.x", vertices, indices, textureNames, faces);
	}
}
else if(0)
	{
std::vector<stx_Mesh> meshes;
std::vector<stx_Material> vt;
MeshRenderer2 m_Mesh;
m_Mesh.CreateSphere(1.0f, eShaderNone);
std::vector<__WORD__> indices;
for(unsigned int i=0;i<m_Mesh.indices.size();i++)
{
	if(0==((i%3)))
		indices.push_back(0);
}

LOG_PRINT("m_Mesh.vertices.size()=%d\n", m_Mesh.vertices.size());
LOG_PRINT("m_Mesh.indices.size()=%d\n", m_Mesh.indices.size());
LOG_PRINT("indices.size()=%d\n", indices.size());
{
D3DXFROMWINEVECTOR3 vC(0.0f, 1.0f, 0.0f);
stx_Material m("/dx9sdk/earth.bmp", vC);
vt.push_back(m);
stx_Materials_(vt, indices, meshes);
stx_writeXFile("/dx9sdk/sphere3.x", meshes);
}
{
D3DXFROMWINEVECTOR3 vC(0.0f, 1.0f, 0.0f);
stx_Material m("/AccurateAtmosphericalScattering/earthmap1k.jpg", vC);
vt.push_back(m);
stx_Materials_(vt, indices, meshes);
stx_writeXFile("/dx9sdk/sphere4.x", meshes);
}
	}
#endif
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

