#ifndef __APPLE__
/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/Turtle.h>
#ifndef _MSC_VER
std::stack<Turtle> Turtle::_Turtlestack__;
std::stack<Turtle3D> Turtle3D::_Turtle3Dstack__;
#else
std::stack<Turtle> _Turtlestack__;
std::stack<Turtle3D> _Turtle3Dstack__;
void Turtle::push(Turtle& turtle)
{
	_Turtlestack__.push(turtle);
}

Turtle Turtle::pop()
{
	if(!_Turtlestack__.empty())
	{
		Turtle turtle=_Turtlestack__.top();
		_Turtlestack__.pop();
		return turtle;
	} 
	else
		LOG_PRINT("Turtle stack is empty.\n");
	Turtle turtle;
	return turtle;
}

void Turtle3D::push(Turtle3D& turtle)
{
	_Turtle3Dstack__.push(turtle);
}

Turtle3D Turtle3D::pop()
{
	if(!_Turtle3Dstack__.empty())
	{
		Turtle3D turtle=_Turtle3Dstack__.top();
		_Turtle3Dstack__.pop();
		return turtle;
	}
	else
		LOG_PRINT("Turtle3D stack is empty.\n");
	Turtle3D turtle;
	return turtle;
}
#ifndef _MSC_VER
void Turtle_::push(Turtle_& turtle)
{
	_Turtlestack__.push(turtle);
}

Turtle_ Turtle_::pop()
{
	if(!_Turtlestack__.empty())
	{
		Turtle_ turtle=_Turtlestack__.top();
		_Turtlestack__.pop();
		return turtle;
	} 
	else
		LOG_PRINT("Turtle_ stack is empty.\n");
	Turtle_ turtle;
	return turtle;
}
#endif
#endif
#endif

