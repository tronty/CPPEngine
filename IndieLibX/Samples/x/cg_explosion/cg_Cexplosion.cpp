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
#include <nv_dds/nv_dds.h>
#include <shared/read_text_file.h>
#include <shared/array_texture.h>

#include <math.h>

#include <vector>
#include <string>

#include "cg_Cexplosion.h"
#include "glError.h"

//
// Externals
//
extern data_path progpath;
extern CPerlinNoise perlinnoise;
extern void SND_BOOM();


#if defined(WIN32)

#define CHECKCGERROR() \
    Ret = cgGetError();\
    if(Ret != CG_NO_ERROR)\
    {\
        LOGMSG(LOG_ERR,"CgError : %s", cgGetErrorString(Ret));\
    }

#elif defined(UNIX)

#define CHECKCGERROR() \
    Ret = cgGetError();\
    if(Ret != CG_NO_ERROR)\
    {\
        fprintf(stderr,"CgError : %s", cgGetErrorString(Ret));\
    }

#endif

static string error_msg;

glh::tex_object_3D *CNV30Effect::noisetex = NULL;
FilteredNoiseEx  *CNV30Effect::noise = NULL;

const char * CNV30Effect::get_error_msg()
{
    return error_msg.c_str();
}

CNV30Effect::CNV30Effect() :
  binit(false),
  Context(NULL),
  vertexProg(NULL)
  {
    objx = 0.0f;
    objy = 0.0f;
    objz = 0.0f;
    ss_lifetime = 0.001;
    ss_starttime = -10.0f;
    gradients = NULL;
    displacement = 1.27;
    noisescale = 1.55;
    noise_rotate = 54;
    freq = 1;
    amp = 1;
    sfadeout = 1;

    sphscale[0] = 0.39;
    sphscale[1] = 0.39;
    sphscale[2] = 0.39;
    sphslices = 120;
    texscale = 0.4;
    texbias = 0;
    moveup = 0.05;
    silouette=0;
    silouettelength=0.5;
  }
CNV30Effect::~CNV30Effect()
{
	if(gradients)
		delete gradients;
	if(noise)
		delete noise;
	if(noisetex)
		delete noisetex;
	noisetex = NULL;
	gradients= NULL;
	noise = NULL;
}
/**
 ** Init for opengl
 **/
bool CNV30Effect::init(const char * vpfname, const char * fpfname)
{
	string pathname;
    char *cgProg;
    CGenum fptype, vptype;
    glh::tex_object_2D tex;

	//
	// Load and connect meshes, blendshape and some curves
	//
	std::string file;
    //
    // Check for the type of source code
    //
    if(!vpfname)
        vpfname = "cg_explosion_vertex.cg";
    if(!fpfname)
        fpfname = "cg_explosion_fragment.cg";
    if((!strcmp(&fpfname[strlen(fpfname)-3], ".cg")))
        fptype = CG_SOURCE;
    else if(!strcmp(&fpfname[strlen(fpfname)-3], ".fp"))
        fptype = CG_OBJECT;
    else
    {
        LOGMSG(LOG_ERR, "bad suffix for the fragment program : %s", fpfname);
        return false;
    }
    if((!strcmp(&vpfname[strlen(vpfname)-3], ".cg")))
        vptype = CG_SOURCE;
    else if(!strcmp(&vpfname[strlen(vpfname)-3], ".vp"))
        vptype = CG_OBJECT;
    else
    {
        LOGMSG(LOG_ERR, "bad suffix for the vertex program : %s", vpfname);
        return false;
    }
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
    if (!glh_init_extensions("GL_VERSION_1_4 GL_ARB_multitexture"))
    {
        error_msg += "need this extension(s): ";
        error_msg += glh_get_unsupported_extensions();
        LOGMSG(LOG_ERR, error_msg.c_str());
        return false;
    }
    //
    // Cg Prog
    //
    if(fragmentProg)
        cgDestroyProgram(fragmentProg);
    if(vertexProg) 
        cgDestroyProgram(vertexProg);
    if(Context) 
        cgDestroyContext(Context);

    Context = cgCreateContext();
    assert(Context != NULL);
    //
    // Get the latest program available
    //
    vertexprofile = CG_PROFILE_ARBVP1;//cgGLGetLatestProfile(CG_GL_VERTEX); if you want the latest...
    cgGLSetOptimalOptions(vertexprofile);
    fragmentprofile = CG_PROFILE_ARBFP1;//cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgGLSetOptimalOptions(fragmentprofile);
    LOGMSG(LOG_MSG, "using vertex profile %s for explosion sphere", cgGetProfileString(vertexprofile));
    LOGMSG(LOG_MSG, "using fragment profile %s for explosion sphere", cgGetProfileString(fragmentprofile));

    if((fragmentprofile == CG_PROFILE_UNKNOWN)||(vertexprofile == CG_PROFILE_UNKNOWN))
    {
        error_msg += "profiles are missing !";
        return false;
    }

    //
    // Load the vertex program. Optionaly have a look in the resources
    //
    LOGMSG(LOG_MSG, "Loading %s", vpfname);
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
    param_mat_mvinv = cgGetNamedParameter(vertexProg, "modelViewI");
    param_mat_mvit = cgGetNamedParameter(vertexProg, "modelViewIT");

    if(param_mat_mvp == NULL) LOGMSG(LOG_WARN, "failed to find modelViewProj...");
    if(param_mat_mv == NULL) LOGMSG(LOG_WARN, "failed to find modelView...");
    if(param_mat_mvinv == NULL) LOGMSG(LOG_WARN, "failed to find modelViewI...");
    if(param_mat_mvit == NULL) LOGMSG(LOG_WARN, "failed to find modelViewIT...");

    param_NoiseMatrix = cgGetNamedParameter(vertexProg, "NoiseMatrix");
    if(param_NoiseMatrix == NULL) LOGMSG(LOG_WARN, "failed to find NoiseMatrix...");

    param_permutetable = cgGetNamedParameter(vertexProg, "pg");
    if(param_permutetable == NULL) LOGMSG(LOG_WARN, "failed to find permutetable pg...");
    loadPermuteTable();

    param_displacement = cgGetNamedParameter(vertexProg, "Displacement");
    if(param_displacement == NULL) LOGMSG(LOG_WARN, "failed to find Displacement...");
    if(param_displacement)
    {
        cgGLSetParameter1f(param_displacement, displacement );
    }

    param_scale = cgGetNamedParameter(vertexProg, "Scale");
    if(param_scale == NULL) LOGMSG(LOG_WARN, "failed to find Scale...");
    if(param_scale)
    {
        cgGLSetParameter1f(param_scale, noisescale);
    }

    param_FreqAmp = cgGetNamedParameter(vertexProg, "FreqAmp");
    if(param_FreqAmp == NULL) LOGMSG(LOG_WARN, "failed to find FreqAmp...");
    if(param_FreqAmp)
    {
        cgGLSetParameter2f(param_FreqAmp, freq, amp);
    }
    //
    // load the fragment program
    //
    LOGMSG(LOG_MSG, "Loading %s", fpfname);
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
    param_scalebias = cgGetNamedParameter(fragmentProg, "scalebias");
    if(param_scalebias == NULL) LOGMSG(LOG_WARN, "failed to find scalebias param...");

    //
    // load 3D texture
    //
    param_Noise3D = cgGetNamedParameter(fragmentProg, "NoiseTexture");
    if(param_Noise3D == NULL) 
    { LOGMSG(LOG_ERR, "failed to find NoiseTexture..."); }
    else {
        glActiveTexture( GL_TEXTURE0 );
		if(!noisetex)
			noisetex = new glh::tex_object_3D();
        noisetex->bind();
        noisetex->parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        noisetex->parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        noisetex->parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        noisetex->parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        noisetex->parameter(GL_TEXTURE_WRAP_R, GL_REPEAT);
        CHECKGLERRORS();
        stx_srand(250673);
		if(!noise)
		{
			noise = new FilteredNoiseEx(16,16,16);
			noise->createNoiseTexture3D(64,64,64, 4, 1, FilteredNoise:: NOISE, *noisetex);
		}
        cgGLEnableTextureParameter(param_Noise3D);
        cgGLSetTextureParameter(param_Noise3D, noisetex->texture);
    }
    //
    // load 1D texture
    //
	nv_dds::CDDSImage ddsimage;
	pathname = progpath.get_file("grad3.dds");
	if(!pathname.empty())
	{
		if(ddsimage.load(pathname))
		{
			glh::array2<glh::vec4ub> img;
			ramptex.bind();
			if(!ddsimage.upload_texture1D())
			{
				LOGMSG(LOG_ERR, "failed to upload grad3.dds in 1D OpenGL texture");
			}
			CHECKGLERRORS();
			ramptex.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			ramptex.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			ramptex.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			ramptex.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			param_Ramp1D = cgGetNamedParameter(fragmentProg, "RampTexture");
			if(param_Ramp1D == NULL) LOGMSG(LOG_WARN, "failed to find RampTexture...");
			cgGLEnableTextureParameter(param_Ramp1D); // Very important to allow Cg to setup the units
			cgGLSetTextureParameter(param_Ramp1D, ramptex.texture);
		}
		else
		{
			LOGMSG(LOG_ERR, "failed to load grad3.dds");
		}
	}
	else
	{
		LOGMSG(LOG_ERR, "grad3.dds not found");
	}
	binit = true;
    return true;
}
/**
 ** free
 **/
void CNV30Effect::finish()
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
	if(noisetex)
		delete noisetex;
	if(noise)
		delete noise;

	noise = NULL;
	noisetex = NULL;
	vertexProg= NULL;
	fragmentProg = NULL;
	Context = NULL;
}
/************************************************************************/ /**
 **/
void CNV30Effect::begin(bool mode)
{
    glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|GL_LIGHTING_BIT|GL_POLYGON_BIT);

    // enable the vertex program
    cgGLBindProgram(vertexProg);
    cgGLEnableProfile(vertexprofile);

    //
    // build noise transform
    //
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glRotatef(noise_rotate, 0,1,0);
    glRotatef(noise_rotate*0.7, 0,0,1);
glGetFloatv(GL_TEXTURE_MATRIX, noisematrixcache); // save it because we'll need it outside...
    glMatrixMode(GL_MODELVIEW);
    //
    // set some params to the vertex program
    //
    if(param_mat_mvp)
        cgGLSetStateMatrixParameter(param_mat_mvp, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    if(param_NoiseMatrix)
        cgGLSetStateMatrixParameter(param_NoiseMatrix, CG_GL_TEXTURE_MATRIX, CG_GL_MATRIX_IDENTITY);
    if(param_mat_mv)
        cgGLSetStateMatrixParameter(param_mat_mv, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
    if(param_mat_mvinv)
        cgGLSetStateMatrixParameter(param_mat_mvinv, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE);
    if(param_mat_mvit)
        cgGLSetStateMatrixParameter(param_mat_mvit, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);

    if(param_displacement)
        cgGLSetParameter1f(param_displacement, displacement );
    if(param_scale)
        cgGLSetParameter1f(param_scale, noisescale);

    if(param_FreqAmp)
        cgGLSetParameter2f(param_FreqAmp, freq, amp);
	//
	// fragment params
	//
	if(param_scalebias)	cgGLSetParameter2f(param_scalebias, texscale, texbias);
    glActiveTexture( GL_TEXTURE1 ); // ???? should not be needed !!
	ramptex.bind();
	cgGLSetTextureParameter(param_Ramp1D, ramptex.texture);
    glActiveTexture( GL_TEXTURE0 );
	if(noisetex)
  {
    noisetex->bind();
	  cgGLSetTextureParameter(param_Noise3D, noisetex->texture);
  }
  //
  // Send the eye pos in object space
  //

  // enable the fragment program
  cgGLEnableProfile(fragmentprofile);
  cgGLBindProgram(fragmentProg);

  if(noisetex) noisetex->bind(); // ???????????????

  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
}
/************************************************************************/ /**
    
 **/
void CNV30Effect::end()
{
    glPopAttrib();
    cgGLDisableProfile(fragmentprofile); // same as     glBindProgramNV(GL_FRAGMENT_PROGRAM_NV, 0); ???
    cgGLDisableProfile(vertexprofile);
}
/*----------------------------------------------------------------------------------*/ /**

returns a random floating point number between 0.0 and 1.0

**/ //----------------------------------------------------------------------------------
float frand()
{
    return (float) (stx_rand() / (float) RAND_MAX);
}
/*----------------------------------------------------------------------------------*/ /**

returns a random floating point number between -1.0 and 1.0

**/ //----------------------------------------------------------------------------------
float sfrand()
{
    return (float) (stx_rand() * 2.0/ (float) RAND_MAX) - 1.0;
}

// build permuation/gradient table for noise
void CNV30Effect::loadPermuteTable()
{
    int table_size = 32;
    stx_srand(42);
    int *p = new int[table_size*2+2];                // permutation table
    glh::vec4f *g;
    if(gradients)
        delete [] gradients;
    gradients = new glh::vec4f[table_size*2+2];  // gradient table
    g = gradients;
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

        g[i][3] = (float) p[i];

        // mirror first half of table into second half
        g[i+table_size][0] = g[i][0];
        g[i+table_size][1] = g[i][1];
        g[i+table_size][2] = g[i][2];
        g[i+table_size][3] = g[i][3];
    }

    cgGLSetParameterArray4f(param_permutetable, 0, table_size*2, g[0].v);
    CHECKGLERRORS();
    cgGLSetParameterArray4f(param_permutetable, (table_size*2), 1, g[0].v);
    CHECKGLERRORS();
    cgGLSetParameterArray4f(param_permutetable, (table_size*2)+1, 1, g[1].v);
    CHECKGLERRORS();

    delete [] p;
}

#ifndef M_PI
#define M_PI 3.141592654f
#endif

/*----------------------------------------------------------------------------------*/ /**

Sphere drawing function stolen from Mesa

\remark using and posponed mode for the plugs.
Thus we need to call plug_updateFlushes() in order to update \b all the data through the connections.
This avoids to make redundant updates through various connections

**/ //----------------------------------------------------------------------------------
void CNV30Effect::drawMesh()
{
}
inline float yexpand(float y)
{
}
inline float fnsilouette(float y)
{
	y*=0.33f;
	if(y<0)
		y=0;
	if(y>1)
		y=1;
	return 0.1f+16.2*(y*y)-59.4944*(y*y*y)+74.8889*(y*y*y*y)-30.6944*(y*y*y*y*y);
}
inline float fnsilouette_d(float y)
{
	y*=0.33f;
	if(y<0)
		y=0;
	if(y>1)
		y=1;
	return 32.4*y-178.483*(y*y)+299.556*(y*y*y)-153.472*(y*y*y*y);
}
/*----------------------------------------------------------------------------------*/ /**

Sphere drawing & silouette drawing

  OK: not optimized at all. But, not at all !!

**/ //----------------------------------------------------------------------------------
void CNV30Effect::drawSphere(int mode)
{
    GLfloat rho, drho, theta, dtheta;
    GLfloat x, y, z;
    GLfloat s, t, ds, dt;
    GLint i, j;
    
    /* texturing: s goes from 0.0/0.25/0.5/0.75/1.0 at +y/+x/-y/-x/+y axis */
    /* t goes from -1.0/+1.0 at z = -radius/+radius (linear along longitudes) */
    /* cannot use triangle fan on texturing (s coord. at top/bottom tip varies) */
    
    int slices = (int)sphslices;
    int stacks = slices;

    ds = 1.0 / sphslices;//slices;
    dt = 1.0 / sphslices;//stacks;
    t = 1.0;
    drho = M_PI / (GLfloat) stacks;
    dtheta = 2.0 * M_PI / (GLfloat) slices;
    
    /* draw intermediate stacks as quad strips */
    for (i= 0; i < stacks; i++) {
        rho = i * drho;

        glColor4f(0,0,0,sfadeout);

        glBegin(GL_QUAD_STRIP);
        s = 0.0;
        for (j = 0; j<=slices; j++) {
            theta = (j==slices) ? 0.0 : j * dtheta;
            x = -sin(theta) * sin(rho);
            z = cos(theta) * sin(rho);
            y = -cos(rho);

			if(mode==1)
			{
	            glNormal3f(x, y, z);
				y += 1;
	            glVertex3f(objx+x*sphscale[0], objy+y*sphscale[1], objz+z*sphscale[2]);
			}
			else if(silouette > 0.001f)
			{
				y += 1+(sphscale[1]*moveup);
				x *= sphscale[0]; y *= sphscale[1]; z *= sphscale[2];
				float c = fnsilouette(y*silouettelength)*silouette;
				// find back new normal
				if(fabs(cos(rho)) > 0.90f)
				{
					float nx = -sin(theta) * sin(rho);
					float nz = cos(theta) * sin(rho);
					float ny = -cos(rho);
					glNormal3f(nx,ny,nz);
				}
				else
				{
					float dc = fnsilouette_d(y*silouettelength)*silouette;
					float dc2 = 1.0f/sqrtf(dc*dc+1);
					float nx = sin(rho);
					float ny = -cos(rho);
					float nx2 = (nx*dc2)+(ny*dc*dc2);
					float ny2 = ((ny*dc2)-(nx*dc*dc2));
					float nx3 = -sin(theta)*nx2;
					float nz3 = cos(theta)*nx2;
					glNormal3f(nx3, ny2, nz3);
				}
	            glVertex3f(objx+x*c, objy+y, objz+z*c);
			}
			else
			{
	            glNormal3f(x, y, z);
	            glVertex3f(objx+x*sphscale[0], objy+y*sphscale[1], objz+z*sphscale[2]);
			}

            x = -sin(theta) * sin(rho+drho);
            z = cos(theta) * sin(rho+drho);
            y = -cos(rho+drho);

			if(mode==1)
			{
	            glNormal3f(x, y, z);
				y += 1;
	            glVertex3f(objx+x*sphscale[0], objy+y*sphscale[1], objz+z*sphscale[2]);
			}
			else if(silouette > 0.001f)
			{
				y += 1+(sphscale[1]*moveup);
				x *= sphscale[0]; y *= sphscale[1]; z *= sphscale[2];
				float c = fnsilouette(y*silouettelength)*silouette;
				// find back new normal
				if(fabs(cos(rho+drho)) > 0.90f)
				{
					float nx = -sin(theta) * sin(rho+drho);
					float nz = cos(theta) * sin(rho+drho);
					float ny = -cos(rho+drho);
					glNormal3f(nx,ny,nz);
				}
				else
				{
					float dc = fnsilouette_d(y*silouettelength)*silouette;
					float dc2 = 1.0f/sqrtf(dc*dc+1);
					float nx = sin(rho+drho);
					float ny = -cos(rho+drho);
					float nx2 = (nx*dc2)+(ny*dc*dc2);
					float ny2 = ((ny*dc2)-(nx*dc*dc2));
					float nx3 = -sin(theta)*nx2;
					float nz3 = cos(theta)*nx2;
					glNormal3f(nx3, ny2, nz3);
				}
	            glVertex3f(objx+x*c, objy+y, objz+z*c);
			}
			else
			{
	            glNormal3f(x, y, z);
	            glVertex3f(objx+x*sphscale[0], objy+y*sphscale[1], objz+z*sphscale[2]);
			}
            s += ds;
        }
        glEnd();
        t -= dt;
    }
}

void CNV30Effect::AnimSubSphere(CNV30Effect *parent, float time)
{
	float VtxPos[3];
	float N[3];
	float t = (time - ss_starttime)/ss_lifetime;
	if((t >= 1.0) || (t<0))
	{
		t = 0;
		SND_BOOM();
		ss_starttime = time;
		ss_lifetime = 20.0f + (float)(stx_rand()&0xF);
		ss_al = (float)(stx_rand()&0xFF)*M_PI/255.0f - (0.5f*M_PI);
		ss_be = (float)(stx_rand()&0xFF)*M_PI/255.0f;
	}
	sfadeout = cosf(M_PI*0.5f*t);
	sphscale[0] = parent->sphscale[0] * 0.3f * t;
	sphscale[1] = sphscale[0];
	sphscale[2] = sphscale[0];
	glRotatef(ss_be * 180.0f / M_PI, 1,0,0);
	glRotatef(ss_al * 180.0f / M_PI, 0,1,0);

    N[0] = -sin(ss_al) * sin(ss_be);
    N[2] = cos(ss_al) * sin(ss_be);
    N[1] = cos(ss_be);
	VtxPos[0] = N[0] * parent->sphscale[0];
	VtxPos[1] = N[1] * parent->sphscale[1];
	VtxPos[2] = N[2] * parent->sphscale[2];
	perlinnoise.EvalFractalNoise(VtxPos,N, parent->noisescale , parent->displacement , parent->noisematrixcache);

	objx = VtxPos[0];
	objy = VtxPos[1];
	objz = VtxPos[2];
}
