#include <renderer/sdlframework.h>
#include "forestgamelet.h"

int ApplicationLogic()
{
  return SDL_FrameWork(new ForestGamelet()).run();
}

/*

../../../Libraries/otl/include/renderer/mesh.h:draw:203
vertexlist->getVertices()=ff59b010
vertexlist->getCount()=22594
indexlist->getTriangles()=9d6eca50
indexlist->getCount()=2
../../../Libraries/otl/src/renderer/renderer.cpp:drawMesh:321
g_program=3
g_vf=3
g_numvertices=0
num_triangles=2
triangles=9d6eca50
g_OTL_vertices=ff59b010
sizeof(OTL_Vertex)=44

*/

