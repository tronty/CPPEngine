/*
  Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define defaultshader eSimpleShader
//define alternativeshader eAmbientLighting
#define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
//define alternativeshader ePhongLighting2
//define alternativeshader eper_pixel_fragment_shader
//define alternativeshader eper_pixel_fragment_shader_no_tex.shd
//define alternativeshader eper_pixel_fragment_shader_tex_and_light

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

class NativeLesson : public STXGUI {
public:
	NativeLesson()
	{
		m_i=1;
	}
	virtual ~NativeLesson(){}
	D3DXFROMWINEMATRIX mViewMatrix;
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
int init(const char* aTitle)
{
		STXGUI::init("/3DAnimation/GUILayout6.xml");
		m_i=1;
		for(unsigned int i=0;i<elementsOf(filename);i++)
		{
				m_Mesh[i].Load(filename[i], shaders[i], true, true);
		
				m_Mesh[i].SetPlay(false);
				m_Mesh[i].SetAnimIndex(0);
				m_Mesh[i].SetPlay(true);
					D3DXFROMWINEVECTOR4 e(0.0f, 0.0f, -1.0f, 1.0f);
					D3DXFROMWINEVECTOR4 gold=D3DXFROMWINEVECTOR4(133.0f, 109.0f, 74.0f, 255.0f)/255.0f;
					D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eForestGreen);
					m_Mesh[i].SetAmbientColor(silver*0.1f);
					m_Mesh[i].SetDiffuseColor(silver);
					m_Mesh[i].SetEyeDir(e);
			}
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=0.0f;
	float mAngleY=295.0f;

		D3DXFROMWINEMATRIX mvp;

	stx_GetAngles(mAngleX, mAngleY, 0.5f);

    AngleX = mAngleX+angles[2*m_i+0];
    AngleY = mAngleY+angles[2*m_i+1];
		D3DXFROMWINEMatrixRotationYawPitchRoll( &mvp,
		                            D3DXFROMWINEToRadian(AngleX),
		                            D3DXFROMWINEToRadian(AngleY),
		                            0.0f );

    D3DXFROMWINEMATRIX mLightModelMatrix;
    D3DXFROMWINEVECTOR4 mLightPosInWorldSpace, mLightPosInModelSpace;
	static float angleInRadians = 0.0f;
	angleInRadians += (float)D3DXFROMWINE_PI * 0.005f;
    D3DXFROMWINEMatrixRotationY(&mLightModelMatrix, angleInRadians);
    mLightPosInModelSpace=D3DXFROMWINEVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

    D3DXFROMWINEVec4TransformCoord(&mLightPosInWorldSpace, &mLightPosInModelSpace, &mLightModelMatrix);
					m_Mesh[m_i].SetLightDir(mLightPosInWorldSpace);

		m_Mesh[m_i].render(&mvp);
  		STXGUI::update();

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("NativeLesson");
	IInput*    i=STX_Service::GetInputInstance();
	NativeLesson app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}

