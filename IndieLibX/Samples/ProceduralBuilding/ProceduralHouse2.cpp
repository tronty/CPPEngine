/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/ProceduralHouse.h>

ProceduralHouse proceduralBuilding;
stx_SkyBox sb;

int init(const char* aTitle)
{
	proceduralBuilding.init();
	proceduralBuilding.SetFloorCount(3);
	proceduralBuilding.SetAxiom("example", "wallwithdoor:Ry:wallwithwindows:Ry:wallwithwindows:Ry:wallwithwindows");
	proceduralBuilding.AddRule("wallwithdoor", "wall:window:wall:door:wall:window:wall");
	proceduralBuilding.AddRule("wallwithwindows", "wall:window:wall:window:wall:window:wall");

	proceduralBuilding.SetWallTexture("/textures/ATI_SDK/1024px-brick.png");
	//proceduralBuilding.SetWallTexture("/textures/NVIDIA_Corporation/2d/relief_rockwall.jpg");
	proceduralBuilding.SetRoofTexture("/TextureLibrary/640px-Frankfurter_Pfanne_rot.png");
#if 1
	proceduralBuilding.SetWindowTexture("/TextureLibrary/Finland/Texture_Library_Finland-OpenStreetMap_Wiki_tiedostot/128px-FI20F2_OdlWoodWindow0001.jpg");
#else
	proceduralBuilding.SetWindowTexture("/AdvAniCh12/window.bmp");
#endif
	proceduralBuilding.SetDoorTexture("/RadeonTerrainDemo/CastleDoor.bmp");

	proceduralBuilding.SetTitle("ProceduralBuilding6");
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
	IRenderer* r=IRenderer::GetRendererInstance("ProceduralBuilding6");	
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
	}
	return 0;
}

