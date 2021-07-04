//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: camera.h
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Defines a camera's position and orientation.
//         
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __cameraH__
#define __cameraH__

#include <Framework3/IRenderer.h>

class Camera3
{
public:
	enum CameraType { LANDOBJECT, AIRCRAFT };

	Camera3();
	Camera3(CameraType cameraType);
	~Camera3();
	Camera3(const Camera3& rhs){_cameraType=rhs._cameraType;}

	void reset();

	void strafe(float units=0.0f); // left/right
	void fly(float units=0.0f);    // up/down
	void walk(float units=0.0f);   // forward/backward
	
	void pitch(float angle=0.0f); // rotate on right vector
	void yaw(float angle=0.0f);   // rotate on up vector
	void roll(float angle=0.0f);  // rotate on look vector

	void getViewMatrix(D3DXFROMWINEMATRIX* V);
	/*
	WIN10:
1>c:\indielibx\libraries\framework3\include\framework3\camera3.h(37): error C2662: 'void Camera3::getViewMatrix(D3DXFROMWINEMATRIX *)': cannot convert 'this' pointer from 'const Camera3' to 'Camera3 &'
1>  c:\indielibx\libraries\framework3\include\framework3\camera3.h(37): note: Conversion loses qualifiers
1>c:\indielibx\libraries\framework3\include\framework3\camera3.h(59): error C2662: 'void Camera3::getPosition(D3DXFROMWINEVECTOR3 *)': cannot convert 'this' pointer from 'const Camera3' to 'Camera3 &'
1>  c:\indielibx\libraries\framework3\include\framework3\camera3.h(59): note: Conversion loses qualifiers
	*/
	D3DXFROMWINEMATRIX getModelViewMatrix()
#if !defined(_MSC_VER) && !defined(__APPLE__)
const
#endif
{D3DXFROMWINEMATRIX V;getViewMatrix(&V);return V;}
	D3DXFROMWINEMATRIX getProjectionMatrix(){return mProjectionMatrix;}
	void updateClipPlanes(float nearClip, float farClip)
	{
		D3DXFROMWINEMatrixPerspectiveFovLH(&mProjectionMatrix, 90.0f, 1.0f*IRenderer::GetRendererInstance()->GetViewportWidth()/IRenderer::GetRendererInstance()->GetViewportHeight(), nearClip, farClip);
	}
	D3DXFROMWINEMATRIX GetMVP()
	{
		D3DXFROMWINEMATRIX mvp, V;
		getViewMatrix(&V);
		mvp=V*mProjectionMatrix;
		return mvp;
	}

void elevate(float speed)
{
	_pos.y  += speed;
	//focus.y     += speed;
}

	void setCameraType(CameraType cameraType); 
	void getPosition(D3DXFROMWINEVECTOR3* pos); 
	D3DXFROMWINEVECTOR3 getViewerPosition()
#if !defined(_MSC_VER) && !defined(__APPLE__)
const
#endif
{D3DXFROMWINEVECTOR3 p;getPosition(&p);return p;}
	void setPosition(D3DXFROMWINEVECTOR3* pos);
void setLook(D3DXFROMWINEVECTOR3* look)
{
	_look = *look;
}
void setUp(D3DXFROMWINEVECTOR3* up)
{
	_up = *up;
}
	void getRight(D3DXFROMWINEVECTOR3* right);
	void getUp(D3DXFROMWINEVECTOR3* up);
	void getLook(D3DXFROMWINEVECTOR3* look);
	D3DXFROMWINEVECTOR3 getR(){return R;}

	D3DXFROMWINEVECTOR3 GetVelocity();
	void SetVelocity(D3DXFROMWINEVECTOR3& v);
	D3DXFROMWINEVECTOR3 GetAcceleration();
	void Accelerate(D3DXFROMWINEVECTOR3& vAccel, float fSeconds, float RESISTANCE);
	void update();
private:
	CameraType  _cameraType;
	D3DXFROMWINEVECTOR3 _right;
	D3DXFROMWINEVECTOR3 _up;
	D3DXFROMWINEVECTOR3 _look;
	D3DXFROMWINEVECTOR3 _pos;
	D3DXFROMWINEVECTOR3 velocity, acceleration, R;
	D3DXFROMWINEMATRIX  mProjectionMatrix;
};
#endif // __cameraH__
