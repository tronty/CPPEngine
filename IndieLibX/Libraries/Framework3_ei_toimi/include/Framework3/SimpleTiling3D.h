#ifndef __SimpleTiling3D___
#define __SimpleTiling3D___
/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define USEMESH 1

#define defaultshader eSimpleShader
//define alternativeshader eAmbientLighting
#define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
//define alternativeshader ePhongLighting2
std::vector<stx_Mesh> mesh_;
struct SimpleTiling3D
{
	std::string m_sfiletosave;
	bool m_bUseColor;

	TextureID mortarjointTexID;
	TextureID redbrickTexID;
	TextureID texID;

	std::vector<TextureID> textures;
	std::vector<stx_Material> vt;

	MeshRenderer2 mesh;
	std::vector<D3DXFROMWINEVECTOR3> g_vertices3;
	std::vector<D3DXFROMWINEVECTOR3> g_normals3;
	std::vector<D3DXFROMWINEVECTOR2> g_texcoords2;
	std::vector<__WORD__> g_indices1;
	std::vector<__WORD__> g_indices2;
	D3DXFROMWINEVECTOR3 g_color3_;
	D3DXFROMWINEVECTOR4 g_color4_;

MeshRenderer2 meshCube3D;
MeshRenderer2 meshCantorDust3D;

void CubeToArrays(D3DXFROMWINEVECTOR3 aPosition, D3DXFROMWINEVECTOR3 aSize, D3DXFROMWINEVECTOR3 aAngle)
{
#if 0
	LOG_FNLN;
	LOG_PRINT("aPosition.x=%f\naPosition.y=%f\naPosition.z=%f\n", aPosition.x, aPosition.y, aPosition.z);
	LOG_PRINT("aSize.x=%f\naSize.y=%f\naSize.z=%f\n", aSize.x, aSize.y, aSize.z);
	LOG_PRINT("aAngle.x=%f\naAngle.y=%f\naAngle.z=%f\n", aAngle.x, aAngle.y, aAngle.z);
#endif
	D3DXFROMWINEMATRIX R, S, T, W;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, aAngle.x, aAngle.y, aAngle.z);
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, 0.0f, 0.0f, 0.0f);
	D3DXFROMWINEMatrixScaling(&S, aSize.x, aSize.y, aSize.z);
	D3DXFROMWINEMatrixTranslation(&T, aPosition.x, aPosition.y, aPosition.z);
	W=R*S*T;
	stx_VertexPositionNormalTexture v_;
	if(meshCube3D.indices.size())
	for(unsigned int j=0;j<meshCube3D.indices.size();j++)
	{
		stx_VertexPositionNormalTexture v=meshCube3D.vertices[meshCube3D.indices[j]];
		D3DXFROMWINEVec3TransformCoord(&v.Position, &v.Position, &W);
		meshCantorDust3D.vertices.push_back(v);
		meshCantorDust3D.indices.push_back(meshCantorDust3D.vertices.size()-1);
		g_vertices3.push_back(v.Position);
		g_normals3.push_back(v.Normal);
		g_texcoords2.push_back(v.Tex);
		g_indices1.push_back(meshCantorDust3D.vertices.size()-1);
		if(texID==mortarjointTexID)
			g_indices2.push_back(0);
		else if(texID==redbrickTexID)
			g_indices2.push_back(1);
		v_.Position=v.Position;
		v_.Normal=v.Normal;
		v_.Tex=v.Tex;
		mesh_[0].vertices.push_back(v_);
		mesh_[0].indices.push_back(meshCantorDust3D.vertices.size()-1);
	}
	else
	for(unsigned int j=0;j<meshCube3D.vertices.size();j++)
	{
		stx_VertexPositionNormalTexture v=meshCube3D.vertices[j];
		D3DXFROMWINEVec3TransformCoord(&v.Position, &v.Position, &W);
		meshCantorDust3D.vertices.push_back(v);
		g_vertices3.push_back(v.Position);
		g_normals3.push_back(v.Normal);
		g_texcoords2.push_back(v.Tex);
		v_.Position=v.Position;
		v_.Normal=v.Normal;
		v_.Tex=v.Tex;
		mesh_[0].vertices.push_back(v_);
		mesh_[0].indices.push_back(meshCantorDust3D.vertices.size()-1);
	}
}

	SimpleTiling3D(std::string afiletosave="", bool abUseColor=false)
	{
		stx_Mesh m;
		mesh_.push_back(m);
		m_bUseColor=abUseColor;

		mortarjointTexID=-1;
		redbrickTexID=-1;
		texID=-1;

		g_color3_=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f);
		g_color4_=D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

		init(afiletosave);
	}
	virtual ~SimpleTiling3D()
	{
		textures.clear();
		vt.clear();
		g_vertices3.clear();
		g_normals3.clear();
		g_texcoords2.clear();
		g_indices1.clear();
		g_indices2.clear();
	}

int init(std::string afiletosave)
{
	float awidth=1.0f;
	float aheight=1.0f;
	float adepth=1.0f;
	//tShader aShader=eShaderNone;
	meshCube3D.CreateTexturedBox(awidth, aheight, adepth, defaultshader);
	meshCantorDust3D.CreateTexturedBox(awidth, aheight, adepth, defaultshader);
	
	meshCantorDust3D.vertices.clear();
	meshCantorDust3D.indices.clear();

	m_sfiletosave=afiletosave;

	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	D3DXFROMWINEVECTOR3 vC(0.0f, 1.0f, 0.0f);

	mortarjointTexID=IRenderer::GetRendererInstance()->addImageLibTexture("/mortarjoint.png", false, ss); 
	{
		textures.push_back(mortarjointTexID);
		stx_Material m("mortarjoint.png", vC);
		vt.push_back(m);
	}
	redbrickTexID=IRenderer::GetRendererInstance()->addImageLibTexture("/redbrick.png", false, ss);
	{
		textures.push_back(redbrickTexID);
		stx_Material m("redbrick.png", vC);
		vt.push_back(m);
	}

	D3DXFROMWINEVECTOR3 aAngle(0.0f, 0.0f, 0.0f);
	for(unsigned int i=0;i<4;i++)
	{
	for(unsigned int j=0;j<5;j++)
	{
		texID=redbrickTexID;
		{
			D3DXFROMWINEVECTOR3 p(i*(1.0f+0.05f), j*(0.5f+0.05f), 0.0f);
			D3DXFROMWINEVECTOR3 s(1.0f, 0.5f, 1.0f);
			CubeToArrays(p, s, aAngle);

		}
		texID=mortarjointTexID;
		{
			D3DXFROMWINEVECTOR3 p(0.0f+i*1.05f, -0.05f+j*0.55f, 0.0f);
			D3DXFROMWINEVECTOR3 s(1.05f, 0.05f, 1.0f);
			CubeToArrays(p, s, aAngle);
		}
		{
			D3DXFROMWINEVECTOR3 p(-0.05f+i*1.05f, -0.05f+j*0.55f, 0.0f);
			D3DXFROMWINEVECTOR3 s(0.05f, 0.55f, 1.0f);
			CubeToArrays(p, s, aAngle);
		}
	}
	}

	LOG_PRINT("g_vertices3.size() =%d\n", g_vertices3.size());
	LOG_PRINT("g_normals3.size()  =%d\n", g_normals3.size());
	LOG_PRINT("g_texcoords2.size()=%d\n", g_texcoords2.size());
	LOG_PRINT("g_indices1.size()  =%d\n", g_indices1.size());
	LOG_PRINT("g_indices2.size()  =%d\n", g_indices2.size());
	#ifdef USEMESH
#if 0
	stx_Materials_(vt, g_indices2, mesh_);
	stx_writeXFile(afiletosave.c_str(), mesh_); // g_vertices3, g_normals3, g_texcoords2, g_indices1, texture);
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
	mesh_[0].indices=g_indices1;
	stx_Materials_(vt, g_indices2, mesh_);
	stx_writeXFile(afiletosave.c_str(), mesh_);
	}
#endif
	#if 0
	FW3::GetInstance()->GetMesh( mesh, afiletosave.c_str(), eSimpleShader);
	#else
	mesh.Load(afiletosave.c_str(), eSimpleShader);
	#endif
	#endif

	return 0;
}

void render()
{
	D3DXFROMWINEMATRIX W, R, S;
	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY);
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
	//D3DXFROMWINEMatrixScaling(&S, 0.5f, 0.5f, 0.5f);
	D3DXFROMWINEMatrixScaling(&S, 1.0f, 1.0f, 1.0f);
	W=R*S;
	mesh.render(&W);
}
};
#endif

