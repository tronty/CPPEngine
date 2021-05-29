#include "precomp.h"
#include "Mesh.h"
#include "GL\glew.h"

void Mesh::draw() const
{
	/*glBegin(GL_QUADS);
		glColor3f(0,0,1);
		glVertex3f(0,0,0);
		glVertex3f(1000,0,0);
		glVertex3f(1000,1000,0);
		glVertex3f(0,1000,0);
	glEnd();*/

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glBegin(GL_TRIANGLES);
	for (std::vector<int>::const_iterator corner = m_corners.cbegin(); corner != m_corners.cend(); corner++)
	{
		const Vertex<float>& curVertex = m_vertices[*corner];
		glVertex3f(curVertex.point().X(), curVertex.point().Y(), curVertex.point().Z());
	}
	glEnd();
}

int Mesh::addVertex(Vertex<float> vertex)
{
	m_vertices.push_back(vertex);
	return m_vertices.size();
}

int Mesh::addTriangle(int c1, int c2, int c3)
{
	m_corners.push_back(c1);
	m_corners.push_back(c2);
	m_corners.push_back(c3);
	return m_corners.size()/3;
}