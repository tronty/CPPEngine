/*****************************************************************************************
 * Desc: IndieLib singleton initialization class
 *****************************************************************************************/

#ifndef _CINDIELIB_
#define _CINDIELIB_

#ifdef SWIG
#define INDIELIBAPIENTRY
#define INDIELIBAPI
#endif

#include "Indie.h"

class INDIELIBAPI CIndieLib
{
public:
	static CIndieLib* Instance();
#ifndef SWIG
	static bool init;//=true;
	static std::string m_title;//[1024];
#endif
	void SaveScreenShot(const char* fn=0);

	bool Init (const char* title="", tRenderer rtb=eGLGLSL_1_1);
	void End ();

	// ----- IndieLib objects -----
#if defined( BUILD_MESHRENDERER) 
	IND_3dMeshManager		*MeshManager;
#endif
	IInput				*Input;
	IND_Window				*Window;
	IND_Render				*Render;
#if defined( BUILD_MESHRENDERER) 
	IND_LightManager		*LightManager;
#endif
	IND_ImageManager		*ImageManager;
	IND_SurfaceManager		*SurfaceManager;
	IND_AnimationManager	*AnimationManager;
	IND_FontManager			*FontManager;
	IND_Entity2dManager		*Entity2dManager;
#if defined( BUILD_MESHRENDERER) 
	IND_Entity3dManager		*Entity3dManager;
#endif
	IND_Math				*Math;

protected:

	CIndieLib()  {
		Input=0;
	}
#ifndef SWIG
	CIndieLib(const CIndieLib&);
	CIndieLib& operator = (const CIndieLib&);
public:
		static CIndieLib *pinstance;
#endif
private:

	void ResetCurrentDirectory_W( void );
};
#endif // _CINDIELIB_

