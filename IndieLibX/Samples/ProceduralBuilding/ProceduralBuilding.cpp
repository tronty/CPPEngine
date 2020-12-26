/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/ProceduralBuilding.h>

stx_SkyBox sb;
ProceduralBuilding proceduralBuilding;

int init(const char* aTitle)
{
#if 1
	proceduralBuilding.init("/ProceduralBuilding/ProceduralBuilding.xml", "/ProceduralBuilding.x", false);
#else
	proceduralBuilding.init("/ProceduralBuilding/ProceduralBuilding.lua", "/ProceduralBuilding.x", false);
#endif
    sb.init("Olympus", "jpg");
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();	
    sb.render();
	proceduralBuilding.render();
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("ProceduralBuilding");	
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
	#if 0
	{
	MeshRenderer2 shape3D;
	shape3D.CreateBox(2.0f, 2.0f, 2.0f, eShaderNone);
	LOG_PRINT("Box.vertices.size()=%d\n", shape3D.vertices.size());
	LOG_PRINT("Box.indices.size()=%d\n", shape3D.indices.size());

	for(unsigned int i=0;i<shape3D.indices.size();i++)
	{
		stx_VertexPositionNormalTexture v=shape3D.vertices[shape3D.indices[i]];
		LOG_PRINT("TexCoord(%5.2f, %5.2f)Vertex(%5.2f, %5.2f, %5.2f)\n", v.Tex.x, v.Tex.y, 
							v.Position.x, v.Position.y, v.Position.z);
	}
	}
	{
	MeshRenderer2 shape3D;
	shape3D.CreateTexturedBox(2.0f, 2.0f, 2.0f, eShaderNone);
	LOG_PRINT("TexturedBox.vertices.size()=%d\n", shape3D.vertices.size());
	LOG_PRINT("TexturedBox.indices.size()=%d\n", shape3D.indices.size());

	for(unsigned int i=0;i<shape3D.indices.size();i++)
	{
		stx_VertexPositionNormalTexture v=shape3D.vertices[shape3D.indices[i]];
		LOG_PRINT("TexCoord(%5.2f, %5.2f)Vertex(%5.2f, %5.2f, %5.2f)\n", v.Tex.x, v.Tex.y, 
							v.Position.x, v.Position.y, v.Position.z);
	}
	}
	#else
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
		//stx_exit(0);
	}
	#endif
	return 0;
}

