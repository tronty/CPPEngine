#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_

class IDrawable 
{
public:
	virtual void draw() const = 0;
	virtual ~IDrawable() = 0 {}
};

#endif//_DRAWABLE_H_