/*
  Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

stx_Effect* g_pEffect=0;
MeshRenderer2 m_Mesh;
TextureID texture=-1;

int init(const char* aTitle)
{
    g_pEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/SimpleTexture/SimpleMesh.fx");
    if(!g_pEffect)
        stx_exit(0);
	texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, IRenderer::GetRendererInstance()->Getlinear());
	m_Mesh.CreateSphere(1.0f, eShaderNone);
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
    unsigned int cPasses=0;
    unsigned int Value=0;
	g_pEffect->Begin(&cPasses, Value);
	for(unsigned int iPass=0;iPass<cPasses;iPass++)
	{
	g_pEffect->BeginPass(iPass);
	
	if(g_pEffect->m_iTechnique==-1)
		STX_PRINT("g_pEffect->m_iTechnique==-1\n");

	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	g_pEffect->SetVector("color", &color);
	D3DXFROMWINEMATRIX mvp;
    static float mAngleX=0.0f;
	static float mAngleY=0.0f;
		stx_GetAngles(mAngleX, mAngleY, 0.5f);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &mvp,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
	g_pEffect->SetMatrix("worldViewProj", &mvp);
	g_pEffect->SetTexture("Base", texture);
#if 1
	m_Mesh.render(&mvp, texture);
#else
	D3DXFROMWINEVECTOR4 a(0.0f, 0.1f, 0.0f, 1.0f);
	D3DXFROMWINEVECTOR4 d(0.0f, 1.0f, 0.0f, 1.0f);
	D3DXFROMWINEVECTOR4 l(0.0f, 1.0f, -1.0f, 1.0f);
	D3DXFROMWINEVECTOR4 e(0.0f, 0.0f, -1.0f, 1.0f);
        m_Mesh.Draw(&mvp, texture, -1, -1, a, d, l, e);
#endif);
	g_pEffect->EndPass();
	}
	g_pEffect->End();
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
	//stx_exit(0);
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("SimpleMesh");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

