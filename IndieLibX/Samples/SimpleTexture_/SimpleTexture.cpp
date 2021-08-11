/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

ShaderID shd=-1;
TextureID texture=-1;
VertexFormatID vf=-1;

int init(const char* aTitle)
{
	//XFileParser();
LOG_FNLN;
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.shd", "mainVS", "mainPS");
LOG_FNLN;
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
LOG_FNLN;
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
LOG_FNLN;
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
LOG_FNLN;
	texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);
LOG_FNLN;
	//texture=IRenderer::GetRendererInstance()->addImageLibTexture("/treetextures/tree35s.dds", false, ss);

	//stx_OBJFile2XFile("../../IndieLib_resources/blender_3ds/cube.obj");

	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->setTexture("Base", texture);
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

	//		   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
LOG_FNLN;
#if 0
#ifndef _MSC_VER
									//	Ubuntu: Ubuntu:	Darwin:	Windows10:
									//	32-bit  64-bit
	LOG_PRINT("sizeof(char)=%ld\n", sizeof(char));			//	1	1	1	1
	LOG_PRINT("sizeof(unsigned char)=%ld\n", sizeof(unsigned char));//	1	1	1	1
	LOG_PRINT("sizeof(unsigned short)=%ld\n", sizeof(unsigned short));//	2	2	2	2
	LOG_PRINT("sizeof(unsigned int)=%ld\n", sizeof(unsigned int));	//	4	4	4	4
	LOG_PRINT("sizeof(unsigned long)=%ld\n", sizeof(unsigned long));//	4	8	8	4
	LOG_PRINT("sizeof(WORD)=%ld\n", sizeof(WORD));			//	2	2	2	2
	LOG_PRINT("sizeof(DWORD)=%ld\n", sizeof(DWORD));		//	4	4	4	4
	LOG_PRINT("sizeof(UCHAR)=%ld\n", sizeof(UCHAR));		//	1	1	1	1
	LOG_PRINT("sizeof(USHORT)=%ld\n", sizeof(USHORT));		//	2	2	2	2
	LOG_PRINT("sizeof(UINT)=%ld\n", sizeof(UINT));			//	4	4	4	4
	LOG_PRINT("sizeof(ULONG)=%ld\n", sizeof(ULONG));		//	4	4	8	4
	LOG_PRINT("sizeof(float)=%ld\n", sizeof(float));		//	4	4	4	4
	LOG_PRINT("sizeof(double)=%ld\n", sizeof(double));		//	8	8	8	8
#else
  ofstream myfile ("typesizes.txt");
  if (myfile.is_open())
  {
	myfile << "sizeof(char)=%ld\n" << sizeof(char)  << '\n';			//	1	1	1	1
	myfile << "sizeof(unsigned char)=%ld\n" << sizeof(unsigned char)  << '\n';	//	1	1	1	1
	myfile << "sizeof(unsigned short)=%ld\n" << sizeof(unsigned short)  << '\n';	//	2	2	2	2
	myfile << "sizeof(unsigned int)=%ld\n" << sizeof(unsigned int)  << '\n';	//	4	4	4	4
	myfile << "sizeof(unsigned long)=%ld\n" << sizeof(unsigned long)  << '\n';	//	4	8	8	4
	myfile << "sizeof(WORD)=%ld\n" << sizeof(WORD)  << '\n';			//	2	2	2	2
	myfile << "sizeof(DWORD)=%ld\n" << sizeof(DWORD)  << '\n';			//	4	4	4	4
	myfile << "sizeof(UCHAR)=%ld\n" << sizeof(UCHAR)  << '\n';			//	1	1	1	1
	myfile << "sizeof(USHORT)=%ld\n" << sizeof(USHORT)  << '\n';			//	2	2	2	2
	myfile << "sizeof(UINT)=%ld\n" << sizeof(UINT)  << '\n';			//	4	4	4	4
	myfile << "sizeof(ULONG)=%ld\n" << sizeof(ULONG)  << '\n';			//	4	4	8	4
	myfile << "sizeof(float)=%ld\n" << sizeof(float)  << '\n';			//	4	4	4	4
	myfile << "sizeof(double)=%ld\n" << sizeof(double)  << '\n';			//	8	8	8	8
    myfile.close();
  }
#endif	
#endif	
LOG_FNLN;
	IRenderer* r=IRenderer::GetRendererInstance("SimpleTexture");	
LOG_FNLN;
	IInput*    i=STX_Service::GetInputInstance();
LOG_FNLN;
	init("");
LOG_FNLN;
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
LOG_FNLN;
	return 0;
}

