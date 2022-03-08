#include "Terrain.h"
#include "../SceneFrame.h"
//using namespace NatureScene;

Terrain::Terrain()
{
  occlusionCullCells  =   false;
  frustumCullCells    =   false;
  renderCellsAABBs    =   false;
  alphaToCoverage     =   false;
  alphaReference      =   0.25f;
  alphaBooster        =   1.5f;
  queryClock          =   false;
  wireFrame           =   false;
  drawGrass           =   true;
  drawTerrain         =   true;
  drawTBN             =   false;
  portals             =   0;
  dirty               =   false;

  fungus=-1;
  weight=-1;
  dirt=-1;

  terrainShader=-1;
  grassShader=-1;
}

bool  Terrain::initialize(bool compile)
{
  LOG_FNLN;
  if(portals)
    return true;

  LOG_FNLN;
  //if(!GLEE_ARB_vertex_buffer_object)
    //return LOG_PRINT("This demo needs the GL_ARB_vertex_buffer_object extension to run");

  		int index = 0,
               x     = 0,
               y     = 0;

  Image3        randommap,
               heightmap,
			         coverage,
               watermap;
#if 0
  if(!watermap.load("watermap.jpg"))
    return LOG_PRINT("Invalid watermap image");

  if(!coverage.load("coverage.png"))
    return LOG_PRINT("Invalid coverage image");

  if(!heightmap.load("heightmap.jpg"))
    return LOG_PRINT("Invalid heightmap image");
#else

  LOG_FNLN;
	std::string name="/NatureScene/Terrain/watermap.png";
	std::string n2=stx_convertpath(name);
  if(!watermap.loadImageLibImage(n2.c_str(),false)){
    LOG_PRINT("Invalid watermap image\n");return;}
	name="/NatureScene/Grass/coverage.png";
n2=stx_convertpath(name);
  LOG_FNLN;
 if(!coverage.loadImageLibImage(n2.c_str(),false)){
    LOG_PRINT("Invalid coverage image\n");return;}
	name="/NatureScene/Terrain/heightmap.png";
n2=stx_convertpath(name);
  LOG_FNLN;
  if(!heightmap.loadImageLibImage(n2.c_str(),false)){
    LOG_PRINT("Invalid heightmap image\n");return;}
#endif
  
  LOG_FNLN;
  if(!terrainInfo.setup(heightmap, watermap))
    return false;

  LOG_FNLN;
  createRandomMap(heightmap.getWidth(), heightmap.getHeight(), randommap);

  LOG_FNLN;
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

    if(!portals[index].setup(terrainInfo,  Tuple2i(x*xRatio + xOf, y*yRatio + yOf), randommap, coverage,terrainShader,grassShader))
    {
      LOG_PRINT("Error setting up portal %d\n", index);
	return;
	}
  }
  LOG_FNLN;
  TerrainCell::setupIndices(compile);
  appendToGreenChannel(watermap, heightmap);
#if 0
  dirt.load2D("dirt.png", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true); 
  dirt.setAnisoLevel(2);

  fungus.load2D("fungus.png", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true); 
  fungus.setAnisoLevel(2);

  grass.load2D("grasslayer.png", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true); 
  grass.setAnisoLevel(2);

  weight.load2DImage(watermap, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true); 
  weight.setAnisoLevel(2);

  grassPack.load2D("grassPack.png", GL_REPEAT, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);
  grassPack.setAnisoLevel(4);

  terrainShader.loadXMLSettings("TerrainShader.xml");
  grassShader.loadXMLSettings("GrassShader.xml");
#else
  dirt = IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Terrain/dirt.png", false, IRenderer::GetRendererInstance()->Getlinear());
  fungus = IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Terrain/fungus.png", false, IRenderer::GetRendererInstance()->Getlinear());
  weight = IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Terrain/watermap.png", false, IRenderer::GetRendererInstance()->Getlinear());

  LOG_PRINT("addImageLibTexture:/NatureScene/Terrain/dirt.png:%x\n", dirt);
  LOG_PRINT("addImageLibTexture:/NatureScene/Terrain/fungus.png:%x\n", fungus);
  LOG_PRINT("addImageLibTexture:/NatureScene/Terrain/watermap.png:%x\n", weight);

  terrainShader = IRenderer::GetRendererInstance()->addShaderFromFile("/NatureScene/TerrainShader.hlsl", "main", "main");
  grassShader = IRenderer::GetRendererInstance()->addShaderFromFile("/NatureScene/GrassShader.hlsl", "main", "main");

	//
	//LOG_PRINT("grassShader = %x\n", grassShader);

#endif
  return true;
}
/*
bool Terrain::portalsCompstd(const LPPortal & s0, const LPPortal &s1) const
{
       int i=s0.getDistance() < s1.getDistance();
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
}
*/
Tuple4i &Terrain::render(//const Camera     &camera,
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

    
    D3DXFROMWINEMATRIX mv=g_SceneFrame->GetCamera().getModelViewMatrix();
    if(prevView != mv)//camera.getModelViewMatrix())
    {
      //const D3DXFROMWINEVECTOR3  &viewerPosition = camera.getViewerPosition();
	const D3DXFROMWINEVECTOR3  &viewerPosition = IRenderer::GetRendererInstance()->GetEyePt();
                      //prevView       = camera.getModelViewMatrix();
		prevView	     = mv;

      for(y = 0; y < CELL_ROW_COUNT; y++)
      for(x = 0; x < CELL_COLUMN_COUNT; x++)
      {
        index = y*CELL_COLUMN_COUNT + x;
        portals[index].setVisiblePixelsCount(frustumCullCells ? frustum.AABBInFrustum(portals[index].getAABB()) : true);
	float dis=getDistance(viewerPosition,portals[index].getAABB().getCenterAABB());
        portalsManager[index].set(dis,//viewerPosition.getDistance(portals[index].getAABB().getCenterAABB()),
                                  portals + index);
      }

	#if 0//ndef __APPLE__   
	if(portalsManager.size()>1) std::sort(portalsManager.begin(), portalsManager.end(), portalsCompstd);
	#endif

      if(occlusionCullCells)// && GLEE_ARB_occlusion_query)
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

Tuple4i &Terrain::renderSimple(float elapsedTime)
{
  info.set(0,0,0,0);
  if(portals)
  {
    GrassCell    *gCell        = 0;
    Portal       *portal       = 0;         
    int           visibleCells = 0,
                  index        = 0,
                  x            = 0,
                  y            = 0;
 #if 0
    glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
 
    terrainShader.activate();
    weight.activate(0);
    fungus.activate(1);
    dirt.activate(2);
    grass.activate(3);
#else
#if 0
	
	LOG_PRINT("terrainShader = %x\n", terrainShader);
	LOG_PRINT("weight = %x\n", weight);
	LOG_PRINT("fungus = %x\n", fungus);
	LOG_PRINT("dirt = %x\n", dirt);
	LOG_PRINT("grass = %x\n", grass);

    IRenderer::GetRendererInstance()->setShader(terrainShader);
    
	//D3DXFROMWINEMATRIX matWorld=IRenderer::GetRendererInstance()->Getww();
	D3DXFROMWINEMATRIX matView=IRenderer::GetRendererInstance()->Getvv();
	D3DXFROMWINEMATRIX matProj=IRenderer::GetRendererInstance()->Getpp();
  	D3DXFROMWINEMATRIX matVP = /* matWorld* */ matProj*matView;
	float determinant=0.0f;
	D3DXFROMWINEMatrixInverse(&matVP, &determinant, &matVP);
  	D3DXFROMWINEMatrixTranspose(&matVP, &matVP);
	//D3DXFROMWINEMatrixIdentity(&matVP);


	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matVP);
    IRenderer::GetRendererInstance()->setTexture("weight", weight);
    IRenderer::GetRendererInstance()->setTexture("fungus", fungus);
    IRenderer::GetRendererInstance()->setTexture("dirt", dirt);
    IRenderer::GetRendererInstance()->setTexture("grass", g_SceneFrame->getGrassTexture());
#endif
#endif
    if(queryClock && dirty)
    {
      for(x = 0; x < CELL_COUNT; x++)
      {
        portalsManager[x].getObject()->startOcclusionQuery();
        visibleCells += (portalsManager[x].getObject()->getVisiblePixelsCount() != 0);
      }
    }
    else
      for(x = 0; x < CELL_COUNT; x++)
        visibleCells += portalsManager[x].getObject()->render(Portal::TERRAIN,terrainShader,grassShader) ? 1 : 0;
 #if 0
    grass.deactivate();
    dirt.deactivate();
    fungus.deactivate();
    weight.deactivate();
    terrainShader.deactivate();
#endif

    if(drawGrass)
    {
#if 0
      if(alphaToCoverage)
      {
        glEnable(GL_MULTISAMPLE_ARB);
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
      }

      glEnable(GL_ALPHA_TEST);
      glDisable(GL_CULL_FACE);

      glAlphaFunc(GL_GEQUAL, alphaReference);

      grassShader.updateElementsf("alphaBooster", 1, &alphaBooster);
      grassShader.updateElementsf("elapsedTime",  1, &elapsedTime);
      grassShader.activate();

      grassPack.activate();
#else
  //LOG_PRINT("grassShader = %x\n", grassShader);
  IRenderer::GetRendererInstance()->setShader(grassShader);

  D3DXFROMWINEMATRIX matVP = g_SceneFrame->GetCamera().getModelViewMatrix()*g_SceneFrame->GetProj();
  
  IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matVP);
  IRenderer::GetRendererInstance()->setShaderConstant1f("alphaBooster", alphaBooster);
  IRenderer::GetRendererInstance()->setShaderConstant1f("elapsedTime", timeGetTime()/1000.0f);
  IRenderer::GetRendererInstance()->setTexture("grass", g_SceneFrame->getGrassPackTexture());
#endif
      for(x = 0; x < CELL_COUNT; x++)
      {
        gCell = &portalsManager[x].getObject()->getGrassCell();
        if(portalsManager[x].getObject()->render(Portal::GRASS,terrainShader,grassShader))
          info.x += gCell->getTriangleCount();
      }
/*
      if(alphaToCoverage)
      {
        glDisable(GL_MULTISAMPLE_ARB);
        glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
      }

      grassPack.deactivate();
      grassShader.deactivate();

      glEnable(GL_CULL_FACE);
      glDisable(GL_ALPHA_TEST);
*/
    }

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if(renderCellsAABBs)
      for(y = 0; y < CELL_ROW_COUNT; y++)
      for(x = 0; x < CELL_COLUMN_COUNT; x++)
        portals[y*CELL_COLUMN_COUNT + x].render(Portal::AABB,terrainShader,grassShader);
 
    if(drawTBN)
      terrainInfo.render();

    info.x += (TILE_COLUMN_COUNT - 1)*TILE_ROW_COUNT*visibleCells;
    info.y  = visibleCells*100/CELL_COUNT;
  }

   dirty = false;

  return info;
}

bool  Terrain::appendToGreenChannel(Image3 &dst, const  Image3 &src)
{
  ubyte *hmBytes  = (ubyte*)dst.getPixels(),
          *wmBytes  = (ubyte*)src.getPixels();

  int      hmByteCount  = getChannelCount(dst.getFormat()),
           wmByteCount  = getChannelCount(src.getFormat()),
           height       = dst.getHeight(),
           width        = dst.getWidth(),
           index        = 0;

  if(!wmBytes || ! hmBytes){
    LOG_PRINT("Null heightmap or watermap content\n");return;}

  if(!(height == src.getHeight()) || !(width == src.getWidth())){
    LOG_PRINT("Watermap has unexpected width or height\n");return;}

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

const void Terrain::adjustCameraYLocation(NSCamera &camera, float cameraRadius)
{
  D3DXFROMWINEVECTOR3 position   = IRenderer::GetRendererInstance()->GetEyePt();//camera.getViewerPosition();
  float   height     = terrainInfo.getInterpolatedHeight(position),
          difference = (height - position.y + cameraRadius);
  if(difference > 0.1f)
    camera.elevate(difference);
}

void  Terrain::resetRenderingRoutine()
{
  //prevView.setIdentity();
	prevView=identity4();  
  queryClock = false;   
}

void Terrain::clearFlags()
{
	occlusionCullCells = false;
	alphaToCoverage    = false;
	frustumCullCells   = false;
	renderCellsAABBs   = false;
	wireFrame          = false;
	drawGrass          = false;
	drawTerrain        = false;
	drawTBN            = false;
}

void  Terrain::setOcclusionCullingFlag(bool occCull){ occlusionCullCells = occCull;
                                                      resetRenderingRoutine();     }
void  Terrain::setAlphaToCoverageFlag(bool alpha)   { alphaToCoverage    = alpha;  }
void  Terrain::setFrustumCullingFlag(bool frCull)   { frustumCullCells   = frCull;
                                                      resetRenderingRoutine();     }
void  Terrain::setRenderAABBFlag(bool  rAABBs)      { renderCellsAABBs   = rAABBs; }
void  Terrain::setWireFrameFlag(bool  wFrame)       { wireFrame          = wFrame; }
void  Terrain::setDrawGrassFlag(bool  dGrass)       { drawGrass          = dGrass; }
void  Terrain::setDrawTerrainFlag(bool  dTerrain)       { drawTerrain          = dTerrain; }
void  Terrain::setDrawTBNFlag(bool  dTBN)           { drawTBN            = dTBN;   }

void  Terrain::setAlphaReference(float ref)         { alphaReference     = ref;    }
void  Terrain::setAlphaBooster(float boost)         { alphaBooster       = boost;  }

const bool  Terrain::getAlphaToCoverageFlag(){ return alphaToCoverage ;    }
const bool  Terrain::getFrustumCullingFlag() { return frustumCullCells;    }
const bool  Terrain::getRenderAABBFlag()     { return renderCellsAABBs;    }
const bool  Terrain::getWireFrameFlag()      { return wireFrame;           }
const bool  Terrain::getDrawGrassFlag()      { return drawGrass;           }
const bool  Terrain::getDrawTerrainFlag()      { return drawTerrain;           }
const bool  Terrain::getDrawTBNFlag()        { return drawTBN;             }

const float Terrain::getAlphaReference()     { return alphaReference;      }
const float Terrain::getAlphaBooster()       { return alphaBooster;        }

void Terrain::createRandomMap(int width, int height, Image3 &image)
{
  LOG_FNLN;
  //const char* verified = MediaPathManager::lookUpMediaPath("/NatureScene/randommap.png");

  if(1)
  {
  LOG_FNLN;
	std::string name="/NatureScene/randommap.png";
	//LOG_PRINT("verified=%s\n", name.c_str());
    //image.load(verified);
	std::string n2=stx_convertpath(name);
	image.loadImageLibImage(n2.c_str(),false);
  LOG_FNLN;
    return;
  }

  LOG_FNLN;
#if 0
	ofstream randommap;

  randommap.open("/NatureScene/randommap.png", ios::binary);

	ubyte	 TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
           infoHeader[6];

  
  Tuple4<unsigned char> *data = new Tuple4<unsigned char>[width*height];

  for(int y = 0, index = 0; y < height; y++)
  for(int x = 0; x < width ; x++, index++)
  {
    data[index].set(unsigned char(getNextRandom()*255.0f),
                    unsigned char(getNextRandom()*255.0f),
                    unsigned char(getNextRandom()*255.0f),
                    unsigned char(getNextRandom()*255.0f));
  }

	randommap.write((char*)TGAheader, sizeof(TGAheader));

	infoHeader[0] = (width  & 0x00FF);
	infoHeader[1] = (width  & 0xFF00) >> 8;
	infoHeader[2] = (height & 0x00FF);
	infoHeader[3] = (height & 0xFF00) >> 8;
	infoHeader[4] = 32;
	infoHeader[5] = 0;

	randommap.write((char*)infoHeader, sizeof(infoHeader));
	randommap.write((char*)data, 4*width*height);
	randommap.close();

  image.setComponentsCount(4);
  image.setInternalFormat(FORMAT_RGBA8);
  image.setFormat(FORMAT_RGBA8);
  image.setHeight(height);
  image.setWidth(width);
  image.setDataBuffer(&data[0].x);
 
  deleteArray(data);
#else

  LOG_FNLN;
int height7	 = height;
int width7	 = width;
ofstream randommap;
  randommap.open("/NatureScene/randommap.png", ios::binary);

	unsigned char	 TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	   infoHeader[6];

  unsigned char *data = new unsigned char[width7*height7*4];
  int y = 0, index = 0;

  LOG_FNLN;
  for(y = 0, index = 0; y < height7; y++)
  for(int x = 0; x < width7 ; x++, index++)
  {
    unsigned char c1=int(float(stx_rand())/(RAND_MAX+1)*255.0f);
    unsigned char c2=int(float(stx_rand())/(RAND_MAX+1)*255.0f);
    unsigned char c3=int(float(stx_rand())/(RAND_MAX+1)*255.0f);
    unsigned char c4=int(float(stx_rand())/(RAND_MAX+1)*255.0f);
	  data[index*4+0]= c1;
	  data[index*4+1]= c2;
	  data[index*4+2]= c3;
	  data[index*4+3]= c4;
  }

	randommap.write((char*)TGAheader, sizeof(TGAheader));
	infoHeader[0] = (width7  & 0x00FF);
	infoHeader[1] = (width7  & 0xFF00) >> 8;
	infoHeader[2] = (height7 & 0x00FF);
	infoHeader[3] = (height7 & 0xFF00) >> 8;
	infoHeader[4] = 32;
	infoHeader[5] = 0;
	randommap.write((char*)infoHeader, sizeof(infoHeader));
	randommap.write((char*)data, 4*width7*height7);
	randommap.close();

  ubyte *fDest = image.create(FORMAT_RGBA8, width7, height7, 1, 1);

	stx_memcpy(fDest,data,index*4*sizeof(unsigned char));

  delete[] data;

  LOG_FNLN;
#endif
}

Terrain::~Terrain()
{
  deleteArray(portals);
  portalsManager.clear();

  //terrainShader.destroy();
  //grassShader.destroy();

}
