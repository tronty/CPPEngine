#include "precomp.h"
#include "Turtle.h"

void Turtle::reset()
{
	m_orientation.set(1,0,0,0,
			          0,1,0,0,
			          0,0,1,0,
			          0,0,0,1);
	m_position.set(100,0,100);
}

void Turtle::translate(float amount) throw()
{
	translate(amount, heading());
}

void Turtle::translate(float amount, const Vector3& direction) throw()
{
	//Vector3 translation = Vector3(0, amount, 0);
	//m_position += m_orientation * translation;
	Vector3 directionNormal = direction;
	directionNormal.normalize();
	Vector3 componentWiseTranslation(amount * directionNormal.dot(left()), amount * directionNormal.dot(heading()), amount * directionNormal.dot(up()));

	//Matrix4 orientation = m_orientation;
	Vector3 vectorWorld = m_orientation * componentWiseTranslation;
	m_position += vectorWorld;
}

void Turtle::changeHeading(float angle) throw()
{
	angle = PI*angle/180;
	float cosAngle = cos(angle);
	float sinAngle = sin(angle);
	Matrix4 headingRot(cosAngle, -sinAngle, 0, 0,
				    sinAngle, cosAngle, 0, 0,
				    0, 0, 1, 0,
				    0, 0, 0, 1);
	m_orientation = m_orientation * headingRot;
}

void Turtle::changePitch(float angle) throw()
{
	angle = PI*angle/180;
	float cosAngle = cos(angle);
	float sinAngle = sin(angle);
	Matrix4 pitchRot(1, 0, 0, 0,
				     0, cosAngle, -sinAngle, 0,
				     0, sinAngle, cosAngle, 0,
				     0, 0, 0, 1);
	m_orientation = m_orientation * pitchRot;
}

void Turtle::changeRoll(float angle) throw()
{
	angle = PI*angle/180;
	float cosAngle = cos(angle);
	float sinAngle = sin(angle);
	Matrix4 rollRot(cosAngle, 0, sinAngle, 0,
				       0, 1, 0, 0,
				      -sinAngle, 0, cosAngle, 0,
				       0, 0, 0, 1);
	m_orientation = m_orientation * rollRot;
}