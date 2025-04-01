/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

class Parabola : public STXGUI {
public:
	Parabola(){}
	virtual ~Parabola(){}
	float SizeX, SizeY; 
	virtual void init(const char* aTitle="")
	{
		SizeX=0.5f;
		SizeY=0.5f;
		STXGUI::init("/Parabola/GUILayout.xml");
	}

	virtual void render(void)
	{
		IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
		IRenderer::GetRendererInstance()->BeginScene();
#if 0
	virtual unsigned int drawLineVrtl(const float x0, const float y0, const float x1, const float y1, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
	virtual unsigned int drawRectVrtl(const float x0, const float y0, const float x1, const float y1, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
	virtual unsigned int drawRoundRectVrtl(const float x0, const float y0, const float x1, const float y1, const float r, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);

virtual unsigned int drawEllipseVrtl(const float originx, const float originy, const float width, const float height, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
virtual unsigned int drawParaboleVrtl(const float originx, const float originy, const float width, const float height, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
virtual unsigned int drawHyperboleVrtl(const float originx, const float originy, const float width, const float height, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
virtual unsigned int drawBezierVrtl(D3DXFROMWINEVECTOR2 anchor1, D3DXFROMWINEVECTOR2 anchor2, D3DXFROMWINEVECTOR2 control1, D3DXFROMWINEVECTOR2 control2, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);

	virtual unsigned int drawCircleVrtl(const float x, const float y, const float r, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
	virtual unsigned int drawTriangleVrtl(const float x0, const float y0, const float x1, const float y1, const float x2, const float y2, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
	virtual unsigned int drawQuadVrtl(const float x0, const float y0, const float x1, const float y1, const float s0, const float t0, const float s1, const float t1, const D3DXFROMWINEVECTOR4 &color);
#elif 1
IRenderer::GetRendererInstance()->drawParabole(100.0f, 25.0f, 25, 50, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(200.0f, 50.0f, 25, 50, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(300.0f, 50.0f, 25, 50, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(500.0f, 50.0f, 100.0f*SizeX, 100.0f*SizeY, D3DXFROMWINEVECTOR4(1,1,1,1));
#elif 0
IRenderer::GetRendererInstance()->drawParabole(100.0f, 25.0f, 25, 50, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(200.0f, 50.0f, 25, 50, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(300.0f, 50.0f, 25, 50, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(500.0f, 50.0f, 100.0f*SizeX, 100.0f*SizeY, D3DXFROMWINEVECTOR4(1,1,1,1));
#endif
  		STXGUI::update();

		char txt[128];
		stx_snprintf(txt, 128, "SizeX=%f\n", 100.0f*SizeX);
		IRenderer::GetRendererInstance()->drawText(txt, 10, 500, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		stx_snprintf(txt, 128, "SizeY=%f\n", 100.0f*SizeY);
		IRenderer::GetRendererInstance()->drawText(txt, 10, 520, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present();
	}

	virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
	GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
	if(radioButton->isPressed() && radioButton->isChecked())
	{
		#if 0
		if(callbackString == "") 
		{
			m_i=;
		}
		#endif
	}
	}
  if(widgetType == SLIDER)
  {

    GUISlider   *slider = (GUISlider*)sourceRectangle;
    char buf[256];

    if(callbackString == "SizeX")
    {
      SizeX=slider->getProgress();
      stx_snprintf(buf, 256, "SizeX: %4.2f", slider->getProgress());
      slider->setLabelString(buf);
    }
    if(callbackString == "SizeY")
    {
      SizeX=slider->getProgress();
      stx_snprintf(buf, 256, "SizeY: %4.2f", slider->getProgress());
      slider->setLabelString(buf);
    }
}
}
};

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Parabola");		
	IInput*    i=STX_Service::GetInputInstance();	
	Parabola app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}

