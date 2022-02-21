/*
  Copyright (c) 2022 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

const char* filename[] =
{
"/new/R2VB_VTF/R2VB-OceanWater/WaterRender2.shd"
};

class _Shape3D
{
	TextureID texID;
	MeshRenderer2 shape3D[3];
	std::vector<ShaderID> shader;
	std::vector<VertexFormatID> vf;
	std::vector<TextureID> tex;
	unsigned int s_i, m_i, t_i;
public:
int init(const char* aTitle)
{
    	t_i=0;
    	m_i=2;
    	s_i=0;

	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/ViewportProjectionContent/bluetexture.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/ViewportProjectionContent/greentexture.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/ViewportProjectionContent/redtexture.png", false, IRenderer::GetRendererInstance()->Getlinear()));

	for(unsigned int i=0;i<elementsOf(filename);i++)
	{	
			//printf("Shader=%s failed!\n", filename[i]);
		ShaderID id=IRenderer::GetRendererInstance()->addShaderFromFile(filename[i], "main2", "main");
		if(id==-1)
		{
			printf("Shader=%s failed!\n", filename[i]);
			//stx_exit(0);
			//continue;
		}
		shader.push_back(id);
		
		#if 1
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
		#else
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
			0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
			0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
		};
		#endif
		if(shader[shader.size()-1]!=-1)
		vf.push_back(IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shader[shader.size()-1]));
		else
		vf.push_back(-1);
	}
	return 0;
}

void render( )
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMATRIX matRot;
		D3DXFROMWINEMatrixRotationYawPitchRoll( &matRot,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	matRot=I;
	IRenderer::GetRendererInstance()->setShader(shader[s_i]);
	IRenderer::GetRendererInstance()->setVertexFormat(vf[s_i]);
	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorldViewProjection", matRot);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorld", matRot);
	IRenderer::GetRendererInstance()->setShaderConstant3f("g_vWSEyePos", D3DXFROMWINEVECTOR3(0, 5, 0));
	#if 0
	if(0)
        	IRenderer::GetRendererInstance()->setShaderConstant1f("g_useConstantColor", -1.0f);
	else
		IRenderer::GetRendererInstance()->setShaderConstant1f("g_useConstantColor", 1.0f);
	#endif
	IRenderer::GetRendererInstance()->setTexture("HighFreqWaterNormalSampler", tex[0]);
	IRenderer::GetRendererInstance()->setTexture("SkySampler", tex[2]);

	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	#if 1
	//		   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	unsigned int N=4;
	#else
	float v[16] =  {  1.0f,-1.0f,
			  1.0f, 1.0f,
			 -1.0f,-1.0f,
			 -1.0f, 1.0f };
	unsigned int N=2;
	#endif
#if 1
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], N*sizeof(float));
#else
	__WORD__ i[6] ={0,1,2,2,1,3};
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, 4, 2, &i[0], &i[0], CONSTANT_INDEX2, &v[0], &v[0], N*sizeof(float));
#endif
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
};
int ApplicationLogic()
{	
	IRenderer* r=IRenderer::GetRendererInstance("WaterRender");	
	IInput*    i=STX_Service::GetInputInstance();
	_Shape3D shape3D;
	shape3D.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		shape3D.render();
	}
	return 0;
}

