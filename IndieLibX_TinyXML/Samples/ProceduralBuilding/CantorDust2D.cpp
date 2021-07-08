/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

void CantorDust(D3DXFROMWINEVECTOR2 aPosition, D3DXFROMWINEVECTOR2 aSize)
{
	if(aSize.x < 0.005)
		return;

	 D3DXFROMWINEVECTOR2 size(aSize.x/3.0,aSize.y);
	{D3DXFROMWINEVECTOR2 position(aPosition.x, aPosition.y+aSize.y);
	 CantorDust(position, size);}
	{D3DXFROMWINEVECTOR2 position(aPosition.x+2.0*aSize.x/3.0, aPosition.y+aSize.y);
	 CantorDust(position, size);}

	STX_Rect rekt;
	rekt.x = aPosition.x	*	IRenderer::GetRendererInstance()->GetViewportWidth();
	rekt.y = aPosition.y	*	IRenderer::GetRendererInstance()->GetViewportHeight();
	rekt.w = aSize.x	*	IRenderer::GetRendererInstance()->GetViewportWidth();
	rekt.h = aSize.y	*	IRenderer::GetRendererInstance()->GetViewportHeight();
	STX_RenderFillRect(rekt);
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

	D3DXFROMWINEVECTOR2 aPosition(0.0f, 0.0f);
	D3DXFROMWINEVECTOR2 aSize(1.0f, 0.1f);
	CantorDust(aPosition, aSize);

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("CantorDust2D");
	IInput* in=STX_Service::GetInputInstance();
	init("");
	while (!in->OnKeyPress (STX_KEY_ESCAPE) && !in->Quit())
	{
		in->Update();
		render();
	}
	return 0;
}

