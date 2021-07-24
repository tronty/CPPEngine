/*****************************************************************************************
 * Desc: IndieLib singleton initialization class
 *****************************************************************************************/
#define __ImageLib__ 1

#include <CIndieLib.h>
//using namespace CG;

/* === Windows === */
//include "TCHAR.h"

/*
==================
Init singleton
==================
*/
CIndieLib* CIndieLib::pinstance = 0;// initialize pointer
CIndieLib* CIndieLib::Instance ()
{
	if (pinstance == 0)  // is it the first call?
	{
		pinstance = new CIndieLib; // create sole instance
	}
	return pinstance; // address of sole instance
}

/*
==================
Sets working path to the directory of exe file (Windows)
==================
*/
void CIndieLib::ResetCurrentDirectory_W( void )
{
}


/*
==================
Init IndieLib
==================
*/
bool CIndieLib::init=true;
std::string CIndieLib::m_title;
bool CIndieLib::Init(const char* title, tRenderer rtb)
{
	int argc=0;
	char **argv=0;
	if(!init) return true;
	init=false;
	
	STX_PRINT("CIndieLib::Init::title=%s\n", title);
	m_title=title;
	ResetCurrentDirectory_W();

	// IndieLib Initialization, a debug.log file will be created.
	IndieLib::Init (IND_DEBUG_MODE);

	Window			=	new		IND_Window;
	Render			=	new		IND_Render;

#if defined( BUILD_MESHRENDERER) 
	LightManager		=	new		IND_LightManager;
#endif
	ImageManager		=	new		IND_ImageManager;
	SurfaceManager		=	new		IND_SurfaceManager;
#if defined( BUILD_MESHRENDERER) 
	MeshManager		=	new		IND_3dMeshManager;
#endif
	AnimationManager	=	new		IND_AnimationManager;
	FontManager		=	new		IND_FontManager;
	Entity2dManager		=	new		IND_Entity2dManager;
#if defined( BUILD_MESHRENDERER) 
	Entity3dManager		=	new		IND_Entity3dManager;
#endif
	Math			=	new		IND_Math;


#if defined( BUILD_MESHRENDERER) 
	assert(MeshManager);
#endif
	assert(Window);
	assert(Render);
#if defined( BUILD_MESHRENDERER) 
	assert(LightManager);
#endif
	assert(ImageManager);
	assert(SurfaceManager);
	assert(AnimationManager);
	assert(FontManager);
	assert(Entity2dManager);
#if defined( BUILD_MESHRENDERER) 
	assert(Entity3dManager);
#endif
	assert(Math);

	if (!Window		->Init (m_title.c_str(), 800, 600, 32, 0, 0))					return 0;
	
	//LOG_PRINT("ST:CIndieLib::Init\n");
	if (!Render		->Init (Window,rtb, argc,argv))
		return 0;

	Input			=   STX_Service::GetInputInstance();

	assert(Input);

#if defined( BUILD_MESHRENDERER) 
	if (!LightManager	->Init(Render))									return 0;
#endif
	
	if (!ImageManager	->Init ())										return 0;
	
	if (!SurfaceManager	->Init (ImageManager, Render))			return 0;
	
	if (!AnimationManager	->Init (ImageManager, SurfaceManager))		return 0;
	
	if (!FontManager	->Init (ImageManager, SurfaceManager))		return 0;
	
	if (!Entity2dManager	->Init (Render))									return 0;

#if defined( BUILD_MESHRENDERER) 
	if (!Entity3dManager	->Init (Render))									return 0;

	if (!MeshManager	->Init (Render))									return 0;

#endif
	//if (!Input		->Init (Render))									return 0;

	if (!Math		->Init ())										return 0;


	m_title=title;

	char buf[1024] = {0};
	stx_strlcpy(buf, "IndieLibX - ", 1024);
	stx_strlcat(buf, m_title.c_str(), 1024);
	stx_strlcat(buf, " - ", 1024);
	stx_strlcat(buf, IRenderer::GetRendererInstance()->getString(), 1024);

	if(sizeof(void *) == 4)
		stx_strlcat(buf, " - 32 bit", 1024);
	else if(sizeof(void *) == 8)
		stx_strlcat(buf, " - 64 bit", 1024);

	STX_Service::GetWindowInstance()->SetCaption(buf);

	GetDebugAPI()->Header	("Renderer:", 3);
	GetDebugAPI()->DataChar (buf, 0);

	return 1;
}

	void CIndieLib::SaveScreenShot(const char* fn){
	if(
		IRenderer::GetSDL()&&
		CIndieLib::Instance()->Input&&
		(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_x)))
	{
		char buf[256];
		char buft[256];
		if(!fn)
			stx_snprintf(buft,256,m_title.c_str());
		else
			stx_snprintf(buft,256,fn);

		stx_snprintf(buf,256,"%s - %s",IRenderer::GetRendererInstance()->getString(),buft);

	IRenderer::GetRendererInstance()->SaveScreenShot(buf);
	}
	}
/*
==================
Free Indielib managers
==================
*/
void CIndieLib::End()
{
	STX_TRY;
	// ----- Freeing objects -----
;
	Math				->End();
	;
#if defined( BUILD_MESHRENDERER) 
	MeshManager			->End();
	;
	Entity3dManager		->End();
#endif
	;
//	Input				->End();
	Entity2dManager		->End();
	;
	FontManager			->End();
	;
	AnimationManager	->End();
	;
	SurfaceManager		->End();
	;
	ImageManager		->End();
	;
#if defined( BUILD_MESHRENDERER) 
	LightManager		->End();
#endif
	;
	Render				->End();
	;
	Window				->End();
;
	delete Math;
	;
	//delete Input;
	;
	delete Entity2dManager;
	;
#if defined( BUILD_MESHRENDERER) 
	delete MeshManager;
	;
	delete Entity3dManager;
#endif
	;
	delete FontManager;
	;
	delete AnimationManager;
	;
	delete SurfaceManager;
	;
	delete ImageManager;
	;
#if defined( BUILD_MESHRENDERER) 
	delete LightManager;
#endif
	;
	//LOG_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	;
	delete Window;
;
	//LOG_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	IndieLib::End ();
;
	//LOG_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	delete pinstance;
	//LOG_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	pinstance = 0;
;
	//LOG_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	STX_Service::CloseAll();
;
	//LOG_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	stx_closeall();
	//LOG_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	//???delete Render;
	//STX_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	;
	STX_CATCH;

#ifdef _MSC_VER
_CrtDumpMemoryLeaks();
#endif
}

