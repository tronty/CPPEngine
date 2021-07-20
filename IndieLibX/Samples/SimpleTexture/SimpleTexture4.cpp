/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

ShaderID shd=-1;
TextureID texture[6];
VertexFormatID vf=-1;

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.shd", "mainVS", "mainPS");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	SamplerStateID ss=IRenderer::GetRendererInstance()->GetlinearClamp();
	texture[0]=IRenderer::GetRendererInstance()->addImageLibTexture("/dx8sdk/tree01S.png", false, ss);
	texture[1]=IRenderer::GetRendererInstance()->addImageLibTexture("/dx8sdk/tree02S.png", false, ss);
	texture[2]=IRenderer::GetRendererInstance()->addImageLibTexture("/dx8sdk/tree35S.png", false, ss);
	texture[3]=IRenderer::GetRendererInstance()->addImageLibTexture("/AdvAniCh12/Particle_People1.png", false, ss);
	texture[4]=IRenderer::GetRendererInstance()->addImageLibTexture("/AdvAniCh12/Particle_People2.png", false, ss);
	texture[5]=IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Grass/grassPack.png", false, ss);
	return 0;
}

void render1()
{
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setTexture("Base", texture[5]);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);

	if(IRenderer::GetRendererInstance()->getString()=="D3D11") IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
	//if(IRenderer::GetRendererInstance()->getString()=="D3D11") IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOneReverseSubstract());
  	else IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	//		   x	 y     tx    ty
	float v1[16] =  {  0.0f,0.0f, 1.0f, 1.0f,
			  0.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,0.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v1[0], &v1[0], 4*sizeof(float));

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setTexture("Base", texture[1]);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
  	if(IRenderer::GetRendererInstance()->getString()=="D3D11") IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
	//if(IRenderer::GetRendererInstance()->getString()=="D3D11") IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOneReverseSubstract());
  	else IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	//		   x	 y     tx    ty
	float v2[16] =  {  1.0f,0.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 0.0f,0.0f, 0.0f, 1.0f,
			 0.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v2[0], &v2[0], 4*sizeof(float));


	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setTexture("Base", texture[3]);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
  	if(IRenderer::GetRendererInstance()->getString()=="D3D11") IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
	//if(IRenderer::GetRendererInstance()->getString()=="D3D11") IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOneReverseSubstract());
  	else IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	//		   x	 y     tx    ty
	float v3[16] =  {  0.0f,-1.0f, 1.0f, 1.0f,
			  0.0f, 0.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 0.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v3[0], &v3[0], 4*sizeof(float));


	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setTexture("Base", texture[4]);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
  	if(IRenderer::GetRendererInstance()->getString()=="D3D11") IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
	//if(IRenderer::GetRendererInstance()->getString()=="D3D11") IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOneReverseSubstract());
  	else IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	//		   x	 y     tx    ty
	float v4[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 0.0f, 1.0f, 0.0f,
			 0.0f,-1.0f, 0.0f, 1.0f,
			 0.0f, 0.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v4[0], &v4[0], 4*sizeof(float));
}

void render2()
{
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
  	if(IRenderer::GetRendererInstance()->getString()=="D3D11") IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
	//if(IRenderer::GetRendererInstance()->getString()=="D3D11") IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOneReverseSubstract());
  	else IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->setTexture("Base", texture[5]);
	//		   x	 y     tx    ty
	float v1[16] =  { 0.0f, 0.0f, 1.0f, 1.0f,
			  0.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f, 0.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v1[0], &v1[0], 4*sizeof(float), iDrawFlag);
	IRenderer::GetRendererInstance()->setTexture("Base", texture[1]);
	//		   x	 y     tx    ty
	float v2[16] =  { 1.0f, 0.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			  0.0f, 0.0f, 0.0f, 1.0f,
			  0.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v2[0], &v2[0], 4*sizeof(float), iDrawFlag);
	IRenderer::GetRendererInstance()->setTexture("Base", texture[3]);
	//		   x	 y     tx    ty
	float v3[16] =  { 0.0f,-1.0f, 1.0f, 1.0f,
			  0.0f, 0.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 0.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v3[0], &v3[0], 4*sizeof(float), iDrawFlag);
	IRenderer::GetRendererInstance()->setTexture("Base", texture[4]);
	//		   x	 y     tx    ty
	float v4[16] =  { 1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 0.0f, 1.0f, 0.0f,
			  0.0f,-1.0f, 0.0f, 1.0f,
			  0.0f, 0.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v4[0], &v4[0], 4*sizeof(float), iDrawFlag);
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	render1();

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("SimpleTexture4");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
		//stx_exit(0);
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

