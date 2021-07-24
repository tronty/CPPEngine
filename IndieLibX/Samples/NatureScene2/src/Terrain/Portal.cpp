#include "TerrainUtils.h"

Portal::Portal()
{
  visiblePixelsCount = 1;
  occlusionID        = 0;
}

Portal::~Portal()
{
  //if(occlusionID)
    //glDeleteQueriesARB(1, &occlusionID);
}

bool Portal::setup(const HeightMap &hMap,
                   const D3DXFROMWINEVECTOR2   &offset,
                   const Image3     *random,
			       const Image3     *coverage)
{

  //LOG_FNLN;
  //LOG_PRINT("coverage=%x\n", coverage);
  //LOG_PRINT("random=%x\n", random);
  bool tInfo = tCell.setup(hMap, offset);
  bool gInfo = gCell.setup(hMap, offset, random, coverage);

  const D3DXFROMWINEVECTOR2 &vSpan0 = tCell.getVerticalBounds(),
                &vSpan1 = gCell.getVerticalBounds();
  float minHeight       = vSpan0.x < vSpan1.x ? vSpan0.x : vSpan1.x,
        maxHeight       = vSpan0.y > vSpan1.y ? vSpan0.y : vSpan1.y;

  aabb.computeBounds(float(-TERRAIN_X_OFFSET + offset.x)*TERRAIN_WIDTH_SCALE,
                     minHeight,
                     float(-TERRAIN_Z_OFFSET + offset.y)*TERRAIN_DEPTH_SCALE,
                     float(-TERRAIN_X_OFFSET - 1 + offset.x + TILE_COLUMN_COUNT)*TERRAIN_WIDTH_SCALE,
                     maxHeight,
                     float(-TERRAIN_Z_OFFSET - 1 + offset.y + TILE_ROW_COUNT)*TERRAIN_DEPTH_SCALE);                   

  if(tInfo && gInfo)
  {
    //glGenQueriesARB(1, &occlusionID);
    return true;
  }
             
  return false;
}

const bool Portal::render(int token)
{
  if(visiblePixelsCount)
  {
    if(token & GRASS)
      gCell.render();

    if((token & OCCLUSIONS) || (token & TERRAIN))
      tCell.render();

    if(token & AABB)
      aabb.render(BoundsDescriptor::WIRE | BoundsDescriptor::AABB);
  }

  return (visiblePixelsCount != 0);
}

const bool Portal::startOcclusionQuery()
{
  if(!occlusionID || !visiblePixelsCount)
    return false;

  //glBeginQueryARB(GL_SAMPLES_PASSED_ARB, occlusionID);
  render(OCCLUSIONS);
  //glEndQueryARB(GL_SAMPLES_PASSED_ARB);
  return true;
}

const bool Portal::endOcclusionQuery()
{
  int available = 0;

  if(!occlusionID || !visiblePixelsCount)
    return false;

  //while(!available)
    //glGetQueryObjectivARB(occlusionID, GL_QUERY_RESULT_AVAILABLE_ARB, &available);

  //glGetQueryObjectuivARB(occlusionID, GL_QUERY_RESULT_ARB, &visiblePixelsCount);
  setVisiblePixelsCount(visiblePixelsCount);

  return true;
}

const unsigned int Portal::getOcclusionID() const
{
  return occlusionID;
}

void Portal::setVisiblePixelsCount(unsigned int visible)
{
  visiblePixelsCount = visible;
}

const unsigned int Portal::getVisiblePixelsCount() const
{
  return visiblePixelsCount;
}

const BoundsDescriptor &Portal::getAABB()  const{ return aabb; }
TerrainCell            &Portal::getTerrainCell(){ return tCell; }
GrassCell              &Portal::getGrassCell()  { return gCell; }
