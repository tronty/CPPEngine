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

int renderSpine2DAnimations(const char* aani);
int initSpine2DAnimations();
void renderspineboy();
void rendergoblins();
void renderraptor();

const char* name[] =
{
"GoblinWalk",
"RaptorWalk",
"RaptorEmpty",
"RaptorGungrab",
"SpineBoyTest",
"SpineBoyWalk",
"SpineBoyJump",
"SpineBoyRun",
0
};

class TestSpine: public GUIEventListener
{
    bool mouseLocked;
    unsigned int irender;
public:
int init(const char* aTitle)
{
	initSpine2DAnimations();
  	irender=7;
	STXGUI::init("/spine/GUILayout.xml");
	return 0;
}

void render( )
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, false, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
#if 1
	renderSpine2DAnimations (name[irender]);
#else
	if(brenderspineboy)
		renderspineboy();
	if(brendergoblins)
		rendergoblins();
	if(brenderraptor)
		renderraptor();
#endif
	
	STXGUI::update();
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
private:
void actionPerformed(GUIEvent &evt)
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
	IRenderer* r=IRenderer::GetRendererInstance("testspine");		
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

