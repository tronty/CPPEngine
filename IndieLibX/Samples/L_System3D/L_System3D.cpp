/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 3.1:
      http://opensource.org/licenses/LGPL-3.1
*/
/*
# The L-system for the 3D-modelling of an house.

# scene
# highway
# road
# path
# yard
# house
# wall
# door
# window
# ceiling/roof
# chimney
# rain gutter

# VertexVector
# IndexVector
# TRIANGLESTRIP
# TRIANGLE
*/
#include <Framework3/IRenderer.h>
#include <Framework3/Turtle.h>
//include <Tools/std::string.h>

#include <string>
#include <stack>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>

rules immortal;

const std::string axiom="F";
const std::string rule="F:-:F:+:+:F:-:F";
//const std::string rule="F:+:F:-:-:F:+:F";
const float angle = 60.0f*D3DXFROMWINE_PI/180.0f;
const float distance_ = 1.0f;
const unsigned int numIters=4;
vector<string> inst;
const D3DXFROMWINEVECTOR4 col(1.0f,1.0f,1.0f,1.0f);
D3DXFROMWINEVECTOR2 window(800, 600);
D3DXFROMWINEVECTOR3 p0(0.0f, 0.0f, 0.0f);
const D3DXFROMWINEVECTOR3 r0(0.0f, 0.0f, 0.0f);
std::string population=axiom;

IRenderer* r=0;
IInput*    in=0;
ShaderID shd = -1;
VertexFormatID vf = -1;

#ifndef _MSC_VER
std::stack<Turtle3D> Turtle3D::_Turtle3Dstack__;
#endif

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/plainSHD2.HLSL.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	window=D3DXFROMWINEVECTOR2(
		STX_Service::GetWindowInstance()->GetWidth(),
		STX_Service::GetWindowInstance()->GetHeight());
	p0=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);

  immortal.insert(std::pair<const std::string,const std::string>(axiom, rule));
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
	split(population, ':', inst);
	return 0;
}
void drawQuad(const float x0, const float y0, const float x1, const float y1){
	D3DXFROMWINEVECTOR4 color(1.0f, 1.0f, 1.0f, 1.0f);
	uint col = toBGRA(color);

	D3DXFROMWINEVECTOR3 vertices[4] = {
		D3DXFROMWINEVECTOR3(x0, y0, 0.0f),
		D3DXFROMWINEVECTOR3(x1, y0, 0.0f),
		D3DXFROMWINEVECTOR3(x1, y1, 0.0f),
		D3DXFROMWINEVECTOR3(x0, y1, 0.0f)
	};
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);

	D3DXFROMWINEMATRIX w;
	D3DXFROMWINEMatrixIdentity(&w);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", w);
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", color);
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR3));
}
void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
#if 0
	//drawQuad(window.x/2.0f, window.y/2.0f, window.x, window.y);
	//drawQuad(-0.25f, -0.25f, 0.25f, 0.25f);
	drawQuad(-0.5f, -0.5f, 0.5f, 0.5f);
	//drawQuad(-0.75f, -0.75f, 0.75f, 0.75f);
	//drawQuad(-1.0f, -1.0f, 1.0f, 1.0f);
#elif 0
	Turtle3D turtle(D3DXFROMWINEVECTOR3(-0.5f, -0.5f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f));
	turtle.forward(1.0f);
	turtle.turnX(90.0f*D3DXFROMWINE_PI/180.0f);
	turtle.forward(1.0f);
	turtle.turnX(90.0f*D3DXFROMWINE_PI/180.0f);
	turtle.forward(1.0f);
	turtle.turnX(90.0f*D3DXFROMWINE_PI/180.0f);
	turtle.forward(1.0f);
#else
	Turtle3D turtle(p0, r0);	
	draw3DLsystem(turtle, inst, angle, distance_);
#endif
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	r=IRenderer::GetRendererInstance( /* window.x, window.y, */ "L_System3D");
	in=STX_Service::GetInputInstance();
	init("" /* window.x, window.y */ );
	while (!in->OnKeyPress (STX_KEY_ESCAPE) && !in->Quit())
	{
		in->Update();
		render();
	}
	return 0;
}

