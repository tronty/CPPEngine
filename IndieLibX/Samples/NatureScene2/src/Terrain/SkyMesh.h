#ifndef _SkyMesh_H_
#define _SkyMesh_H_
/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
struct SkyMesh {
public:
	SkyMesh();
	virtual ~SkyMesh();
	MeshRenderer2 shape3D;
    	TextureID skyTexture;
	ShaderID skyShader;
    	VertexFormatID skyVF;
	virtual void init(tShader aShader=eShaderNone);
	virtual unsigned int  render(D3DXFROMWINEMATRIX* mvp=0);
};
#endif

