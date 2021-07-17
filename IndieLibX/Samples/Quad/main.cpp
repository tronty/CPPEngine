/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

//		     x	   y     tx    ty
float  vv[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
		    1.0f, 1.0f, 1.0f, 0.0f,
		   -1.0f,-1.0f, 0.0f, 1.0f,
		   -1.0f, 1.0f, 0.0f, 0.0f };

ShaderID shd=-1;
TextureID texture=-1;
VertexFormatID vf=-1;
std::vector<float> vvv;
std::vector<float> vvv2;

int init(const char* aTitle)
{
	vvv.push_back(vv[0]);
	vvv.push_back(vv[1]);
	vvv.push_back(1.0f);
	vvv.push_back(vv[2]);
	vvv.push_back(vv[3]);
	vvv.push_back(vv[4]);
	vvv.push_back(vv[5]);
	vvv.push_back(1.0f);
	vvv.push_back(vv[6]);
	vvv.push_back(vv[7]);
	vvv.push_back(vv[8]);
	vvv.push_back(vv[9]);
	vvv.push_back(1.0f);
	vvv.push_back(vv[10]);
	vvv.push_back(vv[11]);
	vvv.push_back(vv[12]);
	vvv.push_back(vv[13]);
	vvv.push_back(1.0f);
	vvv.push_back(vv[14]);
	vvv.push_back(vv[15]);
	vvv.push_back(vv[0]);
	vvv.push_back(vv[1]);
	vvv.push_back(-1.0f);
	vvv.push_back(vv[2]);
	vvv.push_back(vv[3]);
	vvv.push_back(vv[4]);
	vvv.push_back(vv[5]);
	vvv.push_back(-1.0f);
	vvv.push_back(vv[6]);
	vvv.push_back(vv[7]);
	vvv.push_back(vv[8]);
	vvv.push_back(vv[9]);
	vvv.push_back(-1.0f);
	vvv.push_back(vv[10]);
	vvv.push_back(vv[11]);
	vvv.push_back(vv[12]);
	vvv.push_back(vv[13]);
	vvv.push_back(-1.0f);
	vvv.push_back(vv[14]);
	vvv.push_back(vv[15]);
	vvv.push_back(vv[0]);
	vvv.push_back(1.0f);
	vvv.push_back(vv[1]);
	vvv.push_back(vv[2]);
	vvv.push_back(vv[3]);
	vvv.push_back(vv[4]);
	vvv.push_back(1.0f);
	vvv.push_back(vv[5]);
	vvv.push_back(vv[6]);
	vvv.push_back(vv[7]);
	vvv.push_back(vv[8]);
	vvv.push_back(1.0f);
	vvv.push_back(vv[9]);
	vvv.push_back(vv[10]);
	vvv.push_back(vv[11]);
	vvv.push_back(vv[12]);
	vvv.push_back(1.0f);
	vvv.push_back(vv[13]);
	vvv.push_back(vv[14]);
	vvv.push_back(vv[15]);
	vvv.push_back(vv[0]);
	vvv.push_back(-1.0f);
	vvv.push_back(vv[1]);
	vvv.push_back(vv[2]);
	vvv.push_back(vv[3]);
	vvv.push_back(vv[4]);
	vvv.push_back(-1.0f);
	vvv.push_back(vv[5]);
	vvv.push_back(vv[6]);
	vvv.push_back(vv[7]);
	vvv.push_back(vv[8]);
	vvv.push_back(-1.0f);
	vvv.push_back(vv[9]);
	vvv.push_back(vv[10]);
	vvv.push_back(vv[11]);
	vvv.push_back(vv[12]);
	vvv.push_back(-1.0f);
	vvv.push_back(vv[13]);
	vvv.push_back(vv[14]);
	vvv.push_back(vv[15]);
	vvv.push_back(1.0f);
	vvv.push_back(vv[0]);
	vvv.push_back(vv[1]);
	vvv.push_back(vv[2]);
	vvv.push_back(vv[3]);
	vvv.push_back(1.0f);
	vvv.push_back(vv[4]);
	vvv.push_back(vv[5]);
	vvv.push_back(vv[6]);
	vvv.push_back(vv[7]);
	vvv.push_back(1.0f);
	vvv.push_back(vv[8]);
	vvv.push_back(vv[9]);
	vvv.push_back(vv[10]);
	vvv.push_back(vv[11]);
	vvv.push_back(1.0f);
	vvv.push_back(vv[12]);
	vvv.push_back(vv[13]);
	vvv.push_back(vv[14]);
	vvv.push_back(vv[15]);
	vvv.push_back(-1.0f);
	vvv.push_back(vv[0]);
	vvv.push_back(vv[1]);
	vvv.push_back(vv[2]);
	vvv.push_back(vv[3]);
	vvv.push_back(-1.0f);
	vvv.push_back(vv[4]);
	vvv.push_back(vv[5]);
	vvv.push_back(vv[6]);
	vvv.push_back(vv[7]);
	vvv.push_back(-1.0f);
	vvv.push_back(vv[8]);
	vvv.push_back(vv[9]);
	vvv.push_back(vv[10]);
	vvv.push_back(vv[11]);
	vvv.push_back(-1.0f);
	vvv.push_back(vv[12]);
	vvv.push_back(vv[13]);
	vvv.push_back(vv[14]);
	vvv.push_back(vv[15]);
		
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/texSHD1.HLSL.shd", "main", "main");
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

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->setTexture("Sampler", texture);

	D3DXFROMWINEMATRIX R;
	{
	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
	}
/*
	if(0)
	{
		D3DXFROMWINEVec3TransformCoordArray((D3DXFROMWINEVECTOR3*)&vvv2[0], 5*sizeof(float), (D3DXFROMWINEVECTOR3*)&vvv[0], 5*sizeof(float), &R, 120);

		D3DXFROMWINEVec3TransformCoord(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv, &R);

		D3DXFROMWINEMATRIX I;
		D3DXFROMWINEMatrixIdentity(&I);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 118, &vvv2[0], &vvv2[0], 5*sizeof(float));
	}else
*/
	{
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", R);
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 118, &vvv[0], &vvv[0], 5*sizeof(float));
	}

		const char* txt = "Use mouse buttons to rotate the model.";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 10, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		txt = "CTRL-r resets the scene.";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 30, 
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
	IRenderer* r=IRenderer::GetRendererInstance("Quad");	
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
		  	   x	  y     tx    ty
	float v[16] =  { -1.0f, -1.0f, 0.0f, 1.0f,  
			  1.0f, -1.0f, 1.0f, 1.0f,   
			  1.0f,  1.0f, 1.0f, 0.0f,  
			 -1.0f,  1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, &v[0], &v[0], 4*sizeof(float));
*/
/*
		   	   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
*/

