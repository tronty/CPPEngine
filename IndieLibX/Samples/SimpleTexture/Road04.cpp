/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#if 1
#define STX_PRINT
#define STX_FNLN
#else
#define STX_PRINT(...) printf(__VA_ARGS__)
#define STX_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)
#endif

ShaderID shd=-1;
VertexFormatID vf=-1;
MeshRenderer2 m_Mesh;

int init(const char* aTitle)
{
	STX_FNLN;
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/glslsandbox.com/e#63847.0.shd", "main2", "main");
	STX_FNLN;
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	STX_FNLN;
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	STX_FNLN;
	m_Mesh.CreateSphere(1.0f, shd);
	return 0;
}

void draw()
{
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	//D3DXFROMWINEVECTOR2 iResolution(IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());
	D3DXFROMWINEVECTOR2 iResolution(1,1);
	float iTime=0.001f*timeGetTime();
#if 0
	m_Mesh.BeginDraw(&I);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iResolution", iResolution);
	IRenderer::GetRendererInstance()->setShaderConstant1f("iTime", iTime);
	m_Mesh.EndDraw();
#else
	STX_FNLN;
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iResolution", iResolution);
	IRenderer::GetRendererInstance()->setShaderConstant1f("time", iTime);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	#if 1
	//		           x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			          1.0f, 1.0f, 1.0f, 0.0f,
			         -1.0f,-1.0f, 0.0f, 1.0f,
			         -1.0f, 1.0f, 0.0f, 0.0f };
	unsigned int N=4;
	#else
	float v[16] =  {  1.0f,-1.0f,
			           1.0f, 1.0f,
			          -1.0f,-1.0f,
			          -1.0f, 1.0f };
	unsigned int N=2;
	#endif
#if 1
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], N*sizeof(float));
#else
	__WORD__ i[6] ={0,1,2,2,1,3};
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, 4, 2, &i[0], &i[0], CONSTANT_INDEX2, &v[0], &v[0], N*sizeof(float));
#endif
#endif
	STX_FNLN;
}

void render()
{
	STX_FNLN;
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	STX_FNLN;
	IRenderer::GetRendererInstance()->BeginScene();
	STX_FNLN;
	draw();
	STX_FNLN;
	IRenderer::GetRendererInstance()->EndScene();
	STX_FNLN;
	IRenderer::GetRendererInstance()->Present( );
	STX_FNLN;
}

int ApplicationLogic()
{
	STX_FNLN;
	IRenderer* r=IRenderer::GetRendererInstance("Road04");
	STX_FNLN;
	IInput*    i=STX_Service::GetInputInstance();
	STX_FNLN;
	init("");
	STX_FNLN;
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
	STX_FNLN;
		i->Update();
	STX_FNLN;
		render();
	STX_FNLN;
		//stx_exit(0);
	}
	STX_FNLN;
	return 0;
}

