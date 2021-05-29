#if defined(WIN32)
# include <windows.h>
# pragma warning (disable : 4786)
#endif

#include <Cg/cgGL.h>

#undef GLH_EXT_SINGLE_FILE
#include <glh/glh_extensions.h>
#include <glh/glh_glut.h>
#include <glh/glh_obs.h>

#include <nv_GenericTools/nv_GenericTools.h>

#include "CVolumelines.h"

//
//----> Constants
//
const float PI = 3.141592654;

//using namespace glh;
//
//----> GLH controls
//
glh::glut_callbacks cb;
glh::glut_simple_mouse_interactor camera, object;
glh::glut_perspective_reshaper reshaper;

GLuint vertex_program_id;      // Vertex program handle
GLuint bind_vertex_program; // Display list handle for initializing vertex program attributes
GLuint bind_texture_shader; // Display list handle for initializing texture shader
GLuint bind_register_combiners; // Display list handle for initializing register combiners

bool b[256];

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

CVLines lines;
//CPLines plines;
GLfloat m_fTestConstant1 = 1;
GLfloat m_fTestConstant2 = 3.7f;
GLfloat epais = 0.20;

#define MAXTEX 9
int curtex = 0;
static char *texturesnames[] =
{
    "1d_INNER1.png",
    "1d_INNER2.png",
    "1d_TUBE.png",
    "1d_SPIRAL.png",
    "1d_SPIRAL_.png",
    "1d_RING2.png",
    "1d_glow1.png",
    "1d_debug.png",
    "1d_debug2.png"
};

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
    glutCreateWindow("Volume Line test");

    b['c'] = false;
    b[' '] = true;
    b['w'] = false;
    b['m'] = false;
    //
    // Init tool windows
    //
    OPENMODULES("opengl");
    CREATEWINDOWCONTAINER_FOLDING("params",0,512+25,512,200,NULL);
      CREATECTRL_VALUE("thickness", "params", &epais, 0, 1);
      CREATECTRL_COMBO("textures", "params", &curtex);
      CTRL_NOTIFYME_CB("textures", lines.SetTexture(texturesnames[(int)val]); ,0 , 0)
      for(int i=0; i<MAXTEX; i++)
      {
        CREATECOMBOITEM(texturesnames[i], "textures", i);
      }
      FLUSHPTR(&curtex);
    //
    // put in 1 windows the console and log window
    //
    CREATEWINDOWCONTAINER_SPLIT("logs",0,512+25+200+25,512,200,NULL);
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
    glutAddMenuEntry( "Toggle wireframe [w]", 'w' );
    glutAddMenuEntry( "Increase thickness [+]", '+' );
    glutAddMenuEntry( "Decrease thickness [-]", '-' );
    glutAddMenuEntry( "Next texture [t]", 't' );
    glutAddMenuEntry( "Previous texture [T]", 'T' );
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
void make_normals_texture(glh::array2<glh::vec3ub> &img)
{
}

void cleanExit(int exitval)
{
    lines.free();
    CLOSEMODULES();
    exit(exitval);
}

void cgErrorCallback(void)
{
    CGerror LastError = cgGetError();

    if(LastError)
    {
        const char *Listing = cgGetLastListing(lines.get_cg_context());
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
    //
    // Check for extensions
    //
    if (!glh_init_extensions("\
         GL_EXT_secondary_color \
         GL_ARB_multitexture \
         GL_NV_texture_shader \
         GL_NV_register_combiners \
        "))
    {
        cout << "Demo requires the following extension(s): " << glh_get_unsupported_extensions() << "\n";
        exit(0);
    }

    cgSetErrorCallback(cgErrorCallback);

    lines.init();
    lines.SetTexture(texturesnames[curtex]);
}

void menu( int v )
{
    char strtmp[100];
    b[v] = !b[v];

    switch (v)
    {
    case '+':
        epais += 0.01f;
        FLUSHPTR(&epais);
        break;
    case '-':
        epais -= 0.01f;
        FLUSHPTR(&epais);
        break;
    case 't':
        curtex++;
        if(curtex >= MAXTEX) curtex = 0;
        lines.SetTexture(texturesnames[curtex]);
        FLUSHPTR(&curtex);
        break;
    case 'T':
        curtex--;
        if(curtex < 0) curtex = MAXTEX-1;
        lines.SetTexture(texturesnames[curtex]);
        FLUSHPTR(&curtex);
        break;
    case 27:
        cleanExit(0);

    case ' ':
        if(b[v])
            glutIdleFunc(idle);
        else
            glutIdleFunc(0);
        break;

            case 'z':
            {
                m_fTestConstant1 += 0.1f;
                sprintf(strtmp,"cst1 = %f\n", m_fTestConstant1);
                break;
            }
            case 'a':
            {
                m_fTestConstant1-= 0.1f;
                sprintf(strtmp,"cst1 = %f\n", m_fTestConstant1);
                break;
            }

            case 's':
            {
                m_fTestConstant2+=0.01f;
                sprintf(strtmp,"cst2 = %f\n", m_fTestConstant2);
                break;
            }
            case 'q':
            {
                m_fTestConstant2-=0.01f;
                sprintf(strtmp,"cst2 = %f\n", m_fTestConstant2);
//              if(m_fTestConstant < 0.01f)
//                  m_fTestConstant = 0.01f;
                break;
            }
    }


    glutPostRedisplay();
}

void key(unsigned char k, int x, int y)
{
    menu((int)k);
}

void idle()
{
    glutPostRedisplay();
}

float queryElapsedTime()
{
    return glutGet(GLenum(GLUT_ELAPSED_TIME)) * 0.001f;
}

void draw_complex()
{
                glh::vec3f pos0(0,0,0); 
                float f=0.0f;
                float g=0.001f*queryElapsedTime();
                float size0=0.0f,size1;

                glColor3f(1.0f,1.0f,1.0f);

                lines.lines_begin();

                for(int i=0; i<20; f+=0.2f,g+=0.2f)
                {
                    glh::vec3f pos1;

                    pos1=glh::vec3f(sin(g+f)*cos(g+f*1.5f),
                                     sin(g+f*2.7f)*cos(g+f),sin(g+f*1.4f)*cos(g+f*0.7f));
                    pos1 *= 2.0f;

                    size0 = size1 = epais;

                    if((f!=0.0f))
                    {
                        lines.draw_line(pos0 , pos1, size0,size1);
                    i++;

                    }

                    pos0=pos1;size0=size1;
                }
                lines.lines_end();

}
void draw_grass()
{
                glh::vec3f pos0(0,0,0); 
                glh::vec3f pos1;
                glh::vec3f tige;
                float dx,dy, h, a, b;
                int ix,iy;
                float f=0.0f;
                float f2=0.0f;
                float g=0.001f*queryElapsedTime();
                float size0=0.0f,size1;

                glColor3f(1.0f,1.0f,1.0f);

                lines.lines_begin();

                a = b = 0;
                h = m_fTestConstant1;
                for(f=0, ix=0, dx = -4; ix<40; ix++, dx+=0.3, f+= 0.4,a=cos(f+g)*PI/10)
                  for(f2=0,iy=0, dy = -4; iy<40; iy++, dy+=0.3, f2+= 0.4,b=sin(f2+g)*PI/10)
                {
                    pos0=glh::vec3f(dx, dy, 0);

                    tige.set_value(
                        h*sin(b), 
                        -h*cos(b)*sin(a), 
                        h*cos(a)*cos(b));

                    size0 = size1 = epais;

                    pos1 = pos0 + tige;

                    lines.draw_line(pos0 , pos1, size0,size1);

                }
                lines.lines_end();

}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    //  apply various transformations
    //
    glPushMatrix();
    camera.apply_inverse_transform(); // glh...
    //
    // various settings
    //
    glDisable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, .9);

    glEnable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, b['w'] ? GL_LINE : GL_FILL);

    glDisable(GL_CULL_FACE);
    // if we with per-pixel lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);

    object.apply_transform();
    glColor3f(1.0f,0.0f,0.5f);
    //
    //----> Draw lines
    //
    glScalef(0.5,0.5,0.5);
if(b['c'])
    draw_complex();

//else
{
#define DRAWL lines.draw_line
//#define DRAWL lines.draw_particle
    glColor3f(1.0f,1.0f,1.0f);

    lines.lines_begin();
    glh::vec3f v1;
    glh::vec3f v2;
        v1.set_value(0, -2, 0);
        v2.set_value(0,1, 2);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(-2, 0, 0);
        v2.set_value(0,1, 2);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(2, 0, 0);
        v2.set_value(0,1, 2);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(0, 2, 0);
        v2.set_value(0,1, 2);
        DRAWL(v1, v2, epais, epais);

        v1.set_value(0, -2, 0);
        v2.set_value(0,-1, 2);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(-2, 0, 0);
        v2.set_value(0,-1, 2);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(2, 0, 0);
        v2.set_value(0,-1, 2);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(0, 2, 0);
        v2.set_value(0,-1, 2);
        DRAWL(v1, v2, epais, epais);

        v1.set_value(-m_fTestConstant1, m_fTestConstant1, -m_fTestConstant1);
        v2.set_value(m_fTestConstant1,m_fTestConstant1, -m_fTestConstant1);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(m_fTestConstant1, m_fTestConstant1, -m_fTestConstant1);
        v2.set_value(m_fTestConstant1,-m_fTestConstant1, -m_fTestConstant1);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(m_fTestConstant1, -m_fTestConstant1, -m_fTestConstant1);
        v2.set_value(-m_fTestConstant1,-m_fTestConstant1, -m_fTestConstant1);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(-m_fTestConstant1, -m_fTestConstant1, -m_fTestConstant1);
        v2.set_value(-m_fTestConstant1, m_fTestConstant1, -m_fTestConstant1);
        DRAWL(v1, v2, epais, epais);

        v1.set_value(-m_fTestConstant1, m_fTestConstant1, m_fTestConstant1);
        v2.set_value(m_fTestConstant1,m_fTestConstant1, m_fTestConstant1);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(m_fTestConstant1, m_fTestConstant1, m_fTestConstant1);
        v2.set_value(m_fTestConstant1,-m_fTestConstant1, m_fTestConstant1);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(m_fTestConstant1, -m_fTestConstant1, m_fTestConstant1);
        v2.set_value(-m_fTestConstant1,-m_fTestConstant1, m_fTestConstant1);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(-m_fTestConstant1, -m_fTestConstant1, m_fTestConstant1);
        v2.set_value(-m_fTestConstant1, m_fTestConstant1, m_fTestConstant1);
        DRAWL(v1, v2, epais, epais);

        v1.set_value(-m_fTestConstant1, m_fTestConstant1, -m_fTestConstant1);
        v2.set_value(-m_fTestConstant1,m_fTestConstant1, m_fTestConstant1);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(m_fTestConstant1, m_fTestConstant1, -m_fTestConstant1);
        v2.set_value(m_fTestConstant1, m_fTestConstant1, m_fTestConstant1);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(m_fTestConstant1, -m_fTestConstant1, -m_fTestConstant1);
        v2.set_value(m_fTestConstant1,-m_fTestConstant1, m_fTestConstant1);
        DRAWL(v1, v2, epais, epais);
        v1.set_value(-m_fTestConstant1, -m_fTestConstant1, -m_fTestConstant1);
        v2.set_value(-m_fTestConstant1, -m_fTestConstant1, m_fTestConstant1);
        DRAWL(v1, v2, epais, epais);
    lines.lines_end();
//  glutWireCube(2*m_fTestConstant1);
}
    //
    //----> Finish the scene
    //
    glPopMatrix();

    glutSwapBuffers();
}
