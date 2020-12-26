/*
 * Copyright (C) 2007 David Adam
 * Copyright (C) 2007 Tony Wasserka
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//include <Framework3/Platform.h>

#ifndef __D3DXFROMWINE9MATH_H__
#define __D3DXFROMWINE9MATH_H__

#include <STX/STX.h>

#ifdef stx_max
#undef stx_max
#endif
#define stx_max(x, y)	(((x) > (y)) ? (x) : (y))
//define max std::max

#ifdef stx_min
#undef stx_min
#endif
#define stx_min(x, y)	(((x) < (y)) ? (x) : (y))
//define min std::min

#if defined(_WIN32) && !defined(__MINGW32__)
#if 0
	#include <mmsystem.h>
// ShellExecute()
#include <shellapi.h>
#include <commctrl.h>
// GetOpenFileName()
#include <Commdlg.h>
#elif !defined(WIN8_APP) && !defined(WIN_PHONE_APP) 
#include <shellapi.h>
#include <Winbase.h>//include <windows.h>
#include <Wingdi.h>//include <windows.h>
#include <Winuser.h>
#include <Commctrl.h>
#include <Commdlg.h>//include <windows.h>
#include <Winbase.h>//include <windows.h>
#include <Mmsystem.h>//include <windows.h>
//???include <Shlobj.h>
#endif

#else
#ifndef BASETYPES
#define BASETYPES
typedef unsigned int ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;

#define MAX_PATH          260

#if 0
#ifdef __cplusplus
#define 0    0
#else
#define 0    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#undef far
#undef near
#undef pascal

#define far
#define near
#if (!defined(_MAC)) && ((_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED))
#define pascal __stdcall
#else
#define pascal
#endif

#if defined(DOSWIN32) || defined(_MAC)
#define cdecl _cdecl
#ifndef CDECL
#define CDECL _cdecl
#endif
#else
#define cdecl
#ifndef CDECL
#define CDECL
#endif
#endif

#ifdef _MAC
#define CALLBACK    PASCAL
#define WINAPI      CDECL
#define WINAPIV     CDECL
#define APIENTRY    WINAPI
#define APIPRIVATE  CDECL
#ifdef _68K_
#define PASCAL      __pascal
#else
#define PASCAL
#endif
#elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define CALLBACK    __stdcall
#define WINAPI      __stdcall
#define WINAPIV     __cdecl
#define APIENTRY    WINAPI
#define APIPRIVATE  __stdcall
#define PASCAL      __stdcall
#else
#define CALLBACK
#define WINAPI
#define WINAPIV
#undef APIENTRY
#define APIENTRY    WINAPI
#define APIPRIVATE
#define PASCAL      pascal
#endif

#ifdef _M_CEE_PURE
#define WINAPI_INLINE  __clrcall
#else
#define WINAPI_INLINE  WINAPI
#endif

#undef FAR
#undef  NEAR
#define FAR                 far
#define NEAR                near
#ifndef CONST
#define CONST               const
#endif

#ifdef _MSC_VER
typedef unsigned int       __DWORD__;
#else
typedef unsigned int        __DWORD__;
#endif
typedef unsigned short __WORD__;
typedef unsigned int __DWORD__;
typedef unsigned int __D3DCOLOR__;

typedef int                 BOOL;
typedef unsigned char       UBYTE;
typedef unsigned char       BYTE;
typedef unsigned short      __WORD__;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL near           *PBOOL;
typedef BOOL far            *LPBOOL;
typedef BYTE near           *PBYTE;
typedef BYTE far            *LPBYTE;
typedef int near            *PINT;
typedef int far             *LPINT;
typedef __WORD__ near           *PWORD;
typedef __WORD__ far            *LPWORD;
typedef int far            *LPLONG;
typedef __DWORD__ near          *PDWORD;
typedef __DWORD__ far           *LPDWORD;
typedef void far            *LPVOID;
typedef CONST void far      *LPCVOID;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

#ifndef _WIN32
typedef int                 LONG;
typedef struct  tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT, NEAR *NPPOINT, FAR *LPPOINT;

typedef char CHAR;
typedef char TCHAR;
#endif
typedef unsigned char byte;
typedef signed int          INT32;
typedef unsigned int        UINT32;

//
// ANSI (Multi-byte Character) types
//
typedef CHAR* PCHAR, *LPCH, *PCH;
typedef CONST CHAR *LPCCH, *PCCH;

#if defined(_WIN32) && !defined(__MINGW32__) && !defined(__MINGW64__)
typedef __nullterminated CHAR *NPSTR, *LPSTR, *PSTR;
typedef __nullterminated PSTR *PZPSTR;
typedef __nullterminated CONST PSTR *PCZPSTR;
//typedef __nullterminated CONST CHAR *LPCSTR, *PCSTR;
//typedef __nullterminated PCSTR *PZPCSTR;
#else
typedef CHAR *NPSTR, *LPSTR, *PSTR;
typedef PSTR *PZPSTR;
typedef CONST PSTR *PCZPSTR;
//typedef CONST CHAR *LPCSTR, *PCSTR;
//typedef PCSTR *PZPCSTR;
#endif

typedef void    VOID;

#ifndef NOMINMAX

#ifndef stx_max
#define stx_max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef stx_min
#define stx_min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX */
#endif
#if !defined(HRESULT) && !defined(__MINGW32__)
#define HRESULT int
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#define S_OK            ((HRESULT)0L)
#define S_FALSE   	((HRESULT)1L)
#define E_FAIL   	((HRESULT)1L)
#define E_INVALIDARG	(0x80070057L)
#endif

#endif  /* !BASETYPES */


#if defined(QT_BUILD) && !defined(__APPLE__)
#include <qmath.h>
#endif
#include <math.h>

#ifdef _WIN32
#include <float.h>  // for _isnan() on VC++
#define isnan(x) _isnan(x)  // VC++ uses _isnan() instead of isnan()
//#else
//#include <math.h>  // for isnan() everywhere else
#endif

#define D3DXFROMWINE_DEFAULT -1

#define D3DXFROMWINE_PI    ((FLOAT)3.141592654)
#define D3DXFROMWINE_1BYPI ((FLOAT)0.318309886)

#define D3DXFROMWINEToRadian(degree) ((degree) * (D3DXFROMWINE_PI / 180.0f))
#define D3DXFROMWINEToDegree(radian) ((radian) * (180.0f / D3DXFROMWINE_PI))

typedef struct _D3DFROMWINEVIEWPORT9 {
/*
     customViewports[0].TopLeftX = 0;
     customViewports[0].TopLeftY = 0;
     customViewports[0].MinDepth = 0.0f;
     customViewports[0].MaxDepth = 1.0f;
*/
union {
	__DWORD__       X;
	__DWORD__       TopLeftX;
	};
union {
    __DWORD__       Y;            /* Viewport Top left */
	__DWORD__       TopLeftY;
	};
    __DWORD__       Width;
    __DWORD__       Height;       /* Viewport Dimensions */
union {
    float       MinZ;         /* Min/max of clip Volume */
	float MinDepth;
	};
union {
    float       MaxZ;
	float MaxDepth;
	};
} D3DFROMWINEVIEWPORT9;

typedef struct  D3DXFROMWINEVECTOR2
{
#ifdef __cplusplus
	float angle(const D3DXFROMWINEVECTOR2& av)
	{
		float dot = x*av.x + y*av.y;// dot product between [x1, y1] and [x2, y2]
		float det = x*av.y - y*av.x;// determinant
		float angle = atan2(det, dot);// atan2(y, x) or atan2(sin, cos)
		return angle;
	}
	D3DXFROMWINEVECTOR2& set(FLOAT fx, FLOAT fy)
	{
    		x = fx;
    		y = fy;
		return *this;
	}


	FLOAT getLength()
	{
		CONST D3DXFROMWINEVECTOR2 *pv=this;
    		return sqrt( (pv->x) * (pv->x) + (pv->y) * (pv->y) );
	}
	D3DXFROMWINEVECTOR2& normalize();
    D3DXFROMWINEVECTOR2();
    D3DXFROMWINEVECTOR2(CONST FLOAT *pf);
    D3DXFROMWINEVECTOR2(FLOAT fx, FLOAT fy);
#if !defined(SWIG)
    operator FLOAT* ();
    operator CONST FLOAT* () const;
#endif

    D3DXFROMWINEVECTOR2& operator += (CONST D3DXFROMWINEVECTOR2&);
    D3DXFROMWINEVECTOR2& operator -= (CONST D3DXFROMWINEVECTOR2&);
    D3DXFROMWINEVECTOR2& operator *= (FLOAT);
    D3DXFROMWINEVECTOR2& operator /= (FLOAT);

    D3DXFROMWINEVECTOR2 operator + () const;
    D3DXFROMWINEVECTOR2 operator - () const;

    D3DXFROMWINEVECTOR2 operator + (CONST D3DXFROMWINEVECTOR2&) const;
    D3DXFROMWINEVECTOR2 operator - (CONST D3DXFROMWINEVECTOR2&) const;
    D3DXFROMWINEVECTOR2 operator * (FLOAT) const;
    D3DXFROMWINEVECTOR2 operator / (FLOAT) const;
#if !defined(SWIG)
    friend D3DXFROMWINEVECTOR2 operator * (FLOAT, CONST D3DXFROMWINEVECTOR2&);
#endif

    BOOL operator == (CONST D3DXFROMWINEVECTOR2&) const;
    BOOL operator != (CONST D3DXFROMWINEVECTOR2&) const;
/*
	void toString()
	{
		LOG_FNLN;
		LOG_PRINT("x=%f\n", x);
		LOG_PRINT("y=%f\n", y);
	}
*/
#endif /* __cplusplus */
    FLOAT x, y;
} D3DXFROMWINEVECTOR2, *LPD3DXFROMWINEVECTOR2;

#ifndef D3DFROMWINEVECTOR_DEFINED
typedef struct  _D3DFROMWINEVECTOR {
    float x;
    float y;
    float z;
} D3DFROMWINEVECTOR;
#define D3DFROMWINEVECTOR_DEFINED
#endif


#ifdef __cplusplus
typedef struct  D3DXFROMWINEVECTOR3 : public D3DFROMWINEVECTOR
{
	float angle(const D3DXFROMWINEVECTOR3& av)
	{
		float dot = x*av.x + y*av.y + z*av.z;// between [x1, y1, z1] and [x2, y2, z2]
		float lenSq1 = x*x + y*y + z*z;
		float lenSq2 = av.x*av.x + av.y*av.y + av.z*av.z; 
		float angle = acos(dot/sqrt(lenSq1 * lenSq2));
		return angle;
	}
	const float getDistance(const D3DXFROMWINEVECTOR3& v2) const;
	D3DXFROMWINEVECTOR3& set(FLOAT fx, FLOAT fy, FLOAT fz);
	FLOAT getLength();
	D3DXFROMWINEVECTOR3& normalize();
D3DXFROMWINEVECTOR3& crossProduct(CONST D3DXFROMWINEVECTOR3& v1, CONST D3DXFROMWINEVECTOR3& v2);
	FLOAT getDotProduct(CONST D3DXFROMWINEVECTOR3& a);

    D3DXFROMWINEVECTOR3();
    D3DXFROMWINEVECTOR3(CONST FLOAT *pf);
    D3DXFROMWINEVECTOR3(CONST D3DFROMWINEVECTOR& v);
    D3DXFROMWINEVECTOR3(FLOAT fx, FLOAT fy, FLOAT fz);
    D3DXFROMWINEVECTOR3(D3DXFROMWINEVECTOR2 v, FLOAT fz)
	{
		x=v.x;
		y=v.y;
		z=fz;
	}
#if !defined(SWIG)

    operator FLOAT* ();
    operator CONST FLOAT* () const;
#endif

    D3DXFROMWINEVECTOR3& operator += (CONST D3DXFROMWINEVECTOR3&);
    D3DXFROMWINEVECTOR3& operator -= (CONST D3DXFROMWINEVECTOR3&);
    D3DXFROMWINEVECTOR3& operator *= (FLOAT);
    D3DXFROMWINEVECTOR3& operator /= (FLOAT);

    D3DXFROMWINEVECTOR3 operator + () const;
    D3DXFROMWINEVECTOR3 operator - () const;

    D3DXFROMWINEVECTOR3 operator + (CONST D3DXFROMWINEVECTOR3&) const;
    D3DXFROMWINEVECTOR3 operator - (CONST D3DXFROMWINEVECTOR3&) const;
    D3DXFROMWINEVECTOR3 operator * (FLOAT) const;
    D3DXFROMWINEVECTOR3 operator / (FLOAT) const;
#if !defined(SWIG)

    friend D3DXFROMWINEVECTOR3 operator * (FLOAT, CONST struct  D3DXFROMWINEVECTOR3&);
#endif

    BOOL operator == (CONST D3DXFROMWINEVECTOR3&) const;
    BOOL operator != (CONST D3DXFROMWINEVECTOR3&) const;
/*
	void toString()
	{
		LOG_FNLN;
		LOG_PRINT("x=%f\n", x);
		LOG_PRINT("y=%f\n", y);
		LOG_PRINT("z=%f\n", z);
	}
*/
} D3DXFROMWINEVECTOR3, *LPD3DXFROMWINEVECTOR3;
#else /* !__cplusplus */
typedef struct  _D3DVECTOR D3DXFROMWINEVECTOR3, *LPD3DXFROMWINEVECTOR3;
#endif /* !__cplusplus */

typedef struct  D3DXFROMWINEVECTOR4
{
#ifdef __cplusplus
    D3DXFROMWINEVECTOR3 xyz(){return D3DXFROMWINEVECTOR3(x, y, z);}
	D3DXFROMWINEVECTOR4& set(FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw);
	FLOAT getLength();
	D3DXFROMWINEVECTOR4& normalize();

    D3DXFROMWINEVECTOR4();
    D3DXFROMWINEVECTOR4(CONST FLOAT *pf);
    D3DXFROMWINEVECTOR4(FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw);
    D3DXFROMWINEVECTOR4(D3DXFROMWINEVECTOR3& v, FLOAT fw)
	{
		x=v.x;
		y=v.y;
		z=v.z;
		w=fw;
	}
#if !defined(SWIG)

    operator FLOAT* ();
    operator CONST FLOAT* () const;
#endif

    D3DXFROMWINEVECTOR4& operator += (CONST D3DXFROMWINEVECTOR4&);
    D3DXFROMWINEVECTOR4& operator -= (CONST D3DXFROMWINEVECTOR4&);
    D3DXFROMWINEVECTOR4& operator *= (FLOAT);
    D3DXFROMWINEVECTOR4& operator /= (FLOAT);

    D3DXFROMWINEVECTOR4 operator + () const;
    D3DXFROMWINEVECTOR4 operator - () const;

    D3DXFROMWINEVECTOR4 operator + (CONST D3DXFROMWINEVECTOR4&) const;
    D3DXFROMWINEVECTOR4 operator - (CONST D3DXFROMWINEVECTOR4&) const;
    D3DXFROMWINEVECTOR4 operator * (FLOAT) const;
    D3DXFROMWINEVECTOR4 operator / (FLOAT) const;
#if !defined(SWIG)

    friend D3DXFROMWINEVECTOR4 operator * (FLOAT, CONST D3DXFROMWINEVECTOR4&);
#endif

    BOOL operator == (CONST D3DXFROMWINEVECTOR4&) const;
    BOOL operator != (CONST D3DXFROMWINEVECTOR4&) const;
/*
	void toString()
	{
		LOG_FNLN;
		LOG_PRINT("x=%f\n", x);
		LOG_PRINT("y=%f\n", y);
		LOG_PRINT("z=%f\n", z);
		LOG_PRINT("w=%f\n", w);
	}
*/
#endif /* __cplusplus */
    FLOAT x, y, z, w;
} D3DXFROMWINEVECTOR4, *LPD3DXFROMWINEVECTOR4;

#ifdef __cplusplus

typedef struct  D3DFROMWINEMATRIX {
   union {
        struct 
        {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;
        };
        float m[4][4];
        float m2[16];
    };
} D3DFROMWINEMATRIX;

struct  D3DXFROMWINEMATRIX;

static inline D3DXFROMWINEMATRIX* D3DXFROMWINEMatrixIdentity(D3DXFROMWINEMATRIX *pout);
inline D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixTranslation(D3DXFROMWINEMATRIX *pout, FLOAT x, FLOAT y, FLOAT z);

typedef struct  D3DXFROMWINEMATRIX : public D3DFROMWINEMATRIX
{
	D3DXFROMWINEVECTOR3 getRightVec();
	D3DXFROMWINEVECTOR3 getUpVec();
	D3DXFROMWINEVECTOR3 getForwardVec();
    void setIdentity(){D3DXFROMWINEMatrixIdentity(this);}
    void translate(D3DXFROMWINEVECTOR3 camPos){D3DXFROMWINEMatrixTranslation(this, camPos.x, camPos.y, camPos.z);}
    D3DXFROMWINEMATRIX();
    D3DXFROMWINEMATRIX(CONST FLOAT *pf);
    D3DXFROMWINEMATRIX(CONST D3DFROMWINEMATRIX& mat);
    D3DXFROMWINEMATRIX(FLOAT f11, FLOAT f12, FLOAT f13, FLOAT f14,
               FLOAT f21, FLOAT f22, FLOAT f23, FLOAT f24,
               FLOAT f31, FLOAT f32, FLOAT f33, FLOAT f34,
               FLOAT f41, FLOAT f42, FLOAT f43, FLOAT f44);
	void set(FLOAT f11, FLOAT f12, FLOAT f13, FLOAT f14,
               FLOAT f21, FLOAT f22, FLOAT f23, FLOAT f24,
               FLOAT f31, FLOAT f32, FLOAT f33, FLOAT f34,
               FLOAT f41, FLOAT f42, FLOAT f43, FLOAT f44)
        {
            _11=f11; _12=f12; _13=f13; _14=f14;
            _21=f21; _22=f22; _23=f23; _24=f24;
            _31=f31; _32=f32; _33=f33; _34=f34;
            _41=f41; _42=f42; _43=f43; _44=f44;
        };
    FLOAT& operator () (UINT row, UINT col);
#if !defined(SWIG)
    FLOAT operator () (UINT row, UINT col) const;

    operator FLOAT* ();
    operator CONST FLOAT* () const;
#endif

    D3DXFROMWINEMATRIX& operator *= (CONST D3DXFROMWINEMATRIX&);
    D3DXFROMWINEMATRIX& operator += (CONST D3DXFROMWINEMATRIX&);
    D3DXFROMWINEMATRIX& operator -= (CONST D3DXFROMWINEMATRIX&);
    D3DXFROMWINEMATRIX& operator *= (FLOAT);
    D3DXFROMWINEMATRIX& operator /= (FLOAT);

    D3DXFROMWINEMATRIX operator + () const;
    D3DXFROMWINEMATRIX operator - () const;

    D3DXFROMWINEMATRIX operator * (CONST D3DXFROMWINEMATRIX&) const;
    D3DXFROMWINEMATRIX operator + (CONST D3DXFROMWINEMATRIX&) const;
    D3DXFROMWINEMATRIX operator - (CONST D3DXFROMWINEMATRIX&) const;
    D3DXFROMWINEMATRIX operator * (FLOAT) const;
    D3DXFROMWINEMATRIX operator / (FLOAT) const;
#if !defined(SWIG)

    friend D3DXFROMWINEMATRIX operator * (FLOAT, CONST D3DXFROMWINEMATRIX&);
#endif
/*
	void toString()
	{
		LOG_FNLN;
		LOG_PRINT("m[%d][%d]=%f\n", 0, 0, m[0][0]);
		LOG_PRINT("m[%d][%d]=%f\n", 0, 1, m[0][1]);
		LOG_PRINT("m[%d][%d]=%f\n", 0, 2, m[0][2]);
		LOG_PRINT("m[%d][%d]=%f\n", 0, 3, m[0][3]);
		LOG_PRINT("m[%d][%d]=%f\n", 1, 0, m[1][0]);
		LOG_PRINT("m[%d][%d]=%f\n", 1, 1, m[1][1]);
		LOG_PRINT("m[%d][%d]=%f\n", 1, 2, m[1][2]);
		LOG_PRINT("m[%d][%d]=%f\n", 1, 3, m[1][3]);
		LOG_PRINT("m[%d][%d]=%f\n", 2, 0, m[2][0]);
		LOG_PRINT("m[%d][%d]=%f\n", 2, 1, m[2][1]);
		LOG_PRINT("m[%d][%d]=%f\n", 2, 2, m[2][2]);
		LOG_PRINT("m[%d][%d]=%f\n", 2, 3, m[2][3]);
		LOG_PRINT("m[%d][%d]=%f\n", 3, 0, m[3][0]);
		LOG_PRINT("m[%d][%d]=%f\n", 3, 1, m[3][1]);
		LOG_PRINT("m[%d][%d]=%f\n", 3, 2, m[3][2]);
		LOG_PRINT("m[%d][%d]=%f\n", 3, 3, m[3][3]);
	}
*/
    BOOL operator == (CONST D3DXFROMWINEMATRIX&) const;
    BOOL operator != (CONST D3DXFROMWINEMATRIX&) const;
	float& index(const unsigned int i, const unsigned int j) const {return *const_cast<float*>(&m[i][j]);}
} D3DXFROMWINEMATRIX, *LPD3DXFROMWINEMATRIX;
#else /* !__cplusplus */
typedef struct  _D3DMATRIX D3DXFROMWINEMATRIX, *LPD3DXFROMWINEMATRIX;
#endif /* !__cplusplus */

inline D3DXFROMWINEMATRIX translate(FLOAT x, FLOAT y, FLOAT z){D3DXFROMWINEMATRIX out;D3DXFROMWINEMatrixTranslation(&out, x, y, z);return out;}

#ifndef SWIG
typedef struct  D3DXFROMWINEQUATERNION
{
#ifdef __cplusplus
    D3DXFROMWINEQUATERNION();
    D3DXFROMWINEQUATERNION(CONST FLOAT *pf);
    D3DXFROMWINEQUATERNION(FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw);
#if !defined(SWIG)

    operator FLOAT* ();
    operator CONST FLOAT* () const;
#endif

    D3DXFROMWINEQUATERNION& operator += (CONST D3DXFROMWINEQUATERNION&);
    D3DXFROMWINEQUATERNION& operator -= (CONST D3DXFROMWINEQUATERNION&);
    D3DXFROMWINEQUATERNION& operator *= (CONST D3DXFROMWINEQUATERNION&);
    D3DXFROMWINEQUATERNION& operator *= (FLOAT);
    D3DXFROMWINEQUATERNION& operator /= (FLOAT);

    D3DXFROMWINEQUATERNION  operator + () const;
    D3DXFROMWINEQUATERNION  operator - () const;

    D3DXFROMWINEQUATERNION operator + (CONST D3DXFROMWINEQUATERNION&) const;
    D3DXFROMWINEQUATERNION operator - (CONST D3DXFROMWINEQUATERNION&) const;
    D3DXFROMWINEQUATERNION operator * (CONST D3DXFROMWINEQUATERNION&) const;
    D3DXFROMWINEQUATERNION operator * (FLOAT) const;
    D3DXFROMWINEQUATERNION operator / (FLOAT) const;
#if !defined(SWIG)

    friend D3DXFROMWINEQUATERNION operator * (FLOAT, CONST D3DXFROMWINEQUATERNION&);
#endif

    BOOL operator == (CONST D3DXFROMWINEQUATERNION&) const;
    BOOL operator != (CONST D3DXFROMWINEQUATERNION&) const;
#endif /* __cplusplus */
    FLOAT x, y, z, w;
} D3DXFROMWINEQUATERNION, *LPD3DXFROMWINEQUATERNION;
#endif

typedef struct  D3DXFROMWINEPLANE
{
#ifdef __cplusplus
    D3DXFROMWINEPLANE();
    D3DXFROMWINEPLANE(CONST FLOAT *pf);
    D3DXFROMWINEPLANE(FLOAT fa, FLOAT fb, FLOAT fc, FLOAT fd);
#if !defined(SWIG)

    operator FLOAT* ();
    operator CONST FLOAT* () const;
#endif

    D3DXFROMWINEPLANE operator + () const;
    D3DXFROMWINEPLANE operator - () const;

    BOOL operator == (CONST D3DXFROMWINEPLANE&) const;
    BOOL operator != (CONST D3DXFROMWINEPLANE&) const;
#endif /* __cplusplus */
    FLOAT a, b, c, d;
} D3DXFROMWINEPLANE, *LPD3DXFROMWINEPLANE;

#ifndef D3DFROMWINECOLORVALUE_DEFINED
typedef struct  _D3DFROMWINECOLORVALUE {
    float r;
    float g;
    float b;
    float a;
} D3DFROMWINECOLORVALUE;
#define D3DFROMWINECOLORVALUE_DEFINED
#endif

typedef struct  D3DXFROMWINECOLOR
{
#ifdef __cplusplus
    D3DXFROMWINECOLOR();
    D3DXFROMWINECOLOR(__DWORD__ col);
    D3DXFROMWINECOLOR(CONST FLOAT *pf);
    D3DXFROMWINECOLOR(CONST D3DFROMWINECOLORVALUE& col);
    D3DXFROMWINECOLOR(FLOAT fr, FLOAT fg, FLOAT fb, FLOAT fa);
#if !defined(SWIG)

    operator __DWORD__ () const;

    operator FLOAT* ();
    operator CONST FLOAT* () const;

    operator D3DFROMWINECOLORVALUE* ();
    operator CONST D3DFROMWINECOLORVALUE* () const;

    operator D3DFROMWINECOLORVALUE& ();
    operator CONST D3DFROMWINECOLORVALUE& () const;
#endif

    D3DXFROMWINECOLOR& operator += (CONST D3DXFROMWINECOLOR&);
    D3DXFROMWINECOLOR& operator -= (CONST D3DXFROMWINECOLOR&);
    D3DXFROMWINECOLOR& operator *= (FLOAT);
    D3DXFROMWINECOLOR& operator /= (FLOAT);

    D3DXFROMWINECOLOR operator + () const;
    D3DXFROMWINECOLOR operator - () const;

    D3DXFROMWINECOLOR operator + (CONST D3DXFROMWINECOLOR&) const;
    D3DXFROMWINECOLOR operator - (CONST D3DXFROMWINECOLOR&) const;
    D3DXFROMWINECOLOR operator * (FLOAT) const;
    D3DXFROMWINECOLOR operator / (FLOAT) const;
#if !defined(SWIG)

    friend D3DXFROMWINECOLOR operator * (FLOAT, CONST D3DXFROMWINECOLOR&);
#endif

    BOOL operator == (CONST D3DXFROMWINECOLOR&) const;
    BOOL operator != (CONST D3DXFROMWINECOLOR&) const;
#endif /* __cplusplus */
    FLOAT r, g, b, a;
} D3DXFROMWINECOLOR, *LPD3DXFROMWINECOLOR;

#ifdef __cplusplus
extern "C" {
#endif

 D3DXFROMWINECOLOR* WINAPI D3DXFROMWINEColorAdjustContrast(D3DXFROMWINECOLOR *pout, CONST D3DXFROMWINECOLOR *pc, FLOAT s);
 D3DXFROMWINECOLOR* WINAPI D3DXFROMWINEColorAdjustSaturation(D3DXFROMWINECOLOR *pout, CONST D3DXFROMWINECOLOR *pc, FLOAT s);

 FLOAT WINAPI D3DXFROMWINEFresnelTerm(FLOAT costheta, FLOAT refractionindex);

#ifndef SWIG
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixAffineTransformation(D3DXFROMWINEMATRIX *pout, FLOAT scaling, CONST D3DXFROMWINEVECTOR3 *rotationcenter, CONST D3DXFROMWINEQUATERNION *rotation, CONST D3DXFROMWINEVECTOR3 *translation);
#endif
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixAffineTransformation2D(D3DXFROMWINEMATRIX *pout, FLOAT scaling, CONST D3DXFROMWINEVECTOR2 *protationcenter, FLOAT rotation, CONST D3DXFROMWINEVECTOR2 *ptranslation);
#ifndef SWIG
 HRESULT WINAPI D3DXFROMWINEMatrixDecompose(D3DXFROMWINEVECTOR3 *poutscale, D3DXFROMWINEQUATERNION *poutrotation, D3DXFROMWINEVECTOR3 *pouttranslation, CONST D3DXFROMWINEMATRIX *pm);
#endif
 FLOAT WINAPI D3DXFROMWINEMatrixDeterminant(CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixInverse(D3DXFROMWINEMATRIX *pout, FLOAT *pdeterminant, CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixLookAtLH(D3DXFROMWINEMATRIX *pout, CONST D3DXFROMWINEVECTOR3 *peye, CONST D3DXFROMWINEVECTOR3 *pat, CONST D3DXFROMWINEVECTOR3 *pup);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixLookAtRH(D3DXFROMWINEMATRIX *pout, CONST D3DXFROMWINEVECTOR3 *peye, CONST D3DXFROMWINEVECTOR3 *pat, CONST D3DXFROMWINEVECTOR3 *pup);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixMultiply(D3DXFROMWINEMATRIX *pout, CONST D3DXFROMWINEMATRIX *pm1, CONST D3DXFROMWINEMATRIX *pm2);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixMultiplyTranspose(D3DXFROMWINEMATRIX *pout, CONST D3DXFROMWINEMATRIX *pm1, CONST D3DXFROMWINEMATRIX *pm2);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixOrthoLH(D3DXFROMWINEMATRIX *pout, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixOrthoOffCenterLH(D3DXFROMWINEMATRIX *pout, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn, FLOAT zf);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixOrthoOffCenterRH(D3DXFROMWINEMATRIX *pout, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn, FLOAT zf);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixOrthoLH(D3DXFROMWINEMATRIX *pout, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixOrthoRH(D3DXFROMWINEMATRIX *pout, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixPerspectiveFovLH(D3DXFROMWINEMATRIX *pout, FLOAT fovy, FLOAT aspect, FLOAT zn, FLOAT zf);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixPerspectiveFovRH(D3DXFROMWINEMATRIX *pout, FLOAT fovy, FLOAT aspect, FLOAT zn, FLOAT zf);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixPerspectiveLH(D3DXFROMWINEMATRIX *pout, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixPerspectiveOffCenterLH(D3DXFROMWINEMATRIX *pout, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn, FLOAT zf);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixPerspectiveOffCenterRH(D3DXFROMWINEMATRIX *pout, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn, FLOAT zf);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixPerspectiveRH(D3DXFROMWINEMATRIX *pout, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixReflect(D3DXFROMWINEMATRIX *pout, CONST D3DXFROMWINEPLANE *pplane);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixRotationAxis(D3DXFROMWINEMATRIX *pout, CONST D3DXFROMWINEVECTOR3 *pv, FLOAT angle);
#ifndef SWIG
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixRotationQuaternion(D3DXFROMWINEMATRIX *pout, CONST D3DXFROMWINEQUATERNION *pq);
#endif
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixRotationX(D3DXFROMWINEMATRIX *pout, FLOAT angle);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixRotationY(D3DXFROMWINEMATRIX *pout, FLOAT angle);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixRotationYawPitchRoll(D3DXFROMWINEMATRIX *pout, FLOAT yaw, FLOAT pitch, FLOAT roll);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixRotationZ(D3DXFROMWINEMATRIX *pout, FLOAT angle);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixScaling(D3DXFROMWINEMATRIX *pout, FLOAT sx, FLOAT sy, FLOAT sz);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixShadow(D3DXFROMWINEMATRIX *pout, CONST D3DXFROMWINEVECTOR4 *plight, CONST D3DXFROMWINEPLANE *pPlane);
#ifndef SWIG
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixTransformation(D3DXFROMWINEMATRIX *pout, CONST D3DXFROMWINEVECTOR3 *pscalingcenter, CONST D3DXFROMWINEQUATERNION *pscalingrotation, CONST D3DXFROMWINEVECTOR3 *pscaling, CONST D3DXFROMWINEVECTOR3 *protationcenter, CONST D3DXFROMWINEQUATERNION *protation, CONST D3DXFROMWINEVECTOR3 *ptranslation);
#endif
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixTransformation2D(D3DXFROMWINEMATRIX *pout, CONST D3DXFROMWINEVECTOR2 *pscalingcenter, FLOAT scalingrotation, CONST D3DXFROMWINEVECTOR2 *pscaling, CONST D3DXFROMWINEVECTOR2 *protationcenter, FLOAT rotation, CONST D3DXFROMWINEVECTOR2 *ptranslation);
 D3DXFROMWINEMATRIX* WINAPI D3DXFROMWINEMatrixTranspose(D3DXFROMWINEMATRIX *pout, CONST D3DXFROMWINEMATRIX *pm);

 D3DXFROMWINEPLANE* WINAPI D3DXFROMWINEPlaneFromPointNormal(D3DXFROMWINEPLANE *pout, CONST D3DXFROMWINEVECTOR3 *pvpoint, CONST D3DXFROMWINEVECTOR3 *pvnormal);
 D3DXFROMWINEPLANE* WINAPI D3DXFROMWINEPlaneFromPoints(D3DXFROMWINEPLANE *pout, CONST D3DXFROMWINEVECTOR3 *pv1, CONST D3DXFROMWINEVECTOR3 *pv2, CONST D3DXFROMWINEVECTOR3 *pv3);
 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEPlaneIntersectLine(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEPLANE *pp, CONST D3DXFROMWINEVECTOR3 *pv1, CONST D3DXFROMWINEVECTOR3 *pv2);
 D3DXFROMWINEPLANE* WINAPI D3DXFROMWINEPlaneNormalize(D3DXFROMWINEPLANE *pout, CONST D3DXFROMWINEPLANE *pp);
 D3DXFROMWINEPLANE* WINAPI D3DXFROMWINEPlaneTransform(D3DXFROMWINEPLANE *pout, CONST D3DXFROMWINEPLANE *pplane, CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEPLANE* WINAPI D3DXFROMWINEPlaneTransformArray(D3DXFROMWINEPLANE *pout, UINT outstride, CONST D3DXFROMWINEPLANE *pplane, UINT pstride, CONST D3DXFROMWINEMATRIX *pm, UINT n);
#ifndef SWIG

 D3DXFROMWINEQUATERNION* WINAPI D3DXFROMWINEQuaternionBaryCentric(D3DXFROMWINEQUATERNION *pout, CONST D3DXFROMWINEQUATERNION *pq1, CONST D3DXFROMWINEQUATERNION *pq2, CONST D3DXFROMWINEQUATERNION *pq3, FLOAT f, FLOAT g);
 D3DXFROMWINEQUATERNION* WINAPI D3DXFROMWINEQuaternionExp(D3DXFROMWINEQUATERNION *pout, CONST D3DXFROMWINEQUATERNION *pq);
 D3DXFROMWINEQUATERNION* WINAPI D3DXFROMWINEQuaternionInverse(D3DXFROMWINEQUATERNION *pout, CONST D3DXFROMWINEQUATERNION *pq);
 D3DXFROMWINEQUATERNION* WINAPI D3DXFROMWINEQuaternionLn(D3DXFROMWINEQUATERNION *pout, CONST D3DXFROMWINEQUATERNION *pq);
 D3DXFROMWINEQUATERNION* WINAPI D3DXFROMWINEQuaternionMultiply(D3DXFROMWINEQUATERNION *pout, CONST D3DXFROMWINEQUATERNION *pq1, CONST D3DXFROMWINEQUATERNION *pq2);
 D3DXFROMWINEQUATERNION* WINAPI D3DXFROMWINEQuaternionNormalize(D3DXFROMWINEQUATERNION *pout, CONST D3DXFROMWINEQUATERNION *pq);
 D3DXFROMWINEQUATERNION* WINAPI D3DXFROMWINEQuaternionRotationAxis(D3DXFROMWINEQUATERNION *pout, CONST D3DXFROMWINEVECTOR3 *pv, FLOAT angle);
 D3DXFROMWINEQUATERNION* WINAPI D3DXFROMWINEQuaternionRotationMatrix(D3DXFROMWINEQUATERNION *pout, CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEQUATERNION* WINAPI D3DXFROMWINEQuaternionRotationYawPitchRoll(D3DXFROMWINEQUATERNION *pout, FLOAT yaw, FLOAT pitch, FLOAT roll);
 D3DXFROMWINEQUATERNION* WINAPI D3DXFROMWINEQuaternionSlerp(D3DXFROMWINEQUATERNION *pout, CONST D3DXFROMWINEQUATERNION *pq1, CONST D3DXFROMWINEQUATERNION *pq2, FLOAT t);
 D3DXFROMWINEQUATERNION* WINAPI D3DXFROMWINEQuaternionSquad(D3DXFROMWINEQUATERNION *pout, CONST D3DXFROMWINEQUATERNION *pq1, CONST D3DXFROMWINEQUATERNION *pq2, CONST D3DXFROMWINEQUATERNION *pq3, CONST D3DXFROMWINEQUATERNION *pq4, FLOAT t);
void WINAPI D3DXFROMWINEQuaternionToAxisAngle(CONST D3DXFROMWINEQUATERNION *pq, D3DXFROMWINEVECTOR3 *paxis, FLOAT *pangle);
#endif
 D3DXFROMWINEVECTOR2* WINAPI D3DXFROMWINEVec2BaryCentric(D3DXFROMWINEVECTOR2 *pout, CONST D3DXFROMWINEVECTOR2 *pv1, CONST D3DXFROMWINEVECTOR2 *pv2, CONST D3DXFROMWINEVECTOR2 *pv3, FLOAT f, FLOAT g);
 D3DXFROMWINEVECTOR2* WINAPI D3DXFROMWINEVec2CatmullRom(D3DXFROMWINEVECTOR2 *pout, CONST D3DXFROMWINEVECTOR2 *pv0, CONST D3DXFROMWINEVECTOR2 *pv1, CONST D3DXFROMWINEVECTOR2 *pv2, CONST D3DXFROMWINEVECTOR2 *pv3, FLOAT s);
 D3DXFROMWINEVECTOR2* WINAPI D3DXFROMWINEVec2Hermite(D3DXFROMWINEVECTOR2 *pout, CONST D3DXFROMWINEVECTOR2 *pv1, CONST D3DXFROMWINEVECTOR2 *pt1, CONST D3DXFROMWINEVECTOR2 *pv2, CONST D3DXFROMWINEVECTOR2 *pt2, FLOAT s);
 D3DXFROMWINEVECTOR2* WINAPI D3DXFROMWINEVec2Normalize(D3DXFROMWINEVECTOR2 *pout, CONST D3DXFROMWINEVECTOR2 *pv);
 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec2Transform(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR2 *pv, CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec2TransformArray(D3DXFROMWINEVECTOR4 *pout, UINT outstride, CONST D3DXFROMWINEVECTOR2 *pv, UINT vstride, CONST D3DXFROMWINEMATRIX *pm, UINT n);
 D3DXFROMWINEVECTOR2* WINAPI D3DXFROMWINEVec2TransformCoord(D3DXFROMWINEVECTOR2 *pout, CONST D3DXFROMWINEVECTOR2 *pv, CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEVECTOR2* WINAPI D3DXFROMWINEVec2TransformCoordArray(D3DXFROMWINEVECTOR2 *pout, UINT outstride, CONST D3DXFROMWINEVECTOR2 *pv, UINT vstride, CONST D3DXFROMWINEMATRIX *pm, UINT n);
 D3DXFROMWINEVECTOR2* WINAPI D3DXFROMWINEVec2TransformNormal(D3DXFROMWINEVECTOR2 *pout, CONST D3DXFROMWINEVECTOR2 *pv, CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEVECTOR2* WINAPI D3DXFROMWINEVec2TransformNormalArray(D3DXFROMWINEVECTOR2 *pout, UINT outstride, CONST D3DXFROMWINEVECTOR2 *pv, UINT vstride, CONST D3DXFROMWINEMATRIX *pm, UINT n);



 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEVec3BaryCentric(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv1, CONST D3DXFROMWINEVECTOR3 *pv2, CONST D3DXFROMWINEVECTOR3 *pv3, FLOAT f, FLOAT g);
 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEVec3CatmullRom( D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv0, CONST D3DXFROMWINEVECTOR3 *pv1, CONST D3DXFROMWINEVECTOR3 *pv2, CONST D3DXFROMWINEVECTOR3 *pv3, FLOAT s);
 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEVec3Hermite(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv1, CONST D3DXFROMWINEVECTOR3 *pt1, CONST D3DXFROMWINEVECTOR3 *pv2, CONST D3DXFROMWINEVECTOR3 *pt2, FLOAT s);
 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEVec3Normalize(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv);
 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEVec3Project(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv, CONST D3DFROMWINEVIEWPORT9 *pviewport, CONST D3DXFROMWINEMATRIX *pprojection, CONST D3DXFROMWINEMATRIX *pview, CONST D3DXFROMWINEMATRIX *pworld);
 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEVec3ProjectArray(D3DXFROMWINEVECTOR3 *pout, UINT outstride, CONST D3DXFROMWINEVECTOR3 *pv, UINT vstride, CONST D3DFROMWINEVIEWPORT9 *pviewport, CONST D3DXFROMWINEMATRIX *pprojection, CONST D3DXFROMWINEMATRIX *pview, CONST D3DXFROMWINEMATRIX *pworld, UINT n);
 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec3Transform(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR3 *pv, CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec3TransformArray(D3DXFROMWINEVECTOR4 *pout, UINT outstride, CONST D3DXFROMWINEVECTOR3 *pv, UINT vstride, CONST D3DXFROMWINEMATRIX *pm, UINT n);
 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEVec3TransformCoord(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv, CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEVec3TransformCoordArray(D3DXFROMWINEVECTOR3 *pout, UINT outstride, CONST D3DXFROMWINEVECTOR3 *pv, UINT vstride, CONST D3DXFROMWINEMATRIX *pm, UINT n);

 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec4TransformCoord(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv, CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec4TransformCoordArray(D3DXFROMWINEVECTOR4 *pout, UINT outstride, CONST D3DXFROMWINEVECTOR4 *pv, UINT vstride, CONST D3DXFROMWINEMATRIX *pm, UINT n);

 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEVec3TransformNormal(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv, CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEVec3TransformNormalArray(D3DXFROMWINEVECTOR3 *pout, UINT outstride, CONST D3DXFROMWINEVECTOR3 *pv, UINT vstride, CONST D3DXFROMWINEMATRIX *pm, UINT n);
 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEVec3Unproject(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv, CONST D3DFROMWINEVIEWPORT9 *pviewport, CONST D3DXFROMWINEMATRIX *pprojection, CONST D3DXFROMWINEMATRIX *pview, CONST D3DXFROMWINEMATRIX *pworld);
 D3DXFROMWINEVECTOR3* WINAPI D3DXFROMWINEVec3UnprojectArray(D3DXFROMWINEVECTOR3 *pout, UINT outstride, CONST D3DXFROMWINEVECTOR3 *pv, UINT vstride, CONST D3DFROMWINEVIEWPORT9 *pviewport, CONST D3DXFROMWINEMATRIX *pprojection, CONST D3DXFROMWINEMATRIX *pview, CONST D3DXFROMWINEMATRIX *pworld, UINT n);

 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec4BaryCentric(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv1, CONST D3DXFROMWINEVECTOR4 *pv2, CONST D3DXFROMWINEVECTOR4 *pv3, FLOAT f, FLOAT g);
 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec4CatmullRom(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv0, CONST D3DXFROMWINEVECTOR4 *pv1, CONST D3DXFROMWINEVECTOR4 *pv2, CONST D3DXFROMWINEVECTOR4 *pv3, FLOAT s);
 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec4Cross(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv1, CONST D3DXFROMWINEVECTOR4 *pv2, CONST D3DXFROMWINEVECTOR4 *pv3);
 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec4Hermite(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv1, CONST D3DXFROMWINEVECTOR4 *pt1, CONST D3DXFROMWINEVECTOR4 *pv2, CONST D3DXFROMWINEVECTOR4 *pt2, FLOAT s);
 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec4Normalize(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv);
 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec4Transform(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv, CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEVECTOR4* WINAPI D3DXFROMWINEVec4TransformArray(D3DXFROMWINEVECTOR4 *pout, UINT outstride, CONST D3DXFROMWINEVECTOR4 *pv, UINT vstride, CONST D3DXFROMWINEMATRIX *pm, UINT n);

#ifdef __cplusplus
}
#endif

#if 0
typedef interface ID3DXFROMWINEMatrixStack *LPD3DXFROMWINEMATRIXSTACK;

DEFINE_GUID(IID_ID3DXFROMWINEMatrixStack,
0xc7885ba7, 0xf990, 0x4fe7, 0x92, 0x2d, 0x85, 0x15, 0xe4, 0x77, 0xdd, 0x85);

#undef INTERFACE
#define INTERFACE ID3DXFROMWINEMatrixStack

DECLARE_INTERFACE_(ID3DXFROMWINEMatrixStack, IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(Pop)(THIS) PURE;
    STDMETHOD(Push)(THIS) PURE;
    STDMETHOD(LoadIdentity)(THIS) PURE;
    STDMETHOD(LoadMatrix)(THIS_ CONST D3DXFROMWINEMATRIX* pM ) PURE;
    STDMETHOD(MultMatrix)(THIS_ CONST D3DXFROMWINEMATRIX* pM ) PURE;
    STDMETHOD(MultMatrixLocal)(THIS_ CONST D3DXFROMWINEMATRIX* pM ) PURE;
    STDMETHOD(RotateAxis)(THIS_ CONST D3DXFROMWINEVECTOR3* pV, FLOAT Angle) PURE;
    STDMETHOD(RotateAxisLocal)(THIS_ CONST D3DXFROMWINEVECTOR3* pV, FLOAT Angle) PURE;
    STDMETHOD(RotateYawPitchRoll)(THIS_ FLOAT Yaw, FLOAT Pitch, FLOAT Roll) PURE;
    STDMETHOD(RotateYawPitchRollLocal)(THIS_ FLOAT Yaw, FLOAT Pitch, FLOAT Roll) PURE;
    STDMETHOD(Scale)(THIS_ FLOAT x, FLOAT y, FLOAT z) PURE;
    STDMETHOD(ScaleLocal)(THIS_ FLOAT x, FLOAT y, FLOAT z) PURE;
    STDMETHOD(Translate)(THIS_ FLOAT x, FLOAT y, FLOAT z ) PURE;
    STDMETHOD(TranslateLocal)(THIS_ FLOAT x, FLOAT y, FLOAT z) PURE;
    STDMETHOD_(D3DXFROMWINEMATRIX*, GetTop)(THIS) PURE;
};

#undef INTERFACE

#if !defined(__cplusplus) || defined(CINTERFACE)

#define ID3DXFROMWINEMatrixStack_QueryInterface(p,a,b)            (p)->lpVtbl->QueryInterface(p,a,b)
#define ID3DXFROMWINEMatrixStack_AddRef(p)                        (p)->lpVtbl->AddRef(p)
#define ID3DXFROMWINEMatrixStack_Release(p)                       (p)->lpVtbl->Release(p)
#define ID3DXFROMWINEMatrixStack_Pop(p)                           (p)->lpVtbl->Pop(p)
#define ID3DXFROMWINEMatrixStack_Push(p)                          (p)->lpVtbl->Push(p)
#define ID3DXFROMWINEMatrixStack_LoadIdentity(p)                  (p)->lpVtbl->LoadIdentity(p)
#define ID3DXFROMWINEMatrixStack_LoadMatrix(p,a)                  (p)->lpVtbl->LoadMatrix(p,a)
#define ID3DXFROMWINEMatrixStack_MultMatrix(p,a)                  (p)->lpVtbl->MultMatrix(p,a)
#define ID3DXFROMWINEMatrixStack_MultMatrixLocal(p,a)             (p)->lpVtbl->MultMatrixLocal(p,a)
#define ID3DXFROMWINEMatrixStack_RotateAxis(p,a,b)                (p)->lpVtbl->RotateAxis(p,a,b)
#define ID3DXFROMWINEMatrixStack_RotateAxisLocal(p,a,b)           (p)->lpVtbl->RotateAxisLocal(p,a,b)
#define ID3DXFROMWINEMatrixStack_RotateYawPitchRoll(p,a,b,c)      (p)->lpVtbl->RotateYawPitchRoll(p,a,b,c)
#define ID3DXFROMWINEMatrixStack_RotateYawPitchRollLocal(p,a,b,c) (p)->lpVtbl->RotateYawPitchRollLocal(p,a,b,c)
#define ID3DXFROMWINEMatrixStack_Scale(p,a,b,c)                   (p)->lpVtbl->Scale(p,a,b,c)
#define ID3DXFROMWINEMatrixStack_ScaleLocal(p,a,b,c)              (p)->lpVtbl->ScaleLocal(p,a,b,c)
#define ID3DXFROMWINEMatrixStack_Translate(p,a,b,c)               (p)->lpVtbl->Translate(p,a,b,c)
#define ID3DXFROMWINEMatrixStack_TranslateLocal(p,a,b,c)          (p)->lpVtbl->TranslateLocal(p,a,b,c)
#define ID3DXFROMWINEMatrixStack_GetTop(p)                        (p)->lpVtbl->GetTop(p)

#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif

//HRESULT WINAPI D3DXFROMWINECreateMatrixStack(__DWORD__ flags, LPD3DXFROMWINEMATRIXSTACK* ppstack);

#ifdef __cplusplus
}
#endif
#ifndef D3DFROMWINECOLORVALUE_DEFINED
typedef struct _D3DFROMWINECOLORVALUE {
    float r;
    float g;
    float b;
    float a;
} D3DFROMWINECOLORVALUE;
#define D3DFROMWINECOLORVALUE_DEFINED
#endif

typedef struct _D3DFROMWINEMATERIAL9 {
    D3DFROMWINECOLORVALUE   Diffuse;        /* Diffuse color RGBA */
    D3DFROMWINECOLORVALUE   Ambient;        /* Ambient color RGB */
    D3DFROMWINECOLORVALUE   Specular;       /* Specular 'shininess' */
    D3DFROMWINECOLORVALUE   Emissive;       /* Emissive color RGB */
    float           Power;          /* Sharpness if specular highlight */
} D3DFROMWINEMATERIAL9;

typedef enum _D3DFROMWINELIGHTTYPE {
    D3DFROMWINELIGHT_POINT          = 1,
    D3DFROMWINELIGHT_SPOT           = 2,
    D3DFROMWINELIGHT_DIRECTIONAL    = 3,
    D3DFROMWINELIGHT_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
} D3DFROMWINELIGHTTYPE;/*
typedef struct _D3DFROMWINEVECTOR {
    float x;
    float y;
    float z;
} D3DFROMWINEVECTOR;*/
typedef struct _D3DFROMWINELIGHT9 {
    D3DFROMWINELIGHTTYPE    Type;            /* Type of light source */
    D3DFROMWINECOLORVALUE   Diffuse;         /* Diffuse color of light */
    D3DFROMWINECOLORVALUE   Specular;        /* Specular color of light */
    D3DFROMWINECOLORVALUE   Ambient;         /* Ambient color of light */
    D3DFROMWINEVECTOR       Position;         /* Position in world space */
    D3DFROMWINEVECTOR       Direction;        /* Direction in world space */
    float           Range;            /* Cutoff range */
    float           Falloff;          /* Falloff */
    float           Attenuation0;     /* Constant attenuation */
    float           Attenuation1;     /* Linear attenuation */
    float           Attenuation2;     /* Quadratic attenuation */
    float           Theta;            /* Inner angle of spotlight cone */
    float           Phi;              /* Outer angle of spotlight cone */
} D3DFROMWINELIGHT9;
#ifndef SWIG
#include "d3dx9mathFromWine.inl"
#endif

typedef D3DXFROMWINEVECTOR2 vec2;
typedef D3DXFROMWINEVECTOR3 vec3;
typedef D3DXFROMWINEVECTOR4 vec4;
typedef D3DXFROMWINEVECTOR2 float2;
typedef D3DXFROMWINEVECTOR3 float3;
typedef D3DXFROMWINEVECTOR4 float4;
typedef D3DXFROMWINEMATRIX  mat4;

inline void D3DXFROMWINEGetScaling(const D3DXFROMWINEMATRIX& w, D3DXFROMWINEVECTOR3& out)
{ 
	D3DXFROMWINEVECTOR3 v1(w._11, w._12, w._13);
	D3DXFROMWINEVECTOR3 v2(w._21, w._22, w._23);
	D3DXFROMWINEVECTOR3 v3(w._31, w._32, w._33);
	out.x=D3DXFROMWINEVec3Length(&v1);
	out.y=D3DXFROMWINEVec3Length(&v2);
	out.z=D3DXFROMWINEVec3Length(&v3);
}
inline void D3DXFROMWINEGetTranslation(const D3DXFROMWINEMATRIX& w, D3DXFROMWINEVECTOR3& out)
    { 
	out.x=w._41;
	out.y=w._42;
	out.z=w._43;
}
inline void D3DXFROMWINEGetRotation(const D3DXFROMWINEMATRIX& w, D3DXFROMWINEVECTOR3& out)
    {
        if (w._11 == 1.0f)
        {
            out.x = atan2f(w._13, w._34);
            out.y = 0;
            out.z = 0;

        }else if (w._11 == -1.0f)
        {
            out.x = atan2f(w._13, w._34);
            out.y = 0;
            out.z = 0;
        }else 
        {

            out.x = atan2(-w._31,w._11);
            out.y = asin(w._21);
            out.z = atan2(-w._23,w._22);
        }
	}

inline D3DXFROMWINEVECTOR3 getRightVec(const D3DXFROMWINEMATRIX& elem)   { return D3DXFROMWINEVECTOR3(elem._11, elem._12, elem._13); }
inline D3DXFROMWINEVECTOR3 getUpVec(const D3DXFROMWINEMATRIX& elem)      { return D3DXFROMWINEVECTOR3(elem._21, elem._22, elem._23); }
inline D3DXFROMWINEVECTOR3 getForwardVec(const D3DXFROMWINEMATRIX& elem) { return D3DXFROMWINEVECTOR3(elem._31, elem._32, elem._33); }

struct Vector2i
{
	int x;
	int y;
	Vector2i(int ax=0, int ay=0)
	{
		x=ax;
		y=ay;
	}
	Vector2i(const Vector2i& rhs)
	{
		x=rhs.x;
		y=rhs.y;
	}
};
inline float deltaDistanceVector2i(const Vector2i& start, const Vector2i& end)
	{
    		D3DXFROMWINEVECTOR2 v(end.x-start.x, end.y-start.y);
    		FLOAT norm = D3DXFROMWINEVec2Length(&v);
		return norm;
	}

typedef struct D3DFROMWINE_BOX {
  UINT left;
  UINT top;
  UINT front;
  UINT right;
  UINT bottom;
  UINT back;
} D3DFROMWINE_BOX;
/*
inline BOOL WriteFile(
  HANDLE       hFile,
  const void*      lpBuffer,
  __DWORD__        nNumberOfBytesToWrite,
  LPDWORD      lpNumberOfBytesWritten,
  LPOVERLAPPED lpOverlapped
);
*/
typedef D3DXFROMWINEMATRIX D3DXFROMWINEMATRIXA16;
#endif /* __D3DXFROMWINE9MATH_H__ */

