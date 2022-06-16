/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#undef DRAWGLOBE

const char* filename[] =
{
"/skeleton/skeleton.x",
"/skeleton/skeleton.bin.x",
"/DXJune2010/Soldier/soldier.sdkmesh",
"/DXJune2010/MotionBlur/Warrior.sdkmesh",
"/DXJune2010/Dwarf/dwarf.sdkmesh",
"/DXJune2010/trees/tree.sdkmesh",
0
};
unsigned int g_i=0;

class _3DAnimation : public STXGUI {
public:
	_3DAnimation(){}
	virtual ~_3DAnimation(){}
	MeshRenderer2 m_Mesh;
	TextureID texID;
	virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
	GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
	unsigned int i=0;
	if(radioButton->isPressed() && radioButton->isChecked())
	{
		while(i<5)
		{
			char buf[256] = {0};
			stx_snprintf(buf, 256, "Skeleton_%d\0", i+1);
      			if(callbackString == buf)
			{
				m_Mesh.SetPlay(false);
				m_Mesh.SetAnimIndex(i);
				m_Mesh.SetPlay(true);
				break;
			}
			i++;
		}
	}
  }
}
	virtual void init(const char* aTitle="")
	{

		STXGUI::init("/skeleton/GUILayout.xml");
        	texID=-1;
		std::string fn=stx_convertpath("/Framework3/Framework3.xml");
    		TiXmlDocument doc(fn.c_str());
    		if (doc.LoadFile())
		{
			TiXmlHandle docHandle(&doc);
			{
				TiXmlHandle windowsHandle = docHandle.FirstChild("Body").FirstChild("windows");
				TiXmlHandle windowHandle = windowsHandle.FirstChild("window");
				TiXmlElement* windowElement = windowHandle.Element();
				unsigned int i=0;
				while ( windowElement )
				{
					std:string title_=windowHandle.FirstChild("title").Element()->GetText();
					if(title_=="3DAnimation")
					{
						g_i = atoi(windowHandle.FirstChild("g_i").Element()->Attribute("Value"));
						if(g_i>=elementsOf(filename))
							g_i=0;
						printf("g_i=%x\n", g_i);
						break;
					}
					windowHandle = windowsHandle.Child( "window", i++ );
					windowElement = windowHandle.Element();
				}
			}
		}
#ifndef DRAWGLOBE
#if 0
		FW3::GetInstance()->GetMesh(m_Mesh. filename[g_i], eSimpleShader);
#elif 1
		m_Mesh.Load(filename[g_i], eSimpleShader, true, true);
#elif 0
		m_Mesh.Load(filename[g_i], ePhongLighting1, true, true);
#endif
#else
		//m_Mesh.CreateSphere(1.0f, eShaderNone);
		m_Mesh.CreateSphere(1.0f, eSimpleShader);
		SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
		//ss=IRenderer::GetRendererInstance()->GetnearestClamp();
		texID=IRenderer::GetRendererInstance()->addImageLibTexture("/dx9sdk/earth.bmp", false, ss);
		//texID=IRenderer::GetRendererInstance()->addImageLibTexture("/AccurateAtmosphericalScattering/earthmap1k.jpg", true, ss);
		//texID=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", true, ss);
#endif

		m_Mesh.SetPlay(false);
		m_Mesh.SetAnimIndex(0);
		m_Mesh.SetPlay(true);
		size_t n=m_Mesh.GetNumAnimations();
		LOG_PRINT("NumAnimations=%d\n", n);
		LOG_PRINT("m_Mesh.vertices.size()=%d\n", m_Mesh.vertices.size());
		LOG_PRINT("m_Mesh.indices.size()=%d\n", m_Mesh.indices.size());
if(m_Mesh.meshes.size())
{
		LOG_PRINT("m_Mesh.meshes[0].vertices.size()=%d\n", m_Mesh.meshes[0].vertices.size());
		LOG_PRINT("m_Mesh.meshes[0].indices.size()=%d\n", m_Mesh.meshes[0].indices.size());
}
	if(0)
	{
	std::vector<stx_Material> vt;
	std::vector<__WORD__> g_indices2;
	std::vector<stx_Mesh> mesh_;
	stx_Mesh m;
	m.vertices=m_Mesh.meshes[0].vertices;
	m.indices=m_Mesh.meshes[0].indices;

	for(unsigned int i=0;i<m_Mesh.meshes[0].textures.size();i++)
		if(m_Mesh.meshes[0].textureNames[i]=="BONES.BMP")
			for(unsigned int j=0;j<m_Mesh.meshes[0].rindices.size()/3;j++)
				g_indices2.push_back(i);
		else
			for(unsigned int j=0;j<m_Mesh.meshes[0].rindices.size()/3;j++)
				g_indices2.push_back(0);

	mesh_.push_back(m);
	stx_Materials_(vt, g_indices2, mesh_);
	stx_writeXFile("/assimp--1.0.412-sdk/test2.x", mesh_);
	}
	}
	virtual void render(void)
	{

	IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	D3DXFROMWINEMATRIX mvp, R, S, T;
	float mAngleX=0.0f;
	float mAngleY=0.0f;
	if
	(0)
	//(std::string(filename[g_i])=="/Orcs/Bonfire_wOrcs.x")
	{
    		float s=1.0f;
    		D3DXFROMWINEVECTOR3 t(0.0f, 0.0f, -7.5f);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX+18.37f),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
		D3DXFROMWINEMatrixScaling( &S, s, s, s);
		D3DXFROMWINEMatrixTranslation( &T, t.x, t.y, t.z);
    		mvp= R*S*T;
	} else {
		stx_GetAngles(mAngleX, mAngleY, 0.5f);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &mvp,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
		}
#ifndef DRAWGLOBE
	m_Mesh.render(&mvp, texID);
#else
	D3DXFROMWINEVECTOR4 a(0.0f, 0.1f, 0.0f, 1.0f);
	D3DXFROMWINEVECTOR4 d(0.0f, 1.0f, 0.0f, 1.0f);
	D3DXFROMWINEVECTOR4 l(0.0f, 1.0f, -1.0f, 1.0f);
	D3DXFROMWINEVECTOR4 e(0.0f, 0.0f, -1.0f, 1.0f);
	/* D3DXFROMWINEVECTOR4 gold=stx_GetRGBA(eGold);
	D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
	a=silver;
	d=silver; */
        m_Mesh.Draw(&mvp, texID, -1, -1, a, d, l, e);
#endif
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
	IRenderer::GetRendererInstance()->Present();
	}
};
int ApplicationLogic()
{
	//LOG_PRINT("elementsOf(filename)=%d\n", elementsOf(filename));
	IRenderer* r=IRenderer::GetRendererInstance("3DAnimation");
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

