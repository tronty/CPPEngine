#if defined(WIN32)
# include <windows.h>
# pragma warning(disable: 4786)
#elif defined(UNIX)
# include <GL/glx.h>
#endif

#include <Cg/cgGL.h>

#define GLH_EXT_SINGLE_FILE
#include <glh/glh_obs.h>
#include <glh/glh_convenience.h>
#include <shared/read_text_file.h>

#include <nv_GenericTools/nv_GenericTools.h>

#include "CVolumelines.h"

extern void cleanExit(int exitval);

CVLines::CVLines() :
  binit(false),
  lpstr_err_msg(NULL),
  dl_draw_lines(0),
  bListChanged(false)
{
    Context = 0;
    vertexProgram = 0;
    fragmentProgram = 0;
    hMatMViewProj = 0;
    hMatMViewI = 0;
    hMatMViewIT = 0;
    hMatMView = 0;
    hMatProj = 0;

    hVarParam0 = 0;
    hVarParam1 = 0;
    hVarParam8 = 0;
    hVarParam9 = 0;

    hTexture0 = 0;
    hTexture1 = 0;
}
  
/**
 ** Init for opengl
 **/
int CVLines::SetTexture(const char *texname)
{
    texture.bind();
    glh::array2<glh::vec3ub> img;
    read_png_rgb(texname, img);
    make_rgb_texture( img, false);
    texture.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    texture.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    return 1;
}
/**
 ** Init for opengl
 **/
int CVLines::init()
{
    glh::tex_object_2D tex;

    if(binit)
        return 1;

	//
	// Resource type for PNG and vertex progs. This is OPTIONAL. No conflict with linux...
	//
	unsigned int hModule = 0;
#ifdef WIN32
	hModule = (unsigned int)GetModuleHandle("cg_VolumeLine.bfx");
	set_png_module_handle(hModule);
	set_png_module_restypename("PNG");
	set_module_handle(hModule);
	set_module_restypename("NV");
#endif

    // Use GL_ARB_vertex_program extension if supported by video card
    if (cgGLIsProfileSupported(CG_PROFILE_ARBVP1))
        vertexProfile = CG_PROFILE_ARBVP1;
    else if (cgGLIsProfileSupported(CG_PROFILE_VP20))
        vertexProfile = CG_PROFILE_VP20;
    else
    {
        STX_PRINT("Vertex programming extensions (GL_ARB_vertex_program or "
               "GL_NV_vertex_program) not supported, exiting...\n");
        return 0;
    }

    // use ARB_fragment_profile if register combiners not supported
    if (cgGLIsProfileSupported(CG_PROFILE_FP20))
        fragmentProfile = CG_PROFILE_FP20;
    else if (cgGLIsProfileSupported(CG_PROFILE_ARBFP1))
        fragmentProfile = CG_PROFILE_ARBFP1;
    else
    {
        STX_PRINT("Fragment programming extensions (GL_NV_register_combiners2 or "
               "GL_ARB_fragment_program) not supported, exiting...\n");
        return 0;
    }
    
    //
    // load the default texture
    //
    texture.bind();
    glh::array2<glh::vec3ub> img;
    read_png_rgb("1d_INNER2.png", img);
    //read_png_rgb("1d_TUBE.png", img);
    //read_png_rgb("1d_SPIRAL.png", img);
    //read_png_rgb("1d_RING2.png", img);
    //read_png_rgb("1d_glow1.png", img);
    //read_png_rgb("1d_debug2.png", img);
    make_rgb_texture( img, false);//true );
    texture.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    texture.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    //
    // Create the context
    //
    Context = cgCreateContext();
    
    //
    // Load and compile the program
    //
	// Load the program. Optionaly have a look in the resources
    char *cgProg = read_text_file("cg_Volumelines/cg_Volumelines.cg", "NV", hModule);
    if (!cgProg)
    {
        STX_PRINT("Unable to load cg_Volumelines.cg, exiting...\n");
        cleanExit(0);
    }

    vertexProgram = cgCreateProgram(Context, 
                                    CG_SOURCE, cgProg,
                                    vertexProfile, NULL, NULL);
    cgGLLoadProgram(vertexProgram);

    delete cgProg;

    //
    // Get back handle of various uniform parameters
    //
    hMatMViewProj = cgGetNamedParameter(vertexProgram, "ModelViewProj");
    if(!hMatMViewProj) LOGMSG(LOG_WARN, "unable to find parameter ModelViewProj");
    hMatMViewIT = cgGetNamedParameter(vertexProgram, "ModelViewIT");
    if(!hMatMViewIT) LOGMSG(LOG_WARN, "unable to find parameter ModelViewIT");
    hMatMViewI = cgGetNamedParameter(vertexProgram, "ModelViewI");
    if(!hMatMViewI) LOGMSG(LOG_WARN, "unable to find parameter ModelViewI");
    hMatMView = cgGetNamedParameter(vertexProgram, "ModelView");
    if(!hMatMView) LOGMSG(LOG_WARN, "unable to find parameter ModelView");
    hMatProj = cgGetNamedParameter(vertexProgram, "Projection");
    if(!hMatProj) LOGMSG(LOG_WARN, "unable to find parameter Projection");
    //
    // Get back handles of varying parameters
    //
    hVarParam0 = cgGetNamedParameter(vertexProgram, "IN.startpos"); // subject to change in "IN.param0"
    if(!hVarParam0) LOGMSG(LOG_WARN, "unable to find parameter IN.startpos");
    hVarParam1 = cgGetNamedParameter(vertexProgram, "IN.endpos"); 
    if(!hVarParam1) LOGMSG(LOG_WARN, "unable to find parameter IN.endpos");
    hVarParam8 = cgGetNamedParameter(vertexProgram, "IN.param8"); 
    if(!hVarParam8) LOGMSG(LOG_WARN, "unable to find parameter IN.param8");
    hVarParam9 = cgGetNamedParameter(vertexProgram, "IN.param9"); 
    if(!hVarParam9) LOGMSG(LOG_WARN, "unable to find parameter IN.param9");

    cgProg = read_text_file("cg_Volumelines/cg_Volumelines_fragment.cg", "NV", hModule);
    if (!cgProg)
    {
        STX_PRINT("Unable to load cg_Volumelines.cg, exiting...\n");
        cleanExit(0);
    }

    fragmentProgram = cgCreateProgram(Context, 
                                      CG_SOURCE, cgProg,
                                      fragmentProfile, NULL, NULL);
    cgGLLoadProgram(fragmentProgram);

    delete cgProg;

    hTexture0 = cgGetNamedParameter(fragmentProgram, "texture0");
    hTexture1 = cgGetNamedParameter(fragmentProgram, "texture1");

    if (!hTexture0 || !hTexture1)
    {
        STX_PRINT("Unable to retrieve fragment program parameters, exiting...\n");
        cleanExit(0);
    }

    cgGLSetTextureParameter(hTexture0, texture.texture);
    cgGLSetTextureParameter(hTexture1, texture.texture);

    return 1;
}
const char *CVLines::get_err_msg()
{
    return lpstr_err_msg ? lpstr_err_msg : "no errors";
}
/**
 ** free
 **/
int CVLines::finish()
{
    if(!binit)
        return 1;
    if(dl_draw_lines)
        glDeleteLists(dl_draw_lines,1);
    return 0;
}
/**
 ** Render
 **/
void CVLines::render()
{
    lines_begin();
    if(bListChanged)
        compile_lines(true);
    else
        glCallList(dl_draw_lines);
    lines_end();
}
/**
 ** Setup OpenGL for drawing.
 ** can be used when calling draw_line().
 ** is used by the render() method.
 **/
void CVLines::lines_begin()
{
    glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|GL_LIGHTING_BIT|GL_POLYGON_BIT);
    //
    // Enable the use of Vertex Prog
    //
    cgGLEnableProfile(vertexProfile);
    //
    // Bind the program
    //
    cgGLBindProgram(vertexProgram);

    cgGLEnableProfile(fragmentProfile);
    cgGLBindProgram(fragmentProgram);

    //
    // Setup the Uniform values
    //
    if(hMatMViewI) cgGLSetStateMatrixParameter(hMatMViewI, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE);
    if(hMatMViewIT) cgGLSetStateMatrixParameter(hMatMViewIT, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
    if(hMatMViewProj) cgGLSetStateMatrixParameter(hMatMViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    if(hMatMView) cgGLSetStateMatrixParameter(hMatMView, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
    if(hMatProj) cgGLSetStateMatrixParameter(hMatProj, CG_GL_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

    cgGLEnableTextureParameter(hTexture0);
    cgGLEnableTextureParameter(hTexture1);

    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    
    glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_TEST);

    glBegin(GL_QUADS);
}
void CVLines::lines_end()
{
    glEnd();

    cgGLDisableTextureParameter(hTexture0);
    cgGLDisableTextureParameter(hTexture1);

    cgGLDisableProfile(vertexProfile);
    cgGLDisableProfile(fragmentProfile);

    glPopAttrib();
}
/**
 ** Draw a particle : well fitted to short lines...
 ** uses 4 vertex
 **/
void CVLines::draw_line(    glh::vec3f &inFromPos, glh::vec3f &inToPos, 
            GLfloat inFromSize, GLfloat inToSize)
{
    cgGLSetParameter3fv(hVarParam1, inToPos.v );
    cgGLSetParameter4f(hVarParam8, inFromSize,inToSize,0,0); // (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, -inFromSize,0,inFromSize*0.5f);            // SZy
    cgGLSetParameter3fv(hVarParam0, inFromPos.v); // Attrib #0 MUST be called at the END

    cgGLSetParameter3fv(hVarParam1, inFromPos.v);
    cgGLSetParameter4f(hVarParam8, inToSize,inFromSize,0.25f,0);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, inToSize,0,inFromSize*0.5f);               // SZy
    cgGLSetParameter3fv(hVarParam0, inToPos.v);

    cgGLSetParameter3fv(hVarParam1, inFromPos.v);
    cgGLSetParameter4f(hVarParam8, inToSize,inFromSize,0.25f,0.25f);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, -inToSize,0,inFromSize*0.5f);                  // SZy
    cgGLSetParameter3fv(hVarParam0, inToPos.v);

    cgGLSetParameter3fv(hVarParam1, inToPos.v);
    cgGLSetParameter4f(hVarParam8, inFromSize,inToSize,0,0.25f);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, inFromSize,0,inFromSize*0.5f);             // SZy
    cgGLSetParameter3fv(hVarParam0, inFromPos.v);
}
/**
 ** Draw a line. Well fitted for int lines.
 ** used by the display list OR can be used directly after lines_begin()
 ** uses 8 vertex
 **/
void CVLines::draw_linetoforget(    glh::vec3f &inFromPos, glh::vec3f &inToPos, 
            GLfloat inFromSize, GLfloat inToSize)
{
#define TEXWIDTH 0.25f
#define TEXHEIGHT 0.25f
    //
    // Start quad
    //
    cgGLSetParameter4f(hVarParam1, inToPos[0], inToPos[1], inToPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inFromSize,inToSize,TEXWIDTH,0);  // (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, -inFromSize,-inToSize,0);         // SZy
    cgGLSetParameter4f(hVarParam0, inFromPos[0], inFromPos[1], inFromPos[2], 1); // Attrib #0 MUST be called at the END

    cgGLSetParameter4f(hVarParam1, inToPos[0], inToPos[1], inToPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inFromSize,inToSize,0,0);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, -inFromSize,-inToSize,inFromSize);                // SZy
    cgGLSetParameter4f(hVarParam0, inFromPos[0], inFromPos[1], inFromPos[2], 1); // Attrib #0 MUST be called at the END

    cgGLSetParameter4f(hVarParam1, inToPos[0], inToPos[1], inToPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inFromSize,inToSize,0,TEXHEIGHT);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, inFromSize,inToSize,inFromSize);                  // SZy
    cgGLSetParameter4f(hVarParam0, inFromPos[0], inFromPos[1], inFromPos[2], 1); // Attrib #0 MUST be called at the END

    cgGLSetParameter4f(hVarParam1, inToPos[0], inToPos[1], inToPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inFromSize,inToSize,TEXWIDTH,TEXHEIGHT);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, inFromSize,inToSize,0);           // SZy
    cgGLSetParameter4f(hVarParam0, inFromPos[0], inFromPos[1], inFromPos[2], 1); // Attrib #0 MUST be called at the END
    //
    // Middle quad
    //
    cgGLSetParameter4f(hVarParam1, inToPos[0], inToPos[1], inToPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inFromSize,inToSize,0,0); // (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, -inFromSize,-inToSize,inFromSize);            // SZy
    cgGLSetParameter4f(hVarParam0, inFromPos[0], inFromPos[1], inFromPos[2], 1); // Attrib #0 MUST be called at the END

    cgGLSetParameter4f(hVarParam1, inFromPos[0], inFromPos[1], inFromPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inToSize,inFromSize,0.25f,0);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, inToSize,inFromSize,inToSize);                // SZy
    cgGLSetParameter4f(hVarParam0, inToPos[0], inToPos[1], inToPos[2], 1);

    cgGLSetParameter4f(hVarParam1, inFromPos[0], inFromPos[1], inFromPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inToSize,inFromSize,0.25f,0.25f);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, -inToSize,-inFromSize,inToSize);                  // SZy
    cgGLSetParameter4f(hVarParam0, inToPos[0], inToPos[1], inToPos[2], 1);

    cgGLSetParameter4f(hVarParam1, inToPos[0], inToPos[1], inToPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inFromSize,inToSize,0,0.25f);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, inFromSize,inToSize,inFromSize);              // SZy
    cgGLSetParameter4f(hVarParam0, inFromPos[0], inFromPos[1], inFromPos[2], 1);
    //
    // End quad
    //
    cgGLSetParameter4f(hVarParam1, inFromPos[0], inFromPos[1], inFromPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inToSize,inFromSize,TEXWIDTH,0);  // (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, inToSize,inFromSize,0);           // SZy
    cgGLSetParameter4f(hVarParam0, inToPos[0], inToPos[1], inToPos[2], 1); // Attrib #0 MUST be called at the END

    cgGLSetParameter4f(hVarParam1, inFromPos[0], inFromPos[1], inFromPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inToSize,inFromSize,0,0);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, inToSize,inFromSize,inToSize);                // SZy
    cgGLSetParameter4f(hVarParam0, inToPos[0], inToPos[1], inToPos[2], 1); // Attrib #0 MUST be called at the END

    cgGLSetParameter4f(hVarParam1, inFromPos[0], inFromPos[1], inFromPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inToSize,inFromSize,0,TEXHEIGHT);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, -inToSize,-inFromSize,inToSize);                  // SZy
    cgGLSetParameter4f(hVarParam0, inToPos[0], inToPos[1], inToPos[2], 1); // Attrib #0 MUST be called at the END

    cgGLSetParameter4f(hVarParam1, inFromPos[0], inFromPos[1], inFromPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inToSize,inFromSize,TEXWIDTH,TEXHEIGHT);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, -inToSize,-inFromSize,0);             // SZy
    cgGLSetParameter4f(hVarParam0, inToPos[0], inToPos[1], inToPos[2], 1); // Attrib #0 MUST be called at the END
}
/**
 ** clear the list
 **/
void CVLines::clear_buffer()
{
    lines.clear();
    bListChanged = true;
}
/**
 ** Add a line. return the index in list
 **/
void CVLines::push_line(    float *inFromPos, float *inToPos, 
            float inFromSize, float inToSize)
{
    SLine sline;
    stx_memcpy(sline.FromPos, inFromPos, sizeof(float)*3);
    stx_memcpy(sline.ToPos, inToPos, sizeof(float)*3);
    sline.FromSize = inFromSize;
    sline.ToSize = inToSize;
    lines.push_back(sline);

    bListChanged = true;
}
/**
 ** Change a line
 **/
void CVLines::change_line(  float *inFromPos, float *inToPos, 
            float inFromSize, float inToSize )
{
}
/**
 ** re-create the display-list / array vertex buffer
 **/
void CVLines::compile_lines(bool bDraw)
{
    std::vector<SLine>::iterator it_line;
    if(!dl_draw_lines)
        dl_draw_lines = glGenLists(1);
    glNewList(dl_draw_lines, bDraw ? GL_COMPILE_AND_EXECUTE : GL_COMPILE);

    glBegin(GL_QUADS);
    for(it_line = lines.begin(); it_line != lines.end(); it_line++)
    {
//          draw_line(it_line->FromPos, it_line->ToPos, it_line->FromSize, it_line->ToSize);
    }
    glEnd();

    glEndList();
}
/**
 ** get the number of lines
 **/
int CVLines::get_num_lines()
{
    return lines.size();
}

void CVLines::free()
{
    if (vertexProgram)
    {
        cgDestroyProgram(vertexProgram);
        vertexProgram = 0;
    }

    if (fragmentProgram)
    {
        cgDestroyProgram(fragmentProgram);
        fragmentProgram = 0;
    }

    if (Context)
    {
        cgDestroyContext(Context);
        Context = 0;
    }
}

