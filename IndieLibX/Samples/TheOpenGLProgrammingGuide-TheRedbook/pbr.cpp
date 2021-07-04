/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

class Teapot : public STXGUI {
public:
	Teapot(){}
	virtual ~Teapot(){}
	MeshRenderer2 teapot;
	float mAngleX;
	float mAngleY;

	virtual void init(const char* aTitle="")
	{
		//STXGUI::init("/3DAnimation/GUILayout.xml");
		teapot.Load("/dx9sdk/teapot.x", ePhysicalBasedLighting, true, true);
	mAngleX=0.0f;
	mAngleY=295.0f;
  float ambient[] =
  {0.0f, 0.0f, 0.0f, 1.0};
  float diffuse[] =
  {1.0f, 1.0f, 1.0f, 1.0};
  float position[] =
  {0.0f, 3.0f, 3.0f, 0.0};

  float lmodel_ambient[] =
  {0.2, 0.2, 0.2, 1.0};
  float local_view[] =
  {0.0};
}

void renderTeapot(std::string name, float ax, float ay, const D3DXFROMWINEVECTOR3& color, float roughness, float metallic)
{
  float x=ax-10.0f;
  float y=ay-10.0f;
  float mat[4];
  //glTranslatef(x, y, 0.0);
  D3DXFROMWINEMATRIX I,mvp,R,S,T;
  float s=10.0f;
  D3DXFROMWINEMatrixTranslation(&T, x/s-s*0.5f, y/s-s*0.5f, 0.0f);
  D3DXFROMWINEMatrixTranslation(&T, x/s, y/s, 0.0f);
  D3DXFROMWINEMatrixScaling(&S, 1.0f/s, 1.0f/s, 1.0f/s);
  D3DXFROMWINEMatrixIdentity(&I);
  //glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
  //glMaterialf(GL_FRONT, GL_SHININESS, shine * 128.0);
#if 0
  D3DXFROMWINEVECTOR4 a(ambr, ambg, ambb, 1.0f);
  D3DXFROMWINEVECTOR4 d(difr, difg, difb, 1.0f);
  teapot.SetAmbientColor(a);
  teapot.SetDiffuseColor(d);
#endif
  D3DXFROMWINEVECTOR4 e(0.0f, 0.0f, -1.0f, 1.0f);

  teapot.SetEyeDir(e);

    D3DXFROMWINEMATRIX mLightModelMatrix;
    D3DXFROMWINEVECTOR4 mLightPosInWorldSpace, mLightPosInModelSpace;
	static float angleInRadians = 0.0f;
	angleInRadians += (float)D3DXFROMWINE_PI * 0.005f;
    D3DXFROMWINEMatrixRotationY(&mLightModelMatrix, angleInRadians);
    mLightPosInModelSpace=D3DXFROMWINEVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

    D3DXFROMWINEVec4TransformCoord(&mLightPosInWorldSpace, &mLightPosInModelSpace, &mLightModelMatrix);
					teapot.SetLightDir(mLightPosInWorldSpace);

	stx_GetAngles(mAngleX, mAngleY, 0.5f);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
  mvp=R*S*T;
#if 0
  teapot.render(&mvp);
#else
  teapot.BeginDraw(&mvp);
  //D3DXFROMWINEVECTOR3 objPos(x/s-s*0.5f, y/s-s*0.5f, 0.0f);
	D3DXFROMWINEVECTOR3 objPos(x/s, y/s, 0.0f);
  IRenderer::GetRendererInstance()->setShaderConstant3f("objPos", objPos);
  IRenderer::GetRendererInstance()->setShaderConstant1f("roughness", roughness);
  IRenderer::GetRendererInstance()->setShaderConstant1f("metallic", metallic);
  IRenderer::GetRendererInstance()->setShaderConstant3f("color", color);
  teapot.EndDraw();
#endif
}

	virtual void render(void)
	{
		IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
		IRenderer::GetRendererInstance()->BeginScene();

		// Setup some default materials (source: https://seblagarde.wordpress.com/2011/08/17/feeding-a-physical-based-lighting-mode/)
		// https://github.com/SaschaWillems/Vulkan.git
		renderTeapot("Gold", 2.0f, 17.0f, D3DXFROMWINEVECTOR3(1.0f, 0.765557f, 0.336057f), 0.1f, 1.0f);
		renderTeapot("Copper", 2.0f, 14.0f, D3DXFROMWINEVECTOR3(0.955008f, 0.637427f, 0.538163f), 0.1f, 1.0f);
		renderTeapot("Chromium", 2.0f, 11.0f, D3DXFROMWINEVECTOR3(0.549585f, 0.556114f, 0.554256f), 0.1f, 1.0f);
		renderTeapot("Nickel", 6.0f, 17.0f, D3DXFROMWINEVECTOR3(0.659777f, 0.608679f, 0.525649f), 0.1f, 1.0f);
		renderTeapot("Titanium", 6.0f, 14.0f, D3DXFROMWINEVECTOR3(0.541931f, 0.496791f, 0.449419f), 0.1f, 1.0f);
		renderTeapot("Cobalt", 6.0f, 11.0f, D3DXFROMWINEVECTOR3(0.662124f, 0.654864f, 0.633732f), 0.1f, 1.0f);
		renderTeapot("Platinum", 6.0f, 8.0f, D3DXFROMWINEVECTOR3(0.672411f, 0.637331f, 0.585456f), 0.1f, 1.0f);
		// Testing materials
		renderTeapot("White", 10.0f, 17.0f, D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f), 0.1f, 1.0f);
		renderTeapot("Red", 10.0f, 14.0f, D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f), 0.1f, 1.0f);
		renderTeapot("Blue", 10.0f, 11.0f, D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f), 0.1f, 1.0f);
		renderTeapot("Black", 10.0f, 18.0f, D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), 0.1f, 1.0f);

  		//STXGUI::update();

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

		stx_snprintf(txt2, 128, "AngleX=%f\n", mAngleX);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 50,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		stx_snprintf(txt2, 128, "AngleY=%f\n", mAngleY);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 70,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present();
	}

	virtual void actionPerformed(GUIEvent &evt)
{
  const std::string &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
	GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
	if(radioButton->isPressed() && radioButton->isChecked())
	{
		#if 0
		if(callbackString == "")
		{
			m_i=;
		}
		#endif
	}
	}
}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Physical Based Rendering");
	IInput*    i=STX_Service::GetInputInstance();
	Teapot app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}

