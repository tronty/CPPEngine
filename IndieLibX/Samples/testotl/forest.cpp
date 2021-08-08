/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#include <opentree/opentree.h>
#include "opentree/utils/otutils.h"
/*
#include <renderer/gamelet.h>
#include <renderer/mesh.h>
#include <renderer/tangentlist.h>
#include <renderer/texture.h>
#include <renderer/shader_cg.h>
*/
#include <renderer/vertices.h>
#include <renderer/vertexlist.h>
#include <renderer/indexlist.h>
#include <opentree/mesher/treemesher.h>
#include <opentree/mesher/leafmesher.h>
/*
#include <renderer/renderer.h>
#include <renderer/sdlframework.h>
*/
#include <helper/xmlparser.h>
#include <helper/xmlparserarbaro.h>
#include <opentree/weber/weber.h>
#include <time.h>
#include <fstream>

// #define BBC //BillBoard Cloud

#ifdef BBC
#include <opentree/mesher/bbcsimple.h>
#endif

#include <forest.h>
#include <loader.h>
#include <xmlparserforest.h>


		/*
		shdleaves=-1;
		shdnmap=-1;
		shdnormals=-1;
		shdparallax=-1;
		*/
ShaderID shd=-1;
VertexFormatID vf=-1;

int init(const char* aTitle)
{
  	Renderer* renderer=0;
  	Forest forest;
  	Loader loader(forest, renderer);
	xmlParserForest parser(&loader);
	std::string fn="/otldata/forest-xml/demoforest.xml";
    	std::string cfn=stx_convertpath(fn);
	parser.parse(cfn.c_str());
/*
  opentree::otArray<TreeObject*> trees;
  opentree::otArray<TreeFactory*> factories;
  opentree::otArray<Texture*> textures;
  opentree::otArray<TreeParams*> treeparams;
  opentree::otArray<VertexList*> vertexlist;
*/
    	for (unsigned int i=0; i < forest.trees.getCount(); i++)
    	{
		//LOG_PRINT("forest.tree[%d]:\n", i);
		LOG_PRINT("forest.tree[%d]=%s:\n", i, forest.trees[i]->getName());
	}
    	for (unsigned int i=0; i < forest.vertexlist.getCount(); i++)
    	{
		//LOG_PRINT("forest.vertexlist[%d]:\n", i);
		LOG_PRINT("forest.vertexlist[%d]=%s:\n", i, forest.vertexlist[i]->getName());
	}
    	for (unsigned int i=0; i < forest.textures.getCount(); i++)
    	{
		//LOG_PRINT("forest.texture[%d]:\n", i);
		LOG_PRINT("forest.texture[%d]=%s:\n", i, forest.textures[i]->getName());
	}

#if 0
  shdleaves = IRenderer::GetRendererInstance()->addShaderFromFile("/otldata/shaders/leaves.shd", "main", "main");
  shdnmap = IRenderer::GetRendererInstance()->addShaderFromFile("/otldata/shaders/nmap.shd", "main", "main");
  shdnormals = IRenderer::GetRendererInstance()->addShaderFromFile("/otldata/shaders/normals.shd", "main", "main");
  shdparallax = IRenderer::GetRendererInstance()->addShaderFromFile("/otldata/shaders/parallax.shd", "main", "main");
#elif 1
  shd = IRenderer::GetRendererInstance()->addShaderFromFile("/otldata/shaders/shd.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
#endif

	return 0;
}

void render()
{
	#if 1
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (0.3f, 0.1f, 0.0f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	
	D3DXFROMWINEMATRIX W,R,T,P;
#if 0
	D3DXFROMWINEMatrixIdentity(&W);
#else
	float mAngleX=90.0f;
	float mAngleY=0.0f;//z=180.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
	D3DXFROMWINEMatrixTranslation(&T, 0.0f,-10.0f,-20.0f);
	D3DXFROMWINEMatrixPerspectiveFovLH(&P, 60.0f, 1.5f, .25f, 1000.0f * .25f);
	W=R*T*P;
#endif
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);

	if(tv.trunk_triangle_.size())
	{
	IRenderer::GetRendererInstance()->setTexture("TexDiffuse", tv.stem_tex);
	IRenderer::GetRendererInstance()->setTexture("TexNormal", tv.stem_tex_n);
	IRenderer::GetRendererInstance()->setTexture("TexHeight", tv.stem_tex_h);
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, tv.vertices_.size(), tv.trunk_triangle_.size()/3, &tv.trunk_triangle_[0], &tv.trunk_triangle_[0], CONSTANT_INDEX2, &tv.vertices_[0], &tv.vertices_[0], sizeof(OTL_Vertex), RESET_VB|RESET_IB);
	}
	if(tv.branch_triangle_.size())
	{
	IRenderer::GetRendererInstance()->setTexture("TexDiffuse", tv.branch_tex);
	IRenderer::GetRendererInstance()->setTexture("TexNormal", tv.branch_tex_n);
	IRenderer::GetRendererInstance()->setTexture("TexHeight", tv.branch_tex_h);
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, tv.vertices_.size(), tv.branch_triangle_.size()/3, &tv.branch_triangle_[0], &tv.branch_triangle_[0], CONSTANT_INDEX2, &tv.vertices_[0], &tv.vertices_[0], sizeof(OTL_Vertex), RESET_VB|RESET_IB);
	}
	if(tv.leaves_triangle_.size())
	{
	IRenderer::GetRendererInstance()->setTexture("TexDiffuse", tv.leaves_tex);
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, tv.vertices_.size(), tv.leaves_triangle_.size()/3, &tv.leaves_triangle_[0], &tv.leaves_triangle_[0], CONSTANT_INDEX2, &tv.vertices_[0], &tv.vertices_[0], sizeof(OTL_Vertex), RESET_VB|RESET_IB);
	}
	if(tv.subbranch_triangle_.size())
	{
		IRenderer::GetRendererInstance()->setTexture("TexDiffuse", tv.branch_tex);
		IRenderer::GetRendererInstance()->setTexture("TexNormal", tv.branch_tex_n);
		IRenderer::GetRendererInstance()->setTexture("TexHeight", tv.branch_tex_h);
		IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, tv.vertices_.size(), tv.subbranch_triangle_.size()/3, &tv.subbranch_triangle_[0], &tv.subbranch_triangle_[0], CONSTANT_INDEX2, &tv.vertices_[0], &tv.vertices_[0], sizeof(OTL_Vertex), RESET_VB|RESET_IB);
	}
	D3DXFROMWINEVECTOR4 lightcolor(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXFROMWINEVECTOR4 LightPos;
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelViewI", I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelViewIT", I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelViewProj", I);
	IRenderer::GetRendererInstance()->setShaderConstant4f("lightcolor", lightcolor);
	IRenderer::GetRendererInstance()->setShaderConstant4f("LightPos", LightPos);

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
	#endif
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Forest v0.1");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

