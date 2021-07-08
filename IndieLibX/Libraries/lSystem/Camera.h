#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Vectors.h"
#include "Matrices.h"
#include "Points.h"

class Camera
{
private:
	Matrix4 m_state;
	Vector4 m_position;
	Matrix4 m_stateInvert;

	void updateInverse();

public:
	Camera(const Vector3& eye, const Vector3& center, const Vector3& up) throw();
	void rotateX(float angle) throw();
	void rotateY(float angle) throw();
	void rotateZ(float angle) throw();
	void strafe(float value) throw();
	void zoom(float value) throw();
	void moveY(float value) throw();
	const Matrix4& getState() const throw() { return m_state; }
	const float* getMultMatrix() throw() { return m_stateInvert.getTranspose(); }
	const Point3Df position() const throw() { return Point3Df(m_position.x, m_position.y, m_position.z); }
};


#endif//_CAMERA_H_