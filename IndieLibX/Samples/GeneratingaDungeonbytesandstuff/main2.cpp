// https://eskerda.com/bsp-dungeon-generation/

/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/BSPDungeon.h>

BSPDungeon* BSPDungeon_=0;

int init(const char* aTitle)
{
	BSPDungeon_=new BSPDungeon();
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	BSPDungeon_->render();

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}


int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("BSPDungeon");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
		//stx_exit(0);
	}
	return 0;
}

