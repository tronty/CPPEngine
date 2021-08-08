/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <STX/LuaUtil.h>
using namespace LuaUtil;

TextureID texID=0;

std::vector<stx_Texture> textures;
std::vector<stx_Material> vt;
std::vector<Vertex> g_vertices_;

std::vector<D3DXFROMWINEVECTOR3> g_vertices3;
std::vector<D3DXFROMWINEVECTOR3> g_normals3;
std::vector<D3DXFROMWINEVECTOR2> g_texcoords2;
std::vector<__WORD__> g_indices1;
std::vector<__WORD__> g_indices2;
D3DXFROMWINEVECTOR3 g_color3(1, 1, 1);

void DrawVertex(D3DXFROMWINEMATRIX& aW, D3DXFROMWINEVECTOR3& aC, D3DXFROMWINEVECTOR2& aTC, D3DXFROMWINEVECTOR3& aV_)
{
	//LOG_FNLN;
	D3DXFROMWINEVECTOR3 aV;
	D3DXFROMWINEVec3TransformCoord(&aV, &aV_, &aW);
	g_vertices3.push_back(aV);
	D3DXFROMWINEVECTOR3 N;
	D3DXFROMWINEVec3Normalize(&N, &aV);
	g_normals3.push_back(N);
	g_texcoords2.push_back(aTC);
	g_indices1.push_back(g_vertices3.size()-1);
	if(0==((g_indices1.size()%3)))
		g_indices2.push_back(texID);
	g_color3=aC;
	Vertex vx(g_vertices3[g_vertices3.size()-1], g_texcoords2[g_texcoords2.size()-1]);
	g_vertices_.push_back(vx);
}

int init(const char* aTitle)
{
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
#if 0
	LuaScript script;
	bool r=script.Load("/ProceduralBuilding/ProceduralBuilding.lua");
	std::string texturefilenames_ = script.get<std::string>("textures");
#else
	TiXmlDocument* document;
	TiXmlHandle* docHandle;
	TiXmlHandle* BodyHandle;
	std::string fn=stx_convertpath("/ProceduralBuilding/ProceduralBuilding.xml");
	LOG_PRINT("fn.c_str()=%s\n", fn.c_str());

	TiXmlDocument document(fn.c_str());
	document.LoadFile();
	TiXmlHandle docHandle( &document );
	TiXmlHandle BodyHandle = docHandle.FirstChild("Body");
	TiXmlElement* BodyElement = BodyHandle.Element();
	if(!BodyElement) {LOG_PRINT("!BodyElement\n");stx_exit(0);}

	std::string texturefilenames_ = BodyHandle.FirstChild("Textures").Element()->GetText();
#endif
	std::vector<std::string> vtex=stx_split(texturefilenames_, ':');
	D3DXFROMWINEVECTOR3 vC(0.0f, 1.0f, 0.0f);
	for(unsigned int i=0;i<vtex.size();i++)
	{
		//LOG_PRINT("vtex[%d]:%s\n", i, vtex[i].c_str());
		textures.push_back(IRenderer::GetRendererInstance()->addImageLibTexture(vtex[i].c_str(), false, ss));
		vt.push_back(stx_Material(vtex[i].c_str()+1, vC));
	}

	LOG_PRINT("g_vertices3.size()=%d\n", g_vertices3.size());
	LOG_PRINT("g_normals3.size()=%d\n", g_normals3.size());
	LOG_PRINT("g_texcoords2.size()=%d\n", g_texcoords2.size());
	LOG_PRINT("g_indices1.size()=%d\n", g_indices1.size());
	LOG_PRINT("g_indices2.size()=%d\n", g_indices2.size());

	#if 0
	stx_Texture texture(vt, g_indices2);
	stx_writeXFile("/TexturedCube.x", g_vertices3, g_normals3, g_texcoords2, g_indices1, texture);
	#endif
	return 0;
}

void draw()
{

	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY);

	IRenderer::GetRendererInstance()->InitTex3(IRenderer::GetRendererInstance()->GettexShader3(), IRenderer::GetRendererInstance()->GettexVF3());
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader3());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GettexVF3());
	D3DXFROMWINEMATRIX R, S, W;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
	D3DXFROMWINEMatrixScaling(&S, 0.25f, 0.25f, 0.25f);
	W=R*S;
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
	IRenderer::GetRendererInstance()->setTexture("Sampler", texID);
	if(g_indices1.size())
		IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, g_vertices_.size(), g_indices1.size()/3, &g_indices1[0], &g_indices1[0], sizeof(__WORD__), &g_vertices_[0], &g_vertices_[0], sizeof(Vertex));
	else
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, g_vertices_.size()/3, &g_vertices_[0], &g_vertices_[0], sizeof(Vertex));
}

