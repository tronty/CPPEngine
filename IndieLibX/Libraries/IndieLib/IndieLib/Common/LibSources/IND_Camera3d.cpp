#if defined( BUILD_MESHRENDERER) 
/*****************************************************************************************
 * File: IND_Camera3d.h
 * Dess: Camera for 3d scenes
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (javierlopezpro@gmail.com)

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



#include <IND_Camera3d.h>


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------
void IND_Camera3d::InitVars()
{
	IND_Camera3d (0.0f,0.0f,0.0f);
}
/*!
\b Parameters:

\arg \b pX, \b pY, \b pZ		Position of the camera

\b Operation:

This function initializes a 3d camera and sets it in a position.
*/
IND_Camera3d::IND_Camera3d (float pX, float pY, float pZ)
{
	// Position
	mPos.x = pX;
	mPos.y = pY;
	mPos.z = pZ;

	// Orientation
	mYaw = 0;
	mPitch = 0;
	mRoll = 0;

	// Rest of values
	mZoom = 1.0f;
	mFov = D3DXFROMWINE_PI / 4;
	mAspect = 4.0f/3.0f;
#if 0
	mNearClippingPlane = 0.01f;
	mFarClippingPlane = 100.0f;
#else
	mNearClippingPlane = 0.001f;
	mFarClippingPlane = 100000.0f;
#endif
	mOrtho = 0;
	mOrthoWidth = mOrthoHeight = 0;
}


/*
*************************
End
*************************
*/
IND_Camera3d::~IND_Camera3d()
{

}


// --------------------------------------------------------------------------------
//										 Public methods
// --------------------------------------------------------------------------------


/*!
\b Parameters:

\arg \b pX, \b pY, \b pZ		Position of the camera

\b Operation:

This function sets the position of the camera.
*/
void IND_Camera3d::SetPosition (float pX, float pY, float pZ)
{
	mPos.x = pX;
	mPos.y = pY;
	mPos.z = pZ;
}


/*!
\b Parameters:

\arg \b pZoom		Zoom factor

\b Operation:

This function zooms the camera. Use this method if you want to zoom in or out the scenery.

Default: 1.0f
*/
void IND_Camera3d::SetZoom (float pZoom)
{
	mZoom = pZoom;
}


/*!
\b Parameters:

\arg \b pRoll		Roll angle in degrees

\b Operation:

This function sets the angle of camera rotation around the roll axis.

Default: 0.0f
*/
void IND_Camera3d::SetRoll (float pRoll)
{
	mRoll = pRoll;
}


/*!
\b Parameters:

\arg \b pPitch		Pitch angle in degrees

\b Operation:

This function sets the angle of camera rotation around the pitch axis.

Default: 0.0f
*/
void IND_Camera3d::SetPitch (float pPitch)
{
	mPitch = pPitch;
}


/*!
\b Parameters:

\arg \b pYaw		Yaw angle in degrees

\b Operation:

This function sets the angle of camera rotation around the yaw axis.

Default: 0.0f
*/
void IND_Camera3d::SetYaw (float pYaw)
{
	mYaw = pYaw;
}


/*!
\b Parameters:

\arg \b pValue		Distancy to "walk" the camera

\b Operation:

This function moves the camera from it's current position towards the direction it is facing.
*/
void IND_Camera3d::MoveWalk (float pValue)
{
	// Advance
	mPos.y += mLook.y * pValue;
	mPos.x += mLook.x * pValue;
	mPos.z += mLook.z * pValue;
}


/*!
\b Parameters:

\arg \b pStrafe		Distancy to "strafe" the camera

\b Operation:

This function moves the camera from it's current position in right / left directions.
*/
void IND_Camera3d::MoveStrafe (float pStrafe)
{
	mPos.y += mRight.y * pStrafe;
	mPos.x += mRight.x * pStrafe;
	mPos.z += mRight.z * pStrafe;
}


/*!
\b Parameters:

\arg \b pFly		Distancy to "fly" the camera

\b Operation:

This function moves the camera from it's current position in up / left right directions.
*/
void IND_Camera3d::MoveFly (float pFly)
{
	mPos.x += mUp.x * pFly;
	mPos.y += mUp.y * pFly;
	mPos.z += mUp.z * pFly;
}


/*!
\b Parameters:

\arg \b pFov		Fov value

\b Operation:

This function sets the field of view of the camera.

Default: D3DXFROMWINE_PI / 4
*/
void IND_Camera3d::SetFov (float pFov)
{
	mFov = pFov;
}


/*!
\b Parameters:

\arg \b pAspect		Aspect value

\b Operation:

This function sets the aspect of camera.

Default: IRenderer::GetRendererInstance()->GetAspect()
*/
void IND_Camera3d::SetAspect (float pAspect)
{
	mAspect = pAspect;
}


/*!
\b Parameters:

\arg \b pNearPlane		Distancy to the near plane

\b Operation:

This function sets the distancy to the near plane.

Default: 0.01f
*/
void IND_Camera3d::SetNearClippingPlane	(float pNearPlane)
{
	mNearClippingPlane = pNearPlane;
}


/*!
\b Parameters:

\arg \b pNearPlane		Distancy to the far plane

\b Operation:

This function sets the distancy to the far plane.

Default: 100.0f
*/
void IND_Camera3d::SetFarClippingPlane (float pFarPlane)
{
	mFarClippingPlane = pFarPlane;
}


/*!
\b Parameters:

\arg \b pOrtho		Orhot projection 0 / 1 = off / on

\b Operation:

This function activates / deactivates the orthonormal projection.

Default: 0
*/
void IND_Camera3d::ToggleOrthoProjection (bool pOrtho)
{
	mOrtho = pOrtho;
}


/*!
\b Parameters:

\arg \b pOrthoWidth		Width value for the ortho projection

\b Operation:

This function sets the width of the ortho projection. This value should usually be the same
as the viewport width, meaning you can set this value to IND_Render::GetViewPortWidth()

Default: 0
*/
void IND_Camera3d::SetOrthoWidth (float pOrthoWidth)
{
	mOrthoWidth = pOrthoWidth;
}


/*!
\b Parameters:

\arg \b pOrthoHeight		Hight value for the ortho projection

\b Operation:

This function sets the height of the ortho projection. This value should usually be the same
as the viewport height, meaning you can set this value to IND_Render::GetViewPortHeight()

Default: 0
*/
void IND_Camera3d::SetOrthoHeight (float pOrthoHeight)
{
	mOrthoHeight = pOrthoHeight;
}


// --------------------------------------------------------------------------------
//									 Private methods
// --------------------------------------------------------------------------------




#endif

