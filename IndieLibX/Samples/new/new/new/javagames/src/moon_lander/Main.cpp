/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include "Game.h"
	
Game* game=0;		
int init(const char* aTitle)
{		
    newGame();
    // Sets variables and objects.
    Initialize();
    // Load files - images, sounds, ...
    LoadContent();
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
		   
                if(e.getKeyCode() == KeyEvent.VK_ENTER)
                    restartGame();

                    gameTime += System.nanoTime() - lastTime;
                    
                    game->UpdateGame(gameTime, mousePosition());
                    
                    lastTime = System.nanoTime();
                game->Draw(g2d, mousePosition(), gameTime);
                game->DrawStatistic(g2d, gameTime);

                const char* txt = "Use w, a, d or arrow keys to move the helicopter.";
		IRenderer::GetRendererInstance()->drawText(txt,
			frameWidth / 2 - 117, frameHeight / 2 - 30,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
                txt = "Use left mouse button to fire bullets and right mouse button to fire rockets.";
		IRenderer::GetRendererInstance()->drawText(txt,
			, frameWidth / 2 - 180, frameHeight / 2,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
                txt = "Press any key to start the game or ESC to exit.";
		IRenderer::GetRendererInstance()->drawText(txt,
			frameWidth / 2 - 114, frameHeight / 2 + 30,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("moon_lander");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

