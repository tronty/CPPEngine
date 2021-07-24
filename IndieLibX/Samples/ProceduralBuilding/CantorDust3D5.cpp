/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

unsigned int Niterations=2;

TextureID texID=-1;
std::vector<stx_Material> vt;
std::vector<stx_Mesh> mesh_;

std::vector<D3DXFROMWINEVECTOR3> g_vertices3;
std::vector<D3DXFROMWINEVECTOR3> g_normals3;
std::vector<D3DXFROMWINEVECTOR2> g_texcoords2;
std::vector<__WORD__> g_indices1;
std::vector<__WORD__> g_indices2;

MeshRenderer2 g_mesh;

void CantorDust(D3DXFROMWINEVECTOR3 aPosition, D3DXFROMWINEVECTOR3 aSize, D3DXFROMWINEVECTOR3 aAngle, unsigned int NIter)
{

	if(NIter==1)
	{
#if 0
	LOG_FNLN;
	LOG_PRINT("NIter=%d\n", NIter);
	LOG_PRINT("aPosition.x=%f\naPosition.y=%f\naPosition.z=%f\n", aPosition.x, aPosition.y, aPosition.z);
	LOG_PRINT("aSize.x=%f\naSize.y=%f\naSize.z=%f\n", aSize.x, aSize.y, aSize.z);
	//LOG_PRINT("aAngle.x=%f\naAngle.y=%f\naAngle.z=%f\n", aAngle.x, aAngle.y, aAngle.z);
	//stx_exit(0);
#endif
		stx_AddTexturedCube(g_vertices3, g_normals3, g_texcoords2, g_indices1, g_indices2, 1, aPosition, aSize, aAngle);
		return;
	}

	 D3DXFROMWINEVECTOR3 size(aSize.x/3.0f,aSize.y/3.0f,aSize.z/3.0f);
	
	{D3DXFROMWINEVECTOR3 position(aPosition.x-size.x, aPosition.y+size.y, aPosition.z-size.z);
	 CantorDust(position, size, aAngle, NIter-1);}
	{D3DXFROMWINEVECTOR3 position(aPosition.x+size.x, aPosition.y+size.y, aPosition.z-size.z);
	 CantorDust(position, size, aAngle, NIter-1);}
	{D3DXFROMWINEVECTOR3 position(aPosition.x-size.x, aPosition.y+size.y, aPosition.z+size.z);
	 CantorDust(position, size, aAngle, NIter-1);}
	{D3DXFROMWINEVECTOR3 position(aPosition.x+size.x, aPosition.y+size.y, aPosition.z+size.z);
	 CantorDust(position, size, aAngle, NIter-1);}
	
	{D3DXFROMWINEVECTOR3 position(aPosition.x-size.x, aPosition.y-size.y, aPosition.z-size.z);
	 CantorDust(position, size, aAngle, NIter-1);}
	{D3DXFROMWINEVECTOR3 position(aPosition.x+size.x, aPosition.y-size.y, aPosition.z-size.z);
	 CantorDust(position, size, aAngle, NIter-1);}
	{D3DXFROMWINEVECTOR3 position(aPosition.x-size.x, aPosition.y-size.y, aPosition.z+size.z);
	 CantorDust(position, size, aAngle, NIter-1);}
	{D3DXFROMWINEVECTOR3 position(aPosition.x+size.x, aPosition.y-size.y, aPosition.z+size.z);
	 CantorDust(position, size, aAngle, NIter-1);}
}

int init(const char* aTitle)
{
#if 0
	LuaScript script;
	bool r=script.Load("/CantorDust3D.lua");
	if(!r){
		Niterations = int(script.get<float>("Niterations"));
		epsilon=script.get<float>("epsilon");
	}
#elif 0
	TiXmlDocument* document;
	TiXmlHandle* docHandle;
	TiXmlHandle* BodyHandle;
	std::string fn=stx_convertpath("/CantorDust3D.xml");
	LOG_PRINT("fn.c_str()=%s\n", fn.c_str());
	 		TiXmlDocument document(fn.c_str());
			document.LoadFile();
			TiXmlElement* BodyElement=TiXmlHandle(&document)->FirstChild("Body")->Element();
	if(BodyElement) {LOG_PRINT("!BodyElement\n");stx_exit(0);}

	Niterations = atoi(BodyHandle.FirstChild("Niterations").Element()->Attribute("Value"));
	epsilon = atof(BodyHandle.FirstChild("epsilon").Element()->Attribute("Value"));
#endif
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texID=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);
	vt.push_back(stx_Material("test.bmp", D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
	
	D3DXFROMWINEVECTOR3 aPosition(0.0f, 0.0f, 0.0f);
	D3DXFROMWINEVECTOR3 aSize(1.0f, 1.0f, 1.0f);
	D3DXFROMWINEVECTOR3 aAngle(0.0f, 0.0f, 0.0f);
	#if 1
	CantorDust(aPosition, aSize, aAngle, Niterations);
	#else
	D3DXFROMWINEVECTOR3 aAngle(0.0f, 0.0f, 0.0f);
	drawCube(aPosition, aSize, aAngle);
	#endif

	LOG_PRINT("g_vertices3.size() =%d\n", g_vertices3.size());
	LOG_PRINT("g_normals3.size()  =%d\n", g_normals3.size());
	LOG_PRINT("g_texcoords2.size()=%d\n", g_texcoords2.size());
	LOG_PRINT("g_indices1.size()  =%d\n", g_indices1.size());
	LOG_PRINT("g_indices2.size()  =%d\n", g_indices2.size());

	stx_Mesh m;
	mesh_.push_back(m);
	for(unsigned int i=0;i<g_vertices3.size();i++)
	{
		stx_VertexPositionNormalTexture v;
		v.Position=g_vertices3[i];
		v.Normal=g_normals3[i];
		v.Tex=g_texcoords2[i];
		mesh_[0].vertices.push_back(v);
	}
	mesh_[0].indices=g_indices1;
	//stx_Texture texture(vt, g_indices2);
	stx_Materials_(vt, g_indices2, mesh_);
	stx_writeXFile("/CantorDust3D5.x", mesh_);//, g_vertices3, g_normals3, g_texcoords2, g_indices1, texture);

#if 0
	FW3::GetInstance()->GetMesh(g_mesh, "/CantorDust3D5.x", eSimpleShader);
#else
	g_mesh.Load("/CantorDust3D5.x", eSimpleShader, true, true);
#endif
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	D3DXFROMWINEMATRIX w;
	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMatrixRotationYawPitchRoll( &w,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );

	g_mesh.render(&w);

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
	IRenderer* r=IRenderer::GetRendererInstance("CantorDust3D5");
	IInput* in=STX_Service::GetInputInstance();
	init("");
	while (!in->OnKeyPress (STX_KEY_ESCAPE) && !in->Quit())
	{
		in->Update();
		render();
	}
	return 0;
}

