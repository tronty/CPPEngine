/*****************************************************************************************
/* File: IND_Camera2d.h
/* Dess: Camera for 2d scenes
/*****************************************************************************************/
 
/*
IndieLib 2d library Copyright (C) 2005 Javier López López (javierlopezpro@gmail.com) 

This library is free software; you can redistribute it and/or modify it under the 
terms of the GNU Lesser General Public License as published by the Free Software 
Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with 
this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
Suite 330, Boston, MA 02111-1307 USA 
*/
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	 
*/

#ifndef _IND_CAMERA2D_
#define _IND_CAMERA2D_


// ----- Includes -----
/*
#include <d3d9.h> 
#include <d3dx9.h>
*/

// --------------------------------------------------------------------------------
//									   IND_Camera
// --------------------------------------------------------------------------------

/*!
\defgroup Camera2d Camera2d
\ingroup Cameras
This is the camera for 2d scenarios. Using this class you can control all the parameters of a 2d camera. You
can have as many cameras as you want. Use ::IND_Render::SetCamera2d() method before drawing in order
to choose the camera you want to use.
*/
/*@{*/ 
class DLL_EXP IND_Camera2d //ñ
{
public:

	// ----- Init/End -----

	IND_Camera2d		(int pX, int pY);
	IND_Camera2d		(float pX, float pY){IND_Camera2d(int(pX), int(pY));}
	~IND_Camera2d		();
	IND_Camera2d		(){ /*InitVars ();*/ };

	// ----- Public methods -----

	void SetPosition			(int pX, int pY);
	void SetPosition			(float pX, float pY){SetPosition(int(pX), int(pY));}
	void SetZoom				(float pZoom);
	void SetAngle				(float pAngle);

	// ----- Public gets -----
	D3DXFROMWINEMATRIX w;
	D3DXFROMWINEMATRIX v;
	D3DXFROMWINEMATRIX p;

	//! Returns the x position of the camera
	int GetPosX				()	{ return mX;				}
	//! Returns the y position of the camera
	int GetPosY				()	{ return mY;				}
	//! Returns the zoom of the camera
	float GetZoom			()	{ return mZoom;				}
	//! Returns the angle of the camera
	float GetAngle			()	{ return mAngle;			}

private:

	// ----- Private -----
	
	bool mOk;
	int mX, mY;

	// ----- Vars -----

	D3DXFROMWINEVECTOR3 mPos;
	D3DXFROMWINEVECTOR3 mLook;
	D3DXFROMWINEVECTOR3 mUp;
	D3DXFROMWINEVECTOR3 mRight;

	float mZoom;
	float mAngle;
	
	// ----- Private methods -----
	
	void				Strafe			(int pStrafe);
	void				Fly				(int pFly);
	//void				InitVars		();
	void				FreeVars		();

	// ----- Friends -----

	friend class IND_Render;
};
	
#endif // _IND_CAMERA_
