/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#if 1
#ifndef _Camera2_H
#define _Camera2_H

#include <Framework3/IRenderer.h>
#if 0
struct BoundingBox
{
	D3DXFROMWINEVECTOR3 Min;
	D3DXFROMWINEVECTOR3 Max;
};
enum BoundingFrustumEnum {
	ContainmentTypeDisjoint
};
struct BoundingFrustum
{
	BoundingFrustum(){}
	virtual ~BoundingFrustum(){}

	BoundingFrustumEnum Contains(BoundingBox& box)
	{//???
		return ContainmentTypeDisjoint;
	}

};
#endif
class IInput;
class Camera2
{
public:
	//BoundingFrustum ViewFrustum;
	enum CameraType { LANDOBJECT, AIRCRAFT };

	Camera2(IInput* Input);
	Camera2(const Camera2 &a);
	Camera2& operator =(const Camera2 &a);

	Camera2(CameraType cameraType,IInput* Input);
	~Camera2();

	void strafe(float units); // left/right
	void fly(float units);	  // up/down
	void walk(float units);   // forward/backward

	void pitch(float angle); // rotate on right vector
	void yaw(float angle);	 // rotate on up vector
	void roll(float angle);  // rotate on look vector

	void getViewMatrix(D3DXFROMWINEMATRIX* V);
	void setCameraType(CameraType cameraType);
	void getPosition(D3DXFROMWINEVECTOR3* pos);
	void setPosition(D3DXFROMWINEVECTOR3* pos);

	void getRight(D3DXFROMWINEVECTOR3* right);
	void getUp(D3DXFROMWINEVECTOR3* up);
	void setLook(D3DXFROMWINEVECTOR3* look);
	void getLook(D3DXFROMWINEVECTOR3* look);

	void getQuaternion(D3DXFROMWINEQUATERNION* q);
	bool controls(D3DXFROMWINEVECTOR3* pos,D3DXFROMWINEQUATERNION* q,IInput* Input=0);
	bool controls2(D3DXFROMWINEVECTOR3* pos,D3DXFROMWINEQUATERNION* q,IInput* Input=0);
	bool controls3(IInput* Input=0)
	{
		D3DXFROMWINEVECTOR3 pos;
		D3DXFROMWINEQUATERNION q;
		bool b=controls2(&pos,&q,Input);
	D3DXFROMWINEVECTOR3 e=D3DXFROMWINEVECTOR3(_pos.x,_pos.y,_pos.z);
	D3DXFROMWINEVECTOR3 l=D3DXFROMWINEVECTOR3(_look.x,_look.y,_look.z);
	D3DXFROMWINEVECTOR3 u=D3DXFROMWINEVECTOR3(_up.x,_up.y,_up.z);
	IRenderer::GetRendererInstance()->buildLookAtMatrix(e,l,u);
		return b;
	}/*
	void controls4(float x, float y, float z, float yaw, float pitch, float roll)
	{
	 D3DXFROMWINEVECTOR3 vpos(x,y,z);
	 D3DXFROMWINEQUATERNION q;
	 D3DXFROMWINEQuaternionRotationYawPitchRoll(&q,D3DXFROMWINEToRadian(yaw), D3DXFROMWINEToRadian(pitch), D3DXFROMWINEToRadian(roll));
	 g_pCamera->controls2(&vpos,&q);//???
	}*/
	void init(IInput* Input=0);
	void reset(IInput* Input=0);

	void SetProjParams( float fradian, float fAspectRatio, float fnear, float ffar )
	{
			m_fradian=fradian;
			m_fAspectRatio=fAspectRatio;
			m_fnear=fnear;
			m_ffar=ffar;
#if 0
		D3DXFROMWINEMatrixPerspectiveFovLH
			(
			&m_mProj,
			fradian,
			fAspectRatio,
			fnear,
			ffar
			);
#elif 0
	IRenderer::GetRendererInstance()->buildPerspectiveMatrix(
  fradian*180.0f/PI,
			fAspectRatio,
			fnear,
			ffar);
	mat4 p=IRenderer::GetRendererInstance()->Getp();
	stx_memcpy(&m_mProj,&p,16*sizeof(float));
#elif 1
    D3DXFROMWINEMATRIX matProj;
    D3DXFROMWINEMatrixPerspectiveFovLH( &matProj, fradian,
			fAspectRatio,
			fnear,
			ffar
								);
    //IRenderer::GetRendererInstance()->SetTransform( TS_PROJECTION, &matProj );
	m_mProj=matProj;
#endif
	}
#if 1
	void SetViewParams( 
		D3DXFROMWINEVECTOR3&  pvEyePt, 
		D3DXFROMWINEVECTOR3& pvLookatPt )
	{
		//D3DXFROMWINEMATRIX m_mView;
		//D3DXFROMWINEMatrixLookAtLH( &m_mView, &vEye, &vAt, &D3DXFROMWINEVECTOR3(0.0f,1.0f,0.0f) );

    _pos0 = _pos = pvEyePt;
    _look0 = _look = pvLookatPt;

    // Calc the view matrix
    D3DXFROMWINEVECTOR3 vUp(0,1,0);
	_up0=_up=vUp;
#if 0
	D3DXFROMWINEMatrixLookAtLH( &m_mView, &pvEyePt, &pvLookatPt, &vUp );
#else
getViewMatrix(&m_mView);
#endif
	D3DXFROMWINEVECTOR3 e=D3DXFROMWINEVECTOR3(pvEyePt.x,pvEyePt.y,pvEyePt.z);
	D3DXFROMWINEVECTOR3 l=D3DXFROMWINEVECTOR3(pvLookatPt.x,pvLookatPt.y,pvLookatPt.z);
	D3DXFROMWINEVECTOR3 u=D3DXFROMWINEVECTOR3(vUp.x,vUp.y,vUp.z);
	IRenderer::GetRendererInstance()->buildLookAtMatrix(e,l,u);

    D3DXFROMWINEMATRIX mInvView;
    D3DXFROMWINEMatrixInverse( &mInvView, 0, &m_mView );

    // The axis basis vectors and camera position are stored inside the
    // position matrix in the 4 rows of the camera's world matrix.
    // To figure out the yaw/pitch of the camera, we just need the Z basis vector
    D3DXFROMWINEVECTOR3* pZBasis = (D3DXFROMWINEVECTOR3*) &mInvView.m[2][0];

    m_yaw   = atan2f( pZBasis->x, pZBasis->z );
    float fLen = sqrtf(pZBasis->z*pZBasis->z + pZBasis->x*pZBasis->x);
    m_pitch = -atan2f( pZBasis->y, fLen );
	m_roll=0.0f;

		_up0=_up=D3DXFROMWINEVECTOR3(0.0f,1.0f,0.0f);
/*
	(*V)(0,0) = _right.x; (*V)(0, 1) = _up.x; (*V)(0, 2) = _look.x; (*V)(0, 3) = 0.0f;
	(*V)(1,0) = _right.y; (*V)(1, 1) = _up.y; (*V)(1, 2) = _look.y; (*V)(1, 3) = 0.0f;
	(*V)(2,0) = _right.z; (*V)(2, 1) = _up.z; (*V)(2, 2) = _look.z; (*V)(2, 3) = 0.0f;
	(*V)(3,0) = x;	      (*V)(3, 1) = y;	  (*V)(3, 2) = z;	(*V)(3, 3) = 1.0f;
*/
		_right0=_right=D3DXFROMWINEVECTOR3(m_mView.m[0][1],m_mView.m[1][1],m_mView.m[2][1]);

  D3DXFROMWINEMATRIX matView;
                     D3DXFROMWINEVECTOR3 v1(pvEyePt.x, pvEyePt.y, pvEyePt.z);
                     D3DXFROMWINEVECTOR3 v2(pvLookatPt.x, pvLookatPt.y, pvLookatPt.z);
                     D3DXFROMWINEVECTOR3 v3(0.0f, 1.0f, 0.0f);
  D3DXFROMWINEMatrixLookAtLH(&matView, &v1, &v2, &v3);
  //IRenderer::GetRendererInstance()->SetTransform(TS_VIEW, &matView);
m_mView=matView;

	}
#else
	void SetViewParams( D3DXFROMWINEVECTOR3&  pvEyePt, D3DXFROMWINEVECTOR3& pvLookatPt )
	{
	D3DXFROMWINEMatrixIdentity(&m_mWorld);
    _pos0 = pvEyePt;
    _look0 = pvLookatPt;
	_up0=_up=D3DXFROMWINEVECTOR3(0,1,0);
	_pos = _pos0;
    _look = _look0;
    D3DXFROMWINEMatrixLookAtLH( &m_mView, &pvEyePt, &pvLookatPt, &_up );
#if 1
	_right0=_right=D3DXFROMWINEVECTOR3(m_mView.m[0][1],m_mView.m[1][1],m_mView.m[2][1]);
#elif 0
	resetMatrices();//pvEyePt, pvLookatPt);
	}
	void resetMatrices()//D3DXFROMWINEVECTOR3&  pvEyePt, D3DXFROMWINEVECTOR3& pvLookatPt)
	{
    // Calc the view matrix
    D3DXFROMWINEMatrixLookAtLH( &m_mView, &_pos, &_look, &_up );
/*
	(*V)(0,0) = _right.x; (*V)(0, 1) = _up.x; (*V)(0, 2) = _look.x; (*V)(0, 3) = 0.0f;
	(*V)(1,0) = _right.y; (*V)(1, 1) = _up.y; (*V)(1, 2) = _look.y; (*V)(1, 3) = 0.0f;
	(*V)(2,0) = _right.z; (*V)(2, 1) = _up.z; (*V)(2, 2) = _look.z; (*V)(2, 3) = 0.0f;
	(*V)(3,0) = x;	      (*V)(3, 1) = y;	  (*V)(3, 2) = z;	(*V)(3, 3) = 1.0f;
*/
		_right0=_right=D3DXFROMWINEVECTOR3(m_mView.m[0][1],m_mView.m[1][1],m_mView.m[2][1]);
#endif
    D3DXFROMWINEMATRIX mInvView;
    D3DXFROMWINEMatrixInverse( &mInvView, 0, &m_mView );

    // The axis basis vectors and camera position are stored inside the
    // position matrix in the 4 rows of the camera's world matrix.
    // To figure out the yaw/pitch of the camera, we just need the Z basis vector
    D3DXFROMWINEVECTOR3* pZBasis = (D3DXFROMWINEVECTOR3*) &mInvView._31;

    m_yaw   = atan2f( pZBasis->x, pZBasis->z );
    float fLen = sqrtf(pZBasis->z*pZBasis->z + pZBasis->x*pZBasis->x);
    m_pitch = -atan2f( pZBasis->y, fLen );
	m_roll=0.0f;

/*
	D3DXFROMWINEVec3Cross(&_right, &_up, &_look);
	D3DXFROMWINEVec3Normalize(&_right, &_right);
*/
/*
	D3DXFROMWINEVec3Cross(&_up0, &_look0, &_right0);
	D3DXFROMWINEVec3Normalize(&_up0, &_up0);
	_up=_up0;
*/
	}
#endif
	D3DXFROMWINEMATRIX GetWorldMatrix(){return m_mWorld;}
	D3DXFROMWINEMATRIX GetProjMatrix(){return m_mProj;}
	D3DXFROMWINEMATRIX GetViewMatrix(){return m_mView;}//D3DXFROMWINEMATRIX V;getViewMatrix(&V);return V;}
void elevate(float speed)
{
  //_pos.y += units;
	_pos.y	+= speed;
  //focus.y	+= speed;
}
public:
	D3DXFROMWINEMATRIX m_mWorld;
	D3DXFROMWINEMATRIX m_mProj;
	D3DXFROMWINEMATRIX m_mView;
	CameraType  _cameraType;
	D3DXFROMWINEVECTOR3 _right;
	D3DXFROMWINEVECTOR3 _up;
	D3DXFROMWINEVECTOR3 _look;
	D3DXFROMWINEVECTOR3 _pos;
	D3DXFROMWINEVECTOR3 _right0;
	D3DXFROMWINEVECTOR3 _up0;
	D3DXFROMWINEVECTOR3 _look0;
	D3DXFROMWINEVECTOR3 _pos0;
	float x0,y0,z0,speed,speed0;
	float m_yaw  ,m_fSpinX;//(3.0f * D3DXFROMWINE_PI) / 2.0f;
	float m_pitch ,m_fSpinY;//D3DXFROMWINE_PI/2;
	float m_roll ,m_fSpinZ;//D3DXFROMWINE_PI/2;

	float m_fradian;
	float m_fAspectRatio;
	float m_fnear;
	float m_ffar;

};//end class Camera2
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//extern Camera2 g_Camera;

#endif
#endif
