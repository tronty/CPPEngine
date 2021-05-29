#include "TerrainUtils.h"
#include "../SceneFrame.h"

Terrain::Terrain()
{
  occlusionCullCells  =   true;
  frustumCullCells    =   true;
  renderCellsAABBs    =  false;
  alphaToCoverage     =   true;
  alphaReference      =  0.25f;
  alphaBooster        =   1.5f;
  queryClock          =  false;
  wireFrame           =  false;
  drawGrass           =   true;
  drawTerrain           =   true;
  drawTBN             =  false;
  portals             =   0;
  dirty               =  false;

  grassPack=-1;
  fungus=-1;
  weight=-1;
  weightR=-1;
  weightG=-1;
  weightB=-1;
  grass=-1;
  dirt=-1;
  tree1=-1;
  tree2=-1;
  tree3=-1;
  terrainShader=-1,
  grassShader=-1;
  terrainVF=-1,
  grassVF=-1;
}

bool  Terrain::initialize(bool compile)
{
  if(portals)
    return true;

  register int index = 0,
               x     = 0,
               y     = 0;

  Image3        randommap,
               heightmap,
			         coverage,
               watermap;

  //LOG_FNLN;
  std::string fn="/NatureScene/Terrain/watermap.jpg";
  std::string fn2=stx_convertpath(fn);
  watermap.loadImage(fn2.c_str());
  watermap.decompress();

  fn="/NatureScene/Grass/coverage.png";
  fn2=stx_convertpath(fn);
  coverage.loadImage(fn2.c_str());
  coverage.decompress();

  fn="/NatureScene/Terrain/heightmap.png";
  fn2=stx_convertpath(fn);
  heightmap.loadImage(fn2.c_str());
  heightmap.decompress();

  const ubyte *hmBytes      = heightmap.getPixels();
  const ubyte *wmBytes      = watermap.getPixels();
  if(!terrainInfo.setup(&heightmap, &watermap))
    return false;

  createRandomMap(heightmap.getWidth(), heightmap.getHeight(), &randommap);

  portals     = new Portal[CELL_COUNT];
  int     xOf    = 0, yOf = 0,
          xRatio = HEIGHT_MAP_WIDTH/CELL_COLUMN_COUNT,
          yRatio = HEIGHT_MAP_DEPTH/CELL_ROW_COUNT;

  for(y = 0; y < CELL_ROW_COUNT; y++)
  for(x = (CELL_COLUMN_COUNT - 1); x > -1; x--)
  {
    index = y*CELL_COLUMN_COUNT + x;
    portalsManager.push_back(portals + index);

    xOf = (x == (CELL_COLUMN_COUNT - 1)) ? -1 : 0;
    yOf = (y == (CELL_ROW_COUNT    - 1)) ? -1 : 0;

    if(!portals[index].setup(terrainInfo,  D3DXFROMWINEVECTOR2(x*xRatio + xOf, y*yRatio + yOf), &randommap, &coverage))
      { char buf[256];stx_snprintf(buf, 256, "Error setting up portal %d\n", index);LOG_PRINT(buf);return 0;}
  }

  TerrainCell::setupIndices(compile);
  appendToGreenChannel(&watermap, &heightmap);

  dirt = IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Terrain/dirt.png", false, IRenderer::GetRendererInstance()->Getlinear());
  fungus = IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Terrain/fungus.png", false, IRenderer::GetRendererInstance()->Getlinear());

  Image3 watermapR=watermap;
  Channel(&watermapR, &heightmap, 0);
  Image3 watermapG=watermap;
  Channel(&watermapG, &heightmap, 1);
  Image3 watermapB=watermap;
  Channel(&watermapB, &heightmap, 2);

  weight = IRenderer::GetRendererInstance()->addTexture(&watermap, false, IRenderer::GetRendererInstance()->Getlinear());
  weightR = IRenderer::GetRendererInstance()->addTexture(&watermapR, false, IRenderer::GetRendererInstance()->Getlinear());
  weightG = IRenderer::GetRendererInstance()->addTexture(&watermapG, false, IRenderer::GetRendererInstance()->Getlinear());
  weightB = IRenderer::GetRendererInstance()->addTexture(&watermapB, false, IRenderer::GetRendererInstance()->Getlinear());

  grass = IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Terrain/grasslayer.png", false, IRenderer::GetRendererInstance()->Getlinear());

  grassPack = IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Grass/grassPack.png", false, IRenderer::GetRendererInstance()->GetlinearClamp());
  #if 0
  tree1=IRenderer::GetRendererInstance()->addImageLibTexture("/dx8sdk/tree01S.png", false, IRenderer::GetRendererInstance()->GetlinearClamp());
  tree2=IRenderer::GetRendererInstance()->addImageLibTexture("/dx8sdk/tree02S.png", false, IRenderer::GetRendererInstance()->GetlinearClamp());
  tree3=IRenderer::GetRendererInstance()->addImageLibTexture("/dx8sdk/tree35S.png", false, IRenderer::GetRendererInstance()->GetlinearClamp());
  #endif

  terrainShader = IRenderer::GetRendererInstance()->addShaderFromFile("/NatureScene/TerrainShader.shd", "main", "main");
  grassShader = IRenderer::GetRendererInstance()->addShaderFromFile("/NatureScene/GrassShader.shd", "main", "main");
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	terrainVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), terrainShader);}
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
		0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
		0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3
	};
	grassVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), grassShader);}






if(1){
	std::string fn=stx_convertpath("/Framework3/Framework3.xml");
    TiXmlDocument doc(fn.c_str());
    if (!doc.LoadFile())
	{//LOG_PRINT("!doc.LoadFile(%s)\n", fn.c_str());
      stx_exit(0);}
    TiXmlHandle docHandle(&doc);
    std::string Str = docHandle.FirstChild("Body").FirstChild("Debug").Element()->Attribute("Value");
	if(Str!="False")
	{
  drawGrass           =   false;
  drawTerrain           =   false;
	}}
  return true;
}

D3DXFROMWINEVECTOR4 &Terrain::render(Camera3     &camera,
                         const Frustum    &frustum,
                         float elapsedTime )
{
  if(portals)
  {
    GrassCell    *gCell        = 0;
    Portal       *portal       = 0;         
    int           visibleCells = 0,
                  index        = 0,
                  x            = 0,
                  y            = 0;

    if(prevView != camera.getModelViewMatrix())
    {
      const D3DXFROMWINEVECTOR3  viewerPosition = camera.getViewerPosition();
                      prevView       = camera.getModelViewMatrix();

      for(y = 0; y < CELL_ROW_COUNT; y++)
      for(x = 0; x < CELL_COLUMN_COUNT; x++)
      {
        index = y*CELL_COLUMN_COUNT + x;
        portals[index].setVisiblePixelsCount(frustumCullCells ? frustum.AABBInFrustum(portals[index].getAABB()) : true);
        portalsManager[index].set(viewerPosition.getDistance(portals[index].getAABB().getCenterAABB()),
                                  portals + index);
      }

#ifndef __APPLE__
      sort(portalsManager.begin(), portalsManager.end());
#endif

      if(occlusionCullCells)
        queryClock = true;
    }
    else
    {
      if(queryClock)
      {
        for(x = 3; x < CELL_COUNT; x++)
          portalsManager[x].getObject()->endOcclusionQuery();
      }
      queryClock = false;
    }
  }
  dirty = true;

  return renderSimple(elapsedTime);
}

D3DXFROMWINEVECTOR4 &Terrain::renderSimple(float elapsedTime)
{
  info.set(0,0,0,0);
  if(portals)
  {
    TerrainCell    *tCell        = 0;
    GrassCell    *gCell        = 0;
    Portal       *portal       = 0;         
    int           visibleCells = 0,
                  index        = 0,
                  x            = 0,
                  y            = 0;
	
	

  D3DXFROMWINEMATRIX mvp=g_SceneFrame->camera.getModelViewMatrix()*g_SceneFrame->camera.getProjectionMatrix();
  D3DXFROMWINEMatrixTranspose(&mvp, &mvp);

    if(0)//queryClock && dirty)
    {
      for(x = 0; x < CELL_COUNT; x++)
      {
        portalsManager[x].getObject()->startOcclusionQuery();
        visibleCells += (portalsManager[x].getObject()->getVisiblePixelsCount() != 0);
      }
    }
    if(drawTerrain)
    {
      IRenderer::GetRendererInstance()->setShader(terrainShader);
      IRenderer::GetRendererInstance()->setVertexFormat(terrainVF);
      IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", mvp);
      IRenderer::GetRendererInstance()->setTexture("weight", weight);
      IRenderer::GetRendererInstance()->setTexture("fungus", fungus);
      IRenderer::GetRendererInstance()->setTexture("dirt",   dirt);
      IRenderer::GetRendererInstance()->setTexture("grass",  grass);
      for(x = 0; x < CELL_COUNT; x++)
	{
        visibleCells += portalsManager[x].getObject()->render(Portal::TERRAIN) ? 1 : 0;
        tCell = &portalsManager[x].getObject()->getTerrainCell();
          info.x += tCell->getTriangleCount();
	}
    }

    if(drawGrass)
    {
      IRenderer::GetRendererInstance()->setShader(grassShader);
      IRenderer::GetRendererInstance()->setVertexFormat(grassVF);
      IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", mvp);
      IRenderer::GetRendererInstance()->setShaderConstant1f("alphaBooster", alphaBooster);
      IRenderer::GetRendererInstance()->setShaderConstant1f("elapsedTime", timeGetTime()/1000.0f);
      IRenderer::GetRendererInstance()->setTexture("grass", grassPack);
      IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
      IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

    for(x = 0; x < CELL_COUNT; x++)
      {
        gCell = &portalsManager[x].getObject()->getGrassCell();
        if(portalsManager[x].getObject()->render(Portal::GRASS))
          info.x += gCell->getTriangleCount();
      }
    }

    if(renderCellsAABBs)
      for(y = 0; y < CELL_ROW_COUNT; y++)
      for(x = 0; x < CELL_COLUMN_COUNT; x++)
        portals[y*CELL_COLUMN_COUNT + x].render(Portal::AABB);
 
    if(drawTBN)
      terrainInfo.render();

    info.y  = visibleCells*100/CELL_COUNT;
  }

   dirty = false;

  return info;
}

bool  Terrain::appendToGreenChannel(Image3 *dst, const  Image3 *src)
{
  ubyte *hmBytes  = (ubyte*)dst->getPixels(),
          *wmBytes  = (ubyte*)src->getPixels();

  int      hmByteCount  = dst->getComponentsCount(),
           wmByteCount  = src->getComponentsCount(),
           height       = dst->getHeight(),
           width        = dst->getWidth(),
           index        = 0;
  if(!wmBytes || ! hmBytes)
    { /* return */ LOG_PRINT("Null heightmap or watermap content");return 0;}
  if(!(height == src->getHeight()) || !(width == src->getWidth()))
    { /* return */ LOG_PRINT("Watermap has unexpected width or height");return 0;}

  for(int y = 0; y < height; y++)
  {
    for(int x = 0; x < width; x++)
    {
      index                          = (y*width + x);
      hmBytes[index*hmByteCount + 1] = wmBytes[index*wmByteCount];
    }
  }
  return true;
}
bool  Terrain::Channel(Image3 *dst, const  Image3 *src, const unsigned int N)
{
  ubyte *hmBytes  = (ubyte*)dst->getPixels(),
          *wmBytes  = (ubyte*)src->getPixels();

  int      hmByteCount  = dst->getComponentsCount(),
           wmByteCount  = src->getComponentsCount(),
           height       = dst->getHeight(),
           width        = dst->getWidth(),
           index        = 0;
  hmBytes  = (ubyte*)dst->create(src->getFormat(), width, height, 1, 1);

  for(int y = 0; y < height; y++)
  {
    for(int x = 0; x < width; x++)
    {
      index                          = (y*width + x);
      hmBytes[index*N] = wmBytes[index*N];
    }
  }
  return true;
}

const void Terrain::adjustCameraYLocation(Camera3 *camera, float cameraRadius)
{
  D3DXFROMWINEVECTOR3 position   = camera->getViewerPosition();
  float   height     = terrainInfo.getInterpolatedHeight(position),
          difference = (height - position.y + cameraRadius);
  if(difference > 0.1f)
    camera->elevate(difference);
}

void  Terrain::resetRenderingRoutine()
{
  prevView.setIdentity();  
  queryClock = false;   
}

void  Terrain::setOcclusionCullingFlag(bool occCull){ occlusionCullCells = occCull;
                                                      resetRenderingRoutine();     }
void  Terrain::setAlphaToCoverageFlag(bool alpha)   { alphaToCoverage    = alpha;  }
void  Terrain::setFrustumCullingFlag(bool frCull)   { frustumCullCells   = frCull;
                                                      resetRenderingRoutine();     }
void  Terrain::setRenderAABBFlag(bool  rAABBs)      { renderCellsAABBs   = rAABBs; }
void  Terrain::setWireFrameFlag(bool  wFrame)       { wireFrame          = wFrame; }
void  Terrain::setDrawGrassFlag(bool  dGrass)       { drawGrass          = dGrass; }
void  Terrain::setDrawTerrainFlag(bool  dTerrain)   { drawTerrain          = dTerrain; }
void Terrain::setDrawTBNFlag(bool  dTBN)            { drawTBN            = dTBN;   }

void  Terrain::setAlphaReference(float ref)         { alphaReference     = ref;    }
void  Terrain::setAlphaBooster(float boost)         { alphaBooster       = boost;  }

const bool  Terrain::getAlphaToCoverageFlag(){ return alphaToCoverage ;    }
const bool  Terrain::getFrustumCullingFlag() { return frustumCullCells;    }
const bool  Terrain::getRenderAABBFlag()     { return renderCellsAABBs;    }
const bool  Terrain::getWireFrameFlag()      { return wireFrame;           }
const bool  Terrain::getDrawGrassFlag()      { return drawGrass;           }
const bool  Terrain::getDrawTerrainFlag()    { return drawTerrain;         }
const bool  Terrain::getDrawTBNFlag()        { return drawTBN;             }

const float Terrain::getAlphaReference()     { return alphaReference;      }
const float Terrain::getAlphaBooster()       { return alphaBooster;        }

void Terrain::createRandomMap(int width, int height, Image3 *image)
{
  std::string fn=stx_convertpath("/NatureScene/randommap.png");
  image->loadImageLibImage(fn.c_str(), false);
  return;
}

Terrain::~Terrain()
{
  portalsManager.clear();
}

