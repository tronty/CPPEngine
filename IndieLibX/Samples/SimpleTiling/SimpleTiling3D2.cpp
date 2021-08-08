/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define defaultshader eSimpleShader
//define alternativeshader eAmbientLighting
#define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
//define alternativeshader ePhongLighting2

std::vector<stx_Texture> Textures;
std::vector<stx_Material> vt;

std::vector<D3DXFROMWINEVECTOR3> g_vertices3;
std::vector<D3DXFROMWINEVECTOR3> g_normals3;
std::vector<D3DXFROMWINEVECTOR2> g_texcoords2;
std::vector<__WORD__> g_indices1;
std::vector<__WORD__> g_indices2;

MeshRenderer2* g_mesh;
std::vector<stx_Mesh> mesh_;

TextureID texID=-1;

TextureID mortarjointTexID=-1;
TextureID redbrickTexID=-1;

ShaderID shd=-1;
VertexFormatID vf=-1;

void RenderTex(TextureID id, D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize, D3DXFROMWINEVECTOR2 texposition=D3DXFROMWINEVECTOR2(0.0f, 0.0f), D3DXFROMWINEVECTOR2 texsize=D3DXFROMWINEVECTOR2(1.0f, 1.0f))
{
	D3DXFROMWINEVECTOR4 col(1.0f, 1.0f, 1.0f, 1.0f);
	TexVertex dest[4];
	float x=aposition.x;
	float y=aposition.y;
	dest[0].position = D3DXFROMWINEVECTOR2(x + asize.x, y);
	dest[0].texCoord = D3DXFROMWINEVECTOR2(texposition.x+texsize.x, texposition.y);
	dest[1].position = D3DXFROMWINEVECTOR2(x + asize.x, y + asize.y);
	dest[1].texCoord = D3DXFROMWINEVECTOR2(texposition.x+texsize.x, texposition.y+texsize.y);
	dest[2].position = D3DXFROMWINEVECTOR2(x, y);
	dest[2].texCoord = D3DXFROMWINEVECTOR2(texposition.x, texposition.y);
	dest[3].position = D3DXFROMWINEVECTOR2(x, y + asize.y);
	dest[3].texCoord = D3DXFROMWINEVECTOR2(texposition.x, texposition.y+texsize.y);
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", col);
	IRenderer::GetRendererInstance()->setTexture("Base", id);
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &dest[0], &dest[0], sizeof(TexVertex));
}

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Texture/Texture.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texID=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss); 
	redbrickTexID=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/redbrick.png", false, ss);
	mortarjointTexID=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/mortarjoint.png", false, ss);
	vt.push_back(stx_Material("textures/ATI_SDK/redbrick.png", D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
	vt.push_back(stx_Material("textures/ATI_SDK/mortarjoint.png", D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));

	D3DXFROMWINEVECTOR3 aAngle(0.0f, 0.0f, 0.0f);
	for(unsigned int i=0;i<4;i++)
	{
	for(unsigned int j=0;j<5;j++)
	{
		{
			D3DXFROMWINEVECTOR3 p(i*(1.0f+0.05f), j*(0.5f+0.05f), 0.0f);
			D3DXFROMWINEVECTOR2 s(1.0f, 0.5f);
			stx_AddTexturedCube(g_vertices3, g_normals3, g_texcoords2, g_indices1, g_indices2, 1, p, s, aAngle);
		}
		{
			D3DXFROMWINEVECTOR3 p(0.0f+i*1.05f, -0.05f+j*0.55f, 0.0f);
			D3DXFROMWINEVECTOR2 s(1.05f, 0.05f);
			stx_AddTexturedCube(g_vertices3, g_normals3, g_texcoords2, g_indices1, g_indices2, 2, p, s, aAngle);
		}
		{
			D3DXFROMWINEVECTOR3 p(-0.05f+i*1.05f, -0.05f+j*0.55f, 0.0f);
			D3DXFROMWINEVECTOR2 s(0.05f, 0.55f);
			stx_AddTexturedCube(g_vertices3, g_normals3, g_texcoords2, g_indices1, g_indices2, 2, p, s, aAngle);
		}
	}
	}

	LOG_PRINT("g_vertices3.size() =%d\n", g_vertices3.size());
	LOG_PRINT("g_normals3.size()  =%d\n", g_normals3.size());
	LOG_PRINT("g_texcoords2.size()=%d\n", g_texcoords2.size());
	LOG_PRINT("g_indices1.size()  =%d\n", g_indices1.size());
	LOG_PRINT("g_indices2.size()  =%d\n", g_indices2.size());
#if 0
	stx_Texture texture(vt, g_indices2);
	stx_writeXFile("/SimpleTiling3D2.x", mesh_);//, g_vertices3, g_normals3, g_texcoords2, g_indices1, texture);
#else
	if(mesh_.size()){
	mesh_[0].vertices.clear();
	mesh_[0].indices.clear();
	for(unsigned int i=0;i<g_vertices3.size();i++)
	{
		stx_VertexPositionNormalTexture v;
		v.Position=g_vertices3[i];
		v.Normal=g_normals3[i];
		v.Tex=g_texcoords2[i];
		mesh_[0].vertices.push_back(v);
	}
	mesh_[0].indices=gindices1;
	stx_Materials_(vt, g_indices2, mesh_);
	stx_writeXFile("/SimpleTiling3D2.x", mesh_);
	}
#endif

#if 0
	g_mesh=FW3::GetInstance()->GetMesh("/SimpleTiling3D2.x", eSimpleShader);
#else
	g_mesh=new MeshRenderer2("/SimpleTiling3D2.x", eSimpleShader);
#endif
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	D3DXFROMWINEMATRIX w;
	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMatrixRotationYawPitchRoll( &w,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );

	g_mesh->render(w);

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
	IRenderer* r=IRenderer::GetRendererInstance("SimpleTiling3D2");	
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

