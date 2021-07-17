#ifndef __GLERROR__
#define __GLERROR__

#ifdef MACOS
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#define DOCHECKGLERRORS

#include <nv_GenericTools/nv_GenericTools.h>


#ifdef DOCHECKGLERRORS
#define CHECKGLERRORS() check_ogl_error(__FILE__, __LINE__)
#else
#define CHECKGLERRORS()
#endif

inline void check_ogl_error(char *fname, int line)
{
    char tmp[200];
    GLenum e = glGetError();
    if(GL_NO_ERROR == e)
        return;
    sprintf(tmp, "%s\n", gluErrorString(e));
    
    fprintf(stderr, tmp);
    char tmp2[200];
    sprintf(tmp2, "(%s) : %d %s", fname, line, tmp);
    LOGMSG(LOG_ERR, tmp2);
#if defined(WIN32)
    OutputDebugString(tmp2);
#endif
}
#endif
