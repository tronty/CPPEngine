// http://trlewis.net/generating-a-dungeon-part-1/
// http://trlewis.net/generating-a-dungeon-part-2/
// http://trlewis.net/generating-a-dungeon-part-3/

/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <stdio.h>
#include <string>

#include <iostream>
#include <vector>

#include <Framework3/GeneratingaDungeonbytesandstuff_GeneratedMap.hpp>

int mapwidth = 25;
int mapheight = 16;
GeneratedMap generatedMap;

int init(const char* aTitle)
{
	generatedMap=GeneratedMap(mapwidth, mapheight);
	generatedMap.generateMap();
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_g))
	{
		generatedMap = GeneratedMap(mapwidth, mapheight);
		generatedMap.generateMap();
	}

	generatedMap.DrawMap();

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}


int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("DungeonGeneration01-03");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

