#ifndef _LINEDRAWABLE_H_
#define _LINEDRAWABLE_H_

#include "drawable.h"

class LineDrawable : public IDrawable
{
private:
	std::vector<std::tuple<Point3Df, Point3Df>> m_lines;

public:
	void onNewInterpretationStart() throw() { m_lines.clear(); }
	void addPoints(const Point3Df& one, const Point3Df& other);
	void draw() const override;
};

#endif//_LINEDRAWABLE_H_