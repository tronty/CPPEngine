#ifndef PLANE_H

#define PLANE_H



#include <Framework3/IRenderer.h>

//template <class T>

class Plane

{

  public:

    Plane()

    {

      normal=D3DXFROMWINEVECTOR3(0,0,0);

      offset = 0;

    }



    Plane(const Plane &copy)

    {

      operator=(copy);

    }



    Plane &operator= (const Plane &copy)

    {

      normal = copy.normal;

      offset = copy.offset;

      return *this;

    }



    void set(const /*Tuple3<T>*/D3DXFROMWINEVECTOR3 &p1,

             const /*Tuple3<T>*/D3DXFROMWINEVECTOR3 &p2,

             const /*Tuple3<T>*/D3DXFROMWINEVECTOR3 &p3)

    {

      static /*Tuple3<T>*/D3DXFROMWINEVECTOR3 v1, v2;

      v1 = p2;

      v2 = p3;



      v1 -= p1;

      v2 -= p1;

      //normal=cross(v1, v2);

      //normal=normalize(normal);

      //offset = dot((-normal),p1);


      D3DXFROMWINEVec3Cross(&normal, &v1, &v2);
      D3DXFROMWINEVec3Normalize(&normal, &normal);
      normal=-normal;
      offset=D3DXFROMWINEVec3Dot(&normal,&p1);

   }



   void replaceOffset(const /*Tuple3<T>*/D3DXFROMWINEVECTOR3 &p1)

   {

      //offset = dot((-normal),p1);
	normal=-normal;
      offset=D3DXFROMWINEVec3Dot(&normal,&p1);

   }



   const /*Tuple3<T>*/D3DXFROMWINEVECTOR3 &getNormal() const { return normal; }

   const float      getOffset() const { return offset; }



  private:

    /*Tuple3<T>*/D3DXFROMWINEVECTOR3 normal;

    float         offset;

};



typedef Plane/*<int>*/    Planei;//???

typedef Plane/*<float>*/  Planef;//???

typedef Plane/*<double>*/ Planed;//???



#endif

