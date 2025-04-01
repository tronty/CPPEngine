/*
  Copyright (c) 2018 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

int QUAD=0;
int ORIGINALHLSLSIMPLEXNOISE=3;

MeshRenderer2 shape3D;

ShaderID g_pEffect1=-1;
ShaderID g_pEffect2=-1;
ShaderID g_pEffect3=-1;
ShaderID g_pEffect4=-1;
ShaderID g_pEffect5=-1;

//endif
//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------

//TextureID	g_pTexture	= -1;
ShaderID shd = -1;
VertexFormatID vf = -1;

void init(void);
void render(void);
void shutDown(void);

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------

int init(const char* aTitle)
{
    shape3D.CreateSphere(1.0f, eShaderNone);
	g_pEffect1= -1;//IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise2/HLSL-noise1.fx", "main", "main");
	g_pEffect2= -1;//IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise2/HLSL-noise2.fx", "main", "main");
	g_pEffect3= IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise2/HLSL-noise3.fx", "main", "main");
	g_pEffect4= -1;//IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise2/HLSL-noise4.fx", "main", "main");
	g_pEffect5= -1;//IRenderer::GetRendererInstance()->addShaderFromFile("/HLSL-noise2/HLSL-noise5.fx", "main", "main");
 	
	shd=g_pEffect3;

	__DWORD__ t1=timeGetTime();
	__DWORD__ t2=timeGetTime();
	while((t2-t1)/1000.0f<10.0f)
	{
		t2=timeGetTime();
	}		 

	return 0;
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
    //shd->SetVectorArray("pg",&g[0],table_size*2+2);
}

void setTechniqueVariables(D3DXFROMWINEMATRIX R)
{
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("modelViewProjection", R);

	//LOG_PRINT("ORIGINALHLSLSIMPLEXNOISE=%d\n",ORIGINALHLSLSIMPLEXNOISE);
if (ORIGINALHLSLSIMPLEXNOISE==1){
	//IRenderer::GetRendererInstance()->setShaderConstant4x4f("matWorld", g_matWorld ));
	
}else if (ORIGINALHLSLSIMPLEXNOISE==2){
}else if (ORIGINALHLSLSIMPLEXNOISE==4){
    D3DXFROMWINEVECTOR3 noise_scale = D3DXFROMWINEVECTOR3(1.0f,1.0f,1.0f);
    D3DXFROMWINEVECTOR3 noise_translate = D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
    static float displacement = 1.0;
	// build noise transform
    D3DXFROMWINEMATRIX TextureMatrix,scale,trans;
    //D3DXFROMWINEMatrixIdentity(&TextureMatrix);
    D3DXFROMWINEMatrixScaling(&scale,noise_scale[0], noise_scale[1], noise_scale[2]);
    D3DXFROMWINEMatrixTranslation(&trans,noise_translate[0], noise_translate[1], noise_translate[2]);
	TextureMatrix=scale*trans;
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("TextureMatrix", TextureMatrix );
	IRenderer::GetRendererInstance()->setShaderConstant1f("Displacement",displacement);
	IRenderer::GetRendererInstance()->setShaderConstantArray4f("pg",&g[0],table_size*2+2);
}else if (ORIGINALHLSLSIMPLEXNOISE==3){
}
	float t=timeGetTime()/1000.0f;
	IRenderer::GetRendererInstance()->setShaderConstant1f("time", t );
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	D3DXFROMWINEMATRIX R;
	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );

		D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
		D3DXFROMWINEVECTOR4 AmbientColor=silver;
		D3DXFROMWINEVECTOR4 DiffuseColor=0.1f*silver;
	D3DXFROMWINEVECTOR4 LightDir(0.0f, 0.0f, -1.0f, 1.0f);
	D3DXFROMWINEVECTOR4 EyeDir(0.0f, 0.0f, 1.0f, 1.0f);
	shape3D.BeginDraw(&R, -1, shd, vf, AmbientColor, DiffuseColor, LightDir, EyeDir);

	static int bE=1;
	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_e)) bE^=1;
	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_q)) QUAD^=1;

	static int VE=3;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1))

	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_F1)) {VE=1;shd=g_pEffect1;ORIGINALHLSLSIMPLEXNOISE=1;}
	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_F2)) {VE=2;shd=g_pEffect2;ORIGINALHLSLSIMPLEXNOISE=2;}
	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_F3)) {VE=3;shd=g_pEffect3;ORIGINALHLSLSIMPLEXNOISE=3;}
	//if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_F4)) {VE=4;shd=g_pEffect4;ORIGINALHLSLSIMPLEXNOISE=4;}

	static int VT=6;
	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_1)) VT=1;
	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_2)) VT=2;
	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_3)) VT=3;
	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_4)) VT=4;
	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_5)) VT=5;
	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_6)) VT=6;
	//if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_7)) VT=7;

	setTechniqueVariables(R);
	shape3D.EndDraw();
		const char* txt = "F1-F3 key";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 10, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());
		txt = "1-6 key";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 30, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("HLSL-noise2");	
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
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

