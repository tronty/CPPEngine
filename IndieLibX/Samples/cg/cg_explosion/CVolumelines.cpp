#if defined(WIN32)
# include <windows.h>
# pragma warning(disable: 4786)
#elif defined(UNIX)
# include <GL/glx.h>
#endif

#include <Cg/cgGL.h>

#include <glh/glh_obs.h>
#include <glh/glh_convenience.h>
#include <shared/read_text_file.h>
#include <shared/data_path.h>
#include <nv_dds/nv_dds.h>
//#include <nv_rtmodules/modhelper.h>
#include <nv_GenericTools/nv_GenericTools.h>

#include "CVolumelines.h"
#include "glError.h"

#include "cg_Cdisc.h"
extern data_path progpath;
extern CDisc nv30effectdisc;
extern CPerlinNoise perlinnoise;

#define CHECKPARAM(p) if(p == NULL) LOGMSG(LOG_WARN, "failed to find %s...", #p);

CVLines::CVLines() :
  binit(false),
  lpstr_err_msg(NULL),
  dl_draw_lines(0),
  bListChanged(false)
{
    Context = 0;
    vertexprogram = 0;
    fragmentprogram = 0;
    hMatMViewProj = 0;
    hMatMView = 0;
    hMatProj = 0;

    hVarParam0 = 0;
    hVarParam1 = 0;
    hVarParam8 = 0;
    hVarParam9 = 0;

    hTexture0 = 0;
    hTexture1 = 0;
    hTexture0_mask = 0;
    hTexture1_mask = 0;
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
    CGenum fptype, vptype;
	const char * vpfname;
	const char * fpfname;
    glh::tex_object_2D tex;

    if(binit)
        return 1;

	bulletnoisescale = 0.25f;
	bulletnoiseamp = 0.12f;
	bulletnoisetime = 0.0f;

    vpfname = "cg_disc_vertex.cg";
    fpfname = "cg_disc_fragment.cg";
    fptype = CG_SOURCE;
    vptype = CG_SOURCE;
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
    //
    // check for extensions : some are need
    //
    if (!glh_init_extensions("GL_VERSION_1_4"))
    {
        LOGMSG(LOG_ERR, "need this extension(s): %s", glh_get_unsupported_extensions());
        return false;
    }

    //
    // Get the latest program available
    //
    vertexprofile = CG_PROFILE_ARBVP1;//cgGLGetLatestProfile(CG_GL_VERTEX);
    cgGLSetOptimalOptions(vertexprofile);
    fragmentprofile = CG_PROFILE_ARBFP1;//cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgGLSetOptimalOptions(fragmentprofile);
    LOGMSG(LOG_MSG, "using vertex profile %s for Volumeline", cgGetProfileString(vertexprofile));
    LOGMSG(LOG_MSG, "using fragment profile %s for Volumeline", cgGetProfileString(fragmentprofile));

    if((fragmentprofile == CG_PROFILE_UNKNOWN)||(vertexprofile == CG_PROFILE_UNKNOWN))
    {
        LOGMSG(LOG_WARN, "profiles are missing !");
        return false;
    }

	//
	// Noise: just take the one from CDisk for this time
	//
	// nv30effectdisc.noisetex2...
    //
    // load the default texture
    //
	nv_dds::CDDSImage ddsimage;
	std::string pathname;
	pathname = progpath.get_file("fire_right.dds");
	if(!pathname.empty())
	{
		if(ddsimage.load(pathname))
		{
			texture.bind();
			if(!ddsimage.upload_texture2D())
			{
				LOGMSG(LOG_ERR, "failed to upload fire_right.dds in 2D OpenGL texture");
			}
			texture.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			texture.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			texture.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			CHECKGLERRORS();
		}
		else {
			LOGMSG(LOG_ERR, "failed to load fire_right.dds");
		}
	}
	else {
		LOGMSG(LOG_ERR, "fire_right.dds not found");
	}
	pathname = progpath.get_file("fire_right_mask.dds");
	if(!pathname.empty())
	{
		if(ddsimage.load(pathname))
		{
			texture_mask.bind();
			if(!ddsimage.upload_texture2D())
			{
				LOGMSG(LOG_ERR, "failed to upload fire_right_mask.dds in 2D OpenGL texture");
			}
			texture_mask.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			texture_mask.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			texture_mask.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			CHECKGLERRORS();
		}
		else {
			LOGMSG(LOG_ERR, "failed to load fire_right_mask.dds");
		}
	}
	else {
		LOGMSG(LOG_ERR, "fire_right_mask.dds not found");
	}
	pathname = progpath.get_file("fire_front.dds");
	if(!pathname.empty())
	{
		if(ddsimage.load(pathname))
		{
			texture1.bind();
			if(!ddsimage.upload_texture2D())
			{
				LOGMSG(LOG_ERR, "failed to upload fire_front.dds in 2D OpenGL texture");
			}
			texture1.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			texture1.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			texture1.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			CHECKGLERRORS();
		}
		else {
			LOGMSG(LOG_ERR, "failed to load fire_front.dds");
		}
	}
	else {
		LOGMSG(LOG_ERR, "fire_front.dds not found");
	}
	pathname = progpath.get_file("fire_front_mask.dds");
	if(!pathname.empty())
	{
		if(ddsimage.load(pathname))
		{
			texture1_mask.bind();
			if(!ddsimage.upload_texture2D())
			{
				LOGMSG(LOG_ERR, "failed to upload fire_front_mask.dds in 2D OpenGL texture");
			}
			texture1_mask.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			texture1_mask.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			texture1_mask.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			CHECKGLERRORS();
		}
		else {
			LOGMSG(LOG_ERR, "failed to load fire_front_mask.dds");
		}
	}
	else {
		LOGMSG(LOG_ERR, "fire_front_mask.dds not found");
	}

    //
    // Create the context
    //
    Context = cgCreateContext();
    
    //
    // Load and compile the program
    //
	// Load the program. Optionaly have a look in the resources
    char *cgProg = read_text_file("cg_Volumelines.cg", "NV", hModule);
    if (!cgProg)
    {
        LOGMSG(LOG_ERR, "Unable to load cg_Volumelines.cg, exiting...\n");
        return 0;
    }

    vertexprogram = cgCreateProgram(Context, 
                                    CG_SOURCE, cgProg,
                                    vertexprofile, NULL, NULL);
    cgGLLoadProgram(vertexprogram);

    delete cgProg;

    //
    // Get back handle of various uniform parameters
    //
    hMatMViewProj = cgGetNamedParameter(vertexprogram, "ModelViewProj");
	CHECKPARAM(hMatMViewProj);
    hMatMView = cgGetNamedParameter(vertexprogram, "ModelView");
	CHECKPARAM(hMatMView);
    hMatProj = cgGetNamedParameter(vertexprogram, "Projection");
	CHECKPARAM(hMatProj);
    //
    // Get back handles of varying parameters
    //
    hVarParam0 = cgGetNamedParameter(vertexprogram, "IN.startpos"); // subject to change in "IN.param0"
	CHECKPARAM(hVarParam0);
    hVarParam1 = cgGetNamedParameter(vertexprogram, "IN.endpos"); 
	CHECKPARAM(hVarParam1);
    hVarParam8 = cgGetNamedParameter(vertexprogram, "IN.param8"); 
	CHECKPARAM(hVarParam8);
    hVarParam9 = cgGetNamedParameter(vertexprogram, "IN.param9"); 
	CHECKPARAM(hVarParam9);

    if (!hMatMViewProj || !hMatMView || !hMatProj || !hVarParam0 || !hVarParam1 || !hVarParam8 || !hVarParam9)
    {
        LOGMSG(LOG_ERR, "Unable to retrieve vertex program parameters, exiting...\n");
        return 0;
    }

    cgProg = read_text_file("cg_Volumelines_fragment.cg", "NV", hModule);
    if (!cgProg)
    {
        LOGMSG(LOG_ERR, "Unable to load cg_Volumelines.cg, exiting...\n");
        return 0;
    }

    fragmentprogram = cgCreateProgram(Context, 
                                      CG_SOURCE, cgProg,
                                      fragmentprofile, NULL, NULL);
    cgGLLoadProgram(fragmentprogram);

    delete cgProg;

    hTexture0 = cgGetNamedParameter(fragmentprogram, "texture0");
	CHECKPARAM(hTexture0);
    hTexture0_mask = cgGetNamedParameter(fragmentprogram, "texture0_mask");
	CHECKPARAM(hTexture0_mask);
    hTexture1 = cgGetNamedParameter(fragmentprogram, "texture1");
	CHECKPARAM(hTexture1);
    hTexture1_mask = cgGetNamedParameter(fragmentprogram, "texture1_mask");
	CHECKPARAM(hTexture1_mask);
    param_Noise3D = cgGetNamedParameter(fragmentprogram, "noisetex");
	CHECKPARAM(param_Noise3D);
    param_NoiseScaleAmpTime = cgGetNamedParameter(fragmentprogram, "NoiseScaleAmpTime");
	CHECKPARAM(param_NoiseScaleAmpTime);

    cgGLSetTextureParameter(hTexture0, texture.texture);
    cgGLSetTextureParameter(hTexture1, texture1.texture);
    cgGLSetTextureParameter(hTexture0_mask, texture_mask.texture);
    cgGLSetTextureParameter(hTexture1_mask, texture1_mask.texture);
    cgGLSetTextureParameter(param_Noise3D, nv30effectdisc.noisetex2.texture);
	binit = 1;
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
    cgGLEnableProfile(vertexprofile);
    //
    // Bind the program
    //
    cgGLBindProgram(vertexprogram);

    cgGLEnableProfile(fragmentprofile);
    cgGLBindProgram(fragmentprogram);

    //
    // Setup the Uniform values
    //
    cgGLSetStateMatrixParameter(hMatMViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(hMatMView, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(hMatProj, CG_GL_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

    cgGLEnableTextureParameter(hTexture0);
    cgGLEnableTextureParameter(hTexture1);
    cgGLEnableTextureParameter(hTexture0_mask);
    cgGLEnableTextureParameter(hTexture1_mask);
    cgGLEnableTextureParameter(param_Noise3D);

    cgGLSetParameter3f(param_NoiseScaleAmpTime, bulletnoisescale, bulletnoiseamp, bulletnoisetime*0.04f);

    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    glBegin(GL_QUADS);
}
void CVLines::lines_end()
{
    glEnd();

    cgGLDisableTextureParameter(hTexture0);
    cgGLDisableTextureParameter(hTexture1);
    cgGLDisableTextureParameter(hTexture0_mask);
    cgGLDisableTextureParameter(hTexture1_mask);
    cgGLEnableTextureParameter(param_Noise3D);

    cgGLDisableProfile(vertexprofile);
    cgGLDisableProfile(fragmentprofile);

    glPopAttrib();
}
#define PARAM9FACT 0.9f
/**
 ** Draw a particle : well fitted to short lines...
 ** uses 4 vertex
 **/
void CVLines::draw_line(    glh::vec3f &inFromPos, glh::vec3f &inToPos, 
            GLfloat inFromSize, GLfloat inToSize)
{
    cgGLSetParameter4f(hVarParam1, inToPos[0], inToPos[1], inToPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inFromSize,inToSize,0,0); // (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, -inFromSize,0,inFromSize*PARAM9FACT);            // SZy
    cgGLSetParameter4f(hVarParam0, inFromPos[0], inFromPos[1], inFromPos[2], 1); // Attrib #0 MUST be called at the END

    cgGLSetParameter4f(hVarParam1, inFromPos[0], inFromPos[1], inFromPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inToSize,inFromSize,1,0);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, inToSize,0,inFromSize*PARAM9FACT);               // SZy
    cgGLSetParameter4f(hVarParam0, inToPos[0], inToPos[1], inToPos[2], 1);

    cgGLSetParameter4f(hVarParam1, inFromPos[0], inFromPos[1], inFromPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inToSize,inFromSize,1,1);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, -inToSize,0,inFromSize*PARAM9FACT);                  // SZy
    cgGLSetParameter4f(hVarParam0, inToPos[0], inToPos[1], inToPos[2], 1);

    cgGLSetParameter4f(hVarParam1, inToPos[0], inToPos[1], inToPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inFromSize,inToSize,0,1);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, inFromSize,0,inFromSize*PARAM9FACT);             // SZy
    cgGLSetParameter4f(hVarParam0, inFromPos[0], inFromPos[1], inFromPos[2], 1);
}
/**
 ** Draw a particle : well fitted to short lines...
 ** uses 4 vertex
 **/
void CVLines::draw_line(    float *inFromPos, float *inToPos, 
            GLfloat inFromSize, GLfloat inToSize)
{
    cgGLSetParameter4f(hVarParam1, inToPos[0], inToPos[1], inToPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inFromSize,inToSize,0,0); // (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, -inFromSize,0,inFromSize*PARAM9FACT);            // SZy
    cgGLSetParameter4f(hVarParam0, inFromPos[0], inFromPos[1], inFromPos[2], 1); // Attrib #0 MUST be called at the END

    cgGLSetParameter4f(hVarParam1, inFromPos[0], inFromPos[1], inFromPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inToSize,inFromSize,1,0);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, inToSize,0,inFromSize*PARAM9FACT);               // SZy
    cgGLSetParameter4f(hVarParam0, inToPos[0], inToPos[1], inToPos[2], 1);

    cgGLSetParameter4f(hVarParam1, inFromPos[0], inFromPos[1], inFromPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inToSize,inFromSize,1,1);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, -inToSize,0,inFromSize*PARAM9FACT);                  // SZy
    cgGLSetParameter4f(hVarParam0, inToPos[0], inToPos[1], inToPos[2], 1);

    cgGLSetParameter4f(hVarParam1, inToPos[0], inToPos[1], inToPos[2], 1);
    cgGLSetParameter4f(hVarParam8, inFromSize,inToSize,0,1);// (SZx1, SZx2, TexX, TexY)
    cgGLSetParameter3f(hVarParam9, inFromSize,0,inFromSize*PARAM9FACT);             // SZy
    cgGLSetParameter4f(hVarParam0, inFromPos[0], inFromPos[1], inFromPos[2], 1);
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
    if (vertexprogram)
    {
        cgDestroyProgram(vertexprogram);
        vertexprogram = 0;
    }

    if (fragmentprogram)
    {
        cgDestroyProgram(fragmentprogram);
        fragmentprogram = 0;
    }

    if (Context)
    {
        cgDestroyContext(Context);
        Context = 0;
    }
}

void VLineEntity::Anim(CNV30Effect *parent, float time, float *mmv)
{
	float t = (time - ss_starttime)/ss_lifetime;
	if((t >= 1.0) || (t<0))
	{
		t = 0;
		ss_starttime = time;
		ss_lifetime = 20.0f + (float)(stx_rand()&0x1F);
		ss_al = (float)(stx_rand()&0xFF)*M_PI/255.0f - (0.5f*M_PI);
		ss_be = (float)(stx_rand()&0xFF)*M_PI/255.0f;
		N[0] = -sin(ss_al) * sin(ss_be);
		N[2] = cos(ss_al) * sin(ss_be);
		N[1] = cos(ss_be);
		VtxPos[0] = N[0] * parent->sphscale[0];
		VtxPos[1] = N[1] * parent->sphscale[1];
		VtxPos[2] = N[2] * parent->sphscale[2];
		perlinnoise.EvalFractalNoise(VtxPos,N, parent->noisescale , parent->displacement , parent->noisematrixcache);
	}
	alpha = (time - ss_starttime)/ss_lifetime;

	VtxPos1[0] = VtxPos[0] + N[0]*2.5f*alpha;
	VtxPos1[1] = VtxPos[1] + N[1]*2.5f*alpha;
	VtxPos1[2] = VtxPos[2] + N[2]*2.5f*alpha;

	VtxPos2[0] = VtxPos[0] + N[0]*10.0f*alpha;
	VtxPos2[1] = VtxPos[1] + N[1]*10.0f*alpha;
	VtxPos2[2] = VtxPos[2] + N[2]*10.0f*alpha;

	//
	// Modelview pos for sorting the objects
	//
	float p[4];
	p[0] = 0.5f*(VtxPos1[0] + VtxPos2[0]);
	p[1] = 0.5f*(VtxPos1[1] + VtxPos2[1]);
	p[2] = 0.5f*(VtxPos1[2] + VtxPos2[2]);
	p[3] = 0;
	MVPos[0] =  mmv[0] * p[0] 
				+ mmv[4] * p[1] 
				+ mmv[8] * p[2] 
				+ mmv[12] * p[3];
	MVPos[1] =  mmv[0+1] * p[0] 
				+ mmv[4+1] * p[1] 
				+ mmv[8+1] * p[2] 
				+ mmv[12+1] * p[3];
	MVPos[2] =  mmv[0+2] * p[0] 
				+ mmv[4+2] * p[1] 
				+ mmv[8+2] * p[2] 
				+ mmv[12+2] * p[3];

}
void VLineEntity::Draw(CVLines & vlines)
{
	if(alpha <= 0.1)
		glColor3f((1 - alpha*alpha*alpha*alpha)*alpha*10.0f,0,0);
	else
		glColor3f(1.0f - alpha*alpha*alpha*alpha,0,0);
	vlines.draw_line(VtxPos1,VtxPos2,(1.0f+2.0f*alpha)*0.4f,0.4f);
}


