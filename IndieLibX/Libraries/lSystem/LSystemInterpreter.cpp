#include "precomp.h"
#include "LSystemInterpreter.h"
#include "LineDrawable.h"
#include "CylinderDrawable.h"
#include "Token.h"

//#define DRAW_LINE

std::unique_ptr<IDrawable> LSystemInterpreter::interpret(const std::string& string)
{
	const float length = 10;
	const float angle = PI/4;

	StringTokenizer tokenizer(string);
	ExpandedToken currentToken;

	m_turtle.reset();

#ifdef DRAW_LINE
	std::unique_ptr<LineDrawable> pLineDrawable(new LineDrawable());
	pLineDrawable->onNewInterpretationStart();
#else
	std::unique_ptr<CylinderDrawableList> pCylinderDrawable(new CylinderDrawableList());
	pCylinderDrawable->onNewInterpretationStart();
#endif
	
	while (tokenizer.getNextToken(currentToken))
	{
		switch (currentToken.getChar())
		{
		case 'F': 
			{
				Point3Df initPosition = m_turtle.position();
				float distance = 100;
				float widthTop = 10;
				float widthBase = 10;
				currentToken.getParamValueForKey('l', distance);
				currentToken.getParamValueForKey('w', widthBase);
				currentToken.getParamValueForKey('u', widthTop);
#ifndef DRAW_LINE
				pCylinderDrawable->addCylinder(m_turtle.orientation(), m_turtle.position(), distance, widthBase, widthTop);
#endif
				m_turtle.translate(distance);
				Point3Df finalPosition = m_turtle.position();
#ifdef DRAW_LINE
				pLineDrawable->addPoints(initPosition, finalPosition);
#endif
				break;
			}
		case '+': 
			{
				float angle = 45;
				currentToken.getParamValue(angle);
				m_turtle.changeHeading(angle);
				break;
			}
		case '-':
			{
				float angle = 45;
				currentToken.getParamValue(angle);
				m_turtle.changeHeading(-angle);
				break;
			}
		case '&': 
			{
				float angle = 45;
				currentToken.getParamValue(angle);
				m_turtle.changePitch(angle);
				break;
			}
		case '^': 
			{
				float angle = 45;
				currentToken.getParamValue(angle);
				m_turtle.changePitch(-angle);
				break;
			}
		case '\\': 
			{
				float angle = 45;
				currentToken.getParamValue(angle);
				m_turtle.changeRoll(angle);
				break;
			}
		case '/': 
			{
				float angle = 45;
				currentToken.getParamValue(angle);
				m_turtle.changeRoll(-angle);
				break;
			}
		case '[': 
			{
				m_turtle.pushState();
				break;
			}
		case ']': 
			{
				m_turtle.popState();
				break;
			}
		case 'B': 
		case 'L':
			{
#ifndef DRAW_LINE
				pCylinderDrawable->addLeaves(m_turtle.orientation(), m_turtle.position());
#endif
				break;
			}
		default: break;
		}
	}
#ifdef 	DRAW_LINE
	return std::unique_ptr<IDrawable>(std::move(pLineDrawable));
#else
	return std::unique_ptr<IDrawable>(std::move(pCylinderDrawable));
#endif
}

std::unique_ptr<IDrawable> LSystemQueryableInterpreter::interpret(const std::string& string)
{
	const float length = 10;
	const float angle = PI/4;

	StringTokenizer tokenizer(string);
	ExpandedToken currentToken;

	m_turtle.reset();

	while (tokenizer.getNextToken(currentToken))
	{
		switch (currentToken.getChar())
		{
		case 'F': 
			{
				Point3Df initPosition = m_turtle.position();
				float distance = 100;
				float widthTop = 10;
				float widthBase = 10;
				currentToken.getParamValueForKey('l', distance);
				currentToken.getParamValueForKey('w', widthBase);
				currentToken.getParamValueForKey('u', widthTop);
				m_turtle.translate(distance);
				break;
			}
		case '+': 
			{
				float angle = 45;
				currentToken.getParamValue(angle);
				m_turtle.changeHeading(angle);
				break;
			}
		case '-':
			{
				float angle = 45;
				currentToken.getParamValue(angle);
				m_turtle.changeHeading(-angle);
				break;
			}
		case '&': 
			{
				float angle = 45;
				currentToken.getParamValue(angle);
				m_turtle.changePitch(angle);
				break;
			}
		case '^': 
			{
				float angle = 45;
				currentToken.getParamValue(angle);
				m_turtle.changePitch(-angle);
				break;
			}
		case '\\': 
			{
				float angle = 45;
				currentToken.getParamValue(angle);
				m_turtle.changeRoll(angle);
				break;
			}
		case '/': 
			{
				float angle = 45;
				currentToken.getParamValue(angle);
				m_turtle.changeRoll(-angle);
				break;
			}
		case '[': 
			{
				m_turtle.pushState();
				break;
			}
		case ']': 
			{
				m_turtle.popState();
				break;
			}
		default: break;
		}
	}
	return NULL;
}