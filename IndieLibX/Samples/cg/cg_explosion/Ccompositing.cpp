#if defined(WIN32)
# include <windows.h>
#elif defined(UNIX)
# include <GL/glx.h>
#endif

#include <Cg/cgGL.h>

#include <glh/glh_extensions.h>
#include <glh/glh_obs.h>
#include <glh/glh_array.h>

#include <shared/read_text_file.h>
#include <shared/array_texture.h>

#include <math.h>

#include <vector>
#include <string>

#include "CCompositing.h"
#include "glError.h"

extern data_path progpath;

#if defined(WIN32)

#define CHECKCGERROR() \
    Ret = cgGetError();\
    if(Ret != CG_NO_ERROR)\
    {\
        LOGMSG(LOG_ERR,"CgError : %s", cgGetErrorString(Ret));\
    }
#elif defined(UNIX) || defined(MACOS)
    
#define CHECKCGERROR() \
    Ret = cgGetError();\
    if(Ret != CG_NO_ERROR)\
    {\
        fprintf(stderr,"CgError : %s", cgGetErrorString(Ret));\
    }
#endif

#if defined(MACOS)
#define GL_TEXTURE_RECTANGLE_NV GL_TEXTURE_RECTANGLE_EXT
#endif

/*----------------------------------------------------------------------------------*/ /**



**/ //----------------------------------------------------------------------------------
CCompositing::CCompositing() :
  binit(false),
  Context(NULL),
  fragmentProg(NULL),
	pbuffer1(NULL),
	pbuffer2(NULL),
	scale(10),
	width(0), height(0)
  {
  }
/*----------------------------------------------------------------------------------*/ /**



**/ //----------------------------------------------------------------------------------
CCompositing::~CCompositing()
{
}
/*----------------------------------------------------------------------------------*/ /**



**/ //----------------------------------------------------------------------------------
void CCompositing::reshape(int w, int h)
{
	width = w;
	height = h;
	if(pbuffer1)
		delete pbuffer1;
	pbuffer1 = new PBuffer("rgb alpha depth textureRECT");
	pbuffer1->Initialize(w, h, false, true);
	if(pbuffer2)
		delete pbuffer2;
	pbuffer2 = new PBuffer("rgb textureRECT"); // we cannot have a pbuffer with rg only
	pbuffer2->Initialize(w/4, h/4, false, true);

	pbuffer1->Activate();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	pbuffer1->Deactivate();
	pbuffer2->Activate();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	pbuffer2->Deactivate();

	texture1.bind();
    texture1.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    texture1.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	texture2.bind();
    texture2.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    texture2.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}
/*----------------------------------------------------------------------------------*/ /**



**/ //----------------------------------------------------------------------------------
bool CCompositing::init()
{
    char *cgProg;
	char *fpfname;
    CGenum fptype;

    fpfname = "cg_compositing_fragment.cg";
    fptype = CG_SOURCE;

    //
    // Resource type for PNG and vertex progs. This is OPTIONAL. No conflict with linux...
    //
    unsigned int hModule = 0;
#ifdef WIN32
    hModule = (unsigned int)GetModuleHandle("cg_explosion.dll");
    set_module_handle(hModule);
    set_module_restypename("NV");
#endif
    //
    // check for extensions
    //
    if (!glh_init_extensions("GL_VERSION_1_4 "
								"WGL_ARB_pbuffer "
								"GL_NV_texture_rectangle "
								"WGL_ARB_pixel_format "
								"WGL_ARB_render_texture "))
    {
        LOGMSG(LOG_ERR, "need this extension(s): %s", glh_get_unsupported_extensions());
        return false;
    }
	reshape(800, 600);
    //
    // Cg Prog
    //
    if(fragmentProg)
        cgDestroyProgram(fragmentProg);
    if(Context) 
        cgDestroyContext(Context);

    Context = cgCreateContext();
    assert(Context != NULL);
    //
    // Get the latest program available
    //
    fragmentprofile = CG_PROFILE_ARBFP1;//cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgGLSetOptimalOptions(fragmentprofile);
    LOGMSG(LOG_MSG, "using fragment profile %s for Compositing", cgGetProfileString(fragmentprofile));

    if((fragmentprofile == CG_PROFILE_UNKNOWN))
    {
        LOGMSG(LOG_WARN, "profiles are missing !");
        return false;
    }

    //
    // load the fragment program
    //
    LOGMSG(LOG_STATE, "Loading %s", fpfname);
    cgProg = read_text_file(fpfname, "NV", hModule);
    if(cgProg == NULL)
    {
        LOGMSG(LOG_ERR, "failed to load %s...", fpfname);
        return false;
    }
    fragmentProg = cgCreateProgram(Context, fptype, cgProg, fragmentprofile, "main", NULL);
	delete [] cgProg;
    if(fragmentProg == NULL)
    {
        LOGMSG(LOG_ERR, "failed create the program cgCreateProgram(%s)", cgGetProfileString(fragmentprofile));
        LOGMSG(LOG_MSG, cgGetLastListing(Context));
        return false;
    }
    cgGLLoadProgram(fragmentProg);

    param_Scale = cgGetNamedParameter(fragmentProg, "scaling");
    if(param_Scale == NULL) 
    { 
        LOGMSG(LOG_WARN, "failed to find Scale..."); 
    }
    else
    {
        cgGLSetParameter1f(param_Scale, scale);
    }

    //
    // texture 1
    //
    param_Tex1 = cgGetNamedParameter(fragmentProg, "Texture1");
    if(param_Tex1 == NULL) LOGMSG(LOG_WARN, "failed to find Texture1...");
    cgGLEnableTextureParameter(param_Tex1); // Very important to allow Cg to setup the units
    cgGLSetTextureParameter(param_Tex1, texture1.texture);
	texture1.bind();
    texture1.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    texture1.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    CHECKGLERRORS();
    //
    // texture 2
    //
    param_Tex2 = cgGetNamedParameter(fragmentProg, "Texture2");
    if(param_Tex2 == NULL) LOGMSG(LOG_WARN, "failed to find Texture2...");
    cgGLEnableTextureParameter(param_Tex2); // Very important to allow Cg to setup the units
    cgGLSetTextureParameter(param_Tex2, texture2.texture);
	texture2.bind();
    texture2.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    texture2.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    CHECKGLERRORS();

	pbuffer1->Activate();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	pbuffer1->Deactivate();
	binit = true;
    return 1;
}
/*----------------------------------------------------------------------------------*/ /**



**/ //----------------------------------------------------------------------------------
void CCompositing::finish()
{
    if(!binit)
        return;
    if(fragmentProg)
        cgDestroyProgram(fragmentProg);
    if(Context) 
        cgDestroyContext(Context);
	fragmentProg = NULL;
	Context = NULL;
}
/*----------------------------------------------------------------------------------*/ /**

Compositing drawing

**/ //----------------------------------------------------------------------------------
void CCompositing::draw()
{
    cgGLEnableProfile(fragmentprofile);
    cgGLBindProgram(fragmentProg);

	glViewport(0,0,width,height);
/*	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);*/
	glDisable(GL_BLEND);
	/*glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_3D);
	glActiveTextureARB(GL_TEXTURE0_ARB);*/
    glActiveTexture( GL_TEXTURE1 );
	texture2.enable();
	texture2.bind();
	pbuffer2->Bind(WGL_FRONT_LEFT_ARB);
    glActiveTexture( GL_TEXTURE0 );
	texture1.enable();
	texture1.bind();
	pbuffer1->Bind(WGL_FRONT_LEFT_ARB);

    if(param_Scale) 
        cgGLSetParameter1f(param_Scale, scale);
	cgGLSetTextureParameter(param_Tex1, texture1.texture);
	cgGLSetTextureParameter(param_Tex2, texture2.texture);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);

	glBegin(GL_QUADS);
	glColor4f(1,1,1,1);
	glMultiTexCoord2f(0, 0,0);
	glMultiTexCoord2f(1, 0,0);
	glVertex2f(0,0);
	glMultiTexCoord2f(0, width,0);
	glMultiTexCoord2f(1, width/4,0);
	glVertex2f(1,0);
	glMultiTexCoord2f(0, width,height);
	glMultiTexCoord2f(1, width/4,height/4);
	glVertex2f(1,1);
	glMultiTexCoord2f(0, 0,height);
	glMultiTexCoord2f(1, 0,height/4);
	glVertex2f(0,1);
	glEnd();
	glEnable(GL_DEPTH_TEST);

    glActiveTexture( GL_TEXTURE1 );
	pbuffer2->Release(WGL_FRONT_LEFT_ARB);
    glActiveTexture( GL_TEXTURE0 );
	pbuffer1->Release(WGL_FRONT_LEFT_ARB);
    cgGLDisableProfile(fragmentprofile); // same as     glBindProgramNV(GL_FRAGMENT_PROGRAM_NV, 0); ???
}
