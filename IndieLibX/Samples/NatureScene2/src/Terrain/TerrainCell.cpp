#include "TerrainUtils.h"
#include "../SceneFrame.h"

unsigned int   TerrainCell::userCount      =     0;
std::vector<__WORD__> TerrainCell::indices;

TerrainCell::TerrainCell()
{
  verticalBounds.x  =  10000.0f;
  verticalBounds.y  = -10000.0f;
  userCount        +=         1;
  vertices.resize(TILE_COUNT);
  indices.resize(INDEX_COUNT);
}

TerrainCell::~TerrainCell()
{
  vertices.clear();
  indices.clear();
}

bool  TerrainCell::setup(const HeightMap& heightmap,
                         const D3DXFROMWINEVECTOR2&   start)
{
  const HMVertex *HMvertices     =  heightmap.getVertexStream();
  float           maxHeight    = -20000.0f,
                  minHeight    =  20000.0f,
                  newHeight    =      0.0f;
  int             height       = heightmap.getHeight(),
                  width        = heightmap.getWidth(),
                  offset       = 0,
                  index        = 0,
                  x            = 0,
                  y            = 0;

  for(y = start.y; y < start.y + TILE_ROW_COUNT; y++)
  {
    y = clamp(y, 0, height - 1);
    for(x = start.x; x < start.x + TILE_COLUMN_COUNT; x++, offset++)
    {
      x          = clamp(x, 0, width - 1);
      index      = y*width + x;
      newHeight  = HMvertices[index].vertex.y;
      maxHeight  = newHeight > maxHeight ? newHeight : maxHeight;
      minHeight  = newHeight < minHeight ? newHeight : minHeight;

      vertices[offset].vertex = HMvertices[index].vertex;
      vertices[offset].normal = HMvertices[index].normal;
      vertices[offset].uv     = HMvertices[index].uv;
    }
  }

  verticalBounds.set(minHeight, maxHeight);
	//LOG_FNLN;
	//LOG_PRINT("vertices.size()=%d\n", vertices.size());
  D3DXFROMWINEVECTOR3 v3( -1e10f, -1e10f, -1e10f);
  if(1)
  for(unsigned int i=0;i<vertices.size();i++)
  {
  	v3.x = std::max( vertices[i].vertex.x, v3.x);
  	v3.y = std::max( vertices[i].vertex.y, v3.y);
  	v3.z = std::max( vertices[i].vertex.z, v3.z);
  }

  //LOG_PRINT("v3.x=%f\n", v3.x);
  //LOG_PRINT("v3.y=%f\n", v3.y);
  //LOG_PRINT("v3.z=%f\n", v3.z);
 
  return true;
} 

const bool TerrainCell::render()
{
	#if 0
	//LOG_FNLN;
	//LOG_PRINT("vertices.size()=%d\n", vertices.size());
	//LOG_PRINT("indices .size()=%d\n", indices.size());
	#endif
	//D3DXFROMWINEMATRIX MVP=g_SceneFrame->camera.GetMVP();
	#if 0
      IRenderer::GetRendererInstance()->setShader(g_SceneFrame->terrain.terrainShader);
      IRenderer::GetRendererInstance()->setVertexFormat(g_SceneFrame->terrain.terrainVF);
	if(0)
	{
		D3DXFROMWINEMATRIX I;
		D3DXFROMWINEMatrixIdentity(&I);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	}
	else
	{
  		D3DXFROMWINEMATRIX mvp=g_SceneFrame->camera.getModelViewMatrix()*g_SceneFrame->camera.getProjectionMatrix();
  		if(g_SceneFrame->terrain.terrainInvert) mvp=g_SceneFrame->camera.getProjectionMatrix()*g_SceneFrame->camera.getModelViewMatrix();
  		if(g_SceneFrame->terrain.terrainTranspose) D3DXFROMWINEMatrixTranspose(&mvp, &mvp);
      		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", mvp);
	}
      IRenderer::GetRendererInstance()->setTexture("weight", g_SceneFrame->terrain.weight);
      IRenderer::GetRendererInstance()->setTexture("fungus", g_SceneFrame->terrain.fungus);
      IRenderer::GetRendererInstance()->setTexture("dirt", g_SceneFrame->terrain.dirt);
      IRenderer::GetRendererInstance()->setTexture("grass", g_SceneFrame->terrain.grass);
	#endif

	#if 0
	stx_Dump1i(g_SceneFrame->terrain.terrainShader, "terrainShader");
	stx_Dump1i(g_SceneFrame->terrain.terrainVF, "terrainVF");
	stx_Dump4x4f(MVP, "MVP");
	stx_Dump1i(g_SceneFrame->terrain.weight, "weight");
	stx_Dump1i(g_SceneFrame->terrain.fungus, "fungus");
	stx_Dump1i(g_SceneFrame->terrain.dirt, "dirt");
	stx_Dump1i(g_SceneFrame->terrain.grass, "grass");
	#endif
	#if 0
	const uint flags=RESET_ALL;
	#else
	const uint flags=RESET_VB|RESET_IB;
	#endif
	if(indices.size())
		{m_iTriangleCount=indices.size()/3;IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, vertices.size(), indices.size()/3, &indices[0], &indices[0], CONSTANT_INDEX2, &vertices[0], &vertices[0], sizeof(TVertex), flags);}
	else
		{m_iTriangleCount=vertices.size()/3;IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, vertices.size()/3, &vertices[0], &vertices[0], sizeof(TVertex), flags);}
	//LOG_PRINT("TerrainCell::Draw\n");

  return true;
}

const D3DXFROMWINEVECTOR2 &TerrainCell::getVerticalBounds()  { return verticalBounds;   }

/*
const bool TerrainCell::renderAABB()
{
  if(visiblePixelsCount)
    aabb.render(BoundsDescriptor::WIRE | BoundsDescriptor::AABB);

  return (visiblePixelsCount != 0);
}*/

const TVertex *TerrainCell::getVertexStream() const
{
  return &vertices[0];
}

const __WORD__ *TerrainCell::getIndexStream()
{
  return &indices[0];
}

void TerrainCell::setupIndices(bool compile)
{
  bool initialized = false;

  if(initialized)
    return;

  register int index = 0;
  
  for(int y = 0; y < TILE_ROW_COUNT - 1; y++)
  {
    for(int x = 0; x < TILE_COLUMN_COUNT - 1; x++)
    {
      indices[index++] = (y + 0)*TILE_COLUMN_COUNT + x;
      indices[index++] = (y + 1)*TILE_COLUMN_COUNT + x;
      indices[index++] = (y + 0)*TILE_COLUMN_COUNT + x + 1;

      indices[index++] = (y + 1)*TILE_COLUMN_COUNT + x;
      indices[index++] = (y + 1)*TILE_COLUMN_COUNT + x + 1;
      indices[index++] = (y + 0)*TILE_COLUMN_COUNT + x + 1;
    }
  }
  initialized = true;
	//LOG_FNLN;
	//LOG_PRINT("indices.size()=%d\n", indices.size());
}

const unsigned int    TerrainCell::getTriangleCount()   { return m_iTriangleCount;     }

