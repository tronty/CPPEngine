#if defined(WIN32)
# include <windows.h>
#elif defined(UNIX)
# include <GL/glx.h>
#endif

#include <Cg/cgGL.h>

#include <glh/glh_extensions.h>
#include <glh/glh_obs.h>
#include <glh/glh_array.h>

#include <shared/nv_png.h>
#include <shared/read_text_file.h>
#include <shared/array_texture.h>

#include <math.h>

#include <vector>
#include <string>

#include "cg_Cdisc.h"
#include "glError.h"

//
// External functions
//
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

FilteredNoiseEx			*CDisc::noise = NULL;
CPerlinNoise			*CDisc::pnoise = NULL;

CDisc::CDisc() :
  binit(false),
  Context(NULL),
  vertexProg(NULL),
  vertexProg2(NULL),
  shockwave(0.1f)
  {
    noise_scale = 1;
    noise_scale2 = 1.8;
    noise_amp = 1;
    noise_amp2 = 1;
    noise_translate[0] = 0;
    noise_translate[1] = 0;
    noise_translate[2] = 0;
    dfadeout = 1;
    in_scale = 0.0f;
    out_scale = 2.37f;
    slices = 50;
  }
CDisc::~CDisc()
{
	if(noise)
	{
	  delete noise;
	  delete pnoise;
	  noise = NULL;
	  pnoise = NULL;
	}
}
/**
 ** Init for opengl
 **/
bool CDisc::init()
{
    char *cgProg;
    CGenum fptype, vptype;
	const char * vpfname;
	const char * fpfname;
	const char * vpfname2;
	const char * fpfname2;
    glh::tex_object_2D tex;
	FilteredNoiseEx *ptrnoise = NULL;

	if(!noise)
	{
		noise = new FilteredNoiseEx(16,16,16),
		pnoise = new CPerlinNoise();
		ptrnoise = noise;
	}
    vpfname = "cg_disc_vertex.cg";
    fpfname = "cg_disc_fragment.cg";
	fpfname2= "cg_disc_shock_fragment.cg";
	vpfname2= "cg_disc_shock_vertex.cg";
    fptype = CG_SOURCE;
    vptype = CG_SOURCE;
    //
    // Resource type for PNG and vertex progs. This is OPTIONAL. No conflict with linux...
    //
    unsigned int hModule = 0;
#ifdef WIN32
    hModule = (unsigned int)GetModuleHandle("cg_explosion.dll");
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
    // Cg Prog
    //
    if(fragmentProg)
        cgDestroyProgram(fragmentProg);
    if(vertexProg) 
        cgDestroyProgram(vertexProg);
    if(fragmentProg2)
        cgDestroyProgram(fragmentProg2);
    if(vertexProg2) 
        cgDestroyProgram(vertexProg2);
    if(Context) 
        cgDestroyContext(Context);

    Context = cgCreateContext();
    assert(Context != NULL);
    //
    // Get the latest program available
    //
    vertexprofile = CG_PROFILE_ARBVP1;//cgGLGetLatestProfile(CG_GL_VERTEX);
    cgGLSetOptimalOptions(vertexprofile);
    fragmentprofile = CG_PROFILE_ARBFP1;//cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgGLSetOptimalOptions(fragmentprofile);
    LOGMSG(LOG_MSG, "using vertex profile %s for Disk effect", cgGetProfileString(vertexprofile));
    LOGMSG(LOG_MSG, "using fragment profile %s for Disk effect", cgGetProfileString(fragmentprofile));

    if((fragmentprofile == CG_PROFILE_UNKNOWN)||(vertexprofile == CG_PROFILE_UNKNOWN))
    {
        LOGMSG(LOG_WARN, "profiles are missing !");
        return false;
    }

    //
    //====> Load the vertex program. Optionaly have a look in the resources
    //
    LOGMSG(LOG_STATE, "Loading %s", vpfname);
    cgProg = read_text_file(vpfname, "NV", hModule);
    if(cgProg == NULL)
    {
        LOGMSG(LOG_ERR, "failed to load %s...", vpfname);
        return false;
    }
    vertexProg = cgCreateProgram(Context, vptype, cgProg, vertexprofile, "main", NULL);
	delete [] cgProg;
    if(vertexProg == NULL)
    {
        LOGMSG(LOG_ERR, "failed create the program cgCreateProgram(%s)", cgGetProfileString(vertexprofile));
        LOGMSG(LOG_MSG, cgGetLastListing(Context));
        return false;
    }
    cgGLLoadProgram(vertexProg);

    param_mat_mvp = cgGetNamedParameter(vertexProg, "modelViewProj");
    param_mat_mv = cgGetNamedParameter(vertexProg, "modelView");
    /*param_mat_mvinv = cgGetNamedParameter(vertexProg, "modelViewI");
    param_mat_mvit = cgGetNamedParameter(vertexProg, "modelViewIT");*/

    if(param_mat_mvp == NULL) LOGMSG(LOG_WARN, "failed to find modelViewProj...");
    if(param_mat_mv == NULL) LOGMSG(LOG_WARN, "failed to find modelView...");
    /*if(param_mat_mvinv == NULL) LOGMSG(LOG_WARN, "failed to find modelViewI...");
    if(param_mat_mvit == NULL) LOGMSG(LOG_WARN, "failed to find modelViewIT...");*/

    param_NoiseMatrix = cgGetNamedParameter(vertexProg, "NoiseMatrix");
    if(param_NoiseMatrix == NULL) LOGMSG(LOG_WARN, "failed to find NoiseMatrix...");

    //
    //====> load the fragment program
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

    param_NoiseAmp = cgGetNamedParameter(fragmentProg, "NoiseAmp");
    if(param_NoiseAmp == NULL) 
    { 
        LOGMSG(LOG_WARN, "failed to find NoiseScale..."); 
    }
    else
    {
        cgGLSetParameter1f(param_NoiseAmp, noise_amp);
    }

    //
    // perlin texture
    //
    loadPermuteTable3();
    param_perlin = cgGetNamedParameter(fragmentProg, "PerlinTexture");
    if(param_perlin == NULL) LOGMSG(LOG_WARN, "failed to find PerlinTexture...");
    cgGLEnableTextureParameter(param_perlin); // Very important to allow Cg to setup the units
    cgGLSetTextureParameter(param_perlin, textureperlin.texture);
    //
    // load 1D texture
    //
    texture1d.bind();
    /*float table[4*256];
    for(int i=0; i<256; i++)
    {
        table[i*4 + 0] = 0;//(float)i/256.0;
        table[i*4 + 1] = 0;//(float)i/256.0;
        table[i*4 + 2] = (float)i/256.0;
        table[i*4 + 3] = (float)i/256.0;
    }
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_FLOAT, table);
    */
    glh::array2<glh::vec3ub> img;
    read_png_rgb("grad1.png", img);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, img.get_width(), 0,
                     GL_RGB, GL_UNSIGNED_BYTE, (const void *)img.get_pointer());
    CHECKGLERRORS();
    texture1d.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    texture1d.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    param_1Dtexture = cgGetNamedParameter(fragmentProg, "BaseTexture");
    if(param_1Dtexture == NULL) LOGMSG(LOG_WARN, "failed to find BaseTexture...");
    cgGLEnableTextureParameter(param_1Dtexture); // Very important to allow Cg to setup the units
    cgGLSetTextureParameter(param_1Dtexture, texture1d.texture);

    //
    // load 3D texture
    //
    param_Noise3D = cgGetNamedParameter(fragmentProg, "NoiseTexture");
    if(param_Noise3D == NULL) 
    { 
		LOGMSG(LOG_ERR, "failed to find NoiseTexture..."); 
	}
    else 
	{
        glActiveTexture( GL_TEXTURE0 );
        noisetex.bind();
        noisetex.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        noisetex.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        noisetex.parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        noisetex.parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        noisetex.parameter(GL_TEXTURE_WRAP_R, GL_REPEAT);
        CHECKGLERRORS();
        stx_srand(250673);
		if(ptrnoise)
		{
			LOGMSG(LOG_STATE, "computing FilteredNoise..."); 
	        ptrnoise->createNoiseTexture3D(64,64,64, 4, 1, FilteredNoise::ABSNOISE, noisetex);
		}
        cgGLEnableTextureParameter(param_Noise3D);
        cgGLSetTextureParameter(param_Noise3D, noisetex.texture);
	}	

    //
    //====> load the vertex program 2 for shockwave
    //
    LOGMSG(LOG_STATE, "Loading %s", vpfname2);
    cgProg = read_text_file(vpfname2, "NV", hModule);
    if(cgProg == NULL)
    {
        LOGMSG(LOG_ERR, "failed to load %s...", vpfname2);
        return false;
    }
    vertexProg2 = cgCreateProgram(Context, vptype, cgProg, vertexprofile, "main", NULL);
	delete [] cgProg;
    if(vertexProg2 == NULL)
    {
        LOGMSG(LOG_ERR, "failed create the program cgCreateProgram(%s)", cgGetProfileString(vertexprofile));
        LOGMSG(LOG_MSG, cgGetLastListing(Context));
        return false;
    }
    cgGLLoadProgram(vertexProg2);

    param_NoiseAmp2 = cgGetNamedParameter(vertexProg2, "NoiseAmp");
    if(param_NoiseAmp2 == NULL) 
    { 
        LOGMSG(LOG_WARN, "failed to find NoiseAmp..."); 
    }
    else
    {
        cgGLSetParameter1f(param_NoiseAmp2, noise_amp2);
    }
    param_mat_proj2 = cgGetNamedParameter(vertexProg2, "Proj");
    param_mat_mvp2 = cgGetNamedParameter(vertexProg2, "modelViewProj");
    param_mat_mv2 = cgGetNamedParameter(vertexProg2, "modelView");
    param_mat_mvit2 = cgGetNamedParameter(vertexProg2, "modelViewIT");
    param_NoiseMatrix2 = cgGetNamedParameter(vertexProg2, "NoiseMatrix");
    if(param_mat_proj2 == NULL) LOGMSG(LOG_WARN, "failed to find Proj...");
    if(param_mat_mvp2 == NULL) LOGMSG(LOG_WARN, "failed to find modelViewProj...");
    if(param_mat_mv2 == NULL) LOGMSG(LOG_WARN, "failed to find modelView...");
    if(param_mat_mvit2 == NULL) LOGMSG(LOG_WARN, "failed to find modelViewIT...");
    if(param_NoiseMatrix2 == NULL) LOGMSG(LOG_WARN, "failed to find NoiseMatrix...");
    //
    //====> load the fragment program 2 for shockwave
    //
    LOGMSG(LOG_STATE, "Loading %s", fpfname2);
    cgProg = read_text_file(fpfname2, "NV", hModule);
    if(cgProg == NULL)
    {
        LOGMSG(LOG_ERR, "failed to load %s...", fpfname2);
        return false;
    }
    fragmentProg2 = cgCreateProgram(Context, fptype, cgProg, fragmentprofile, "main", NULL);
	delete [] cgProg;
    if(fragmentProg2 == NULL)
    {
        LOGMSG(LOG_ERR, "failed create the program cgCreateProgram(%s)", cgGetProfileString(fragmentprofile));
        LOGMSG(LOG_MSG, cgGetLastListing(Context));
        return false;
    }
    cgGLLoadProgram(fragmentProg2);

    param_Noise3D2 = cgGetNamedParameter(fragmentProg2, "NoiseTexture");
    if(param_Noise3D2 == NULL) 
    { 
		LOGMSG(LOG_ERR, "failed to find NoiseTexture..."); 
	}
    else 
	{
        glActiveTexture( GL_TEXTURE0 );
        noisetex2.bind();
        noisetex2.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        noisetex2.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        noisetex2.parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        noisetex2.parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        noisetex2.parameter(GL_TEXTURE_WRAP_R, GL_REPEAT);
        CHECKGLERRORS();
        stx_srand(250673);
		if(ptrnoise)
		{
		    LOGMSG(LOG_STATE, "computing Perlin 3D texture...");
			ptrnoise->createVectorNoiseTexture3D(64,64,64, 4, 1, FilteredNoise::ABSNOISE, noisetex2);
		}
	    CHECKGLERRORS();
        cgGLEnableTextureParameter(param_Noise3D2);
        cgGLSetTextureParameter(param_Noise3D2, noisetex2.texture);
	}	
    //
    // load 1D texture
    //
    texture1d2.bind();
    read_png_rgb("grad2.png", img);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, img.get_width(), 0,
                     GL_RGB, GL_UNSIGNED_BYTE, (const void *)img.get_pointer());
    CHECKGLERRORS();
    texture1d2.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    texture1d2.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    param_1Dtexture2 = cgGetNamedParameter(fragmentProg, "BaseTexture");
    if(param_1Dtexture2 == NULL) LOGMSG(LOG_WARN, "failed to find BaseTexture...");
    cgGLEnableTextureParameter(param_1Dtexture2); // Very important to allow Cg to setup the units
    cgGLSetTextureParameter(param_1Dtexture2, texture1d2.texture);

	CHECKGLERRORS();
	binit = true;
    return 1;
}
/**
 ** free
 **/
void CDisc::finish()
{
    if(!binit)
        return;
    //
    // Cg Prog
    //
/*    if(fragmentProg)
        cgDestroyProgram(fragmentProg);
    if(vertexProg) 
        cgDestroyProgram(vertexProg);
    if(Context) 
        cgDestroyContext(Context);*/
	vertexProg= NULL;
	fragmentProg = NULL;
	Context = NULL;
	if(noise)
	{
	  delete noise;
	  delete pnoise;
	  noise = NULL;
	  pnoise = NULL;
	}
}
/************************************************************************/ /**
 **/
void CDisc::begin(bool mode)
{
    glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|GL_LIGHTING_BIT|GL_POLYGON_BIT);

    // enable the vertex program
    cgGLBindProgram(vertexProg);
    cgGLEnableProfile(vertexprofile);

    // build noise transform
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(noise_scale, noise_scale, noise_scale);
    glTranslatef(noise_translate[0], noise_translate[1], noise_translate[2]);
    glMatrixMode(GL_MODELVIEW);
    // set some params to the vertex program
    cgGLSetStateMatrixParameter(param_mat_mvp, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(param_NoiseMatrix, CG_GL_TEXTURE_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(param_mat_mv, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
    /*cgGLSetStateMatrixParameter(param_mat_mvinv, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE);
    cgGLSetStateMatrixParameter(param_mat_mvit, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);*/
    //cgGLSetParameter4f(param_lightpos, 10, 10, 0, 1);

    if(param_NoiseAmp)
        cgGLSetParameter1f(param_NoiseAmp, noise_amp);

    // enable the fragment program
    cgGLEnableProfile(fragmentprofile);
    cgGLBindProgram(fragmentProg);

    glActiveTexture( GL_TEXTURE0 );
    texture1d.enable(); // ???????????????????????????
    texture1d.bind();
    glActiveTexture( GL_TEXTURE1 );
	noisetex.bind();
    //textureperlin.bind();
    glActiveTexture( GL_TEXTURE0 );

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

}
/************************************************************************/ /**
    
 **/
void CDisc::end()
{
    glPopAttrib();
    cgGLDisableProfile(fragmentprofile); // same as     glBindProgramNV(GL_FRAGMENT_PROGRAM_NV, 0); ???
    cgGLDisableProfile(vertexprofile);
}
extern float frand();
extern float sfrand();

// build permuation/gradient table for noise
void CDisc::loadPermuteTable3()
{
    int table_size = 32;
    stx_srand(42);
    int *p = new int[table_size*2+2];                // permutation table
    glh::vec4f *g = new glh::vec4f[table_size*2+2];  // gradient table
    int i;

    // initalize random gradients
    for(i=0; i<table_size; i++) {
        p[i] = i;
        g[i][0] = sfrand();
        g[i][1] = sfrand();
        g[i][2] = sfrand();
        g[i].normalize();
    }

    // initialize permutation table (random shuffle)
    for(i=0; i<table_size; i++) {
        int j, t;
        j = (stx_rand() >> 4) % table_size;
        t = p[i];
        p[i] = p[j];
        p[j] = t;

        g[i][3] = (float)p[i];

        // mirror first half of table into second half
        g[i+table_size][0] = g[i][0];
        g[i+table_size][1] = g[i][1];
        g[i+table_size][2] = g[i][2];
        g[i+table_size][3] = g[i][3];
    }
    textureperlin.bind();
    glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV, table_size*2, 1, 0, GL_RGBA, GL_FLOAT, g);
    CHECKGLERRORS();
    textureperlin.parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    textureperlin.parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    delete [] p;
    delete [] g;
}

#ifndef M_PI
#define M_PI 3.1415927f
#endif
/*----------------------------------------------------------------------------------*/ /**

disc drawing function stolen from Mesa

**/ //----------------------------------------------------------------------------------
void CDisc::drawdisc_back()
{
    GLfloat theta, dtheta;
    GLfloat x, y, z;
    GLfloat s, ds;
    GLint j;
    float radius1 = in_scale;
    float radius2 = out_scale;
    
    ds = 1.0 / slices;
    dtheta = 1.0 * M_PI / (GLfloat) slices;

    glColor3f(dfadeout,0,0);

    glBegin(GL_QUAD_STRIP);
    s = 0.0;
    for (j = 0; j<=slices; j++) {
        //theta = (j==slices) ? 0.0 : j * dtheta;
        theta = j * dtheta + M_PI * 0.5f;
        x = -sin(theta);
        z = cos(theta);
        y = 0;

        glTexCoord1f(0);
        glVertex3f(x*radius1, y*radius1, z*radius1);

        glTexCoord1f(1);
        glVertex3f(x*radius2, y*radius2, z*radius2);
        s += ds;
    }
    glEnd();
}
/*----------------------------------------------------------------------------------*/ /**

disc drawing function stolen from Mesa

**/ //----------------------------------------------------------------------------------
void CDisc::drawdisc_front()
{
    GLfloat theta, dtheta;
    GLfloat x, y, z;
    GLfloat s, ds;
    GLint j;
    float radius1 = in_scale;
    float radius2 = out_scale;
    
    ds = 1.0 / slices;
    dtheta = 1.0 * M_PI / (GLfloat) slices;

    glColor3f(dfadeout,0,0);

    glBegin(GL_QUAD_STRIP);
    s = 0.0;
    for (j = 0; j<=slices; j++) {
        //theta = (j==slices) ? 0.0 : j * dtheta;
        theta = j * dtheta + M_PI * 1.5f;
        x = -sin(theta);
        z = cos(theta);
        y = 0;

        glTexCoord1f(0);
        glVertex3f(x*radius1, y*radius1, z*radius1);

        glTexCoord1f(1);
        glVertex3f(x*radius2, y*radius2, z*radius2);
        s += ds;
    }
    glEnd();
}
/*----------------------------------------------------------------------------------*/ /**



**/ //----------------------------------------------------------------------------------
void CDisc::draw_shockwave()
{
    GLfloat theta, dtheta;
    GLfloat x, y, z;
    GLfloat s, ds;
    GLint j;
    float radius1 = in_scale;
    float radius2 = out_scale;
    
    ds = 1.0 / slices;
    dtheta = 2.0 * M_PI / (GLfloat) slices;

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_DEPTH_TEST);

    cgGLEnableProfile(vertexprofile);
    cgGLEnableProfile(fragmentprofile);
    cgGLBindProgram(vertexProg2);
    cgGLBindProgram(fragmentProg2);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(noise_scale2, noise_scale2, noise_scale2);
    glMatrixMode(GL_MODELVIEW);
    cgGLSetStateMatrixParameter(param_mat_proj2, CG_GL_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(param_mat_mvp2, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(param_NoiseMatrix2, CG_GL_TEXTURE_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(param_mat_mv2, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(param_mat_mvit2, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);

    if(param_NoiseAmp2)
        cgGLSetParameter1f(param_NoiseAmp2, noise_amp2);
    glActiveTexture( GL_TEXTURE1 );
	texture1d2.bind();
    glActiveTexture( GL_TEXTURE0 );
	noisetex2.bind();
    cgGLSetTextureParameter(param_Noise3D2, noisetex2.texture);
    cgGLSetTextureParameter(param_1Dtexture2, texture1d2.texture);

    glColor3f(dfadeout,1,0);

    glBegin(GL_QUAD_STRIP);
    s = 0.0;
    for (j = 0; j<=slices; j++) {
        //theta = (j==slices) ? 0.0 : j * dtheta;
        theta = j * dtheta + M_PI * 1.5f;
        x = -sin(theta);
        z = cos(theta);
        y = 0;

		glNormal3f(x,y,z);
		glTexCoord1f(0);
        glVertex3f(1.1f*x*radius2, (y-shockwave)*radius2, 1.1f*z*radius2);

		glNormal3f(x,y,z);
		glTexCoord1f(1);
        glVertex3f(1.1f*x*radius2, (y+shockwave)*radius2, 1.1f*z*radius2);
        s += ds;
    }
    glEnd();

    cgGLDisableProfile(fragmentprofile); // same as     glBindProgramNV(GL_FRAGMENT_PROGRAM_NV, 0); ???
    cgGLDisableProfile(vertexprofile);
}
/*----------------------------------------------------------------------------------*/ /**

adjust params over a time line

**/ //----------------------------------------------------------------------------------
void CDisc::playtime(float t)
{
    static float oldt = 0;
    if(oldt == t)
        return;

	noise_translate[1] += 0.01f;
	FLUSHPTR(noise_translate);

    oldt = t;
}
