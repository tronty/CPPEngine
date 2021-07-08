#include "precomp.h"
#include <iostream>
#include <GL\glew.h>
#include <SDL\SDL.h>
#include "LSystem.h"
#include "LSystemController.h"
#include "Renderer.h"
#include "VoxelWorld.h"
#include "EnvironmentAssets.h"

Renderer* pRenderer;

//TODO msati3: Hack for debugging. Handle consistently / remove
Renderer& getRenderer()
{
	return *pRenderer;
}


int main(int argc, char *argv[])
{
	pRenderer = new Renderer(800,600,32);
	Renderer& renderer = *pRenderer;
	renderer.init();

	std::unique_ptr<Skybox> pSkyDome(new Skybox(renderer.getCamera()));
	Skybox* pEnvironmentAssets = pSkyDome.get(); //TODO msati3: Hack??
	renderer.addDrawable(std::move(pSkyDome));
	std::unique_ptr<VoxelWorld> pWorld(new VoxelWorld(20, 10));
	VoxelWorld* pWorldRaw = pWorld.get(); //TODO msati3: Hack??
	renderer.addDrawable(std::move(pWorld));

	std::unique_ptr<LSystem> pSystem = LSystemBuilder::buildLSystem("C:\\Development\\openSource\\lSystemOld\\lSystem\\openLSystemDescription2.txt", *pWorldRaw);
	LSystemController systemController(std::move(pSystem), renderer);
	//TODO msati3: Move this to a world class
	
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_WM_GrabInput( SDL_GRAB_ON );

	while (true)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type)
			{
				case SDL_QUIT: return false;
					break;
				case SDL_KEYDOWN: 
					{
						SDL_keysym keysym = event.key.keysym;
						if (keysym.sym == 'w')
						{
							renderer.getCamera().zoom(2);
						}
						else if (keysym.sym == 's')
						{
							renderer.getCamera().zoom(-2);
						}
						else if (keysym.sym == 'a')
						{
							renderer.getCamera().strafe(-2);
						}
						else if (keysym.sym == 'd')
						{
							renderer.getCamera().strafe(2);
						}
						else if (keysym.sym == 'p')
						{
							pWorldRaw->addWorldBlock(renderer.getCamera().position());
						}
						else if (keysym.sym == 'r')
						{
							pWorldRaw->removeWorldBlock(renderer.getCamera().position());
						}
						else if (keysym.sym == 'z')
						{
							pWorldRaw->addWall(renderer.getCamera().position());
						}
						else if (keysym.sym == 'q')
						{
							renderer.getCamera().moveY(std::get<1>(pWorldRaw->getGridSizes()));
						}
						else if (keysym.sym == 'e')
						{
							renderer.getCamera().moveY(-std::get<1>(pWorldRaw->getGridSizes()));
						}
						else if (keysym.sym == 'u')
						{
							systemController.onUpdate();
						}
						else if (keysym.sym == '1')
						{
							pWorldRaw->toggleShowGrid();
						}
						else if (keysym.sym == '2')
						{
							pEnvironmentAssets->toggleShow();
						}
					}
					break;
				case SDL_MOUSEMOTION: 
					if (abs(event.motion.xrel) > abs(event.motion.yrel))
					{
						renderer.getCamera().rotateY(-event.motion.xrel*PI/360);
					}
					else
					{
						//renderer.getCamera().rotateX(-event.motion.yrel*PI/360);
					}
                break;
			}
		}
		renderer.draw();
		renderer.flip();
	}
    return 0;
}