/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

const char* filename[] =
{
"/triangle.x",
"/turtle.x",
"/ColoredCube.x",
"/TexturedCube.x",
"/GLEssentials/Assets/demon.x",
"/ProceduralBuilding.x"
};

class _3DAnimation : public STXGUI {
public:
	_3DAnimation()
	{
		m_i=0;
	}
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
		//LOG_PRINT("callbackString=%s\n", callbackString.data);
		if(callbackString == "triangle") 
		{
			m_i=0;
		}
		else if(callbackString == "turtle") 
		{
			m_i=1;
		}
		else if(callbackString == "ColoredCube") 
		{
			m_i=2;
		}
		else if(callbackString == "TexturedCube") 
		{
			m_i=3;
		}
		else if(callbackString == "demon") 
		{
			m_i=4;
		}
		else if(callbackString == "ProceduralBuilding") 
		{
			m_i=5;
		}
	}
	}
}
	virtual void init(const char* aTitle="")
	{		
		STXGUI::init("/3DAnimation/GUILayout3.xml");
		m_i=0;
		for(unsigned int i=0;i<elementsOf(filename);i++)
		{
			tShader aShader=eSimpleShader;
			if(i==2)
				aShader=eSimpleShader2;
#if 0
			FW3::GetInstance()->GetMesh(m_Mesh[i], filename[i], aShader);
#else
			m_Mesh[i].Load(filename[i], aShader);
#endif
			if(1)
			{
				m_Mesh[i].SetPlay(false);
				m_Mesh[i].SetAnimIndex(0);
				m_Mesh[i].SetPlay(true);
				#if 0
				if(shaders[i]!=eSimpleShader)
				{
					m_Mesh[i].SetAmbientColor(D3DXFROMWINEVECTOR4(0.1f, 0.0f, 0.0f, 1.0f));
					m_Mesh[i].SetDiffuseColor(D3DXFROMWINEVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));
					m_Mesh[i].SetLightDir(D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
					m_Mesh[i].SetEyeDir(D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));
				}
				#endif
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
	stx_GetAngles(mAngleX, mAngleY);
	D3DXFROMWINEMATRIX mvp;

		D3DXFROMWINEMatrixRotationYawPitchRoll( &mvp,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
	if(m_Mesh[m_i])
		m_Mesh[m_i].render(mvp);
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

		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present();
	}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("3DAnimation3");		
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

