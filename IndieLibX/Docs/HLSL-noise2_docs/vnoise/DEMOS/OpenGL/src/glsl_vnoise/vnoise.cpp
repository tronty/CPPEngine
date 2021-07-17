/*
  GLSL VP1.0 noise implementation test
*/
#if defined(WIN32)
#  include <windows.h>
#  pragma warning (disable : 4786)
#endif

#define GLH_EXT_SINGLE_FILE
#include <glh/glh_glut.h>
#include <glh/glh_extensions.h>
#include <shared/data_path.h>
#include <shared/read_text_file.h>
#include <shared/quitapp.h>

#include "vnoise_init.h"

using namespace glh;

void display();
void idle();
void resize(int w, int h);
void key(unsigned char k, int x, int y);
void special(int k, int x, int y);
void init_opengl();
void menu( int entry );

GLhandleARB programObject;

GLint displacementParam;
GLint permGradTableParam;

glut_simple_mouse_interactor object;
glut_callbacks cb;
bool b[256];

vec3f noise_translate;
vec3f noise_scale(4.0f);
float displacement = 1.0;
GLuint dlist;

void init_menus()
{
    glutCreateMenu( menu );
    glutAddMenuEntry( "Toggle wireframe [w]", 'w' );
    glutAddMenuEntry( "Toggle animation [ ]", ' ' );
    glutAddMenuEntry( "Increase noise scale [=]", '=' );
    glutAddMenuEntry( "Decrease noise scale [-]", '-' );
    glutAddMenuEntry( "Increase displacment []]", ']' );
    glutAddMenuEntry( "Decrease displacment [[]", '[' );
    glutAddMenuEntry( "Reset [r]", 'r' );
    glutAddMenuEntry( "quit [esc]", 27 );
    glutAttachMenu( GLUT_RIGHT_BUTTON) ;
}

void printInfoLog(GLhandleARB object)
{
    int maxLength = 0;
    glGetObjectParameterivARB(object, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);

    char *infoLog = new char[maxLength];
    glGetInfoLogARB(object, maxLength, &maxLength, infoLog);

    cout << infoLog << endl;
}

void addShader(GLhandleARB programObject, const GLcharARB *shaderSource, GLenum shaderType)
{
    assert(programObject != 0);
    assert(shaderSource != 0);
    assert(shaderType != 0);

    GLhandleARB object = glCreateShaderObjectARB(shaderType);
    GLint length = (GLint)strlen(shaderSource);
    glShaderSourceARB(object, 1, &shaderSource, &length);

    // compile vertex shader object
    glCompileShaderARB(object);

    // check if shader compiled
    GLint compiled = 0;
    glGetObjectParameterivARB(object, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

    if (!compiled)
    {
        printInfoLog(object);
        quitapp(-1);
    }

    // attach vertex shader to program object
    glAttachObjectARB(programObject, object);

    // delete vertex object, no longer needed
    glDeleteObjectARB(object);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
        cout << "OpenGL error: " << gluErrorString(err) << endl;
}

void init_opengl()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    if (!glh_init_extensions("GL_ARB_shader_objects GL_ARB_vertex_shader"))
    {
        cout << "Unsupported extensions: " << glh_get_unsupported_extensions() << endl;
        quitapp(-1);
    }

    init_menus();

    data_path media;
    media.path.push_back(".");
    media.path.push_back(MEDIA_PATH"programs");
    media.path.push_back("../../../../MEDIA/programs");
    media.path.push_back("../../../../../../../MEDIA/programs");

    string filename = media.get_file("glsl_vnoise/vnoise.glsl");
    if (filename == "")
    {
        cout << "Unable to load vnoise.glsl, exiting..." << endl;
        quitapp(-1);
    }

    programObject = glCreateProgramObjectARB();

    GLcharARB *shaderData = read_text_file(filename.c_str());
    addShader(programObject, shaderData, GL_VERTEX_SHADER_ARB);

    glLinkProgramARB(programObject);

    GLint linked = false;
    glGetObjectParameterivARB(programObject, GL_OBJECT_LINK_STATUS_ARB, &linked);
    if (!linked)
        printInfoLog(programObject);

    glUseProgramObjectARB(programObject);

    displacementParam = glGetUniformLocationARB(programObject, "Displacement");
    assert(displacementParam >= 0);

    permGradTableParam = glGetUniformLocationARB(programObject, "pg[0]");
    assert(permGradTableParam >= 0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
        cout << "OpenGL error: " << gluErrorString(err) << endl;

    // initalize constants for noise
    srand(42);
    initVertexNoiseConstants(32);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    object.apply_transform();

    // build noise transform
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(noise_scale[0], noise_scale[1], noise_scale[2]);
    glTranslatef(noise_translate[0], noise_translate[1], noise_translate[2]);
    glMatrixMode(GL_MODELVIEW);

    glUniform1fARB(displacementParam, displacement);

    if (dlist)
        glCallList(dlist);
    else
    {
        dlist = glGenLists(1);
        glNewList(dlist, GL_COMPILE);
        glutSolidSphere(1.0, 100, 100);
        glEndList();
    }

    glutSwapBuffers();
}

void key(unsigned char k, int x, int y)
{
    b[k] = ! b[k];

    // this is called a switch statement :)
    switch(k) {
    case 27:
    case 'q':
        if (programObject)
            glDeleteObjectARB(programObject);

        exit(0);
        break;

    case 'w':
    case 'W':
        if (b[k])
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;

    case '=':
    case '+':
      noise_scale[0] = noise_scale[1] = noise_scale[2] += 0.1;
      break;
    case '-':
    case '_':
      noise_scale[0] = noise_scale[1] = noise_scale[2] -= 0.1;
      break;

    case ']':
      displacement += 0.1f;
      break;
    case '[':
      displacement -= 0.1f;
      break;

    case 'r':
      noise_scale = vec3f(1.0);
      noise_translate = vec3f(0.0);
      break;
    }

    glutPostRedisplay();
}

void special(int k, int x, int y)
{
    glutPostRedisplay();
}

void idle()
{
    if (b[' ']) {
        object.trackball.increment_rotation();
        noise_translate[1] -= 0.01;
    }

    glutPostRedisplay();
}

void resize(int w, int h)
{
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void menu( int entry )
{
    key((unsigned char)entry, 0, 0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowSize(512, 512);
    glutCreateWindow("GLSL noise displacement");

    init_opengl();

    glut_helpers_initialize();

    object.configure_buttons(1);
    object.trackball.legacy_mode = true;
    object.dolly.dolly[2] = -5;

    cb.keyboard_function = key;
    cb.special_function = special;

    glut_add_interactor(&cb);
    glut_add_interactor(&object);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutIdleFunc(idle);
    glutReshapeFunc(resize);

    b[' '] = true;

    glutMainLoop();

    return 0;
}
