#include "Triangle.h"



TriangleNS::TriangleNS( D3DXFROMWINEVECTOR3 *t1,  D3DXFROMWINEVECTOR3 *t2,  D3DXFROMWINEVECTOR3 *t3)

{

  setVertices(t1, t2, t3);

}



void TriangleNS::setVertices( D3DXFROMWINEVECTOR3 *t1,  D3DXFROMWINEVECTOR3 *t2,  D3DXFROMWINEVECTOR3 *t3)

{

  vertices[0] = t1;

  vertices[1] = t2;

  vertices[2] = t3;

  if(t1 && t2 && t3)

    plane.set(*t1, *t2, *t3);

}



void TriangleNS::setNormals( D3DXFROMWINEVECTOR3 *n1,  D3DXFROMWINEVECTOR3 *n2,  D3DXFROMWINEVECTOR3 *n3)

{

  normals[0] = n1;

  normals[1] = n2;

  normals[2] = n3;

}



void TriangleNS::resetNormals()

{

  if(vertices[0] && vertices[1] && vertices[2])

    plane.set(*vertices[0], *vertices[1], *vertices[2]);



  if(normals[0] && normals[1] && normals[2])

  {

    (*normals[0])=D3DXFROMWINEVECTOR3(0,0,0);

    (*normals[1])=D3DXFROMWINEVECTOR3(0,0,0);

    (*normals[2])=D3DXFROMWINEVECTOR3(0,0,0);

  }

}



void TriangleNS::addToNormals()

{

  if(normals[0] && normals[1] && normals[2])

  {

    *normals[0] += plane.getNormal();

    *normals[1] += plane.getNormal();

    *normals[2] += plane.getNormal();

  }

}



void TriangleNS::setFacingPoint(const D3DXFROMWINEVECTOR3 &point)

{

  facingLight = (dot(plane.getNormal(),point) + plane.getOffset() >= 0.0f);

}

