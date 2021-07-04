#ifndef PLANE_H
#define PLANE_H

class Plane
{
  public:
    Plane()
    {
      normal.set(0,0,0);
      offset = 0;
    }
 
    Plane(const Plane &copy)
    {
      operator=(copy);
    }
#if 0
    Plane &operator= (const Plane &copy)
    {
      normal = copy.normal;
      offset = copy.offset
      return *this;
    }
#endif
    void set(const D3DXFROMWINEVECTOR3 &p1,
             const D3DXFROMWINEVECTOR3 &p2, 
             const D3DXFROMWINEVECTOR3 &p3)
    {
      static D3DXFROMWINEVECTOR3 v1, v2;
      v1 = p2;
      v2 = p3;

      v1 -= p1;
      v2 -= p1;
      normal.crossProduct(v1, v2);
      normal.normalize();
      offset = -normal.getDotProduct(p1);
   }

   void replaceOffset(const D3DXFROMWINEVECTOR3 &p1)
   {
     offset = -normal.getDotProduct(p1);
   }

   const D3DXFROMWINEVECTOR3 &getNormal() const { return normal; }
   const float      getOffset() const { return offset; }

  private:
    D3DXFROMWINEVECTOR3 normal;
    float         offset;
};

typedef Plane    Planei;
typedef Plane  Planef;
typedef Plane Planed;

#endif
