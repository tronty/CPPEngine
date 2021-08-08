/****************************************************************/ /**
 **
 ** class to display a list of volumetric lines
 ** Here we want this feature to live with any other
 ** settings the application is using.
 ** Thus we must handle
 ** - needed extensions checking and init
 ** - display list / array buffer for lines
 ** - OpenGL state for rendering
 **
 ** Note that this feature should be called after every rendering since
 ** Blending doesn't affect the Z-buffer
 **
 **/ /****************************************************************/
/** \file CVolumelines.h **/
#ifndef __CVOLUMELINES__
#define __CVOLUMELINES__

#include <math.h>
#include <vector>
#include <glh/glh_obs.h>
#include <glh/glh_linear.h>
#include <glh/glh_array.h>
#include <shared/nv_png.h>
#include <shared/array_texture.h>

#include <Cg/cgGL.h>

#define CVLINES_USEDISPLAYLIST
/**
 ** The class container for these lines
 **/
class CVLines
{
public:
    typedef struct
    {
        float FromPos[3];
        float ToPos[3];
        float FromSize, ToSize;
    } SLine;
private:
    bool binit;
    char *lpstr_err_msg;
    static char vertexcode[];

    GLuint vertex_program_id;
    GLuint dl_draw_lines; ///< only used when CVLINES_USEDISPLAYLIST
    bool bListChanged;
    std::vector<SLine> lines;
    glh::tex_object_2D texture;

    //-------------------------------------------------------------
    CGcontext Context;          ///< Cg context
    CGprogram vertexProgram;    ///< vertex program handler
    CGprofile vertexProfile;    ///< Cg profile
    CGprogram fragmentProgram;  ///< vertex program handler
    CGprofile fragmentProfile;  ///< Cg profile
    CGparameter hMatMViewProj;  ///< Modelview Projection 4x4 matrix
    CGparameter hMatMView;      ///< Modelview 4x4 matrix
    CGparameter hMatMViewI;     ///< Modelview inverse 4x4 matrix
    CGparameter hMatMViewIT;    ///< Modelview inverse transpose 4x4 matrix
    CGparameter hMatProj;       ///< Projection 4x4 matrix

    CGparameter hVarParam0;     ///< to comment
    CGparameter hVarParam1;     ///< to comment
    CGparameter hVarParam8;     ///< to comment
    CGparameter hVarParam9;     ///< to comment

    CGparameter hTexture0;
    CGparameter hTexture1;
    
    //-------------------------------------------------------------
public:
    CVLines();
    //void set_test_cst(GLfloat f1, GLfloat f2);
    int SetTexture(const char *texname);
    int init();
    int finish();
    void render();
    void lines_begin();
    void lines_end();
    void draw_line( glh::vec3f &inFromPos, glh::vec3f &inToPos, GLfloat inFromSize, GLfloat inToSize);
    void draw_linetoforget( glh::vec3f &inFromPos, glh::vec3f &inToPos, GLfloat inFromSize, GLfloat inToSize);
    void clear_buffer();
    void push_line( float *inFromPos, float *inToPos, float inFromSize, float inToSize);
    void change_line(   float *inFromPos, float *inToPos, float inFromSize, float inToSize );
    void compile_lines(bool bDraw=false);
    int get_num_lines();
    const char *CVLines::get_err_msg();

    void free();
    CGcontext get_cg_context() { return Context; }
};

#endif
