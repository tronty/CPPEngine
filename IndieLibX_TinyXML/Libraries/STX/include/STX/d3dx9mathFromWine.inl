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
#ifndef __D3DXFROMWINE9MATH_INL__
#define __D3DXFROMWINE9MATH_INL__

/* constructors & operators */
#ifdef __cplusplus

inline D3DXFROMWINEVECTOR2::D3DXFROMWINEVECTOR2()
{
}

inline D3DXFROMWINEVECTOR2::D3DXFROMWINEVECTOR2(CONST FLOAT *pf)
{
    if(!pf) return;
    x = pf[0];
    y = pf[1];
}

inline D3DXFROMWINEVECTOR2::D3DXFROMWINEVECTOR2(FLOAT fx, FLOAT fy)
{
    x = fx;
    y = fy;
}

inline D3DXFROMWINEVECTOR2::operator FLOAT* ()
{
    return (FLOAT*)&x;
}

inline D3DXFROMWINEVECTOR2::operator CONST FLOAT* () const
{
    return (CONST FLOAT*)&x;
}

inline D3DXFROMWINEVECTOR2& D3DXFROMWINEVECTOR2::operator += (CONST D3DXFROMWINEVECTOR2& v)
{
    x += v.x;
    y += v.y;
    return *this;
}

inline D3DXFROMWINEVECTOR2& D3DXFROMWINEVECTOR2::operator -= (CONST D3DXFROMWINEVECTOR2& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

inline D3DXFROMWINEVECTOR2& D3DXFROMWINEVECTOR2::operator *= (FLOAT f)
{
    x *= f;
    y *= f;
    return *this;
}

inline D3DXFROMWINEVECTOR2& D3DXFROMWINEVECTOR2::operator /= (FLOAT f)
{
    x /= f;
    y /= f;
    return *this;
}

inline D3DXFROMWINEVECTOR2 D3DXFROMWINEVECTOR2::operator + () const
{
    return *this;
}

inline D3DXFROMWINEVECTOR2 D3DXFROMWINEVECTOR2::operator - () const
{
    return D3DXFROMWINEVECTOR2(-x, -y);
}

inline D3DXFROMWINEVECTOR2 D3DXFROMWINEVECTOR2::operator + (CONST D3DXFROMWINEVECTOR2& v) const
{
    return D3DXFROMWINEVECTOR2(x + v.x, y + v.y);
}

inline D3DXFROMWINEVECTOR2 D3DXFROMWINEVECTOR2::operator - (CONST D3DXFROMWINEVECTOR2& v) const
{
    return D3DXFROMWINEVECTOR2(x - v.x, y - v.y);
}

inline D3DXFROMWINEVECTOR2 D3DXFROMWINEVECTOR2::operator * (FLOAT f) const
{
    return D3DXFROMWINEVECTOR2(x * f, y * f);
}

inline D3DXFROMWINEVECTOR2 D3DXFROMWINEVECTOR2::operator / (FLOAT f) const
{
    return D3DXFROMWINEVECTOR2(x / f, y / f);
}
#if !defined(SWIG)
inline D3DXFROMWINEVECTOR2 operator * (FLOAT f, CONST D3DXFROMWINEVECTOR2& v)
{
    return D3DXFROMWINEVECTOR2(f * v.x, f * v.y);
}
#endif
inline BOOL D3DXFROMWINEVECTOR2::operator == (CONST D3DXFROMWINEVECTOR2& v) const
{
    return x == v.x && y == v.y;
}

inline BOOL D3DXFROMWINEVECTOR2::operator != (CONST D3DXFROMWINEVECTOR2& v) const
{
    return x != v.x || y != v.y;
}

inline D3DXFROMWINEVECTOR3::D3DXFROMWINEVECTOR3()
{
}

inline D3DXFROMWINEVECTOR3::D3DXFROMWINEVECTOR3(CONST FLOAT *pf)
{
    if(!pf) return;
    x = pf[0];
    y = pf[1];
    z = pf[2];
}

inline D3DXFROMWINEVECTOR3::D3DXFROMWINEVECTOR3(CONST D3DFROMWINEVECTOR& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
}

inline D3DXFROMWINEVECTOR3::D3DXFROMWINEVECTOR3(FLOAT fx, FLOAT fy, FLOAT fz)
{
    x = fx;
    y = fy;
    z = fz;
}

inline D3DXFROMWINEVECTOR3::operator FLOAT* ()
{
    return (FLOAT*)&x;
}

inline D3DXFROMWINEVECTOR3::operator CONST FLOAT* () const
{
    return (CONST FLOAT*)&x;
}

inline D3DXFROMWINEVECTOR3& D3DXFROMWINEVECTOR3::operator += (CONST D3DXFROMWINEVECTOR3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

inline D3DXFROMWINEVECTOR3& D3DXFROMWINEVECTOR3::operator -= (CONST D3DXFROMWINEVECTOR3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

inline D3DXFROMWINEVECTOR3& D3DXFROMWINEVECTOR3::operator *= (FLOAT f)
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

inline D3DXFROMWINEVECTOR3& D3DXFROMWINEVECTOR3::operator /= (FLOAT f)
{
    x /= f;
    y /= f;
    z /= f;
    return *this;
}

inline D3DXFROMWINEVECTOR3 D3DXFROMWINEVECTOR3::operator + () const
{
    return *this;
}

inline D3DXFROMWINEVECTOR3 D3DXFROMWINEVECTOR3::operator - () const
{
    return D3DXFROMWINEVECTOR3(-x, -y, -z);
}

inline D3DXFROMWINEVECTOR3 D3DXFROMWINEVECTOR3::operator + (CONST D3DXFROMWINEVECTOR3& v) const
{
    return D3DXFROMWINEVECTOR3(x + v.x, y + v.y, z + v.z);
}

inline D3DXFROMWINEVECTOR3 D3DXFROMWINEVECTOR3::operator - (CONST D3DXFROMWINEVECTOR3& v) const
{
    return D3DXFROMWINEVECTOR3(x - v.x, y - v.y, z - v.z);
}

inline D3DXFROMWINEVECTOR3 D3DXFROMWINEVECTOR3::operator * (FLOAT f) const
{
    return D3DXFROMWINEVECTOR3(x * f, y * f, z * f);
}

inline D3DXFROMWINEVECTOR3 D3DXFROMWINEVECTOR3::operator / (FLOAT f) const
{
    return D3DXFROMWINEVECTOR3(x / f, y / f, z / f);
}
#if !defined(SWIG)
inline D3DXFROMWINEVECTOR3 operator * (FLOAT f, CONST D3DXFROMWINEVECTOR3& v)
{
    return D3DXFROMWINEVECTOR3(f * v.x, f * v.y, f * v.z);
}
#endif
inline BOOL D3DXFROMWINEVECTOR3::operator == (CONST D3DXFROMWINEVECTOR3& v) const
{
    return x == v.x && y == v.y && z == v.z;
}

inline BOOL D3DXFROMWINEVECTOR3::operator != (CONST D3DXFROMWINEVECTOR3& v) const
{
    return x != v.x || y != v.y || z != v.z;
}

inline D3DXFROMWINEVECTOR4::D3DXFROMWINEVECTOR4()
{
}

inline D3DXFROMWINEVECTOR4::D3DXFROMWINEVECTOR4(CONST FLOAT *pf)
{
    if(!pf) return;
    x = pf[0];
    y = pf[1];
    z = pf[2];
    w = pf[3];
}

inline D3DXFROMWINEVECTOR4::D3DXFROMWINEVECTOR4(FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw)
{
    x = fx;
    y = fy;
    z = fz;
    w = fw;
}

inline D3DXFROMWINEVECTOR4::operator FLOAT* ()
{
    return (FLOAT*)&x;
}

inline D3DXFROMWINEVECTOR4::operator CONST FLOAT* () const
{
    return (CONST FLOAT*)&x;
}

inline D3DXFROMWINEVECTOR4& D3DXFROMWINEVECTOR4::operator += (CONST D3DXFROMWINEVECTOR4& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

inline D3DXFROMWINEVECTOR4& D3DXFROMWINEVECTOR4::operator -= (CONST D3DXFROMWINEVECTOR4& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

inline D3DXFROMWINEVECTOR4& D3DXFROMWINEVECTOR4::operator *= (FLOAT f)
{
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return *this;
}

inline D3DXFROMWINEVECTOR4& D3DXFROMWINEVECTOR4::operator /= (FLOAT f)
{
    x /= f;
    y /= f;
    z /= f;
    w /= f;
    return *this;
}

inline D3DXFROMWINEVECTOR4 D3DXFROMWINEVECTOR4::operator + () const
{
    return *this;
}

inline D3DXFROMWINEVECTOR4 D3DXFROMWINEVECTOR4::operator - () const
{
    return D3DXFROMWINEVECTOR4(-x, -y, -z, -w);
}

inline D3DXFROMWINEVECTOR4 D3DXFROMWINEVECTOR4::operator + (CONST D3DXFROMWINEVECTOR4& v) const
{
    return D3DXFROMWINEVECTOR4(x + v.x, y + v.y, z + v.z, w + v.w);
}

inline D3DXFROMWINEVECTOR4 D3DXFROMWINEVECTOR4::operator - (CONST D3DXFROMWINEVECTOR4& v) const
{
    return D3DXFROMWINEVECTOR4(x - v.x, y - v.y, z - v.z, w - v.w);
}

inline D3DXFROMWINEVECTOR4 D3DXFROMWINEVECTOR4::operator * (FLOAT f) const
{
    return D3DXFROMWINEVECTOR4(x * f, y * f, z * f, w * f);
}

inline D3DXFROMWINEVECTOR4 D3DXFROMWINEVECTOR4::operator / (FLOAT f) const
{
    return D3DXFROMWINEVECTOR4(x / f, y / f, z / f, w / f);
}
#if !defined(SWIG)
inline D3DXFROMWINEVECTOR4 operator * (FLOAT f, CONST D3DXFROMWINEVECTOR4& v)
{
    return D3DXFROMWINEVECTOR4(f * v.x, f * v.y, f * v.z, f * v.w);
}
#endif
inline BOOL D3DXFROMWINEVECTOR4::operator == (CONST D3DXFROMWINEVECTOR4& v) const
{
    return x == v.x && y == v.y && z == v.z && w == v.w;
}

inline BOOL D3DXFROMWINEVECTOR4::operator != (CONST D3DXFROMWINEVECTOR4& v) const
{
    return x != v.x || y != v.y || z != v.z || w != v.w;
}

inline D3DXFROMWINEMATRIX::D3DXFROMWINEMATRIX()
{
}
#if 0
namespace STX
{
#ifndef stx_memcpy
void *stx_memcpy(void *dst, const void *src, size_t len);
#endif
#ifndef stx_memcmp
int stx_memcmp(const void *s1, const void *s2, size_t len);
#endif
};
#endif
inline D3DXFROMWINEMATRIX::D3DXFROMWINEMATRIX(CONST FLOAT *pf)
{
    if(!pf) return;
	stx_memcpy(&m[0][0], pf, sizeof(D3DXFROMWINEMATRIX));
}

inline D3DXFROMWINEMATRIX::D3DXFROMWINEMATRIX(CONST D3DFROMWINEMATRIX& mat)
{
    stx_memcpy(&m[0][0], &mat, sizeof(D3DXFROMWINEMATRIX));
}

inline D3DXFROMWINEMATRIX::D3DXFROMWINEMATRIX(FLOAT f11, FLOAT f12, FLOAT f13, FLOAT f14,
                              FLOAT f21, FLOAT f22, FLOAT f23, FLOAT f24,
                              FLOAT f31, FLOAT f32, FLOAT f33, FLOAT f34,
                              FLOAT f41, FLOAT f42, FLOAT f43, FLOAT f44)
{
    m[0][0] = f11; m[0][1] = f12; m[0][2] = f13; m[0][3] = f14;
    m[1][0] = f21; m[1][1] = f22; m[1][2] = f23; m[1][3] = f24;
    m[2][0] = f31; m[2][1] = f32; m[2][2] = f33; m[2][3] = f34;
    m[3][0] = f41; m[3][1] = f42; m[3][2] = f43; m[3][3] = f44;
}

inline FLOAT& D3DXFROMWINEMATRIX::operator () (UINT row, UINT col)
{
    return m[row][col];
}

inline FLOAT D3DXFROMWINEMATRIX::operator () (UINT row, UINT col) const
{
    return m[row][col];
}

inline D3DXFROMWINEMATRIX::operator FLOAT* ()
{
    return (FLOAT*)&m[0][0];
}

inline D3DXFROMWINEMATRIX::operator CONST FLOAT* () const
{
    return (CONST FLOAT*)&m[0][0];
}

inline D3DXFROMWINEMATRIX& D3DXFROMWINEMATRIX::operator *= (CONST D3DXFROMWINEMATRIX& mat)
{
    D3DXFROMWINEMatrixMultiply(this, this, &mat);
    return *this;
}

inline D3DXFROMWINEMATRIX& D3DXFROMWINEMATRIX::operator += (CONST D3DXFROMWINEMATRIX& mat)
{
    m[0][0] += mat.m[0][0]; m[0][1] += mat.m[0][1]; m[0][2] += mat.m[0][2]; m[0][3] += mat.m[0][3];
    m[1][0] += mat.m[1][0]; m[1][1] += mat.m[1][1]; m[1][2] += mat.m[1][2]; m[1][3] += mat.m[1][3];
    m[2][0] += mat.m[2][0]; m[2][1] += mat.m[2][1]; m[2][2] += mat.m[2][2]; m[2][3] += mat.m[2][3];
    m[3][0] += mat.m[3][0]; m[3][1] += mat.m[3][1]; m[3][2] += mat.m[3][2]; m[3][3] += mat.m[3][3];
    return *this;
}

inline D3DXFROMWINEMATRIX& D3DXFROMWINEMATRIX::operator -= (CONST D3DXFROMWINEMATRIX& mat)
{
    m[0][0] -= mat.m[0][0]; m[0][1] -= mat.m[0][1]; m[0][2] -= mat.m[0][2]; m[0][3] -= mat.m[0][3];
    m[1][0] -= mat.m[1][0]; m[1][1] -= mat.m[1][1]; m[1][2] -= mat.m[1][2]; m[1][3] -= mat.m[1][3];
    m[2][0] -= mat.m[2][0]; m[2][1] -= mat.m[2][1]; m[2][2] -= mat.m[2][2]; m[2][3] -= mat.m[2][3];
    m[3][0] -= mat.m[3][0]; m[3][1] -= mat.m[3][1]; m[3][2] -= mat.m[3][2]; m[3][3] -= mat.m[3][3];
    return *this;
}

inline D3DXFROMWINEMATRIX& D3DXFROMWINEMATRIX::operator *= (FLOAT f)
{
    m[0][0] *= f; m[0][1] *= f; m[0][2] *= f; m[0][3] *= f;
    m[1][0] *= f; m[1][1] *= f; m[1][2] *= f; m[1][3] *= f;
    m[2][0] *= f; m[2][1] *= f; m[2][2] *= f; m[2][3] *= f;
    m[3][0] *= f; m[3][1] *= f; m[3][2] *= f; m[3][3] *= f;
    return *this;
}

inline D3DXFROMWINEMATRIX& D3DXFROMWINEMATRIX::operator /= (FLOAT f)
{
    FLOAT inv = 1.0f / f;
    m[0][0] *= inv; m[0][1] *= inv; m[0][2] *= inv; m[0][3] *= inv;
    m[1][0] *= inv; m[1][1] *= inv; m[1][2] *= inv; m[1][3] *= inv;
    m[2][0] *= inv; m[2][1] *= inv; m[2][2] *= inv; m[2][3] *= inv;
    m[3][0] *= inv; m[3][1] *= inv; m[3][2] *= inv; m[3][3] *= inv;
    return *this;
}

inline D3DXFROMWINEMATRIX D3DXFROMWINEMATRIX::operator + () const
{
    return *this;
}

inline D3DXFROMWINEMATRIX D3DXFROMWINEMATRIX::operator - () const
{
    return D3DXFROMWINEMATRIX(-m[0][0], -m[0][1], -m[0][2], -m[0][3],
                      -m[1][0], -m[1][1], -m[1][2], -m[1][3],
                      -m[2][0], -m[2][1], -m[2][2], -m[2][3],
                      -m[3][0], -m[3][1], -m[3][2], -m[3][3]);
}

inline D3DXFROMWINEMATRIX D3DXFROMWINEMATRIX::operator * (CONST D3DXFROMWINEMATRIX& mat) const
{
    D3DXFROMWINEMATRIX buf;
    D3DXFROMWINEMatrixMultiply(&buf, this, &mat);
    return buf;
}

inline D3DXFROMWINEMATRIX D3DXFROMWINEMATRIX::operator + (CONST D3DXFROMWINEMATRIX& mat) const
{
    return D3DXFROMWINEMATRIX(m[0][0] + mat.m[0][0], m[0][1] + mat.m[0][1], m[0][2] + mat.m[0][2], m[0][3] + mat.m[0][3],
                      m[1][0] + mat.m[1][0], m[1][1] + mat.m[1][1], m[1][2] + mat.m[1][2], m[1][3] + mat.m[1][3],
                      m[2][0] + mat.m[2][0], m[2][1] + mat.m[2][1], m[2][2] + mat.m[2][2], m[2][3] + mat.m[2][3],
                      m[3][0] + mat.m[3][0], m[3][1] + mat.m[3][1], m[3][2] + mat.m[3][2], m[3][3] + mat.m[3][3]);
}

inline D3DXFROMWINEMATRIX D3DXFROMWINEMATRIX::operator - (CONST D3DXFROMWINEMATRIX& mat) const
{
    return D3DXFROMWINEMATRIX(m[0][0] - mat.m[0][0], m[0][1] - mat.m[0][1], m[0][2] - mat.m[0][2], m[0][3] - mat.m[0][3],
                      m[1][0] - mat.m[1][0], m[1][1] - mat.m[1][1], m[1][2] - mat.m[1][2], m[1][3] - mat.m[1][3],
                      m[2][0] - mat.m[2][0], m[2][1] - mat.m[2][1], m[2][2] - mat.m[2][2], m[2][3] - mat.m[2][3],
                      m[3][0] - mat.m[3][0], m[3][1] - mat.m[3][1], m[3][2] - mat.m[3][2], m[3][3] - mat.m[3][3]);
}

inline D3DXFROMWINEMATRIX D3DXFROMWINEMATRIX::operator * (FLOAT f) const
{
    return D3DXFROMWINEMATRIX(m[0][0] * f, m[0][1] * f, m[0][2] * f, m[0][3] * f,
                      m[1][0] * f, m[1][1] * f, m[1][2] * f, m[1][3] * f,
                      m[2][0] * f, m[2][1] * f, m[2][2] * f, m[2][3] * f,
                      m[3][0] * f, m[3][1] * f, m[3][2] * f, m[3][3] * f);
}

inline D3DXFROMWINEMATRIX D3DXFROMWINEMATRIX::operator / (FLOAT f) const
{
    FLOAT inv = 1.0f / f;
    return D3DXFROMWINEMATRIX(m[0][0] * inv, m[0][1] * inv, m[0][2] * inv, m[0][3] * inv,
                      m[1][0] * inv, m[1][1] * inv, m[1][2] * inv, m[1][3] * inv,
                      m[2][0] * inv, m[2][1] * inv, m[2][2] * inv, m[2][3] * inv,
                      m[3][0] * inv, m[3][1] * inv, m[3][2] * inv, m[3][3] * inv);
}
#if !defined(SWIG)
inline D3DXFROMWINEMATRIX operator * (FLOAT f, CONST D3DXFROMWINEMATRIX& mat)
{
    return D3DXFROMWINEMATRIX(f * mat.m[0][0], f * mat.m[0][1], f * mat.m[0][2], f * mat.m[0][3],
                      f * mat.m[1][0], f * mat.m[1][1], f * mat.m[1][2], f * mat.m[1][3],
                      f * mat.m[2][0], f * mat.m[2][1], f * mat.m[2][2], f * mat.m[2][3],
                      f * mat.m[3][0], f * mat.m[3][1], f * mat.m[3][2], f * mat.m[3][3]);
}
#endif
inline BOOL D3DXFROMWINEMATRIX::operator == (CONST D3DXFROMWINEMATRIX& mat) const
{
#if 0
    return (stx_memcmp(this, &mat, sizeof(D3DXFROMWINEMATRIX)) == 0);
#else
    for (unsigned int i=0; i<4; i++)
    {
     for (unsigned int j=0; j<4; j++)
     {
      if (mat.m[i][j] != m[i][j] ) return FALSE;
     }
    }
	return TRUE;
#endif
}

inline BOOL D3DXFROMWINEMATRIX::operator != (CONST D3DXFROMWINEMATRIX& mat) const
{
#if 0
    return (stx_memcmp(this, &mat, sizeof(D3DXFROMWINEMATRIX)) != 0);
#else
    for (unsigned int i=0; i<4; i++)
    {
     for (unsigned int j=0; j<4; j++)
     {
      if (mat.m[i][j] != m[i][j] ) return TRUE;
     }
    }
	return FALSE;
#endif
}

inline D3DXFROMWINEQUATERNION::D3DXFROMWINEQUATERNION()
{
}

inline D3DXFROMWINEQUATERNION::D3DXFROMWINEQUATERNION(CONST FLOAT *pf)
{
    if(!pf) return;
    x = pf[0];
    y = pf[1];
    z = pf[2];
    w = pf[3];
}

inline D3DXFROMWINEQUATERNION::D3DXFROMWINEQUATERNION(FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw)
{
    x = fx;
    y = fy;
    z = fz;
    w = fw;
}

inline D3DXFROMWINEQUATERNION::operator FLOAT* ()
{
    return (FLOAT*)&x;
}

inline D3DXFROMWINEQUATERNION::operator CONST FLOAT* () const
{
    return (CONST FLOAT*)&x;
}

inline D3DXFROMWINEQUATERNION& D3DXFROMWINEQUATERNION::operator += (CONST D3DXFROMWINEQUATERNION& quat)
{
    x += quat.x;
    y += quat.y;
    z += quat.z;
    w += quat.w;
    return *this;
}

inline D3DXFROMWINEQUATERNION& D3DXFROMWINEQUATERNION::operator -= (CONST D3DXFROMWINEQUATERNION& quat)
{
    x -= quat.x;
    y -= quat.y;
    z -= quat.z;
    w -= quat.w;
    return *this;
}

/* TODO: uncomment this when D3DXFROMWINEQuaternionMultiply has been implemented
inline D3DXFROMWINEQUATERNION& D3DXFROMWINEQUATERNION::operator *= (CONST D3DXFROMWINEQUATERNION& quat)
{
    D3DXFROMWINEQuaternionMultiply(this, this, &quat);
    return *this;
}
*/

inline D3DXFROMWINEQUATERNION& D3DXFROMWINEQUATERNION::operator *= (FLOAT f)
{
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return *this;
}

inline D3DXFROMWINEQUATERNION& D3DXFROMWINEQUATERNION::operator /= (FLOAT f)
{
    FLOAT inv = 1.0f / f;
    x *= inv;
    y *= inv;
    z *= inv;
    w *= inv;
    return *this;
}

inline D3DXFROMWINEQUATERNION D3DXFROMWINEQUATERNION::operator + () const
{
    return *this;
}

inline D3DXFROMWINEQUATERNION D3DXFROMWINEQUATERNION::operator - () const
{
    return D3DXFROMWINEQUATERNION(-x, -y, -z, -w);
}

inline D3DXFROMWINEQUATERNION D3DXFROMWINEQUATERNION::operator + (CONST D3DXFROMWINEQUATERNION& quat) const
{
    return D3DXFROMWINEQUATERNION(x + quat.x, y + quat.y, z + quat.z, w + quat.w);
}

inline D3DXFROMWINEQUATERNION D3DXFROMWINEQUATERNION::operator - (CONST D3DXFROMWINEQUATERNION& quat) const
{
    return D3DXFROMWINEQUATERNION(x - quat.x, y - quat.y, z - quat.z, w - quat.w);
}

/* TODO: uncomment this when D3DXFROMWINEQuaternionMultiply has been implemented
inline D3DXFROMWINEQUATERNION D3DXFROMWINEQUATERNION::operator * (CONST D3DXFROMWINEQUATERNION& quat) const
{
    D3DXFROMWINEQUATERNION buf;
    D3DXFROMWINEQuaternionMultiply(&buf, this, &quat);
    return buf;
}
*/

inline D3DXFROMWINEQUATERNION D3DXFROMWINEQUATERNION::operator * (FLOAT f) const
{
    return D3DXFROMWINEQUATERNION(x * f, y * f, z * f, w * f);
}

inline D3DXFROMWINEQUATERNION D3DXFROMWINEQUATERNION::operator / (FLOAT f) const
{
    FLOAT inv = 1.0f / f;
    return D3DXFROMWINEQUATERNION(x * inv, y * inv, z * inv, w * inv);
}
#if !defined(SWIG)
inline D3DXFROMWINEQUATERNION operator * (FLOAT f, CONST D3DXFROMWINEQUATERNION& quat)
{
    return D3DXFROMWINEQUATERNION(f * quat.x, f * quat.y, f * quat.z, f * quat.w);
}
#endif
inline BOOL D3DXFROMWINEQUATERNION::operator == (CONST D3DXFROMWINEQUATERNION& quat) const
{
    return x == quat.x && y == quat.y && z == quat.z && w == quat.w;
}

inline BOOL D3DXFROMWINEQUATERNION::operator != (CONST D3DXFROMWINEQUATERNION& quat) const
{
    return x != quat.x || y != quat.y || z != quat.z || w != quat.w;
}

inline D3DXFROMWINEPLANE::D3DXFROMWINEPLANE()
{
}

inline D3DXFROMWINEPLANE::D3DXFROMWINEPLANE(CONST FLOAT *pf)
{
    if(!pf) return;
    a = pf[0];
    b = pf[1];
    c = pf[2];
    d = pf[3];
}

inline D3DXFROMWINEPLANE::D3DXFROMWINEPLANE(FLOAT fa, FLOAT fb, FLOAT fc, FLOAT fd)
{
    a = fa;
    b = fb;
    c = fc;
    d = fd;
}

inline D3DXFROMWINEPLANE::operator FLOAT* ()
{
    return (FLOAT*)&a;
}

inline D3DXFROMWINEPLANE::operator CONST FLOAT* () const
{
    return (CONST FLOAT*)&a;
}

inline D3DXFROMWINEPLANE D3DXFROMWINEPLANE::operator + () const
{
    return *this;
}

inline D3DXFROMWINEPLANE D3DXFROMWINEPLANE::operator - () const
{
    return D3DXFROMWINEPLANE(-a, -b, -c, -d);
}

inline BOOL D3DXFROMWINEPLANE::operator == (CONST D3DXFROMWINEPLANE& pl) const
{
    return a == pl.a && b == pl.b && c == pl.c && d == pl.d;
}

inline BOOL D3DXFROMWINEPLANE::operator != (CONST D3DXFROMWINEPLANE& pl) const
{
    return a != pl.a || b != pl.b || c != pl.c || d != pl.d;
}

inline D3DXFROMWINECOLOR::D3DXFROMWINECOLOR()
{
}

inline D3DXFROMWINECOLOR::D3DXFROMWINECOLOR(__DWORD__ col)
{
    CONST FLOAT f = 1.0f / 255.0f;
    r = f * (FLOAT)(unsigned char)(col >> 16);
    g = f * (FLOAT)(unsigned char)(col >>  8);
    b = f * (FLOAT)(unsigned char)col;
    a = f * (FLOAT)(unsigned char)(col >> 24);
}

inline D3DXFROMWINECOLOR::D3DXFROMWINECOLOR(CONST FLOAT *pf)
{
    if(!pf) return;
    r = pf[0];
    g = pf[1];
    b = pf[2];
    a = pf[3];
}

inline D3DXFROMWINECOLOR::D3DXFROMWINECOLOR(CONST D3DFROMWINECOLORVALUE& col)
{
    r = col.r;
    g = col.g;
    b = col.b;
    a = col.a;
}

inline D3DXFROMWINECOLOR::D3DXFROMWINECOLOR(FLOAT fr, FLOAT fg, FLOAT fb, FLOAT fa)
{
    r = fr;
    g = fg;
    b = fb;
    a = fa;
}

inline D3DXFROMWINECOLOR::operator __DWORD__ () const
{
    __DWORD__ _r = r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : (__DWORD__)(r * 255.0f + 0.5f);
    __DWORD__ _g = g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : (__DWORD__)(g * 255.0f + 0.5f);
    __DWORD__ _b = b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : (__DWORD__)(b * 255.0f + 0.5f);
    __DWORD__ _a = a >= 1.0f ? 0xff : a <= 0.0f ? 0x00 : (__DWORD__)(a * 255.0f + 0.5f);

    return (_a << 24) | (_r << 16) | (_g << 8) | _b;
}

inline D3DXFROMWINECOLOR::operator FLOAT * ()
{
    return (FLOAT*)&r;
}

inline D3DXFROMWINECOLOR::operator CONST FLOAT * () const
{
    return (CONST FLOAT*)&r;
}

inline D3DXFROMWINECOLOR::operator D3DFROMWINECOLORVALUE * ()
{
    return (D3DFROMWINECOLORVALUE*)&r;
}

inline D3DXFROMWINECOLOR::operator CONST D3DFROMWINECOLORVALUE * () const
{
    return (CONST D3DFROMWINECOLORVALUE*)&r;
}

inline D3DXFROMWINECOLOR::operator D3DFROMWINECOLORVALUE& ()
{
    return *((D3DFROMWINECOLORVALUE*)&r);
}

inline D3DXFROMWINECOLOR::operator CONST D3DFROMWINECOLORVALUE& () const
{
    return *((CONST D3DFROMWINECOLORVALUE*)&r);
}

inline D3DXFROMWINECOLOR& D3DXFROMWINECOLOR::operator += (CONST D3DXFROMWINECOLOR& col)
{
    r += col.r;
    g += col.g;
    b += col.b;
    a += col.a;
    return *this;
}

inline D3DXFROMWINECOLOR& D3DXFROMWINECOLOR::operator -= (CONST D3DXFROMWINECOLOR& col)
{
    r -= col.r;
    g -= col.g;
    b -= col.b;
    a -= col.a;
    return *this;
}

inline D3DXFROMWINECOLOR& D3DXFROMWINECOLOR::operator *= (FLOAT f)
{
    r *= f;
    g *= f;
    b *= f;
    a *= f;
    return *this;
}

inline D3DXFROMWINECOLOR& D3DXFROMWINECOLOR::operator /= (FLOAT f)
{
    FLOAT inv = 1.0f / f;
    r *= inv;
    g *= inv;
    b *= inv;
    a *= inv;
    return *this;
}

inline D3DXFROMWINECOLOR D3DXFROMWINECOLOR::operator + () const
{
    return *this;
}

inline D3DXFROMWINECOLOR D3DXFROMWINECOLOR::operator - () const
{
    return D3DXFROMWINECOLOR(-r, -g, -b, -a);
}

inline D3DXFROMWINECOLOR D3DXFROMWINECOLOR::operator + (CONST D3DXFROMWINECOLOR& col) const
{
    return D3DXFROMWINECOLOR(r + col.r, g + col.g, b + col.b, a + col.a);
}

inline D3DXFROMWINECOLOR D3DXFROMWINECOLOR::operator - (CONST D3DXFROMWINECOLOR& col) const
{
    return D3DXFROMWINECOLOR(r - col.r, g - col.g, b - col.b, a - col.a);
}

inline D3DXFROMWINECOLOR D3DXFROMWINECOLOR::operator * (FLOAT f) const
{
    return D3DXFROMWINECOLOR(r * f, g * f, b * f, a * f);
}

inline D3DXFROMWINECOLOR D3DXFROMWINECOLOR::operator / (FLOAT f) const
{
    FLOAT inv = 1.0f / f;
    return D3DXFROMWINECOLOR(r * inv, g * inv, b * inv, a * inv);
}
#if !defined(SWIG)
inline D3DXFROMWINECOLOR operator * (FLOAT f, CONST D3DXFROMWINECOLOR& col)
{
    return D3DXFROMWINECOLOR(f * col.r, f * col.g, f * col.b, f * col.a);
}
#endif
inline BOOL D3DXFROMWINECOLOR::operator == (CONST D3DXFROMWINECOLOR& col) const
{
    return r == col.r && g == col.g && b == col.b && a == col.a;
}

inline BOOL D3DXFROMWINECOLOR::operator != (CONST D3DXFROMWINECOLOR& col) const
{
    return r != col.r || g != col.g || b != col.b || a != col.a;
}

#endif /* __cplusplus */

/*_______________D3DXFROMWINECOLOR_____________________*/

static inline D3DXFROMWINECOLOR* D3DXFROMWINEColorAdd(D3DXFROMWINECOLOR *pout, CONST D3DXFROMWINECOLOR *pc1, CONST D3DXFROMWINECOLOR *pc2)
{
    if ( !pout || !pc1 || !pc2 ) return 0;
    pout->r = (pc1->r) + (pc2->r);
    pout->g = (pc1->g) + (pc2->g);
    pout->b = (pc1->b) + (pc2->b);
    pout->a = (pc1->a) + (pc2->a);
    return pout;
}

static inline D3DXFROMWINECOLOR* D3DXFROMWINEColorLerp(D3DXFROMWINECOLOR *pout, CONST D3DXFROMWINECOLOR *pc1, CONST D3DXFROMWINECOLOR *pc2, FLOAT s)
{
    if ( !pout || !pc1 || !pc2 ) return 0;
    pout->r = (1-s) * (pc1->r) + s *(pc2->r);
    pout->g = (1-s) * (pc1->g) + s *(pc2->g);
    pout->b = (1-s) * (pc1->b) + s *(pc2->b);
    pout->a = (1-s) * (pc1->a) + s *(pc2->a);
    return pout;
}

static inline D3DXFROMWINECOLOR* D3DXFROMWINEColorModulate(D3DXFROMWINECOLOR *pout, CONST D3DXFROMWINECOLOR *pc1, CONST D3DXFROMWINECOLOR *pc2)
{
    if ( !pout || !pc1 || !pc2 ) return 0;
    pout->r = (pc1->r) * (pc2->r);
    pout->g = (pc1->g) * (pc2->g);
    pout->b = (pc1->b) * (pc2->b);
    pout->a = (pc1->a) * (pc2->a);
    return pout;
}

static inline D3DXFROMWINECOLOR* D3DXFROMWINEColorNegative(D3DXFROMWINECOLOR *pout, CONST D3DXFROMWINECOLOR *pc)
{
    if ( !pout || !pc ) return 0;
    pout->r = 1.0f - pc->r;
    pout->g = 1.0f - pc->g;
    pout->b = 1.0f - pc->b;
    pout->a = pc->a;
    return pout;
}

static inline D3DXFROMWINECOLOR* D3DXFROMWINEColorScale(D3DXFROMWINECOLOR *pout, CONST D3DXFROMWINECOLOR *pc, FLOAT s)
{
    if ( !pout || !pc ) return 0;
    pout->r = s* (pc->r);
    pout->g = s* (pc->g);
    pout->b = s* (pc->b);
    pout->a = s* (pc->a);
    return pout;
}

static inline D3DXFROMWINECOLOR* D3DXFROMWINEColorSubtract(D3DXFROMWINECOLOR *pout, CONST D3DXFROMWINECOLOR *pc1, CONST D3DXFROMWINECOLOR *pc2)
{
    if ( !pout || !pc1 || !pc2 ) return 0;
    pout->r = (pc1->r) - (pc2->r);
    pout->g = (pc1->g) - (pc2->g);
    pout->b = (pc1->b) - (pc2->b);
    pout->a = (pc1->a) - (pc2->a);
    return pout;
}

/*_______________D3DXFROMWINEVECTOR2________________________*/

static inline D3DXFROMWINEVECTOR2* D3DXFROMWINEVec2Add(D3DXFROMWINEVECTOR2 *pout, CONST D3DXFROMWINEVECTOR2 *pv1, CONST D3DXFROMWINEVECTOR2 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = pv1->x + pv2->x;
    pout->y = pv1->y + pv2->y;
    return pout;
}

static inline FLOAT D3DXFROMWINEVec2CCW(CONST D3DXFROMWINEVECTOR2 *pv1, CONST D3DXFROMWINEVECTOR2 *pv2)
{
    if ( !pv1 || !pv2) return 0.0f;
    return ( (pv1->x) * (pv2->y) - (pv1->y) * (pv2->x) );
}

static inline FLOAT D3DXFROMWINEVec2Dot(CONST D3DXFROMWINEVECTOR2 *pv1, CONST D3DXFROMWINEVECTOR2 *pv2)
{
    if ( !pv1 || !pv2) return 0.0f;
    return ( (pv1->x * pv2->x + pv1->y * pv2->y) );
}

static inline FLOAT D3DXFROMWINEVec2Length(CONST D3DXFROMWINEVECTOR2 *pv)
{
    if (!pv) return 0.0f;
    return sqrt( (pv->x) * (pv->x) + (pv->y) * (pv->y) );
}

static inline FLOAT D3DXFROMWINEVec2LengthSq(CONST D3DXFROMWINEVECTOR2 *pv)
{
    if (!pv) return 0.0f;
    return( (pv->x) * (pv->x) + (pv->y) * (pv->y) );
}

static inline D3DXFROMWINEVECTOR2* D3DXFROMWINEVec2Lerp(D3DXFROMWINEVECTOR2 *pout, CONST D3DXFROMWINEVECTOR2 *pv1, CONST D3DXFROMWINEVECTOR2 *pv2, FLOAT s)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = (1-s) * (pv1->x) + s * (pv2->x);
    pout->y = (1-s) * (pv1->y) + s * (pv2->y);
    return pout;
}

static inline D3DXFROMWINEVECTOR2* D3DXFROMWINEVec2Maximize(D3DXFROMWINEVECTOR2 *pout, CONST D3DXFROMWINEVECTOR2 *pv1, CONST D3DXFROMWINEVECTOR2 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = stx_max(pv1->x , pv2->x);
    pout->y = stx_max(pv1->y , pv2->y);
    return pout;
}

static inline D3DXFROMWINEVECTOR2* D3DXFROMWINEVec2Minimize(D3DXFROMWINEVECTOR2 *pout, CONST D3DXFROMWINEVECTOR2 *pv1, CONST D3DXFROMWINEVECTOR2 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = stx_min(pv1->x , pv2->x);
    pout->y = stx_min(pv1->y , pv2->y);
    return pout;
}

static inline D3DXFROMWINEVECTOR2* D3DXFROMWINEVec2Scale(D3DXFROMWINEVECTOR2 *pout, CONST D3DXFROMWINEVECTOR2 *pv, FLOAT s)
{
    if ( !pout || !pv) return 0;
    pout->x = s * (pv->x);
    pout->y = s * (pv->y);
    return pout;
}

static inline D3DXFROMWINEVECTOR2* D3DXFROMWINEVec2Subtract(D3DXFROMWINEVECTOR2 *pout, CONST D3DXFROMWINEVECTOR2 *pv1, CONST D3DXFROMWINEVECTOR2 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = pv1->x - pv2->x;
    pout->y = pv1->y - pv2->y;
    return pout;
}

/*__________________D3DXFROMWINEVECTOR3_______________________*/

static inline D3DXFROMWINEVECTOR3* D3DXFROMWINEVec3Add(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv1, CONST D3DXFROMWINEVECTOR3 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = pv1->x + pv2->x;
    pout->y = pv1->y + pv2->y;
    pout->z = pv1->z + pv2->z;
    return pout;
}

static inline D3DXFROMWINEVECTOR3* D3DXFROMWINEVec3Cross(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv1, CONST D3DXFROMWINEVECTOR3 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = (pv1->y) * (pv2->z) - (pv1->z) * (pv2->y);
    pout->y = (pv1->z) * (pv2->x) - (pv1->x) * (pv2->z);
    pout->z = (pv1->x) * (pv2->y) - (pv1->y) * (pv2->x);
    return pout;
}

static inline FLOAT D3DXFROMWINEVec3Dot(CONST D3DXFROMWINEVECTOR3 *pv1, CONST D3DXFROMWINEVECTOR3 *pv2)
{
    if ( !pv1 || !pv2 ) return 0.0f;
    return (pv1->x) * (pv2->x) + (pv1->y) * (pv2->y) + (pv1->z) * (pv2->z);
}

static inline FLOAT D3DXFROMWINEVec3Length(CONST D3DXFROMWINEVECTOR3 *pv)
{
    if (!pv) return 0.0f;
    return sqrt( (pv->x) * (pv->x) + (pv->y) * (pv->y) + (pv->z) * (pv->z) );
}

static inline FLOAT D3DXFROMWINEVec3LengthSq(CONST D3DXFROMWINEVECTOR3 *pv)
{
    if (!pv) return 0.0f;
    return (pv->x) * (pv->x) + (pv->y) * (pv->y) + (pv->z) * (pv->z);
}

static inline D3DXFROMWINEVECTOR3* D3DXFROMWINEVec3Lerp(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv1, CONST D3DXFROMWINEVECTOR3 *pv2, FLOAT s)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = (1-s) * (pv1->x) + s * (pv2->x);
    pout->y = (1-s) * (pv1->y) + s * (pv2->y);
    pout->z = (1-s) * (pv1->z) + s * (pv2->z);
    return pout;
}

static inline D3DXFROMWINEVECTOR3* D3DXFROMWINEVec3Maximize(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv1, CONST D3DXFROMWINEVECTOR3 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = stx_max(pv1->x , pv2->x);
    pout->y = stx_max(pv1->y , pv2->y);
    pout->z = stx_max(pv1->z , pv2->z);
    return pout;
}

static inline D3DXFROMWINEVECTOR3* D3DXFROMWINEVec3Minimize(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv1, CONST D3DXFROMWINEVECTOR3 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = stx_min(pv1->x , pv2->x);
    pout->y = stx_min(pv1->y , pv2->y);
    pout->z = stx_min(pv1->z , pv2->z);
    return pout;
}

static inline D3DXFROMWINEVECTOR3* D3DXFROMWINEVec3Scale(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv, FLOAT s)
{
    if ( !pout || !pv) return 0;
    pout->x = s * (pv->x);
    pout->y = s * (pv->y);
    pout->z = s * (pv->z);
    return pout;
}

static inline D3DXFROMWINEVECTOR3* D3DXFROMWINEVec3Subtract(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv1, CONST D3DXFROMWINEVECTOR3 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = pv1->x - pv2->x;
    pout->y = pv1->y - pv2->y;
    pout->z = pv1->z - pv2->z;
    return pout;
}
/*__________________D3DXFROMWINEVECTOR4_______________________*/

static inline D3DXFROMWINEVECTOR4* D3DXFROMWINEVec4Add(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv1, CONST D3DXFROMWINEVECTOR4 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = pv1->x + pv2->x;
    pout->y = pv1->y + pv2->y;
    pout->z = pv1->z + pv2->z;
    pout->w = pv1->w + pv2->w;
    return pout;
}

static inline FLOAT D3DXFROMWINEVec4Dot(CONST D3DXFROMWINEVECTOR4 *pv1, CONST D3DXFROMWINEVECTOR4 *pv2)
{
    if (!pv1 || !pv2 ) return 0.0f;
    return (pv1->x) * (pv2->x) + (pv1->y) * (pv2->y) + (pv1->z) * (pv2->z) + (pv1->w) * (pv2->w);
}

static inline FLOAT D3DXFROMWINEVec4Length(CONST D3DXFROMWINEVECTOR4 *pv)
{
    if (!pv) return 0.0f;
    return sqrt( (pv->x) * (pv->x) + (pv->y) * (pv->y) + (pv->z) * (pv->z) + (pv->w) * (pv->w) );
}

static inline FLOAT D3DXFROMWINEVec4LengthSq(CONST D3DXFROMWINEVECTOR4 *pv)
{
    if (!pv) return 0.0f;
    return (pv->x) * (pv->x) + (pv->y) * (pv->y) + (pv->z) * (pv->z) + (pv->w) * (pv->w);
}

static inline D3DXFROMWINEVECTOR4* D3DXFROMWINEVec4Lerp(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv1, CONST D3DXFROMWINEVECTOR4 *pv2, FLOAT s)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = (1-s) * (pv1->x) + s * (pv2->x);
    pout->y = (1-s) * (pv1->y) + s * (pv2->y);
    pout->z = (1-s) * (pv1->z) + s * (pv2->z);
    pout->w = (1-s) * (pv1->w) + s * (pv2->w);
    return pout;
}


static inline D3DXFROMWINEVECTOR4* D3DXFROMWINEVec4Maximize(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv1, CONST D3DXFROMWINEVECTOR4 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = stx_max(pv1->x , pv2->x);
    pout->y = stx_max(pv1->y , pv2->y);
    pout->z = stx_max(pv1->z , pv2->z);
    pout->w = stx_max(pv1->w , pv2->w);
    return pout;
}

static inline D3DXFROMWINEVECTOR4* D3DXFROMWINEVec4Minimize(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv1, CONST D3DXFROMWINEVECTOR4 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = stx_min(pv1->x , pv2->x);
    pout->y = stx_min(pv1->y , pv2->y);
    pout->z = stx_min(pv1->z , pv2->z);
    pout->w = stx_min(pv1->w , pv2->w);
    return pout;
}

static inline D3DXFROMWINEVECTOR4* D3DXFROMWINEVec4Scale(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv, FLOAT s)
{
    if ( !pout || !pv) return 0;
    pout->x = s * (pv->x);
    pout->y = s * (pv->y);
    pout->z = s * (pv->z);
    pout->w = s * (pv->w);
    return pout;
}

static inline D3DXFROMWINEVECTOR4* D3DXFROMWINEVec4Subtract(D3DXFROMWINEVECTOR4 *pout, CONST D3DXFROMWINEVECTOR4 *pv1, CONST D3DXFROMWINEVECTOR4 *pv2)
{
    if ( !pout || !pv1 || !pv2) return 0;
    pout->x = pv1->x - pv2->x;
    pout->y = pv1->y - pv2->y;
    pout->z = pv1->z - pv2->z;
    pout->w = pv1->w - pv2->w;
    return pout;
}

/*__________________D3DXFROMWINEMatrix____________________*/
#ifdef NONAMELESSUNION
# define D3DXFROMWINE_U(x)  (x).u
#else
# define D3DXFROMWINE_U(x)  (x)
#endif

static inline D3DXFROMWINEMATRIX* D3DXFROMWINEMatrixIdentity(D3DXFROMWINEMATRIX *pout)
{
    if ( !pout ) return 0;
    /*D3DXFROMWINE_U(*/(*pout).m[0][1] = 0.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[0][2] = 0.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[0][3] = 0.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[1][0] = 0.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[1][2] = 0.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[1][3] = 0.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[2][0] = 0.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[2][1] = 0.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[2][3] = 0.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[3][0] = 0.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[3][1] = 0.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[3][2] = 0.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[0][0] = 1.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[1][1] = 1.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[2][2] = 1.0f;
    /*D3DXFROMWINE_U(*/(*pout).m[3][3] = 1.0f;
    return pout;
}
static inline BOOL D3DXFROMWINEMatrixIsIdentity(D3DXFROMWINEMATRIX *pm)
{
    int i,j;
    D3DXFROMWINEMATRIX testmatrix;

    if ( !pm ) return FALSE;
    D3DXFROMWINEMatrixIdentity(&testmatrix);
    for (i=0; i<4; i++)
    {
     for (j=0; j<4; j++)
     {
      if (/* D3DXFROMWINE_U*/(*pm).m[i][j] != /* D3DXFROMWINE_U*/(testmatrix).m[i][j] ) return FALSE;
     }
    }
    return TRUE;
}
#undef D3DXFROMWINE_U

/*__________________D3DXFROMWINEPLANE____________________*/

static inline FLOAT D3DXFROMWINEPlaneDot(CONST D3DXFROMWINEPLANE *pp, CONST D3DXFROMWINEVECTOR4 *pv)
{
    if ( !pp || !pv ) return 0.0f;
    return ( (pp->a) * (pv->x) + (pp->b) * (pv->y) + (pp->c) * (pv->z) + (pp->d) * (pv->w) );
}

static inline FLOAT D3DXFROMWINEPlaneDotCoord(CONST D3DXFROMWINEPLANE *pp, CONST D3DXFROMWINEVECTOR4 *pv)
{
    if ( !pp || !pv ) return 0.0f;
    return ( (pp->a) * (pv->x) + (pp->b) * (pv->y) + (pp->c) * (pv->z) + (pp->d) );
}

static inline FLOAT D3DXFROMWINEPlaneDotNormal(CONST D3DXFROMWINEPLANE *pp, CONST D3DXFROMWINEVECTOR4 *pv)
{
    if ( !pp || !pv ) return 0.0f;
    return ( (pp->a) * (pv->x) + (pp->b) * (pv->y) + (pp->c) * (pv->z) );
}

/*__________________D3DXFROMWINEQUATERNION____________________*/

static inline D3DXFROMWINEQUATERNION* D3DXFROMWINEQuaternionConjugate(D3DXFROMWINEQUATERNION *pout, CONST D3DXFROMWINEQUATERNION *pq)
{
    if ( !pout || !pq) return 0;
    pout->x = -pq->x;
    pout->y = -pq->y;
    pout->z = -pq->z;
    pout->w = pq->w;
    return pout;
}

static inline FLOAT D3DXFROMWINEQuaternionDot(CONST D3DXFROMWINEQUATERNION *pq1, CONST D3DXFROMWINEQUATERNION *pq2)
{
    if ( !pq1 || !pq2 ) return 0.0f;
    return (pq1->x) * (pq2->x) + (pq1->y) * (pq2->y) + (pq1->z) * (pq2->z) + (pq1->w) * (pq2->w);
}

static inline D3DXFROMWINEQUATERNION* D3DXFROMWINEQuaternionIdentity(D3DXFROMWINEQUATERNION *pout)
{
    if ( !pout) return 0;
    pout->x = 0.0f;
    pout->y = 0.0f;
    pout->z = 0.0f;
    pout->w = 1.0f;
    return pout;
}

static inline BOOL D3DXFROMWINEQuaternionIsIdentity(D3DXFROMWINEQUATERNION *pq)
{
    if ( !pq) return FALSE;
    return ( (pq->x == 0.0f) && (pq->y == 0.0f) && (pq->z == 0.0f) && (pq->w == 1.0f) );
}

static inline FLOAT D3DXFROMWINEQuaternionLength(CONST D3DXFROMWINEQUATERNION *pq)
{
    if (!pq) return 0.0f;
    return sqrt( (pq->x) * (pq->x) + (pq->y) * (pq->y) + (pq->z) * (pq->z) + (pq->w) * (pq->w) );
}

static inline FLOAT D3DXFROMWINEQuaternionLengthSq(CONST D3DXFROMWINEQUATERNION *pq)
{
    if (!pq) return 0.0f;
    return (pq->x) * (pq->x) + (pq->y) * (pq->y) + (pq->z) * (pq->z) + (pq->w) * (pq->w);
}

static inline VOID D3DXFROMWINEDumpMatrix(D3DXFROMWINEMATRIX *pout)
{
    int i,j;
    for (i=0; i<4; i++)
    {
        for (j=0; j<4; j++)
        {
		LOG_PRINT("D3DXFROMWINEDumpMatrix[%d][%d]=%f\n",i,j,pout->m[i][j]);
	}
    }
}

static inline VOID D3DXFROMWINEDumpVec4(D3DXFROMWINEVECTOR4 *pout)
{
        LOG_PRINT("D3DXFROMWINEDumpVec4:%f:%f:%f:%f\n",pout->x,pout->y,pout->z,pout->w);
}

static inline VOID D3DXFROMWINEDumpVec3(D3DXFROMWINEVECTOR3 *pout)
{
        LOG_PRINT("D3DXFROMWINEDumpVec3:%f:%f:%f\n",pout->x,pout->y,pout->z);
}

static inline VOID D3DXFROMWINEDumpVec2(D3DXFROMWINEVECTOR2 *pout)
{
        LOG_PRINT("D3DXFROMWINEDumpVec2:%f:%f\n",pout->x,pout->y);
}

static inline D3DXFROMWINEMATRIX perspectiveMatrixX(FLOAT fovy, FLOAT width, FLOAT height, FLOAT zn, FLOAT zf, bool aB=false)
{
    D3DXFROMWINEMATRIX out;
    FLOAT aspect=1.0f*width/height;
    D3DXFROMWINEMatrixPerspectiveFovLH(&out, fovy, aspect, zn, zf);
    return out;
}
static inline D3DXFROMWINEMATRIX rotateXY4(FLOAT wx, FLOAT wy)
{
    D3DXFROMWINEMATRIX out;
    D3DXFROMWINEMatrixRotationYawPitchRoll(&out, wx, wy, 0.0f);
    return out;
}
static inline FLOAT length(CONST D3DXFROMWINEVECTOR3& pv)
{
    return sqrt( (pv.x) * (pv.x) + (pv.y) * (pv.y) + (pv.z) * (pv.z) );
}
#define lerp(t, a, b) (a + t * (b - a))

static inline D3DXFROMWINEMATRIX transpose(CONST D3DXFROMWINEMATRIX& aM)
{
	D3DXFROMWINEMATRIX out;
	D3DXFROMWINEMatrixTranspose(&out, &aM);
	return out;
}

//define distance(u, v) length((u) - (v))
static inline FLOAT distance(CONST D3DXFROMWINEVECTOR3& u, CONST D3DXFROMWINEVECTOR3& v)
{
	D3DXFROMWINEVECTOR3 d=u-v;
	return D3DXFROMWINEVec3Length(&d);
}

static inline D3DXFROMWINEMATRIX pegToFarPlane(const D3DXFROMWINEMATRIX &m){
	D3DXFROMWINEMATRIX mat;

	stx_memcpy((void*)&mat.m2[0], (void*)&m.m2[0], 8 * sizeof(float));
	stx_memcpy((void*)&mat.m2[2], (void*)&m.m2[3], 4 * sizeof(float));
	stx_memcpy((void*)&mat.m2[3], (void*)&m.m2[3], 4 * sizeof(float));

	return mat;
}

static inline D3DXFROMWINEMATRIX orthoMatrix(const float left, const float right, const float top, const float bottom, const float zNear, const float zFar, const bool d3dStyle)
{
	float rl = right - left;
	float tb = top - bottom;
	float fn = zFar - zNear;
	D3DXFROMWINEMATRIX mat(
		2.0f / rl, 0,         0,         -(right + left) / rl,
		0,         2.0f / tb, 0,         -(top + bottom) / tb,
		0,         0,        -2.0f / fn, -(zFar + zNear) / fn,
		0,         0,         0,         1);

	if (d3dStyle){
		mat.m[2][2] = 0.5f * (mat.m[2][2] + mat.m[3][2]);
		mat.m[2][3] = 0.5f * (mat.m[2][3] + mat.m[3][3]);
	}
	return mat;
}

static inline D3DXFROMWINEMATRIX perspectiveMatrixY(const float fov, const int width, const int height, const float zNear, const float zFar, const bool d3dStyle){
	float h = tanf(0.5f * fov);
	float w = (h * width) / height;

	D3DXFROMWINEMATRIX mat(
		1.0f / w, 0,        0, 0,
		0,        1.0f / h, 0, 0,
		0,        0,        (zFar + zNear) / (zFar - zNear), -(2 * zFar * zNear) / (zFar - zNear),
		0,        0,        1, 0);

	if (d3dStyle){
		mat.m[2][2] = 0.5f * (mat.m[2][2] + mat.m[3][2]);
		mat.m[2][3] = 0.5f * (mat.m[2][3] + mat.m[3][3]);
	}
	return mat;
}

#if 0
static inline D3DXFROMWINEMATRIX identity4(){
	return D3DXFROMWINEMATRIX(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
}
#endif
static inline D3DXFROMWINEMATRIX scale4(const float x, const float y, const float z){
    return D3DXFROMWINEMATRIX(x,0,0,0, 0,y,0,0, 0,0,z,0, 0,0,0,1);
}

static inline D3DXFROMWINEMATRIX mirrorMatrix(const D3DXFROMWINEVECTOR3 &planeNormal, const float planeOffset){
	return D3DXFROMWINEMATRIX(
		1 - 2 * planeNormal.x * planeNormal.x,
		  - 2 * planeNormal.x * planeNormal.y,
		  - 2 * planeNormal.x * planeNormal.z,
		  - 2 * planeNormal.x * planeOffset,
		
		  - 2 * planeNormal.y * planeNormal.x,
		1 - 2 * planeNormal.y * planeNormal.y,
		  - 2 * planeNormal.y * planeNormal.z,
		  - 2 * planeNormal.y * planeOffset,

		  - 2 * planeNormal.z * planeNormal.x,
		  - 2 * planeNormal.z * planeNormal.y,
		1 - 2 * planeNormal.z * planeNormal.z,
		  - 2 * planeNormal.z * planeOffset,

		0, 0, 0, 1);
}

/** Fit t to an S-Curve */
inline float sCurve(const float t){
	return t * t * (3 - 2 * t);
}

#if 0
typedef struct _LITVERTEX
{
    float x, y, z;
    D3DXFROMWINECOLOR color;
    float tu, tv;
#if 1//(defined __cplusplus) && (defined D3D_OVERLOADS)
    _LITVERTEX() { }
    _LITVERTEX(const D3DXFROMWINEVECTOR3& v, __D3DCOLOR__ _color, float _tu, float _tv)
    {
        x = v.x; y = v.y; z = v.z;
        color = _color; tu = _tu; tv = _tv;
    }
#endif
} LITVERTEX, *LPLITVERTEX;

typedef struct _LITVERTEX2
{
    float x, y, z;
    float tu, tv;
#if 1//(defined __cplusplus) && (defined D3D_OVERLOADS)
    _LITVERTEX2() { }
    _LITVERTEX2(const D3DXFROMWINEVECTOR3& v, float _tu, float _tv)
    {
        x = v.x; y = v.y; z = v.z; tu = _tu; tv = _tv;
    }
	_LITVERTEX2(LITVERTEX v)
    {
        x = v.x; y = v.y; z = v.z; tu = v.tu; tv = v.tv;
    }
#endif
} LITVERTEX2, *LPLITVERTEX2;

//#define D3DFVF_LVERTEX1 ZZZ
typedef struct _D3DVERTEX {
  union {
    float x;         /* Homogeneous coordinates */
    float dvX;
  };
  union {
    float y;
    float dvY;
  };
  union {
    float z;
    float dvZ;
  };
  union {
    float nx;        /* Normal */
    float dvNX;
  };
  union {
    float ny;
    float dvNY;
  };
  union {
    float nz;
    float dvNZ;
  };
  union {
    float tu;        /* Texture coordinates */
    float dvTU;
  };
  union {
    float tv;
    float dvTV;
  };
#if 1//(DIRECT3D_VERSION >= 0x0500)
#if 1//(defined __cplusplus) && (defined D3D_OVERLOADS)
  _D3DVERTEX() { }
  _D3DVERTEX(const /*D3DXFROMWINEVECTOR3*/D3DXFROMWINEVECTOR3& v, 
const /*D3DXFROMWINEVECTOR3*/D3DXFROMWINEVECTOR3& n, 
float _tu, float _tv)
    {
      x = v.x; y = v.y; z = v.z;
      nx = n.x; ny = n.y; nz = n.z;
      tu = _tu; tv = _tv;
    }
#endif
#endif /* DIRECT3D_VERSION >= 0x0500 */
  enum _FVF
    {
        FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1
    };
} D3DVERTEX, *LPD3DVERTEX;
#define D3DFVF_VERTEX D3DFVF_XYZ|D3DFVF_TEX1




typedef struct _D3DVERTEX2 {
  union {
    float x;         /* Homogeneous coordinates */
    float dvX;
  };
  union {
    float y;
    float dvY;
  };
  union {
    float z;
    float dvZ;
  };/*
  union {
    float nx;        // Normal 
    float dvNX;
  };
  union {
    float ny;
    float dvNY;
  };
  union {
    float nz;
    float dvNZ;
  };*/
  union {
    float tu;        /* Texture coordinates */
    float dvTU;
  };
  union {
    float tv;
    float dvTV;
  };
#if 1//(DIRECT3D_VERSION >= 0x0500)
#if 1//(defined __cplusplus) && (defined D3D_OVERLOADS)
  _D3DVERTEX2() { }
  _D3DVERTEX2(const /*D3DXFROMWINEVECTOR3*/D3DXFROMWINEVECTOR3& v, 
//const /*D3DXFROMWINEVECTOR3*/D3DXFROMWINEVECTOR3& n, 
float _tu, float _tv)
    {
      x = v.x; y = v.y; z = v.z;
//      nx = n.x; ny = n.y; nz = n.z;
      tu = _tu; tv = _tv;
    }
  _D3DVERTEX2(D3DVERTEX v)
{
      x = v.x; y = v.y; z = v.z;
      tu = v.tu; tv = v.tv;
}
#endif
#endif /* DIRECT3D_VERSION >= 0x0500 */
  enum _FVF
    {
        FVF = D3DFVF_XYZ|
//D3DFVF_NORMAL|
D3DFVF_TEX1
    };
} D3DVERTEX2, *LPD3DVERTEX2;
#endif
#endif

