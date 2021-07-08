#ifndef _LSYTEMINTERPRETER_H_
#define _LSYTEMINTERPRETER_H_

#include "Turtle.h"
class IDrawable;

class LSystemInterpreter
{
protected:
	Turtle m_turtle;

public:
	virtual std::unique_ptr<IDrawable> interpret(const std::string& lString);
};

class LSystemQueryableInterpreter : public LSystemInterpreter
{
public:
	std::unique_ptr<IDrawable> interpret(const std::string& lString) override;
	const Point3Df getPosition() { return m_turtle.position(); }
	const Vector3 getHeading() { return m_turtle.heading(); }
	const Matrix4 getOrientation() { return m_turtle.orientation(); }
};

#endif//_LSYTEMINTERPRETER_H_