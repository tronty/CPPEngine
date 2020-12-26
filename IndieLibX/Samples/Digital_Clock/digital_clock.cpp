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

ShaderID shd=-1;
VertexFormatID vf=-1;

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Digital_Clock/digital_clock.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
	//		   x	 y
	float v[8] =  {   1.0f,-1.0f,
			  1.0f, 1.0f,
			 -1.0f,-1.0f,
			 -1.0f, 1.0f  };
	IRenderer::GetRendererInstance()->setShader(shd);
	struct tm *curtime = getTime();

	if (curtime->tm_hour > 12)
	    curtime->tm_hour -= 12;
	if (curtime->tm_hour == 0)
	    curtime->tm_hour = 12;
	IRenderer::GetRendererInstance()->setShaderConstant1f("hour", float(curtime->tm_hour));
	IRenderer::GetRendererInstance()->setShaderConstant1f("minute", float(curtime->tm_min));
	IRenderer::GetRendererInstance()->setShaderConstant1f("second", float(curtime->tm_sec));
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 2*sizeof(float));
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Digital_Clock");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

