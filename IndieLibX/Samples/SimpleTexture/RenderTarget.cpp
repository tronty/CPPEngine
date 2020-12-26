/*
  Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

ShaderID shd=-1;
TextureID texture=-1;
VertexFormatID vf=-1;
TextureID heightRT=-1;
TextureID terrainRT=-1;
#define TERRAIN_SIZE 512

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTextureVTF.shd", "mainVS", "mainPS");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, IRenderer::GetRendererInstance()->Getlinear());
	heightRT = IRenderer::GetRendererInstance()->addRenderTarget(TERRAIN_SIZE, TERRAIN_SIZE, FORMAT_R32F, IRenderer::GetRendererInstance()->GetnearestClamp());
	terrainRT = IRenderer::GetRendererInstance()->addRenderTarget(TERRAIN_SIZE, TERRAIN_SIZE, FORMAT_RGBA32F, IRenderer::GetRendererInstance()->GetnearestClamp());
	return 0;
}

void drawQuad(TextureID texture_)
{ 
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->setTexture("Base", texture_, IRenderer::GetRendererInstance()->GetnearestClamp());
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//		           x	 y     tx    ty
	float v[16] =  {          1.0f,-1.0f, 1.0f, 1.0f,
			          1.0f, 1.0f, 1.0f, 0.0f,
			         -1.0f,-1.0f, 0.0f, 1.0f,
			         -1.0f, 1.0f, 0.0f, 0.0f };
	float v2[16] =  {  1.0f,-1.0f,
			           1.0f, 1.0f,
			          -1.0f,-1.0f,
			          -1.0f, 1.0f };
#if 1
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
#else
	__WORD__ i[6] ={0,1,2,2,1,3};
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, 4, 2, &i[0], &i[0], CONSTANT_INDEX2, &v[0], &v[0], 4*sizeof(float));
#endif
}

void draw()
{
	IRenderer::GetRendererInstance()->changeRenderTarget(heightRT);
	drawQuad(texture);
	IRenderer::GetRendererInstance()->changeRenderTarget(terrainRT);
	drawQuad(heightRT);
	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
	drawQuad(terrainRT);
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	#if 0
	draw();
	#else
	drawQuad(texture);
	#endif

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("RenderTarget");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

