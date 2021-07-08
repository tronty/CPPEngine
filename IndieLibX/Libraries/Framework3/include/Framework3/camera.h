/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef _CAMERA_H
#define _CAMERA_H
#include <FrameworkHLSLCg3/RendererHLSLCg.h>
// This was created to allow us to use this camera code in other projects
// without having to cut and paste code.  This file and camera.cpp could be
// added to a future project.
// This is our camera class
class CCamera {
public:
	D3DXFROMWINEMATRIX matProj;
	D3DXFROMWINEMATRIX matView;
	D3DXFROMWINEMATRIX matWorld;

	// Our camera constructor
	CCamera();
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	// These are are data access functions for our camera's private data
	float3 Position() {	return m_vPosition;		}
	float3 View()		{	return m_vView;			}
	float3 UpVector() {	return m_vUpVector;		}
	float3 Strafe()	{	return m_vStrafe;		}
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	// This changes the position, view, and up vector of the camera.
	// This is primarily used for initialization
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);
	// This rotates the camera's view around the position depending on the values passed in.
	void RotateView(float angle, float X, float Y, float Z);
	// This moves the camera's view by the mouse movements (First person view)
	void SetViewByMouse();
	// This rotates the camera around a point (I.E. your character).
	void RotateAroundPoint(float3 vCenter, float X, float Y, float Z);
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	// This strafes the camera left or right depending on the speed (+/-)
	void StrafeCamera(float speed);
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	// This will move the camera forward or backward depending on the speed
	void MoveCamera(float speed);
	// This checks for keyboard movement
	void CheckForMovement();
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	// This updates the camera's view and other data (Should be called each frame)
	void Update();
	// This uses gluLookAt() to tell OpenGL where to look
	void Look();
private:
	// The camera's position
	float3 m_vPosition;
	// The camera's view
	float3 m_vView;
	// The camera's up vector
	float3 m_vUpVector;
	// The camera's strafe vector
	float3 m_vStrafe;
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
};
#endif
/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES *
//
// This file contains the prototype for our camera class.
//
// We added a few more functions to our CCamera class, along with
// some measly data access functions for our now private member variables.
//
//
// ©2000-2005 GameTutorials
