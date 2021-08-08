#include "precomp.h"
#include "utils.h"
#include "Camera.h"
#include "EnvironmentAssets.h"
#include "GL\glew.h"
#include "GL\glut.h"

Skybox::Skybox(const Camera& camera) : m_camera(camera), m_fShow(false)
{
	m_textureId = createTexture("C:\\Development\\openSource\\lSystemOld\\lSystem\\skydome.bmp");
	m_groundTexture = createTexture("C:\\Development\\openSource\\lSystemOld\\lSystem\\grass.bmp");
	m_pQuadric = gluNewQuadric();
	gluQuadricNormals(m_pQuadric, GLU_SMOOTH);
	gluQuadricTexture(m_pQuadric, GL_TRUE);
	gluQuadricOrientation(m_pQuadric, GLU_INSIDE);
}

void Skybox::draw() const
{
	if (m_fShow)
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
		gluSphere(m_pQuadric,900,20,20);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glCullFace(GL_NONE);
		glDisable(GL_CULL_FACE);
		//glDisable(GL_CLIP_PLANE0);

		//TODO msati3: Replace this by a height map?
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_groundTexture);
		glColor3f(0,255,0);
		glBegin(GL_QUADS);
			glTexCoord2d(0,0); glVertex3f(-1000,0,1000);
			glTexCoord2d(1,0); glVertex3f(1000,0,1000);
			glTexCoord2d(1,1); glVertex3f(1000,0,-1000);
			glTexCoord2d(0,1); glVertex3f(-1000,0,-1000);
		glEnd();
	
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}
}