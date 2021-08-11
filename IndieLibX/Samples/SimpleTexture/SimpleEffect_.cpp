/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

stx_Effect* g_pEffect=0;
TextureID texture=-1;

int init(const char* aTitle)
{
    g_pEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/new/Clipmaps/Clipmaps_.fx");
    if(!g_pEffect)
    	stx_exit(0);
    texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, IRenderer::GetRendererInstance()->Getlinear());
    int g_StackDepth = 0;
    float aspectRatio = 4.0f/3.0f;
    //g_pEffect->SetInt("g_StackDepth", &g_StackDepth);
    g_pEffect->SetFloat("g_ScreenAspectRatio", aspectRatio);
    return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
	#if 1
            g_pEffect->GetTechniqueByName( "StackDrawPass" )->GetPassByIndex( 0 )->Apply( 0 );
        #else
    unsigned int cPasses=0;
    unsigned int Value=0;
	g_pEffect->Begin(&cPasses, Value);
	for(unsigned int iPass=0;iPass<cPasses;iPass++)
	{
	g_pEffect->BeginPass(iPass);
	#endif
	IRenderer::GetRendererInstance()->setTexture("Texture", texture);
	//		           x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			          1.0f, 1.0f, 1.0f, 0.0f,
			         -1.0f,-1.0f, 0.0f, 1.0f,
			         -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
	#if 0
	g_pEffect->EndPass();
	}
	g_pEffect->End();
	#endif
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("SimpleEffect_");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

