
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include "../Platform.h"
#include "../SIMD.h"
#include "Half.h"

#include <math.h>

#define PI 3.1415926535f

struct bool2
{
	union
	{
		ubyte vec;
		struct
		{
			bool x : 1;
			bool y : 1;
		};
	};

	// Constructors
	bool2(){}
	bool2(const bool _x, const bool _y);
};

bool any(const bool2 b);
bool all(const bool2 b);


struct bool3
{
	union
	{
		ubyte vec;
		struct
		{
			bool x : 1;
			bool y : 1;
			bool z : 1;
		};
	};

	// Constructors
	bool3(){}
	bool3(const bool _x, const bool _y, const bool _z);
};

bool any(const bool3 b);
bool all(const bool3 b);


struct bool4
{
	union
	{
		ubyte vec;
		struct
		{
			bool x : 1;
			bool y : 1;
			bool z : 1;
			bool w : 1;
		};
	};

	// Constructors
	bool4(){}
	bool4(const bool _x, const bool _y, const bool _z, const bool _w);
};

bool any(const bool4 b);
bool all(const bool4 b);



/************************************************************************/


struct int2
{
	int32 x, y;

	// Constructors
	int2(){}
	int2(const int32 _x, const int32 _y);
};


/************************************************************************/


struct int3
{
	int32 x, y, z;

	// Constructors
	int3(){}
	int3(const int32 _x, const int32 _y, const int32 _z);
};


/************************************************************************/




struct ALIGN(16) int4
{
	union
	{
		__m128i vec;
		struct
		{
			int32 x, y, z, w;
		};
	};

	// Constructors
	int4(){}
	int4(const int32 _x, const int32 _y, const int32 _z, const int32 _w);

	// Swizzles
	int4 xxxx() const;
	int4 yyyy() const;
	int4 zzzz() const;
	int4 wwww() const;

	int4 xyzw() const;
	int4 wzyx() const;
	int4 xyxy() const;
	int4 zwzw() const;

	void operator += (const int4 &v);
	void operator += (const int s);
	void operator -= (const int4 &v);
	void operator -= (const int s);
// 	void operator *= (const int4 &v);
// 	void operator *= (const int s);
// 	void operator /= (const int4 &v);
// 	void operator /= (const int s);
};

int4 operator + (const int4 &v0, const int4 &v1);
int4 operator + (const int4 &v, const int s);
int4 operator + (const int s, const int4 &v);

int4 operator - (const int4 &v0, const int4 &v1);
int4 operator - (const int4 &v, const int s);
int4 operator - (const int s, const int4 &v);
int4 operator - (const int4 &v);

int4 operator * (const int4 &v0, const int4 &v1);
// int4 operator * (const int4 &v, const int s);
// int4 operator * (const int s, const int4 &v);
// 
// int4 operator / (const int4 &v0, const int4 &v1);
// int4 operator / (const int4 &v, const int s);
// int4 operator / (const int s, const int4 &v);



/************************************************************************/

struct float2
{
	float x, y;

	operator float *() const { return (float *) &x; }

	// Constructors
	float2() = default;
	force_inline float2(const float _x, const float _y)
	{
		x = _x;
		y = _y;
	}

	// Swizzles
	float2 xx() const;
	float2 yy() const;

	float2 xy() const;
	float2 yx() const;

	void operator += (const float2 &v);
	void operator += (const float s);
	void operator -= (const float2 &v);
	void operator -= (const float s);
	void operator *= (const float2 &v);
	void operator *= (const float s);
	void operator /= (const float2 &v);
	void operator /= (const float s);
};

float2 operator + (const float2 &v0, const float2 &v1);
float2 operator + (const float2 &v, const float s);
float2 operator + (const float s, const float2 &v);

float2 operator - (const float2 &v0, const float2 &v1);
float2 operator - (const float2 &v, const float s);
float2 operator - (const float s, const float2 &v);
float2 operator - (const float2 &v);

float2 operator * (const float2 &v0, const float2 &v1);
float2 operator * (const float2 &v, const float s);
float2 operator * (const float s, const float2 &v);

float2 operator / (const float2 &v0, const float2 &v1);
float2 operator / (const float2 &v, const float s);
float2 operator / (const float s, const float2 &v);

bool2 operator > (const float2 &v0, const float2 &v1);
bool2 operator < (const float2 &v0, const float2 &v1);
bool2 operator >= (const float2 &v0, const float2 &v1);
bool2 operator <= (const float2 &v0, const float2 &v1);
bool2 operator == (const float2 &v0, const float2 &v1);
bool2 operator != (const float2 &v0, const float2 &v1);

float dot(const float2 &v0, const float2 &v1);

float length(const float2 &v);
float2 normalize(const float2 &v);


/************************************************************************/

struct float3
{
	float x, y, z;

	operator float *() const { return (float *) &x; }

	// Constructors
	float3() = default;
	force_inline float3(const float _x, const float _y, const float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	force_inline float3(const float2 &xy, const float _z)
	{
		x = xy.x;
		y = xy.y;
		z = _z;
	}

	force_inline float3(const float _x, const float2 &yz)
	{
		x = _x;
		y = yz.x;
		z = yz.y;
	}

	// Swizzles
	float3 xxx() const;
	float3 yyy() const;
	float3 zzz() const;

	float3 xyz() const;
	float3 zyx() const;

	void operator += (const float3 &v);
	void operator += (const float s);
	void operator -= (const float3 &v);
	void operator -= (const float s);
	void operator *= (const float3 &v);
	void operator *= (const float s);
	void operator /= (const float3 &v);
	void operator /= (const float s);
};

force_inline float3 operator + (const float3 &v0, const float3 &v1) { return float3(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z); }
force_inline float3 operator + (const float3 &v,  const float   s)  { return float3(v.x + s, v.y + s, v.z + s); }
force_inline float3 operator + (const float   s,  const float3 &v)  { return float3(s + v.x, s + v.y, s + v.z); }

force_inline float3 operator - (const float3 &v0, const float3 &v1) { return float3(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z); }
force_inline float3 operator - (const float3 &v,  const float   s)  { return float3(v.x - s, v.y - s, v.z - s); }
force_inline float3 operator - (const float   s,  const float3 &v)  { return float3(s - v.x, s - v.y, s - v.z); }

force_inline float3 operator - (const float3 &v) { return float3(-v.x, -v.y, -v.z); }

force_inline float3 operator * (const float3 &v0, const float3 &v1) { return float3(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z); }
force_inline float3 operator * (const float3 &v,  const float s)    { return float3(v.x * s, v.y * s, v.z * s); }
force_inline float3 operator * (const float s,    const float3 &v)  { return float3(s * v.x, s * v.y, s * v.z); }

force_inline float3 operator / (const float3 &v0, const float3 &v1) { return float3(v0.x / v1.x, v0.y / v1.y, v0.z / v1.z); }
force_inline float3 operator / (const float3 &v,  const float s)    { return float3(v.x / s, v.y / s, v.z / s); }
force_inline float3 operator / (const float s,    const float3 &v)  { return float3(s / v.x, s / v.y, s / v.z); }

bool3 operator > (const float3 &v0, const float3 &v1);
bool3 operator < (const float3 &v0, const float3 &v1);
bool3 operator >= (const float3 &v0, const float3 &v1);
bool3 operator <= (const float3 &v0, const float3 &v1);
bool3 operator == (const float3 &v0, const float3 &v1);
bool3 operator != (const float3 &v0, const float3 &v1);

float dot(const float3 &v0, const float3 &v1);
float dotXY(const float3 &v0, const float3 &v1);
float dotYZ(const float3 &v0, const float3 &v1);
float dotXZ(const float3 &v0, const float3 &v1);

float length(const float3 &v);
float3 normalize(const float3 &v);
float3 cross(const float3 &v0, const float3 &v1);



/************************************************************************/


struct ALIGN(16) float4
{
	union
	{
		__m128 vec;
		struct
		{
			float x, y, z, w;
		};
	};

	operator float *() const { return (float *) &x; }

	// Constructors
	float4() = default;
	force_inline float4(const float _x, const float _y, const float _z, const float _w)
	{
		vec = _mm_setr_ps(_x, _y, _z, _w);
	}

	force_inline float4(const float2 &xy, const float2 &zw)
	{
		vec = _mm_setr_ps(xy.x, xy.y, zw.x, zw.y);
	}

	force_inline float4(const float3 &xyz, const float _w)
	{
		vec = _mm_setr_ps(xyz.x, xyz.y, xyz.z, _w);
	}

	// Swizzles
	float4 xxxx() const;
	float4 yyyy() const;
	float4 zzzz() const;
	float4 wwww() const;

	float4 xyzw() const;
	float4 wzyx() const;
	float4 xyxy() const;
	float4 zwzw() const;

	void operator += (const float4 &v);
	void operator += (const float s);
	void operator -= (const float4 &v);
	void operator -= (const float s);
	void operator *= (const float4 &v);
	void operator *= (const float s);
	void operator /= (const float4 &v);
	void operator /= (const float s);
};

float4 operator + (const float4 &v0, const float4 &v1);
float4 operator + (const float4 &v, const float s);
float4 operator + (const float s, const float4 &v);

float4 operator - (const float4 &v0, const float4 &v1);
float4 operator - (const float4 &v, const float s);
float4 operator - (const float s, const float4 &v);
float4 operator - (const float4 &v);

float4 operator * (const float4 &v0, const float4 &v1);
float4 operator * (const float4 &v, const float s);
float4 operator * (const float s, const float4 &v);

float4 operator / (const float4 &v0, const float4 &v1);
float4 operator / (const float4 &v, const float s);
float4 operator / (const float s, const float4 &v);

bool4 operator > (const float4 &v0, const float4 &v1);
bool4 operator < (const float4 &v0, const float4 &v1);
bool4 operator >= (const float4 &v0, const float4 &v1);
bool4 operator <= (const float4 &v0, const float4 &v1);
bool4 operator == (const float4 &v0, const float4 &v1);
bool4 operator != (const float4 &v0, const float4 &v1);


float dot(const float4 &v0, const float4 &v1);
float dotXY(const float4 &v0, const float4 &v1);
float dotXZ(const float4 &v0, const float4 &v1);
float dotXYZ(const float4 &v0, const float4 &v1);

float4 dot4(const float4 &v0, const float4 &v1);

float length(const float4 &v);
float4 normalize(const float4 &v);



/************************************************************************/



struct ALIGN(16) float4x4
{
	float4 rows[4];

	float& operator () (uint row, uint col) { return rows[row][col]; }
	float  operator () (uint row, uint col) const { return rows[row][col]; }

	float4x4(){}
	float4x4(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23,
		const float m30, const float m31, const float m32, const float m33);
	
	void transpose();
};

float4x4 operator * (const float4x4 &mat, const float s);
float4x4 operator * (const float s, const float4x4 &mat);

float4x4 mul(const float4x4 &mat0, const float4x4 &mat1);
float4 mul(const float4x4 &mat, const float4 &vec);
float4 mul(const float4 &vec, const float4x4 &mat);

float4x4 translate(const float3& vec);
float4x4 transpose(const float4x4& mat);
float4x4 inverse(const float4x4& mat);


/************************************************************************/


struct half4
{
//	union
//	{
//		uint64 vec;
//		struct
//		{
			half x, y, z, w;
//		};
//	};

	// Constructors
	half4(){}
	half4(const half _x, const half _y, const half _z, const half _w);
	half4(const float _x, const float _y, const float _z, const float _w);
	half4(const float4 &v);
};

/************************************************************************/

// Utility functions

float4x4 PerspectiveMatrix(const float fov, const float ratio, const float z_near, const float z_far);
float4x4 PerspectiveMatrixInverse(const float fov, const float ratio, const float z_near, const float z_far);

float4x4 RotateX(const float angle);
float4x4 RotateY(const float angle);
float4x4 RotateZ(const float angle);

/************************************************************************/

float2 min(const float2 &v0, const float2 &v1);
float3 min(const float3 &v0, const float3 &v1);
float4 min(const float4 &v0, const float4 &v1);
float2 max(const float2 &v0, const float2 &v1);
float3 max(const float3 &v0, const float3 &v1);
float4 max(const float4 &v0, const float4 &v1);

float saturate(const float x);
float lerp(const float x0, const float x1, const float t);

// Vulkan compatibility
typedef int2 ivec2;
typedef int3 ivec3;
typedef int4 ivec4;
typedef float2 vec2;
typedef float3 vec3;
typedef float4 vec4;
typedef float4x4 mat4;
