#include <vector>
#include <GL\glew.h>
#include "drawable.h"

//TODO msati3: Remove Renderer fetching hack
#include "Renderer.h"

#ifndef _UTILS_H_
#define _UTILS_H_

std::vector<std::string>& split(const std::string&s, char delim, std::vector<std::string>& elems);
GLuint createTexture(const std::string& fileName);

//TODO msati3: Remove Renderer fetching hack
Renderer& getRenderer();
void solidCube(GLdouble size);

class DrawableVector : public IDrawable
{
private:
	Vector3 m_vector;
	Point3Df m_initPoint;
public:
	DrawableVector(const Vector4& vector, const Point3Df& initPoint) : m_vector(vector.x, vector.y, vector.z), m_initPoint(initPoint) {}
	DrawableVector(const Point3Df& initPoint) throw() : m_initPoint(initPoint) {}
	void draw() const;
};

#endif//_UTILS_H_