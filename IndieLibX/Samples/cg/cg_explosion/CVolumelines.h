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

#include "cg_Cexplosion.h"

#include <Cg/cgGL.h>

#define CVLINES_USEDISPLAYLIST
#ifndef M_PI
#define M_PI 3.1415927f
#endif
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
    glh::tex_object_2D texture1;
    glh::tex_object_2D texture_mask;
    glh::tex_object_2D texture1_mask;

    //-------------------------------------------------------------
    CGcontext Context;          ///< Cg context
    CGprogram vertexprogram;    ///< vertex program handler
    CGprofile vertexprofile;    ///< Cg profile
    CGprogram fragmentprogram;  ///< vertex program handler
    CGprofile fragmentprofile;  ///< Cg profile
    CGparameter hMatMViewProj;  ///< Projection 4x4 matrix
    CGparameter hMatMView;      ///< Projection 4x4 matrix
    CGparameter hMatProj;       ///< Projection 4x4 matrix

    CGparameter hVarParam0;     ///< to comment
    CGparameter hVarParam1;     ///< to comment
    CGparameter hVarParam8;     ///< to comment
    CGparameter hVarParam9;     ///< to comment

    CGparameter hTexture0;
    CGparameter hTexture1;
    CGparameter hTexture0_mask;
    CGparameter hTexture1_mask;
    CGparameter param_Noise3D;
    CGparameter param_NoiseScaleAmpTime;
    
    //-------------------------------------------------------------
public:
	float bulletnoisescale;
	float bulletnoiseamp;
	float bulletnoisetime;

  CVLines();
    //void set_test_cst(GLfloat f1, GLfloat f2);
    int SetTexture(const char *texname);
    int init();
    int finish();
    void render();
    void lines_begin();
    void lines_end();
    void draw_line( glh::vec3f &inFromPos, glh::vec3f &inToPos, GLfloat inFromSize, GLfloat inToSize);
    void draw_line( float *inFromPos, float *inToPos, GLfloat inFromSize, GLfloat inToSize);
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
struct VLineEntity
{
	float ss_starttime;
	float ss_al, ss_be;
	float MVPos[3];
	float VtxPos[3];
	float VtxPos1[3];
	float VtxPos2[3];
	float N[3];
	float alpha;
	float ss_lifetime;
	VLineEntity()
	{
		ss_lifetime = 0;
		ss_starttime = 0;
		ss_al = ss_be = 0;
		alpha = 0;
	}
	void Anim(CNV30Effect *parent, float time, float *mmv);
	void Draw(CVLines & vlines);
};

#endif
