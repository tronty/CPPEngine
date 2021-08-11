#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <iostream>
using namespace std;
#include <GL/glut.h>

GLuint makeaTree;
float x,y,z;

void makeCylinder(float height, float base){
GLUquadric *obj = gluNewQuadric();
//gluQuadricDrawStyle(obj, GLU_LINE);
glColor3f(0.64f, 0.16, 0.16f);glPushMatrix();
glRotatef(-90, 1.0,0.0,0.0);
gluCylinder(obj, base,base-(0.2*base), height, 20,20);
glPopMatrix();
glutSwapBuffers();
}

 


void makeTree(float height, float base){

float angle;
makeCylinder(height, base); glTranslatef(0.0, height, 0.0);
height -= height*.2; base-= base*0.3;
for(int a= 0; a<3; a++){
angle = stx_rand()%50+20;
if(angle >48)
angle = -(stx_rand()%50+20);
if (height >1){
glPushMatrix();
glRotatef(angle,1,0.0,1);
makeTree(height,base);
glPopMatrix();

}
 //else glColor3f(0.0,1.0/a,0.0);glutSolidSphere(.2,10,10); add like fruits...
}

}
void init(void)
{ glClearColor(1.0,1.0,1.0,1.0);
glShadeModel(GL_SMOOTH);
glEnable(GL_DEPTH_TEST);
makeaTree=glGenLists(1);
glNewList(makeaTree, GL_COMPILE);
makeTree(4,0.2);
glEndList();
}
void keyboard(unsigned char key, int x, int y){
switch (key){
case 'x':
x +=10;
glutPostRedisplay();
break;
case 'y':
y +=10;
glutPostRedisplay();
break;
case 'z':
z+=10;
glutPostRedisplay();
break;
}
}

void display(){
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glPushMatrix();
glRotatef(x,1.0,0.0,0.0);
glRotatef(y,0.0,1.0,0.0);
glRotatef(z,0.0,0.0,1.0);
glCallList(makeaTree);

glPopMatrix();
glutSwapBuffers();
}
void reshape(int w, int h)
{ glViewport (0, 0, (GLsizei) w, (GLsizei) h);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(30.0, (GLfloat) w/(GLfloat) h, 0.001, 1000.0);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
glTranslatef(0.0,-8.0,-50.0);
}
void main(int argc, char **argv)
{
glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
 glutInitWindowSize (1200, 800); glutInitWindowPosition(0,0);
glutCreateWindow("3D Tree Using Recursion");
init();
glutReshapeFunc(reshape);
 glutKeyboardFunc(keyboard);

glutDisplayFunc(display); 


 glutMainLoop();


}
