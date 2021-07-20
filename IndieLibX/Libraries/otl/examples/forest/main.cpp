#include <renderer/sdlframework.h>

#include "forestgamelet.h"

//int OTL_main (int argc, char ** argv)
int ApplicationLogic()
{
  LOG_FNLN;
	IRenderer* r=IRenderer::GetRendererInstance("Forest");
  return SDL_FrameWork(new ForestGamelet()).run();
}


