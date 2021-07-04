/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

ShaderID shd=-1;
TextureID texture=-1;
VertexFormatID vf=-1;

std::vector<D3DXFROMWINEVECTOR3> pv;
float distance_=90.0f;
float angle;
D3DXFROMWINEVECTOR2 window;
D3DXFROMWINEVECTOR3 p0;
D3DXFROMWINEVECTOR3 p_;

    	void move(float adistance)
    	{
		p_.x += adistance * cosf(angle);
        	p_.y += adistance * sinf(angle);
/*
		LOG_PRINT("p_.x=%f\n", p_.x);
		LOG_PRINT("p_.y=%f\n", p_.y);
*/
	}
    	void forward(float adistance)
    	{
        	move(adistance);
		pv.push_back(D3DXFROMWINEVECTOR3(p_.x, p_.y, 0.0f));
    }
    void turn(float a)
    {
        angle += a;
    }
    void draw(const D3DXFROMWINEMATRIX& MVP)
    {
    		D3DXFROMWINEVECTOR4 color(1.0f, 1.0f, 1.0f, 1.0f);
		D3DXFROMWINEVECTOR2 tc[3]={	D3DXFROMWINEVECTOR2(0.0f, 0.0f),
						D3DXFROMWINEVECTOR2(0.0f, 1.0f),
						D3DXFROMWINEVECTOR2(1.0f, 1.0f)};

    		IRenderer::GetRendererInstance()->Begin(PRIM_LINES);
		//LOG_PRINT("pv.size()=%d\n", pv.size());
		for(unsigned int i=0;i<pv.size();i++)
		{
    			IRenderer::GetRendererInstance()->Vertex3f( pv[3*i].x, pv[3*i].y, 0.0f);
    			//IRenderer::GetRendererInstance()->TexCoord2f(tc[0].x, tc[0].y);
    			IRenderer::GetRendererInstance()->Color3fv(&color.x);
    			IRenderer::GetRendererInstance()->Vertex3f( pv[3*i+1].x, pv[3*i+1].y, 0.0f);
    			//IRenderer::GetRendererInstance()->TexCoord2f(tc[1].x, tc[1].y);
    			IRenderer::GetRendererInstance()->Color3fv(&color.x);
    			IRenderer::GetRendererInstance()->Vertex3f( pv[3*i+2].x, pv[3*i+2].y, 0.0f);
    			//IRenderer::GetRendererInstance()->TexCoord2f(tc[2].x, tc[2].y);
    			IRenderer::GetRendererInstance()->Color3fv(&color.x);
		}
    		IRenderer::GetRendererInstance()->End(texture, &MVP);
		pv.clear();
    }

int init(const char* aTitle)
{
	window=D3DXFROMWINEVECTOR2(
		STX_Service::GetWindowInstance()->GetWidth(),
		STX_Service::GetWindowInstance()->GetHeight());
	p0=D3DXFROMWINEVECTOR3(window.x/2.0f, window.y/2.0f, 0.0f);
	p_=p0;
	pv.push_back(D3DXFROMWINEVECTOR3(p_.x, p_.y, 0.0f));
	angle = 0.0f*D3DXFROMWINE_PI/180.0f;
#if defined(USE_HLSL_SHADERS)
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture_.shd", "main", "main");
#else
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.GLSL.shd", "main", "main");
#endif
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);
	return 0;
}

void drawQuad(const D3DXFROMWINEMATRIX& w, const D3DXFROMWINEVECTOR4 color=D3DXFROMWINEVECTOR4(0.6f, 0.6f, 0.6f, 1.0f))
{
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setTexture("Base", texture);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", w);
	//IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	//		   x	 y     z     tx    ty
	float v[20] =  {  1.0f,-1.0f, -1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, -1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, -1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 5*sizeof(float));
}

void getRM(D3DXFROMWINEMATRIX& R, const float mAngleX, const float mAngleY, const unsigned int n=0, const unsigned int m=0)
{
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX)+n*D3DXFROMWINE_PI/2.0f,
		                            D3DXFROMWINEToRadian(mAngleY)+m*D3DXFROMWINE_PI/2.0f,
		                            0.0f );
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMATRIX R;
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );

	D3DXFROMWINEMATRIX I, S, T, MVP;
	D3DXFROMWINEMatrixIdentity(&I);
	D3DXFROMWINEMatrixScaling(&S, 0.25f, 0.25f, 0.25f);
	D3DXFROMWINEMatrixTranslation(&T, 0.0f, 0.0f, 0.0f);
#if 0
	p_=D3DXFROMWINEVECTOR3(window.x/2.0f, window.y/2.0f, 0.0f);
	float angle_ = 90.0f*D3DXFROMWINE_PI/180.0f;

	MVP=I;//R*S;

	forward(distance_);	
	turn(angle_);

	forward(distance_);
	turn(angle_);

	forward(distance_);
	turn(angle_);

	forward(distance_);
	draw(MVP);
#elif 1
	for(unsigned int i=0;i<4;i++)
	{
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX)+i*D3DXFROMWINE_PI/2.0f,
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
		MVP=R*S*T;
		drawQuad(MVP);
	}
	for(unsigned int i=1;i<4;i+=2)
	{
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY)+i*D3DXFROMWINE_PI/2.0f,
		                            0.0f );
		MVP=R*S*T;
		drawQuad(MVP);
	}
#elif 0
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX)+0*D3DXFROMWINE_PI/2.0f,
		                            D3DXFROMWINEToRadian(mAngleY)+1*D3DXFROMWINE_PI/2.0f,
		                            0.0f );
		MVP=R*S*T;
		drawQuad(MVP);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX)+0*D3DXFROMWINE_PI/2.0f,
		                            D3DXFROMWINEToRadian(mAngleY)+3*D3DXFROMWINE_PI/2.0f,
		                            0.0f );
		MVP=R*S*T;
		drawQuad(MVP);
#endif
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );

	//stx_exit(0);
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("TexturedCube2");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
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

