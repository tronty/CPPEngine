#include "SceneFrame.h"

SceneFrame* g_SceneFrame=0;
unsigned int MVPTranspose=1;
unsigned int MVPInvert=0;
	#if 0
unsigned int gDrawWater=0;
unsigned int gDrawSky=1;
unsigned int gDrawGrass=1;
unsigned int gDrawTerrain=1;
#endif
int init(const char* aTitle)
{
	//LOG_FNLN;
  	if(!g_SceneFrame->initialize())
    		return -1;
	//LOG_FNLN;
	//stx_exit(0);
	return 0;
}

void render()
{
	#if 0
	const char* MVPTransposetxt = "MVPTranspose";
	const char* MVPInverttxt = "";
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1))
	{
		MVPTranspose^=1;
		if(MVPTranspose)
			MVPTransposetxt = "MVPTranspose";
		else
			MVPTransposetxt = "";
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2))
	{
		MVPInvert^=1;
		if(MVPInvert)
			MVPInverttxt = "MVPInvert";
		else
			MVPInverttxt = "";
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F3))
	{
		gDrawWater^=1;
		g_SceneFrame->renderWater=gDrawWater;
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F4))
	{
		gDrawSky^=1;
		g_SceneFrame->renderSky=gDrawSky;
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F5))
	{
		gDrawGrass^=1;
		g_SceneFrame->terrain.setDrawGrassFlag(gDrawGrass);
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F6))
	{
		gDrawTerrain^=1;
		g_SceneFrame->terrain.setDrawTerrainFlag(gDrawGrass);
	}
	#endif
	//float f=1.0f/256.0f;
	//D3DXFROMWINEVECTOR4 color(64.0f, 147.0f, 198.0f, 256.0f);
	//D3DXFROMWINEVECTOR4 color(116.0f, 162.0f, 195.0f, 256.0f);
	//D3DXFROMWINEVECTOR4 color(g_SceneFrame->skyInfo.x, g_SceneFrame->skyInfo.y, g_SceneFrame->skyInfo.z, 1.0f);
	IRenderer::GetRendererInstance()->Clear(true, true, g_SceneFrame->skyInfo);

	//LOG_PRINT("2:BeginScene\n");
	IRenderer::GetRendererInstance()->BeginScene();
	//LOG_FNLN;
	g_SceneFrame->run();
	//LOG_FNLN;
	//LOG_PRINT("EndScene\n");
	#if 0
		IRenderer::GetRendererInstance()->drawText(MVPTransposetxt, 10, 10, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		IRenderer::GetRendererInstance()->drawText(MVPInverttxt, 10, 30, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	#endif
		const char* txt = "F1-F4 key";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 90, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	//LOG_PRINT("2:EndScene\n");
	IRenderer::GetRendererInstance()->EndScene();
	//LOG_PRINT("2:Present\n");
	IRenderer::GetRendererInstance()->Present( );
	//stx_exit(0);
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("NatureScene2");	
	IInput*    i=STX_Service::GetInputInstance();
	g_SceneFrame=new SceneFrame();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
/*
IRenderer::OnPresent:Title:Framework3 - NatureScene2 - GLSL_1_1 - 64 bit
m_nBeginScene=3
m_nEndScene=2
*/

