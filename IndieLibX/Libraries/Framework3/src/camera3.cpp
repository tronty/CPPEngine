//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: camera.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Defines a camera's position and orientation.
//         
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <STX/STXCPP.h>
#include <Framework3/camera3.h>

void Camera3::reset()
{
	_pos   = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
	_right = D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f);
	_up    = D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);
	_look  = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);

	velocity = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
	acceleration = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
}

Camera3::Camera3()
{
	_cameraType = AIRCRAFT;

	reset();
}

Camera3::Camera3(CameraType cameraType)
{
	_cameraType = cameraType;

	reset();
}

Camera3::~Camera3()
{

}

void Camera3::getPosition(D3DXFROMWINEVECTOR3* pos)
{
	*pos = _pos;
}

void Camera3::setPosition(D3DXFROMWINEVECTOR3* pos)
{
	_pos = *pos;
}

void Camera3::getRight(D3DXFROMWINEVECTOR3* right)
{
	*right = _right;
}

void Camera3::getUp(D3DXFROMWINEVECTOR3* up)
{
	*up = _up;
}

void Camera3::getLook(D3DXFROMWINEVECTOR3* look)
{
	*look = _look;
}

D3DXFROMWINEVECTOR3 Camera3::GetVelocity()
{
	return velocity;
}
void Camera3::SetVelocity(D3DXFROMWINEVECTOR3& v)
{
	velocity=v;
}

D3DXFROMWINEVECTOR3 Camera3::GetAcceleration()
{
	return acceleration;
}

void Camera3::Accelerate(D3DXFROMWINEVECTOR3& vAccel, float fSeconds, float RESISTANCE)
{
	acceleration=vAccel;
}
void Camera3::walk(float units)
{
	// move only on xz plane for land object
	if( _cameraType == LANDOBJECT )
		_pos += D3DXFROMWINEVECTOR3(_look.x, 0.0f, _look.z) * units;

	if( _cameraType == AIRCRAFT )
		_pos += _look * units;
}

void Camera3::strafe(float units)
{
	// move only on xz plane for land object
	if( _cameraType == LANDOBJECT )
		_pos += D3DXFROMWINEVECTOR3(_right.x, 0.0f, _right.z) * units;

	if( _cameraType == AIRCRAFT )
		_pos += _right * units;
}

void Camera3::fly(float units)
{
	// move only on y-axis for land object
	if( _cameraType == LANDOBJECT )
		_pos.y += units;

	if( _cameraType == AIRCRAFT )
		_pos += _up * units;
}

void Camera3::pitch(float angle)
{
	R.x+=angle;
	D3DXFROMWINEMATRIX T;
	D3DXFROMWINEMatrixRotationAxis(&T, &_right,	angle);

	// rotate _up and _look around _right vector
	D3DXFROMWINEVec3TransformCoord(&_up,&_up, &T);
	D3DXFROMWINEVec3TransformCoord(&_look,&_look, &T);
}

void Camera3::yaw(float angle)
{
	R.y+=angle;
	D3DXFROMWINEMATRIX T;

	// rotate around world y (0, 1, 0) always for land object
	if( _cameraType == LANDOBJECT )
		D3DXFROMWINEMatrixRotationY(&T, angle);

	// rotate around own up vector for aircraft
	if( _cameraType == AIRCRAFT )
		D3DXFROMWINEMatrixRotationAxis(&T, &_up, angle);

	// rotate _right and _look around _up or y-axis
	D3DXFROMWINEVec3TransformCoord(&_right,&_right, &T);
	D3DXFROMWINEVec3TransformCoord(&_look,&_look, &T);
}

void Camera3::roll(float angle)
{
	R.z+=angle;
	// only roll for aircraft type
	if( _cameraType == AIRCRAFT )
	{
		D3DXFROMWINEMATRIX T;
		D3DXFROMWINEMatrixRotationAxis(&T, &_look,	angle);

		// rotate _up and _right around _look vector
		D3DXFROMWINEVec3TransformCoord(&_right,&_right, &T);
		D3DXFROMWINEVec3TransformCoord(&_up,&_up, &T);
	}
}

void Camera3::getViewMatrix(D3DXFROMWINEMATRIX* V)
{
	// Keep camera's axes orthogonal to eachother
	D3DXFROMWINEVec3Normalize(&_look, &_look);

	D3DXFROMWINEVec3Cross(&_up, &_look, &_right);
	D3DXFROMWINEVec3Normalize(&_up, &_up);

	D3DXFROMWINEVec3Cross(&_right, &_up, &_look);
	D3DXFROMWINEVec3Normalize(&_right, &_right);

	// Build the view matrix:
	float x = -D3DXFROMWINEVec3Dot(&_right, &_pos);
	float y = -D3DXFROMWINEVec3Dot(&_up, &_pos);
	float z = -D3DXFROMWINEVec3Dot(&_look, &_pos);

	(*V)(0,0) = _right.x; (*V)(0, 1) = _up.x; (*V)(0, 2) = _look.x; (*V)(0, 3) = 0.0f;
	(*V)(1,0) = _right.y; (*V)(1, 1) = _up.y; (*V)(1, 2) = _look.y; (*V)(1, 3) = 0.0f;
	(*V)(2,0) = _right.z; (*V)(2, 1) = _up.z; (*V)(2, 2) = _look.z; (*V)(2, 3) = 0.0f;
	(*V)(3,0) = x;        (*V)(3, 1) = y;     (*V)(3, 2) = z;       (*V)(3, 3) = 1.0f;
}

void Camera3::setCameraType(CameraType cameraType)
{
	_cameraType = cameraType;
}

void Camera3::update()
{
	const float SPEED = 1.0f;
	const float ROTATE_SPEED = 1.0f;
	static float time1=timeGetTime();
	float fSeconds=0.001f*(timeGetTime()-time1);
	if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_a))
		strafe( - fSeconds * SPEED);
	else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_d))
		strafe(fSeconds * SPEED);
	else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_s))
		walk( - fSeconds * SPEED);
	else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_w))
		walk(fSeconds * SPEED);

    // Turn left/right means rotate around the up axis
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_KP6) || STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_RIGHT))
	{
		yaw(fSeconds * -ROTATE_SPEED);
	}

	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_KP4) || STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_LEFT))
	{
		yaw(fSeconds * ROTATE_SPEED);
	}

	// Turn up/down means rotate around the right axis
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_KP8) || STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_UP))
	{
		pitch(fSeconds * -ROTATE_SPEED);
	}
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_KP2) || STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_DOWN))
	{
		yaw(fSeconds * ROTATE_SPEED);
	}

	// Roll means rotate around the view axis
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_KP7))
	{
		roll(fSeconds * -ROTATE_SPEED);
	}
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_KP9))
	{
		roll(fSeconds * -ROTATE_SPEED);
	}
}

