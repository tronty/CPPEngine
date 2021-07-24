#ifndef _TURTLE_H_
#define _TURTLE_H_

#include <stack>

class Turtle
{
private:
	Matrix4 m_orientation;
	Point3Df m_position;
	const Matrix3& getMatrixForState() const;
	std::stack<Matrix4> m_orientationStack;
	std::stack<Point3Df> m_positionStack;

public:
	Turtle() : m_orientation(1,0,0,0,
					         0,1,0,0,
					         0,0,1,0,
					         0,0,0,1), m_position(100,0,100) {reset();}

	void reset();
	void translate(float amount) throw();
	void translate(float amount, const Vector3& direction) throw();
	const Vector3 left() const throw() { const float* data = m_orientation.get(); return Vector3(data[0], data[4], data[8]); }
	const Vector3 up() const throw() { const float* data = m_orientation.get(); return Vector3(data[2], data[6], data[10]); }
	const Vector3 heading() const throw() { const float* data = m_orientation.get(); return Vector3(data[1], data[5], data[9]); }
	const Matrix4 orientation() const throw() { return m_orientation; }
	const Point3Df position() const throw() { return m_position; }
	void changeHeading(float angle) throw();
	void changePitch(float angle) throw();
	void changeRoll(float angle) throw();
	void pushState() { m_orientationStack.push(m_orientation); m_positionStack.push(m_position); } 
	void popState() { m_orientation = m_orientationStack.top(); m_orientationStack.pop(); m_position = m_positionStack.top(); m_positionStack.pop(); }
};

#endif//_TURTLE_H_