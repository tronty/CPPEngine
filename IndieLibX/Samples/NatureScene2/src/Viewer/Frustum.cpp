//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Frustum Culling									 //
//																		 //
//		$Description:	Demonstrates checking if shapes are in view		 //
//																		 //
//		$Date:			8/28/01											 //
//																		 //
//***********************************************************************//

#include "Frustum.h"

void Frustum::update()
{    
	static D3DXFROMWINEMATRIX proj,								// This will hold our projection matrix
	                modl,								// This will hold our modelview matrix
                  clip;								// This will hold the clipping planes
  D3DXFROMWINEVECTOR4        *p;

	// //glGetFloatv() is used to extract information about our OpenGL world.
	// Below, we pass in GL_PROJECTION_MATRIX to abstract our projection matrix.
	// It then stores the matrix into an array of [16].
	//glGetFloatv( GL_PROJECTION_MATRIX, proj );

	// By passing in GL_MODELVIEW_MATRIX, we can abstract our model view matrix.
	// This also stores it in an array of [16].
	//glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	// Now that we have our modelview and projection matrix, if we combine these 2 matrices,
	// it will give us our clipping planes.  To combine 2 matrices, we multiply them.

  clip   = proj;
 	clip  *= modl;
  p = &frustumPlane[RIGHT];
  p->x=clip[3]-clip[0];
  p->y=clip[7]-clip[4];
  p->z=clip[11]-clip[8];
  p->w=clip[15]-clip[12];
   
  p=&frustumPlane[LEFT];
  p->x=clip[3]+clip[0];
  p->y=clip[7]+clip[4];
  p->z=clip[11]+clip[8];
  p->w=clip[15]+clip[12];
   
  p=&frustumPlane[BOTTOM];
  p->x=clip[3]+clip[1];
  p->y=clip[7]+clip[5];
  p->z=clip[11]+clip[9];
  p->w=clip[15]+clip[13];
   
  p=&frustumPlane[TOP];
  p->x=clip[3]-clip[1];
  p->y=clip[7]-clip[5];
  p->z=clip[11]-clip[9];
  p->w=clip[15]-clip[13];
   
  p=&frustumPlane[BACK];
  p->x=clip[3]-clip[2];
  p->y=clip[7]-clip[6];
  p->z=clip[11]-clip[10];
  p->w=clip[15]-clip[14];
   
  p=&frustumPlane[FRONT];
  p->x=clip[3]+clip[2];
  p->y=clip[7]+clip[6];
  p->z=clip[11]+clip[10];
  p->w=clip[15]+clip[14];
   
  // Normalize all plane normals
	float magnitude = 0.0f;
  for(int i=0;i<6;i++)
  { 
    magnitude = sqrtf(frustumPlane[i].x * frustumPlane[i].x + 
								               frustumPlane[i].y * frustumPlane[i].y + 
								               frustumPlane[i].z * frustumPlane[i].z );
   if(magnitude)
     frustumPlane[i] /= magnitude;
  }
}

const bool Frustum::pointInFrustum(const D3DXFROMWINEVECTOR3 & center)
{
  return pointInFrustum(center.x, center.y, center.z);
}

const bool Frustum::pointInFrustum( float x, float y, float z )
{
	// Go through all the sides of the frustum
	for(int i = 0; i < 6; i++ )
	{
		// Calculate the plane equation and check if the point is behind a side of the frustum
		if(frustumPlane[i].x * x + frustumPlane[i].y * y + frustumPlane[i].z * z + frustumPlane[i].w <= 0)
		{
			// The point was behind a side, so it ISN'T in the frustum
			return false;
		}
	}

	// The point was inside of the frustum (In front of ALL the sides of the frustum)
	return true;
}


/*
 *This retrieves our distance from a frustum plane
 */

const float Frustum::getDistanceToPlane(int i, const D3DXFROMWINEVECTOR3 &center){
  return frustumPlane[i].x * center.x +
         frustumPlane[i].y * center.y +
         frustumPlane[i].z * center.z +
         frustumPlane[i].w;
}

const bool Frustum::sphereInFrustum(const D3DXFROMWINEVECTOR3 &center, float radius)
{
  return sphereInFrustum(center.x, center.y, center.z, radius);
}

const bool Frustum::sphereInFrustum(float x, float y, float z, float radius)
{
	// Go through all the sides of the frustum
	for(int i = 0; i < 6; i++ )	
	{
		// If the center of the sphere is farther away from the plane than the radius
		if( frustumPlane[i].x * x + frustumPlane[i].y * y + frustumPlane[i].z * z + frustumPlane[i].w <= -radius )
		{
			// The distance was greater than the radius so the sphere is outside of the frustum
			return false;
		}
	}
	
	// The sphere was inside of the frustum!
	return true;
}

const bool Frustum::AABBInFrustum(const BoundsDescriptor &aabb) const
{
  D3DXFROMWINEVECTOR3  center   = aabb.getCenterAABB(),
           halfDiag = aabb.getMaxEndAABB();
  
  float    m        = 0.0f,
           n        = 0.0f;

  halfDiag -= center;

  for(int i = 0; i < 6; i++)
  {
     m = (center.x * frustumPlane[i].x) + (center.y * frustumPlane[i].y) + 
         (center.z * frustumPlane[i].z) + frustumPlane[i].w;
     n = (halfDiag.x * fabs(frustumPlane[i].x)) + (halfDiag.y * fabs(frustumPlane[i].y)) +
         (halfDiag.z * fabs(frustumPlane[i].z));

    if (m + n < 0)
      return false;
    
  }
  return true;
}
