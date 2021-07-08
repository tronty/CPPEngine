#include "precomp.h"
#include "Camera.h"
#include "VoxelWorld.h"

Camera::Camera(const Vector3& position, const Vector3& center, const Vector3& up) throw()
{
	m_state.setColumn(3, Vector4(0, 0, 0, 1));
	m_position.set(position.x, position.y, position.z, 1);
	//TODO msati3: derive x, y and z from center and up vectors using Euler angles
	Vector3 left = up.cross(center);
	left.normalize();
	const float x[] = {1, 0, 0, 0};
	const float y[] = {0, 1, 0, 0};
	const float z[] = {0, 0, 1, 0};
	m_state.setColumn(0, x);
	m_state.setColumn(1, y);
	m_state.setColumn(2, z);
}

void Camera::updateInverse()
{
	m_stateInvert.set(m_state.get()); 
	Matrix4 translation(1, 0, 0, -m_position.x,
				        0, 1, 0, -m_position.y,
				        0, 0, 1, -m_position.z,
				        0, 0, 0, 1);
	m_stateInvert.invert();
	m_stateInvert = m_stateInvert * translation;
}

void Camera::rotateX(float angle) throw()
{
	float cosAngle = cos(angle);
	float sinAngle = sin(angle);
	Matrix4 multX(1, 0, 0, 0,
				   0, cosAngle, -sinAngle, 0,
				   0, sinAngle, cosAngle, 0,
				   0, 0, 0, 1);
	m_state = multX * m_state;
	updateInverse();
}

void Camera::rotateY(float angle) throw()
{
	float cosAngle = cos(angle);
	float sinAngle = sin(angle);
	Matrix4 multY(cosAngle, 0, sinAngle, 0,
				   0, 1, 0, 0,
				  -sinAngle, 0, cosAngle, 0,
				   0, 0, 0, 1);
	m_state = multY * m_state;
	updateInverse();
}

void Camera::rotateZ(float angle) throw()
{
	float cosAngle = cos(angle);
	float sinAngle = sin(angle);
	Matrix4 multZ(cosAngle, -sinAngle, 0, 0,
				   sinAngle, cosAngle, 0, 0,
				   0, 0, 1, 0,
				   0, 0, 0, 1);
	m_state = multZ * m_state;
	updateInverse();
}

void Camera::zoom(float distance) throw()
{
	Vector4 translation = Vector4(0, 0, -distance, 0);
	m_position += m_state * translation;
	updateInverse();
}

void Camera::strafe(float distance) throw()
{
	Vector4 translation = Vector4(distance, 0, 0, 0);
	m_position += m_state * translation;
	updateInverse();
}

void Camera::moveY(float distance) throw()
{
	Vector4 translation = Vector4(0, distance, 0, 0);
	m_position += m_state * translation;
	updateInverse();
}