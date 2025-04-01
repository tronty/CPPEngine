#include <Framework3/IRenderer.h>
/*
  Copyright (c) 2017 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <FW3.h>
#include <GUI/GUIUtils.h>

const char* name[] =
{
"SimpleExplosion", 
"ExplosionCluster",
0
};

class _2DAnimation : public STXGUI
{
public:
	unsigned int m_i;
	_2DAnimation()
	{
		m_i=0;
		for(unsigned int i=0;i<elementsOf(name)-1;i++)
			anim[i]=0;
	}
	virtual ~_2DAnimation()
	{
		for(unsigned int i=0;i<elementsOf(name)-1;i++)
			delete anim[i];
	}
public:
	FW3_Animation* anim[elementsOf(name)];

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
			anim[m_i]->SetSequence(name[i]);
			break;
		}
		i++;
	}
    }
  }
}
int init(const char* aTitle)
{
	for(unsigned int i=0;i<elementsOf(name)-1;i++)
	{
		char buf[256];
		stx_snprintf(buf, 256, "/20-Explosions/Ch20p%d_%s/%s.xml", i+1, name[i], name[i]);
		anim[i]=new FW3_Animation(buf);
		anim[i]->SetSequence(name[i]);

		anim[i]->SetPosition(
			IRenderer::GetRendererInstance()->GetWidth()/2-150.0f,
			IRenderer::GetRendererInstance()->GetHeight()/2-150.0f);
		anim[i]->SetSize(300.0f,300.0f);
	}
	STXGUI::init("/20-Explosions/GUILayout.xml");
	return 0;
}
void render()
{
	IRenderer::GetRendererInstance()->Clear( true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	anim[m_i]->render();
	STXGUI::update();

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("20-Explosions");
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


