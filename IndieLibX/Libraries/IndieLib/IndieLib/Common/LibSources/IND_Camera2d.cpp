/*****************************************************************************************
 * File: IND_Camera2d.h
 * Dess: Camera for 2d scenes
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



#include <IND_Camera2d.h>


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pX, \b pY		Position of the camera

\b Operation:

This function initializes a 2d camera and sets it in a position. In order to display on all of the
screen's area this position is usually defined as the middle of the screen.
*/
/*
void IND_Camera2d::InitVars()
{
	IND_Camera2d (0,0);
}
*/
IND_Camera2d::IND_Camera2d (int pX, int pY)
{
	mX = pX;
	mY = pY;
	mPos.x = 0;// (float) -pX
	mPos.y = 0; //(float) -pY
	mPos.z = -0.5f;

	mLook	= D3DXFROMWINEVECTOR3 (0.0f, 0.0f, 1.0f);
	mUp		= D3DXFROMWINEVECTOR3 (0.0f, 1.0f, 0.0f);
	mRight	= D3DXFROMWINEVECTOR3 (-1.0f, 0.0f, 0.0f);

	mZoom = 1.0f;
	mAngle = 0.0f;
}


/*
*************************
End
*************************
*/
IND_Camera2d::~IND_Camera2d()
{

}


// --------------------------------------------------------------------------------
//										 Public methods
// --------------------------------------------------------------------------------


/*!
\b Parameters:

\arg \b pX, \b pY		Position of the camera

\b Operation:

This function sets the position of the camera. In order to display on all of the
screen's area this position is usually defined as the middle of the screen.
*/
void IND_Camera2d::SetPosition (int pX, int pY)
{
	mX = pX;
	mY = pY;
}


/*!
\b Parameters:

\arg \b pZoom		Zoom factor

\b Operation:

This function zooms the camera. Use this method in order to zoom in or out of the scenery.

Default: 1.0f
*/
void IND_Camera2d::SetZoom (float pZoom)
{
	mZoom = pZoom;
}


/*!
\b Parameters:

\arg \b pAngle		Camera angle in degrees.

\b Operation:

This function sets the angle of the camera. Use this method in order to rotate the scenary.

Default: 0.0f
*/
void IND_Camera2d::SetAngle	(float pAngle)
{
	mAngle = pAngle;
}


// --------------------------------------------------------------------------------
//									 Private methods
// --------------------------------------------------------------------------------

/*
*************************
Strafe displacement
*************************
*/
void IND_Camera2d::Strafe (int pStrafe)
{
	mPos.x += mRight.x * pStrafe;
	mPos.y += mRight.y * pStrafe;
	mPos.z += mRight.z * pStrafe;
}


/*
*************************
Fly displacement
*************************
*/
void IND_Camera2d::Fly (int pFly)
{
	mPos.x += mUp.x * pFly;
	mPos.y += mUp.y * pFly;
	mPos.z += mUp.z * pFly;
}

