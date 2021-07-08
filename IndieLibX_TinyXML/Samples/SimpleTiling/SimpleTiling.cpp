/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

TextureID mortarjointTexID=-1;
TextureID redbrickTexID=-1;

ShaderID shd=-1;
VertexFormatID vf=-1;

void RenderTex(TextureID id, D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize, D3DXFROMWINEVECTOR2 texposition=D3DXFROMWINEVECTOR2(0.0f, 0.0f), D3DXFROMWINEVECTOR2 texsize=D3DXFROMWINEVECTOR2(1.0f, 1.0f))
{
	D3DXFROMWINEVECTOR4 col(1.0f, 1.0f, 1.0f, 1.0f);
	TexVertex dest[4];
	float x=aposition.x;
	float y=aposition.y;
	dest[0].position = D3DXFROMWINEVECTOR2(x + asize.x, y);
	dest[0].texCoord = D3DXFROMWINEVECTOR2(texposition.x+texsize.x, texposition.y);
	dest[1].position = D3DXFROMWINEVECTOR2(x + asize.x, y + asize.y);
	dest[1].texCoord = D3DXFROMWINEVECTOR2(texposition.x+texsize.x, texposition.y+texsize.y);
	dest[2].position = D3DXFROMWINEVECTOR2(x, y);
	dest[2].texCoord = D3DXFROMWINEVECTOR2(texposition.x, texposition.y);
	dest[3].position = D3DXFROMWINEVECTOR2(x, y + asize.y);
	dest[3].texCoord = D3DXFROMWINEVECTOR2(texposition.x, texposition.y+texsize.y);
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", col);
	IRenderer::GetRendererInstance()->setTexture("Base", id);
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &dest[0], &dest[0], sizeof(TexVertex));
}

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Texture/Texture.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	TextureID texID=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss); 
	mortarjointTexID=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/mortarjoint.png", false, ss); 
	redbrickTexID=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/redbrick.png", false, ss);
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	for(unsigned int i=0;i<4;i++)
	{
	for(unsigned int j=0;j<5;j++)
	{
		{
			D3DXFROMWINEVECTOR2 p(i*(200.0f+10.0f), j*(100.0f+10.0f));
			D3DXFROMWINEVECTOR2 s(200.0f, 100.0f);
			RenderTex(redbrickTexID, p, s);
		}
		{
			D3DXFROMWINEVECTOR2 p(0.0f+i*210.0f, -10.0f+j*110.0f);
			D3DXFROMWINEVECTOR2 s(210.0f, 10.0f);
			RenderTex(mortarjointTexID, p, s);
		}
		{
			D3DXFROMWINEVECTOR2 p(-10.0f+i*210.0f, -10.0f+j*110.0f);
			D3DXFROMWINEVECTOR2 s(10.0f, 110.0f);
			RenderTex(mortarjointTexID, p, s);
		}
	}
	}
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("SimpleTiling");	
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

