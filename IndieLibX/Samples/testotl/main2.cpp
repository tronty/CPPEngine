#include <renderer/sdlframework.h>
#include "forestgamelet.h"

#include <Framework3/IRenderer.h>

IRenderer* r=0;
IInput*    i=0;

int ApplicationLogic()
{
  r=IRenderer::GetRendererInstance("testOTL2");		
  i=STX_Service::GetInputInstance();
  ForestGamelet* forest=new ForestGamelet(stx_convertpath("/otldata/forest-xml/demoforest.xml").c_str());
  forest->init("");
  while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
  {
    i->Update();
    Events ev;
    forest->handleEvent(ev);
    forest->draw();
    stx_exit(0);
  }
  return 0;
}

