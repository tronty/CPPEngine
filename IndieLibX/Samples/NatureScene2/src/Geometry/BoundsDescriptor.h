#ifndef BOUNDSDESCRIPTOR_H
#define BOUNDSDESCRIPTOR_H

#include <Framework3/IRenderer.h>
#include "Plane.h"

class BoundsDescriptor
{
  public:

    BoundsDescriptor();
    BoundsDescriptor(const BoundsDescriptor &copy);
    BoundsDescriptor &operator =(const BoundsDescriptor &copy);

    void operator*=(const D3DXFROMWINEMATRIX &matrix);
    void operator+=(const BoundsDescriptor &copy);

    BoundsDescriptor  operator *(const D3DXFROMWINEMATRIX &matrix);
    BoundsDescriptor  operator +(const BoundsDescriptor &copy);

    const bool  frustumOverlapsAABB(const Planef *frustumPlane, int planeCount) const;
    const bool  sphereOverlapsAABB(const D3DXFROMWINEVECTOR3 &center, float radius)   const;
    const bool  sphereOverlapsAABB(const D3DXFROMWINEVECTOR4 &centerAndRad)           const;
    const bool  sphereOverlapsOBB(const D3DXFROMWINEVECTOR3 &center, float radius)    const;
    const bool  sphereOverlapsOBB(const D3DXFROMWINEVECTOR4 &centerAndRad)            const;
    const bool  lineOverlapsOBB(const D3DXFROMWINEVECTOR3& start, const D3DXFROMWINEVECTOR3 &end) const;
    bool  isInitialized();

    const void  render(int info) const;
    void  reset();

    void  computeBounds(const D3DXFROMWINEVECTOR3 &minEnd, const D3DXFROMWINEVECTOR3 &maxEnd);
    void  computeBounds(float minX, float minY, float minZ, 
                        float maxX, float maxY, float maxZ);
    
    const D3DXFROMWINEVECTOR3 &getMinEndAABB()         const;
    const D3DXFROMWINEVECTOR3 &getMaxEndAABB()         const;
    const D3DXFROMWINEVECTOR3 &getCenterAABB()         const;
    const D3DXFROMWINEVECTOR3 &getVertexOBB(int index) const;
    const D3DXFROMWINEVECTOR3 &getCenterOBB()          const;
    const D3DXFROMWINEVECTOR3 &getExtents()            const;
    const D3DXFROMWINEVECTOR3 &getAxis(int index)      const;

    enum BRenderType {
      WIRE = 0x01,
      FILL = 0x02,
      AABB = 0x04,
      OBB  = 0x08
    };

  private:
    static unsigned short indicesFill[36],
                          wire[24];

    void  computeAxiiAndExtents(); 
    void  computeAABBEnds();

    D3DXFROMWINEVECTOR3 verticesOBB[8],
            minEndAABB,
            maxEndAABB,
            centerAABB,
            centerOBB,
            extents,
            axii[3];

    bool    initialized;
    int     type;
};

#endif
