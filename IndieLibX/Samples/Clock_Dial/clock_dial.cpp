/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <time.h>

struct tm *getTime()
{
    time_t curtime;
    time(&curtime);
    return localtime(&curtime);
}
 
float degrees06 = (float) (D3DXFROMWINE_PI / 30);
float degrees30 = degrees06 * 5;
float degrees90 = degrees30 * 3;

int size_ = 590;
int spacing = 40;
int diameter = size_ - 2 * spacing;
int cx = diameter / 2 + spacing;
int cy = diameter / 2 + spacing;

void drawLine(const float x0,  const float y0,  const float x1,  const float y1)
{
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GetplainShader());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GetplainVF());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
	D3DXFROMWINEVECTOR4 col(1.0f,1.0f,1.0f,1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", col);	
	IRenderer::GetRendererInstance()->drawLine(x0, y0, x1, y1, col, 1);
}

void drawHand(float angle, int radius)
{
        int x = cx + (int) (radius * cos(angle));
        int y = cy - (int) (radius * sin(angle));
        drawLine(cx, cy, x, y);
}

void drawCircles(int radius)
{
	for(unsigned int i=0;i<12;i++)
	{
		float angle = (float) (D3DXFROMWINE_PI / 6);
        	int x = cx + (int) (radius * cos(angle*i));
        	int y = cy - (int) (radius * sin(angle*i));
		IRenderer::GetRendererInstance()->drawCircle(x, y, 5.0f, D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f));
	}
}

int init(const char* aTitle)
{
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	drawCircles(diameter / 2 - 30);

	struct tm *curtime = getTime();

	if (curtime->tm_hour > 12)
	    curtime->tm_hour -= 12;
	if (curtime->tm_hour == 0)
	    curtime->tm_hour = 12;

        int hour = curtime->tm_hour;
        int minute = curtime->tm_min;
        int second = curtime->tm_sec;

        float angle = degrees90 - (degrees06 * second);
        drawHand(angle, diameter / 2 - 30);//, Color.red);
 
        float minsecs = (minute + second / 60.0f);
        angle = degrees90 - (degrees06 * minsecs);
        drawHand(angle, diameter / 3 + 10);//, Color.black);
 
        float hourmins = (hour + minsecs / 60.0f);
        angle = degrees90 - (degrees30 * hourmins);
        drawHand(angle, diameter / 4 + 10);//, Color.black);

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Clock_Dial");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

