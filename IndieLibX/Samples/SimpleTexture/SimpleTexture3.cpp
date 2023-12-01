/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <STX/PerlinNoise.h>

const char* filename[] =
{
	"/Smoke/FireTransferFunction.png",
	"/Smoke/fireBase.png",
	//"/textures/NVIDIA_Corporation/1D/fire_gradient3.png",
	//"/textures/NVIDIA_Corporation/1D/FireGrade.png",
	//"/textures/ATI_SDK/Spectrum.png",
	//"/textures/NVIDIA_Corporation/2d/flame.png",
	//"/Orcs/Firetex.png",
	//"/12_Explosion_Particle_Effect/explosion.png",
	"/new/R2VB_VTF/R2VB-Sort/FirePalette.png",
	"/new/R2VB_VTF/R2VB-Sort/SmokeAnimation.dds",// black ??? TRILINEAR, CLAMP
	"/DeferredParticles/DeferredParticle.png"
};
unsigned int dim[]={1,2, /*2,*/ 2,1,3,2,2,3};
unsigned int m_i=0;

ShaderID shaders[3];
TextureID textures[9];
VertexFormatID vf[3];

int init(const char* aTitle)
{
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	SamplerStateID ss3=IRenderer::GetRendererInstance()->GettrilinearClamp();
	unsigned int i=0;
	for(i=0;i<elementsOf(filename);i++)
	{
		if(i==5)
			textures[i]=IRenderer::GetRendererInstance()->addImageLibTexture(filename[i], false, ss3);
		else
			textures[i]=IRenderer::GetRendererInstance()->addImageLibTexture(filename[i], false, ss);
	}

	Image3 img2D=stx_Create2DNoiseTexture(64, 64);
	textures[7]=IRenderer::GetRendererInstance()->addTexture(&img2D, false, ss);
	Image3 img3D=stx_Create3DNoiseTexture(64, 64, 64);
	textures[8]=IRenderer::GetRendererInstance()->addTexture(&img3D, false, ss);

	shaders[0] = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.hlsl", "mainVS", "main1D");
	shaders[1] = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.hlsl", "mainVS", "main2D");
	shaders[2] = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.hlsl", "mainVS", "main3D");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf[0] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[0]);
	vf[1] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[1]);
	vf[2] = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shaders[2]);
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1))
		m_i=0;
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2))
		m_i=1;
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F3))
		m_i=2;
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F4))
		m_i=3;
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F5))
		m_i=4;
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F6))
		m_i=5;
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F7))
		m_i=6;
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F8))
		m_i=7;
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F9))
		m_i=8;

	IRenderer::GetRendererInstance()->setShader(shaders[dim[m_i]-1]);
	if(dim[m_i]==1)
		IRenderer::GetRendererInstance()->setTexture("Base1", textures[m_i]);
	else if(dim[m_i]==2)
		IRenderer::GetRendererInstance()->setTexture("Base", textures[m_i]);
	else if(dim[m_i]==3)
		IRenderer::GetRendererInstance()->setTexture("Base3", textures[m_i]);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);

	float z = (stx_rand() % 100)/99.0f;
	//LOG_PRINT("z=%f\n", z);
	IRenderer::GetRendererInstance()->setShaderConstant1f("z", z);

	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf[dim[m_i]-1]);
	//		   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
		const char* txt = "F1-F9 key";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 10, 
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
	IRenderer* r=IRenderer::GetRendererInstance("SimpleTexture3");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
/*
		   	   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
*/
/*
		  	   x	  y     tx    ty
	float v[16] =  { -1.0f, -1.0f, 0.0f, 1.0f,  
			  1.0f, -1.0f, 1.0f, 1.0f,   
			  1.0f,  1.0f, 1.0f, 0.0f,  
			 -1.0f,  1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, &v[0], &v[0], 4*sizeof(float));
*/

