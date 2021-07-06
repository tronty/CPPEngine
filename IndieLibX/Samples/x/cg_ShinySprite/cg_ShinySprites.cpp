#if defined(WIN32)
# include <windows.h>
# pragma warning (disable : 4786)
#elif defined(UNIX)
# include <GL/glx.h>
#endif

#include <GL/gl.h>
#undef GL_GLEXT_PROTOTYPES

#include <Cg/cgGL.h>

#undef GLH_EXT_SINGLE_FILE
#include <glh/glh_extensions.h>
#include <glh/glh_obs.h>
#include <glh/glh_glut.h>
#include <glh/glh_linear.h>
#include <nvparse/nvparse.h>
#include <math.h>

#include "nv_GenericTools/nv_GenericTools.h"

#include "cg_CShinySprites.h"

//using namespace glh;
//
//----> GLH controls
//
glh::glut_callbacks cb;
glh::glut_simple_mouse_interactor camera, object;

glh::tex_object_2D texture;

bool b[256];
bool bInitialized = false;
//
//----> glut-ish callbacks
//
void display();
void key(unsigned char k, int x, int y);
void menu( int v );
void idle();
//
//----> my functions
//
void init_opengl();

GLfloat m_fTestConstant1 = 0;
GLfloat m_fTestConstant2 = 3.7f;
GLfloat fsize = 0.21;
GLfloat alpha = 0, beta = 0;

CSSprite sprites;

GLfloat light_pos[] = {5,5,10,1};
GLfloat lightDiff[] = {1,1,1,1};
GLfloat lightSpec[] = {0.9,0.9,0.9,1};
GLfloat shinefact = 50;

const GLfloat PI = 3.141592654f;
#define NUMSHINES 400
glh::vec3f shines[NUMSHINES];


/**
Overloading the glut_perspective_reshaper because we want to catch window resizing for the PBuffer
 **/
class perspective_reshaper : public glh::glut_perspective_reshaper
{
public:
	perspective_reshaper(float infovy = 60.f, float inzNear = .1f, float inzFar = 10.f) :
	  glut_perspective_reshaper(infovy, inzNear, inzFar)
		  {}
	void reshape(int w, int h)
	{
		glut_perspective_reshaper::reshape(w, h);
	}
};
perspective_reshaper reshaper;


//-------------------------------------------------------------------
//----> Main entry point
//
int main(int argc, char **argv)
{
    //
    // Init the glut
    //
    glutInit(&argc, argv);
    //
    //  Display mode through the glut
    //
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowSize(512, 512);
    glutCreateWindow("Shiny Sprites Demo");

  	b['o'] = true;
    b['d'] = false;
    b['q'] = false;
    b[' '] = true;
    b['w'] = false;
    b['s'] = true;
    //
    // Additional windows
    //
    OPENMODULES("opengl");
    CREATEWINDOWCONTAINER_FOLDING("params",0,512+25,512,200,NULL);
      CREATECTRL_VALUE("shinefact", "params", &shinefact, 0, 128);
      CREATECTRL_CHECK("anim", "params", &b[' ']);
      CREATECTRL_CHECK("occlusion", "params", &b['o']);
      CREATECTRL_CHECK("light", "params", &b['d']);
      CREATECTRL_CHECK("wireframe", "params", &b['w']);
      CREATECTRL_CHECK("view_spheres", "params", &b['s']);
  CREATEWINDOWCONTAINER_SPLIT("logs",0,512+25+200+25,512,300,NULL);
      APPENDWINDOW("logwnd","logs");
      APPENDWINDOW("console","logs");
    //
    //  Init my stuff
    //
    init_opengl();
    //
    // glut again...
    //
    glh::glut_helpers_initialize();
    //
    //  GLH Interactors
    //
    cb.keyboard_function = key;
    camera.configure_buttons(1);
    camera.set_camera_mode(true);

    object.configure_buttons(1);
    object.dolly.dolly[2] = -3;
    object.trackball.r.set_value(glh::vec3f(1, 0, 0), 0);//-PI/4.);

    glh::glut_add_interactor(&cb);
    glh::glut_add_interactor(&reshaper);
    glh::glut_add_interactor(&object);
    //
    // glut Menu
    //
    glutCreateMenu( menu );
    glutAddMenuEntry( "wireframe [w]", 'w' );
    glutAddMenuEntry( "anim [ ]", ' ' );
    glutAddMenuEntry( "increase shininess [+]", '+' );
    glutAddMenuEntry( "decrease shininess [-]", '-' );
    glutAddMenuEntry( "view sphere [s]", 's' );
    glutAddMenuEntry( "specular on sphere [q]", 'q' );
    glutAddMenuEntry( "occlusion map [o]", 'o' );
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

void cleanExit(int exitval)
{
    sprites.free();
    CLOSEMODULES();
    exit(exitval);
}

void cgErrorCallback(void)
{
    CGerror LastError = cgGetError();

    if(LastError)
    {
        const char *Listing = cgGetLastListing(sprites.get_cg_context());
        STX_PRINT("\n---------------------------------------------------\n");
        STX_PRINT("%s\n\n", cgGetErrorString(LastError));
        STX_PRINT("%s\n", Listing);
        STX_PRINT("---------------------------------------------------\n");
        STX_PRINT("Cg error, exiting...\n");
        cleanExit(0);
    }
}

void init_opengl()
{

    glClearColor(.25f, .25f, .25f, 1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, lightSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shinefact);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

    cgSetErrorCallback(cgErrorCallback);

    bInitialized = sprites.init();
	if (!bInitialized) 
	{
	    STX_PRINT("Initialisation failed: %s\n", sprites.get_error_msg());
	    cleanExit(0);
	}

	#define RADIUS 1.01f
    for(int i=0; i<NUMSHINES; i++)
    {
        float be = PI*(float)(stx_rand()&255)/255.0;
        float al = 2*PI*(float)(stx_rand()&255)/255.0;
        shines[i].set_value(RADIUS*cos(al)*sin(be), RADIUS*sin(al), RADIUS*cos(al)*cos(be));
    }
}

void menu( int v )
{
    char strtmp[100];
    b[v] = !b[v];

    switch (v)
    {
    case '+':
        shinefact += 1;
        if(shinefact > 128)
            shinefact = 128;
        FLUSHPTR(&shinefact);
        break;
    case '-':
        shinefact -= 1;
        if(shinefact <= 0)
            shinefact = 1;
        FLUSHPTR(&shinefact);
        break;
    case 27:
        cleanExit(0);

    case 'z':
        m_fTestConstant1+= 0.01f;
        sprintf(strtmp,"cst1 = %f\n", m_fTestConstant1);
        break;
    case 'a':
        m_fTestConstant1-= 0.01f;
        sprintf(strtmp,"cst1 = %f\n", m_fTestConstant1);
        break;
    }


    glutPostRedisplay();
}

void key(unsigned char k, int x, int y)
{
    menu((int)k);
}

void idle()
{
    //sleep(0);
	if (bInitialized) 
	    glutPostRedisplay();
}
void display()
{
	int i;
	if (!bInitialized) 
		return;

	//
	//  apply various transformations
	//
	glPushMatrix();
	camera.apply_inverse_transform(); // glh...
	object.apply_transform();
	glRotatef(alpha, 0,1,0);
	glRotatef(beta, 1,0,0);
	//
	// First : occlusion pass
	//
	sprites.begin_occlusion(b['d']);
		glDisable(GL_LIGHTING);
		texture.disable();
		glColor3f(0.0f,0.0f,0.0f);
		if(b['s'])
		{
			glutSolidSphere(1,30,30);
			glPushMatrix();
			glTranslatef(1.5,0,0);
			glutSolidSphere(0.5,30,30);
			glPopMatrix();
		}

		sprites.set_test_cst(m_fTestConstant1*PI*2, 0);
		sprites.bind_light(light_pos, shinefact);
		sprites.begin();
		for(i=0; i< NUMSHINES; i++)
		{
			sprites.draw_sprite(shines[i], shines[i], 0.01, 0.01);
			glh::vec3f pos(shines[i]);
			pos *= 0.5;
			pos[0] += 1.5;
			sprites.draw_sprite(pos, shines[i], .01, 0.01);
		}
		sprites.end();  
		glPopMatrix();
	sprites.end_occlusion(b['d']);
	if (!b['d']) 
	{
		//
		// second : Render
		//
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//
		// various settings
		//

		glPolygonMode(GL_FRONT_AND_BACK, b['w'] ? GL_LINE : GL_FILL);

		glDisable(GL_CULL_FACE);

		if(glActiveTextureARB)
		{
			glActiveTextureARB( GL_TEXTURE1_ARB );
			glDisable(GL_TEXTURE_2D);
			glActiveTextureARB( GL_TEXTURE0_ARB );
		}
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shinefact);
		if(b['s'])
		{
			glColor3f(0.5f,0.0f,0.2f);
			glutSolidSphere(1,30,30);
			glPushMatrix();
			glTranslatef(1.5,0,0);
			glColor3f(0.0f,0.2f,0.5f);
			glutSolidSphere(0.5,30,30);
			glPopMatrix();
		}

		sprites.set_test_cst(m_fTestConstant1*PI*2, 0);
		sprites.bind_light(light_pos, shinefact);

		sprites.begin();
		for(i=0; i< NUMSHINES; i++)
		{
			sprites.draw_sprite(shines[i], shines[i], .1, 0.7);
			glh::vec3f pos(shines[i]);
			pos *= 0.5;
			pos[0] += 1.5;
			sprites.draw_sprite(pos, shines[i], .1, 0.7);
		}
		sprites.end();

		if(b['o'])
			sprites.display_occlusion_map();
		//
		//----> Finish the scene
		//
		glPopMatrix();
	}
	if(b[' '])
	{
		alpha += 0.6;
		beta += 0.2234;
	}
  glutSwapBuffers();
}
