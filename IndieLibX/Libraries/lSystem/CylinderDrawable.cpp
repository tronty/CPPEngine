#include "precomp.h"
#include "CylinderDrawable.h"
#include <GL\glu.h>
#include <GL\glut.h>

void CylinderDrawableList::draw() const throw()
{
	glEnable(GL_TEXTURE_2D); 
	glBindTexture(GL_TEXTURE_2D, m_texture); 
	for (std::vector<CylinderDrawable>::const_iterator iter = m_cylinderDrawableList.cbegin(); iter != m_cylinderDrawableList.cend(); iter++)
	{ 
		iter->draw(); 
	} 
	for (std::vector<LeafDrawable>::const_iterator iter = m_leavesDrawableList.cbegin(); iter != m_leavesDrawableList.cend(); iter++)
	{ 
		iter->draw(); 
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void LeafDrawable::draw() const throw()
{
	glPushMatrix();
	glColor3f(0,255,0);
	glTranslatef(m_position.X(), m_position.Y(), m_position.Z());
	glMultMatrixf(m_orientation.getTranspose());
	glutSolidCube(5);
	glPopMatrix();
}
CylinderDrawable::CylinderDrawable(const Matrix4& orientation, const Point3Df& position, float length, float widthBase, float widthTop) : 
						m_orientation(orientation), m_position(position), m_length(length), m_widthBase(widthBase), m_widthTop(widthTop)
{ 
	m_pQuadric = gluNewQuadric();
	gluQuadricNormals(m_pQuadric, GLU_SMOOTH);
	gluQuadricTexture(m_pQuadric, GL_TRUE);
	gluQuadricOrientation(m_pQuadric, GLU_OUTSIDE);
}

void CylinderDrawable::draw() const throw()
{
	glPushMatrix();
	glColor3f(255,255,255);
	glTranslatef(m_position.X(), m_position.Y(), m_position.Z());
	glMultMatrixf(m_orientation.getTranspose());
	glTranslatef(0, m_length, 0);
	glutSolidSphere(m_widthTop/2, 10, 10);
	glTranslatef(0, -m_length, 0);
	glRotatef(-90,1,0,0);
	//gluQuadricDrawStyle(m_pQuadric, GLU_LINE);
	gluQuadricDrawStyle(m_pQuadric, GLU_FILL); //this makes it solid
	gluCylinder(m_pQuadric, m_widthBase/2, m_widthTop/2, m_length, 10, 10);
	glPopMatrix();
}