/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

ShaderID g_shd=-1;
VertexFormatID g_vf=-1;
TextureID texID=-1;

std::vector<Vertex> g_vertices_;

std::vector<D3DXFROMWINEVECTOR3> g_vertices3;
std::vector<D3DXFROMWINEVECTOR3> g_normals3;
std::vector<D3DXFROMWINEVECTOR2> g_texcoords2;
std::vector<__WORD__> g_indices1;
std::vector<__WORD__> g_indices2;

std::vector<stx_Texture> Textures;
std::vector<std::tuple<std::string, D3DXFROMWINEVECTOR3>> vt;

void DrawQuad(D3DXFROMWINEMATRIX& aM, D3DXFROMWINEVECTOR2 asize=D3DXFROMWINEVECTOR2(1.0f, 1.0f))
{
	{D3DXFROMWINEVECTOR3 v(-asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 0.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v( asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 0.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v( -asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 1.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	g_indices2.push_back(vt.size()-1);
	{D3DXFROMWINEVECTOR3 v(-asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 1.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v( asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 1.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v(asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 0.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	g_indices2.push_back(vt.size()-1);
}

class Turtle3D_
{
protected:
	Vertex P;
	D3DXFROMWINEVECTOR3 R;
	D3DXFROMWINEVECTOR3 m_color;
	float  m_width;
public:
	void PushVertices(D3DXFROMWINEVECTOR3& aposition)
	{
		Vertex vertex;
		vertex.position=aposition;
		g_vertices_.push_back(vertex);
	}
    Turtle3D_(D3DXFROMWINEVECTOR3 p=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3 r=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3 acolor=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f),
const float& awidth=0.0f)
    {
	P.position = p;
	R = r;
	m_color=acolor;
	m_width=awidth;
    }
    Turtle3D_(const Turtle3D_& rhs)
    {
	P = rhs.P;
	R  = rhs.R;
	m_color=rhs.m_color;
	m_width=rhs.m_width;
	_Turtle3Dstack__=rhs._Turtle3Dstack__;
    }
	std::vector<Turtle3D_> _Turtle3Dstack__;
	void push(){_Turtle3Dstack__.push_back(*this);}
	Turtle3D_ pop()
{
	if(_Turtle3Dstack__.size())
	{
		Turtle3D_ turtle=_Turtle3Dstack__[_Turtle3Dstack__.size()-1];
		_Turtle3Dstack__.erase (_Turtle3Dstack__.begin()+_Turtle3Dstack__.size()-1); 

		
		return turtle;
	}
	else
		LOG_PRINT("Turtle3D_ stack empty.\n");
	Turtle3D_ turtle;
	return turtle;
}
    	void move(float adistance)
    	{
		P.position.x += cosf(R.x) * cosf(R.y) * adistance;
		P.position.y += sinf(R.x) * cosf(R.y) * adistance;
		P.position.z += sinf(R.y) * adistance;
		#if 1
		LOG_PRINT("P.position.x=%f\n", P.position.x);
		LOG_PRINT("P.position.y=%f\n", P.position.y);
		LOG_PRINT("P.position.z=%f\n", P.position.z);
		#endif
	}

    void forward(float adistance)
    {
	move(adistance);
	D3DXFROMWINEMATRIX w=stx_GetPivotMatrix(R, P.position);
	D3DXFROMWINEVECTOR2 size(adistance, adistance);// ???
	DrawQuad(w, size);
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
    void color(D3DXFROMWINEVECTOR3& acolor)
    {
        m_color = acolor;
    }
    void width(const float& awidth)
    {
        m_width = awidth;
    }
};

IRenderer* r=0;
IInput*    in=0;
Turtle3D_ turtle(D3DXFROMWINEVECTOR3(-0.5f, -0.5f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f));
D3DXFROMWINEVECTOR2 window;
D3DXFROMWINEVECTOR3 p0;

int init(const char* aTitle)
{
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texID=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);
	g_shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/texSHD2N.HLSL.shd", "main", "main");

	FormatDesc format[] =
	{
		0, TYPE_VERTEX,		FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,		FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD,	FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD,	FORMAT_FLOAT, 2
	};
	g_vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), g_shd);
	
	window=D3DXFROMWINEVECTOR2(
		STX_Service::GetWindowInstance()->GetWidth(),
		STX_Service::GetWindowInstance()->GetHeight());

	p0=D3DXFROMWINEVECTOR3(window.x/2.0f, window.y/2.0f, 0.0f);

	turtle.width(0.5f);
	D3DXFROMWINEVECTOR3 color(1.0f, 0.0f, 0.0f);
	turtle.color(color);
	turtle.forward(1.0f);
	turtle.turnX(90.0f*D3DXFROMWINE_PI/180.0f);
	color=D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);
	turtle.color(color);
	turtle.forward(1.0f);
	turtle.turnX(90.0f*D3DXFROMWINE_PI/180.0f);
	color=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
	turtle.color(color);
	turtle.forward(1.0f);
	turtle.turnX(90.0f*D3DXFROMWINE_PI/180.0f);
	color=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f);
	turtle.color(color);
	turtle.forward(1.0f);
#if 0
	turtle.turnZ(90.0f*D3DXFROMWINE_PI/180.0f);
	turtle.move(1.0f);

	turtle.turnY(D3DXFROMWINE_PI/2.0f);
	color=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f);
	turtle.color(color);
	turtle.forward(1.0f);
	turtle.turnY(D3DXFROMWINE_PI);
	color=D3DXFROMWINEVECTOR4(0.5f, 0.5f, 0.5f);
	turtle.color(color);
	turtle.forward(1.0f);
#endif
	return 0;
}
void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	static float mAngleX=45.0f;
	static float mAngleY=45.0f;

	stx_GetAngles(mAngleX, mAngleY, 0.25f);

	IRenderer::GetRendererInstance()->InitTex3(IRenderer::GetRendererInstance()->GettexShader3(), IRenderer::GetRendererInstance()->GettexVF3());
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader3());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GettexVF3());
	D3DXFROMWINEMATRIX R, S, W;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
	D3DXFROMWINEMatrixScaling(&S, 0.25f, 0.25f, 0.25f);
	W=R*S;
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
	IRenderer::GetRendererInstance()->setTexture("Sampler", texID);
	if(g_indices1.size())
		IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, g_vertices_.size(), g_indices1.size()/3, &g_indices1[0], &g_indices1[0], CONSTANT_INDEX2, &g_vertices_[0], &g_vertices_[0], sizeof(Vertex));
	else
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, g_vertices_.size()/3, &g_vertices_[0], &g_vertices_[0], sizeof(Vertex));

		IRenderer::GetRendererInstance()->drawText("Use mouse buttons to rotate the model.", 
			10, 10, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		IRenderer::GetRendererInstance()->drawText("CTRL-r resets the scene.", 
			10, 30, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	r=IRenderer::GetRendererInstance(/* window.x, window.y, */ "L_System3D2");
	in=STX_Service::GetInputInstance();
	init("" /* window.x, window.y */ );
	while (!in->OnKeyPress (STX_KEY_ESCAPE) && !in->Quit())
	{
		in->Update();
		render();
	}
	return 0;
}
/*
		   	   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
*/
/*
		  	   x	  y     tx    ty
	float v[16] =  { -1.0f, -1.0f, 0.0f, 1.0f,  
			  1.0f, -1.0f, 1.0f, 1.0f,   
			  1.0f,  1.0f, 1.0f, 0.0f,  
			 -1.0f,  1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, &v[0], &v[0], 4*sizeof(float));
*/

