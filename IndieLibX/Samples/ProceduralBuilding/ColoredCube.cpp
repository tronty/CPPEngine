/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

IRenderer* r=0;
IInput*    in=0;

std::vector<stx_Texture> Textures;
std::vector<stx_Material> vt;
std::vector<stx_Mesh> mesh_;

TextureID texID=-1;
ShaderID shd=-1;
VertexFormatID vf=-1;

std::vector<VertexC> g_vertices_;

std::vector<D3DXFROMWINEVECTOR3> g_vertices3;
std::vector<D3DXFROMWINEVECTOR3> g_normals3;
std::vector<D3DXFROMWINEVECTOR2> g_texcoords2;
std::vector<__WORD__> g_indices1;
std::vector<__WORD__> g_indices2;

void DrawQuad(D3DXFROMWINEMATRIX& aM, D3DXFROMWINEVECTOR4& m_color, D3DXFROMWINEVECTOR2 asize=D3DXFROMWINEVECTOR2(1.0f, 1.0f))
{
	stx_VertexPositionNormalTexture v_;
	{D3DXFROMWINEVECTOR3 v(-asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 0.0f);VertexC vx(vo, m_color, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);
v_.Position=vo;mesh_[0].indices.push_back(g_vertices3.size()-1);v_.Tex=tc;
mesh_[0].vertices.push_back(v_);}
	{D3DXFROMWINEVECTOR3 v( asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 0.0f);VertexC vx(vo, m_color, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);
v_.Position=vo;mesh_[0].indices.push_back(g_vertices3.size()-1);v_.Tex=tc;
mesh_[0].vertices.push_back(v_);}
	{D3DXFROMWINEVECTOR3 v( -asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 1.0f);VertexC vx(vo, m_color, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);
v_.Position=vo;mesh_[0].indices.push_back(g_vertices3.size()-1);v_.Tex=tc;
mesh_[0].vertices.push_back(v_);}
	#if 0
	if(0==((g_indices1.size()%3)))
	{
		if(texID>-1)
		{
			unsigned int  pos = find(textures.begin(), textures.end(), texID) - textures.begin();
			if(texID==textures[pos])
				g_indices2.push_back(pos);
			else
				g_indices2.push_back(0);
		}
		else
			g_indices2.push_back(0);
	}
	#endif
	{D3DXFROMWINEVECTOR3 v(-asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 1.0f);VertexC vx(vo, m_color, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);
v_.Position=vo;mesh_[0].indices.push_back(g_vertices3.size()-1);v_.Tex=tc;
mesh_[0].vertices.push_back(v_);}
	{D3DXFROMWINEVECTOR3 v( asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 1.0f);VertexC vx(vo, m_color, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);
v_.Position=vo;mesh_[0].indices.push_back(g_vertices3.size()-1);v_.Tex=tc;
mesh_[0].vertices.push_back(v_);}
	{D3DXFROMWINEVECTOR3 v(asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 0.0f);VertexC vx(vo, m_color, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);
v_.Position=vo;mesh_[0].indices.push_back(g_vertices3.size()-1);v_.Tex=tc;
mesh_[0].vertices.push_back(v_);}
	#if 0
	if(0==((g_indices1.size()%3)))
	{
		if(texID>-1)
		{
			unsigned int  pos = find(textures.begin(), textures.end(), texID) - textures.begin();
			if(texID==textures[pos])
				g_indices2.push_back(pos);
			else
				g_indices2.push_back(0);
		}
		else
			g_indices2.push_back(0);
	}
	#endif
}

void drawCube(D3DXFROMWINEVECTOR3 aPosition, D3DXFROMWINEVECTOR2 aSize, D3DXFROMWINEVECTOR3 aAngles)
{
	D3DXFROMWINEVECTOR2 size=aSize;
	D3DXFROMWINEVECTOR4 color;
	D3DXFROMWINEMATRIX  w;
	D3DXFROMWINEVECTOR3 vR, vP;

	vR=D3DXFROMWINEVECTOR3(0*90.0f+aAngles.x, 0.0f+aAngles.y, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f+aPosition.x, 0.0f+aPosition.y, -1.0f+aPosition.z);
	
	color=stx_GetRGBA(eGreen);
	vt.push_back(stx_Material("Orcs/Black.png", D3DXFROMWINEVECTOR3(color.x, color.y, color.z)));
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, color, size);

	vR=D3DXFROMWINEVECTOR3(1*90.0f+aAngles.x, 0.0f+aAngles.y, 0.0f);
	vP=D3DXFROMWINEVECTOR3(1.0f+aPosition.x, 0.0f+aPosition.y, 0.0f+aPosition.z);
	
	color=stx_GetRGBA(eRed);
	vt.push_back(stx_Material("Orcs/Black.png", D3DXFROMWINEVECTOR3(color.x, color.y, color.z)));
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, color, size);

	vR=D3DXFROMWINEVECTOR3(0*90.0f+aAngles.x, 0.0f+aAngles.y, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f+aPosition.x, 0.0f+aPosition.y, 1.0f+aPosition.z);
	
	color=stx_GetRGBA(eGreen);
	vt.push_back(stx_Material("Orcs/Black.png", D3DXFROMWINEVECTOR3(color.x, color.y, color.z)));
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, color, size);

	vR=D3DXFROMWINEVECTOR3(1*90.0f+aAngles.x, 0.0f+aAngles.y, 0.0f);
	vP=D3DXFROMWINEVECTOR3(-1.0f+aPosition.x, 0.0f+aPosition.y, 0.0f+aPosition.z);
	
	color=stx_GetRGBA(eRed);
	vt.push_back(stx_Material("Orcs/Black.png", D3DXFROMWINEVECTOR3(color.x, color.y, color.z)));
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, color, size);

	vR=D3DXFROMWINEVECTOR3(0.0f+aAngles.x, 1*90.0f+aAngles.y, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f+aPosition.x, 1.0f+aPosition.y, 0.0f+aPosition.z);
	
	color=stx_GetRGBA(eBlue);
	vt.push_back(stx_Material("Orcs/Black.png", D3DXFROMWINEVECTOR3(color.x, color.y, color.z)));
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, color, size);

	vR=D3DXFROMWINEVECTOR3(0.0f+aAngles.x, -1*90.0f+aAngles.y, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f+aPosition.x, -1.0f+aPosition.y, 0.0f+aPosition.z);
	
	color=stx_GetRGBA(eBlue);
	vt.push_back(stx_Material("Orcs/Black.png", D3DXFROMWINEVECTOR3(color.x, color.y, color.z)));
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, color, size);
}

int init(const char* aTitle)
{
	stx_Mesh m;
	mesh_.push_back(m);
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/texSHD3N.HLSL.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,     FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD,   FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texID=IRenderer::GetRendererInstance()->addImageLibTexture("/Orcs/Black.png", false, ss);

	D3DXFROMWINEVECTOR3 aPosition(0.0f, 0.0f, 0.0f);
	D3DXFROMWINEVECTOR2 aSize(1.0f, 1.0f);
	D3DXFROMWINEVECTOR3 aAngles(0.0f, 0.0f, 0.0f);
	drawCube(aPosition, aSize, aAngles);

	LOG_PRINT("g_vertices3.size() =%d\n", g_vertices3.size());
	LOG_PRINT("g_normals3.size()  =%d\n", g_normals3.size());
	LOG_PRINT("g_texcoords2.size()=%d\n", g_texcoords2.size());
	LOG_PRINT("g_indices1.size()  =%d\n", g_indices1.size());
	LOG_PRINT("g_indices2.size()  =%d\n", g_indices2.size());

	//stx_writeXFile("/ColoredCube.x", mesh_);//g_vertices3, g_normals3, g_texcoords2, g_indices1, texture);

	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY);

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	D3DXFROMWINEMATRIX R, S, W;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
	D3DXFROMWINEMatrixScaling(&S, 0.25f, 0.25f, 0.25f);
	W=R*S;
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
	IRenderer::GetRendererInstance()->setTexture("Sampler", texID);
	if(g_indices1.size())
		IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, g_vertices_.size(), g_indices1.size()/3, &g_indices1[0], &g_indices1[0], CONSTANT_INDEX2, &g_vertices_[0], &g_vertices_[0], sizeof(VertexC));
	else
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, g_vertices_.size()/3, &g_vertices_[0], &g_vertices_[0], sizeof(VertexC));

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
	r=IRenderer::GetRendererInstance("ColoredCube");
	in=STX_Service::GetInputInstance();
	init("");
	while (!in->OnKeyPress (STX_KEY_ESCAPE) && !in->Quit())
	{
		in->Update();
		render();
	}
	return 0;
}

