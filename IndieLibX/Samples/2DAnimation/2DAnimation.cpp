/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

const float _size=50.0f;
const char* name[] =
{
"mario_stand", 
"mario_walk", 
"Mario_jump",
0
};

class _2DAnimation : public STXGUI
{
public:
	_2DAnimation()
	{
		anim=0;
	}
	virtual ~_2DAnimation()
	{
		delete anim;
	}
public:
	FW3_Animation* anim;

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
			anim->SetSequence(name[i]);
			break;
		}
		i++;
	}
    }
  }
}
int init(const char* aTitle)
{
	anim=new FW3_Animation("/2DAnimation/mario.xml");
	anim->SetSequence("mario_stand");
	//anim->SetSequence("mario_walk");
	//anim->SetSequence("Mario_jump");

	anim->SetPosition(
		(IRenderer::GetRendererInstance()->GetWidth()-_size)/2,
		(IRenderer::GetRendererInstance()->GetHeight()-_size)/2);
	anim->SetSize(_size,_size);
	STXGUI::init("/2DAnimation/GUILayout.xml");
	return 0;
}
void render()
{
	IRenderer::GetRendererInstance()->Clear( true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	anim->render();
	STXGUI::update();

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("2DAnimation");		
	IInput*    i=STX_Service::GetInputInstance();
	_2DAnimation app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();	
		app.render();
	}
	return 0;
}

