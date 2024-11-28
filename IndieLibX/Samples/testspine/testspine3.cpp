/*
  Copyright (c) 2024 Tommi Roenty   http://www.tommironty.fi/
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

const char* name[] =
{
	0
};
/*
./windmill/export/windmill.atlas
./windmill/export/windmill-pma.atlas
./vine/export/vine-pma.atlas
./vine/export/vine.atlas
./spineboy/export/spineboy.atlas
./spineboy/export/spineboy-pma.atlas
./spineboy/export/spineboy-run.atlas
./raptor/export/raptor-pma.atlas
./raptor/export/raptor.atlas
./raptor/manual-maps/raptor-pma.atlas
./raptor/manual-maps/raptor.atlas
./goblins/export/goblins.atlas
./goblins/export/goblins-pma.atlas
./tank/export/tank-pma.atlas
./tank/export/tank.atlas
./coin/export/coin.atlas
./coin/export/coin-pma.atlas
./chibi-stickers/export/chibi-stickers-pma.atlas
./chibi-stickers/export/chibi-stickers.atlas
./alien/export/alien.atlas
./alien/export/alien-pma.atlas
./owl/export/owl.atlas
./owl/export/owl-pma.atlas
./mix-and-match/export/mix-and-match-pma.atlas
./mix-and-match/export/mix-and-match.atlas
./stretchyman/export/stretchyman-pma.atlas
./stretchyman/export/stretchyman.atlas
./stretchyman/manual-maps/stretchyman-pma.atlas
./stretchyman/manual-maps/stretchyman.atlas
./hero/export/hero.atlas
./hero/export/hero-pma.atlas
./speedy/export/speedy.atlas
./speedy/export/speedy-pma.atlas
./powerup/export/powerup.atlas
./powerup/export/powerup-pma.atlas
./dragon/export/dragon-pma.atlas
./dragon/export/dragon.atlas

./windmill/export/windmill-ess.json
./vine/export/vine-pro.json
./spineboy/export/spineboy-pro.json
./spineboy/export/spineboy-ess.json
./raptor/export/raptor-pro.json
./goblins/export/goblins-ess.json
./goblins/export/goblins-pro.json
./goblins/images/pack.json
./tank/export/tank-pro.json
./coin/export/coin-pro.json
./chibi-stickers/export/chibi-stickers.json
./alien/export/alien-ess.json
./alien/export/alien-pro.json
./spinosaurus/export/spinosaurus-ess.json
./owl/export/owl-pro.json
./mix-and-match/export/mix-and-match-pro.json
./mix-and-match/images/pack.json
./stretchyman/export/stretchyman-pro.json
./spine-unity/spineboy-pro/import/spineboy-pro.json
./spine-unity/footsoldier/import/FootSoldier.json
./spine-unity/raptor/import/raptor.json
./spine-unity/spineboy-unity/import/spineboy-unity.json
./spine-unity/raggedyspineboy/import/raggedy spineboy.json
./spine-unity/spineunitygirl/import/Doi.json
./spine-unity/whirlyblendmodes/import/whirlyblendmodes.json
./spine-unity/eyes/import/eyes.json
./spine-unity/gauge/import/Gauge.json
./hero/export/hero-pro.json
./hero/export/hero-ess.json
./speedy/export/speedy-ess.json
./powerup/export/powerup-ess.json
./powerup/export/powerup-pro.json
./dragon/export/dragon-ess.json

./windmill/export/windmill-ess.skel
./vine/export/vine-pro.skel
./spineboy/export/spineboy-ess.skel
./spineboy/export/spineboy-pro.skel
./raptor/export/raptor-pro.skel
./goblins/export/goblins-ess.skel
./goblins/export/goblins-pro.skel
./tank/export/tank-pro.skel
./coin/export/coin-pro.skel
./chibi-stickers/export/chibi-stickers.skel
./alien/export/alien-pro.skel
./alien/export/alien-ess.skel
./spinosaurus/export/spinosaurus-ess.skel
./owl/export/owl-pro.skel
./mix-and-match/export/mix-and-match-pro.skel
./stretchyman/export/stretchyman-pro.skel
./hero/export/hero-ess.skel
./hero/export/hero-pro.skel
./speedy/export/speedy-ess.skel
./powerup/export/powerup-ess.skel
./powerup/export/powerup-pro.skel
./dragon/export/dragon-ess.skel
*/

const char* state[] =
{
	0
};

const unsigned int y[] =
{
	0
};

class TestSpine: public STXGUI
{
    	unsigned int irender;
	Atlas* atlas[13];
	SkeletonData *skeletonData[13];
	SkeletonDrawable* drawable[13];
public:
void i_(const int aI, const char* aJson, const char* aSkel, const char* aAtlas, const float f)
{
	char atlasfile[256];
	char jsonfile[256];
	char skelfile[256];
	stx_snprintf(atlasfile, 256, "/spine/%s/export/%s.atlas", aAtlas, aAtlas);
	stx_snprintf(jsonfile, 256, "/spine/%s/export/%s.json", aJson, aJson);
	stx_snprintf(skelfile, 256, "/spine/%s/export/%s.skel", aSkel, aSkel);
	std::string atlasfile2=stx_convertpath(std::string(atlasfile));
	std::string jsonfile2=stx_convertpath(std::string(jsonfile));
	std::string skelfile2=stx_convertpath(std::string(skelfile));
	
}
int init()
{
  	irender=13;

	i_(0, "data/dragon-ess.json", "data/dragon-ess.skel", "data/dragon-pma.atlas", 0.6f);
	i_(1, "data/spineboy-pro.json", "data/spineboy-pro.skel", "data/spineboy-pma.atlas", 0.6f);
	i_(2, "data/spineboy-pro.json", "data/spineboy-pro.skel", "data/spineboy-pma.atlas", 0.6f);
	i_(3, "data/coin-pro.json", "data/coin-pro.skel", "data/coin-pma.atlas", 0.5f);
	i_(4, "data/mix-and-match-pro.json", "data/mix-and-match-pro.skel", "data/mix-and-match-pma.atlas", 0.5f);
	i_(5, "data/tank-pro.json", "data/tank-pro.skel", "data/tank-pma.atlas", 1.0f);
	i_(6, "data/owl-pro.json", "data/owl-pro.skel", "data/owl-pma.atlas", 0.5f);
	i_(7, "data/vine-pro.json", "data/vine-pro.skel", "data/vine-pma.atlas", 0.5f);
	i_(8, "data/tank-pro.json", "data/tank-pro.skel", "data/tank-pma.atlas", 0.2f);
	i_(9, "data/raptor-pro.json", "data/raptor-pro.skel", "data/raptor-pma.atlas", 0.5f);
	i_(10, "data/goblins-pro.json", "data/goblins-pro.skel", "data/goblins-pma.atlas", 1.4f);
	i_(11, "data/stretchyman-pro.json", "data/stretchyman-pro.skel", "data/stretchyman-pma.atlas", 0.6f);
	i_(12, "data/goblins-pro.json", "data/goblins-pro.skel", "data/goblins-pma.atlas", 1.4f);

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

