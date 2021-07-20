/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <noise/noise.h>
#include <Framework3/IRenderer.h>

TextureID texture=-1;

void CreateILTexture(ubyte *data, int width, int height, TextureID& id)
{
	Image3 img;
	ubyte* data_=img.create(FORMAT_RGBA8, width, height, 1, 1);
	stx_memcpy(data_, data, width*height*4*sizeof(ubyte));
	id=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());
}

int CreateTexture()
{
	int width = 128;
	int height = 128;
	noise::module::Perlin perlinNoise;
	// Base frequency for octave 1.
	perlinNoise.SetFrequency(4.0);
	ubyte *data = new ubyte[ width * height * 4 ];
	double xRange = 1.0;
	double yRange = 1.0;
	double xFactor = xRange / width;
	double yFactor = yRange / height;

	for( int oct = 0; oct < 4; oct++ )
	{
		perlinNoise.SetOctaveCount(oct+1);
		for( int i=0;i<width;i++)
		{
			for( int j=0;j<height;j++)
			{
				double x = xFactor * i;
				double y = yFactor * j;
				double z = 0.0;
				float val = (float)perlinNoise.GetValue(x,y,z);
				// Scale and translate to roughly between 0 and 1
				val = (val + 1.0f) * 0.5f;
				// Clamp strictly between 0 and 1
				val = val> 1.0f ? 1.0f :val;
				val = val< 0.0f ? 0.0f :val;
				// Store in texture
				data[((j * width + i) * 4) + oct] = (ubyte) ( val * 255.0f );
			}
		}
	}
#if 0
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA, GL_UNSIGNED_BYTE,data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#else
	CreateILTexture(data, width, height, texture);
#endif
	delete [] data;

	return 0;
}

ShaderID shd=-1;
VertexFormatID vf=-1;

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/libnoise/rendersample.shd", "main", "main");

	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	//texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);
	CreateTexture();
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
	//		   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setTexture("Base", texture);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
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

