/*
  Copyright (c) 2023 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <spine/Debug.h>
#include <spine/spine-sfml.h>

using namespace std;
using namespace spine;
#include <stdio.h>
#include <stdlib.h>

/*
tronty@dcx-d8yyyyyyyyyyyyycy-3:~/IndieLib_resources/spine$ find . -name *.skel
./hero_new/export/hero-pro.skel
./hero_new/export/hero-ess.skel
./goblins_new/export/goblins-pro.skel
./goblins_new/export/goblins-ess.skel

./raptor/export/raptor.atlas
./hero_new/export/hero.atlas
./hero_new/export/hero-pma.atlas
./goblins-ffd/export/goblins-ffd.atlas
./goblins-ffd/export/goblins.atlas
./speedy/export/speedy.atlas
./dragon/export/dragon.atlas
./spineboy/export/spineboy.atlas
./alien/export/alien.atlas
./powerup/export/powerup.atlas
./spineboy-old/export/spineboy-old.atlas
./goblins_new/export/goblins-pma.atlas
./goblins_new/export/goblins.atlas
./hero-mesh/export/hero-mesh.atlas
./hero-mesh/export/hero.atlas
./goblins/export/goblins-ffd.atlas
./goblins/export/goblins.atlas
./hero/export/hero-mesh.atlas
./hero/export/hero.atlas
./spine-sfml/goblins-mesh.atlas
./spine-sfml/raptor.atlas
./spine-sfml/spineboy.atlas

./raptor/export/raptor.json
./hero_new/export/hero-ess.json
./hero_new/export/hero-pro.json
./goblins-ffd/images/pack.json
./goblins-ffd/export/goblins.json
./goblins-ffd/export/goblins-ffd.json
./speedy/export/speedy.json
./dragon/export/dragon.json
./spineboy/export/spineboy-hoverboard.json
./spineboy/export/spineboy-mesh.json
./spineboy/export/spineboy.json
./alien/export/alien.json
./powerup/export/powerup.json
./spineboy-old/images/normal/pack.json
./spineboy-old/images/diffuse/pack.json
./spineboy-old/export/spineboy-old.json
./goblins_new/images/pack.json
./goblins_new/export/goblins-pro.json
./goblins_new/export/goblins-ess.json
./hero-mesh/export/hero-mesh.json
./hero-mesh/export/hero.json
./spinosaurus/export/spinosaurus.json
./goblins/images/pack.json
./goblins/export/goblins.json
./goblins/export/goblins-ffd.json
./hero/export/hero-mesh.json
./hero/export/hero.json
./spine-sfml/raptor.json
./spine-sfml/goblins-mesh.json
./spine-sfml/spineboy.json
*/

const char* name[] =
{
	"raptor",
	"hero",
	"hero-mesh",
	"powerup",
	"spineboy",
	"speedy",
	"alien",
	"goblins",
	"goblins-ffd",
	"dragon",
	"spineboy-old",
	0
};

const char* state[] =
{
	"walk",
	"Attack",
	"Attack",
	"???",
	"run",
	"run",
	"run",
	"walk",
	"walk",
	"flying",
	"walk",
	0
};

const unsigned int y[] =
{
	590,
	460,
	460,
	460,
	460,
	460,
	460,
	460,
	460,
	400,
	460,
	0
};
class TestSpine: public STXGUI
{
    	unsigned int irender;
	Atlas* atlas[11];
	SkeletonData *skeletonData[11];
	SkeletonDrawable* drawable[11];
public:
void init_(unsigned int ai)
{
	char atlasfile[256];
	char jsonfile[256];
	stx_snprintf(atlasfile, 256, "/spine/%s/export/%s.atlas", name[ai], name[ai]);
	stx_snprintf(jsonfile, 256, "/spine/%s/export/%s.json", name[ai], name[ai]);
	std::string atlasfile2=stx_convertpath(std::string(atlasfile));
	std::string jsonfile2=stx_convertpath(std::string(jsonfile));
}
int init(const char* aTitle)
{
  	irender=9;
	init_(0);
	init_(1);
	init_(2);
	init_(4);
	init_(5);
	init_(6);
	init_(7);
	init_(8);
	init_(9);
	init_(10);


	testcase(dragon, "data/dragon-ess.json", "data/dragon-ess.skel", "data/dragon-pma.atlas", 0.6f);
	testcase(ikDemo, "data/spineboy-pro.json", "data/spineboy-pro.skel", "data/spineboy-pma.atlas", 0.6f);
	testcase(spineboy, "data/spineboy-pro.json", "data/spineboy-pro.skel", "data/spineboy-pma.atlas", 0.6f);
	testcase(coin, "data/coin-pro.json", "data/coin-pro.skel", "data/coin-pma.atlas", 0.5f);
	testcase(mixAndMatch, "data/mix-and-match-pro.json", "data/mix-and-match-pro.skel", "data/mix-and-match-pma.atlas", 0.5f);
	testcase(test, "data/tank-pro.json", "data/tank-pro.skel", "data/tank-pma.atlas", 1.0f);
	testcase(owl, "data/owl-pro.json", "data/owl-pro.skel", "data/owl-pma.atlas", 0.5f);
	testcase(vine, "data/vine-pro.json", "data/vine-pro.skel", "data/vine-pma.atlas", 0.5f);
	testcase(tank, "data/tank-pro.json", "data/tank-pro.skel", "data/tank-pma.atlas", 0.2f);
	testcase(raptor, "data/raptor-pro.json", "data/raptor-pro.skel", "data/raptor-pma.atlas", 0.5f);
	testcase(goblins, "data/goblins-pro.json", "data/goblins-pro.skel", "data/goblins-pma.atlas", 1.4f);
	testcase(stretchyman, "data/stretchyman-pro.json", "data/stretchyman-pro.skel", "data/stretchyman-pma.atlas", 0.6f);
	testcase(testSkinsApi, "data/goblins-pro.json", "data/goblins-pro.skel", "data/goblins-pma.atlas", 1.4f);


	STXGUI::init("/spine/GUILayout2.xml");
	return 0;
}

void render( )
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	float delta = timeGetTime()/1000000.0f;

	drawable[irender]->update(delta);			
	drawable[irender]->draw();
	
	STXGUI::update();

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
private:
virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
    GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
    if(radioButton->isPressed() && radioButton->isChecked())
    {
	unsigned int i=0;
	while(1)
	{
		if(name[i]==0)
			break;
      		if(callbackString == name[i])
      		{
			irender = i;
			break;
		}
		i++;
	}
    }
  }
}
};

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("testspine3");		
	IInput*    i=STX_Service::GetInputInstance();
	TestSpine test;
	test.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();	
		test.render();
	}
	return 0;
}

