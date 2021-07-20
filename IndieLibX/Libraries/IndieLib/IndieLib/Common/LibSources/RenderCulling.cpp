/*****************************************************************************************
 * File: RenderCulling.cpp
 * Desc: Culling for 3d objects
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (info@pixelartgames.com)

This library is free software you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free Software
Foundation either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this library if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA 
*/
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	
*/

// ----- Includes -----
#include <CIndieLib.h>


#include <IND_Render.h>

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

// --------------------------------------------------------------------------------
//							         Private methods
// --------------------------------------------------------------------------------

/*
*************************
Calculates 6 planes defining the frustum
*************************
*/
INLINE void IND_Render::CalculeFrustumPlanes	()
{
	D3DXFROMWINEMATRIX mMatView, mMatProj;
	mInfo.mDevice->GetTransform (TS_VIEW, mMatView);
	mInfo.mDevice->GetTransform (TS_PROJECTION, mMatProj);

	// Get combined matrix
	D3DXFROMWINEMATRIX/*A16*/ matComb;
#if 1
	D3DXFROMWINEMatrixMultiply(&matComb, &mMatView, &mMatProj);
#else
	D3DXFROMWINEMatrixMultiply(&matComb, &mMatProj, &mMatView);
#endif
	// Left clipping plane
	mFrustumPlanes[0].mNormal.x		= -(matComb.m[0][3] + matComb.m[0][0]);
	mFrustumPlanes[0].mNormal.y		= -(matComb.m[1][3] + matComb.m[1][0]);
	mFrustumPlanes[0].mNormal.z		= -(matComb.m[2][3] + matComb.m[2][0]);
	mFrustumPlanes[0].mDistance		= -(matComb.m[3][3] + matComb.m[3][0]);

	// Right clipping plane
	mFrustumPlanes[1].mNormal.x		= -(matComb.m[0][3] - matComb.m[0][0]);
	mFrustumPlanes[1].mNormal.y		= -(matComb.m[1][3] - matComb.m[1][0]);
	mFrustumPlanes[1].mNormal.z		= -(matComb.m[2][3] - matComb.m[2][0]);
	mFrustumPlanes[1].mDistance		= -(matComb.m[3][3] - matComb.m[3][0]);

	// Top clipping plane
	mFrustumPlanes[2].mNormal.x		= -(matComb.m[0][3] - matComb.m[0][1]);
	mFrustumPlanes[2].mNormal.y		= -(matComb.m[1][3] - matComb.m[1][1]);
	mFrustumPlanes[2].mNormal.z		= -(matComb.m[2][3] - matComb.m[2][1]);
	mFrustumPlanes[2].mDistance		= -(matComb.m[3][3] - matComb.m[3][1]);

	// Bottom clipping plane
	mFrustumPlanes[3].mNormal.x		= -(matComb.m[0][3] + matComb.m[0][1]);
	mFrustumPlanes[3].mNormal.y		= -(matComb.m[1][3] + matComb.m[1][1]);
	mFrustumPlanes[3].mNormal.z		= -(matComb.m[2][3] + matComb.m[2][1]);
	mFrustumPlanes[3].mDistance		= -(matComb.m[3][3] + matComb.m[3][1]);

	// Near clipping plane
	mFrustumPlanes[4].mNormal.x		= -(matComb.m[0][3] + matComb.m[0][2]);
	mFrustumPlanes[4].mNormal.y		= -(matComb.m[1][3] + matComb.m[1][2]);
	mFrustumPlanes[4].mNormal.z		= -(matComb.m[2][3] + matComb.m[2][2]);
	mFrustumPlanes[4].mDistance		= -(matComb.m[3][3] + matComb.m[3][2]);

	// Far clipping plane
	mFrustumPlanes[5].mNormal.x		= -(matComb.m[0][3] - matComb.m[0][2]);
	mFrustumPlanes[5].mNormal.y		= -(matComb.m[1][3] - matComb.m[1][2]);
	mFrustumPlanes[5].mNormal.z		= -(matComb.m[2][3] - matComb.m[2][2]);
	mFrustumPlanes[5].mDistance		= -(matComb.m[3][3] - matComb.m[3][2]);

	//for (int i = 0 i < 6 i++)
		//mFrustumPlanes [i].Normalise()
}


/*
*************************
Taking an AABB min and max in world space, work out its interaction with the view frustum
0 is outside
1 is partially in
2 is completely within
Note: the viewing frustum must be calculated first
*************************
*/
__WORD__ IND_Render::CullFrustumBox (const D3DXFROMWINEVECTOR3 &pAABBMin, const D3DXFROMWINEVECTOR3 &pAABBMax)
{
	bool mIntersect = 0;
	__WORD__ mResult = 0;
	D3DXFROMWINEVECTOR3 mMinExtreme, mMaxExtreme;

	for (__WORD__ i = 0; i < 6; i++)
	{
		if (mFrustumPlanes[i].mNormal.x >= 0)
        {
			mMinExtreme.x = pAABBMin.x;
			mMaxExtreme.x = pAABBMax.x;
        }
        else
        {
			mMinExtreme.x = pAABBMax.x;
			mMaxExtreme.x = pAABBMin.x;
        }

        if (mFrustumPlanes[i].mNormal.y >= 0)
        {
			mMinExtreme.y = pAABBMin.y;
			mMaxExtreme.y = pAABBMax.y;
        }
        else
        {
			mMinExtreme.y = pAABBMax.y;
			mMaxExtreme.y = pAABBMin.y;
        }

        if (mFrustumPlanes[i].mNormal.z >= 0)
        {
			mMinExtreme.z = pAABBMin.z;
			mMaxExtreme.z = pAABBMax.z;
        }
        else
        {
			mMinExtreme.z = pAABBMax.z;
			mMaxExtreme.z = pAABBMin.z;
        }

        if (mFrustumPlanes[i].DistanceToPoint (mMinExtreme) > 0)
		{
			mResult  = 0;
			return mResult;
        }

        if (mFrustumPlanes[i].DistanceToPoint (mMaxExtreme) >= 0)
           mIntersect = 1;
      }

      if (mIntersect)
         mResult = 1;
      else
         mResult = 2;

      return mResult;

}


/*
*************************
Creates a bounding rectangle surronding the block for discarding it using frustum culling
*************************
*/
void IND_Render::CalculateBoundingRectangle (D3DXFROMWINEVECTOR3 *mP1, D3DXFROMWINEVECTOR3 *mP2, D3DXFROMWINEVECTOR3 *mP3, D3DXFROMWINEVECTOR3 *mP4)
{
	int mMinX, mMaxX, mMinY, mMaxY;

	MinAndMax4 ((int) mP1->x, (int) mP2->x, (int) mP3->x, (int) mP4->x, &mMaxX, &mMinX);
	MinAndMax4 ((int) mP1->y, (int) mP2->y, (int) mP3->y, (int) mP4->y, &mMaxY, &mMinY);

	mP1->x = (float) mMinX;
	mP1->y = (float) mMinY;
	mP2->x = (float) mMaxX;
	mP2->y = (float) mMaxY;
}


/*
*************************
Creates a bounding rectangle surronding the block for discarding it using frustum culling
*************************
*/
void IND_Render::Transform4Vertices (float pX1, float pY1,
									 float pX2, float pY2,
									 float pX3, float pY3,
									 float pX4, float pY4,
									 D3DXFROMWINEVECTOR4 *mP1Res,
									 D3DXFROMWINEVECTOR4 *mP2Res,
									 D3DXFROMWINEVECTOR4 *mP3Res,
									 D3DXFROMWINEVECTOR4 *mP4Res)
{
	// ----- Transform 4 vertices of the quad into world space coordinates -----

	D3DXFROMWINEMATRIX mMatWorld;
	mInfo.mDevice->GetTransform (TS_WORLD, mMatWorld);

	D3DXFROMWINEVECTOR2 mP1 (pX1, pY1);
	D3DXFROMWINEVECTOR2 mP2 (pX2, pY2);
	D3DXFROMWINEVECTOR2 mP3 (pX3, pY3);
	D3DXFROMWINEVECTOR2 mP4 (pX4, pY4);

	D3DXFROMWINEVECTOR4 mP1_f, mP2_f, mP3_f, mP4_f;
	D3DXFROMWINEVec2Transform (mP1Res, &mP1, &mMatWorld);
	D3DXFROMWINEVec2Transform (mP2Res, &mP2, &mMatWorld);
	D3DXFROMWINEVec2Transform (mP3Res, &mP3, &mMatWorld);
	D3DXFROMWINEVec2Transform (mP4Res, &mP4, &mMatWorld);
}


/*
*************************
Returns the max and min of 4 values
*************************
*/
INLINE void IND_Render::MinAndMax4	(int p1,
									int p2,
									int p3,
									int p4,
									int *pMax,
									int *pMin)
{
	*pMax = MAX (p1, p2);
	*pMax = MAX (*pMax, p3);
	*pMax = MAX (*pMax, p4);
	*pMin = MIN (p1, p2);
	*pMin = MIN (*pMin, p3);
	*pMin = MIN (*pMin, p4);
}

