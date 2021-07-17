#ifndef TRIANGLE_H

#define TRIANGLE_H

#include <Framework3/IRenderer.h>

#include "Plane.h"



class TriangleNS

{

  public:

    TriangleNS( D3DXFROMWINEVECTOR3 *t1 = 0,

              D3DXFROMWINEVECTOR3 *t2 = 0,

              D3DXFROMWINEVECTOR3 *t3 = 0);



    void setFacingPoint(const D3DXFROMWINEVECTOR3 &point);

    void addToNormals();

    void resetNormals();



    void setVertices( D3DXFROMWINEVECTOR3 *t1,

                      D3DXFROMWINEVECTOR3 *t2,

                      D3DXFROMWINEVECTOR3 *t3);



    void setNormals ( D3DXFROMWINEVECTOR3 *n1,

                      D3DXFROMWINEVECTOR3 *n2,

                      D3DXFROMWINEVECTOR3 *n3);



    D3DXFROMWINEVECTOR3 *vertices[3],

            *normals[3];

    Planef   plane;

    bool     facingLight;



};



#endif

