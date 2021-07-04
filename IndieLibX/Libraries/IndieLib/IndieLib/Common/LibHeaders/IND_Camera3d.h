#if defined( BUILD_MESHRENDERER) 
/*****************************************************************************************
/* File: IND_Camera3d.h
/* Dess: Camera for 3d scenes
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

#ifndef _IND_CAMERA3D_
#define _IND_CAMERA3D_


// ----- Includes -----
/*
#include <d3d9.h> 
#include <d3dx9.h>
*/

// --------------------------------------------------------------------------------
//									   IND_Camera
// --------------------------------------------------------------------------------

/*!
\defgroup Camera3d Camera3d
\ingroup Cameras
Camera for 3d scenarios. Using this class you can control all the parameters of a 3d camera. You
can have as many cameras as you want. Use ::IND_Render::SetCamera3d() method before drawing in order
to choose the camera you want to use.
*/
/*@{*/ 
class DLL_EXP IND_Camera3d //ñ
{
public:

	// ----- Init/End -----

	IND_Camera3d		(float pX, float pY, float pZ);
	//IND_Camera3d		(float pX, float pY, float pZ){IND_Camera3d(int(pX), int(pY), int(pZ));}
	~IND_Camera3d		();
	IND_Camera3d		(){ InitVars (); };
	
	// ----- Public methods -----

	// View
	void SetPosition				(float pX, float pY, float pZ);
	void SetPitch					(float pAngle);
	void SetYaw						(float pAngle);
	void SetRoll					(float pAngle);
	void SetZoom					(float pZoom);
	void MoveWalk					(float pWalk);
	void MoveFly					(float pFly);
	void MoveStrafe					(float pStrafe);

	// Projection
	void SetFov						(float pFov);
	void SetAspect					(float pAspect);
	void SetNearClippingPlane		(float pNearPlane);
	void SetFarClippingPlane		(float pFarPlane);
	void ToggleOrthoProjection		(bool pOrtho);
	void SetOrthoWidth				(float pOrthoWidth);
	void SetOrthoHeight				(float pOrthoHeight);

	// ----- Public gets -----

	//! Returns the x position of the camera
	float GetPosX					()	{ return mPos.x;				}
	//! Returns the y position of the camera
	float GetPosY					()	{ return mPos.y;				}
	//! Returns the z position of the camera
	float GetPosZ					()	{ return mPos.z;				}
	//! Returns the first component of the lookat camera vector
	float GetLookX					()	{ return mLook.x;				}
	//! Returns the second component of the lookat camera vector	
	float GetLookY					()	{ return mLook.y;				}
	//! Returns the third component of the lookat camera vector	
	float GetLookZ					()	{ return mLook.z;				}
	//! Returns the first component of the up camera vector	
	float GetUpX					()	{ return mUp.z;					}
	//! Returns the second component of the up camera vector		
	float GetUpY					()	{ return mUp.y;					}
	//! Returns the third component of the up camera vector		
	float GetUpZ					()	{ return mUp.z;					}
	//! Returns the first component of the right camera vector		
	float GetRightX					()	{ return mRight.x;				}
	//! Returns the second component of the right camera vector		
	float GetRightY					()	{ return mRight.y;				}
	//! Returns the third component of the right camera vector		
	float GetRightZ					()	{ return mRight.z;				}
	//! Returns the zoom factor  of the camera		
	float GetZoom					()	{ return mZoom;					}
	//! Returns the fov of the camera		
	float GetFov					()	{ return mFov;					}
	//! Returns the aspect of the camera		
	float GetAspect					()	{ return mAspect;				}
	//! Returns the near clipping plane of the camera
	float GetNearClippingPlane		()	{ return mNearClippingPlane;	}
	//! Returns the far clipping plane of the camera	
	float GetFarClippingPlane		()	{ return mFarClippingPlane;		}
	//! Returns true if the camera has an ortho projection assigned		
	bool IsOrthoProjection			()	{ return mOrtho;				}
	//! Returns the width of the camera	ortho projection
	float GetOrthoWidth				()  { return mOrthoWidth;			}
	//! Returns the height of the camera ortho projection	
	float GetOrthoHeight			()  { return mOrthoHeight;			}	

	D3DXFROMWINEMATRIX w;
	D3DXFROMWINEMATRIX v;
	D3DXFROMWINEMATRIX p;

private:

	// ----- Private -----
	
	bool mOk;

	// ----- Vars -----

	D3DXFROMWINEVECTOR3 mPos;
	D3DXFROMWINEVECTOR3 mLook;
	D3DXFROMWINEVECTOR3 mUp;
	D3DXFROMWINEVECTOR3 mRight;

	float mPitch;
	float mYaw;
	float mRoll;

	float mZoom;
	float mFov;
	float mAspect;
	float mNearClippingPlane;
	float mFarClippingPlane;
	bool mOrtho;
	float mOrthoWidth, mOrthoHeight;
	
	// ----- Private methods -----
	
	void				InitVars		();
	void				FreeVars		();

	// ----- Friends -----

	friend class IND_Render;
};
	
#endif // _IND_CAMERA3D_
#endif

