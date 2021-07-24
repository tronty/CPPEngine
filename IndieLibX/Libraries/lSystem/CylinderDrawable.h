#ifndef _CYLINDERDRAWABLE_H_
#define _CYLINDERDRAWABLE_H_

#include "drawable.h"
#include <GL\glew.h>
#include "utils.h"

class CylinderDrawable : public IDrawable
{
private:
	Matrix4 m_orientation;
	Point3Df m_position;
	float m_length;
	float m_widthBase;
	float m_widthTop;
	GLUquadric* m_pQuadric;

public:
	CylinderDrawable(const Matrix4& orientation, const Point3Df& position, float length, float widthBase, float widthTop);
	void draw() const throw() override;
};

class LeafDrawable : public IDrawable
{
private:
	Matrix4 m_orientation;
	Point3Df m_position;
public:
	LeafDrawable(const Matrix4& orientation, const Point3Df& position) : m_orientation(orientation), m_position(position) {}
	void draw() const throw() override;
};

class CylinderDrawableList : public IDrawable
{
private:
	std::vector<CylinderDrawable> m_cylinderDrawableList;
	std::vector<LeafDrawable> m_leavesDrawableList;
	GLuint m_texture;

public:
	CylinderDrawableList() {m_texture = createTexture("C:\\Development\\openSource\\lSystemOld\\lSystem\\bark.bmp");}
	void onNewInterpretationStart() throw() { m_cylinderDrawableList.clear(); m_leavesDrawableList.clear(); }
	void addCylinder(const Matrix4& orientation, const Point3Df& position, float length, float widthTop, float widthBase) { m_cylinderDrawableList.push_back(CylinderDrawable(orientation, position, length, widthTop, widthBase)); }
	void addLeaves(const Matrix4& orientation, const Point3Df& position) { m_leavesDrawableList.push_back(LeafDrawable(orientation, position)); }
	void draw() const throw();
};

#endif//_CYLINDERDRAWABLE_H_