/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <FW3.h>


#define defaultshader eDiffuseLighting
//define alternativeshader eSimpleShader
//define alternativeshader eAmbientLighting
#define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
//define alternativeshader ePhongLighting2
std::vector<stx_Mesh> mesh_;
class XFile : public STXGUI {
public:
	XFile(){}
	virtual ~XFile(){}
	MeshRenderer2 mesh;
	virtual void actionPerformed(GUIEvent &evt){}
	virtual void init(const char* aTitle="")
	{
	MeshRenderer2 shape3D;
	//shape3D.CreateCylinder(1.0f, 1.0f, 1.0f, defaultshader);
	//shape3D.CreateBox(1.0f, 1.0f, 1.0f, alternativeshader);
	shape3D.CreateTexturedBox(1.0f, 1.0f, 1.0f, defaultshader);

	std::vector<D3DXFROMWINEVECTOR3> vertices(shape3D.vertices.size());
	std::vector<D3DXFROMWINEVECTOR3> normals(shape3D.vertices.size());
	std::vector<D3DXFROMWINEVECTOR2> texcoords(shape3D.vertices.size());

	for(unsigned int i=0;i<shape3D.indices.size();i++)
	{
		vertices[shape3D.indices[i]]=shape3D.vertices[shape3D.indices[i]].Position;
		D3DXFROMWINEVECTOR3 n;
		D3DXFROMWINEVec3Normalize(&n, &vertices[shape3D.indices[i]]);
		normals[shape3D.indices[i]]=n;
		texcoords[shape3D.indices[i]]=shape3D.vertices[shape3D.indices[i]].Tex;
	}
	std::vector<stx_Material> vt;
	vt.push_back(stx_Material("test.bmp", D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f)));
	std::vector<__WORD__> inds1;
	std::vector<__WORD__> inds2;
	for(unsigned int i=0;i<shape3D.indices.size();i++)
		inds1.push_back(shape3D.indices[i]);
	for(unsigned int i=0;i<shape3D.indices.size()/3;i++)
		inds2.push_back(0);

	stx_Texture texture(vt, inds2);
	stx_writeXFile("/turtle.x", mesh_);//, vertices, normals, texcoords, inds1, texture);
	mesh.Load("/turtle.x", eSimpleShader, false, false);
	//stx_exit(0);
	}
	virtual void render(void)
	{
	IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);
	D3DXFROMWINEMATRIX R;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
	mesh.render(&R);
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present();
	}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("XFile");
	IInput*    i=STX_Service::GetInputInstance();
	XFile app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
		app.render();
	return 0;
}
