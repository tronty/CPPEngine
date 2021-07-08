
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

#include "Vector.h"

#include <math.h>

// Constructors
bool2::bool2(const bool _x, const bool _y)
{
	x = _x;
	y = _y;
}

bool any(const bool2 b)
{
	return (b.vec != 0);
}

bool all(const bool2 b)
{
	return (b.vec == 0x3);
}


// Constructors
bool3::bool3(const bool _x, const bool _y, const bool _z)
{
	x = _x;
	y = _y;
	z = _z;
}

bool any(const bool3 b)
{
	return (b.vec != 0);
}

bool all(const bool3 b)
{
	return (b.vec == 0x7);
}


// Constructors
bool4::bool4(const bool _x, const bool _y, const bool _z, const bool _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

bool any(const bool4 b)
{
	return (b.vec != 0);
}

bool all(const bool4 b)
{
	return (b.vec == 0xF);
}



/************************************************************************/

int2::int2(const int32 _x, const int32 _y)
{
	x = _x;
	y = _y;
}


/************************************************************************/


int3::int3(const int32 _x, const int32 _y, const int32 _z)
{
	x = _x;
	y = _y;
	z = _z;
}

/************************************************************************/




int4::int4(const int32 _x, const int32 _y, const int32 _z, const int32 _w)
{
	vec = _mm_setr_epi32(_x, _y, _z, _w);
}

int4 int4::xxxx() const
{
	__m128i r = _mm_shuffle_epi32(vec, 0x0);
	return (int4 &) r;
}

int4 int4::yyyy() const
{
	__m128i r = _mm_shuffle_epi32(vec, 0x55);
	return (int4 &) r;
}

int4 int4::zzzz() const
{
	__m128i r = _mm_shuffle_epi32(vec, 0xAA);
	return (int4 &) r;
}

int4 int4::wwww() const
{
	__m128i r = _mm_shuffle_epi32(vec, 0xFF);
	return (int4 &) r;
}

int4 int4::xyxy() const
{
	__m128i r = _mm_shuffle_epi32(vec, 0x44);
	return (int4 &) r;
}

int4 int4::zwzw() const
{
	__m128i r = _mm_shuffle_epi32(vec, 0xEE);
	return (int4 &) r;
}

int4 int4::xyzw() const
{
	return (int4 &) vec;
}

int4 int4::wzyx() const
{
	__m128i r = _mm_shuffle_epi32(vec, 0x1B);
	return (int4 &) r;
}

void int4::operator += (const int4 &v)
{
	*this = operator + (*this, v);
}

void int4::operator += (const int s)
{
	*this = operator + (*this, s);
}

void int4::operator -= (const int4 &v)
{
	*this = operator - (*this, v);
}

void int4::operator -= (const int s)
{
	*this = operator - (*this, s);
}
/*
void int4::operator *= (const int4 &v)
{
	*this = operator * (*this, v);
}

void int4::operator *= (const int s)
{
	*this = operator * (*this, s);
}

void int4::operator /= (const int4 &v)
{
	*this = operator / (*this, v);
}

void int4::operator /= (const int s)
{
	*this = operator / (*this, s);
}
*/

int4 operator + (const int4 &v0, const int4 &v1)
{
	__m128i r = _mm_add_epi32((__m128i &) v0, (__m128i &) v1);
	return (int4 &) r;
}

int4 operator + (const int4 &v, const int s)
{
	__m128i r = _mm_add_epi32((__m128i &) v, _mm_set1_epi32(s));
	return (int4 &) r;
}

int4 operator + (const int s, const int4 &v)
{
	__m128i r = _mm_add_epi32(_mm_set1_epi32(s), (__m128i &) v);
	return (int4 &) r;
}

int4 operator - (const int4 &v0, const int4 &v1)
{
	__m128i r = _mm_sub_epi32((__m128i &) v0, (__m128i &) v1);
	return (int4 &) r;
}

int4 operator - (const int4 &v, const int s)
{
	__m128i r = _mm_sub_epi32((__m128i &) v, _mm_set1_epi32(s));
	return (int4 &) r;
}

int4 operator - (const int s, const int4 &v)
{
	__m128i r = _mm_sub_epi32(_mm_set1_epi32(s), (__m128i &) v);
	return (int4 &) r;
}

int4 operator - (const int4 &v)
{
	__m128i r = _mm_sub_epi32(_mm_setzero_si128(), (__m128i &) v);
	return (int4 &) r;
}


int4 operator * (const int4 &v0, const int4 &v1)
{
#ifdef USE_SSE4
	__m128i r = _mm_mullo_epi32((__m128i &) v0, (__m128i &) v1);
#else
	// Works with 16bit numbers only
	__m128i mask = _mm_set1_epi32(0x0000FFFF);
	__m128i z = _mm_and_si128((__m128i &) v0, mask);
	__m128i r = _mm_madd_epi16(z, (__m128i &) v1);
#endif
	return (int4 &) r;
}
/*
int4 operator * (const int4 &v, const int s)
{
	__m128i r = _mm_mul_epi32((__m128i &) v, _mm_set_ps1(s));
	return (int4 &) r;
}

int4 operator * (const int s, const int4 &v)
{
	__m128i r = _mm_mul_epi32(_mm_set_ps1(s), (__m128i &) v);
	return (int4 &) r;
}

int4 operator / (const int4 &v0, const int4 &v1)
{
	__m128i r = _mm_div_ps((__m128i &) v0, (__m128i &) v1);
	return (int4 &) r;
}

int4 operator / (const int4 &v, const int s)
{
	__m128i r = _mm_div_ps((__m128i &) v, _mm_set_ps1(s));
	return (int4 &) r;
}

int4 operator / (const int s, const int4 &v)
{
	__m128i r = _mm_div_ps(_mm_set_ps1(s), (__m128i &) v);
	return (int4 &) r;
}
*/


/************************************************************************/

float2 float2::xx() const
{
	return float2(x, x);
}

float2 float2::yy() const
{
	return float2(y, y);
}

float2 float2::xy() const
{
	return *this;
}

float2 float2::yx() const
{
	return float2(y, x);
}

void float2::operator += (const float2 &v)
{
	x += v.x;
	y += v.y;
}

void float2::operator += (const float s)
{
	x += s;
	y += s;
}

void float2::operator -= (const float2 &v)
{
	x -= v.x;
	y -= v.y;
}

void float2::operator -= (const float s)
{
	x -= s;
	y -= s;
}

void float2::operator *= (const float2 &v)
{
	x *= v.x;
	y *= v.y;
}

void float2::operator *= (const float s)
{
	x *= s;
	y *= s;
}

void float2::operator /= (const float2 &v)
{
	x /= v.x;
	y /= v.y;
}

void float2::operator /= (const float s)
{
	x /= s;
	y /= s;
}

float2 operator + (const float2 &v0, const float2 &v1)
{
	return float2(v0.x + v1.x, v0.y + v1.y);
}

float2 operator + (const float2 &v, const float s)
{
	return float2(v.x + s, v.y + s);
}

float2 operator + (const float s, const float2 &v)
{
	return float2(s + v.x, s + v.y);
}

float2 operator - (const float2 &v0, const float2 &v1)
{
	return float2(v0.x - v1.x, v0.y - v1.y);
}

float2 operator - (const float2 &v, const float s)
{
	return float2(v.x - s, v.y - s);
}

float2 operator - (const float s, const float2 &v)
{
	return float2(s - v.x, s - v.y);
}

float2 operator - (const float2 &v)
{
	return float2(-v.x, -v.y);
}

float2 operator * (const float2 &v0, const float2 &v1)
{
	return float2(v0.x * v1.x, v0.y * v1.y);
}

float2 operator * (const float2 &v, const float s)
{
	return float2(v.x * s, v.y * s);
}

float2 operator * (const float s, const float2 &v)
{
	return float2(s * v.x, s * v.y);
}

float2 operator / (const float2 &v0, const float2 &v1)
{
	return float2(v0.x / v1.x, v0.y / v1.y);
}

float2 operator / (const float2 &v, const float s)
{
	return float2(v.x / s, v.y / s);
}

float2 operator / (const float s, const float2 &v)
{
	return float2(s / v.x, s / v.y);
}

bool2 operator > (const float2 &v0, const float2 &v1)
{
	return bool2(v0.x > v1.x, v0.y > v1.y);
}

bool2 operator < (const float2 &v0, const float2 &v1)
{
	return bool2(v0.x < v1.x, v0.y < v1.y);
}

bool2 operator >= (const float2 &v0, const float2 &v1)
{
	return bool2(v0.x >= v1.x, v0.y >= v1.y);
}

bool2 operator <= (const float2 &v0, const float2 &v1)
{
	return bool2(v0.x <= v1.x, v0.y <= v1.y);
}

bool2 operator == (const float2 &v0, const float2 &v1)
{
	return bool2(v0.x == v1.x, v0.y == v1.y);
}

bool2 operator != (const float2 &v0, const float2 &v1)
{
	return bool2(v0.x != v1.x, v0.y != v1.y);
}

float dot(const float2 &v0, const float2 &v1)
{
	return v0.x * v1.x + v0.y * v1.y;
}

float length(const float2 &v)
{
	return sqrtf(dot(v, v));
}

float2 normalize(const float2 &v)
{
	return v * (1.0f / sqrtf(dot(v, v)));
}

/************************************************************************/

float3 float3::xxx() const
{
	return float3(x, x, x);
}

float3 float3::yyy() const
{
	return float3(y, y, y);
}

float3 float3::zzz() const
{
	return float3(z, z, z);
}

float3 float3::xyz() const
{
	return *this;
}

float3 float3::zyx() const
{
	return float3(z, y, x);
}

void float3::operator += (const float3 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

void float3::operator += (const float s)
{
	x += s;
	y += s;
	z += s;
}

void float3::operator -= (const float3 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

void float3::operator -= (const float s)
{
	x -= s;
	y -= s;
	z -= s;
}

void float3::operator *= (const float3 &v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
}

void float3::operator *= (const float s)
{
	x *= s;
	y *= s;
	z *= s;
}

void float3::operator /= (const float3 &v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
}

void float3::operator /= (const float s)
{
	x /= s;
	y /= s;
	z /= s;
}

bool3 operator > (const float3 &v0, const float3 &v1)
{
	return bool3(v0.x > v1.x, v0.y > v1.y, v0.z > v1.z);
}

bool3 operator < (const float3 &v0, const float3 &v1)
{
	return bool3(v0.x < v1.x, v0.y < v1.y, v0.z < v1.z);
}

bool3 operator >= (const float3 &v0, const float3 &v1)
{
	return bool3(v0.x >= v1.x, v0.y >= v1.y, v0.z >= v1.z);
}

bool3 operator <= (const float3 &v0, const float3 &v1)
{
	return bool3(v0.x <= v1.x, v0.y <= v1.y, v0.z <= v1.z);
}

bool3 operator == (const float3 &v0, const float3 &v1)
{
	return bool3(v0.x == v1.x, v0.y == v1.y, v0.z == v1.z);
}

bool3 operator != (const float3 &v0, const float3 &v1)
{
	return bool3(v0.x != v1.x, v0.y != v1.y, v0.z != v1.z);
}

float dot(const float3 &v0, const float3 &v1)
{
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}

float dotXY(const float3 &v0, const float3 &v1)
{
	return v0.x * v1.x + v0.y * v1.y;
}

float dotYZ(const float3 &v0, const float3 &v1)
{
	return v0.y * v1.y + v0.z * v1.z;
}

float dotXZ(const float3 &v0, const float3 &v1)
{
	return v0.x * v1.x + v0.z * v1.z;
}

float length(const float3 &v)
{
	return sqrtf(dot(v, v));
}

float3 normalize(const float3 &v)
{
	return v * (1.0f / sqrtf(dot(v, v)));
}

float3 cross(const float3 &v0, const float3 &v1)
{
	return float3(
		v0.y * v1.z - v0.z * v1.y,
		v0.z * v1.x - v0.x * v1.z,
		v0.x * v1.y - v0.y * v1.x);
}

/************************************************************************/

float4 float4::xxxx() const
{
	float4 r;
	r.vec = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), 0x0));
	return r;
}

float4 float4::yyyy() const
{
	float4 r;
	r.vec = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), 0x55));
	return r;
}

float4 float4::zzzz() const
{
	float4 r;
	r.vec = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), 0xAA));
	return r;
}

float4 float4::wwww() const
{
	float4 r;
	r.vec = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), 0xFF));
	return r;
}

float4 float4::xyxy() const
{
	float4 r;
	r.vec = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), 0x44));
	return r;
}

float4 float4::zwzw() const
{
	float4 r;
	r.vec = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), 0xEE));
	return r;
}

float4 float4::xyzw() const
{
	return *this;
}

float4 float4::wzyx() const
{
	float4 r;
	r.vec = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), 0x1B));
	return r;
}

void float4::operator += (const float4 &v)
{
	*this = operator + (*this, v);
}

void float4::operator += (const float s)
{
	*this = operator + (*this, s);
}

void float4::operator -= (const float4 &v)
{
	*this = operator - (*this, v);
}

void float4::operator -= (const float s)
{
	*this = operator - (*this, s);
}

void float4::operator *= (const float4 &v)
{
	*this = operator * (*this, v);
}

void float4::operator *= (const float s)
{
	*this = operator * (*this, s);
}

void float4::operator /= (const float4 &v)
{
	*this = operator / (*this, v);
}

void float4::operator /= (const float s)
{
	*this = operator / (*this, s);
}

float4 operator + (const float4 &v0, const float4 &v1)
{
	__m128 r = _mm_add_ps((__m128 &) v0, (__m128 &) v1);
	return (float4 &) r;
}

float4 operator + (const float4 &v, const float s)
{
	__m128 r = _mm_add_ps((__m128 &) v, _mm_set_ps1(s));
	return (float4 &) r;
}

float4 operator + (const float s, const float4 &v)
{
	__m128 r = _mm_add_ps(_mm_set_ps1(s), (__m128 &) v);
	return (float4 &) r;
}

float4 operator - (const float4 &v0, const float4 &v1)
{
	__m128 r = _mm_sub_ps((__m128 &) v0, (__m128 &) v1);
	return (float4 &) r;
}

float4 operator - (const float4 &v, const float s)
{
	__m128 r = _mm_sub_ps((__m128 &) v, _mm_set_ps1(s));
	return (float4 &) r;
}

float4 operator - (const float s, const float4 &v)
{
	__m128 r = _mm_sub_ps(_mm_set_ps1(s), (__m128 &) v);
	return (float4 &) r;
}

float4 operator - (const float4 &v)
{
	__m128 r = _mm_sub_ps(_mm_setzero_ps(), (__m128 &) v);
	return (float4 &) r;
}

float4 operator * (const float4 &v0, const float4 &v1)
{
	__m128 r = _mm_mul_ps((__m128 &) v0, (__m128 &) v1);
	return (float4 &) r;
}

float4 operator * (const float4 &v, const float s)
{
	__m128 r = _mm_mul_ps((__m128 &) v, _mm_set_ps1(s));
	return (float4 &) r;
}

float4 operator * (const float s, const float4 &v)
{
	__m128 r = _mm_mul_ps(_mm_set_ps1(s), (__m128 &) v);
	return (float4 &) r;
}

float4 operator / (const float4 &v0, const float4 &v1)
{
	__m128 r = _mm_div_ps((__m128 &) v0, (__m128 &) v1);
	return (float4 &) r;
}

float4 operator / (const float4 &v, const float s)
{
	__m128 r = _mm_div_ps((__m128 &) v, _mm_set_ps1(s));
	return (float4 &) r;
}

float4 operator / (const float s, const float4 &v)
{
	__m128 r = _mm_div_ps(_mm_set_ps1(s), (__m128 &) v);
	return (float4 &) r;
}

bool4 operator > (const float4 &v0, const float4 &v1)
{
	bool4 result;
	result.vec = (ubyte) _mm_movemask_ps(_mm_cmpgt_ps((__m128 &) v0, (__m128 &) v1));
	return result;
}

bool4 operator < (const float4 &v0, const float4 &v1)
{
	bool4 result;
	result.vec = (ubyte) _mm_movemask_ps(_mm_cmplt_ps((__m128 &) v0, (__m128 &) v1));
	return result;
}

bool4 operator >= (const float4 &v0, const float4 &v1)
{
	bool4 result;
	result.vec = (ubyte) _mm_movemask_ps(_mm_cmpge_ps((__m128 &) v0, (__m128 &) v1));
	return result;
}

bool4 operator <= (const float4 &v0, const float4 &v1)
{
	bool4 result;
	result.vec = (ubyte) _mm_movemask_ps(_mm_cmple_ps((__m128 &) v0, (__m128 &) v1));
	return result;
}

bool4 operator == (const float4 &v0, const float4 &v1)
{
	bool4 result;
	result.vec = (ubyte) _mm_movemask_ps(_mm_cmpeq_ps((__m128 &) v0, (__m128 &) v1));
	return result;
}

bool4 operator != (const float4 &v0, const float4 &v1)
{
	bool4 result;
	result.vec = (ubyte) _mm_movemask_ps(_mm_cmpneq_ps((__m128 &) v0, (__m128 &) v1));
	return result;
}

float dot(const float4 &v0, const float4 &v1)
{
#ifdef USE_SSE4
	__m128 d = _mm_dp_ps((__m128 &) v0, (__m128 &) v1, 0xFF);
#else
	__m128 d = _mm_mul_ps((__m128 &) v0, (__m128 &) v1);
	d = _mm_hadd_ps(d, d);
	d = _mm_hadd_ps(d, d);
#endif
	return _mm_cvtss_f32(d);
}

float dotXY(const float4 &v0, const float4 &v1)
{
#ifdef USE_SSE4
	__m128 d = _mm_dp_ps((__m128 &) v0, (__m128 &) v1, 0x31);
#else
	__m128 d = _mm_mul_ps((__m128 &) v0, (__m128 &) v1);
	d = _mm_hadd_ps(d, d);
#endif
	return _mm_cvtss_f32(d);
}

// Disable "uninitialized local variable used" warning
#ifdef WIN32
#pragma warning (disable: 4700)
#else
#pragma GCC diagnostic ignored "-Wuninitialized"
#endif

float dotXZ(const float4 &v0, const float4 &v1)
{
#ifdef USE_SSE4
	__m128 d = _mm_dp_ps((__m128 &) v0, (__m128 &) v1, 0x51);
#else
	__m128 d = _mm_mul_ps((__m128 &) v0, (__m128 &) v1);
	__m128 e = _mm_movehl_ps(e, d);
	d = _mm_add_ss(d, e);
#endif
	return _mm_cvtss_f32(d);
}

float dotXYZ(const float4 &v0, const float4 &v1)
{
#ifdef USE_SSE4
	__m128 d = _mm_dp_ps((__m128 &) v0, (__m128 &) v1, 0x71);
#else
	__m128 d = _mm_mul_ps((__m128 &) v0, (__m128 &) v1);
	__m128 e = _mm_movehl_ps(e, d);
	d = _mm_add_ss(d, e);
	d = _mm_hadd_ps(d, d);
#endif
	return _mm_cvtss_f32(d);
}

// Enable warning again
#ifdef WIN32
#pragma warning (default: 4700)
#else
//#pragma GCC diagnostic warning "-Wuninitialized"
#endif

float4 dot4(const float4 &v0, const float4 &v1)
{
#ifdef USE_SSE4
	return (float4 &) _mm_dp_ps((__m128 &) v0, (__m128 &) v1, 0xFF);
#else
	__m128 d = _mm_mul_ps((__m128 &) v0, (__m128 &) v1);
	d = _mm_hadd_ps(d, d);
	d = _mm_hadd_ps(d, d);
	return (float4 &) d;
#endif
}

float length(const float4 &v)
{
#ifdef USE_SSE4
	__m128 d = _mm_dp_ps((__m128 &) v, (__m128 &) v, 0xFF);
#else
	__m128 d = _mm_mul_ps((__m128 &) v, (__m128 &) v);
	d = _mm_hadd_ps(d, d);
	d = _mm_hadd_ps(d, d);
#endif
	return _mm_cvtss_f32(_mm_sqrt_ss(d));
}

float4 normalize(const float4 &v)
{
#ifdef USE_SSE4
	__m128 d = _mm_dp_ps((__m128 &) v, (__m128 &) v, 0xFF);
#else
	__m128 d = _mm_mul_ps((__m128 &) v, (__m128 &) v);
	d = _mm_hadd_ps(d, d);
	d = _mm_hadd_ps(d, d);
#endif
	d = _mm_mul_ps((__m128 &) v, _mm_rsqrt_ps(d));
	return (float4 &) d;
}



/************************************************************************/



float4x4::float4x4(
	const float m00, const float m01, const float m02, const float m03,
	const float m10, const float m11, const float m12, const float m13,
	const float m20, const float m21, const float m22, const float m23,
	const float m30, const float m31, const float m32, const float m33)
{
	__m128 r0 = _mm_setr_ps(m00, m01, m02, m03);
	__m128 r1 = _mm_setr_ps(m10, m11, m12, m13);
	__m128 r2 = _mm_setr_ps(m20, m21, m22, m23);
	__m128 r3 = _mm_setr_ps(m30, m31, m32, m33);
	rows[0] = (float4 &) r0;
	rows[1] = (float4 &) r1;
	rows[2] = (float4 &) r2;
	rows[3] = (float4 &) r3;
}

void float4x4::transpose()
{
	__m128 r0 = _mm_shuffle_ps((__m128 &) rows[0], (__m128 &) rows[1], 0x44);
	__m128 r1 = _mm_shuffle_ps((__m128 &) rows[0], (__m128 &) rows[1], 0xEE);
	__m128 r2 = _mm_shuffle_ps((__m128 &) rows[2], (__m128 &) rows[3], 0x44);
	__m128 r3 = _mm_shuffle_ps((__m128 &) rows[2], (__m128 &) rows[3], 0xEE);

	__m128 r4 = _mm_shuffle_ps(r0, r2, 0x88);
	__m128 r5 = _mm_shuffle_ps(r0, r2, 0xDD);
	__m128 r6 = _mm_shuffle_ps(r1, r3, 0x88);
	__m128 r7 = _mm_shuffle_ps(r1, r3, 0xDD);

	rows[0] = (float4 &) r4;
	rows[1] = (float4 &) r5;
	rows[2] = (float4 &) r6;
	rows[3] = (float4 &) r7;
}

float4x4 operator * (const float4x4 &mat, const float s)
{
	float4x4 result;

	result.rows[0] = mat.rows[0] * s;
	result.rows[1] = mat.rows[1] * s;
	result.rows[2] = mat.rows[2] * s;
	result.rows[3] = mat.rows[3] * s;

	return result;
}

float4x4 operator * (const float s, const float4x4 &mat)
{
	float4x4 result;

	result.rows[0] = mat.rows[0] * s;
	result.rows[1] = mat.rows[1] * s;
	result.rows[2] = mat.rows[2] * s;
	result.rows[3] = mat.rows[3] * s;

	return result;
}

float4x4 mul(const float4x4 &mat0, const float4x4 &mat1)
{
	float4x4 result;

	result.rows[0] = mul(mat0.rows[0], mat1);
	result.rows[1] = mul(mat0.rows[1], mat1);
	result.rows[2] = mul(mat0.rows[2], mat1);
	result.rows[3] = mul(mat0.rows[3], mat1);

	return result;
}

float4 mul(const float4x4 &mat, const float4 &vec)
{

	
	
#if 0

	//return vec4(dot(m.rows[0], v), dot(m.rows[1], v), dot(m.rows[2], v), dot(m.rows[3], v));
	float4 result;

	result.x = mat.rows[0].x * vec.x + mat.rows[0].y * vec.y + mat.rows[0].z * vec.z + mat.rows[0].w * vec.w;
	result.y = mat.rows[1].x * vec.x + mat.rows[1].y * vec.y + mat.rows[1].z * vec.z + mat.rows[1].w * vec.w;
	result.z = mat.rows[2].x * vec.x + mat.rows[2].y * vec.y + mat.rows[2].z * vec.z + mat.rows[2].w * vec.w;
	result.w = mat.rows[3].x * vec.x + mat.rows[3].y * vec.y + mat.rows[3].z * vec.z + mat.rows[3].w * vec.w;

	return result;

#else
	
	
	
	
	
	
	
	
	
	
	
/*
#ifdef USE_SSE4
	__m128 r0 = _mm_dp_ps((__m128 &) mat.rows[0], (__m128 &) vec, 0xF1);
	__m128 r1 = _mm_dp_ps((__m128 &) mat.rows[1], (__m128 &) vec, 0xF2);
	__m128 r2 = _mm_dp_ps((__m128 &) mat.rows[2], (__m128 &) vec, 0xF4);
	__m128 r3 = _mm_dp_ps((__m128 &) mat.rows[3], (__m128 &) vec, 0xF8);

	__m128 r4 = _mm_or_ps(r0, r1);
	__m128 r5 = _mm_or_ps(r2, r3);

	__m128 r6 = _mm_or_ps(r4, r5);

	return (float4 &) r6;
#else
*/
	__m128 r0 = _mm_mul_ps((__m128 &) mat.rows[0], (__m128 &) vec);
	__m128 r1 = _mm_mul_ps((__m128 &) mat.rows[1], (__m128 &) vec);
	__m128 r2 = _mm_mul_ps((__m128 &) mat.rows[2], (__m128 &) vec);
	__m128 r3 = _mm_mul_ps((__m128 &) mat.rows[3], (__m128 &) vec);

	__m128 r4 = _mm_hadd_ps(r0, r1);
	__m128 r5 = _mm_hadd_ps(r2, r3);

	__m128 r6 = _mm_hadd_ps(r4, r5);

	return (float4 &) r6;
//#endif

#endif
}

float4 mul(const float4& vec, const float4x4& mat)
{
	__m128 s0 = _mm_shuffle_ps((__m128 &) vec, (__m128 &) vec, 0x00);
	__m128 s1 = _mm_shuffle_ps((__m128 &) vec, (__m128 &) vec, 0x55);
	__m128 s2 = _mm_shuffle_ps((__m128 &) vec, (__m128 &) vec, 0xAA);
	__m128 s3 = _mm_shuffle_ps((__m128 &) vec, (__m128 &) vec, 0xFF);

	__m128 r0 = _mm_mul_ps((__m128 &) mat.rows[0], s0);
	__m128 r1 = _mm_mul_ps((__m128 &) mat.rows[1], s1);
	__m128 r2 = _mm_mul_ps((__m128 &) mat.rows[2], s2);
	__m128 r3 = _mm_mul_ps((__m128 &) mat.rows[3], s3);

	__m128 r4 = _mm_add_ps(r0, r1);
	__m128 r5 = _mm_add_ps(r2, r3);

	__m128 r6 = _mm_add_ps(r4, r5);

	return (float4 &) r6;
}

float4x4 translate(const float3& vec)
{
	return float4x4(1,0,0,vec.x, 0,1,0,vec.y, 0,0,1,vec.z, 0,0,0,1);
}

float4x4 transpose(const float4x4& mat)
{
	float4x4 result;

	__m128 r0 = _mm_shuffle_ps((__m128 &) mat.rows[0], (__m128 &) mat.rows[1], 0x44);
	__m128 r1 = _mm_shuffle_ps((__m128 &) mat.rows[0], (__m128 &) mat.rows[1], 0xEE);
	__m128 r2 = _mm_shuffle_ps((__m128 &) mat.rows[2], (__m128 &) mat.rows[3], 0x44);
	__m128 r3 = _mm_shuffle_ps((__m128 &) mat.rows[2], (__m128 &) mat.rows[3], 0xEE);

	__m128 r4 = _mm_shuffle_ps(r0, r2, 0x88);
	__m128 r5 = _mm_shuffle_ps(r0, r2, 0xDD);
	__m128 r6 = _mm_shuffle_ps(r1, r3, 0x88);
	__m128 r7 = _mm_shuffle_ps(r1, r3, 0xDD);

	result.rows[0] = (float4 &) r4;
	result.rows[1] = (float4 &) r5;
	result.rows[2] = (float4 &) r6;
	result.rows[3] = (float4 &) r7;

	return result;
}

float4x4 inverse(const float4x4& mat)
{
	float4x4 result;

	__m128 r0 = _mm_shuffle_ps((__m128 &) mat.rows[0], (__m128 &) mat.rows[1], 0x44);
	__m128 r1 = _mm_shuffle_ps((__m128 &) mat.rows[0], (__m128 &) mat.rows[1], 0xEE);
	__m128 r2 = _mm_shuffle_ps((__m128 &) mat.rows[2], (__m128 &) mat.rows[3], 0x44);
	__m128 r3 = _mm_shuffle_ps((__m128 &) mat.rows[2], (__m128 &) mat.rows[3], 0xEE);

	__m128 row0 = _mm_shuffle_ps(r0, r2, 0x88);
	__m128 row1 = _mm_shuffle_ps(r2, r0, 0xDD);
	__m128 row2 = _mm_shuffle_ps(r1, r3, 0x88);
	__m128 row3 = _mm_shuffle_ps(r3, r1, 0xDD);

	__m128 tmp, minor0, minor1, minor2, minor3;

	tmp = _mm_mul_ps(row2, row3);
	tmp = _mm_shuffle_ps(tmp, tmp, 0xB1);
	minor0 = _mm_mul_ps(row1, tmp);
	minor1 = _mm_mul_ps(row0, tmp);
	tmp = _mm_shuffle_ps(tmp, tmp, 0x4E);
	minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp), minor0);
	minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp), minor1);
	minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);

	tmp = _mm_mul_ps(row1, row2);
	tmp = _mm_shuffle_ps(tmp, tmp, 0xB1);
	minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp), minor0);
	minor3 = _mm_mul_ps(row0, tmp);
	tmp = _mm_shuffle_ps(tmp, tmp, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp));
	minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp), minor3);
	minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);

	tmp = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
	tmp = _mm_shuffle_ps(tmp, tmp, 0xB1);
	row2 = _mm_shuffle_ps(row2, row2, 0x4E);
	minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp), minor0);
	minor2 = _mm_mul_ps(row0, tmp);
	tmp = _mm_shuffle_ps(tmp, tmp, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp));
	minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp), minor2);
	minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);

	tmp = _mm_mul_ps(row0, row1);
	tmp = _mm_shuffle_ps(tmp, tmp, 0xB1);
	minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp), minor2);
	minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp), minor3);
	tmp = _mm_shuffle_ps(tmp, tmp, 0x4E);
	minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp), minor2);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp));

	tmp = _mm_mul_ps(row0, row3);
	tmp = _mm_shuffle_ps(tmp, tmp, 0xB1);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp));
	minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp), minor2);
	tmp = _mm_shuffle_ps(tmp, tmp, 0x4E);
	minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp), minor1);
	minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp));

	tmp = _mm_mul_ps(row0, row2);
	tmp = _mm_shuffle_ps(tmp, tmp, 0xB1);
	minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp), minor1);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp));
	tmp = _mm_shuffle_ps(tmp, tmp, 0x4E);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp));
	minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp), minor3);

	__m128 det = _mm_mul_ps(row0, minor0);
	det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
	det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
	tmp = _mm_rcp_ss(det);
	det = _mm_sub_ss(_mm_add_ss(tmp, tmp), _mm_mul_ss(det, _mm_mul_ss(tmp, tmp)));
	det = _mm_shuffle_ps(det, det, 0x00);

	row0 = _mm_mul_ps(det, minor0);
	row1 = _mm_mul_ps(det, minor1);
	row2 = _mm_mul_ps(det, minor2);
	row3 = _mm_mul_ps(det, minor3);
	
	result.rows[0] = (float4 &) row0;
	result.rows[1] = (float4 &) row1;
	result.rows[2] = (float4 &) row2;
	result.rows[3] = (float4 &) row3;

	return result;
}



/************************************************************************/


half4::half4(const half _x, const half _y, const half _z, const half _w)
{
	x.sh = _x.sh;
	y.sh = _y.sh;
	z.sh = _z.sh;
	w.sh = _w.sh;
}

half4::half4(const float _x, const float _y, const float _z, const float _w)
{
	float4 v(_x, _y, _z, _w);
	*this = v;
}

half4::half4(const float4 &v)
{
	// Based on code from here:
	// http://www.devmaster.net/forums/showthread.php?t=10924

	__m128i reg0, reg1, reg2, reg3, reg4, reg5;

	reg0 = _mm_castps_si128(v.vec);

	// Sign
	reg1 = _mm_andnot_si128(g_SSE_Globals.Abs, reg0);
	reg1 = _mm_srai_epi32(reg1, 16);

	// Compute masks
	reg0 = _mm_and_si128(reg0, g_SSE_Globals.Abs);
	reg2 = _mm_castps_si128(_mm_cmpnlt_ps(_mm_castsi128_ps(reg0), _mm_castsi128_ps(g_SSE_Globals.Infinity)));
	reg3 = _mm_castps_si128(_mm_cmplt_ps(_mm_castsi128_ps(reg0), _mm_castsi128_ps(g_SSE_Globals.Denormal)));

	// Denormal case
	reg4 = _mm_and_si128(reg0, g_SSE_Globals.Exponent);
	reg4 = _mm_add_epi32(reg4, g_SSE_Globals.Adjust);
	reg5 = _mm_and_si128(reg0, g_SSE_Globals.Mantissa);
	reg5 = _mm_or_si128(reg5, g_SSE_Globals.OneDot);
	reg5 = _mm_cvtps_epi32(_mm_castsi128_ps(reg5));
	reg5 = _mm_castps_si128(_mm_mul_ps(_mm_castsi128_ps(reg5), _mm_castsi128_ps(reg4)));
	reg5 = _mm_cvttps_epi32(_mm_castsi128_ps(reg5));

	// Normal case and combine
	reg0 = _mm_add_epi32(reg0, g_SSE_Globals.Fixup);
	reg5 = _mm_and_si128(reg5, reg3);
	reg3 = _mm_andnot_si128(reg3, reg0);
	reg5 = _mm_or_si128(reg5, reg3);

	// Correct rounding
	reg0 = _mm_srli_epi32(reg5, 13);
	reg0 = _mm_and_si128(reg0, g_SSE_Globals.Round0);
	reg0 = _mm_add_epi32(reg0, g_SSE_Globals.Round1);
	reg0 = _mm_add_epi32(reg0, reg5);

	// Combine with sign and infinity
	reg0 = _mm_srli_epi32(reg0, 13);
	reg0 = _mm_or_si128(reg0, reg2);
	reg0 = _mm_and_si128(reg0, g_SSE_Globals.Base);

	// Result in lower words of each element
	reg0 = _mm_or_si128(reg0, reg1);

	// Convert to 16bit
	reg0 = _mm_packs_epi32(reg0, reg0);

	// Store result
	//_mm_storel_pi((__m64 *) &vec, _mm_castsi128_ps(reg0));
	_mm_storel_pi((__m64 *) &x, _mm_castsi128_ps(reg0));
}

float4x4 PerspectiveMatrix(const float fov, const float ratio, const float z_near, const float z_far)
{
	const float f = 0.5f * fov;
	const float h = cosf(f) / sinf(f);
	const float w = h * ratio;

	return float4x4(
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, z_far / (z_far - z_near), -(z_far * z_near) / (z_far - z_near),
		0, 0, 1, 0);
}

float4x4 PerspectiveMatrixInverse(const float fov, const float ratio, const float z_near, const float z_far)
{
	const float f = 0.5f * fov;
	const float h = cosf(f) / sinf(f);
	const float w = h * ratio;

	return float4x4(
		1.0f / w, 0, 0, 0,
		0, 1.0f / h, 0, 0,
		0, 0, 0, 1.0f,
		0, 0, (z_near - z_far) / (z_far * z_near), 1.0f / z_near);
}

float4x4 RotateX(const float angle)
{
	float cosA = cosf(angle);
	float sinA = sinf(angle);

	return float4x4(
		1, 0,     0,    0,
		0, cosA, -sinA, 0,
		0, sinA,  cosA, 0,
		0, 0,     0,    1);
}

float4x4 RotateY(const float angle)
{
	float cosA = cosf(angle);
	float sinA = sinf(angle);

	return float4x4(
		cosA, 0, -sinA, 0,
		0,    1,  0,    0,
		sinA, 0,  cosA, 0,
		0,    0,  0,    1);
}

float4x4 RotateZ(const float angle)
{
	float cosA = cosf(angle);
	float sinA = sinf(angle);

	return float4x4(
		cosA, -sinA, 0, 0,
		sinA,  cosA, 0, 0,
		0,     0,    1, 0,
		0,     0,    0, 1);
}

float2 min(const float2 &v0, const float2 &v1)
{
	return float2(min(v0.x, v1.x), min(v0.y, v1.y));
}

float3 min(const float3 &v0, const float3 &v1)
{
	return float3(min(v0.x, v1.x), min(v0.y, v1.y), min(v0.z, v1.z));
}

float4 min(const float4 &v0, const float4 &v1)
{
	float4 r;
	r.vec = _mm_min_ps(v0.vec, v1.vec);
	return r;
}

float2 max(const float2 &v0, const float2 &v1)
{
	return float2(max(v0.x, v1.x), max(v0.y, v1.y));
}

float3 max(const float3 &v0, const float3 &v1)
{
	return float3(max(v0.x, v1.x), max(v0.y, v1.y), max(v0.z, v1.z));
}

float4 max(const float4 &v0, const float4 &v1)
{
	float4 r;
	r.vec = _mm_max_ps(v0.vec, v1.vec);
	return r;
}

float saturate(const float x)
{
	return (x < 0.0f)? 0.0f : (x > 1.0f)? 1.0f : x;
}

float lerp(const float x0, const float x1, const float t)
{
	return (1.0f - t) * x0 + t * x1;
}
