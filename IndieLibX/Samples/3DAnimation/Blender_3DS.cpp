/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

const char* filename[] =
{
#if 1
"/blender_3ds/flat.x",
"/blender_3ds/untitled.x",
#else
"/blender_3ds/house_01/untitled.3ds",
"/blender_3ds/house_01/untitled.obj",
#endif
};

class _3DAnimation /*: public STXGUI*/ {
public:
	_3DAnimation(){}
	virtual ~_3DAnimation(){}
	MeshRenderer2 m_Mesh;
	static const unsigned int N=2;
#if 0
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
#endif
	virtual void init(const char* aTitle="")
	{
#if 0
		STXGUI::init("/skeleton/GUILayout.xml");
		FW3::GetInstance()->GetMesh(m_Mesh, filename[N], eSimpleShader);
#else
		m_Mesh.Load(filename[N], eSimpleShader, true, true);
#endif

		m_Mesh.SetPlay(false);
#if 0
		m_Mesh.SetAnimIndex(0);
		m_Mesh.SetPlay(true);
		size_t n=m_Mesh.GetNumAnimations();
		LOG_PRINT("NumAnimations=%d\n", n);
#endif
	}
	virtual void render(void)
	{

	IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	D3DXFROMWINEMATRIX w;
#if 0
	D3DXFROMWINEMatrixIdentity(&w);
#else
	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY, 0.25f);

		D3DXFROMWINEMatrixRotationYawPitchRoll( &w,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
#endif
	m_Mesh.render(&w);
#if 0
	STXGUI::update();
#endif

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
	IRenderer* r=IRenderer::GetRendererInstance("Blender_3DS");		
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

