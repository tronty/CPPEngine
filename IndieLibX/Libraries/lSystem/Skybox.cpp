#include "precomp.h"
#include "utils.h"
#include "Camera.h"
#include "Skybox.h"
#include "GL\glew.h"
#include "GL\glut.h"

Skybox::Skybox(const Camera& camera) : m_camera(camera)
{
	m_textureId = createTexture("C:\\Development\\openSource\\lSystem\\lSystem\\Debug\\skydome.bmp");
	m_pQuadric = gluNewQuadric();
	gluQuadricNormals(m_pQuadric, GLU_SMOOTH);
	gluQuadricTexture(m_pQuadric, GL_TRUE);
	gluQuadricOrientation(m_pQuadric, GLU_INSIDE);
}

void Skybox::draw() const
{
	double planeEq[] = {0,1,0,0};
	glEnable(GL_CLIP_PLANE0);
	//glClipPlane(0, planeEq); 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT_FACE);
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glColor3f(255,255,255);
	glTranslatef(m_camera.position().X(), m_camera.position().Y(), m_camera.position().Z());
	glRotatef(90, 1, 0, 0);
    gluSphere(m_pQuadric,1000,20,20);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glCullFace(GL_NONE);
	glDisable(GL_CULL_FACE);
	//glDisable(GL_CLIP_PLANE0);
}