#include "HeightMap.h"
//include "../Tools/Logger.h"
//using namespace NatureScene;

HeightMap::HeightMap()
{
  indexCount = 0; 
  height     = 0;
  width      = 0;  

  vertices   = 0;
}

HeightMap::~HeightMap()
{
  delete[] vertices;
}

bool HeightMap::setup(const Image3&  heightmap,
                      const Image3&  watermap)
{

  if(vertices)
    return true;


  const ubyte *hmBytes      = heightmap.getPixels(),
                *wmBytes      = watermap.getPixels();


  float          waterHeight  = 0,
                 newHeight    = 0;

  int            hmByteCount  = getChannelCount(heightmap.getFormat()),
                 wmByteCount  = getChannelCount(watermap.getFormat()),
                 index        = 0;


  height       = heightmap.getHeight();
  width        = heightmap.getWidth();


  if(!wmBytes || ! hmBytes){
    LOG_PRINT("Null heightmap or watermap content");return;}


  if(!(height == HEIGHT_MAP_DEPTH) || !(width == HEIGHT_MAP_WIDTH)){
    LOG_PRINT("Heightmap has unexpected width or height");return;}


  if(!(height == watermap.getHeight()) || !(width == watermap.getWidth())){
    LOG_PRINT("Watermap has unexpected width or height");return;}


  vertices = new HMVertex[height*width];


  for(int y = 0; y < height; y++)
  {
    for(int x = 0; x < width; x++)
    {
      index       = (y*width + x);
      waterHeight = float(wmBytes[index*wmByteCount])/255.0f;
      newHeight   = float(hmBytes[index*hmByteCount])/255.0f;
      newHeight   = (waterHeight < 0.8f) ? (0.8f - waterHeight)*WATER_MIN_HEIGHT : newHeight *TERRAIN_MAX_HEIGHT;

      vertices[index].vertex.set(float(-TERRAIN_X_OFFSET + x)*TERRAIN_WIDTH_SCALE, 
                                 newHeight,
                                 float(-TERRAIN_Z_OFFSET + y)*TERRAIN_DEPTH_SCALE);

      vertices[index].uv.set(float(x)/HEIGHT_MAP_WIDTH, float(y)/HEIGHT_MAP_DEPTH);
    }
  }


  computeBTangents();


  return true;
}

const float HeightMap::getInterpolatedHeight(const D3DXFROMWINEVECTOR3 &position) const
{
  if(!vertices)
    return -1;

  D3DXFROMWINEVECTOR3 local    = vertices[0].vertex;
  float   resutl   = 0.0f,
          xInter   = 0.0f,
          zInter   = 0.0f;
  int     x0       = 0,
          z0       = 0,
          x1       = 0,
          z1       = 0;

  local.x *= -1.0f;
  local.z *= -1.0f;
  local   += position;
  
  x0       = clampNS(int(local.x/TERRAIN_WIDTH_SCALE), 0, width  - 2);
  z0       = clampNS(int(local.z/TERRAIN_DEPTH_SCALE), 0, height - 2);

  x1       = x0 + 1;   
  z1       = z0 + 1;   

  xInter   = (position.x -  vertices[z0*width + x0].vertex.x)/TERRAIN_WIDTH_SCALE;
  zInter   = (position.z -  vertices[z0*width + x0].vertex.z)/TERRAIN_DEPTH_SCALE;
 
  resutl  = ((1.0f - xInter)*vertices[z0*width + x0].vertex.y + xInter*vertices[z0*width + x1].vertex.y)*(1.0f - zInter) + 
            ((1.0f - xInter)*vertices[z1*width + x0].vertex.y + xInter*vertices[z1*width + x1].vertex.y)*zInter; 
  return resutl;
}

   
bool HeightMap::render()
{
  int x      = 0,
      y      = 0,
      i      = 0;

  if(!vertices)
    return false;

  IRenderer::GetRendererInstance()->Begin(PRIM_LINES);
  IRenderer::GetRendererInstance()->Color3f(0.0, 0.0, 1.0);
  for(y = 0; y < height; y++)
  {
    for(x = 0; x < width; x++)
    {
      i = y*width + x;

      IRenderer::GetRendererInstance()->Vertex3fv(vertices[i].vertex);
      IRenderer::GetRendererInstance()->Vertex3fv(vertices[i].vertex + vertices[i].normal*20.0f);
    }
  }
 
  IRenderer::GetRendererInstance()->Color3f(0.0, 1.0, 0.0);
  for(y = 0; y < height; y++)
  {
    for(x = 0; x < width; x++)
    {
      i = y*width + x;

      IRenderer::GetRendererInstance()->Vertex3fv(vertices[i].vertex);
      IRenderer::GetRendererInstance()->Vertex3fv(vertices[i].vertex + vertices[i].binormal*20.0f);
    }
  }

  IRenderer::GetRendererInstance()->Color3f(1.0, 0.0, 0.0);

  for(y = 0; y < height; y++)
  {
    for(x = 0; x < width; x++)
    {
      i = y*width + x;

      IRenderer::GetRendererInstance()->Vertex3fv(vertices[i].vertex);
      IRenderer::GetRendererInstance()->Vertex3fv(vertices[i].vertex + vertices[i].tangent*20.0f);
    }
  }
 
  IRenderer::GetRendererInstance()->End();

  IRenderer::GetRendererInstance()->Color3f(1.0, 1.0, 1.0);

  return true;
}
void  HeightMap::computeBTangents()
{
  int x1, x2,
      y1, y2,
      x = 0,
      y = 0,
      i = 0;

  D3DXFROMWINEVECTOR3  neighborsV[6],
           neighborsN[6],
           binormal,
           vector1,
           vector2,
           tangent;

  
  for(y = 0; y < height; y++)
  {
    for(x = 0; x < width; x++)
    {
      i             = y*width + x;

      x1            = clampNS(x - 1, 0, width  - 1);
      x2            = clampNS(x + 1, 0, width  - 1);
      y1            = clampNS(y - 1, 0, height - 1);
      y2            = clampNS(y + 1, 0, height - 1);

      neighborsV[0] = vertices[y1 * width + x ].vertex;
      neighborsV[1] = vertices[y  * width + x1].vertex;
      neighborsV[2] = vertices[y2 * width + x1].vertex;
      neighborsV[3] = vertices[y2 * width + x ].vertex;
      neighborsV[4] = vertices[y  * width + x2].vertex;
      neighborsV[5] = vertices[y1 * width + x2].vertex;

      vector1       = neighborsV[0];
      vector1      -= vertices[i].vertex;
      vector2       = neighborsV[1];
      vector2      -= vertices[i].vertex;
      //neighborsN[0].crossProduct(vector1, vector2);
      D3DXFROMWINEVec3Cross(&neighborsN[0], &vector1, &vector2);

      vector1       = neighborsV[1];
      vector1      -= vertices[i].vertex;
      vector2       = neighborsV[2];
      vector2      -= vertices[i].vertex;
      //neighborsN[1].crossProduct(vector1, vector2);
      D3DXFROMWINEVec3Cross(&neighborsN[1], &vector1, &vector2);

      vector1       = neighborsV[2];
      vector1      -= vertices[i].vertex;
      vector2       = neighborsV[3];
      vector2      -= vertices[i].vertex;
      //neighborsN[2].crossProduct(vector1, vector2);
      D3DXFROMWINEVec3Cross(&neighborsN[2], &vector1, &vector2);

      vector1       = neighborsV[3];
      vector1      -= vertices[i].vertex;
      vector2       = neighborsV[4];
      vector2      -= vertices[i].vertex;
      tangent       = vector2;

      //neighborsN[3].crossProduct(vector1, vector2);
      D3DXFROMWINEVec3Cross(&neighborsN[3], &vector1, &vector2);

      vector1       = neighborsV[4];
      vector1      -= vertices[i].vertex;
      vector2       = neighborsV[5];
      vector2      -= vertices[i].vertex;
      //neighborsN[4].crossProduct(vector1, vector2);
      D3DXFROMWINEVec3Cross(&neighborsN[4], &vector1, &vector2);

      vector1       = neighborsV[5];
      vector1      -= vertices[i].vertex;
      vector2       = neighborsV[0];
      vector2      -= vertices[i].vertex;
      //neighborsN[5].crossProduct(vector1, vector2);
      D3DXFROMWINEVec3Cross(&neighborsN[5], &vector1, &vector2);

      neighborsN[0] += neighborsN[1];
      neighborsN[0] += neighborsN[2];
      neighborsN[0] += neighborsN[3];
      neighborsN[0] += neighborsN[4];
      neighborsN[0] += neighborsN[5];

      //vertices[i].normal = neighborsN[0].normalize();
      D3DXFROMWINEVec3Normalize(&vertices[i].normal, &neighborsN[0]);

      //vertices[i].binormal.crossProduct(tangent, neighborsN[0]);
      //vertices[i].binormal.normalize();
      D3DXFROMWINEVec3Cross(&vertices[i].binormal, &tangent, &neighborsN[0]);
      D3DXFROMWINEVec3Normalize(&vertices[i].binormal, &vertices[i].binormal);

      //vertices[i].tangent.crossProduct(neighborsN[0], vertices[i].binormal);
      //vertices[i].tangent.normalize();
      D3DXFROMWINEVec3Cross(&vertices[i].tangent, &neighborsN[0], &vertices[i].binormal);
      D3DXFROMWINEVec3Normalize(&vertices[i].tangent, &vertices[i].tangent);
  /*
      LOG_PRINT("vertices[%d].vertex.x=%f\n", i, vertices[i].vertex.x);
      LOG_PRINT("vertices[%d].vertex.y=%f\n", i, vertices[i].vertex.y);
      LOG_PRINT("vertices[%d].vertex.z=%f\n", i, vertices[i].vertex.z);
      LOG_PRINT("vertices[%d].normal.x=%f\n", i, vertices[i].normal.x);
      LOG_PRINT("vertices[%d].normal.y=%f\n", i, vertices[i].normal.y);
      LOG_PRINT("vertices[%d].normal.z=%f\n", i, vertices[i].normal.z);
*/
    }
  }
}

bool  HeightMap::buildWeightMap(Image3 & image)
{
  if(!vertices)
    return false;

  ubyte *bytes = new ubyte[width*height*4];
  ZeroMemory(bytes, width*height*4);
#if 0
  image.setComponentsCount(4);
  image.setInternalFormat(FORMAT_RGBA8);
  image.setFormat(FORMAT_RGBA8);
  image.setHeight(256);
  image.setWidth(256);
#else
  image.create(FORMAT_RGBA8, 256, 256, 1, 1);
#endif
  int byteIndex = 0,
	  vIndex    = 0;

  for(int y = 0; y < height; y++)
  {
    for(int x = 0; x < width; x++)
    {
	  vIndex    = y*width + x;
      byteIndex = vIndex*4;

	  bytes[byteIndex + 0] = ubyte((vertices[vIndex].vertex.y/TERRAIN_MAX_HEIGHT)*vertices[vIndex].normal.y*255.0f);
	  bytes[byteIndex + 1] = ubyte((vertices[vIndex].vertex.y/TERRAIN_MAX_HEIGHT)*(1.0f - vertices[vIndex].normal.y)*255.0f);

	  if(vertices[vIndex].vertex.y < 0.0f)
		bytes[byteIndex + 2] = ubyte((vertices[vIndex].vertex.y/-WATER_MIN_HEIGHT)*255.0f);

	}
  }

  image.setPixels(bytes);
  delete[] bytes;
  return true;
}

const HMVertex     *HeightMap::getVertexStream() const { return vertices; }

const int  HeightMap::getIndexCount() const { return indexCount;  }
const int  HeightMap::getHeight()     const { return height;      }
const int  HeightMap::getWidth()      const { return width;       }
