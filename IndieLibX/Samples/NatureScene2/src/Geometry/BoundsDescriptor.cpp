#include "BoundsDescriptor.h"

//include "../Renderer/Renderer.h"



unsigned short BoundsDescriptor::indicesFill[36] = {0, 4, 7, 7, 3, 0,

						    3, 7, 6, 6, 2, 3,

						    2, 6, 5, 5, 1, 2,

						    1, 5, 0, 0, 5, 4,

						    4, 5, 7, 7, 5, 6,

						    0, 3, 2, 2, 1, 0};



BoundsDescriptor::BoundsDescriptor()

{

  initialized = false;

}



BoundsDescriptor:: BoundsDescriptor(const BoundsDescriptor &copy)

{

  initialized = false;

  operator =(copy);

}



BoundsDescriptor &BoundsDescriptor::operator=(const BoundsDescriptor &copy)

{

  if(this != &copy)

  {

    initialized = copy.initialized;

    minEndAABB	= copy.minEndAABB;

    maxEndAABB	= copy.maxEndAABB;

    centerAABB	= copy.centerAABB;

    centerOBB	= copy.centerOBB;

    extents	= copy.extents;

    stx_memcpy(axii       , copy.axii	, sizeof(D3DXFROMWINEVECTOR3)*3);

    stx_memcpy(verticesOBB, copy.verticesOBB, sizeof(D3DXFROMWINEVECTOR3)*8);

  }

  return *this;

}



void BoundsDescriptor::reset()

{

  stx_memset(verticesOBB, 0, sizeof(D3DXFROMWINEVECTOR3)*8);

  initialized = false;

  minEndAABB=D3DXFROMWINEVECTOR3(0,0,0);

  maxEndAABB=D3DXFROMWINEVECTOR3(0,0,0);

  centerAABB=D3DXFROMWINEVECTOR3(0,0,0);

  centerOBB=D3DXFROMWINEVECTOR3(0,0,0);

  axii[0]=D3DXFROMWINEVECTOR3(0,0,0);

  axii[1]=D3DXFROMWINEVECTOR3(0,0,0);

  axii[2]=D3DXFROMWINEVECTOR3(0,0,0);

  extents=D3DXFROMWINEVECTOR3(0,0,0);

}



void BoundsDescriptor::operator+=(const BoundsDescriptor &bounds)

{

  D3DXFROMWINEVECTOR3 min,

	  max;

  min.x = (minEndAABB.x < bounds.minEndAABB.x) ? minEndAABB.x : bounds.minEndAABB.x;

  min.y = (minEndAABB.y < bounds.minEndAABB.y) ? minEndAABB.y : bounds.minEndAABB.y;

  min.z = (minEndAABB.z < bounds.minEndAABB.z) ? minEndAABB.z : bounds.minEndAABB.z;



  max.x = (maxEndAABB.x > bounds.maxEndAABB.x) ? maxEndAABB.x : bounds.maxEndAABB.x;

  max.y = (maxEndAABB.y > bounds.maxEndAABB.y) ? maxEndAABB.y : bounds.maxEndAABB.y;

  max.z = (maxEndAABB.z > bounds.maxEndAABB.z) ? maxEndAABB.z : bounds.maxEndAABB.z;



  computeBounds(min, max);

}



BoundsDescriptor BoundsDescriptor::operator+(const BoundsDescriptor &bounds)

{

  BoundsDescriptor descriptor;

  D3DXFROMWINEVECTOR3 min,

	  max;



  min.x = (minEndAABB.x < bounds.minEndAABB.x) ? minEndAABB.x : bounds.minEndAABB.x;

  min.y = (minEndAABB.y < bounds.minEndAABB.y) ? minEndAABB.y : bounds.minEndAABB.y;

  min.z = (minEndAABB.z < bounds.minEndAABB.z) ? minEndAABB.z : bounds.minEndAABB.z;



  max.x = (maxEndAABB.x > bounds.maxEndAABB.x) ? maxEndAABB.x : bounds.maxEndAABB.x;

  max.y = (maxEndAABB.y > bounds.maxEndAABB.y) ? maxEndAABB.y : bounds.maxEndAABB.y;

  max.z = (maxEndAABB.z > bounds.maxEndAABB.z) ? maxEndAABB.z : bounds.maxEndAABB.z;



  descriptor.computeBounds(min, max);

  return descriptor;

}



void BoundsDescriptor::operator*=(const D3DXFROMWINEMATRIX &matrix)

{

  if(initialized)

  {
#if 0
	transformVector(verticesOBB[0],matrix,verticesOBB[0]);

	transformVector(verticesOBB[1],matrix,verticesOBB[1]);

	transformVector(verticesOBB[2],matrix,verticesOBB[2]);

	transformVector(verticesOBB[3],matrix,verticesOBB[3]);

	transformVector(verticesOBB[4],matrix,verticesOBB[4]);

	transformVector(verticesOBB[5],matrix,verticesOBB[5]);

	transformVector(verticesOBB[6],matrix,verticesOBB[6]);

	transformVector(verticesOBB[7],matrix,verticesOBB[7]);

	transformVector(centerOBB,matrix,centerOBB);
#elif 0//???
	D3DXFROMWINEVec4Transform(&verticesOBB[0], &verticesOBB[0], matrix);
	D3DXFROMWINEVec4Transform(&verticesOBB[1], &verticesOBB[1], matrix);
	D3DXFROMWINEVec4Transform(&verticesOBB[2], &verticesOBB[2], matrix);
	D3DXFROMWINEVec4Transform(&verticesOBB[3], &verticesOBB[3], matrix);
	D3DXFROMWINEVec4Transform(&verticesOBB[4], &verticesOBB[4], matrix);
	D3DXFROMWINEVec4Transform(&verticesOBB[5], &verticesOBB[5], matrix);
	D3DXFROMWINEVec4Transform(&verticesOBB[6], &verticesOBB[6], matrix);
	D3DXFROMWINEVec4Transform(&verticesOBB[7], &verticesOBB[7], matrix);
	D3DXFROMWINEVec4Transform(&centerOBB,&centerOBB, matrix);
#
#endif
    computeAABBEnds();

    computeAxiiAndExtents();

  }

}



BoundsDescriptor BoundsDescriptor::operator *(const D3DXFROMWINEMATRIX &matrix)

{

  BoundsDescriptor descriptor;

  if(initialized)

  {

    descriptor.initialized = true;
#if 0//???
	descriptor.verticesOBB[0]=matrix*verticesOBB[0];

	descriptor.verticesOBB[1]=matrix*verticesOBB[1];

	descriptor.verticesOBB[2]=matrix*verticesOBB[2];

	descriptor.verticesOBB[3]=matrix*verticesOBB[3];

	descriptor.verticesOBB[4]=matrix*verticesOBB[4];

	descriptor.verticesOBB[5]=matrix*verticesOBB[5];

	descriptor.verticesOBB[6]=matrix*verticesOBB[6];

	descriptor.verticesOBB[7]=matrix*verticesOBB[7];

	descriptor.centerOBB=matrix*centerOBB;
#endif

    descriptor.computeAABBEnds();

    descriptor.computeAxiiAndExtents();

  }

  return descriptor;

}



bool BoundsDescriptor::isInitialized()

{

  return initialized;

}



void  BoundsDescriptor::computeBounds(const D3DXFROMWINEVECTOR3 &minEnd, const D3DXFROMWINEVECTOR3 &maxEnd)

{

  computeBounds(minEnd.x, minEnd.y, minEnd.z,

		maxEnd.x, maxEnd.y, maxEnd.z);

}



void  BoundsDescriptor::computeBounds(float minX, float minY, float minZ,

				      float maxX, float maxY, float maxZ)

{

  initialized  = true;



  verticesOBB[0]=D3DXFROMWINEVECTOR3(minX, minY, minZ);

  verticesOBB[1]=D3DXFROMWINEVECTOR3(maxX, minY, minZ);

  verticesOBB[2]=D3DXFROMWINEVECTOR3(maxX, maxY, minZ);

  verticesOBB[3]=D3DXFROMWINEVECTOR3(minX, maxY, minZ);



  verticesOBB[4]=D3DXFROMWINEVECTOR3(minX, minY, maxZ);

  verticesOBB[5]=D3DXFROMWINEVECTOR3(maxX, minY, maxZ);

  verticesOBB[6]=D3DXFROMWINEVECTOR3(maxX, maxY, maxZ);

  verticesOBB[7]=D3DXFROMWINEVECTOR3(minX, maxY, maxZ);



  maxEndAABB=D3DXFROMWINEVECTOR3(maxX, maxY, maxZ);

  minEndAABB=D3DXFROMWINEVECTOR3(minX, minY, minZ);



  centerAABB  = maxEndAABB;

  centerAABB += minEndAABB;

  centerAABB /= 2.0f;



  centerOBB   = centerAABB;

  computeAxiiAndExtents();

}



void  BoundsDescriptor::computeAxiiAndExtents()

{

  axii[0]     = verticesOBB[5];

  axii[0]    -= verticesOBB[4];

  extents.x   = D3DXFROMWINEVec3Length(&axii[0])/2.0f;



  axii[1]     = verticesOBB[7];

  axii[1]    -= verticesOBB[4];

  extents.y   = D3DXFROMWINEVec3Length(&axii[1])/2.0f;



  axii[2]     = verticesOBB[0];

  axii[2]    -= verticesOBB[4];

  extents.z   = D3DXFROMWINEVec3Length(&axii[2])/2.0f;



  //axii[0]=normalize(axii[0]);
  //axii[1]=normalize(axii[1]);
  //axii[2]=normalize(axii[2]);
  D3DXFROMWINEVec3Normalize(&axii[0], &axii[0]);
  D3DXFROMWINEVec3Normalize(&axii[1], &axii[1]);
  D3DXFROMWINEVec3Normalize(&axii[2], &axii[2]);

}



const bool BoundsDescriptor::sphereOverlapsOBB(const D3DXFROMWINEVECTOR4 &t) const

{

  return sphereOverlapsOBB(D3DXFROMWINEVECTOR3(t.x, t.y, t.z) , t.w);

}



const bool BoundsDescriptor::lineOverlapsOBB(const D3DXFROMWINEVECTOR3 &start, const D3DXFROMWINEVECTOR3& end) const

{

  float fAWdU[3], fAWxDdU[3], fRhs;



  D3DXFROMWINEVECTOR3 kDiff	    = start;

	  kDiff     -= centerAABB;



  D3DXFROMWINEVECTOR3 direction  = end;

	  direction -= start;



  D3DXFROMWINEVECTOR3 kWxD	    = direction;

	  kWxD	     ^= kDiff;



  //direction=normalize(direction);
  D3DXFROMWINEVec3Normalize(&direction, &direction);



  fAWdU[1] = fabs(D3DXFROMWINEVec3Dot(&direction,&axii[1]));

  fAWdU[2] = fabs(D3DXFROMWINEVec3Dot(&direction,&axii[2]));

  fAWxDdU[0] = fabs(D3DXFROMWINEVec3Dot(&kWxD,&axii[0]));

  fRhs = extents[1]*fAWdU[2] + extents[2]*fAWdU[1];

  if( fAWxDdU[0] > fRhs )

    return false;



  fAWdU[0]   = fabs(D3DXFROMWINEVec3Dot(&direction,&axii[0]));

  fAWxDdU[1] = fabs(D3DXFROMWINEVec3Dot(&kWxD,&axii[1]));

  fRhs = extents[0]*fAWdU[2] + extents[2]*fAWdU[0];

  if( fAWxDdU[1] > fRhs )

    return false;



  fAWxDdU[2] = fabs(D3DXFROMWINEVec3Dot(&kWxD,&axii[2]));

  fRhs	     = extents[0]*fAWdU[1] + extents[1]*fAWdU[0];

  if(fAWxDdU[2] > fRhs )

    return false;



  return true;

}



const bool BoundsDescriptor::sphereOverlapsOBB(const D3DXFROMWINEVECTOR3 &sphrCen, float radius) const

{

  static D3DXFROMWINEVECTOR3 normals[6] = { D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 1.0f), D3DXFROMWINEVECTOR3( 0.0f, 0.0f,-1.0f),

				D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f), D3DXFROMWINEVECTOR3( 0.0f,-1.0f, 0.0f),

				D3DXFROMWINEVECTOR3( 1.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(-1.0f, 0.0f, 0.0f) };



  if(D3DXFROMWINEVec3Dot(&normals[0],&sphrCen) - D3DXFROMWINEVec3Dot(&normals[0],&verticesOBB[0]) >= radius) return false;

  if(D3DXFROMWINEVec3Dot(&normals[3],&sphrCen) - D3DXFROMWINEVec3Dot(&normals[3],&verticesOBB[0]) >= radius) return false;

  if(D3DXFROMWINEVec3Dot(&normals[5],&sphrCen) - D3DXFROMWINEVec3Dot(&normals[5],&verticesOBB[0]) >= radius) return false;



  if(D3DXFROMWINEVec3Dot(&normals[1],&sphrCen) - D3DXFROMWINEVec3Dot(&normals[1],&verticesOBB[6]) >= radius) return false;

  if(D3DXFROMWINEVec3Dot(&normals[2],&sphrCen) - D3DXFROMWINEVec3Dot(&normals[2],&verticesOBB[6]) >= radius) return false;

  if(D3DXFROMWINEVec3Dot(&normals[4],&sphrCen) - D3DXFROMWINEVec3Dot(&normals[4],&verticesOBB[6]) >= radius) return false;



  return true;

}



const bool BoundsDescriptor::sphereOverlapsAABB(const D3DXFROMWINEVECTOR4 &t) const

{

  return sphereOverlapsAABB(D3DXFROMWINEVECTOR3(t.x, t.y, t.z), t.w);

}



const bool BoundsDescriptor::frustumOverlapsAABB(const Planef *frustumPlane, int planeCount) const

{

  if(!frustumPlane)

    return false;



  D3DXFROMWINEVECTOR3  halfDiag  = maxEndAABB;

	   halfDiag -= centerAABB;



  float    m	      = 0.0f,

	   n	      = 0.0f,

	   d	      = 0.0f;



  for(int i = 0; i < planeCount; i++)

  {

     const D3DXFROMWINEVECTOR3 &normal = frustumPlane[i].getNormal();

     d	    = frustumPlane[i].getOffset();

     m	    = (centerAABB.x * normal.x) + (centerAABB.y * normal.y) +

	      (centerAABB.z * normal.z) + d;

     n	    = (halfDiag.x * fabs(normal.x)) + (halfDiag.y * fabs(normal.y)) +

	      (halfDiag.z * fabs(normal.z));

    if (m + n < 0)

      return false;

  }

  return true;

}



const bool BoundsDescriptor::sphereOverlapsAABB(const D3DXFROMWINEVECTOR3 &sphrCen, float radius) const

{

  float s, d = 0;

  for(int i=0 ; i<3 ; i++ )

  {

    if(sphrCen[i] < minEndAABB[i] )

    {

      s = sphrCen[i] - minEndAABB[i];

      d += s*s;

    }

    else

      if(sphrCen[i] > maxEndAABB[i])

      {

	s = sphrCen[i] - maxEndAABB[i];

	d += s*s;

      }

  }

  return d <= radius*radius;

}



const void BoundsDescriptor::render(int info)  const

{

  if(info & OBB)

  {

    if(info & FILL)

    {

	IRenderer::GetRendererInstance()->InitPlain3(IRenderer::GetRendererInstance()->GetplainShader3(),IRenderer::GetRendererInstance()->GetplainVF3());
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GetplainShader3());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GetplainVF3());
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(0x00,0x00,0xFF,0xFF));
/*
      IRenderer::GetRendererInstance()->VertexPointer(3, GL_FLOAT, 0, verticesOBB);

      glEnableClientState(GL_VERTEX_ARRAY);

      glDrawElements(PRIM_TRIANGLES, 36, GL_UNSIGNED_SHORT, indicesFill);

      //glDisableClientState(GL_VERTEX_ARRAY);

 */
	
	//LOG_PRINT("DrawIndexedPrimitiveUP\n");
	(IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(
		PRIM_TRIANGLES, 0,8,12,
		indicesFill,indicesFill,
		CONSTANT_INDEX2,
		verticesOBB,verticesOBB, 3*sizeof(float)));

	}

    else

    {

	IRenderer::GetRendererInstance()->InitPlain3(IRenderer::GetRendererInstance()->GetplainShader3(),IRenderer::GetRendererInstance()->GetplainVF3());
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GetplainShader3());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GetplainVF3());
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(0x00,0x00,0xFF,0xFF));

	D3DXFROMWINEVECTOR3 verts[] = {

      verticesOBB[3],

	  verticesOBB[0],

      verticesOBB[1],

	  verticesOBB[2],

      verticesOBB[3],

	  verticesOBB[7],

      verticesOBB[6],

	  verticesOBB[5],

      verticesOBB[4],

	  verticesOBB[7]

    };

	

	
	//LOG_PRINT("DrawPrimitiveUP\n");
	(IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_LINES, 10, verts, verts, 3*sizeof(float)));



	D3DXFROMWINEVECTOR3 verts2[] = {

      verticesOBB[0],

	  verticesOBB[4],

      verticesOBB[1],

	  verticesOBB[5],

      verticesOBB[2],

	  verticesOBB[6],

    };

	
	//LOG_PRINT("DrawPrimitiveUP\n");
	(IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_LINES, 6, verts, verts, 3*sizeof(float)));

    }

  }

  else

  {

	IRenderer::GetRendererInstance()->InitPlain3(IRenderer::GetRendererInstance()->GetplainShader3(),IRenderer::GetRendererInstance()->GetplainVF3());
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GetplainShader3());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GetplainVF3());
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(0x00,0x00,0xFF,0xFF));

	float verts[] = {

    minEndAABB.x, minEndAABB.y, minEndAABB.z,

	maxEndAABB.x, minEndAABB.y, minEndAABB.z,

    minEndAABB.x, minEndAABB.y, minEndAABB.z,

	minEndAABB.x, maxEndAABB.y, minEndAABB.z,

    minEndAABB.x, minEndAABB.y, minEndAABB.z,

	minEndAABB.x, minEndAABB.y, maxEndAABB.z,



    maxEndAABB.x, maxEndAABB.y, maxEndAABB.z,

	minEndAABB.x, maxEndAABB.y, maxEndAABB.z,

    maxEndAABB.x, maxEndAABB.y, maxEndAABB.z,

	maxEndAABB.x, maxEndAABB.y, minEndAABB.z,

    maxEndAABB.x, maxEndAABB.y, maxEndAABB.z,

	maxEndAABB.x, minEndAABB.y, maxEndAABB.z,



    minEndAABB.x, minEndAABB.y, maxEndAABB.z,

    maxEndAABB.x, minEndAABB.y, maxEndAABB.z,



    minEndAABB.x, maxEndAABB.y, minEndAABB.z,

    maxEndAABB.x, maxEndAABB.y, minEndAABB.z,



    minEndAABB.x, minEndAABB.y, maxEndAABB.z,

    minEndAABB.x, maxEndAABB.y, maxEndAABB.z,



    maxEndAABB.x, maxEndAABB.y, minEndAABB.z,

    maxEndAABB.x, maxEndAABB.y, maxEndAABB.z,



    maxEndAABB.x, minEndAABB.y, minEndAABB.z,

    maxEndAABB.x, maxEndAABB.y, minEndAABB.z,



    maxEndAABB.x, minEndAABB.y, minEndAABB.z,

    maxEndAABB.x, minEndAABB.y, maxEndAABB.z,



    minEndAABB.x, maxEndAABB.y, minEndAABB.z,

    minEndAABB.x, maxEndAABB.y, maxEndAABB.z,

    };

	
	//LOG_PRINT("DrawPrimitiveUP\n");
	(IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_LINES, 26, verts, verts, 3*sizeof(float)));

  }

}



void BoundsDescriptor::computeAABBEnds()

{

  minEndAABB.x = maxEndAABB.x = verticesOBB[0].x;

  minEndAABB.y = maxEndAABB.y = verticesOBB[0].y;

  minEndAABB.z = maxEndAABB.z = verticesOBB[0].z;



  for(int i=1;i<8;i++)

  {

    if(verticesOBB[i].x < minEndAABB.x) minEndAABB.x = verticesOBB[i].x;

    if(verticesOBB[i].x > maxEndAABB.x) maxEndAABB.x = verticesOBB[i].x;

    if(verticesOBB[i].y < minEndAABB.y) minEndAABB.y = verticesOBB[i].y;

    if(verticesOBB[i].y > maxEndAABB.y) maxEndAABB.y = verticesOBB[i].y;

    if(verticesOBB[i].z < minEndAABB.z) minEndAABB.z = verticesOBB[i].z;

    if(verticesOBB[i].z > maxEndAABB.z) maxEndAABB.z = verticesOBB[i].z;

  }

  centerAABB  = maxEndAABB;

  centerAABB += minEndAABB;

  centerAABB /= 2.0f;

}



const D3DXFROMWINEVECTOR3 &BoundsDescriptor::getAxis(int index)const

{

  return (index < 0 || index > 2) ? axii[0] : axii[index];

}

const D3DXFROMWINEVECTOR3 &BoundsDescriptor::getMinEndAABB() const { return minEndAABB; }

const D3DXFROMWINEVECTOR3 &BoundsDescriptor::getMaxEndAABB() const { return maxEndAABB; }

const D3DXFROMWINEVECTOR3 &BoundsDescriptor::getCenterAABB() const { return centerAABB; }

const D3DXFROMWINEVECTOR3 &BoundsDescriptor::getCenterOBB()	const { return centerOBB;  }

const D3DXFROMWINEVECTOR3 &BoundsDescriptor::getExtents()	const { return extents;    }



const D3DXFROMWINEVECTOR3 &BoundsDescriptor::getVertexOBB(int index) const

{

  return (index <0 || index > 7) ? verticesOBB[0] : verticesOBB[index];

}

