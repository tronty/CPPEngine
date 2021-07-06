/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __MATRIX_H__
#define __MATRIX_H__
#include <assert.h>
#include <math.h>
#include "d3dx9mathFromWine.h"
//include <Framework3/IRenderer.h>
#if !defined(_MSC_VER) && !defined(__MINGW32__) && !defined(__MINGW64__)
inline void ZeroMemory(void* p, const int s)
{
	stx_memset(p,s,0);
}
#endif
inline D3DXFROMWINEMATRIX identity4()
{
    D3DXFROMWINEMATRIX pout;
	D3DXFROMWINEMatrixIdentity(&pout);
    return pout;
}
inline D3DXFROMWINEVECTOR3 cross(CONST D3DXFROMWINEVECTOR3 &pv1, CONST D3DXFROMWINEVECTOR3 &pv2)
{
    D3DXFROMWINEVECTOR3 pout=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
    pout.x = (pv1.y) * (pv2.z) - (pv1.z) * (pv2.y);
    pout.y = (pv1.z) * (pv2.x) - (pv1.x) * (pv2.z);
    pout.z = (pv1.x) * (pv2.y) - (pv1.y) * (pv2.x);
    return pout;
}
inline D3DXFROMWINEVECTOR4 cross(CONST D3DXFROMWINEVECTOR4 &pv1, CONST D3DXFROMWINEVECTOR4 &pv2)
{
    D3DXFROMWINEVECTOR4 pout=D3DXFROMWINEVECTOR4(0.0f,0.0f,0.0f,0.0f);
    pout.x = (pv1.y) * (pv2.z) - (pv1.z) * (pv2.y);
    pout.y = (pv1.z) * (pv2.x) - (pv1.x) * (pv2.z);
    pout.z = (pv1.x) * (pv2.y) - (pv1.y) * (pv2.x);
    return pout;
}
inline D3DXFROMWINEVECTOR2 normalize(const D3DXFROMWINEVECTOR2 &v){
	float invLen = 1.0f / sqrtf(v.x * v.x + v.y * v.y);
	return v * invLen;
}

inline D3DXFROMWINEVECTOR3 normalize(const D3DXFROMWINEVECTOR3 &v){
	float invLen = 1.0f / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return v * invLen;
}

inline D3DXFROMWINEVECTOR4 normalize(const D3DXFROMWINEVECTOR4 &v){
	float invLen = 1.0f / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	return v * invLen;
}
inline FLOAT dot(CONST D3DXFROMWINEVECTOR3 &pv1, CONST D3DXFROMWINEVECTOR3 &pv2)
{
    return (pv1.x) * (pv2.x) + (pv1.y) * (pv2.y) + (pv1.z) * (pv2.z);
}
inline FLOAT dot(CONST D3DXFROMWINEVECTOR4 &pv1, CONST D3DXFROMWINEVECTOR4 &pv2)
{
    return (pv1.x) * (pv2.x) + (pv1.y) * (pv2.y) + (pv1.z) * (pv2.z) + (pv1.w) * (pv2.w);
}

inline D3DXFROMWINEVECTOR3 crossProduct(CONST D3DXFROMWINEVECTOR3& pv1, CONST D3DXFROMWINEVECTOR3& pv2)
{
	D3DXFROMWINEVECTOR3 pout;
    pout.x = (pv1.y) * (pv2.z) - (pv1.z) * (pv2.y);
    pout.y = (pv1.z) * (pv2.x) - (pv1.x) * (pv2.z);
    pout.z = (pv1.x) * (pv2.y) - (pv1.y) * (pv2.x);
    return pout;
}
/*
inline D3DXFROMWINEVECTOR3& cross(CONST D3DXFROMWINEVECTOR3& pv1, CONST D3DXFROMWINEVECTOR3& pv2)
{
	return crossProduct(pv1, pv2);
}

inline D3DXFROMWINEVECTOR3& normalize(D3DXFROMWINEVECTOR3& a)
{
	CONST D3DXFROMWINEVECTOR3 pv1=a;
	CONST D3DXFROMWINEVECTOR3 pv2=a;
   	FLOAT n=sqrtf((pv1.x) * (pv2.x) + (pv1.y) * (pv2.y) + (pv1.z) * (pv2.z));
    		a.x /= n;
    		a.y /= n;
    		a.z /= n;
	return a;
}

inline FLOAT& dot(CONST D3DXFROMWINEVECTOR3& a,CONST D3DXFROMWINEVECTOR3& b)
{
	CONST D3DXFROMWINEVECTOR3 pv1=a;
	CONST D3DXFROMWINEVECTOR3 pv2=b;
   	FLOAT n=sqrtf((pv1.x) * (pv2.x) + (pv1.y) * (pv2.y) + (pv1.z) * (pv2.z));
	return n;
}
*/
void buildPerspectiveMatrixGL(float fieldOfView,
			   float aspectRatio,
			   float zNear, float zFar,
			   D3DXFROMWINEMATRIX& m);

void buildLookAtMatrixGL(float eyex, float eyey, float eyez,
		      float centerx, float centery, float centerz,
		      float upx, float upy, float upz,
		      D3DXFROMWINEMATRIX& m);
void buildPerspectiveMatrixD3D(float fieldOfView,
			   float aspectRatio,
			   float zNear, float zFar,
			   D3DXFROMWINEMATRIX& m);

void buildLookAtMatrixD3D(float eyex, float eyey, float eyez,
		      float centerx, float centery, float centerz,
		      float upx, float upy, float upz,
		      D3DXFROMWINEMATRIX& m);

unsigned int toRGBA(const D3DXFROMWINEVECTOR4 &u);
unsigned int toBGRA(const D3DXFROMWINEVECTOR4 &u);
D3DXFROMWINEVECTOR3 rgbeToRGB(unsigned char *rgbe);
unsigned int rgbToRGBE8(const D3DXFROMWINEVECTOR3 &rgb);
unsigned int rgbToRGB9E5(const D3DXFROMWINEVECTOR3 &rgb);
// Note: returns true for 0
inline bool isPowerOf2(const int x){
	return (x & (x - 1)) == 0;
}
struct  half {
	unsigned short sh;
	half(){};
    half(const float x);
	operator float () const;
};
struct  half4 {
	half x,y,z,w;
	half4()
	{
		x=0.0f;
		y=0.0f;
		z=0.0f;
		w=0.0f;
	}
	half4(half ax,half ay,half az,half aw)
	{
		x=ax;
		y=ay;
		z=az;
		w=aw;
	}
};
#if 0
float clamp(const float v, const float c0, const float c1);
D3DXFROMWINEVECTOR2  clamp(const D3DXFROMWINEVECTOR2 &v, const float c0, const float c1);
D3DXFROMWINEVECTOR2  clamp(const D3DXFROMWINEVECTOR2 &v, const D3DXFROMWINEVECTOR2 &c0, const D3DXFROMWINEVECTOR2 &c1);
D3DXFROMWINEVECTOR3  clamp(const D3DXFROMWINEVECTOR3 &v, const float c0, const float c1);
D3DXFROMWINEVECTOR3  clamp(const D3DXFROMWINEVECTOR3 &v, const D3DXFROMWINEVECTOR3 &c0, const D3DXFROMWINEVECTOR3 &c1);
D3DXFROMWINEVECTOR4  clamp(const D3DXFROMWINEVECTOR4 &v, const float c0, const float c1);
D3DXFROMWINEVECTOR4  clamp(const D3DXFROMWINEVECTOR4 &v, const D3DXFROMWINEVECTOR4 &c0, const D3DXFROMWINEVECTOR4 &c1);
//#define clamp(x, lo, hi) max(min(x, hi), lo)
#define saturate(x) clamp(x, 0, 1)
#define lerp_(t, a, b) (a + t * (b - a))
#endif
    inline D3DXFROMWINEVECTOR3 operator ^(const D3DXFROMWINEVECTOR3 &f, const D3DXFROMWINEVECTOR3 &t)
    {
      return D3DXFROMWINEVECTOR3(f.y   * t.z - f.z   * t.y,
                       t.x * f.z   - t.z * f.x,
                       f.x   * t.y - f.y   * t.x);
    }

    inline D3DXFROMWINEVECTOR3 /*&*/operator ^=(const D3DXFROMWINEVECTOR3 &f, const D3DXFROMWINEVECTOR3 &t)
    {
       return D3DXFROMWINEVECTOR3(f.y   * t.z - f.z   * t.y,
           t.x * f.z   - t.z * f.x,
           f.x   * t.y - f.y   * t.x);

    }
#endif

