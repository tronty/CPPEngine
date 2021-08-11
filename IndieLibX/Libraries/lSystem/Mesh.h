#ifndef _MESH_H_
#define _MESH_H_

#include "utils.h"
#include "drawable.h"

template <class T>
class Vertex
{
private:
	Point3D<T> m_point;
public:
	Vertex(const Point3D<T>& p) : m_point(p) {}
	const Point3D<T>& point() const { return m_point; }
};

class Mesh : public IDrawable
{
private:
	std::vector<Vertex<float>> m_vertices;
	std::vector<int> m_corners;
	Mesh(const Mesh& other) {}
	Mesh& operator=(const Mesh& other) {}

public:
	Mesh() {}
	Mesh(Mesh&& other) : m_vertices(std::move(other.m_vertices)), m_corners(std::move(other.m_corners)) {}
	~Mesh() {}

	void draw() const;
	int addVertex(Vertex<float> vertex);
	int addTriangle(int c1, int c2, int c3);
	int numVerts() { return m_vertices.size(); }
	int numTriangles() { return m_corners.size() / 3; }
};

#endif//_MESH_H_