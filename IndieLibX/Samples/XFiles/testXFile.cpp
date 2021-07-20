/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

std::vector<D3DXFROMWINEVECTOR3> vertices3;
std::vector<D3DXFROMWINEVECTOR3> normals3;
std::vector<D3DXFROMWINEVECTOR2> texcoords2;
std::vector<stx_Mesh> mesh_;

int ApplicationLogic()
{
	stx_Mesh m;
	mesh_.push_back(m);
	stx_VertexPositionNormalTexture v_;
	vertices3.push_back(D3DXFROMWINEVECTOR3(-1.0f, -1.0f, 0.0f));
	D3DXFROMWINEVECTOR3 n;
	D3DXFROMWINEVec3Normalize(&n,&vertices3[vertices3.size()-1]);
	normals3.push_back(n);
	texcoords2.push_back(D3DXFROMWINEVECTOR2(0.0f, 0.0f));
	v_.Position=vertices3[vertices3.size()-1];
	v_.Tex=texcoords2[vertices3.size()-1];
	mesh_[0].vertices.push_back(v_);
	mesh_[0].indices.push_back(vertices3.size()-1);

	vertices3.push_back(D3DXFROMWINEVECTOR3(1.0f, -1.0f, 0.0f));
	D3DXFROMWINEVec3Normalize(&n,&vertices3[vertices3.size()-1]);
	normals3.push_back(n);
	texcoords2.push_back(D3DXFROMWINEVECTOR2(1.0f, 0.0f));
	v_.Position=vertices3[vertices3.size()-1];
	v_.Tex=texcoords2[vertices3.size()-1];
	mesh_[0].vertices.push_back(v_);
	mesh_[0].indices.push_back(vertices3.size()-1);

	vertices3.push_back(D3DXFROMWINEVECTOR3(-1.0f, 1.0f, 0.0f));
	D3DXFROMWINEVec3Normalize(&n,&vertices3[vertices3.size()-1]);
	normals3.push_back(n);
	texcoords2.push_back(D3DXFROMWINEVECTOR2(0.0f, 1.0f));
	v_.Position=vertices3[vertices3.size()-1];
	v_.Tex=texcoords2[vertices3.size()-1];
	mesh_[0].vertices.push_back(v_);
	mesh_[0].indices.push_back(vertices3.size()-1);

	vertices3.push_back(D3DXFROMWINEVECTOR3(-1.0f, 1.0f, 0.0f));
	D3DXFROMWINEVec3Normalize(&n,&vertices3[vertices3.size()-1]);
	normals3.push_back(n);
	texcoords2.push_back(D3DXFROMWINEVECTOR2(0.0f, 1.0f));
	v_.Position=vertices3[vertices3.size()-1];
	v_.Tex=texcoords2[vertices3.size()-1];
	mesh_[0].vertices.push_back(v_);
	mesh_[0].indices.push_back(vertices3.size()-1);

	vertices3.push_back(D3DXFROMWINEVECTOR3(1.0f, 1.0f, 0.0f));
	D3DXFROMWINEVec3Normalize(&n,&vertices3[vertices3.size()-1]);
	normals3.push_back(n);
	texcoords2.push_back(D3DXFROMWINEVECTOR2(1.0f, 1.0f));
	v_.Position=vertices3[vertices3.size()-1];
	v_.Tex=texcoords2[vertices3.size()-1];
	mesh_[0].vertices.push_back(v_);
	mesh_[0].indices.push_back(vertices3.size()-1);

	vertices3.push_back(D3DXFROMWINEVECTOR3(1.0f, -1.0f, 0.0f));
	D3DXFROMWINEVec3Normalize(&n,&vertices3[vertices3.size()-1]);
	normals3.push_back(n);
	texcoords2.push_back(D3DXFROMWINEVECTOR2(1.0f, 0.0f));
	v_.Position=vertices3[vertices3.size()-1];
	v_.Tex=texcoords2[vertices3.size()-1];
	mesh_[0].vertices.push_back(v_);
	mesh_[0].indices.push_back(vertices3.size()-1);

	stx_writeXFile("/triangle.x", mesh_);//, vertices3, normals3, texcoords2, inds1, texture);
	return 0;
}

