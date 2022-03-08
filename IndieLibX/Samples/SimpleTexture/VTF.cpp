/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

ShaderID shd=-1;
VertexFormatID vf=-1;
TextureID nn=-1;
TextureID nl=-1;
TextureID ln=-1;
TextureID ll=-1;

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/VTF.hlsl", "mainVS2", "mainPS");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
#if 1
    	nn=IRenderer::GetRendererInstance()->addImageLibTexture("test.bmp", false,
		//IRenderer::GetRendererInstance()->Getlinear());
		IRenderer::GetRendererInstance()->GetnearestClamp());
	nl=IRenderer::GetRendererInstance()->addImageLibTexture("test.bmp", false,
		//IRenderer::GetRendererInstance()->Getlinear());
		IRenderer::GetRendererInstance()->GetnearestClamp());
	ln=IRenderer::GetRendererInstance()->addImageLibTexture("test.bmp", false,
		IRenderer::GetRendererInstance()->Getlinear());
		//IRenderer::GetRendererInstance()->GetnearestClamp());
	ll=IRenderer::GetRendererInstance()->addImageLibTexture("test.bmp", false,
		IRenderer::GetRendererInstance()->Getlinear());
		//IRenderer::GetRendererInstance()->GetnearestClamp());
#else
	Image3 img1, img2;
	#ifdef _MSC_VER
	img1.loadBMP("test.bmp");
	#else
	img1.loadBMP("/test.bmp");
	#endif
	int w = 128;// img1.getWidth();
	int h = 128;// img1.getHeight();
	ubyte* src=img1.getPixels();
	float* dst = (float*)img2.create(FORMAT_RGBA8, w, h, 1, 1);
	if(0)
	for(int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
			int i=y*w+x;
			dst[4*i+0] = 1*src[3 * i + 0];
			dst[4*i+1]=1*src[3*i+1];
			dst[4*i+2]=1*src[3*i+2];
			dst[4*i+3]=255;
		}
	}
    else
	    img2 = img1;
	nn=IRenderer::GetRendererInstance()->addTexture(&img2, true, IRenderer::GetRendererInstance()->GetnearestClamp());
	ll=IRenderer::GetRendererInstance()->addTexture(&img2, true, IRenderer::GetRendererInstance()->Getlinear());
	ln=IRenderer::GetRendererInstance()->addTexture(&img2, true, IRenderer::GetRendererInstance()->Getlinear());
	nl=IRenderer::GetRendererInstance()->addTexture(&img2, true, IRenderer::GetRendererInstance()->GetnearestClamp());
#endif
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	switch(0){
	case 1:
	IRenderer::GetRendererInstance()->setTexture("tex", nl,
		IRenderer::GetRendererInstance()->Getlinear());
		//IRenderer::GetRendererInstance()->GetnearestClamp());
		break;
	case 2:
	IRenderer::GetRendererInstance()->setTexture("tex", ln,
		//IRenderer::GetRendererInstance()->Getlinear());
		IRenderer::GetRendererInstance()->GetnearestClamp());
		break;
	case 3:
	IRenderer::GetRendererInstance()->setTexture("tex", ll,
		IRenderer::GetRendererInstance()->Getlinear());
		//IRenderer::GetRendererInstance()->GetnearestClamp());
		break;
	default:
	IRenderer::GetRendererInstance()->setTexture("tex", nn,
		//IRenderer::GetRendererInstance()->Getlinear());
		IRenderer::GetRendererInstance()->GetnearestClamp());
		break;
	};
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//		     x	   y
	#if 0
	float v[8] =  {    1.0f,-1.0f,
			    1.0f, 1.0f,
			   -1.0f,-1.0f,
			   -1.0f, 1.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 2*sizeof(float));
	#elif 1
	float v[16] =  {    1.0f,-1.0f, 1.0f, 1.0f,
			    1.0f, 1.0f, 1.0f, 0.0f,
			   -1.0f,-1.0f, 0.0f, 1.0f,
			   -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
	#elif 0
	float v[16] =  {    1.0f,-1.0f, 0.0f, 0.0f,
			    1.0f, 1.0f, 0.0f, 0.0f,
			   -1.0f,-1.0f, 0.0f, 0.0f,
			   -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
	#endif
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("VTF");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

