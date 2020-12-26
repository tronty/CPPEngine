/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/ProceduralBuilding3D.h>

ProceduralBuilding2 proceduralBuilding;
D3DXFROMWINEVECTOR4 fogColor;
const bool mipmaps=false;
#if 0
TextureID env;
ShaderID skyBox;
#else
stx_SkyBox sb;
#endif
D3DXFROMWINEVECTOR3 camPos;
D3DXFROMWINEMATRIX mvp;

int init(const char* aTitle)
{
#if 0
	float f = stx_rand() / (RAND_MAX + 1.0f);
	for(unsigned int i=0;i<100;i++)
	{
		f = 1.0f*stx_rand() / (RAND_MAX + 1.0f);
		LOG_PRINT("f=%f\n", f);
	}
	stx_exit(0);
#endif
#if 1
	proceduralBuilding.init("/ProceduralBuilding/ProceduralBuilding2.xml", "/ProceduralBuilding3D.x", false);
#else
	proceduralBuilding.init("/ProceduralBuilding/ProceduralBuilding2.lua", "/ProceduralBuilding3D.x", false);
#endif
	camPos=D3DXFROMWINEVECTOR3(0.0f, 0.8f, 0.0f);
	fogColor = D3DXFROMWINEVECTOR4(1.0f, 0.5f, 0.1f, 1.0f);
	D3DXFROMWINEMatrixPerspectiveFovLH(&mvp, D3DXFROMWINE_PI / 2.0f, float(IRenderer::GetRendererInstance()->GetViewportWidth())/float(IRenderer::GetRendererInstance()->GetViewportHeight()), 0.001f, 1200000.0f);

	#if 0
	skyBox = IRenderer::GetRendererInstance()->addShaderFromFile("/SkyBox/skybox.shd", "main", "main");
	FormatDesc skyboxFmt[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };	
	skyboxVF = IRenderer::GetRendererInstance()->addVertexFormat(skyboxFmt, elementsOf(skyboxFmt), skyBox);
	env = IRenderer::GetRendererInstance()->addCubemap("Olympus", "jpg");
	#else
    	sb.init("Olympus", "jpg");
	#endif

	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	#if 1	
        sb.render();
	#else
	float vertices[] = { 0, 2, 0, 0, -2, -1, -1, -2, 1, 1, -2, 1 };
	ushort indices[] = { 0, 1, 2, 3, 0, 1 };

	IRenderer::GetRendererInstance()->setShader(skyBox);
	IRenderer::GetRendererInstance()->setVertexFormat(skyboxVF);

#if defined(CUBEMAPS)
	IRenderer::GetRendererInstance()->setTexture("Env", env, IRenderer::GetRendererInstance()->Gettrilinear());
#else
	IRenderer::GetRendererInstance()->setTexture("Env", env, IRenderer::GetRendererInstance()->GetbilinearClamp());
#endif
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthWrite());
	#if 0
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", I);
	#else
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	#endif
	IRenderer::GetRendererInstance()->setShaderConstant4f("fogColor", fogColor);
	IRenderer::GetRendererInstance()->setShaderConstant2f("scaleBias", D3DXFROMWINEVECTOR2(2 + 0.3f * camPos.y, camPos.y * 1.3f - 1.2f));

	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLE_STRIP, 0, 4, 4, indices, indices, CONSTANT_INDEX2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR3));

	#endif
	proceduralBuilding.render();

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("ProceduralBuilding3D");	
	IInput*    i=STX_Service::GetInputInstance();
	{
	#ifdef _MSC_VER
	float f=128.0f/256.0f;
	D3DXFROMWINEVECTOR4 col(f, f, f, 1.0f);
	IRenderer::GetRendererInstance()->Clear(true, true, col);
	IRenderer::GetRendererInstance()->BeginScene();

		const char* txt = "Initializing. Please wait a moment.";
		IRenderer::GetRendererInstance()->drawText(txt, 250, 250, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
	#endif
	}
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
		//stx_exit(0);
	}
	return 0;
}

