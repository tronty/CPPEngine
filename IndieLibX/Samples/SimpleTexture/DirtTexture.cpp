/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

TextureID dirtTexture=-1;
ShaderID dirtshd=-1;
VertexFormatID dirtvf=-1;

int init(const char* aTitle)
{
	dirtshd = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/texSHD1.HLSL.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	dirtvf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), dirtshd);
	dirtTexture=IRenderer::GetRendererInstance()->addImageLibTexture(
				//"/AdvAniCh12/dirtTexture.jpg",
				"/Orcs/GRYDIRT1.jpg",
			false,
			IRenderer::GetRendererInstance()->Getlinear()
			//, 0xFFFF
			);
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
  	D3DXFROMWINEMATRIX matView, g_matProj;
	D3DXFROMWINEVECTOR3 g_vecChopper(0.0f, 0.0f, 0.0f);
  
    D3DXFROMWINEMatrixPerspectiveFovLH( &g_matProj, D3DXFROMWINEToRadian( 45.0f ),
                                //640.0f / 480.0f, 0.1f, 100.0f
								IRenderer::GetRendererInstance()->GetAspect(), 0.001f, 20000.0f
								);
  {D3DXFROMWINEVECTOR3 v1(g_vecChopper.x, 80.0f, g_vecChopper.z - 150.0f);
   D3DXFROMWINEVECTOR3 v2(g_vecChopper.x, 0.0f, g_vecChopper.z);
   D3DXFROMWINEVECTOR3 v3(0.0f, 1.0f, 0.0f);
  D3DXFROMWINEMatrixLookAtLH(&matView,
                     &v1,
                     &v2,
                     &v3);}

	IRenderer::GetRendererInstance()->setShader(dirtshd);
	IRenderer::GetRendererInstance()->setTexture("Sampler", dirtTexture);
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", color);
	float dim=1500.0f;
	D3DXFROMWINEMATRIX T;
	D3DXFROMWINEMatrixTranslation(&T, 0.0f, 0.0f, 0.0f);
	D3DXFROMWINEMATRIX W=T*matView*g_matProj;
	D3DXFROMWINEMatrixTranspose(&W, &W);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  	IRenderer::GetRendererInstance()->setVertexFormat(dirtvf);
	//		   x	     y     z         tx    ty
	float v[20] =  {  1.0f*dim, 0.0f,-1.0f*dim, 1.0f, 1.0f,
			  1.0f*dim, 0.0f, 1.0f*dim, 1.0f, 0.0f,
			 -1.0f*dim, 0.0f,-1.0f*dim, 0.0f, 1.0f,
			 -1.0f*dim, 0.0f, 1.0f*dim, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 5*sizeof(float));

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("DirtTexture");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

