#include <renderer/sdlframework.h>
#include <treeview.h>

#include <Framework3/IRenderer.h>

IRenderer* r=0;
IInput*    i=0;

int ApplicationLogic()
{
  r=IRenderer::GetRendererInstance("testOTL1");		
  i=STX_Service::GetInputInstance();

  int argc0 = 4;
  #if 1
  const char *argv0[] = {"testOTL1", "otl-xml", "otl-weber", "/otldata/weber-xml/weeping_willow.xml"};
  #else
  const char *argv0[] = {"testOTL1", "arbaro-xml", "arbaro-weber", "/otldata/arbaro-xml/wheat.xml"};
  #endif

  if (argc0 != 4)
  {
    LOG_PRINT("Usage: treeview [otl-xml|arbaro-xml] [otl-weber|arbaro-weber] xmlfile\n");
    return 1;
  }

  std::string f=stx_convertpath(argv0[3]);
  std::ifstream test(f.c_str(), std::ios::in);
  if (!test)
  {
    LOG_PRINT("File %s does not exist or could not be read.  Exiting.\n", f.c_str());
    return -1;
  }
  test.close();

  opentree::setRandomSeed(4);
  
  TreeView* treeView = new TreeView(argv0[1], argv0[2], f.c_str());
  treeView->init("");
  while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
  {
    i->Update();
    Events ev;
    treeView->handleEvent(ev);
    treeView->draw();
    stx_exit(0);
  }
  return 0;
}

