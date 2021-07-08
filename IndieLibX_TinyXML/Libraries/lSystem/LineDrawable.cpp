#include "precomp.h"
#include "LineDrawable.h"
#include "GL\glew.h"

void LineDrawable::addPoints(const Point3Df& one, const Point3Df& other)
{
	m_lines.push_back(std::tuple<Point3Df, Point3Df>(one, other));
}

void LineDrawable::draw() const
{
	glColor3f(255,0,0);
	glPushMatrix();
	//Rotate about x axis by 90 degrees to make stuff visible

	glBegin(GL_LINES);
	for (std::vector<std::tuple<Point3Df, Point3Df>>::const_iterator lineEndpoints = m_lines.cbegin(); lineEndpoints != m_lines.cend(); lineEndpoints++)
	{
		glVertex3d(std::get<0>(*lineEndpoints).X(), std::get<0>(*lineEndpoints).Y(), std::get<0>(*lineEndpoints).Z());
		glVertex3d(std::get<1>(*lineEndpoints).X(), std::get<1>(*lineEndpoints).Y(), std::get<1>(*lineEndpoints).Z());
	}
	glEnd();
	glPopMatrix();
	glColor3f(255,255,255);
}
