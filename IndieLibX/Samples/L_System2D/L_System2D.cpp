/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
/*
# The L-system for the 2D-modelling.
*/
#include <Framework3/Turtle.h>
//include <Tools/NSString.h>

string axiom;
float angle;
float distance_ = 5.0f;
const unsigned int numIters=4;
vector<string> inst;
D3DXFROMWINEVECTOR2 window;
D3DXFROMWINEVECTOR2 p0;

IRenderer* r=0;
IInput*    in=0;

#ifndef _MSC_VER
std::stack<Turtle> Turtle::_Turtlestack__;
#endif

void getInstructions(unsigned int aI, vector<string>& inst_)
{
  inst_.clear();
  std::string population;
  rules immortal;
#if 1
  RuleTurtle2D r=get_rule(aI);
  axiom=r.m_axiom;
  immortal=r.m_rules;
  angle=r.m_angle;
#else
  axiom="F";
  immortal.insert(std::pair<const std::string,const std::string>("F", "F:-:F:+:+:F:-:F"));
  //immortal.insert(std::pair<const std::string,const std::string>("F", "F:+:F:-:-:F:+:F"));
  angle = 60.0f*D3DXFROMWINE_PI/180.0f;
#endif
  population=axiom;
  for (int i=0;i<numIters;i++)
   {
#if 1
    std::cout
     << std::setw(3) << population.size()
     << " "
     << population << std::endl; 
#endif
    next(population,immortal);
   }
	inst_=stx_split(population, ':');
}

int init(const char* aTitle)
{
	window=D3DXFROMWINEVECTOR2(
		STX_Service::GetWindowInstance()->GetWidth(),
		STX_Service::GetWindowInstance()->GetHeight());

	p0=D3DXFROMWINEVECTOR2(window.x/2.0f, window.y/2.0f);

	getInstructions(1, inst);
	return 0;
}
	
void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
#if 1
	static unsigned int iii=1;
	if(STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_LEFT))	
	{
		iii++;
		if(iii>45)
			iii=1;
		getInstructions(iii, inst);
	}
	else if(STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_RIGHT))	
	{
		iii--;
		if(iii==0)
			iii=45;
		getInstructions(iii, inst);
	}

	Turtle turtle(p0, angle);	
	drawLsystem(turtle, inst, angle, distance_);
#else
	angle = 90.0f*D3DXFROMWINE_PI/180.0f;
	distance_ = 90.0f;
	Turtle turtle(p0, angle);

	turtle.forward(distance_);
	turtle.turn(angle);

	turtle.forward(distance_);
	turtle.turn(angle);

	turtle.forward(distance_);
	turtle.turn(angle);

	turtle.forward(distance_);
#endif
		const char* txt = "Use mouse buttons to change the L-system.";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 10, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

#if 1
		char txt2[128];
		stx_snprintf(txt2, 128, "iii=%d\n", iii);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 30, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());
#endif

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	r=IRenderer::GetRendererInstance( /* window.x, window.y, */ "L_System2D");
	in=STX_Service::GetInputInstance();
	init("" /* window.x, window.y */ );
	while (!in->OnKeyPress (STX_KEY_ESCAPE) && !in->Quit())
	{
		in->Update();
		render();
	}
	return 0;
}

