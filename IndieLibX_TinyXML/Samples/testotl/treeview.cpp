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

#undef USEMESH

const char* filename[] =
{
"/otldata/forest-xml/demoforest.xml",
"/otldata/arbaro-xml/cactus.xml",
"/otldata/arbaro-xml/barley.xml",
"/otldata/arbaro-xml/fern.xml",
"/otldata/arbaro-xml/quaking_aspen.xml",
"/otldata/arbaro-xml/palm.xml",
"/otldata/arbaro-xml/tamarack.xml",
"/otldata/arbaro-xml/rush.xml",
"/otldata/arbaro-xml/wheat.xml",
"/otldata/arbaro-xml/desert_bush.xml",
"/otldata/arbaro-xml/black_tupelo.xml",
"/otldata/arbaro-xml/shave-grass.xml",
"/otldata/arbaro-xml/european_larch.xml",
"/otldata/arbaro-xml/sassafras.xml",
"/otldata/arbaro-xml/fir.xml",
"/otldata/arbaro-xml/ca_black_oak.xml",
"/otldata/arbaro-xml/asparagus.xml",
"/otldata/arbaro-xml/lombardy_poplar.xml",
"/otldata/arbaro-xml/weeping_willow.xml",
"/otldata/arbaro-xml/fanpalm.xml",
"/otldata/arbaro-xml/eastern_cottonwood.xml",
"/otldata/test-xml/webertest.xml",
"/otldata/weber-xml/quaking_aspen.xml",
"/otldata/weber-xml/brancaldra.xml",
"/otldata/weber-xml/black_tupelo.xml",
"/otldata/weber-xml/ca_black_oak.xml",
"/otldata/weber-xml/weeping_willow.xml",
};

	std::vector<D3DXFROMWINEVECTOR3> g_vertices3;
	std::vector<D3DXFROMWINEVECTOR3> g_normals3;
	std::vector<D3DXFROMWINEVECTOR2> g_texcoords2;
	std::vector<__WORD__> g_indices1;
	std::vector<__WORD__> g_indices2;

	MeshRenderer2 mesh;
std::vector<stx_Mesh> mesh_;
#if 0
  class OTL_Vertex
  {
  public:
    ///position
    float x, y, z;
    ///normal
    float nx, ny, nz;
    ///color
    float r, g, b;
    ///texture
    float u, v;
  };
#endif

struct TreeView
{
  opentree::MesherTree* tree;

  VertexList vertices;

  IndexList trunk_triangle;
  IndexList branch_triangle;
  IndexList subbranch_triangle;
  IndexList leaves_triangle;

  //TangentList trunk_tangents;
	//const unsigned int treefileID=0;
	const unsigned int treefileID=1;
	//const unsigned int treefileID=22;
	D3DXFROMWINEMATRIX W;
	ShaderID shd;//shdleaves, shdnmap, shdnormals, shdparallax;
	VertexFormatID vf;
	TextureID stem_tex;
	TextureID stem_tex_n;
	TextureID stem_tex_h;
	TextureID branch_tex;
	TextureID branch_tex_n;
	TextureID branch_tex_h, leaves_tex;
	std::vector<stx_Material> vt;

	unsigned int offset_s, offset_b, offset_sb;
	int vertexCount, trunk_triangleCount, branch_triangleCount, subbranch_triangleCount, leaves_triangleCount;

	std::vector<OTL_Vertex> vertices_;
	std::vector<__WORD__> trunk_triangle_;
	std::vector<__WORD__> branch_triangle_;
	std::vector<__WORD__> subbranch_triangle_;
	std::vector<__WORD__> leaves_triangle_;
	TreeView(){}
	void init(const char* afilename=filename[1]){
		stx_Mesh m;
		mesh_.push_back(m);
		/*
		shdleaves=-1;
		shdnmap=-1;
		shdnormals=-1;
		shdparallax=-1;
		*/
		vf=-1;
		stem_tex=-1;
		stem_tex_n=-1;
		stem_tex_h=-1;
		branch_tex=-1;
		branch_tex_n=-1;
		branch_tex_h=-1;
		offset_s=0;
		offset_b = 0;
		offset_sb = 0;

  opentree::setRandomSeed(4);
  opentree::TreeData params;

  opentree::iWeber* gen = 0;

  if(1)
  {
    opentree::xmlParser parser;
    std::string fn=stx_convertpath(afilename);
  if (!parser.Load(params, fn.c_str()))
    {
      LOG_PRINT("OTL parser could not load %s\n", fn.c_str());
      return;
    }
    gen = opentree::newWeber();
  }
  else
  {
    opentree::xmlParserArbaro parser;
    std::string fn=stx_convertpath(filename[1]);
    if (!parser.Load(params, fn.c_str()))
    {
      LOG_PRINT("Arbaro parser could not load %s\n", fn.c_str());
      return;
    }
    gen = opentree::newWeberArbaro();
  }

  params.leafQuality = 0.6f;
  gen->setParams(params);

  LOG_PRINT("Generating Tree...\n");
  clock_t start = clock(); 

  opentree::otTree* ottree = gen->generate();
  clock_t finish = clock();

  double elapsed = ((double)(finish-start)) / CLOCKS_PER_SEC;
  LOG_PRINT("Elapsed time: %.3f seconds.\n", elapsed);

  delete gen;

  start = clock(); 
  tree = new opentree::MesherTree(ottree);
  finish = clock();

  elapsed = ((double)(finish-start)) / CLOCKS_PER_SEC;
  LOG_PRINT("Elapsed time: %.3f seconds.\n", elapsed);

  // Set curveRes and circleRes for all stems
  for (unsigned int i = 0; i < 4; i++)
  {
    tree->setCurveRes(i,6);
    tree->setCircleRes(i,7);
  }

  tree->useQuadLeaves(); //TriangleLeaves();

  LOG_PRINT("Done!\n");

  LOG_PRINT("Generating Mesh...\n");
  start = clock(); 

  unsigned int offset_s = 0, offset_b = 0, offset_sb = 0;

#ifdef BBC
  opentree::BBCSimple bbc(ottree, 3);
#endif

  //Get the amount of vertices used with the current LoD.
  vertexCount = 0;
  tree->getVerticesCount(0, &vertexCount);
  offset_s = vertexCount;
  tree->getVerticesCount(1, &vertexCount);
  offset_b = vertexCount;
  tree->getVerticesCount(2, &vertexCount);
  offset_sb = vertexCount;
#ifndef BBC
  tree->getLeavesVerticesCount(&vertexCount);
#else
  vertexCount += bbc.getBillboardCount() * 4;
#endif

  vertices.reserveVertices(vertexCount);

  tree->getVertices(0, vertices);
  tree->getVertices(1, vertices);
  tree->getVertices(2, vertices);
#ifndef BBC
  tree->getLeavesVertices(vertices);
#else
  for (unsigned int i = 0; i < bbc.getBillboardCount(); i++)
  {
    bbc.getBillboard(i)->getVertices(vertices);
  }
#endif

  trunk_triangleCount = 0;
  tree->getIndicesCount(0, &trunk_triangleCount);
  trunk_triangle.reserveTriangles(trunk_triangleCount);
  tree->getIndices(0, trunk_triangle, 0);

  branch_triangleCount = 0;
  tree->getIndicesCount(1, &branch_triangleCount);
  branch_triangle.reserveTriangles(branch_triangleCount);
  tree->getIndices(1, branch_triangle, 0);

  subbranch_triangleCount = 0;
  tree->getIndicesCount(2, &subbranch_triangleCount);
  subbranch_triangle.reserveTriangles(subbranch_triangleCount);
  tree->getIndices(2, subbranch_triangle, 0);

  leaves_triangleCount = 0;
#ifndef BBC
  tree->getLeavesIndicesCount(&leaves_triangleCount);
#else
  leaves_triangleCount += bbc.getBillboardCount() * 2;
#endif
  leaves_triangle.reserveTriangles(leaves_triangleCount);
#ifndef BBC
  tree->getLeavesIndices(leaves_triangle, offset_sb);
#else
  for (unsigned int i = 0; i < bbc.getBillboardCount(); i++)
  {
    bbc.getBillboard(i)->getIndices(leaves_triangle, offset_sb +  i * 4);
  }
#endif

#if 0
  Mesh mesh;
  mesh.setVertices(&vertices);
  mesh.setIndices(&trunk_triangle);
  mesh.setTangents(&trunk_tangents);
  mesh.createTangents();
  mesh.setIndices(&branch_triangle);
  mesh.createTangents();
  mesh.setIndices(&subbranch_triangle);
  mesh.createTangents();
#endif

  const OTL_Vertex* vertices__=  vertices.getVertices();
  for(unsigned int i=0;i<vertexCount;i++)
  {
	vertices_.push_back(vertices__[i]);
	g_vertices3.push_back(D3DXFROMWINEVECTOR3(vertices_[i].x, vertices_[i].y, vertices_[i].z));
	D3DXFROMWINEVECTOR3 N;
	D3DXFROMWINEVec3Normalize(&N, &g_vertices3[g_vertices3.size()-1]);
	g_normals3.push_back(N);
	g_texcoords2.push_back(D3DXFROMWINEVECTOR2(vertices_[i].u, vertices_[i].v));

	stx_VertexPositionNormalTexture v_;
	v_.Position=g_vertices3[g_vertices3.size()-1];
	v_.Normal=g_normals3[g_vertices3.size()-1];
	v_.Tex=g_texcoords2[g_vertices3.size()-1];
	mesh_[0].vertices.push_back(v_);
	mesh_[0].indices.push_back(g_vertices3.size()-1);
  }

	D3DXFROMWINEVECTOR3 aiVecs[2] = {D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f),
		D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f) };

for( unsigned int a = 0; a < vertexCount;++a)
		{
			D3DXFROMWINEVECTOR3 pc =g_vertices3[a];
			D3DXFROMWINEVECTOR3 pc1=pc;
			
			//D3DXFROMWINEVec3TransformCoord((D3DXFROMWINEVECTOR3*)&pc1,(D3DXFROMWINEVECTOR3*)&pc, (D3DXFROMWINEMATRIX*)&aiMe);

			aiVecs[0].x = std::min( aiVecs[0].x, pc1.x);
			aiVecs[0].y = std::min( aiVecs[0].y, pc1.y);
			aiVecs[0].z = std::min( aiVecs[0].z, pc1.z);
			aiVecs[1].x = std::max( aiVecs[1].x, pc1.x);
			aiVecs[1].y = std::max( aiVecs[1].y, pc1.y);
			aiVecs[1].z = std::max( aiVecs[1].z, pc1.z);
		}

	D3DXFROMWINEVECTOR3 vDelta = aiVecs[1]-aiVecs[0];
	D3DXFROMWINEVECTOR3 vHalf =  aiVecs[0] + (vDelta / 2.0f);
	float fScale = 2.0f / D3DXFROMWINEVec3Length(&vDelta);

	D3DXFROMWINEMATRIX m_mWorld =  D3DXFROMWINEMATRIX(
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		-vHalf.x,-vHalf.y,-vHalf.z,1.0f) *
		D3DXFROMWINEMATRIX(
		fScale,0.0f,0.0f,0.0f,
		0.0f,fScale,0.0f,0.0f,
		0.0f,0.0f,fScale,0.0f,
		0.0f,0.0f,0.0f,1.0f);

  for(unsigned int i=0;i<vertexCount;i++)
	D3DXFROMWINEVec3TransformCoord(&g_vertices3[g_vertices3.size()-1], &g_vertices3[g_vertices3.size()-1], &m_mWorld);

  const Triangle* trunk_triangle__=trunk_triangle.getTriangles();
  for(unsigned int i=0;i<trunk_triangleCount;i++)
  {
	trunk_triangle_.push_back(trunk_triangle__[i].v1);g_indices1.push_back(trunk_triangle_.size()-1);
	trunk_triangle_.push_back(trunk_triangle__[i].v2);g_indices1.push_back(trunk_triangle_.size()-1);
	trunk_triangle_.push_back(trunk_triangle__[i].v3);g_indices1.push_back(trunk_triangle_.size()-1);
	g_indices2.push_back(0);
  }
  const Triangle* branch_triangle__=branch_triangle.getTriangles();
  for(unsigned int i=0;i<branch_triangleCount;i++)
  {
	branch_triangle_.push_back(branch_triangle__[i].v1);g_indices1.push_back(trunk_triangle_.size()-1);
	branch_triangle_.push_back(branch_triangle__[i].v2);g_indices1.push_back(trunk_triangle_.size()-1);
	branch_triangle_.push_back(branch_triangle__[i].v3);g_indices1.push_back(trunk_triangle_.size()-1);
	g_indices2.push_back(1);
  }
  const Triangle* subbranch_triangle__=subbranch_triangle.getTriangles();
  for(unsigned int i=0;i<subbranch_triangleCount;i++)
  {
	subbranch_triangle_.push_back(subbranch_triangle__[i].v1);g_indices1.push_back(trunk_triangle_.size()-1);
	subbranch_triangle_.push_back(subbranch_triangle__[i].v2);g_indices1.push_back(trunk_triangle_.size()-1);
	subbranch_triangle_.push_back(subbranch_triangle__[i].v3);g_indices1.push_back(trunk_triangle_.size()-1);
	g_indices2.push_back(1);
  }
  const Triangle* leaves_triangle__=leaves_triangle.getTriangles();
  for(unsigned int i=0;i<leaves_triangleCount;i++)
  {
	leaves_triangle_.push_back(leaves_triangle__[i].v1);g_indices1.push_back(trunk_triangle_.size()-1);
	leaves_triangle_.push_back(leaves_triangle__[i].v2);g_indices1.push_back(trunk_triangle_.size()-1);
	leaves_triangle_.push_back(leaves_triangle__[i].v3);g_indices1.push_back(trunk_triangle_.size()-1);
	g_indices2.push_back(2);
  }

  finish = clock();
  elapsed = ((double)(finish-start)) / CLOCKS_PER_SEC;
  LOG_PRINT("Elapsed time: %.3f seconds.\n", elapsed);

#if 0
  bump_shader.init("data/shaders/parallax_v.cg", "data/shaders/parallax_f.cg");
  //shader.init("data/shaders/nmap_v.cg", "data/shaders/nmap_f.cg");
  normal_shader.init("data/shaders/normals_v.cg", "data/shaders/normals_f.cg");
  leaf_shader.init("data/shaders/leaves_v.cg", "data/shaders/leaves_f.cg");

  camera.grotXDegree(90);
  camera.grotZDegree(180);
  camera.translate(0,-5,-20);
  renderer->setPerspective(60.0f, 1.5f, .25f, 1000.0f * .25f);
#elif 0
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
	
	D3DXFROMWINEVECTOR3 vC(0.0f, 1.0f, 0.0f);
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
  stem_tex = IRenderer::GetRendererInstance()->addImageLibTexture("/otldata/textures/vertical_bark.jpg", false, ss);
   {stx_Material m("textures/vertical_bark.jpg", vC);vt.push_back(m);}
  stem_tex_n = IRenderer::GetRendererInstance()->addImageLibTexture("/otldata/textures/vertical_bark_n.jpg", false, ss);
  stem_tex_h = IRenderer::GetRendererInstance()->addImageLibTexture("/otldata/textures/vertical_bark_h.jpg", false, ss);

  branch_tex = IRenderer::GetRendererInstance()->addImageLibTexture("/otldata/textures/bark.jpg", false, ss);
   {stx_Material m("textures/bark.jpg", vC);vt.push_back(m);}
  branch_tex_n = IRenderer::GetRendererInstance()->addImageLibTexture("/otldata/textures/bark_n.jpg", false, ss);
  branch_tex_h = IRenderer::GetRendererInstance()->addImageLibTexture("/otldata/textures/bark_h.jpg", false, ss);

#ifndef BBC
  leaves_tex = IRenderer::GetRendererInstance()->addImageLibTexture("/otldata/textures/leaf_tria.png", false, ss);
   {stx_Material m("textures/leaf_tria.png", vC);vt.push_back(m);}
#else
  // TO BE DONE: This shouldn't be loaded from a texture but rendered to one.
  leaves_tex = IRenderer::GetRendererInstance()->addImageLibTexture("/otldata/textures/leaves.png", false, ss);
   {stx_Material m("textures/leaves.png", vC);vt.push_back(m);}
#endif

  //renderer->setViewport(0,0,1024,768);

  LOG_PRINT("vertices_.size()=%d\n", vertices_.size());
  LOG_PRINT("trunk_.size()=%d\n", trunk_triangle_.size());
  LOG_PRINT("branch_.size()=%d\n", branch_triangle_.size());
  LOG_PRINT("subbranch_.size()=%d\n", subbranch_triangle_.size());
  LOG_PRINT("leaves_.size()=%d\n", leaves_triangle_.size());
#ifdef USEMESH
  LOG_PRINT("g_vertices3.size()=%d\n", g_vertices3.size());
  LOG_PRINT("g_normals3.size()=%d\n", g_normals3.size());
  LOG_PRINT("g_texcoords2.size()=%d\n", g_texcoords2.size());
  LOG_PRINT("g_indices1.size()=%d\n", g_indices1.size());
  LOG_PRINT("g_indices2.size()=%d\n", g_indices2.size());

  stx_Texture texture(vt, g_indices2);
  stx_writeXFile("/otldata/tree.x", mesh_);//, g_vertices3, g_normals3, g_texcoords2, g_indices1, texture);
  mesh.Load("/otldata/tree.x", eSimpleShader, false);
#endif
}

virtual ~TreeView()
{
  delete tree;
}
};

TreeView tv;
int init(const char* aTitle)
{
	tv.init();
	return 0;
}

void render()
{
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (0.3f, 0.1f, 0.0f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
#ifdef USEMESH
	D3DXFROMWINEMATRIX W, R, S;
	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY);
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
	//D3DXFROMWINEMatrixScaling(&S, 0.5f, 0.5f, 0.5f);
	D3DXFROMWINEMatrixScaling(&S, 1.0f, 1.0f, 1.0f);
	W=R*S;
	mesh.render(W);
#else
	IRenderer::GetRendererInstance()->setShader(tv.shd);
	IRenderer::GetRendererInstance()->setVertexFormat(tv.vf);
	
	D3DXFROMWINEMATRIX W,R,S,T,P;
#if 1
	D3DXFROMWINEMatrixIdentity(&W);
#else
	float mAngleX=90.0f;
	float mAngleY=0.0f;//z=180.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
	D3DXFROMWINEMatrixTranslation(&T, 0.0f,-5.0f,-20.0f);
	D3DXFROMWINEMatrixTranslation(&T, 0.0f,-5.0f,-20.0f);
	D3DXFROMWINEMatrixPerspectiveFovLH(&P, 60.0f, 1.5f, .25f, 1000.0f * .25f);

	D3DXFROMWINEVECTOR3 aiVecs[2] = {	D3DXFROMWINEVECTOR3(  1e10f,  1e10f,  1e10f),
						D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f) };
	for(unsigned int i=0;i<tv.vertices_.size();i++)
	{
			OTL_Vertex pc1=tv.vertices_[i];
			aiVecs[0].x = std::min( aiVecs[0].x, pc1.x);
			aiVecs[0].y = std::min( aiVecs[0].y, pc1.y);
			aiVecs[0].z = std::min( aiVecs[0].z, pc1.z);
			aiVecs[1].x = std::max( aiVecs[1].x, pc1.x);
			aiVecs[1].y = std::max( aiVecs[1].y, pc1.y);
			aiVecs[1].z = std::max( aiVecs[1].z, pc1.z);
	}
	D3DXFROMWINEVECTOR3 vDelta = aiVecs[1]-aiVecs[0];
	D3DXFROMWINEVECTOR3 vHalf =  aiVecs[0] + (vDelta / 2.0f);
	float fScale = 10.0f / vDelta.getLength();

	W =  D3DXFROMWINEMATRIX(
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		-vHalf.x,-vHalf.y,-vHalf.z,1.0f) *
		D3DXFROMWINEMATRIX(
		fScale,0.0f,0.0f,0.0f,
		0.0f,fScale,0.0f,0.0f,
		0.0f,0.0f,fScale,0.0f,
		0.0f,0.0f,0.0f,1.0f);
	D3DXFROMWINEMatrixScaling(&S, fScale, fScale, fScale); 
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

	/*
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelViewI", );
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelViewIT", );
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelViewProj", );
	IRenderer::GetRendererInstance()->setShaderConstant4f("lightcolor", );
	IRenderer::GetRendererInstance()->setShaderConstant4f("LightPos", );
	*/
#endif
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("TreeView v0.1");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

