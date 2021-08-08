#include "TerrainCell.h"
#include "../SceneFrame.h"
//using namespace NatureScene;

__WORD__ TerrainCell::indices[INDEX_COUNT];
IndexBufferID   TerrainCell::indexBufferID  =    -1;
unsigned int   TerrainCell::userCount      =     0;
int TerrainCell::m_NumTris=0;

TerrainCell::TerrainCell()
{
  m_pVertexDeclaration=-1;
  geometryBufferID  =        -1;
  verticalBounds.x  =  10000.0f;
  verticalBounds.y  = -10000.0f;
  userCount        +=         1;
  m_NumVert=0;
  m_NumTris=0;
}

TerrainCell::~TerrainCell()
{
  //if(geometryBufferID)
   //SAFE_DELETE_ARRAY(&geometryBufferID);
}

bool  TerrainCell::setup(const HeightMap& heightmap,
                         const Tuple2i&   start,ShaderID sha)
{
  m_pShader=sha;
  const HMVertex *vertices     =  heightmap.getVertexStream();
  TVertex        *gpuBuffer    =      0;
  float           maxHeight0    = 0.0f,
                  minHeight0    =  0.0f;
  float           maxHeight    = -20000.0f,
                  minHeight    =  20000.0f,
                  newHeight    =      0.0f;
  int             height       = heightmap.getHeight(),
                  width        = heightmap.getWidth(),
                  offset       = 0,
                  index        = 0,
                  x            = 0,
                  y            = 0;

  if(!vertices){
    LOG_PRINT("Null heightmap content");return;}

  if(!(height == HEIGHT_MAP_DEPTH) || !(width == HEIGHT_MAP_WIDTH)){
    LOG_PRINT("Heightmap has unexpected width or height");return;}

#if 0
  glGenBuffersARB(1, &geometryBufferID);
    
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, geometryBufferID);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, TILE_COUNT * sizeof(TVertex),
                  0, GL_STATIC_DRAW_ARB);

  if(glGetError() == GL_NO_ERROR)
    gpuBuffer = (TVertex *)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
  else
   return LOG_PRINT("Not enough memory for the interleaved geometry arrays");
#else
	gpuBuffer = new TVertex[TILE_COUNT];
#endif
/*
   float           maxX0    = 0.0f,
                  minX0    =  0.0f;
   float           maxZ0    = 0.0f,
                  minZ0    =  0.0f;
*/
  for(y = start.y; y < start.y + TILE_ROW_COUNT; y++)
  {
    y = clampNS(y, 0, height - 1);
    for(x = start.x; x < start.x + TILE_COLUMN_COUNT; x++, offset++)
    {
      x          = clampNS(x, 0, width - 1);
      index      = y*width + x;
      newHeight  = vertices[index].vertex.y;
      maxHeight  = newHeight > maxHeight ? newHeight : maxHeight;
      minHeight  = newHeight < minHeight ? newHeight : minHeight;

      gpuBuffer[offset].vertex = -vertices[index].vertex;
/*
	float newX = vertices[index].vertex.x;
	float newZ = vertices[index].vertex.z;
      maxX0  = newX > maxX0 ? newX : maxX0;
      minX0  = newX < minX0 ? newX : minX0;
      maxZ0  = newZ > maxZ0 ? newZ : maxZ0;
      minZ0  = newZ < minZ0 ? newZ : minZ0;


	LOG_PRINT("vertex.x=%f\n", vertices[index].vertex.x);
	LOG_PRINT("vertex.y=%f\n", vertices[index].vertex.y);
	LOG_PRINT("vertex.z=%f\n", vertices[index].vertex.z);
*/
      gpuBuffer[offset].normal = vertices[index].normal;
      gpuBuffer[offset].uv     = vertices[index].uv;
    }
  }

  //glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
  //glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	m_NumVert=offset;
	geometryBufferID = IRenderer::GetRendererInstance()->addVertexBuffer(TILE_COUNT * sizeof(TVertex), STATIC, gpuBuffer);
/*
	LOG_FNLN;
	//LOG_PRINT("TILE_COUNT=%d\n",TILE_COUNT);

	LOG_PRINT("minX0=%f\n",minX0);
	LOG_PRINT("maxX0=%f\n",maxX0);

	LOG_PRINT("minZ0=%f\n",minZ0);
	LOG_PRINT("maxZ0=%f\n",maxZ0);
*/
  verticalBounds.set(minHeight, maxHeight);

if(m_pVertexDeclaration==-1)
	{
	FormatDesc fmt[] = {
		{ 0,  TYPE_VERTEX, FORMAT_FLOAT, 3},
		{ 0, TYPE_NORMAL, FORMAT_FLOAT, 3},
	       { 0, TYPE_TEXCOORD, FORMAT_FLOAT, 2},
	};
	m_pVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(fmt, elementsOf(fmt),sha);

	
}
 
  return true;
} 

const bool TerrainCell::render(ShaderID sha)
{
  static int offset1 = sizeof(D3DXFROMWINEVECTOR3),
             offset2 = sizeof(D3DXFROMWINEVECTOR2) + offset1;
#if 0
  if(geometryBufferID)
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, geometryBufferID); 

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(TVertex), OFFSET(0));
 
    glNormalPointer(GL_FLOAT, sizeof(TVertex), OFFSET(offset1));
    glEnableClientState(GL_NORMAL_ARRAY);
 
    glVertexPointer(3, GL_FLOAT, sizeof(TVertex), OFFSET(offset2));
    glEnableClientState(GL_VERTEX_ARRAY);
  }
 
  if(indexBufferID)
  {
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBufferID);
    glDrawElements(GL_TRIANGLES, INDEX_COUNT, GL_UNSIGNED_SHORT,  0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  }
  else
    glDrawElements(GL_TRIANGLES, INDEX_COUNT, GL_UNSIGNED_SHORT, indices);

  if(geometryBufferID)
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0); 
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
#else
	//LOG_PRINT("sha = %x\n", sha);
	IRenderer::GetRendererInstance()->setShader(g_SceneFrame->GetTerrain()->getTerrainShader());

	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullBack());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullNone());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());
	//IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

  	D3DXFROMWINEMATRIX matVP = g_SceneFrame->GetCamera().getModelViewMatrix()*g_SceneFrame->GetProj();
  	D3DXFROMWINEMatrixTranspose(&matVP, &matVP);

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matVP);
    	IRenderer::GetRendererInstance()->setTexture("weight", g_SceneFrame->GetTerrain()->getWeightTexture());
    	IRenderer::GetRendererInstance()->setTexture("fungus", g_SceneFrame->GetTerrain()->getFungusTexture());
    	IRenderer::GetRendererInstance()->setTexture("dirt", g_SceneFrame->GetTerrain()->getDirtTexture());
    	IRenderer::GetRendererInstance()->setTexture("grass", g_SceneFrame->getGrassTexture());

	IRenderer::GetRendererInstance()->setVertexFormat( m_pVertexDeclaration );
	IRenderer::GetRendererInstance()->setVertexBuffer( 0, geometryBufferID);

	if (g_SceneFrame->GetTerrain()->getWireFrameFlag())
		IRenderer::GetRendererInstance()->setRasterizerState(
			IRenderer::GetRendererInstance()->GetfillWireFrame());
	//else
		//IRenderer::GetRendererInstance()->setRasterizerState(
			//IRenderer::GetRendererInstance()->GetfillSolid());

	//LOG_PRINT("m_pVertexDeclaration = %x\n", m_pVertexDeclaration);
	//LOG_PRINT("geometryBufferID = %x\n", geometryBufferID);
	//LOG_PRINT("indexBufferID = %x\n", indexBufferID);
	if(indexBufferID!=-1)
	{
		IRenderer::GetRendererInstance()->setIndexBuffer(indexBufferID);
		
		
		//LOG_FNLN;
		//LOG_PRINT("DrawIndexedPrimitive\n");
		IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, m_NumVert, 0, m_NumTris /*, RESET_VB|RESET_IB*/);
	}
	else
	{
		
		
		//LOG_FNLN;
		//LOG_PRINT("DrawPrimitive\n");
		IRenderer::GetRendererInstance()->DrawPrimitive( PRIM_TRIANGLES, 0, m_NumTris /*, RESET_VB|RESET_IB*/);
	}

#endif
  return (geometryBufferID != 0);
}

const unsigned int TerrainCell::getIndexBufferID()
{
  return indexBufferID;
}

const D3DXFROMWINEVECTOR2 &TerrainCell::getVerticalBounds()  { return verticalBounds;   }

/*
const bool TerrainCell::renderAABB()
{
  if(visiblePixelsCount)
    aabb.render(BoundsDescriptor::WIRE | BoundsDescriptor::AABB);

  return (visiblePixelsCount != 0);
}*/

const unsigned int TerrainCell::getGeometryBufferID() const
{
  return geometryBufferID;
}

const unsigned short *TerrainCell::getIndexStream()
{
  return indices;
}

void TerrainCell::setupIndices(bool compile)
{
  bool initialized = false;

  if(initialized)
    return;

  int index = 0;
  
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
#if 0
  if(compile && GL_ARB_vertex_buffer_object)
  {
    glGenBuffersARB(1, &indexBufferID);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBufferID);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, INDEX_COUNT * sizeof(unsigned short),
                    indices, GL_STATIC_DRAW_ARB);

    if(glGetError() != GL_NO_ERROR) 
      LOG_PRINT("Could not generate the index buffer object");

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  }
#else
 indexBufferID = IRenderer::GetRendererInstance()->addIndexBuffer(INDEX_COUNT,sizeof(__WORD__), STATIC, indices);

	//for(__WORD__ i = 0; i < INDEX_COUNT; i++)
	//	LOG_PRINT("terrain indices=%d\n",indices[i]);

	//LOG_FNLN;
	//LOG_PRINT("INDEX_COUNT=%d\n",INDEX_COUNT);
	m_NumTris=index/3;
#endif
  initialized = true;
}
