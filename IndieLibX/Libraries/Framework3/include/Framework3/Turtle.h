#ifndef __Turtle__
#define __Turtle__
/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#ifndef __APPLE__
#include <string>
#include <stack>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#endif

#define A_LOG_FNLN 
#define A_LOG_PRINT

typedef std::map<const std::string,const std::string> rules;

struct RuleTurtle2D
{
	std::string m_description;
	std::string m_axiom;
	rules m_rules;
	float m_angle;
	RuleTurtle2D(	rules arules, 
			std::string adescription="", 
			std::string aaxiom="", 
			float aangle=0.0f)
	{
		m_description=adescription;
		m_axiom=aaxiom;
		m_rules=arules;
		m_angle=aangle;
	}
	RuleTurtle2D(const RuleTurtle2D& rhs)
	{
		m_description=rhs.m_description;
		m_axiom=rhs.m_axiom;
		m_rules=rhs.m_rules;
		m_angle=rhs.m_angle;
	}
};

inline RuleTurtle2D get_rule(int id)
{
	rules rules_;
	RuleTurtle2D c(rules_, "", "", 0.0f);
	switch(id)
	{
		// https://fedimser.github.io/l-systems.html:
		case 1:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:-:F:+:+:F:-:F"));
			c=RuleTurtle2D(rules_, "Koch's curve", "F", 60.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 2:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:-:F:+:+:F:-:F"));
			c=RuleTurtle2D(rules_, "Koch's snowflake", "F:+:+:F:+:+:F", 60.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 3:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:-:F:+:F"));
			c=RuleTurtle2D(rules_, "", "F:+:F:+:F", 120.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 4:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:F:+:F:+:+:F:+:F"));
			c=RuleTurtle2D(rules_, "Ice fractals", "F:+:F:+:F:+:F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 5:
			rules_.insert(std::pair<const std::string,const std::string>("X","X:+:Y:F:+"));
			rules_.insert(std::pair<const std::string,const std::string>("Y","-:F:X:-:Y"));
			c=RuleTurtle2D(rules_, "Dragon curve", "F:X", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 6:
			rules_.insert(std::pair<const std::string,const std::string>("X","X:+:Y:F:+:+:Y:F:-:F:X:-:F:X:F:X:-:Y:F"));
			rules_.insert(std::pair<const std::string,const std::string>("Y","-:F:X:+:Y:F:Y:F:+:+:Y:F:+:F:X-:F:X-:Y"));
			c=RuleTurtle2D(rules_, "Gosper's curve", "X:F", 60.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 7:
			rules_.insert(std::pair<const std::string,const std::string>("X","X:F:-:F:+:F:-:X:F:+:F:+:X:F:-:F+:F:-:X"));
			c=RuleTurtle2D(rules_, "Serpinski curve", "F:+:X:F:+:F:+:X:F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 8:
			rules_.insert(std::pair<const std::string,const std::string>("X","-:Y:F:+:X:F:X:+:F:Y:-"));
			rules_.insert(std::pair<const std::string,const std::string>("Y","+:X:F:-:Y:F:Y:-:F:X:+"));
			c=RuleTurtle2D(rules_, "Gilbert's curve", "X", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 9:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:F:+:F:+:F:+:F:+:F:F"));
			c=RuleTurtle2D(rules_, "", "F:+:F:+:F:+:F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 10:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:+:F:-:F:-:F:F:+:F:+:F:-:F"));
			c=RuleTurtle2D(rules_, "", "F:+:F:+:F:+:F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 11:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:+:F:-:F:-:F:F:F:+:F:+:F:-:F"));
			c=RuleTurtle2D(rules_, "", "F:+:F:+:F:+:F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 12:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:-:F:F:+:F:F:+:F:+:F:-:F:-:F:F:+:F:+:F:-:F:-:F:F:-:F:F:+:F"));
			c=RuleTurtle2D(rules_, "", "F:+:F:+:F:+:F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 13:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:-:F:+:F:+:F:-:F"));
			c=RuleTurtle2D(rules_, "Quadratic Snowflake", "F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 14:
			rules_.insert(std::pair<const std::string,const std::string>("X","Y:F:+:X:F:+:Y"));
			rules_.insert(std::pair<const std::string,const std::string>("Y","X:F:-:Y:F:-:X"));
			c=RuleTurtle2D(rules_, "", "Y:F", 60.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 15:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:+:F:-:F:+:F:+:F"));
			c=RuleTurtle2D(rules_, "", "F:+:F:+:F:+:F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 16:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:F:+:F:+:F:+:F:+:F:+:F:-:F"));
			c=RuleTurtle2D(rules_, "", "F:+:F:+:F:+:F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 17:
			rules_.insert(std::pair<const std::string,const std::string>("X","X:[:-:F:F:F:]:[:+:F:F:F:]:F:X"));
			rules_.insert(std::pair<const std::string,const std::string>("Y","Y:F:X:[:+:Y:]:[:-:Y:]"));
			c=RuleTurtle2D(rules_, "A bush", "Y", D3DXFROMWINE_PI/7.0f);
			break;
		case 18:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:F:+:[:+:F:-:F:-:F:]:-:[:-:F:+:F:+:F:]"));
			c=RuleTurtle2D(rules_, "A bush", "F", D3DXFROMWINE_PI/8.0f);
			break;
		case 19:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:[:+:F:-:F:]:[:-:F:+:F:]:F:[:-:F:]:[:+:F:]:F"));
			c=RuleTurtle2D(rules_, "A bush", "F", D3DXFROMWINE_PI/5.0f);
			break;
		case 20:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:F"));
			rules_.insert(std::pair<const std::string,const std::string>("X","F:[:+:X:]:F:[:-:X:]:+:X"));
			c=RuleTurtle2D(rules_, "A bush", "X", D3DXFROMWINE_PI/9.0f);
			break;
		case 21:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:-:F:+:F:+:F:-:F"));
			c=RuleTurtle2D(rules_, "A bush", "F:-:F:-:F:-:F:", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 22:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:[:+:F:]:F:[:-:F:]:F"));
			c=RuleTurtle2D(rules_, "", "F", D3DXFROMWINE_PI/7.0f);
			break;
		case 23:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:X:F"));
			rules_.insert(std::pair<const std::string,const std::string>("X","[:-:F:+:F:+:F:]:F:-:F:-:F:+"));
			c=RuleTurtle2D(rules_, "", "F", 60.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 24:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:F"));
			rules_.insert(std::pair<const std::string,const std::string>("X","-:F:X:F:+:+:F:X:F:+:+:F:X:F:-"));
			c=RuleTurtle2D(rules_, "Serpinski triangle", "F:X:F:-:F:F:-:F:F", 60.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 25:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:F:F:[:+:F:F:F:+:F:F:F:+:F:F:F:]"));
			c=RuleTurtle2D(rules_, "Serpinski carpet", "F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 26: 
			rules_.insert(std::pair<const std::string,const std::string>("F","F:-:b:+:F:F:-:F:-:F:F:-:F:b:-:F:F:+:F:+:F:F:+:F:b:+:F:F:F]"));
			rules_.insert(std::pair<const std::string,const std::string>("b","b:b:b:b:b:b"));
			c=RuleTurtle2D(rules_, "", "F:-:F:-:F:-:F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 27:
			rules_.insert(std::pair<const std::string,const std::string>("F","-:F:+:+:F:-"));
			c=RuleTurtle2D(rules_, "", "F:+:+:F:+:+:F:+:+:F", 45.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 28: 
			rules_.insert(std::pair<const std::string,const std::string>("F","F:+:+:F:+:+:F:+:+:+:+:+:F:-:F:+:+:F"));
			c=RuleTurtle2D(rules_, "Levy curve", "F:+:+:F:+:+:F:+:+:F:+:+:F", 36.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 29: 
			rules_.insert(std::pair<const std::string,const std::string>("F","F:+:F:-:F"));
			c=RuleTurtle2D(rules_, "", "F", 120.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 30:
			rules_.insert(std::pair<const std::string,const std::string>("F","F:+:F:-:F:-:F:-:b:+:F:+:F:+:F:-:F"));
			rules_.insert(std::pair<const std::string,const std::string>("b","b:b:b"));
			c=RuleTurtle2D(rules_, "Sierπnski Carpet", "F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 31: 
			rules_.insert(std::pair<const std::string,const std::string>("F","F:-:F:+:+:F:+:F:-:F:-:F"));
			c=RuleTurtle2D(rules_, "", "F:-:F:-:F:-:F:-:F", 72.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 32: 
			rules_.insert(std::pair<const std::string,const std::string>("F",""));
			rules_.insert(std::pair<const std::string,const std::string>("X","-:F:+:+:F:-:X:-:F:-:-:F:+:Y:-:-:-:F:-:-:F:+:Y:+:F:+:+:F:-:X:+:+:+:F:+:+:F:-:X:-:F:+:+:F:-:X:+:+:+:F:-:-:F:+:Y:-:-"));
			rules_.insert(std::pair<const std::string,const std::string>("Y","+:F:+:+:F:-:X:-:F:-:-:F:+:Y:+:F:-:-:F:+:Y:-:-:-:F:-:-:F:+:Y:-:-:-:F:+:+:F:-:X:+:+:+:F:+:+:F:-:X:+:+:+:F:-:-:F:+:Y"));
			c=RuleTurtle2D(rules_, "", "X", 30.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 33: 
			rules_.insert(std::pair<const std::string,const std::string>("F",""));
			rules_.insert(std::pair<const std::string,const std::string>("X","F:X:-:F:Y:-:F:X:+:F:Y:+:F:X:+:F:Y:+:F:X:+:F:Y:+:F:X:-:F:Y:-:F:X:-:F:Y:-:F:X:-:F:Y:-:F:X:+:F:Y:+:F:X"));
			rules_.insert(std::pair<const std::string,const std::string>("Y","F:Y"));
			c=RuleTurtle2D(rules_, "", "F:X", 45.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 34: 
			rules_.insert(std::pair<const std::string,const std::string>("F",""));
			rules_.insert(std::pair<const std::string,const std::string>("X","F:X:+:F:X:+:F:X:F:Y:-:F:Y:-"));
			rules_.insert(std::pair<const std::string,const std::string>("Y","+:F:X:+:F:X:F:Y:-:F:Y:-:F:Y"));
			c=RuleTurtle2D(rules_, "", "X:Y:X:Y:X:Y:X:+:?:X:Y:X:Y:X:Y:X:+:?:X:Y:X:Y:X:Y:X:+:?:X:Y:X:Y:X:Y:X", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 35: 
			rules_.insert(std::pair<const std::string,const std::string>("F","-:F:[:-:-:F:-:-:F:]:+:+:F:-:-:F:+"));
			c=RuleTurtle2D(rules_, "", "F:-:-:F:-:-:F:-:-:F:-:-:F:-:-:F", 30.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 36: 
			rules_.insert(std::pair<const std::string,const std::string>("F","F:+:F:F:-:F"));
			c=RuleTurtle2D(rules_, "", "F:+:F:+:F", 120.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 37: 
			rules_.insert(std::pair<const std::string,const std::string>("F",""));
			rules_.insert(std::pair<const std::string,const std::string>("X","F:X:+:F:Y:F:Y:-:F:Y"));
			rules_.insert(std::pair<const std::string,const std::string>("Y","F:X:-:F:X:F:X:+:F:X"));
			c=RuleTurtle2D(rules_, "", "X", 120.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 38: 
			rules_.insert(std::pair<const std::string,const std::string>("F",""));
			rules_.insert(std::pair<const std::string,const std::string>("X","F:X:+:F:X:+:F:X:F:Y:F:X:+:F:X:F:Y:-:F:Y:-:F:Y:-"));
			rules_.insert(std::pair<const std::string,const std::string>("Y","+:F:X:+:F:X:+:F:X:F:Y:-:F:Y:F:X:F:Y:-:F:Y:-:F:Y"));
			c=RuleTurtle2D(rules_, "", "X", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 39: 
			rules_.insert(std::pair<const std::string,const std::string>("F",""));
			rules_.insert(std::pair<const std::string,const std::string>("X","F:X:-:F:X:-:F:X:+:F:Y:+:F:Y:+:F:X:-:F:X"));
			rules_.insert(std::pair<const std::string,const std::string>("Y","F:Y:+:F:Y:-:F:X:-:F:X:-:F:Y:+:F:Y:+:F:Y"));
			c=RuleTurtle2D(rules_, "", "X:-:X:-:X:-:X:-:X", 72.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 40: 
			rules_.insert(std::pair<const std::string,const std::string>("F","F:-:F:-:F:+:+:F:+:F:-:F"));
			c=RuleTurtle2D(rules_, "", "F:-:F:-:F:-:F:-:F", 72.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 41: 
			rules_.insert(std::pair<const std::string,const std::string>("L","+:R:-:F:-:R:+"));
			rules_.insert(std::pair<const std::string,const std::string>("R","-:L:+:F:+:L:-"));
			c=RuleTurtle2D(rules_, "", "L:-:-:F:-:-:L:-:-:F", 45.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 42: 
			rules_.insert(std::pair<const std::string,const std::string>("X","F:-:F:-:F:+:F:+:F:X:+:+:F:-:F:-:F:+:F:+:F:X:-:-:F:-:F:-:F:+:F:+:F:X"));
			rules_.insert(std::pair<const std::string,const std::string>("F",""));
			c=RuleTurtle2D(rules_, "", "X", 60.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 43: 
			rules_.insert(std::pair<const std::string,const std::string>("F","F:+:F:+:F:+:+:+:+:F:+:F:+:F"));
			c=RuleTurtle2D(rules_, "", "F:+:+:+:+:F", 45.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 44: 
			rules_.insert(std::pair<const std::string,const std::string>("F","F:+:F:+:+:F:+:F:F"));
			c=RuleTurtle2D(rules_, "", "F:+:F:+:F:+:F:+:+:F:-:F:-:F:-:F", 90.0f*D3DXFROMWINE_PI/180.0f);
			break;
		case 45: 
			rules_.insert(std::pair<const std::string,const std::string>("F","F:F:+:+:F:+:+:F:+:+:F:F:F"));
			c=RuleTurtle2D(rules_, "", "F:+:+:F:+:+:F:+:+:+:F:-:-:F:-:-:F", 60.0f*D3DXFROMWINE_PI/180.0f);
			break;
	}
	return c;
}

inline void next(std::string & prev,
                 const rules & r)
 {
  std::string result;
 
 // for (const std::string p : prev)
for (unsigned int i = 0;i<prev.length();i++)
{
	std::string sep="";
	if(i)
		sep=":";
	const std::string p(1, prev[i]);
    // hack to ensure const-ness on rules
    rules::const_iterator x=r.find(p);
    if (x!=r.end())
     result+=sep+x->second;
    else
     result+=sep+'X'; // oh noes!
   }
 
  prev=result;
 }

class Turtle
{
protected:
	D3DXFROMWINEVECTOR2 p;
	float angle;
	D3DXFROMWINEVECTOR4 m_color;
public:
    Turtle(D3DXFROMWINEVECTOR2 ap=D3DXFROMWINEVECTOR2(0.0f, 0.0f), float a=0.0f, D3DXFROMWINEVECTOR4 acolor=D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f))
    {
	p = ap;
	angle = a;
	m_color=acolor;
    }
    Turtle(const Turtle& rhs)
    {
	p = rhs.p;
	angle = rhs.angle;
	m_color=rhs.m_color;
    }
#ifndef _MSC_VER
	static std::stack<Turtle> _Turtlestack__;
	static void push(Turtle& aTurtle){_Turtlestack__.push(aTurtle);}
	static Turtle pop()
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
#else
	void push(Turtle& aTurtle);
	Turtle pop();
#endif
    	void move(float adistance)
    	{
        	p.x += adistance * cosf(angle);
        	p.y += adistance * sinf(angle);
		#if 0
			LOG_PRINT("p.x=%f\n",p.x);
			LOG_PRINT("p.y=%f\n",p.y);
		#endif
	}
    	void forward(float adistance)
    	{
        	D3DXFROMWINEVECTOR2 start=p;
        	move(adistance);
        	D3DXFROMWINEVECTOR2 end=p;
		IRenderer::GetRendererInstance()->drawLine(start.x, start.y, end.x, end.y, m_color, 1);
    }
    void turn(float a)
    {
        angle += a;
    }
    void color(D3DXFROMWINEVECTOR4& acolor)
    {
        m_color = acolor;
    }
};
inline void drawLsystem(Turtle& aTurtle, vector<string>& ainstructions, float aangle, float adistance)
{
	for (unsigned int i = 0;i<ainstructions.size();i++)
	{
		string symbol=ainstructions[i];
		//LOG_PRINT("symbol=%s\n", symbol.c_str());
		if (symbol == "[")
			aTurtle.push(aTurtle);
		else if( symbol == "]")
			aTurtle.pop();
		else if( symbol == "F")
			aTurtle.forward(adistance);
		else if( symbol == "b")
			aTurtle.move(adistance);
		else if( symbol == "B")
			aTurtle.forward(-adistance);
		else if( symbol == "+")
			aTurtle.turn(aangle);
		else if( symbol == "-")
			aTurtle.turn(-aangle);
		else if( symbol[0] == 'c')
		{
			std::size_t foundo = symbol.find("(");
  			std::size_t foundc = symbol.find(")");
  			if ((foundo!=std::string::npos) && (foundc!=std::string::npos) && (foundo<foundc))
			{
				std::vector<std::string> v=stx_split(symbol.substr(foundo+1,foundc-1), ',');
				if(v.size()==4)
				{
					D3DXFROMWINEVECTOR4 c(atof(v[0].c_str()), atof(v[1].c_str()), atof(v[2].c_str()), atof(v[3].c_str()));
					aTurtle.color(c);
				}
			}
		}
	}
}
class Turtle3D
{
protected:
	D3DXFROMWINEVECTOR3 P, R;
	D3DXFROMWINEVECTOR4 m_color;
public:
	std::vector<D3DXFROMWINEVECTOR3> path;
	std::vector<D3DXFROMWINEVECTOR3> normals;
	std::vector<__WORD__> indices;
	std::vector<D3DXFROMWINEVECTOR2> texCoords;
public:
    Turtle3D(D3DXFROMWINEVECTOR3 p=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3 r=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR4 acolor=D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f))
    {
	P = p;
	R = r;
	m_color=acolor;
    }
    Turtle3D(const Turtle3D& rhs)
    {
	P = rhs.P;
	R  = rhs.R;
	m_color=rhs.m_color;
    }
    ~Turtle3D()
    {
	path.clear();
	normals.clear();
	indices.clear();
	texCoords.clear();
    }
#ifndef _MSC_VER
	static std::stack<Turtle3D> _Turtle3Dstack__;
	static void push(Turtle3D& aTurtle3D){_Turtle3Dstack__.push(aTurtle3D);}
	static Turtle3D pop()
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
#else
	void push(Turtle3D& aTurtle3D);
	Turtle3D pop();
#endif
    	void move(float adistance)
    	{
		P.x += cosf(R.x) * cosf(R.y) * adistance;
		P.y += sinf(R.x) * cosf(R.y) * adistance;
		P.z += sinf(R.y) * adistance;
		#if 0
			LOG_PRINT("P.x=%f\n",P.x);
			LOG_PRINT("P.y=%f\n",P.y);
			LOG_PRINT("P.z=%f\n",P.z);
		#endif
	}
    	
    void forward(float adistance)
    {
        D3DXFROMWINEVECTOR3 v[2];
	v[0]=P;

	if(!path.size())
		path.push_back(P);

	move(adistance);
	path.push_back(P);
        v[1]=P;

	{
		IRenderer::GetRendererInstance()->InitPlain3(IRenderer::GetRendererInstance()->GetplainShader3(), IRenderer::GetRendererInstance()->GetplainVF3());
		IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GetplainShader3());
		IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GetplainVF3());
	}
		IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", m_color);

		D3DXFROMWINEMATRIX w;//???=GetMVP();
		D3DXFROMWINEMatrixIdentity(&w);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", w);
	
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_LINES, 1, &v[0], &v[0], sizeof(D3DXFROMWINEVECTOR3));
    }
    void turnX(float a)
    {
        R.x += a;
    }
    void turnY(float a)
    {
        R.y += a;
    }
    void turnZ(float a)
    {
        R.z += a;
    }
    void color(D3DXFROMWINEVECTOR4& acolor)
    {
        m_color = acolor;
    }
};
inline void draw3DLsystem(Turtle3D& aTurtle, vector<string>& ainstructions, float aangle, float adistance)
{
	//aTurtle.path.resize(0);
	//aTurtle.texCoords.resize(0);
	aTurtle.normals.resize(0);
	aTurtle.indices.resize(0);
	for (unsigned int i = 0;i<ainstructions.size();i++)
	{
		string symbol=ainstructions[i];
		//LOG_PRINT("symbol=%s\n", symbol.c_str());
		if (symbol == "[")
			aTurtle.push(aTurtle);
		else if( symbol == "]")
			aTurtle.pop();
		else if( symbol == "F")
			aTurtle.forward(adistance);
		else if( symbol == "b")
			aTurtle.move(adistance);
		else if( symbol == "B")
			aTurtle.forward(-adistance);
		else if( symbol == "+")
			aTurtle.turnX(aangle);
		else if( symbol == "-")
			aTurtle.turnX(-aangle);
		else if( symbol == "Y")
			aTurtle.turnY(aangle);
		else if( symbol == "y")
			aTurtle.turnY(-aangle);
		else if( symbol == "Z")
			aTurtle.turnZ(aangle);
		else if( symbol == "z")
			aTurtle.turnZ(-aangle);
		else if( symbol[0] == 'c')
		{
			std::size_t foundo = symbol.find("(");
  			std::size_t foundc = symbol.find(")");
  			if ((foundo!=std::string::npos) && (foundc!=std::string::npos) && (foundo<foundc))
			{
				std::vector<std::string> v=stx_split(symbol.substr(foundo+1,foundc-1), ',');
				if(v.size()==4)
				{
					D3DXFROMWINEVECTOR4 c(atof(v[0].c_str()), atof(v[1].c_str()), atof(v[2].c_str()), atof(v[3].c_str()));
					aTurtle.color(c);
				}
			}
		}
	}
	//aTurtle.texCoords.resize(0);
	aTurtle.normals.resize(0);
	aTurtle.indices.resize(0);

	#if 0
	std::vector<__WORD__> v;	
	for(unsigned int i=0;i<m_Turtle.mIndices.size()/3;i++)
		v.push_back(0);
	std::vector<stx_Material> vt;
	vt.push_back(stx_Material("/textures/ATI SDK/512px-brick.png", D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
	stx_Texture texture(vt, v);
	stx_writeXFile("/turtle.x", aTurtle.mVertices, aTurtle.mTexCoords, texture);
	#endif
}

struct FN
{
	FN(){}
	FN(std::string aname, std::vector<std::string> aargs)
	{
		name=aname;
		args=aargs;
	}
	virtual ~FN(){}
	std::string name;
	std::vector<std::string> args;
};

inline void split(const string &s, char delim, vector<string> &elems) {
	
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

inline vector<string> split(const string &s, char delim) {
	
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

inline bool isfn(const string &rules, const string &arulename, vector<FN> &ruleargsv)
{
	unsigned int pos1 = 0;
	vector<string> rulelines=split(rules, ';');

	for(unsigned int j=0;j<rulelines.size();j++)
	{
	string rulestr=rulelines[j];
	pos1=rulestr.find ("=", 0);
	if(pos1>rulestr.length()) return false;

	string rulename=rulestr.substr (0, pos1);
	if(rulename!=arulename) continue;

	string rulebody=rulestr.substr (pos1+1,rulestr.size());
	vector<string> rulefnsv=split(rulebody, ':');
	for(unsigned int i=0;i<rulefnsv.size();i++)
	{
		string rulestr=rulefnsv[i];
		pos1=rulestr.find_first_of ("(", 0);
		unsigned int pos2=rulestr.find_last_of (")", rulestr.length());
		string ruleop=rulestr.substr (0,pos1);
		string ruleargs=rulestr.substr (pos1+1,pos2-2);
		vector<string> ruleargsv_=split(ruleargs, ',');
		FN fn(ruleop,ruleargsv_);
		ruleargsv.push_back(fn);
	}
	}
	return true;
}

inline std::string getrule(const std::string& Rules_txt, const std::string& key)
{
	vector<string> v=split(Rules_txt, ';');
	std::string rule = "";
	for (unsigned int k = 0;k<v.size();k++)
	{
		vector<string> v1=split(v[k], '=');
		if(v1[0]==key)
		{
			vector<string> v2=split(v1[1], ':');
			rule.append(v2[0]);
			for(unsigned int i=1;i<v2.size();i++)
			{
				rule.append(":");
				rule.append(v2[i]);
			}
			break;
		}
	}
	return rule;
}

inline unsigned int apply_rules(const std::string& Rules_txt, std::string& tilist_, const int depth=0)
{
	unsigned int steps=0;
	while ((tilist_.length() < 3200) && (steps < 3200))
	{
		std::string newlist = "";
		vector<string> v=split(tilist_, ':');
		for (unsigned int i = 0;i<v.size();i++)
		{
			std::string symbol=v[i];
			std::string rule = getrule(Rules_txt, symbol);

			if(rule.length())
			{
			if(i)
				newlist.append(":");
			newlist.append(rule);
			}
		}
		tilist_ = newlist;		
		steps = steps + 1;
	}
	return steps;
}
#endif

