/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#if 1
#include <STX/STXCPP.h>

#include <Framework3/Camera2.h>

//Camera2 g_Camera;
Camera2::Camera2(IInput* Input)
{
	_cameraType = LANDOBJECT;//AIRCRAFT;
	init(Input);
}

Camera2::Camera2(CameraType cameraType,IInput* Input)
{
	_cameraType = cameraType;
	init(Input);
}

Camera2::~Camera2()
{

}

    Camera2::Camera2(const Camera2 &a)
    {
		this->_cameraType=a._cameraType;
		this->_look0=a._look0;
		this->_pos0=a._pos0;
		this->_right0=a._right0;
		this->_up0=a._up0;
		this->_look=a._look;
		this->_pos=a._pos;
		this->_right=a._right;
		this->_up=a._up;
		this->m_fSpinX=a.m_fSpinX;
		this->m_fSpinY=a.m_fSpinY;
		this->m_fSpinZ=a.m_fSpinZ;

		this->speed=a.speed;
		this->speed0=a.speed0;
		this->x0=a.x0;
		this->y0=a.y0;
		this->z0=a.z0;

		this->m_pitch=a.m_pitch;
		this->m_roll=a.m_roll;
		this->m_yaw=a.m_yaw;
		this->m_mProj=a.m_mProj;
		this->m_mView=a.m_mView;
		this->m_mWorld=a.m_mWorld;
    }
	Camera2& Camera2::operator=(const Camera2 &a)
    {
		this->_cameraType=a._cameraType;
		this->_look0=a._look0;
		this->_pos0=a._pos0;
		this->_right0=a._right0;
		this->_up0=a._up0;
		this->_look=a._look;
		this->_pos=a._pos;
		this->_right=a._right;
		this->_up=a._up;
		this->m_fSpinX=a.m_fSpinX;
		this->m_fSpinY=a.m_fSpinY;
		this->m_fSpinZ=a.m_fSpinZ;

		this->speed=a.speed;
		this->speed0=a.speed0;
		this->x0=a.x0;
		this->y0=a.y0;
		this->z0=a.z0;

		this->m_pitch=a.m_pitch;
		this->m_roll=a.m_roll;
		this->m_yaw=a.m_yaw;
		this->m_mProj=a.m_mProj;
		this->m_mView=a.m_mView;
		this->m_mWorld=a.m_mWorld;

		return *this;
    }




void Camera2::getPosition(D3DXFROMWINEVECTOR3* pos)
{
	*pos = _pos;
}

void Camera2::setPosition(D3DXFROMWINEVECTOR3* pos)
{
	_pos = *pos;
}

void Camera2::getRight(D3DXFROMWINEVECTOR3* right)
{
	*right = _right;
}

void Camera2::getUp(D3DXFROMWINEVECTOR3* up)
{
	*up = _up;
}



void Camera2::setLook(D3DXFROMWINEVECTOR3* look)
{
	 _look = *look;
}

void Camera2::getLook(D3DXFROMWINEVECTOR3* look)
{
	*look = _look;
}

void Camera2::walk(float units)
{
	// move only on xz plane for land object
	if( _cameraType == LANDOBJECT )
		_pos += D3DXFROMWINEVECTOR3(_look.x, 0.0f, _look.z) * units;

	if( _cameraType == AIRCRAFT )
		_pos += _look * units;
}

void Camera2::strafe(float units)
{
	// move only on xz plane for land object
	if( _cameraType == LANDOBJECT )
		_pos += D3DXFROMWINEVECTOR3(_right.x, 0.0f, _right.z) * units;

	if( _cameraType == AIRCRAFT )
		_pos += _right * units;
}

void Camera2::fly(float units)
{
	// move only on y-axis for land object
	if( _cameraType == LANDOBJECT )
		_pos.y += units;

	if( _cameraType == AIRCRAFT )
		_pos += _up * units;
}

void Camera2::pitch(float angle)
{
	D3DXFROMWINEMATRIX T;
	D3DXFROMWINEMatrixRotationAxis(&T, &_right,	angle);

	// rotate _up and _look around _right vector
	D3DXFROMWINEVec3TransformCoord(&_up,&_up, &T);
	D3DXFROMWINEVec3TransformCoord(&_look,&_look, &T);
	m_yaw += angle;
}

void Camera2::yaw(float angle)
{
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
	m_pitch += angle;
}

void Camera2::roll(float angle)
{
	// only roll for aircraft type
	if( _cameraType == AIRCRAFT )
	{
		D3DXFROMWINEMATRIX T;
		D3DXFROMWINEMatrixRotationAxis(&T, &_look,	angle);

		// rotate _up and _right around _look vector
		D3DXFROMWINEVec3TransformCoord(&_right,&_right, &T);
		D3DXFROMWINEVec3TransformCoord(&_up,&_up, &T);
		m_roll += angle;
	}
}

void Camera2::getViewMatrix(D3DXFROMWINEMATRIX* V)
{
	D3DXFROMWINEMatrixIdentity(V);
	// Keep camera's axes orthogonal to eachother
	D3DXFROMWINEVec3Normalize(&_look, &_look);

	//_right=-_right;//???

	D3DXFROMWINEVec3Cross(&_up, &_look, &_right);
	//D3DXFROMWINEVec3Cross(&_up, &_right, &_look);
	D3DXFROMWINEVec3Normalize(&_up, &_up);

	//_up.y=-_up.y;//???

	D3DXFROMWINEVec3Cross(&_right, &_up, &_look);
	D3DXFROMWINEVec3Normalize(&_right, &_right);
/*
	D3DXFROMWINEMATRIX pout;
	D3DXFROMWINEMatrixRotationZ(&pout, PI);
	D3DXFROMWINEVec3TransformCoord(&_look, &_look, &pout);
	D3DXFROMWINEVec3TransformCoord(&_right, &_right, &pout);
	D3DXFROMWINEVec3TransformCoord(&_up, &_up, &pout);
*/
	// Build the view matrix:

	float x = -D3DXFROMWINEVec3Dot(&_right, &_pos);
	float y = -D3DXFROMWINEVec3Dot(&_up, &_pos);
	float z = -D3DXFROMWINEVec3Dot(&_look, &_pos);

	(*V)(0,0) = _right.x; (*V)(0, 1) = _up.x; (*V)(0, 2) = _look.x; (*V)(0, 3) = 0.0f;
	(*V)(1,0) = _right.y; (*V)(1, 1) = _up.y; (*V)(1, 2) = _look.y; (*V)(1, 3) = 0.0f;
	(*V)(2,0) = _right.z; (*V)(2, 1) = _up.z; (*V)(2, 2) = _look.z; (*V)(2, 3) = 0.0f;
	(*V)(3,0) = x;	      (*V)(3, 1) = y;	  (*V)(3, 2) = z;	(*V)(3, 3) = 1.0f;

}

void Camera2::setCameraType(CameraType cameraType)
{
	_cameraType = cameraType;
}

void Camera2::getQuaternion(D3DXFROMWINEQUATERNION* q)
{
	D3DXFROMWINEQuaternionRotationYawPitchRoll(q,D3DXFROMWINEToRadian(m_yaw), D3DXFROMWINEToRadian(m_pitch), D3DXFROMWINEToRadian(m_roll));//???
}
void Camera2::init(IInput* Input)
{
		float f = 0.0f;
		//LuaObject tmp = SLua::state->GetGlobal("x");
		//if(tmp.IsNumber())
		//	f = float(tmp.GetNumber());
		x0=0.0f;
		//tmp = SLua::state->GetGlobal("z");
		//if(tmp.IsNumber())
		//	f = float(tmp.GetNumber());
		z0=0.0f;
		f = 0.0f;
		//tmp = SLua::state->GetGlobal("y");
		//if(tmp.IsNumber())
		//	f = float(tmp.GetNumber());
		y0=f;
		f=0.01f;
		//tmp = SLua::state->GetGlobal("speed");
		//if(tmp.IsNumber())
		//	f = float(tmp.GetNumber());
		speed0=f;
		f = 90.0f;
		//tmp = SLua::state->GetGlobal("sx");
		//if(tmp.IsNumber())
		//	f = float(tmp.GetNumber());
		m_fSpinX=f;
		//tmp = SLua::state->GetGlobal("sy");
		//if(tmp.IsNumber())
		//	f = float(tmp.GetNumber());
		m_fSpinY=f;
		//tmp = SLua::state->GetGlobal("sz");
		//if(tmp.IsNumber())
		//	f = float(tmp.GetNumber());
		m_fSpinZ=f;

		_pos0=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);//D3DXFROMWINEVECTOR3(x0, y0, z0);
		_look0=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
		reset(Input);
}
#if 1
void Camera2::reset(IInput* Input)
{
	D3DXFROMWINEMatrixIdentity(&m_mWorld);
	_pos   = _pos0;
	_right = _right0;//D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f);
	_up    = _up0;//D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);
	_look  = _look0;//D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
	speed=speed0;
	//m_pitch  = m_fSpinX;//(3.0f * D3DXFROMWINE_PI) / 2.0f;
	//m_yaw = m_fSpinY;//D3DXFROMWINE_PI/2;
	//m_roll = m_fSpinZ;//D3DXFROMWINE_PI/2;
#if 1
	if(1)
	{
		_pos   = _pos0;
		Input->GetSpinX()=0.0f;
		Input->GetSpinY()=0.0f;
		Input->GetX()=0.0f;
		Input->GetY()=0.0f;
		Input->GetZ()=0.0f;
	}
	else// if(0)
	{
		_pos   = D3DXFROMWINEVECTOR3(18868.1f,-14433.0f,15798.1f);
		Input->GetSpinX()=0.0f;
		Input->GetSpinY()=0.0f;
		Input->GetX()=0.0f;
		Input->GetY()=0.0f;
		Input->GetZ()=0.0f;
	}
#endif
	D3DXFROMWINEVECTOR3* pvEyePt=&_pos;
	D3DXFROMWINEVECTOR3* pvLookatPt=&_look;

    // Calc the view matrix
    D3DXFROMWINEVECTOR3 vUp(0,1,0);
    D3DXFROMWINEMatrixLookAtLH( &m_mView, pvEyePt, pvLookatPt, &vUp );

    D3DXFROMWINEMATRIX mInvView;
    D3DXFROMWINEMatrixInverse( &mInvView, 0, &m_mView );

    // The axis basis vectors and camera position are stored inside the
    // position matrix in the 4 rows of the camera's world matrix.
    // To figure out the yaw/pitch of the camera, we just need the Z basis vector
    D3DXFROMWINEVECTOR3* pZBasis = (D3DXFROMWINEVECTOR3*) &mInvView.m[2][0];

    m_yaw   = atan2f( pZBasis->x, pZBasis->z );
    float fLen = sqrtf(pZBasis->z*pZBasis->z + pZBasis->x*pZBasis->x);
    m_pitch = -atan2f( pZBasis->y, fLen );
}
#else
void Camera2::reset()
{
	speed=speed0;

	if(1)
	{
		_pos   = _pos0;
		g_fSpinX=0.0f;
		g_fSpinY=0.0f;
		g_fX=0.0f;
		g_fY=0.0f;
		g_fZ=0.0f;
	}
	else// if(0)
	{
		_pos   = D3DXFROMWINEVECTOR3(18868.1f,-14433.0f,15798.1f);
		g_fSpinX=0.0f;
		g_fSpinY=0.0f;
		g_fX=0.0f;
		g_fY=0.0f;
		g_fZ=0.0f;
	}
	SetViewParams( _pos0, _look0 );
}
#endif
bool Camera2::controls(D3DXFROMWINEVECTOR3* pos,D3DXFROMWINEQUATERNION* q,IInput* Input)
{
	bool r=false;
#if 1
	if( Input->OnKeyPress(STX_KEY_HOME ))
	{
	  reset(Input);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_g ))
	{
	  speed *= 10.0f;
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_b ))
	{
	  speed /= 10.0f;
	  r=r||true;
	}

	float units=100.0f*speed;

	if( Input->OnKeyPress(STX_KEY_PAGEUP))
	{
		m_pitch -= 0.5f * speed;
		pitch(m_pitch);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_PAGEDOWN))
	{
		m_pitch += 0.5f * speed;
		pitch(m_pitch);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_LEFT))
	{
		m_yaw -= 0.5f * speed;
		yaw(m_yaw);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_RIGHT))
	{
		m_yaw += 0.5f * speed;
		yaw(m_yaw);
	  r=r||true;
	}
/*	if( Input->OnKeyPress(STX_KEY_PRIOR))
	{
		m_roll -= 0.5f * speed;
		roll(m_roll);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_NEXT))
	{
		m_roll += 0.5f * speed;
		roll(m_roll);
	  r=r||true;
	}*/
	if( Input->OnKeyPress(STX_KEY_w))
	{
		walk(units);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_s))
	{
		walk(-units);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_r))
	{
		fly(units);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_f))
	{
		fly(-units);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_d))
	{
		strafe(units);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_a))
	{
		strafe(-units);
	  r=r||true;
	}

	getPosition(pos);
	//setPosition(pos);
	getQuaternion(q);
	getViewMatrix(&m_mView);
	//D3DXFROMWINEMatrixInverse( &m_mWorld, 0, &m_mView );
#endif
	return r;
}

bool Camera2::controls2(D3DXFROMWINEVECTOR3* pos,D3DXFROMWINEQUATERNION* qI,IInput* Input)
{
	bool r=false;
#if 1
	if( Input->OnKeyPress(STX_KEY_HOME ))
	{
	  reset(Input);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_g ))
	{
	  speed *= 10.0f;
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_b ))
	{
	  speed /= 10.0f;
	  r=r||true;
	}

//
// Update: Update the camera.
//
	float timeDelta=100.0f*speed;
	if( Input->OnKeyPress(STX_KEY_w))
	{
		walk(4.0f * timeDelta);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_s))
	{
		walk(-4.0f * timeDelta);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_a))
	{
		strafe(-4.0f * timeDelta);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_d))
	{
		strafe(4.0f * timeDelta);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_r))
	{
		fly(4.0f * timeDelta);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_f))
	{
		fly(-4.0f * timeDelta);
	  r=r||true;
	}
	timeDelta=100.0f*speed;
	if( Input->OnKeyPress(STX_KEY_PAGEDOWN))
	{
		pitch(1.0f * timeDelta);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_PAGEUP))
	{
		pitch(-1.0f * timeDelta);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_LEFT))
	{
		yaw(-1.0f * timeDelta);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_RIGHT))
	{
		yaw(1.0f * timeDelta);
	  r=r||true;
	}/*
	if( Input->OnKeyPress(STX_KEY_NEXT))
	{
		roll(1.0f * timeDelta);
	  r=r||true;
	}
	if( Input->OnKeyPress(STX_KEY_PRIOR))
	{
		roll(-1.0f * timeDelta);
	  r=r||true;
	}*/
	/*
	getPosition(pos);
	//setPosition(pos);
	getQuaternion(q);
	*/
	/*
	// Update the view matrix representing the cameras
	// new position/orientation.
	D3DXFROMWINEMATRIX V;
	getViewMatrix(&V);
	V(g_pd3dDevice10->SetTransform(D3DTS_VIEW, &V);
	*/
	getViewMatrix(&m_mView);
	//D3DXFROMWINEMatrixInverse( &m_mWorld, 0, &m_mView );
#endif
	return r;
}
#endif
#if 0
int sym[]={
VK_BACK,		SDLK_BACKSPACE,
VK_TAB,			SDLK_TAB,

VK_CLEAR,		SDLK_CLEAR,
VK_RETURN,		SDLK_RETURN,

VK_MENU,		SDLK_MENU,
VK_PAUSE,		SDLK_PAUSE,

VK_ESCAPE,		SDLK_ESCAPE,

VK_SPACE,		SDLK_SPACE,
VK_PRIOR,		SDLK_PAGEUP,//???
VK_NEXT,		SDLK_PAGEDOWN,//???
VK_END,			SDLK_END,
VK_HOME,		SDLK_HOME,
VK_LEFT,		SDLK_LEFT,
VK_UP,			SDLK_UP,
VK_RIGHT,		SDLK_RIGHT,
VK_DOWN,		SDLK_DOWN,
VK_PRINT,		SDLK_PRINT,
VK_INSERT,		SDLK_INSERT,
VK_DELETE,		SDLK_DELETE,
VK_HELP,		SDLK_HELP,

VK_NUMPAD0,		SDLK_KP0,
VK_NUMPAD1,		SDLK_KP1,
VK_NUMPAD2,		SDLK_KP2,
VK_NUMPAD3,		SDLK_KP3,
VK_NUMPAD4,		SDLK_KP4,
VK_NUMPAD5,		SDLK_KP5,
VK_NUMPAD6,		SDLK_KP6,
VK_NUMPAD7,		SDLK_KP7,
VK_NUMPAD8,		SDLK_KP8,
VK_NUMPAD9,		SDLK_KP9,
VK_MULTIPLY,	SDLK_KP_MULTIPLY,
VK_ADD,			SDLK_KP_PLUS,
VK_SEPARATOR,	SDLK_KP_EQUALS,
VK_SUBTRACT,	SDLK_KP_MINUS,
VK_DECIMAL,		SDLK_KP_PERIOD,
VK_DIVIDE,		SDLK_KP_DIVIDE,
VK_F1,			SDLK_F1,
VK_F2,			SDLK_F2,
VK_F3,			SDLK_F3,
VK_F4,			SDLK_F4,
VK_F5,			SDLK_F5,
VK_F6,			SDLK_F6,
VK_F7,			SDLK_F7,
VK_F8,			SDLK_F8,
VK_F9,			SDLK_F9,
VK_F10,			SDLK_F10,
VK_F11,			SDLK_F11,
VK_F12,			SDLK_F12,
VK_F13,			SDLK_F13,
VK_F14,			SDLK_F14,
VK_F15,			SDLK_F15,

VK_NUMLOCK,		SDLK_NUMLOCK,
VK_SCROLL,		SDLK_SCROLLOCK,

VK_LSHIFT,		SDLK_LSHIFT,
VK_RSHIFT,		SDLK_RSHIFT,
VK_LCONTROL,	SDLK_LCTRL,
VK_RCONTROL,	SDLK_RCTRL,

'0',				SDLK_0,
'1',				SDLK_1,
'2',				SDLK_2,
'3',				SDLK_3,
'4',				SDLK_4,
'5',				SDLK_5,
'6',				SDLK_6,
'7',				SDLK_7,
'8',				SDLK_8,
'9',				SDLK_9,

'A',				SDLK_a,
'B',				SDLK_b,
'C',				SDLK_c,
'D',				SDLK_d,
'E',				SDLK_e,
'F',				SDLK_f,
'G',				SDLK_g,
'H',				SDLK_h,
'I',				SDLK_i,
'J',				SDLK_j,
'K',				SDLK_k,
'L',				SDLK_l,
'M',				SDLK_m,
'N',				SDLK_n,
'O',				SDLK_o,
'P',				SDLK_p,
'Q',				SDLK_q,
'R',				SDLK_r,
'S',				SDLK_s,
'T',				SDLK_t,
'U',				SDLK_u,
'V',				SDLK_v,
'W',				SDLK_w,
'X',				SDLK_x,
'Y',				SDLK_y,
'Z',				SDLK_z

//VK_SHIFT,		KMOD_SHIFT,
//VK_CONTROL,		KMOD_CTRL,
//VK_CAPITAL,		KMOD_CAPS
}

#endif


