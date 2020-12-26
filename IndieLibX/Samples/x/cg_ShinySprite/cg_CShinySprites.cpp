#if defined(WIN32)
# include <windows.h>
#elif defined(UNIX)
# include <GL/glx.h>
#endif

#define GLH_EXT_SINGLE_FILE

#include <Cg/cgGL.h>

#include <glh/glh_extensions.h>
#include <glh/glh_obs.h>
#include <glh/glh_array.h>

#include <shared/nv_png.h>
#include <shared/read_text_file.h>
#include <shared/array_texture.h>

#include <nvparse/nvparse.h>
#include <math.h>

#include <vector>
#include <string>

#include <GL/glu.h>

using namespace std;

#include "cg_CShinySprites.h"

#if defined(WIN32)
#define EXTENSIONS "GL_NV_texture_shader " \
                   "GL_NV_register_combiners " \
                   "WGL_ARB_pbuffer " \
                   "WGL_ARB_pixel_format "
#elif defined(UNIX)
#define EXTENSIONS "GL_NV_texture_shader " \
                   "GL_NV_register_combiners " \
                   "GLX_SGIX_pbuffer " \
                   "GLX_SGIX_fbconfig "
#endif

static string error_msg;

extern void cleanExit(int exitval);

const char * CSSprite::get_error_msg()
{
    return error_msg.c_str();
}

CSSprite::CSSprite(bool managed) :
  m_PBuffer(NULL),
  m_inpbuffer(false),
  m_bNoRenderTexture(true),
  binit(false),
  dl_bind_vtxprg(0),
  m_bManaged(managed),
  Context(0),
  Program(0)
{}

CSSprite::~CSSprite()
{
    if (m_bManaged)
        free();
}
void CSSprite::set_test_cst(GLfloat f1, GLfloat f2)
{
    //glProgramParameter4fNV(GL_VERTEX_PROGRAM_NV, 22, f1,f2,0,0);
}
/**
 ** Init for opengl
 **/
bool CSSprite::init()
{
    data_path media;
    char * const * errors;
    glh::tex_object_2D tex;

        if(binit)
        return 1;
    //
    // Resource type for PNG and vertex progs. This is OPTIONAL. No conflict with linux...
    //
    unsigned int hModule = 0;
#ifdef WIN32
    hModule = (unsigned int)GetModuleHandle("cg_ShinySprites.dll");
    set_png_module_handle(hModule);
    set_png_module_restypename("PNG");
    set_module_handle(hModule);
    set_module_restypename("NV");
#endif
    free();

    //
    // check for extensions : some are need
    //
    if (!glh_init_extensions(EXTENSIONS))
    {
        error_msg += "need this extension(s): ";
        error_msg += glh_get_unsupported_extensions();
        return false;
    }
    //
    // check for extensions : some are optional
    //
    if (!glh_init_extensions("WGL_ARB_render_texture"))
        m_bNoRenderTexture = true;
    else
        m_bNoRenderTexture = false;

    glh_init_extensions("GL_ARB_multitexture");

    //
    // For Cg : Use GL_ARB_vertex_program extension if supported by video card
    //
    if (cgGLIsProfileSupported(CG_PROFILE_ARBVP1))
        Profile = CG_PROFILE_ARBVP1;
    else if (cgGLIsProfileSupported(CG_PROFILE_VP20))
        Profile = CG_PROFILE_VP20;
    else
    {
        error_msg = "Video card does not support GL_ARB_vertex_program or GL_NV_vertex_program.\n";
        return false;
    }
    
    //
    //----> create the pbuffer :
    //
    if(!m_PBuffer)
    {
        if(m_bNoRenderTexture)
            m_PBuffer = new PBuffer("rgb depth");
        else
            m_PBuffer = new PBuffer("rgb depth texture");
        m_PBuffer->Initialize(TEX_SZ, TEX_SZ, false, true);
    }
    //
    //----> Initialise pbuffer stuff
    //
    m_PBuffer->Activate();
        glClearColor(0.5, 0, 1, 1.0);
        glEnable(GL_DEPTH_TEST); // need to set it. Not by default
    m_PBuffer->Deactivate();
    //
    //----> Create the depth map
    //
    if (glActiveTextureARB) 
    {
        glActiveTextureARB( GL_TEXTURE1_ARB );
        m_occlusionmap.enable();
        m_occlusionmap.bind();
        m_occlusionmap.parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        m_occlusionmap.parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        m_occlusionmap.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        m_occlusionmap.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        // If no render to texture, create a map of same size than the pbuffer
        if(m_bNoRenderTexture)
        {
            glTexImage2D(m_occlusionmap.target, 0, GL_RGB, TEX_SZ, TEX_SZ, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, (const void *)NULL);
        }
        glActiveTextureARB( GL_TEXTURE0_ARB );
    }
    //
    // load the default texture
    //
    texture.bind();
    glh::array2<glh::vec3ub> img;
    read_png_rgb("shine.png", img);
    make_rgb_texture( img, false);//true );
    texture.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    texture.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    //
    // Cg Prog
    //

    // Load the program. Optionaly have a look in the resources
    char *cgProg = read_text_file("cg_ShinySprite/cg_ShinySprite.cg", "NV", hModule);
    if (!cgProg)
    {
        STX_PRINT("Unable to load cg_ShinySprite.cg, exiting...\n");
        cleanExit(0);
    }

    Context = cgCreateContext();

    Program = cgCreateProgram(Context,
                              CG_SOURCE, cgProg,
                              Profile, NULL, NULL);

    cgGLLoadProgram(Program);
    
    Projection = cgGetNamedParameter(Program, "Projection");
    ModelView = cgGetNamedParameter(Program, "ModelView");
    ModelViewIT = cgGetNamedParameter(Program, "ModelViewIT");
    
    Light = cgGetNamedParameter(Program, "Light");
    Shininess = cgGetNamedParameter(Program, "Shininess");
    Slices = cgGetNamedParameter(Program, "Slices");

    Size = cgGetNamedParameter(Program, "IN.Size");

    CSTable = cgGetNamedParameter(Program, "CSTable");

    if (!Projection || !ModelView || !ModelViewIT || !Light || !Shininess || !Slices || !Size || !CSTable)
    {
        STX_PRINT("Unable to retrieve vertex program parameters, exiting...\n");
        cleanExit(0);
    }

    //
    // create the parameters-binding display-list
    //
    dl_bind_vtxprg = glGenLists(1);
    glNewList(dl_bind_vtxprg, GL_COMPILE);
    if(glActiveTextureARB)
    {
        nvparse(
            "!!RC1.0                                           \n"
            "{ \n"
            "	rgb { \n"
            "		spare0 = tex1.rgb * tex0.rgb; \n"
            "	} \n"
            "} \n"
            "out.rgb = spare0.rgb;                               \n"
            "out.a = tex0.a;                                 \n", 0
        );
    }
    else
    {
        nvparse(
            "!!RC1.0                                           \n"
            "out.rgb = tex0.rgb;                               \n"
            "out.a = tex0.a;                                 \n", 0
        );
    }
    for (errors = nvparse_get_errors(); *errors; errors++)
    {
        fprintf(stderr, *errors);
    }
    glEnable(GL_REGISTER_COMBINERS_NV);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    if(glActiveTextureARB)
        glActiveTextureARB( GL_TEXTURE0_ARB );
    glEnable(GL_TEXTURE_2D);
    texture.bind();
    nvparse(
        "!!TS1.0                                           \n"
        "texture_2d();                                            \n"
        "texture_2d();                                            \n"
        "nop();                                            \n"
        "nop();                                     \n", 0
    );
    for (errors = nvparse_get_errors(); *errors; errors++)
    {
        fprintf(stderr, *errors);
    }
    glEnable(GL_TEXTURE_SHADER_NV);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glEndList();
    //
    // create the parameters-binding display-list for pbuffer version
    //
    dl_bind_vtxprg_pbuffer = glGenLists(1);
    glNewList(dl_bind_vtxprg_pbuffer, GL_COMPILE);
    nvparse(
        "!!RC1.0                                           \n"
        "out.rgb = col0.rgb;                               \n"
        "out.a = col0.a;                                 \n", 0
    );
    for (errors = nvparse_get_errors(); *errors; errors++)
    {
        fprintf(stderr, *errors);
    }
    glEnable(GL_REGISTER_COMBINERS_NV);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    if(glActiveTextureARB)
    {
        glActiveTextureARB( GL_TEXTURE1_ARB );
        glDisable(GL_TEXTURE_2D);
        glActiveTextureARB( GL_TEXTURE0_ARB );
    }
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_SHADER_NV);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEndList();
    return 1;
}
/**
 ** free
 **/
void CSSprite::finish()
{
    if(!binit)
        return;
    if(dl_bind_vtxprg)
        glDeleteLists(dl_bind_vtxprg,1);
}
/**
 ** Setup OpenGL for drawing.
 ** can be used when calling draw_sprite().
 ** is used by the render() method.
 **/
void CSSprite::begin()
{
    glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|GL_LIGHTING_BIT|GL_POLYGON_BIT);
    cgGLBindProgram(Program);
    cgGLEnableProfile(Profile);

    if(m_inpbuffer)
    {
        glCallList(dl_bind_vtxprg_pbuffer);
    }
    else
    {
        glCallList(dl_bind_vtxprg);
        if(glActiveTextureARB)
        {
            glActiveTextureARB( GL_TEXTURE1_ARB );
            m_occlusionmap.enable();
            m_occlusionmap.bind();
#ifdef WIN32            
            if(!m_bNoRenderTexture) m_PBuffer->Bind(WGL_FRONT_LEFT_ARB);
#endif
        }
    }

    cgGLSetStateMatrixParameter(Projection, CG_GL_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(ModelView, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(ModelViewIT, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);

    //====> params for table N.L from cos(0) to cos(90):
    #define C(x) (float)cos(x*3.141592654f/180.0f)
    #define S(x) (float)sin(x*3.141592654f/180.0f)

    CGparameter curParam = 0;

#define TABLE(a) curParam = cgGetArrayParameter(CSTable, reg++); \
                 cgGLSetParameter4f(curParam, C(a), S(a), 0, 0);
    int reg = 0;
#if 0
    TABLE(0)    // ALWAYS 0 
    TABLE(0)    // 86
    TABLE(50)   // 84.2
    TABLE(60)   // 82.3
    TABLE(50)   // 80.4
    TABLE(40)   // 78.5
    TABLE(68)   // 76.5
    TABLE(40)   // 74.5
    TABLE(0)    // 72.5
    TABLE(1)    // 70.5
    TABLE(5)    // 68.5
    TABLE(55.4) // 66.4
    TABLE(5)    // 64.3
    TABLE(1)    // 62.1
    TABLE(20)   // 60
    TABLE(40)   // 57.7
    TABLE(30)   // 55.4
    TABLE(2)    // 53.1
    TABLE(0)    // 50.7
    TABLE(0)    // 48.2
    TABLE(20)   // 45.5
    TABLE(0)    // 42.8
    TABLE(10)   // 39.9
    TABLE(20)   // 36.8
    TABLE(10)   // 33.5
    TABLE(5)    // 29.9
    TABLE(20)   // 25.8
    TABLE(5)    // 21
    TABLE(0)    // 14.8
    TABLE(0)    // 0
#else
    TABLE(0)    // ALWAYS 0 
    TABLE(0)    // 86
    TABLE(0)    // 84.2
    TABLE(0)    // 82.3
    TABLE(0)    // 80.4
    TABLE(0)    // 78.5
    TABLE(60)   // 76.5
    TABLE(64)   // 74.5
    TABLE(55)   // 72.5
    TABLE(0)    // 70.5
    TABLE(0)    // 68.5
    TABLE(0)    // 66.4
    TABLE(0)    // 64.3
    TABLE(20)   // 62.1
    TABLE(0)    // 60
    TABLE(20)   // 57.7
    TABLE(0)    // 55.4
    TABLE(0)    // 53.1
    TABLE(0)    // 50.7
    TABLE(28)   // 48.2
    TABLE(35)   // 45.5
    TABLE(20)   // 42.8
    TABLE(0)    // 39.9
    TABLE(0)    // 36.8
    TABLE(0)    // 33.5
    TABLE(0)    // 29.9
    TABLE(0)    // 25.8
    TABLE(10)   // 21
    TABLE(0)    // 14.8
    TABLE(0)    // 0
#endif
    cgGLSetParameter4f(Slices, (float)reg, 0, 1, 0);

    glBegin(GL_QUADS);
}
void CSSprite::end()
{
    glEnd();
    glPopAttrib();
    if(m_inpbuffer)
    {
        //return;
    }
    else
    {
#ifdef WIN32
        if(glActiveTextureARB)
            if(!m_bNoRenderTexture) m_PBuffer->Release(WGL_FRONT_LEFT_ARB);
#endif          
    }
    if(glActiveTextureARB)
    {
        glActiveTextureARB( GL_TEXTURE1_ARB );
        m_occlusionmap.disable();
        glActiveTextureARB( GL_TEXTURE0_ARB );
    }
    cgGLDisableProfile(Profile);
}
/**
 ** Draw a sprite
 ** uses 4 vertex
 **/
void CSSprite::draw_sprite( glh::vec3f &pos, glh::vec3f &norm, 
            GLfloat MinSz, GLfloat MaxSz)
{
    //glColor4f(1,1,1,1); // Cg don't need it. if used : crashing Cg runtime...

    cgGLSetParameter4f(Size, MinSz, MinSz, MaxSz, MaxSz);
    glTexCoord2f(1, 1);
    glNormal3fv(&(norm[0]));
    glVertex3fv(&(pos[0]));

    cgGLSetParameter4f(Size, -MinSz, MinSz, -MaxSz, MaxSz);
    glTexCoord2f(0, 1);
    glNormal3fv(&(norm[0]));
    glVertex3fv(&(pos[0]));

    cgGLSetParameter4f(Size, -MinSz, -MinSz, -MaxSz, -MaxSz);
    glTexCoord2f(0, 0);
    glNormal3fv(&(norm[0]));
    glVertex3fv(&(pos[0]));

    cgGLSetParameter4f(Size, MinSz, -MinSz, MaxSz, -MaxSz);
    glTexCoord2f(1, 0);
    glNormal3fv(&(norm[0]));
    glVertex3fv(&(pos[0]));
}
/**
 **
 **/
void CSSprite::bind_light(GLfloat *l, GLfloat shine)    
{
    cgGLSetParameter4fv(Light, l);
    cgGLSetParameter4f(Shininess, shine, shine, shine, shine);
}
/************************************************************************/ /**
 * Start PBuffer job
 */ /************************************************************************/
void CSSprite::begin_occlusion(bool bDebug)
{
    float mview[16], mproj[16];
    // Because PBuffer is in another context : must find back matrix
    glGetFloatv(GL_MODELVIEW_MATRIX, mview);
    glGetFloatv(GL_PROJECTION_MATRIX, mproj);

    m_inpbuffer = true;
    //
    // go into the PBuffer :
    //
    if(!bDebug)
    {
        m_PBuffer->Activate();
        glViewport(0, 0, TEX_SZ, TEX_SZ);
    }
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    glDisable(GL_VERTEX_PROGRAM_NV);

    glDisable(GL_REGISTER_COMBINERS_NV);
    glDisable(GL_TEXTURE_SHADER_NV);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(mproj);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(mview);
    glDisable(GL_REGISTER_COMBINERS_NV);
    glDisable(GL_TEXTURE_SHADER_NV);
}
void CSSprite::end_occlusion(bool bDebug)
{
    //
    //----> if we cannot use pbuffer as a texture, copy it.
    //
    if(m_bNoRenderTexture)
    {
        m_occlusionmap.bind();
        glCopyTexSubImage2D(m_occlusionmap.target, 0, 0, 0, 0, 0, TEX_SZ, TEX_SZ);
    }
    //
    //----> End with this PBuffer
    //
    if(!bDebug)
        m_PBuffer->Deactivate();
    m_inpbuffer = false;
}

void CSSprite::begin_render()
{
}
void CSSprite::end_render()
{
}
/**************************************************/ /**
    Display the shadow map for debugging purpose.

    In fact we display either the shadow map or the color map...
 **/
void CSSprite::display_occlusion_map()
{
    int vp[4];
    
    glDisable(GL_LIGHTING);
    glDisable(GL_REGISTER_COMBINERS_NV);
    glDisable(GL_TEXTURE_SHADER_NV);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glGetIntegerv(GL_VIEWPORT, vp);
    glViewport(0, 0, vp[2]/4, vp[3]/4);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    
    // reset our texture matrix
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    m_occlusionmap.enable();
    m_occlusionmap.bind();
#ifdef WIN32
    if(!m_bNoRenderTexture)
        if(!m_PBuffer->Bind(WGL_FRONT_LEFT_ARB))
    {
        fprintf(stderr, "wglReleaseTexImageARB Error");
    }
#endif

    glBegin(GL_QUADS);
    glTexCoord2f(0,0);
    glVertex2f(-1,-1);
    
    glTexCoord2f(0,1);
    glVertex2f(-1, 1);

    glTexCoord2f(1,1);
    glVertex2f( 1, 1);

    glTexCoord2f(1,0);
    glVertex2f( 1,-1);
    glEnd();

    m_occlusionmap.disable();

#ifdef WIN32
    if(!m_bNoRenderTexture)
        if(!m_PBuffer->Release(WGL_FRONT_LEFT_ARB))
    {
        fprintf(stderr, "wglReleaseTexImageARB Error");
    }
#endif  
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(vp[0], vp[1],vp[2], vp[3]);
    glEnable(GL_DEPTH_TEST); // need to set it. Not by default
}

void CSSprite::free()
{
    if(m_PBuffer)
    {
        delete m_PBuffer;
        m_PBuffer = 0;
    }

    if (Program)
    {
        cgDestroyProgram(Program);
        Program = 0;
    }

    if (Context)
    {
        cgDestroyContext(Context);
        Context = 0;
    }
}
