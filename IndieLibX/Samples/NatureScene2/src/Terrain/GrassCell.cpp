#include "TerrainUtils.h"
#include "../SceneFrame.h"

GrassCell::GrassCell()
{
  verticalBounds.x =  10000.0f;
  verticalBounds.y = -10000.0f;
  blockCount       =         0;
  grass.clear();
  indices.clear();
}

GrassCell::~GrassCell()
{
  grass.clear();
  indices.clear();
}


const bool GrassCell::render()
{
	#if 0
	//LOG_FNLN;
	//LOG_PRINT("grass.size()  =%d\n", grass.size());
	//LOG_PRINT("indices.size()=%d\n", indices.size());
	#endif
	
	#if 0
	D3DXFROMWINEMATRIX MVP=g_SceneFrame->camera.GetMVP();
      IRenderer::GetRendererInstance()->setShader(g_SceneFrame->terrain.grassShader);
      IRenderer::GetRendererInstance()->setVertexFormat(g_SceneFrame->terrain.grassVF);
      if(0)
	{
		D3DXFROMWINEMATRIX I;
		D3DXFROMWINEMatrixIdentity(&I);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	}
	else
	{
  		D3DXFROMWINEMATRIX mvp=g_SceneFrame->camera.getModelViewMatrix()*g_SceneFrame->camera.getProjectionMatrix();
  		if(g_SceneFrame->terrain.grassInvert) mvp=g_SceneFrame->camera.getProjectionMatrix()*g_SceneFrame->camera.getModelViewMatrix();
  		if(g_SceneFrame->terrain.grassTranspose) D3DXFROMWINEMatrixTranspose(&mvp, &mvp);	
      		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", mvp);
	}
      IRenderer::GetRendererInstance()->setShaderConstant1f("alphaBooster", g_SceneFrame->terrain.alphaBooster);
      IRenderer::GetRendererInstance()->setShaderConstant1f("elapsedTime", timeGetTime()/1000.0f);
      IRenderer::GetRendererInstance()->setTexture("grass", g_SceneFrame->terrain.grassPack);
      //IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
      //IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	#endif
	#if 0
	stx_Dump1i(g_SceneFrame->terrain.grassShader, "grassShader");
	stx_Dump1i(g_SceneFrame->terrain.grassVF, "grassVF");
  D3DXFROMWINEMATRIX mvp1=g_SceneFrame->camera.getModelViewMatrix();
	stx_Dump4x4f(mvp1, "mvp");
  D3DXFROMWINEMATRIX mvp2=g_SceneFrame->camera.getProjectionMatrix();
	stx_Dump4x4f(mvp2, "mvp");
  D3DXFROMWINEMATRIX mvp=g_SceneFrame->camera.getModelViewMatrix()*g_SceneFrame->camera.getProjectionMatrix();
  D3DXFROMWINEMatrixTranspose(&mvp, &mvp);
	stx_Dump4x4f(mvp, "mvp");

	stx_Dump1f(g_SceneFrame->terrain.alphaBooster, "alphaBooster");
	stx_Dump1f(timeGetTime()/1000.0f, "time");

	stx_Dump1i(g_SceneFrame->terrain.grassPack, "grassPack");
	stx_Dump1i(IRenderer::GetRendererInstance()->GetblendSrcAlpha(), "blendSrcAlpha");
	stx_Dump1i(IRenderer::GetRendererInstance()->GetnoDepthTest(), "noDepthTest");
	stx_exit(0);
	#endif

	#if 0
	const uint flags=RESET_ALL;
	#else
	const uint flags=RESET_VB|RESET_IB;
	#endif
	if(indices.size())
		{m_iTriangleCount=indices.size()/3;IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, grass.size(), indices.size()/3, &indices[0], &indices[0], CONSTANT_INDEX2, &grass[0], &grass[0], sizeof(GVertex), flags);}
	else
		{m_iTriangleCount=grass.size()/3;IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, grass.size()/3, &grass[0], &grass[0], sizeof(GVertex), flags);}
	//LOG_PRINT("GrassCell::Draw\n");
  return true;
}

bool  GrassCell::setup(const HeightMap &distribution,
                       const D3DXFROMWINEVECTOR2   &offset,
                       const Image3     *randommap,
				               const Image3     *coveragemap)
{
  const HMVertex *vertices       = distribution.getVertexStream();

  const int       skipX          = 1,
                  skipY          = 1;

  D3DXFROMWINEMATRIX        yRandRotation;
  D3DXFROMWINEVECTOR4         texCoordinate,
                  random;

  D3DXFROMWINEVECTOR3         vertexOffset;

  const  ubyte *coverageContent = coveragemap->getPixels();
  const  ubyte *randomContent   = randommap->getPixels();

  //LOG_FNLN;
  //LOG_PRINT("coveragemap=%x\n", coveragemap);
  //LOG_PRINT("randommap=%x\n", randommap);
  //LOG_PRINT("coverageContent=%x\n", coverageContent);
  //LOG_PRINT("randomContent=%x\n", randomContent);
  float           randHeight      = 0,
                  jitter          = 0;

  bool            status          = true;

  int             indicesCounter  = 0,
                  indexOffset     = 0,
                  nextGObject     = 0,
                  components      = randommap->getComponentsCount(),
				          cComponents     = coveragemap->getComponentsCount(),
                  height          = randommap->getHeight(),
                  width           = randommap->getWidth(),
                  index           = 0,
                  i0              = 0,
                  y               = 0,
                  x               = 0;

  if(!randomContent || !coverageContent)
    { /* return */ LOG_PRINT("Null randommap || coveragemap");return 0;}

  if(!(height == HEIGHT_MAP_DEPTH) || !(width == HEIGHT_MAP_WIDTH) || (components != 4))
    { /* return */ LOG_PRINT("Randommap has unexpected width, height or components count (must be 4x256x256)");return 0;}

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

  if(indicesCounter <= 0)
    return true;

  blockCount = indicesCounter;

  grass.resize(12*blockCount);
  indices.resize(indicesCounter * 18);

  indicesCounter = 0;

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
      if((vertexOffset.y > GRASS_LOWER_THRESHOLD) &&
         (vertexOffset.y < GRASS_UPPER_THRESHOLD))
      {
        texCoordinate.set(0.0f, 0.0f, 0.25f, 1.0f);

        vertexOffset.x  += (random.x - 0.5f)*TERRAIN_WIDTH_SCALE;
        vertexOffset.z  += (random.y - 0.5f)*TERRAIN_DEPTH_SCALE;
        indexOffset      = 12*nextGObject;
        randHeight       = GRASS_HEIGHT + random.z*GRASS_HEIGHT_OFFSET;
        random.w         = float(coverageContent[index*cComponents+ 0])/255.0f;
        jitter           = clamp(random.w, 0.2f, 1.0f);
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
	D3DXFROMWINEMatrixRotationY(&yRandRotation,random.w*2.0f*D3DXFROMWINE_PI);
	grass[12*nextGObject+ 0].vertex=D3DXFROMWINEVECTOR3(-GRASS_WIDTH,	0.0f, 0.0f);
	D3DXFROMWINEVec3TransformCoord(&grass[12*nextGObject+ 0].vertex,&grass[12*nextGObject+ 0].vertex,&yRandRotation);
	grass[12*nextGObject+ 1].vertex=D3DXFROMWINEVECTOR3( GRASS_WIDTH,	0.0f, 0.0f);
	D3DXFROMWINEVec3TransformCoord(&grass[12*nextGObject+ 1].vertex,&grass[12*nextGObject+ 1].vertex,&yRandRotation);
	grass[12*nextGObject+ 2].vertex=D3DXFROMWINEVECTOR3( GRASS_WIDTH, randHeight, 0.0f);
	D3DXFROMWINEVec3TransformCoord(&grass[12*nextGObject+ 2].vertex,&grass[12*nextGObject+ 2].vertex,&yRandRotation);
	grass[12*nextGObject+ 3].vertex=D3DXFROMWINEVECTOR3(-GRASS_WIDTH, randHeight, 0.0f);
	D3DXFROMWINEVec3TransformCoord(&grass[12*nextGObject+ 3].vertex,&grass[12*nextGObject+ 3].vertex,&yRandRotation);
	grass[12*nextGObject+ 0].uvJitter=D3DXFROMWINEVECTOR3(texCoordinate.x, texCoordinate.y, jitter);
	grass[12*nextGObject+ 1].uvJitter=D3DXFROMWINEVECTOR3(texCoordinate.z, texCoordinate.y, jitter);
	grass[12*nextGObject+ 2].uvJitter=D3DXFROMWINEVECTOR3(texCoordinate.z, texCoordinate.w, jitter);
	grass[12*nextGObject+ 3].uvJitter=D3DXFROMWINEVECTOR3(texCoordinate.x, texCoordinate.w, jitter);
	D3DXFROMWINEMatrixRotationY(&yRandRotation,D3DXFROMWINE_PI/3.0f);

	for(i0 = 4; i0 < 12; i0++)
	{
	  grass[12*nextGObject+i0].uvJitter = grass[12*nextGObject+i0 - 4].uvJitter;
	  grass[12*nextGObject+i0].vertex   = grass[12*nextGObject+i0 - 4].vertex;
	  D3DXFROMWINEVec3TransformCoord(&grass[12*nextGObject+i0].vertex,&grass[12*nextGObject+i0].vertex,&yRandRotation);
	}

	for(i0 = 0; i0 < 12; i0++)
	{
	  grass[12*nextGObject+i0].binormal = vertices[index].binormal;
	  grass[12*nextGObject+i0].tangent  = vertices[index].tangent;
	  grass[12*nextGObject+i0].normal   = vertices[index].normal;
	  grass[12*nextGObject+i0].offset   = vertexOffset;
	}

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
	//LOG_PRINT("grass.size()=%d\n", grass.size());
	//LOG_PRINT("indices.size()=%d\n", indices.size());
  D3DXFROMWINEVECTOR3 v3( -1e10f, -1e10f, -1e10f);
  if(1)
  for(unsigned int i=0;i<grass.size();i++)
  {
  	v3.x = std::max( grass[i].vertex.x, v3.x);
  	v3.y = std::max( grass[i].vertex.y, v3.y);
  	v3.z = std::max( grass[i].vertex.z, v3.z);
  }

  //LOG_PRINT("v3.x=%f\n", v3.x);
  //LOG_PRINT("v3.y=%f\n", v3.y);
  //LOG_PRINT("v3.z=%f\n", v3.z);

  return true;
}

const D3DXFROMWINEVECTOR2        &GrassCell::getVerticalBounds()  { return verticalBounds;   }
const unsigned int    GrassCell::getTriangleCount()   { return m_iTriangleCount;     }
const __WORD__ *GrassCell::getIndexStream()     { return &indices[0];          }

