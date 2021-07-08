#include "GrassCell.h"
#include "Terrain.h"
#include "../SceneFrame.h"
//using namespace NatureScene;

VertexFormatID GrassCell::m_pVertexDeclaration=-1;
IndexBufferID GrassCell::indexBufferID=-1;

GrassCell::GrassCell()
{
  geometryBufferID =        -1;
  verticalBounds.x =  10000.0f;
  verticalBounds.y = -10000.0f;
  indexBufferID    =        -1;
  blockCount       =         0;
  indexCount       =         0;
  indices          =         0;
}

GrassCell::~GrassCell()
{/*
  if(geometryBufferID)
    SAFE_DELETE_ARRAY(&geometryBufferID);

  if(indexBufferID)
    SAFE_DELETE_ARRAY(&indexBufferID);*/

  deleteArray(indices);
}


const bool GrassCell::render(ShaderID sha)
{
  //LOG_FNLN;
  static int offset1 = sizeof(D3DXFROMWINEVECTOR3),
             offset2 = sizeof(D3DXFROMWINEVECTOR3) + offset1,
             offset3 = sizeof(D3DXFROMWINEVECTOR3) + offset2,
             offset4 = sizeof(D3DXFROMWINEVECTOR3) + offset3,
             offset5 = sizeof(D3DXFROMWINEVECTOR3) + offset4;
  //LOG_PRINT("blockCount=%x\n", blockCount);
  //LOG_PRINT("indexCount=%x\n", indexCount);
  //LOG_PRINT("geometryBufferID=%x\n", geometryBufferID);
  if(!blockCount || !indexCount || !geometryBufferID)
    return false;
  //LOG_FNLN;
#if 0
  if(geometryBufferID)
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, geometryBufferID); 

    glClientActiveTextureARB(GL_TEXTURE0_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, sizeof(GVertex), OFFSET(0));
 
    glClientActiveTextureARB(GL_TEXTURE1_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, sizeof(GVertex), OFFSET(offset1));

    glClientActiveTextureARB(GL_TEXTURE2_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, sizeof(GVertex), OFFSET(offset2));

    glClientActiveTextureARB(GL_TEXTURE3_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, sizeof(GVertex), OFFSET(offset3));

    glNormalPointer(GL_FLOAT, sizeof(GVertex), OFFSET(offset4));
    glEnableClientState(GL_NORMAL_ARRAY);
 
    glVertexPointer(3, GL_FLOAT, sizeof(GVertex), OFFSET(offset5));
    glEnableClientState(GL_VERTEX_ARRAY);
  }

  if(indexBufferID)
  {
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBufferID);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT,  0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  }
  else
    glDrawRangeElements(GL_TRIANGLES, 0, indexCount, indexCount, GL_UNSIGNED_SHORT, indices);

  if(geometryBufferID)
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0); 

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
  }
	if(!g_SceneFrame->GetTerrain())
		return false;
	LOG_PRINT("g_SceneFrame = %x\n", g_SceneFrame);
	LOG_PRINT("g_SceneFrame->GetTerrain() = %x\n", g_SceneFrame->GetTerrain());
	LOG_PRINT("g_SceneFrame->GetTerrain()->getGrassShader() = %x\n", g_SceneFrame->GetTerrain()->getGrassShader());
#else
	ShaderID shaID=g_SceneFrame->GetTerrain()->getGrassShader();
	IRenderer::GetRendererInstance()->setShader(shaID);
  	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	ShaderID texID=g_SceneFrame->getGrassPackTexture();
	IRenderer::GetRendererInstance()->setTexture("grass", texID);

  	D3DXFROMWINEMATRIX matVP = g_SceneFrame->GetCamera().getModelViewMatrix()*g_SceneFrame->GetProj();
  	D3DXFROMWINEMatrixTranspose(&matVP, &matVP);

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matVP);

	IRenderer::GetRendererInstance()->setShaderConstant1f("alphaBooster", g_SceneFrame->GetTerrain()->getAlphaBooster());
	IRenderer::GetRendererInstance()->setShaderConstant1f("elapsedTime", timeGetTime()/1000.0f);

	IRenderer::GetRendererInstance()->setVertexFormat( m_pVertexDeclaration );
	IRenderer::GetRendererInstance()->setVertexBuffer( 0, geometryBufferID );

	if (g_SceneFrame->GetTerrain()->getWireFrameFlag())
		IRenderer::GetRendererInstance()->setRasterizerState(
			IRenderer::GetRendererInstance()->GetfillWireFrame());
	//else
		//IRenderer::GetRendererInstance()->setRasterizerState(
			//IRenderer::GetRendererInstance()->GetfillSolid());

	#if 0
	LOG_FNLN;
	LOG_PRINT("m_pVertexDeclaration = %x\n", m_pVertexDeclaration);
	LOG_PRINT("geometryBufferID = %x\n", geometryBufferID);
	LOG_PRINT("indexBufferID = %x\n", indexBufferID);
	#endif
	if(indexBufferID!=-1)
	{
		(IRenderer::GetRendererInstance()->setIndexBuffer(indexBufferID));
		
		
		//LOG_FNLN;
		//LOG_PRINT("DrawIndexedPrimitive\n");
		IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, m_NumVert, 0, m_NumTris/*, RESET_VB|RESET_IB*/);
	}
	else
	{
		
		
		//LOG_FNLN;
		//LOG_PRINT("DrawPrimitive\n");
		IRenderer::GetRendererInstance()->DrawPrimitive( PRIM_TRIANGLES, 0, m_NumTris/*, RESET_VB|RESET_IB*/);
	}
#endif
  return true;
}

bool  GrassCell::setup(const HeightMap &distribution,
                       const Tuple2i   &offset,
                       const Image3     &randommap,
			const Image3     &coveragemap,ShaderID sha)
{
  //LOG_FNLN;
  m_pShader=sha;

  const HMVertex *vertices       = distribution.getVertexStream();

  //GrassBlock     *grass          = 0;
	GVertex     *grass	      = 0;

  const int       skipX          = 1,
                  skipY          = 1;

  D3DXFROMWINEMATRIX        yRandRotation;
  D3DXFROMWINEVECTOR4         texCoordinate,
                  random;

  D3DXFROMWINEVECTOR3         vertexOffset;


  const  ubyte *coverageContent = coveragemap.getPixels(),
	               *randomContent   = randommap.getPixels();

  float           randHeight      = 0,
                  jitter          = 0;

  bool            status          = true;

  int             indicesCounter  = 0,
                  indexOffset     = 0,
                  nextGObject     = 0,




                  components      = getChannelCount(randommap.getFormat()),
				          cComponents     = getChannelCount(coveragemap.getFormat()),
                  height          = randommap.getHeight(),
                  width           = randommap.getWidth(),
                  index           = 0,
                  i0              = 0,
                  y               = 0,
                  x               = 0;
 
  //LOG_FNLN;
  //LOG_PRINT("randomContent:%x\n", randomContent);
  //LOG_PRINT("coverageContent:%x\n", coverageContent);
  if(!randomContent || !coverageContent){
    LOG_PRINT("Null randommap || coveragemap");return;}

  //LOG_FNLN;
  if(!(height == HEIGHT_MAP_DEPTH) || !(width == HEIGHT_MAP_WIDTH) || (components != 4)){
    LOG_PRINT("Randommap has unexpected width, height or components count (must be 4x256x256)");return;}
  //LOG_FNLN;

  for(y = 0, index = 0; y < TILE_ROW_COUNT - 1; y+=skipY)
  {
    for(x = 0; x < TILE_COLUMN_COUNT - 1; x+=skipX)
    {
      index = (y + offset.y)*distribution.getWidth() + offset.x + x;

      if((vertices[index].vertex.y > GRASS_LOWER_THRESHOLD) &&
         (vertices[index].vertex.y < GRASS_UPPER_THRESHOLD))
        indicesCounter++;
    }
  } 

  //LOG_FNLN;
  if(indicesCounter <= 0)
    return true;

  //LOG_FNLN;
  blockCount = indicesCounter;
  indexCount = indicesCounter * 18;

  indicesCounter = 0;
  indices        = new __WORD__[indexCount];

#if 0
  glGenBuffersARB(1, &geometryBufferID);
    
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, geometryBufferID);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, blockCount * sizeof(GrassBlock),
                  0, GL_STATIC_DRAW_ARB);

  if(glGetError() == GL_NO_ERROR)
    grass = (GrassBlock *)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
  else
   return LOG_PRINT("Not enough memory for the interleaved geometry arrays");
#else
  m_NumVert=12*blockCount;
  grass=new GVertex[m_NumVert];
  m_NumTris=indexCount/3;
#endif
  //LOG_FNLN;
  if(!status)
  {
    LOG_PRINT("Could not generate a geometry buffer object, GRASS");return;
  }
  //LOG_FNLN;
/*
   float           maxX0    = 0.0f,
                  minX0    =  0.0f;
   float           maxZ0    = 0.0f,
                  minZ0    =  0.0f;
*/
  for(y = 0, index = 0; y < TILE_ROW_COUNT - 1; y+=skipY)
  {
    for(x = 0; x < TILE_COLUMN_COUNT - 1; x+=skipX)
    {
      index = (y + offset.y)*distribution.getWidth() + offset.x + x;

      random.set(float(randomContent[index*4 + 0])/255.0f,
                 float(randomContent[index*4 + 1])/255.0f,
                 float(randomContent[index*4 + 2])/255.0f,
                 float(randomContent[index*4 + 3])/255.0f);

      vertexOffset = vertices[index].vertex;
/*
	float newX = vertices[index].vertex.x;
	float newZ = vertices[index].vertex.z;
      maxX0  = newX > maxX0 ? newX : maxX0;
      minX0  = newX < minX0 ? newX : minX0;
      maxZ0  = newZ > maxZ0 ? newZ : maxZ0;
      minZ0  = newZ < minZ0 ? newZ : minZ0;
*/
      if((vertexOffset.y > GRASS_LOWER_THRESHOLD) &&
         (vertexOffset.y < GRASS_UPPER_THRESHOLD))
      {
        texCoordinate.set(0.0f, 0.0f, 0.25f, 1.0f);

        vertexOffset.x  += (random.x - 0.5f)*TERRAIN_WIDTH_SCALE;
        vertexOffset.z  += (random.y - 0.5f)*TERRAIN_DEPTH_SCALE;
        indexOffset      = 12*nextGObject;
        randHeight       = GRASS_HEIGHT + random.z*GRASS_HEIGHT_OFFSET;
        random.w         = float(coverageContent[index*cComponents+ 0])/255.0f;
        jitter           = clampNS(random.w, 0.2f, 1.0f);
        vertexOffset.y   = distribution.getInterpolatedHeight(vertexOffset) - 2.0f;

        verticalBounds.x =      vertexOffset.y           < verticalBounds.x ?         vertexOffset.y        : verticalBounds.x;
        verticalBounds.y = (vertexOffset.y + randHeight) > verticalBounds.y ? (vertexOffset.y + randHeight) : verticalBounds.y;

        if( random.w > 0.25 &&  random.w <= 0.5)
        {
          texCoordinate.x += 0.25;
          texCoordinate.z += 0.25;
        }

        if( random.w > 0.5 &&  random.w <= 0.75)
        {
          texCoordinate.x += 0.5;
          texCoordinate.z += 0.5;
        }
        
        if( random.w > 0.75)
        {
          texCoordinate.x += 0.75;
          texCoordinate.z += 0.75;
        }
#if 0
        yRandRotation.rotateY(random.w*TWO_PI);
	grass[nextGObject].vertices[ 0].vertex.set(-GRASS_WIDTH,       0.0f, 0.0f);
        grass[nextGObject].vertices[ 0].vertex *= yRandRotation;
        grass[nextGObject].vertices[ 1].vertex.set( GRASS_WIDTH,       0.0f, 0.0f);
        grass[nextGObject].vertices[ 1].vertex *= yRandRotation;
        grass[nextGObject].vertices[ 2].vertex.set( GRASS_WIDTH, randHeight, 0.0f);
        grass[nextGObject].vertices[ 2].vertex *= yRandRotation;
        grass[nextGObject].vertices[ 3].vertex.set(-GRASS_WIDTH, randHeight, 0.0f);
        grass[nextGObject].vertices[ 3].vertex *= yRandRotation;
        grass[nextGObject].vertices[ 0].uvJitter.set(texCoordinate.x, texCoordinate.y, jitter);
        grass[nextGObject].vertices[ 1].uvJitter.set(texCoordinate.z, texCoordinate.y, jitter);
        grass[nextGObject].vertices[ 2].uvJitter.set(texCoordinate.z, texCoordinate.w, jitter);
        grass[nextGObject].vertices[ 3].uvJitter.set(texCoordinate.x, texCoordinate.w, jitter);
        yRandRotation.rotateY(D3DXFROMWINE_PI/3.0f);
        for(i0 = 4; i0 < 12; i0++)
        {
          grass[nextGObject].vertices[i0].uvJitter = grass[nextGObject].vertices[i0 - 4].uvJitter;
          grass[nextGObject].vertices[i0].vertex   = grass[nextGObject].vertices[i0 - 4].vertex;
          grass[nextGObject].vertices[i0].vertex  *= yRandRotation;
        }
        for(i0 = 0; i0 < 12; i0++)
        {
          grass[nextGObject].vertices[i0].binormal = vertices[index].binormal;
          grass[nextGObject].vertices[i0].tangent  = vertices[index].tangent;
          grass[nextGObject].vertices[i0].normal   = vertices[index].normal;
          grass[nextGObject].vertices[i0].offset   = vertexOffset;
        }
#else
	D3DXFROMWINEMatrixRotationY(&yRandRotation,random.w*2.0f*D3DXFROMWINE_PI);
	grass[12*nextGObject+ 0].vertex=D3DXFROMWINEVECTOR3(-GRASS_WIDTH,	0.0f, 0.0f);
	D3DXFROMWINEVec3TransformNormal(&grass[12*nextGObject+ 0].vertex,&grass[12*nextGObject+ 0].vertex,&yRandRotation);
	grass[12*nextGObject+ 1].vertex=D3DXFROMWINEVECTOR3( GRASS_WIDTH,	0.0f, 0.0f);
	D3DXFROMWINEVec3TransformNormal(&grass[12*nextGObject+ 1].vertex,&grass[12*nextGObject+ 1].vertex,&yRandRotation);
	grass[12*nextGObject+ 2].vertex=D3DXFROMWINEVECTOR3( GRASS_WIDTH, randHeight, 0.0f);
	D3DXFROMWINEVec3TransformNormal(&grass[12*nextGObject+ 2].vertex,&grass[12*nextGObject+ 2].vertex,&yRandRotation);
	grass[12*nextGObject+ 3].vertex=D3DXFROMWINEVECTOR3(-GRASS_WIDTH, randHeight, 0.0f);
	D3DXFROMWINEVec3TransformNormal(&grass[12*nextGObject+ 3].vertex,&grass[12*nextGObject+ 3].vertex,&yRandRotation);
	grass[12*nextGObject+ 0].uvJitter=D3DXFROMWINEVECTOR3(texCoordinate.x, texCoordinate.y, jitter);
	grass[12*nextGObject+ 1].uvJitter=D3DXFROMWINEVECTOR3(texCoordinate.z, texCoordinate.y, jitter);
	grass[12*nextGObject+ 2].uvJitter=D3DXFROMWINEVECTOR3(texCoordinate.z, texCoordinate.w, jitter);
	grass[12*nextGObject+ 3].uvJitter=D3DXFROMWINEVECTOR3(texCoordinate.x, texCoordinate.w, jitter);
	D3DXFROMWINEMatrixRotationY(&yRandRotation,D3DXFROMWINE_PI/3.0f);

	for(i0 = 4; i0 < 12; i0++)
	{
	  grass[12*nextGObject+i0].uvJitter = grass[12*nextGObject+i0 - 4].uvJitter;
	  grass[12*nextGObject+i0].vertex   = grass[12*nextGObject+i0 - 4].vertex;
	  D3DXFROMWINEVec3TransformNormal(&grass[12*nextGObject+i0].vertex,&grass[12*nextGObject+i0].vertex,&yRandRotation);
	}

	for(i0 = 0; i0 < 12; i0++)
	{
	  grass[12*nextGObject+i0].binormal = vertices[index].binormal;
	  grass[12*nextGObject+i0].tangent  = vertices[index].tangent;
	  grass[12*nextGObject+i0].normal   = vertices[index].normal;
	  grass[12*nextGObject+i0].offset   = vertexOffset;
	}
#endif
        indices[indicesCounter++] = indexOffset + 0;
        indices[indicesCounter++] = indexOffset + 1; 
        indices[indicesCounter++] = indexOffset + 3;
        indices[indicesCounter++] = indexOffset + 1;
        indices[indicesCounter++] = indexOffset + 2; 
        indices[indicesCounter++] = indexOffset + 3;

        indices[indicesCounter++] = indexOffset + 4; 
        indices[indicesCounter++] = indexOffset + 5; 
        indices[indicesCounter++] = indexOffset + 7;
        indices[indicesCounter++] = indexOffset + 5;
        indices[indicesCounter++] = indexOffset + 6;
        indices[indicesCounter++] = indexOffset + 7;

        indices[indicesCounter++] = indexOffset +  8;
        indices[indicesCounter++] = indexOffset +  9;
        indices[indicesCounter++] = indexOffset + 11;
        indices[indicesCounter++] = indexOffset +  9; 
        indices[indicesCounter++] = indexOffset + 10;
        indices[indicesCounter++] = indexOffset + 11;

        nextGObject++;
      }
    }
  } 
  //LOG_FNLN;
 #if 0
 	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

  glGenBuffersARB(1, &indexBufferID);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBufferID);
  glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexCount * sizeof(unsigned short),
                  indices, GL_STATIC_DRAW_ARB);

  status = (glGetError() == GL_NO_ERROR);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
#else
  //LOG_FNLN;
  geometryBufferID = IRenderer::GetRendererInstance()->addVertexBuffer(m_NumVert*sizeof(GVertex), STATIC, grass);
/*
	//LOG_FNLN;
	//LOG_PRINT("m_NumVert=%d\n",m_NumVert);

	LOG_PRINT("minX0=%f\n",minX0);
	LOG_PRINT("maxX0=%f\n",maxX0);

	LOG_PRINT("minZ0=%f\n",minZ0);
	LOG_PRINT("maxZ0=%f\n",maxZ0);
*/
  if(indexBufferID==-1) indexBufferID = IRenderer::GetRendererInstance()->addIndexBuffer(
	  indexCount,sizeof(__WORD__), STATIC, indices);

	//for(__WORD__ i = 0; i < indexCount; i++)
		//LOG_PRINT("grass indices=%d\n",indices[i]);

	//LOG_FNLN;
	//LOG_PRINT("indexCount=%d\n",indexCount);


  delete[] grass;

  delete[] indices;

  //LOG_FNLN;
  if(!status)
    LOG_PRINT("Could not generate a geometry buffer object, GRASS\n");
  //LOG_FNLN;

	if(m_pVertexDeclaration==-1)
	{
	FormatDesc fmt[] = {
		{ 0,  TYPE_VERTEX, FORMAT_FLOAT, 3},
		{ 0, TYPE_NORMAL, FORMAT_FLOAT, 3},
	       { 0, TYPE_TEXCOORD, FORMAT_FLOAT, 3},
		{ 0, TYPE_TEXCOORD, FORMAT_FLOAT, 3},
		{ 0, TYPE_TEXCOORD, FORMAT_FLOAT, 3},
		{ 0, TYPE_TEXCOORD, FORMAT_FLOAT, 3},
	};

	m_pVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(fmt, elementsOf(fmt),sha);

	}

#endif
  //LOG_FNLN;
  if(!status){
    LOG_PRINT("Could not generate a geometry buffer object, GRASS");return;}

  //LOG_FNLN;

  return true;
}

const D3DXFROMWINEVECTOR2        &GrassCell::getVerticalBounds()  { return verticalBounds;   }
const unsigned int    GrassCell::getGeometryBufferID(){ return geometryBufferID; }
const unsigned int    GrassCell::getTriangleCount()   { return indexCount/3;     }
const unsigned int    GrassCell::getIndexBufferID()   { return indexBufferID;    }
const __WORD__ *GrassCell::getIndexStream()     { return indices;          }
