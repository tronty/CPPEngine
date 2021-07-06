/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <STX/Noise.h>

ShaderID shd=-1;
TextureID texID;
TextureID texture[7];
unsigned int m_i=0;
VertexFormatID vf=-1;
Image3 image;

#define NSIZE 64
#define SCALE 0.1482f


SamplerStateID GetSS(unsigned int i)
{
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	switch(i)
	{
		case 0:
			ss=IRenderer::GetRendererInstance()->Getlinear();
			break;
		case 1:
			ss=IRenderer::GetRendererInstance()->GetlinearAniso();
			break;
		case 2:
			ss=IRenderer::GetRendererInstance()->GetlinearClamp();
			break;
		case 3:
			ss=IRenderer::GetRendererInstance()->GetFILTER_LINEAR_ANISO();
			break;
		case 4:
			ss=IRenderer::GetRendererInstance()->Getbilinear();
			break;
		case 5:
			ss=IRenderer::GetRendererInstance()->GetbilinearAniso();
			break;
		case 6:
			ss=IRenderer::GetRendererInstance()->GetFILTER_BILINEAR_ANISO();
			break;
		case 7:
			ss=IRenderer::GetRendererInstance()->Gettrilinear();
			break;
		case 8:
			ss=IRenderer::GetRendererInstance()->GettrilinearAniso();
			break;
		case 9:
			ss=IRenderer::GetRendererInstance()->GetFILTER_TRILINEAR_ANISO();
			break;
		case 10:
			ss=IRenderer::GetRendererInstance()->Getnearest();
			break;
		case 11:
			ss=IRenderer::GetRendererInstance()->GetnearestClamp();
			break;
		case 12:
			ss=IRenderer::GetRendererInstance()->GetbilinearClamp();
			break;
		case 13:
			ss=IRenderer::GetRendererInstance()->GettrilinearClamp();
			break;
		default:
			ss=IRenderer::GetRendererInstance()->Getlinear();
			break;
	};
	return ss;
}

int init(const char* aTitle)
{
#if defined(USE_HLSL_SHADERS)
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.shd", "mainVS", "main3");
#else
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.GLSL.shd", "main", "main");
#endif
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
/*
	texture[0]=IRenderer::GetRendererInstance()->addImageLibTexture("/Orcs/Firetex.png", false, ss);
	//texture[0]=IRenderer::GetRendererInstance()->addImageLibTexture("/Smoke/FireTransferFunction.png", false, ss);
	//texture[0]=IRenderer::GetRendererInstance()->addImageLibTexture("/Smoke/fireBase.png", false, ss);
	texture[1]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/flame.png", false, ss);
	texture[2]=stx_Create2DNoiseTexture(ss);
	texture[3]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/1D/fire_gradient3.png", false, ss);
	texture[4]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/1D/FireGrade.png", false, ss);
	//texID[]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/Spectrum.png", false, IRenderer::GetRendererInstance()->Getlinear());
	texture[5]=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);

	texture[6]=stx_Create3DNoiseTexture(IRenderer::GetRendererInstance()->Gettrilinear());
*/
	image=stx_Create3DNoiseTexture();	
	texID=IRenderer::GetRendererInstance()->addTexture(&image, false, GetSS(0));
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	static float z=0.0f;

#if 1
	IRenderer::GetRendererInstance()->setShader(shd);
	if(0)
	{
		IRenderer::GetRendererInstance()->setTexture("Base1", texture[m_i]);
		IRenderer::GetRendererInstance()->setShaderConstant1i("dim", 1);
	}
	else if(1)
	{
		IRenderer::GetRendererInstance()->setTexture("Base3", texID);
		IRenderer::GetRendererInstance()->setShaderConstant1i("dim", 3);
		IRenderer::GetRendererInstance()->setShaderConstant1f("z", z);
	}
	else
	{
		IRenderer::GetRendererInstance()->setTexture("Base", texture[m_i]);
		IRenderer::GetRendererInstance()->setShaderConstant1i("dim", 2);
	}

	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	//		   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
#else
	IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLE_STRIP);

	IRenderer::GetRendererInstance()->Vertex2i(-1.0f, -1.0f); 
	IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, -1.0f);
	IRenderer::GetRendererInstance()->Vertex2i(-1.0f, 1); 
	IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
	IRenderer::GetRendererInstance()->Vertex2i(1, 0); 
	IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
	IRenderer::GetRendererInstance()->Vertex2i(1, 1);
	IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);

	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->End(texture, &I);
#endif
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F3))
	{
		z+=0.25f;
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F4))
	{
		z-=0.25f;
	}
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1) && (m_i>0))
	{
		m_i--;
		texID=IRenderer::GetRendererInstance()->addTexture(&image, false, GetSS(m_i));
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2))
	{
		m_i++;
		if(m_i>13)
			m_i=0;
		texID=IRenderer::GetRendererInstance()->addTexture(&image, false, GetSS(m_i));
	}
		char txt2[128];
		stx_snprintf(txt2, 128, "m_i=%d\n", m_i);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 25, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());
		stx_snprintf(txt2, 128, "z=%f\n", z);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 50, 
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
	IRenderer* r=IRenderer::GetRendererInstance("SimpleTexture");	
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

