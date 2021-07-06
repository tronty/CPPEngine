#include "../Geometry/BoundsDescriptor.h"
#include <math.h>
 

#ifndef FRUSTUM_H
#define FRUSTUM_H

#define RIGHT	  0		// The RIGHT side of the frustum
#define LEFT	  1		// The LEFT	 side of the frustum
#define BOTTOM  2	  // The BOTTOM side of the frustum
#define TOP		  3		// The TOP side of the frustum
#define BACK	  4		// The BACK	side of the frustum
#define FRONT	  5	  // The FRONT side of the frustum
#define A       0		// The X value of the plane's normal
#define B       1		// The Y value of the plane's normal
#define C       2	  // The Z value of the plane's normal
#define D       3		// The distance the plane is from the origin
 
class Frustum
{
  public:
	  void  update();
    const bool  AABBInFrustum(const BoundsDescriptor &aabb) const;
    const float getDistanceToPlane(int plane, const D3DXFROMWINEVECTOR3 &center);

	  const bool pointInFrustum(float x, float y, float z);
    const bool pointInFrustum(const D3DXFROMWINEVECTOR3 & center);

	  const bool sphereInFrustum(const D3DXFROMWINEVECTOR3 &center, float radius);
	  const bool sphereInFrustum(float x, float y, float z, float radius);

  private:
    D3DXFROMWINEVECTOR4   frustumPlane[6];
};

#endif 
