/****************************************************************/ /**
 **
 **
 **/ /****************************************************************/
/** \file cg_CCompositing.h **/
#ifndef __CCompositing__
#define __CCompositing__

#if defined(WIN32)
#  pragma warning (disable:4786)
#endif

#include <glh/glh_linear.h>

#include <glh/glh_linear.h>
#include <gl/glut.h>
#include <shared/data_path.h>
# include <shared/pbuffer.h>
//#include <nv_rtmodules/modhelper.h>
#include <nv_GenericTools/nv_GenericTools.h>


#include "noise_ex.h"

#define TEX_SZ 256

/**
 ** The class container.
 **/
class CCompositing
{
public:

private:
    bool binit;
    /// \name Cg variables
    //@{
    CGcontext Context;
    CGprogram fragmentProg;
    CGprofile   fragmentprofile;

    CGerror Ret;
    //@}
    /// \name params for fragment program
    //@{
    CGparameter param_Scale;
    CGparameter param_Tex1;
    CGparameter param_Tex2;
    //@}
public:
    CCompositing();
    ~CCompositing();
	// NO "delete this" for this smart reference since we know this object is staticaly declared
    const char *get_error_msg();
	void reshape(int w, int h);
    bool init();
    void finish();
    void draw();

    float scale;

	int width,height;
	PBuffer *pbuffer1;
	PBuffer *pbuffer2;
	glh::tex_object_rectangle texture1;
	glh::tex_object_rectangle texture2;

};

#endif
