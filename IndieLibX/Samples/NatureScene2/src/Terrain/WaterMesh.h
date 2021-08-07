/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef _WaterMesh_H_
#define _WaterMesh_H_

struct WaterMesh {
public:
	ShaderID shd;
	TextureID texID;
	VertexFormatID vf;
	std::vector<float> vertices;
	std::vector<__WORD__> indices;
	WaterMesh();
	virtual ~WaterMesh();
	virtual void init(const char* aTitle="/NatureScene/Water/watermesh.3ds", tShader aShader=eSimpleShader);
	virtual unsigned int render(D3DXFROMWINEMATRIX* mvp=0);
};
#endif

