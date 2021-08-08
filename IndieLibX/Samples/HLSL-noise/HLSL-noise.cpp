/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

D3DXFROMWINEMATRIX GetMVP()
{
	D3DXFROMWINEMATRIX matRot;

	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);
		
		D3DXFROMWINEMatrixRotationYawPitchRoll( &matRot,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
#if 0
	D3DXFROMWINEMATRIX mvp = matRot;
#else
	D3DXFROMWINEMATRIX mvp;
	D3DXFROMWINEMatrixIdentity(&mvp);
#endif
	return mvp;
}
//elif ORIGINALHLSLSIMPLEXNOISE==3
int table_size=32;
D3DXFROMWINEVECTOR4 *g =0;
float sfrand()
{
    return (stx_rand() * 2.0f / (float) RAND_MAX) - 1.0f;
}

// build permuation/gradient table for noise
void initVertexNoiseConstants()
{
	int *p = new int[table_size*2+2];		 // permutation table
    g= new D3DXFROMWINEVECTOR4[table_size*2+2];  // gradient table
	int i;

    // initalize random gradients
	for(i=0; i<table_size; i++) {
		p[i] = i;
		g[i][0] = sfrand();
		g[i][1] = sfrand();
		g[i][2] = sfrand();
	D3DXFROMWINEVec4Normalize(&g[i],&g[i]);
	}

	// initialize permutation table (random shuffle)
	for(i=0; i<table_size; i++) {
		int j, t;
		j = (stx_rand() >> 4) % table_size;
		t = p[i];
		p[i] = p[j];
		p[j] = t;

	g[i][3] = (float) p[i];

	// mirror first half of table into second half
	g[i+table_size][0] = g[i][0];
	g[i+table_size][1] = g[i][1];
	g[i+table_size][2] = g[i][2];
	g[i+table_size][3] = g[i][3];
	}
/*
    glUniform4fvARB(permGradTableParam, table_size*2, g[0].v);
    glUniform4fvARB(permGradTableParam + (table_size*2), 1, g[0].v);
    glUniform4fvARB(permGradTableParam + (table_size*2)+1, 1, g[1].v);
*/
    g[table_size*2]=g[0];
    g[table_size*2+1]=g[1];
}

float g_fSpinX=0.0f;
float g_fSpinY=0.0f;

ShaderID g_pEffect[5];

ShaderID g_pEffect_=-1;
VertexFormatID vf_=-1;

VertexFormatID vf[5];

MeshRenderer2 g_pSphereMesh;

void setTechniqueVariables( D3DXFROMWINEMATRIX& I )
{
	//D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("modelViewProjection", I);

	float t=timeGetTime()/1000.0f;
	IRenderer::GetRendererInstance()->setShaderConstant1f( "time", t );

if (g_pEffect_==g_pEffect[1]){
}else if (g_pEffect_==g_pEffect[3]){
    D3DXFROMWINEVECTOR3 noise_scale = D3DXFROMWINEVECTOR3(1.0f,1.0f,1.0f);
    D3DXFROMWINEVECTOR3 noise_translate = D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
    static float displacement = 1.0;
	// build noise transform
    D3DXFROMWINEMATRIX TextureMatrix,scale,trans;
    //D3DXFROMWINEMatrixIdentity(&TextureMatrix);
    D3DXFROMWINEMatrixScaling(&scale,noise_scale[0], noise_scale[1], noise_scale[2]);
    D3DXFROMWINEMatrixTranslation(&trans,noise_translate[0], noise_translate[1], noise_translate[2]);
	TextureMatrix=scale*trans;
	IRenderer::GetRendererInstance()->setShaderConstant4x4f( "TextureMatrix", TextureMatrix );
	IRenderer::GetRendererInstance()->setShaderConstant1f("Displacement",displacement);
}else if (g_pEffect_==g_pEffect[2]){
}
}
#include <GUI/GUIUtils.h>
const char* name[] =
{
"HLSL_noise1",
"HLSL_noise2",
"HLSL_noise3",
"HLSL_noise4",
"HLSL_noise5",
0
};
class App : public STXGUI
{
public:
	App(){}
	virtual ~App(){}
public:

virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
    GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
    if(radioButton->isPressed() && radioButton->isChecked())
    {
		if(callbackString == "HLSL_noise1")
      		{
			g_pEffect_=g_pEffect[0];
			vf_=vf[0];
		}
		else if(callbackString == "HLSL_noise2")
      		{
			g_pEffect_=g_pEffect[1];
			vf_=vf[1];
		}
		else if(callbackString == "HLSL_noise3")
      		{
			g_pEffect_=g_pEffect[2];
			vf_=vf[2];
		}
		else if(callbackString == "HLSL_noise4")
      		{
			g_pEffect_=g_pEffect[3];
			vf_=vf[3];
		}
		else if(callbackString == "HLSL_noise5")
      		{
			g_pEffect_=g_pEffect[4];
			vf_=vf[4];
		}
  	}}
}
int init(const char* aTitle)
{
	HRESULT hr;

	g_pEffect[0] = IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise/HLSL-noise1.shd", "mainVS", "fragmentShaderPNoise");
	g_pEffect[1] = IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise/HLSL-noise2.shd", "mainVS", "fragmentShaderPNoise");
	g_pEffect[2] = IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise/HLSL-noise3.shd", "mainVS", "fragmentShaderPNoise");
	g_pEffect[3] = IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise/HLSL-noise4.shd", "mainVS", "fragmentShaderPNoise");
	g_pEffect[4] = -1;// ??? IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise/HLSL-noise5.shd", "mainVS", "fragmentShaderPNoise"); // fBm ???

	g_pEffect_=g_pEffect[2];
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_BINORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TANGENT,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf[0] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), g_pEffect[0]);
	vf[1] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), g_pEffect[1]);
	vf[2] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), g_pEffect[2]);
	vf[3] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), g_pEffect[3]);
	vf[4] = -1;// ??? IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), g_pEffect[4]);
	vf_=vf[2];

	#if 1
	LOG_FNLN;
	LOG_PRINT("g_pEffect1:%x\n", g_pEffect[0]);
	LOG_PRINT("vf1:%x\n", vf[0]);
	LOG_PRINT("g_pEffect2:%x\n", g_pEffect[1]);
	LOG_PRINT("vf2:%x\n", vf[1]);
	LOG_PRINT("g_pEffect3:%x\n", g_pEffect[2]);
	LOG_PRINT("vf3:%x\n", vf[2]);
	LOG_PRINT("g_pEffect4:%x\n", g_pEffect[3]);
	LOG_PRINT("vf4:%x\n", vf[3]);
	LOG_PRINT("g_pEffect5:%x\n", g_pEffect[4]);
	LOG_PRINT("vf5:%x\n", vf[4]);
	#endif

	g_pSphereMesh.CreateSphere(1.0f, eShaderNone);

	__DWORD__ t1=timeGetTime();
	__DWORD__ t2=timeGetTime();
	while((t2-t1)/1000.0f<10.0f)
	{
		t2=timeGetTime();
	}

	STXGUI::init("/HLSL-noise/GUILayout.xml");
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1)) {g_pEffect_=g_pEffect[0];vf_=vf[0];}
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2)) {g_pEffect_=g_pEffect[1];vf_=vf[1];}
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F3)) {g_pEffect_=g_pEffect[2];vf_=vf[2];}
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F4)) {g_pEffect_=g_pEffect[3];vf_=vf[3];}
	// ??? if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F5)) {g_pEffect_=g_pEffect[4];vf_=vf[4];}

		D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
		D3DXFROMWINEVECTOR4 AmbientColor=silver;
		D3DXFROMWINEVECTOR4 DiffuseColor=0.1f*silver;
	D3DXFROMWINEVECTOR4 LightDir(0.0f, 0.0f, -1.0f, 1.0f);
	D3DXFROMWINEVECTOR4 EyeDir(0.0f, 0.0f, 1.0f, 1.0f);

	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);

	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY, 0.25f);

		D3DXFROMWINEMATRIX matRot;
		D3DXFROMWINEMatrixRotationYawPitchRoll( &matRot,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );

	g_pSphereMesh.BeginDraw(&matRot, -1, g_pEffect_, vf_, AmbientColor, DiffuseColor, LightDir, EyeDir);
	setTechniqueVariables(matRot);
	g_pSphereMesh.EndDraw();

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
		txt = "F1-F12 key";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 50, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
};
int ApplicationLogic()
{
#if 0//def LINUX
	IRenderer* r=IRenderer::GetRendererInstance("HLSL_Noise", eGLGLSL_1_1, eDebugInit);
#else
	IRenderer* r=IRenderer::GetRendererInstance("HLSL_Noise");
#endif
	IInput*    i=STX_Service::GetInputInstance();
	{
	float f=128.0f/256.0f;
	D3DXFROMWINEVECTOR4 col(f, f, f, 1.0f);
	IRenderer::GetRendererInstance()->Clear(true, true, col);
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
	App app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();	
		app.render();
	}
	return 0;
}

