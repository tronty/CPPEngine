/*
  Copyright (c) 2004 Pascal Kirchdorfer, Seth Yastrov

  This software is provided 'as-is', without any express or implied warranty. In
  no event will the authors be held liable for any damages arising from the use
  of this software.

  Permission is granted to anyone to use this software for any purpose, including
  commercial applications, and to alter it and redistribute it freely, subject to
  the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim
       that you wrote the original software. If you use this software in a product,
       an acknowledgment in the product documentation would be appreciated but is
       not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#include "treeview.h"

#include <renderer/renderer.h>
#include <renderer/sdlframework.h>

#include <helper/xmlparser.h>
#include <helper/xmlparserarbaro.h>
#include <opentree/weber/weber.h>
#include <time.h>
#include <fstream>

// #define BBC //BillBoard Cloud

#ifdef BBC
#include <opentree/mesher/bbcsimple.h>
#endif


int ApplicationLogic()
{
	//int argc=4;
	const char* argv_[] =
{
"treeview.bin",
"arbaro-xml",
"arbaro-weber",
"../../IndieLib_resources/otldata/arbaro-xml/cactus.xml"
};
#if 0
  if (argc != 4)
  {
    LOG_PRINT("Usage: treeview [otl-xml|arbaro-xml] [otl-weber|arbaro-weber] xmlfile\n");
    return 1;
  }

  std::ifstream test(argv[3], std::ios::in);
  if (!test)
  {
    LOG_PRINT("File %s does not exist or could not be read.  Exiting.\n", argv[3]);
    return -1;
  }
  test.close();
#endif

  opentree::setRandomSeed(4);
  
  return SDL_FrameWork(new TreeView(argv_[1], argv_[2], argv_[3]), "OTLTreeView").run();
}

TreeView::TreeView(const char* type, const char* algo, const char* treefile)
: tree(0), normals(false), light(true), vbo(false), bumpmap(false), leaves(false)
{
  opentree::TreeData params;

  if (!strcmp(type, "otl-xml"))
  {
    opentree::xmlParser parser;
    if (!parser.Load(params, treefile))
    {
      LOG_PRINT("OTL parser could not load %s\n", treefile);
      return;
    }
  }
  else if (!strcmp(type, "arbaro-xml"))
  {
    opentree::xmlParserArbaro parser;
    if (!parser.Load(params, treefile))
    {
      LOG_PRINT("Arbaro parser could not load %s\n", treefile);
      return;
    }
  }
  else
  {
    LOG_PRINT("Unknown xml format '%s'\n", type);
  }

  params.leafQuality = 0.6f;

  opentree::iWeber* gen = 0;
  
  if (!strcmp(algo, "otl-weber"))
  {
    gen = opentree::newWeber();
  }
  else if (!strcmp(algo, "arbaro-weber"))
  {
    gen = opentree::newWeberArbaro();
  }
  else
  {
    LOG_PRINT("Unknown algorithm '%s'\n", algo);
  }

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
  int vertexCount = 0;
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

  int trunk_triangleCount = 0;
  tree->getIndicesCount(0, &trunk_triangleCount);
  trunk_triangle.reserveTriangles(trunk_triangleCount);
  tree->getIndices(0, trunk_triangle, 0);

  int branch_triangleCount = 0;
  tree->getIndicesCount(1, &branch_triangleCount);
  branch_triangle.reserveTriangles(branch_triangleCount);
  tree->getIndices(1, branch_triangle, 0);

  int subbranch_triangleCount = 0;
  tree->getIndicesCount(2, &subbranch_triangleCount);
  subbranch_triangle.reserveTriangles(subbranch_triangleCount);
  tree->getIndices(2, subbranch_triangle, 0);

  int leaves_triangleCount = 0;
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

  Mesh mesh;
  mesh.setVertices(&vertices);
  mesh.setIndices(&trunk_triangle);
  mesh.setTangents(&trunk_tangents);
  mesh.createTangents();
  mesh.setIndices(&branch_triangle);
  mesh.createTangents();
  mesh.setIndices(&subbranch_triangle);
  mesh.createTangents();

  finish = clock();
  elapsed = ((double)(finish-start)) / CLOCKS_PER_SEC;
  LOG_PRINT("Elapsed time: %.3f seconds.\n", elapsed);
}

TreeView::~TreeView()
{
  delete tree;
}

void TreeView::init()
{
  bump_shader.init("../../IndieLib_resources/otldata/shaders/parallax_v.cg", "../../IndieLib_resources/otldata/shaders/parallax_f.cg");
  //shader.init("../../IndieLib_resources/otldata/shaders/nmap_v.cg", "../../IndieLib_resources/otldata/shaders/nmap_f.cg");
  normal_shader.init("../../IndieLib_resources/otldata/shaders/normals_v.cg", "../../IndieLib_resources/otldata/shaders/normals_f.cg");
  leaf_shader.init("../../IndieLib_resources/otldata/shaders/leaves_v.cg", "../../IndieLib_resources/otldata/shaders/leaves_f.cg");

  camera.grotXDegree(90);
  camera.grotZDegree(180);
  camera.translate(0,-5,-20);
  renderer->setPerspective(60.0f, 1.5f, .25f, 1000.0f * .25f);


  stem_tex = renderer->loadTexture("../../IndieLib_resources/otldata/textures/vertical_bark.png");
  stem_tex_n = renderer->loadTexture("../../IndieLib_resources/otldata/textures/vertical_bark_n.png");
  stem_tex_h = renderer->loadTexture("../../IndieLib_resources/otldata/textures/vertical_bark_h.png");

  branch_tex = renderer->loadTexture("../../IndieLib_resources/otldata/textures/bark.png");
  branch_tex_n = renderer->loadTexture("../../IndieLib_resources/otldata/textures/bark_n.png");
  branch_tex_h = renderer->loadTexture("../../IndieLib_resources/otldata/textures/bark_h.png");

#ifndef BBC
  leaves_tex = renderer->loadTexture("../../IndieLib_resources/otldata/textures/leaf_tria.png");
#else
  // TO BE DONE: This shouldn't be loaded from a texture but rendered to one.
  leaves_tex = renderer->loadTexture("../../IndieLib_resources/otldata/textures/leaves.png");
#endif

  /*
  stem_tex = renderer->loadTexture("../../IndieLib_resources/otldata/textures/box.png");
  branch_tex = renderer->loadTexture("../../IndieLib_resources/otldata/textures/box.png");
  normal_tex = renderer->loadTexture("../../IndieLib_resources/otldata/textures/boxn.png");
  height_tex = renderer->loadTexture("../../IndieLib_resources/otldata/textures/boxh.png");
  */

  renderer->setViewport(0,0,1024,768);

  //renderer->enableLight(false);
  //renderer->showNormals(true);

  vbo_data = renderer->createVBO(vertices.getVertices(), vertices.getCount());

  renderer->setupArray(vertices.getVertices(), vertices.getCount());//, trunk_tangents.getTangents());
#if 0
  LOG_FNLN;
  LOG_PRINT("vertices->getVertices()=%x\n", vertices.getVertices());
  LOG_PRINT("vertices->getCount()=%d\n", vertices.getCount());
#endif
  //stx_exit(0);
}

void TreeView::handleEvent(Events ev, int a, int b, int c)
{
  switch (ev)
  {
    case EV_MOUSE:
    {
      int x = a;
      int y = b;

      if (c & 1)
      {
        camera.lrotZDegree((float)x);
        camera.grotXDegree((float)y);
      }
      if (c & 2)
      {
        camera.translate((float)x*.05f,(float)-y*.05f,0);
      }
      if (c & 4)
      {
        camera.translate(0,0,((float)x+y)*.05f);
      }
      break;
    }
    case EV_KEY:
    {
      if (a == 27)
      {
        SDL_FrameWork::quit();
      }
      if (a == 'w')
      {
        renderer->showWireFrame(true);
        renderer->showTextures(false);
      }
      if (a == 't')
      {
        renderer->showWireFrame(false);
        renderer->showTextures(true);
      }
      if (a == 'l')
      {
        if (light)
          light = false;
        else
          light = true;
        renderer->enableLight(light);
      }
      if (a == 'n')
      {
        if (normals)
          normals = false;
        else
          normals = true;
      }
      if (a == 'b')
      {
        if (bumpmap)
          bumpmap = false;
        else
          bumpmap = true;
      }
      if (a == 'm')
      {
        if (leaves)
          leaves = false;
        else
          leaves = true;
      }
      if (a == 'v')
      {
        if (vbo)
        {
          vbo = false;
          renderer->setupArray(vertices.getVertices(), vertices.getCount());//, trunk_tangents.getTangents());
#if 0
  LOG_FNLN;
  LOG_PRINT("vertices->getVertices()=%x\n", vertices.getVertices());
  LOG_PRINT("vertices->getCount()=%d\n", vertices.getCount());
#endif
        }
        else
        {
          vbo = true;
          renderer->setupVBOArray(vbo_data);
        }
      }
      break;
    }
    case EV_QUIT:
    {
      // We quit now... who cares :)
      break;
    }
    default:
    {
      break;
    }
  }
}

void TreeView::draw()
{
  renderer->clear(0.3f, 0.1f, 0.0f, 0.0f);

  renderer->setCamera(camera);

  renderer->enableTransparency(false);
  renderer->enableAlphaTest(false);

  if (normals)
    normal_shader.preRender(renderer);

  if (bumpmap)
    bump_shader.preRender(renderer);

  renderer->setTexture(stem_tex, 0);
  renderer->setTexture(stem_tex_n, 1);
  renderer->setTexture(stem_tex_h, 2);
  renderer->drawMesh(trunk_triangle.getCount(), trunk_triangle.getTriangles());

  renderer->setTexture(branch_tex, 0);
  renderer->setTexture(branch_tex_n, 1);
  renderer->setTexture(branch_tex_h, 2);
  renderer->drawMesh(branch_triangle.getCount(), branch_triangle.getTriangles());

  //renderer->setTexture(branch_tex, 0);
  //renderer->setTexture(branch_tex_n, 1);
  //renderer->setTexture(branch_tex_h, 2);
  //renderer->drawMesh(subbranch_triangle.getCount(), subbranch_triangle.getTriangles());

  if (bumpmap)
    bump_shader.postRender();

  renderer->enableAlphaTest(true);
  renderer->enableTransparency(false);

  if (leaves)
    leaf_shader.preRender(renderer);

  //renderer->enablePointSprites(true);

  renderer->setTexture(leaves_tex, 0);
  renderer->drawMesh(leaves_triangle.getCount(), leaves_triangle.getTriangles());

  //renderer->enablePointSprites(false);

  if (leaves)
    leaf_shader.postRender();

  if (normals)
    normal_shader.postRender();

  renderer->blit();

  unsigned int triangles = trunk_triangle.getCount() + branch_triangle.getCount() 
                         + subbranch_triangle.getCount() + leaves_triangle.getCount();

  renderer->updateFPS("TreeView v0.1", vertices.getCount(), triangles);
  //stx_exit(0);
}
