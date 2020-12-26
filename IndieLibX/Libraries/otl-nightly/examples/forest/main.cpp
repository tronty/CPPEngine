#include <renderer/sdlframework.h>

#include "forestgamelet.h"

int OTL_main (int argc, char ** argv)
{
  return SDL_FrameWork(new ForestGamelet()).run();
}


