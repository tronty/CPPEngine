/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

ShaderID shd=-1;
TextureID texture=-1;
VertexFormatID vf=-1;
D3DXFROMWINEMATRIX m_Proj;
D3DXFROMWINEMATRIX m_View;

void render_()
{
	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMATRIX matRot, m_T;
		D3DXFROMWINEMatrixRotationYawPitchRoll( &matRot,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setTexture("Base", texture);
/*
	D3DXFROMWINEMATRIX I, m_T;
	D3DXFROMWINEMatrixIdentity(&I);
	D3DXFROMWINEMatrixIdentity(&m_T);

		D3DXFROMWINEMATRIX matWorld=m_R*m_S*m_T;
		D3DXFROMWINEMATRIX mvp = matWorld*m_View*m_Proj;
*/
	D3DXFROMWINEMatrixTranslation(&m_T, 0.0f, 0.0f, 5.0f);
	D3DXFROMWINEMATRIX mvp = matRot*m_T*m_View*m_Proj;

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", mvp);
	//IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
}

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/cube/cube.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		//0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		//0, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);

	D3DXFROMWINEMatrixPerspectiveFovLH(&m_Proj, D3DXFROMWINEToRadian( 45.0f ),
                                		float(IRenderer::GetRendererInstance()->GetViewportWidth())/IRenderer::GetRendererInstance()->GetViewportHeight(), 0.001f, 100.0f );


	D3DXFROMWINEVECTOR3 eye(0.0f, 0.0f, 0.0f);
	D3DXFROMWINEVECTOR3 at(0.0f, 0.0f, 1.0f);
	D3DXFROMWINEVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXFROMWINEMatrixLookAtLH(&m_View, &eye, &at, &up);
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

 static const float vertices[] = {
#if 1
        -1.0f, 1.0f,-1.0f,  0.0f,0.0f,
	 1.0f, 1.0f,-1.0f,  1.0f,0.0f,
	-1.0f,-1.0f,-1.0f,  0.0f,1.0f,
	 1.0f,-1.0f,-1.0f,  1.0f,1.0f,
	
	-1.0f, 1.0f, 1.0f,  1.0f,0.0f,
	-1.0f,-1.0f, 1.0f,  1.0f,1.0f,
	 1.0f, 1.0f, 1.0f,  0.0f,0.0f,
	 1.0f,-1.0f, 1.0f,  0.0f,1.0f,
	
	-1.0f, 1.0f, 1.0f,  0.0f,0.0f,
	 1.0f, 1.0f, 1.0f,  1.0f,0.0f,
	-1.0f, 1.0f,-1.0f,  0.0f,1.0f,
	 1.0f, 1.0f,-1.0f,  1.0f,1.0f,
	
	-1.0f,-1.0f, 1.0f,  0.0f,0.0f,
	-1.0f,-1.0f,-1.0f,  1.0f,0.0f,
	 1.0f,-1.0f, 1.0f,  0.0f,1.0f,
	 1.0f,-1.0f,-1.0f,  1.0f,1.0f,

	 1.0f, 1.0f,-1.0f,  0.0f,0.0f,
	 1.0f, 1.0f, 1.0f,  1.0f,0.0f,
	 1.0f,-1.0f,-1.0f,  0.0f,1.0f,
	 1.0f,-1.0f, 1.0f,  1.0f,1.0f,
	
	-1.0f, 1.0f,-1.0f,  1.0f,0.0f,
	-1.0f,-1.0f,-1.0f,  1.0f,1.0f,
	-1.0f, 1.0f, 1.0f,  0.0f,0.0f,
	-1.0f,-1.0f, 1.0f,  0.0f,1.0f
#else
	 -1.0f,-1.0f, 1.0f, 0.0f,0.0f,
	  1.0f,-1.0f, 1.0f, 1.0f,0.0f,
	  1.0f, 1.0f, 1.0f, 1.0f,1.0f,
	 -1.0f, 1.0f, 1.0f, 0.0f,1.0f,

	 -1.0f,-1.0f,-1.0f, 0.0f,0.0f,
	 -1.0f, 1.0f,-1.0f, 1.0f,0.0f,
	  1.0f, 1.0f,-1.0f, 1.0f,1.0f,
	  1.0f,-1.0f,-1.0f, 0.0f,1.0f,

	 -1.0f, 1.0f,-1.0f, 0.0f,0.0f,
	 -1.0f, 1.0f, 1.0f, 1.0f,0.0f,
	  1.0f, 1.0f, 1.0f, 1.0f,1.0f,
	  1.0f, 1.0f,-1.0f, 0.0f,1.0f,

	 -1.0f,-1.0f,-1.0f, 0.0f,0.0f,
	  1.0f,-1.0f,-1.0f, 1.0f,0.0f,
	  1.0f,-1.0f, 1.0f, 1.0f,1.0f,
	 -1.0f,-1.0f, 1.0f, 0.0f,1.0f,

	  1.0f,-1.0f,-1.0f, 0.0f,0.0f,
	  1.0f, 1.0f,-1.0f, 1.0f,0.0f,
	  1.0f, 1.0f, 1.0f, 1.0f,1.0f,
	  1.0f,-1.0f, 1.0f, 0.0f,1.0f,

	 -1.0f,-1.0f,-1.0f, 0.0f,0.0f,
	 -1.0f,-1.0f, 1.0f, 1.0f,0.0f,
	 -1.0f, 1.0f, 1.0f, 1.0f,1.0f,
	 -1.0f, 1.0f,-1.0f, 0.0f,1.0f
#endif
    };

	render_();
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, vertices+0, vertices+0, 5*sizeof(float));
	render_();
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, vertices+20, vertices+20, 5*sizeof(float));
	render_();
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, vertices+40, vertices+40, 5*sizeof(float));
	render_();
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, vertices+60, vertices+60, 5*sizeof(float));
	render_();
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, vertices+80, vertices+80, 5*sizeof(float));
	render_();
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, vertices+100, vertices+100, 5*sizeof(float));

	float f=0.0f/256.0f;
	D3DXFROMWINEVECTOR4 col(f, f, f, 1.0f);
		const char* txt = "Use mouse buttons to rotate the model.";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 10, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest(), col);

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("cube");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}


