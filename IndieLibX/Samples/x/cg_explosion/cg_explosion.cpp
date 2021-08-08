#define GLFINISHFORTIMING() 
//#define GLFINISHFORTIMING() glFinish() 

#if defined(WIN32)
# include <windows.h>
# include <shared/moviemaker.h>
# pragma warning (disable : 4786)
#elif defined(UNIX)
# include <GL/glx.h>
#endif

#ifdef MACOS
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#define MODHELPER_SINGLE_FILE

#include <Cg/cgGL.h>

#include <glh/glh_extensions.h>
#include <glh/glh_obs.h>
#include <glh/glh_glut.h>
#include <glh/glh_linear.h>
#include <shared/nvidia_logo.h>
#include <shared/read_text_file.h>
#include <math.h>
#include <string>

#include "cg_Cexplosion.h"
#include "cg_Cdisc.h"
#include "CCompositing.h"
#include "CVolumelines.h"
#include "glError.h"

#include "extra_sound.h"

//
//----> glut-ish callbacks
//
void display();
void keyfunc(unsigned char k, int x, int y);
void do_menu( int v );
void idle();
//
//----> my functions
//
void init_opengl();


unsigned char key;
int mousestate;
int mousebutton;
float	mousexy[2];

#ifdef GLH_GLUT_H
glh::glut_interactor g_glut_interactor;
#else
#endif

//using namespace glh;
//
//----> GLH controls
//
glh::glut_callbacks cb;
glh::glut_simple_mouse_interactor camera, object;

GLfloat timeline = 0.0f;
GLfloat timespeed = 0.30f;
GLfloat translate[3];
GLfloat rotation[3];
GLfloat alpha = 0, beta = 45;

#if defined(WIN32)
static MovieMaker   *lpMovie = NULL;
#endif

//#include "cg_Cexplosion.h"
#define NSUBSPHERES 3
#define NVLINES 10

CNV30Effect nv30effectsphere;
CNV30Effect nv30effectsubspheres[NSUBSPHERES];
CDisc nv30effectdisc;
CCompositing compositing;
CVLines vline;
CPerlinNoise perlinnoise;

VLineEntity vlines[NVLINES];
vector<VLineEntity *> vvlines;
bool sortfunctor(VLineEntity *a, VLineEntity *b)
{
	if(a->MVPos[2] < b->MVPos[2])
		return true;
	return false;
}

CurvePool curvepool;

GLfloat light_pos[] = {5,5,10,1};
GLfloat lightDiff[] = {1,1,1,1};
GLfloat lightSpec[] = {0.9,0.9,0.9,1};
GLfloat shinefact = 50;

CNVLogo nvlogo;

bool b[256];
int bInitialized = 0;
int divisions = 50;

data_path progpath;
/**
Overloading the glut_perspective_reshaper because we want to catch window resizing for the PBuffer
 **/
class perspective_reshaper : public glh::glut_perspective_reshaper
{
public:
    perspective_reshaper(float infovy = 60.f, float inzNear = .1f, float inzFar = 100.f) :
      glut_perspective_reshaper(infovy, inzNear, inzFar)
	  { disable(); }
    void reshape(int w, int h)
    {
        glut_perspective_reshaper::reshape(w, h);
		
		compositing.reshape(w,h);
    }
};
perspective_reshaper reshaper;

#include "extra_windows.h"

#ifdef WIN32
//----------------------------------------------------------------------------------
//
/// Since we cannot quit glutMainLoop() in main(), let's do cleanup here...
//
//----------------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
                       __DWORD__  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
    switch(ul_reason_for_call)
    {
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_DETACH:
        CloseExtraWindows();
        CLOSEMODULES();
        break;
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
        break;
    }
    return TRUE;
}
#endif
//-------------------------------------------------------------------
//----> Main entry point
//
int main(int argc, char **argv)
{ 
	timeline = 0;
	translate[0] = 0; translate[1] = 0; translate[2] = 0; 
	rotation[0] = 0; rotation[1] = 0; rotation[2] = 0; 

  for(int i=0; i<NSUBSPHERES; i++)
  {
    nv30effectsubspheres[i].displacement = 0.62;
    nv30effectsubspheres[i].noisescale = 1.94;
    nv30effectsubspheres[i].noise_rotate = 54;
    nv30effectsubspheres[i].sphslices = 30;
    nv30effectsubspheres[i].texscale = 0.4;
  }

    progpath.path.clear();
    progpath.path.push_back(".");
    progpath.path.push_back("../../../../MEDIA/models/cg_explosion");
    progpath.path.push_back("../../../../MEDIA/textures");
    progpath.path.push_back("../../../../MEDIA/textures/1D");
    progpath.path.push_back("../../../../MEDIA/textures/2D");
    progpath.path.push_back("../../../../MEDIA/textures/3D");
    progpath.path.push_back("../../../../MEDIA/programs/cg_explosion");
	set_text_path(progpath);
	//
    // Init the glut
    //
    glutInit(&argc, argv);
    //
    //  Display mode through the glut
    //
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowSize(800,600);
    glutCreateWindow("NRenderGL"); // NRenderGL is for the sound of the demo team...either NRenderGL or GLUT

    OPENMODULES("opengl");
    glh::glut_add_interactor(&g_glut_interactor);
	//
	// Optional sound init
	//
 	SND_INIT();
	//
	// GUI
	//
    InitExtraWindows(); // in extra_windows.h

    b['p'] = true;
    b['e'] = true;
    b['o'] = true;
    b['l'] = false;
    b['d'] = false;
    b['q'] = false;
    b['c'] = false;
    b[' '] = true;
    b['w'] = false;
    b['m'] = false;
    b['v'] = false;
    b['h'] = false;
    b['s'] = true;
	b['r'] = true;
	b['t'] = true;

    //
    //  Init my stuff
    //
    LOGMSG(LOG_STATE, "initialize OpenGL");
	// some curves will be stored in this container. Python will help doing this.
    init_opengl();
	//
	// init the curve system
	//
    CV_INIT();
    CV_CREATE_FROM_FILE(NULL, "..\\..\\..\\..\\MEDIA\\models\\cg_explosion\\cv.txt");
    CV_CREATE_FROM_FILE(NULL, "..\\..\\..\\..\\MEDIA\\models\\cg_explosion\\cvnoise.txt");
	//
	// timeline feeding the animation curves
	//
	CV_CONNECT_IN("cvvolume", &timeline);
	CV_CONNECT_IN("cvbias", &timeline);
	CV_CONNECT_IN("cvscale", &timeline);
	CV_CONNECT_IN("disc_in_scale", &timeline);
	CV_CONNECT_IN("disc_out_scale", &timeline);
	CV_CONNECT_IN("explosion_fadeout", &timeline);
	CV_CONNECT_IN("explosion_rotation", &timeline);
	CV_CONNECT_IN("explosion_scale", &timeline);
	CV_CONNECT_IN("explosion_scale2", &timeline);
	CV_CONNECT_IN("explosion_translate", &timeline);
	CV_CONNECT_IN("cvshock", &timeline);
	CV_CONNECT_IN("cvnoiserot", &timeline);
	CV_CONNECT_IN("cvnoisescale", &timeline);
	CV_CONNECT_IN("cvnoisedisplacement", &timeline);
	//
	// output of the animation curves feeding the variables (using plugs...)
	//
	CV_CONNECT_OUT1D("cvvolume", &bgndvolume);
	CV_CONNECT_OUT1D("cvbias",  &nv30effectsphere.texbias);
	CV_CONNECT_OUT1D("cvscale",  &nv30effectsphere.texscale);

	CV_CONNECT_OUT1D("cvscale", &nv30effectsubspheres[0].texscale);
	CV_CONNECT_OUT1D("cvscale", &nv30effectsubspheres[1].texscale);
	CV_CONNECT_OUT1D("cvscale", &nv30effectsubspheres[2].texscale);

	CV_CONNECT_OUT1D("cvbias", &nv30effectsubspheres[0].texbias);
	CV_CONNECT_OUT1D("cvbias", &nv30effectsubspheres[1].texbias);
	CV_CONNECT_OUT1D("cvbias", &nv30effectsubspheres[2].texbias);

	CV_CONNECT_OUT1D("disc_in_scale", &nv30effectdisc.in_scale);
	CV_CONNECT_OUT1D("disc_out_scale", &nv30effectdisc.out_scale);

	CV_CONNECT_OUT1D("explosion_fadeout", &nv30effectsphere.sfadeout);
	CV_CONNECT_OUT3D("explosion_rotation", rotation);
	CV_CONNECT_OUT3D("explosion_scale", nv30effectsphere.sphscale);
	CV_CONNECT_OUT3D("explosion_translate", translate);

	CV_CONNECT_OUT1D("cvnoiserot", &nv30effectsphere.noise_rotate);
	CV_CONNECT_OUT1D("cvnoisescale", &nv30effectsphere.noisescale);
	CV_CONNECT_OUT1D("cvnoisedisplacement", &nv30effectsphere.displacement);
	CV_CONNECT_OUT1D("cvshock", &nv30effectdisc.shockwave);
    //
    // glut again...
    //
    glh::glut_helpers_initialize();
    //
    //  GLH Interactors
    //
    cb.keyboard_function = keyfunc;
    camera.configure_buttons(1);
    camera.set_camera_mode(true);

    object.configure_buttons(1);
    object.dolly.dolly[2] = -6;
    object.trackball.r.set_value(-0.187038, -0.0562901, -0.0166899, 0.980597); //glh::vec3f(-0.187038, -0.0562901, -0.0166899), 0.980597);

    glh::glut_add_interactor(&cb);
    glh::glut_add_interactor(&reshaper);
    glh::glut_add_interactor(&object);
    //
    // glut Menu
    //
    glutCreateMenu( do_menu );
    glutAddMenuEntry( "wireframe [w]", 'w' );
    glutAddMenuEntry( "anim [ ]", ' ' );
    glutAddMenuEntry( "record movie [m]", 'm' );
    glutAddMenuEntry( "logo [l]", 'l' );
    glutAddMenuEntry( "update Cg code [c]", 'c' );
    glutAddMenuEntry( "update ASM code for fp & vp [a]", 'a' );
    glutAddMenuEntry( "toggle fixed/vpfp pipelines [e]", 'e' );
    glutAddMenuEntry( "inc mesh details [+]", '+' );
    glutAddMenuEntry( "inc mesh details [-]", '-' );
    glutAttachMenu( GLUT_RIGHT_BUTTON );
    //
    // glut Callback functions
    //
    glutIdleFunc(idle);
    glutDisplayFunc(display);
    //
    // Stuck into this func
    //
    glutMainLoop();

    return 0;
}
//----------------------------------------------------------------------------------
//
/// init openGL
//
//----------------------------------------------------------------------------------
void init_opengl()
{

    //
    // check for extensions : some are need
    //
    if (!glh_init_extensions("GL_VERSION_1_4 "
								"WGL_ARB_pbuffer "
								"GL_NV_texture_rectangle "
								"GL_ARB_multitexture "))
    {
        LOGMSG(LOG_ERR, "need this extension(s): %s",glh_get_unsupported_extensions());
        return;
    }
	
    //
    // Effect init
    //
    bInitialized = 1;
    bInitialized -= compositing.init() ? 0:1;

	bInitialized -= nv30effectdisc.init() ? 0:1;
	bInitialized -= nv30effectsphere.init("cg_explosion_vertex.cg", "cg_explosion_fragment.cg") ? 0:1;
	for(int i=0; i< NSUBSPHERES; i++)
	{
		bInitialized -= nv30effectsubspheres[i].init("cg_explosion_vertex.cg", "cg_explosion_fragment.cg") ? 0:1;
	}
	vline.init();
	vvlines.clear();
	for(i=0; i<NVLINES; i++)
		vvlines.push_back(&(vlines[i]));


    if(bInitialized <= 0)
    {
        LOGMSG(LOG_ERR, "Initialisation failed\n");
    }
    //
    //
    //
    nvlogo.init();
}

void do_menu( int v )
{
	int i;
    b[v] = !b[v];

    switch (v)
    {
    case  '1': //"camera off"
    	CV_DISCONNECT_IN("explosion_translate", &timeline);
    	CV_DISCONNECT_OUT("explosion_translate", translate);
	    CV_DISCONNECT_IN("explosion_rotation", &timeline);
	    CV_DISCONNECT_OUT("explosion_rotation", rotation);
      break;
    case  '2': //"camera on"
    	CV_CONNECT_IN("explosion_translate", &timeline);
    	CV_CONNECT_OUT3D("explosion_translate", translate);
	    CV_CONNECT_IN("explosion_rotation", &timeline);
	    CV_CONNECT_OUT3D("explosion_rotation", rotation);
      break;
    case  '3':
      SHOWWINDOW("parameters", true);
      break;
    case  '4':
      SHOWWINDOW("parameters", false);
      break;
    case  's': //"spherical"
			object.dolly.dolly[2] = -6;
			object.trackball.r.set_value(-0.187038, -0.0562901, -0.0166899, 0.980597);
      object.pan.pan.set_value(0,0,0);
    	CV_CONNECT_IN("explosion_translate", &timeline);
    	CV_CONNECT_OUT3D("explosion_translate", translate);
	    CV_CONNECT_IN("explosion_rotation", &timeline);
	    CV_CONNECT_OUT3D("explosion_rotation", rotation);

      CV_DISCONNECT_IN("explosion_scale2", &timeline);
    	CV_DISCONNECT_OUT("explosion_scale2", nv30effectsphere.sphscale);
    	CV_CONNECT_IN("explosion_scale", &timeline);
    	CV_CONNECT_OUT3D("explosion_scale", nv30effectsphere.sphscale);
	    CV_CONNECT_IN("cvnoiserot", &timeline);
	    CV_CONNECT_OUT1D("cvnoiserot", &nv30effectsphere.noise_rotate);
	    CV_CONNECT_IN("cvnoisescale", &timeline);
	    CV_CONNECT_OUT1D("cvnoisescale", &nv30effectsphere.noisescale);
	    CV_CONNECT_IN("cvnoisedisplacement", &timeline);
	    CV_CONNECT_OUT1D("cvnoisedisplacement", &nv30effectsphere.displacement);
      nv30effectsphere.silouette=0;
      FLUSHPTR(&nv30effectsphere.silouette);
      break;
    case  'm': //"Mushroom"
			object.dolly.dolly[2] = -6;
			object.trackball.r.set_value(0.22073, 0.100772619, 0.81732, 0.991305);
      object.pan.pan.set_value(0.44,-1.47,0);
    	CV_DISCONNECT_IN("explosion_translate", &timeline);
    	CV_DISCONNECT_OUT("explosion_translate", translate);
	    CV_DISCONNECT_IN("explosion_rotation", &timeline);
	    CV_DISCONNECT_OUT("explosion_rotation", rotation);

      CV_DISCONNECT_IN("explosion_scale", &timeline);
    	CV_DISCONNECT_OUT("explosion_scale", nv30effectsphere.sphscale);
	    CV_DISCONNECT_IN("cvnoiserot", &timeline);
	    CV_DISCONNECT_OUT("cvnoiserot", &nv30effectsphere.noise_rotate);
	    CV_DISCONNECT_IN("cvnoisescale", &timeline);
	    CV_DISCONNECT_OUT("cvnoisescale", &nv30effectsphere.noisescale);
	    CV_DISCONNECT_IN("cvnoisedisplacement", &timeline);
	    CV_DISCONNECT_OUT("cvnoisedisplacement", &nv30effectsphere.displacement);
    	CV_CONNECT_IN("explosion_scale2", &timeline);
    	CV_CONNECT_OUT3D("explosion_scale2", nv30effectsphere.sphscale);
      nv30effectsphere.silouette=1;
      FLUSHPTR(&nv30effectsphere.silouette);
      nv30effectsphere.displacement=0.6;
      FLUSHPTR(&nv30effectsphere.displacement);
      break;
		case 'u':
			object.dolly.dolly[2] = -6;
			object.trackball.r.set_value(-0.187038, -0.0562901, -0.0166899, 0.980597);
      object.pan.pan.set_value(0,0,0);
			break;
        case 'h':
			if(b['h'])
			{
				EXECUTESCRIPT(false, 
							"mainwin.hide()\n"
							"console.hide()\n"
							"params.hide()\n"
							"toolbar.hide()\n"
							);
			}
			else
			{
				EXECUTESCRIPT(false, 
							"mainwin.show()\n"
							"console.show()\n"
							"params.show()\n"
							"toolbar.show()\n"
							);
			}
			break;
#ifdef WIN32
        case 'v':
        {
        static int mcnt = 0;
        if(b['v'])
        {
            char strtmp[100];
            sprintf(strtmp, "movie_%d.avi", mcnt++);
            lpMovie = new MovieMaker;
            lpMovie->StartCapture(strtmp);
            LOGMSG(LOG_STATE,"starting recording %s", strtmp);
        }
        else
        {
            LOGMSG(LOG_STATE,"stopped recording.");
            lpMovie->EndCapture();
            delete lpMovie;
            lpMovie = NULL;
        }
        break;
        }
#endif
    case 'e':
        break;
    case 27:
#if defined(WIN32)
        if(lpMovie)
        {
            lpMovie->EndCapture();
            delete lpMovie;
        }
#endif
		compositing.pbuffer1->Activate();
			nv30effectdisc.finish();
			nv30effectsphere.finish();
			for(i=0; i< NSUBSPHERES; i++)
			{
				nv30effectsubspheres[i].finish();
			}
			vline.finish();
		compositing.pbuffer1->Deactivate();
		compositing.finish();
		glutPassiveMotionFunc(NULL);
        CloseExtraWindows();
		curvepool.Init(NULL);
        CLOSEMODULES();
        exit(0);
    //----> Update the Cg codes
    case 'c':
        LOGMSG(LOG_STATE, "Updating vertex/fragments through specified code...");

        bInitialized = 1;
		bInitialized -= compositing.init() ? 0:1;

		compositing.pbuffer1->Activate();
			glClearColor(.15f, .15f, .25f, 1);
			bInitialized -= nv30effectdisc.init() ? 0:1;
			bInitialized -= nv30effectsphere.init("cg_explosion_vertex.cg", "cg_explosion_fragment.cg") ? 0:1;
			for(i=0; i< NSUBSPHERES; i++)
			{
				bInitialized -= nv30effectsubspheres[i].init("cg_explosion_vertex.cg", "cg_explosion_fragment.cg") ? 0:1;
			}

			vline.init();

		compositing.pbuffer1->Deactivate();
        if (bInitialized <= 0) 
        {
            LOGMSG(LOG_ERR, "Initialisation failed: %s. Please Quit\n");
        }
        break;
    }

    glutPostRedisplay();
}
/************************************************************************/ /**
    
 **/
void keyfunc(unsigned char k, int x, int y)
{
    do_menu((int)k);
}
/************************************************************************/ /**
    
 **/
void idle()
{
    if(bInitialized && b[' '])
    {
      timeline = timeline + timespeed;
      FLUSHPTR(&timeline);
      if(timeline > 150)
      {
        //stx_srand(22);
        SND_START();
        timeline = 0;
        for(int i=0; i<NVLINES; i++)
        {
          vlines[i].ss_starttime = 10000.0f;
        }
      }
      nv30effectdisc.playtime(timeline);
      SND_UPDATE(false);
    }
    else 
    {
		SND_UPDATE(true);
#if defined(WIN32)
        Sleep(10);
#endif
    }
    glutPostRedisplay();
}
#ifndef M_PI
#define M_PI 3.1415927f
#endif

/************************************************************************/ /**
    
 **/
void display()
{
    if (!bInitialized) 
        return;

	//
	//====> First stage : render to RGB buffer
	//
	if(b['p'])
		compositing.pbuffer1->Activate();

	glEnable(GL_DEPTH_TEST);
    //
    //  apply various transformations
    //
    glLoadIdentity();
     camera.apply_inverse_transform(); // glh...
    object.apply_transform();
    //glRotatef(alpha, 0,1,0);
	glTranslatef(translate[0], translate[1], translate[2]);
    glRotatef(rotation[2], 0,0,1);
    glRotatef(rotation[1], 0,1,0);
    glRotatef(rotation[0], 1,0,0);

    //
    // second : Render
    //
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    // various settings
    //

    glPolygonMode(GL_FRONT_AND_BACK, b['w'] ? GL_LINE : GL_FILL);

    if(glActiveTexture)
    {
        glActiveTexture( GL_TEXTURE1 );
        glDisable(GL_TEXTURE_2D);
        glActiveTexture( GL_TEXTURE0 );
    }
    /*glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shinefact);*/

	glViewport(0,0,compositing.pbuffer1->GetWidth(),compositing.pbuffer1->GetHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	reshaper.apply_perspective();
	glMatrixMode(GL_MODELVIEW);
	//reshaper.apply();

    //
    //----> render the disc
    //
	if(b['e'])
	{
		nv30effectdisc.begin();
		nv30effectdisc.drawdisc_back();
		nv30effectdisc.end();
		//
		//----> EFFECT #2 : render the sphere
		//
		nv30effectsphere.begin();
		nv30effectsphere.drawSphere();
		//nv30effectsphere.drawMesh();
		nv30effectsphere.end();
		GLFINISHFORTIMING(); // for timing purpose
		//
		//----> EFFECT #3 : render the sphere #2
		//
		if(nv30effectsphere.silouette <= 0.0001f)
		{
			for(int i=0; i< NSUBSPHERES; i++)
			{
				glPushMatrix();
				nv30effectsubspheres[i].AnimSubSphere(&nv30effectsphere, timeline);
				nv30effectsubspheres[i].begin();
				nv30effectsubspheres[i].drawSphere(1);
				nv30effectsubspheres[i].end();
				glPopMatrix();
				GLFINISHFORTIMING(); // for timing purpose
			}
		}
		//
		//----> render the disc
		//
		nv30effectdisc.begin();
		nv30effectdisc.drawdisc_front();
		nv30effectdisc.end();
	}
    //
    //----> render the fire particles
    //
	if((b['t']) && (nv30effectsphere.silouette <= 0.0001f))
	{
		int nlines = (int)((float)NVLINES *timeline / 150.0f);
		if(nlines > NVLINES) nlines = NVLINES;
		float mmv[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mmv);
		vvlines.clear();
		vline.lines_begin();
		for(int i=0; i<nlines; i++)
		{
			vlines[i].Anim(&nv30effectsphere, timeline, mmv);
			vvlines.push_back(&vlines[i]);
		}
		sort(vvlines.begin() , vvlines.end(), sortfunctor);
		for(i=0; i<nlines; i++)
		{
			vvlines[i]->Draw(vline);
		}
		vline.lines_end();
	}

	//
	//====> Second stage : render to 'texbem' buffer
	//
	if(b['p'])
	{
		compositing.pbuffer1->Deactivate();
		compositing.pbuffer2->Activate();
		glEnable(GL_DEPTH_TEST);
		//
		//  apply various transformations
		//
		glLoadIdentity();
		camera.apply_inverse_transform(); // glh...
		object.apply_transform();
		//glRotatef(alpha, 0,1,0);
		glTranslatef(translate[0], translate[1], translate[2]);
		glRotatef(rotation[2], 0,0,1);
		glRotatef(rotation[1], 0,1,0);
		glRotatef(rotation[0], 1,0,0);

		//
		// second : Render
		//
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//
		// various settings
		//
		glPolygonMode(GL_FRONT_AND_BACK, b['w'] ? GL_LINE : GL_FILL);
		glDisable(GL_CULL_FACE);

		glViewport(0,0,compositing.pbuffer2->GetWidth(),compositing.pbuffer2->GetHeight());
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		reshaper.apply_perspective();
		glMatrixMode(GL_MODELVIEW);
		//reshaper.apply();
	}
    //
    //----> render the disc
    //
	if(b['r']) 
	{
		nv30effectdisc.draw_shockwave();
	}
    GLFINISHFORTIMING(); // for timing purpose

	if(b['p'])
	{
		compositing.pbuffer2->Deactivate();
		compositing.draw();
	}

    if(b['l']) nvlogo.render_logo();
    glutSwapBuffers();
    CHECKGLERRORS();


#if defined(WIN32)
    if(b['v'])  
        lpMovie->Snap();
#endif

}
