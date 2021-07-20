/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <iostream>
#include <string.h>
#include <spine/spine-sfml.h>
#include <Framework3/IRenderer.h>
#include <GUI/GUIUtils.h>

using namespace spine;

/*
./spinosaurus/export/spinosaurus.json
./spineboy/export/spineboy-hoverboard.json
./spineboy/export/spineboy-mesh.json
./goblins/images/pack.json
./spineboy-old/images/normal/pack.json
./spineboy-old/images/diffuse/pack.json
./spine-sfml/goblins-mesh.atlas
./hero/export/hero-mesh.atlas
./goblins/export/goblins-ffd.atlas
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
/*
./spine-sfml/goblins-mesh.atlas
./spine-sfml/raptor.atlas
./spine-sfml/spineboy.atlas
./raptor/export/raptor.atlas
./hero/export/hero.atlas
./hero/export/hero-mesh.atlas
./goblins-ffd/export/goblins.atlas
./goblins-ffd/export/goblins-ffd.atlas
./hero-mesh/export/hero.atlas
./hero-mesh/export/hero-mesh.atlas
./powerup/export/powerup.atlas
./spineboy/export/spineboy.atlas
./speedy/export/speedy.atlas
./alien/export/alien.atlas
./goblins/export/goblins.atlas
./goblins/export/goblins-ffd.atlas
./dragon/export/dragon.atlas
./spineboy-old/export/spineboy-old.atlas

./spine-sfml/goblins-mesh.json
./spine-sfml/spineboy.json
./spine-sfml/raptor.json
./raptor/export/raptor.json
./hero/export/hero.json
./hero/export/hero-mesh.json
./goblins-ffd/images/pack.json
./goblins-ffd/export/goblins.json
./goblins-ffd/export/goblins-ffd.json
./hero-mesh/export/hero.json
./hero-mesh/export/hero-mesh.json
./powerup/export/powerup.json
./spinosaurus/export/spinosaurus.json
./spineboy/export/spineboy-hoverboard.json
./spineboy/export/spineboy.json
./spineboy/export/spineboy-mesh.json
./speedy/export/speedy.json
./alien/export/alien.json
./goblins/images/pack.json
./goblins/export/goblins.json
./goblins/export/goblins-ffd.json
./dragon/export/dragon.json
./spineboy-old/images/normal/pack.json
./spineboy-old/images/diffuse/pack.json
./spineboy-old/export/spineboy-old.json
*/
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
	//LOG_FNLN;
	//LOG_PRINT("atlasfile=%s\n", atlasfile);
	//LOG_PRINT("atlasfile2=%s\n", atlasfile2.c_str());
	atlas[ai] = Atlas_createFromFile(atlasfile2.c_str(), 0);
	SkeletonJson* json = SkeletonJson_create(atlas[ai]);
	json->scale = 0.5f;
	std::string jsonfile2=stx_convertpath(std::string(jsonfile));
	//LOG_FNLN;
	//LOG_PRINT("jsonfile=%s\n", jsonfile);
	//LOG_PRINT("jsonfile2=%s\n", jsonfile2.c_str());
	skeletonData[ai] = spSkeletonJson_readSkeletonDataFile(json, jsonfile2.c_str());
	if (!skeletonData[ai]) {
		LOG_PRINT("Error: %s\n", json->error);
		stx_exit(0);
	}
	SkeletonJson_dispose(json);
	drawable[ai] = new SkeletonDrawable(skeletonData[ai]);
	drawable[ai]->timeScale = 1;
	Skeleton* skeleton = drawable[ai]->skeleton;
	skeleton->x = 320;
	skeleton->y = y[ai];
	Skeleton_updateWorldTransform(skeleton);
	AnimationState_setAnimationByName(drawable[ai]->state, 0, state[ai], true);
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
	IRenderer* r=IRenderer::GetRendererInstance("testspine2");		
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

