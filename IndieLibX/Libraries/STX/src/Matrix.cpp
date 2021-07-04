/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#if 0
#include <assert.h>
#include <math.h>
#include <stdio.h>

#endif

#include <STX/Matrix.h>
#include <STX/STXCPP.h>
//include <Framework3/IRenderer.h>

static const float myPi = 3.14159265358979323846f;
D3DXFROMWINEMATRIX toGLProjection(const D3DXFROMWINEMATRIX &m){
	D3DXFROMWINEMATRIX mat;
#if 0
	mat.rows[0] = m.rows[0];
	mat.rows[1] = m.rows[1];
	mat.rows[2] = m.rows[2] * 2.0 - m.rows[3];
	mat.rows[3] = m.rows[3];
#else
for(unsigned int i=0;i<4;i++)
{
	mat.index(i,0)=m.index(i,0);
	mat.index(i,1)=m.index(i,1);
	mat.index(i,2)=m.index(i,2) * 2.0 - m.index(i,3);
	mat.index(i,3)=m.index(i,3);
}
#endif
	return mat;
}
D3DXFROMWINEMATRIX perspectiveMatrixX(const float fov, const int width, const int height, const float zNear, const float zFar){
	float w = cosf(0.5f * fov) / sinf(0.5f * fov);
	float h = (w * width) / height;
	return D3DXFROMWINEMATRIX(
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, (zFar + zNear) / (zFar - zNear), -(2 * zFar * zNear) / (zFar - zNear),
		0, 0, 1, 0);
}
void buildPerspectiveMatrixGL(float fieldOfView,
			   float aspectRatio,
			   float zNear, float zFar,
			   D3DXFROMWINEMATRIX& m)
{

  float radians = fieldOfView * myPi / 180.0f;
	m = toGLProjection(perspectiveMatrixX(radians, 
#if 0
IRenderer::GetRendererInstance()->GetViewportWidth(), 
IRenderer::GetRendererInstance()->GetViewportHeight(), 
#else
STX_Service::GetWindowInstance()->GetWidth(), 
STX_Service::GetWindowInstance()->GetHeight(), 
#endif
zNear, zFar));
}
void buildLookAtMatrixGL(float eyex, float eyey, float eyez,
		      float centerx, float centery, float centerz,
		      float upx, float upy, float upz,
		      D3DXFROMWINEMATRIX& m)
{
	centerz=-centerz;
  float x[3], y[3], z[3], mag;
  /* Difference eye and center vectors to make Z vector. */
  z[0] = eyex - centerx;
  z[1] = eyey - centery;
  z[2] = eyez - centerz;
  /* Normalize Z. */
  mag = sqrt(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]);
  if (mag) {
    z[0] /= mag;
    z[1] /= mag;
    z[2] /= mag;
  }
  /* Up vector makes Y vector. */
  y[0] = upx;
  y[1] = upy;
  y[2] = upz;
  /* X vector = Y cross Z. */
  x[0] =  y[1]*z[2] - y[2]*z[1];
  x[1] = -y[0]*z[2] + y[2]*z[0];
  x[2] =  y[0]*z[1] - y[1]*z[0];
  /* Recompute Y = Z cross X. */
  y[0] =  z[1]*x[2] - z[2]*x[1];
  y[1] = -z[0]*x[2] + z[2]*x[0];
  y[2] =  z[0]*x[1] - z[1]*x[0];
  /* Normalize X. */
  mag = sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
  if (mag) {
    x[0] /= mag;
    x[1] /= mag;
    x[2] /= mag;
  }
  /* Normalize Y. */
  mag = sqrt(y[0]*y[0] + y[1]*y[1] + y[2]*y[2]);
  if (mag) {
    y[0] /= mag;
    y[1] /= mag;
    y[2] /= mag;
  }

  /* Build resulting view matrix. */
  m.index(0,0) = (float) x[0];  m.index(0,1) = (float) x[1];
  m.index(0,2) = (float) x[2];  m.index(0,3) = (float) (-x[0]*eyex + -x[1]*eyey + -x[2]*eyez);
  m.index(1,0) = (float) y[0];  m.index(1,1) = (float) y[1];
  m.index(1,2) = (float) y[2];  m.index(1,3) = (float) (-y[0]*eyex + -y[1]*eyey + -y[2]*eyez);
  m.index(2,0) = (float) z[0];  m.index(2,1) = (float) z[1];
  m.index(2,2) = (float) z[2];  m.index(2,3) = (float) (-z[0]*eyex + -z[1]*eyey + -z[2]*eyez);
  m.index(3,0) = 0.0;   m.index(3,1) = 0.0;  m.index(3,2) = 0.0;  m.index(3,3) = 1.0;
}




void buildPerspectiveMatrixD3D(float fieldOfView,
			   float aspectRatio,
			   float zNear, float zFar,
			   D3DXFROMWINEMATRIX& m)
{

  float radians = fieldOfView * myPi / 180.0f;
	m = 
#if 1
		toGLProjection
#elif defined(_MSV_VER)
		toD3DProjection
#endif
		(perspectiveMatrixX(radians, 
#if 0
IRenderer::GetRendererInstance()->GetViewportWidth(), 
IRenderer::GetRendererInstance()->GetViewportHeight(), 
#else
STX_Service::GetWindowInstance()->GetWidth(), 
STX_Service::GetWindowInstance()->GetHeight(), 
#endif
zNear, zFar));
}
void buildLookAtMatrixD3D(float eyex, float eyey, float eyez,
		      float centerx, float centery, float centerz,
		      float upx, float upy, float upz,
		      D3DXFROMWINEMATRIX& m)
{
	centerz=-centerz;
  float x[3], y[3], z[3], mag;
  /* Difference eye and center vectors to make Z vector. */
  z[0] = eyex - centerx;
  z[1] = eyey - centery;
  z[2] = eyez - centerz;
  /* Normalize Z. */
  mag = sqrt(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]);
  if (mag) {
    z[0] /= mag;
    z[1] /= mag;
    z[2] /= mag;
  }
  /* Up vector makes Y vector. */
  y[0] = upx;
  y[1] = upy;
  y[2] = upz;
  /* X vector = Y cross Z. */
  x[0] =  y[1]*z[2] - y[2]*z[1];
  x[1] = -y[0]*z[2] + y[2]*z[0];
  x[2] =  y[0]*z[1] - y[1]*z[0];
  /* Recompute Y = Z cross X. */
  y[0] =  z[1]*x[2] - z[2]*x[1];
  y[1] = -z[0]*x[2] + z[2]*x[0];
  y[2] =  z[0]*x[1] - z[1]*x[0];
  /* Normalize X. */
  mag = sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
  if (mag) {
    x[0] /= mag;
    x[1] /= mag;
    x[2] /= mag;
  }
  /* Normalize Y. */
  mag = sqrt(y[0]*y[0] + y[1]*y[1] + y[2]*y[2]);
  if (mag) {
    y[0] /= mag;
    y[1] /= mag;
    y[2] /= mag;
  }

  /* Build resulting view matrix. */
  m.index(0,0) = (float) x[0];  m.index(0,1) = (float) x[1];
  m.index(0,2) = (float) x[2];  m.index(0,3) = (float) (-x[0]*eyex + -x[1]*eyey + -x[2]*eyez);
  m.index(1,0) = (float) y[0];  m.index(1,1) = (float) y[1];
  m.index(1,2) = (float) y[2];  m.index(1,3) = (float) (-y[0]*eyex + -y[1]*eyey + -y[2]*eyez);
  m.index(2,0) = (float) z[0];  m.index(2,1) = (float) z[1];
  m.index(2,2) = (float) z[2];  m.index(2,3) = (float) (-z[0]*eyex + -z[1]*eyey + -z[2]*eyez);
  m.index(3,0) = 0.0;   m.index(3,1) = 0.0;  m.index(3,2) = 0.0;  m.index(3,3) = 1.0;
}

unsigned int toRGBA(const D3DXFROMWINEVECTOR4 &u){
	return (int(u.x * 255) | (int(u.y * 255) << 8) | (int(u.z * 255) << 16) | (int(u.w * 255) << 24));
}
unsigned int toBGRA(const D3DXFROMWINEVECTOR4 &u){
	return (int(u.z * 255) | (int(u.y * 255) << 8) | (int(u.x * 255) << 16) | (int(u.w * 255) << 24));
}
D3DXFROMWINEVECTOR3 rgbeToRGB(unsigned char *rgbe){
	if (rgbe[3]){
		return D3DXFROMWINEVECTOR3(rgbe[0], rgbe[1], rgbe[2]) * ldexpf(1.0f, rgbe[3] - (int) (128 + 8));
	} else return D3DXFROMWINEVECTOR3(0, 0, 0);
}
unsigned int rgbToRGBE8(const D3DXFROMWINEVECTOR3 &rgb){
	float v = max(rgb.x, rgb.y);
	v = max(v, rgb.z);
	if (v < 1e-32f){
		return 0;
	} else {
		int ex;
		float m = frexpf(v, &ex) * 256.0f / v;
		unsigned int r = (unsigned int) (m * rgb.x);
		unsigned int g = (unsigned int) (m * rgb.y);
		unsigned int b = (unsigned int) (m * rgb.z);
		unsigned int e = (unsigned int) (ex + 128);
		return r | (g << 8) | (b << 16) | (e << 24);
	}
}
unsigned int rgbToRGB9E5(const D3DXFROMWINEVECTOR3 &rgb){
	float v = max(rgb.x, rgb.y);
	v = max(v, rgb.z);
	if (v < 1.52587890625e-5f){
		return 0;
	} else if (v < 65536){
		int ex;
		float m = frexpf(v, &ex) * 512.0f / v;
		unsigned int r = (unsigned int) (m * rgb.x);
		unsigned int g = (unsigned int) (m * rgb.y);
		unsigned int b = (unsigned int) (m * rgb.z);
		unsigned int e = (unsigned int) (ex + 15);
		return r | (g << 9) | (b << 18) | (e << 27);
	} else {
		unsigned int r = (rgb.x < 65536)? (unsigned int) (rgb.x * (1.0f / 128.0f)) : 0x1FF;
		unsigned int g = (rgb.y < 65536)? (unsigned int) (rgb.y * (1.0f / 128.0f)) : 0x1FF;
		unsigned int b = (rgb.z < 65536)? (unsigned int) (rgb.z * (1.0f / 128.0f)) : 0x1FF;
		unsigned int e = 31;
		return r | (g << 9) | (b << 18) | (e << 27);
	}
}
half::half(const float x){
	union {
		float floatI;
		unsigned int i;
	};
	floatI = x;
//	unsigned int i = *((unsigned int *) &x);
	int e = ((i >> 23) & 0xFF) - 112;
	int m =  i & 0x007FFFFF;
	sh = (i >> 16) & 0x8000;
	if (e <= 0){
		// Denorm
		m = ((m | 0x00800000) >> (1 - e)) + 0x1000;
		sh |= (m >> 13);
	} else if (e == 143){
		sh |= 0x7C00;
		if (m != 0){
			// NAN
			m >>= 13;
			sh |= m | (m == 0);
		}
	} else {
		m += 0x1000;
		if (m & 0x00800000){
			// Mantissa overflow
			m = 0;
			e++;
		}
		if (e >= 31){
			// Exponent overflow
			sh |= 0x7C00;
		} else {
			sh |= (e << 10) | (m >> 13);
		}
	}
}
half::operator float () const {
	union {
		unsigned int s;
		float result;
	};
	s = (sh & 0x8000) << 16;
	unsigned int e = (sh >> 10) & 0x1F;
	unsigned int m = sh & 0x03FF;
	if (e == 0){
		// +/- 0
		if (m == 0) return result;
		// Denorm
		while ((m & 0x0400) == 0){
			m += m;
			e--;
		}
		e++;
		m &= ~0x0400;
	} else if (e == 31){
		// INF / NAN
		s |= 0x7F800000 | (m << 13);
		return result;
	}
	s |= ((e + 112) << 23) | (m << 13);
	return result;
}
float clamp(const float v, const float c0, const float c1){
	return min(max(v, c0), c1);
}
D3DXFROMWINEVECTOR2 clamp(const D3DXFROMWINEVECTOR2 &v, const float c0, const float c1){
	return D3DXFROMWINEVECTOR2(min(max(v.x, c0), c1), min(max(v.y, c0), c1));
}
D3DXFROMWINEVECTOR2 clamp(const D3DXFROMWINEVECTOR2 &v, const D3DXFROMWINEVECTOR2 &c0, const D3DXFROMWINEVECTOR2 &c1){
	return D3DXFROMWINEVECTOR2(min(max(v.x, c0.x), c1.x), min(max(v.y, c0.y), c1.y));
}
D3DXFROMWINEVECTOR3 clamp(const D3DXFROMWINEVECTOR3 &v, const float c0, const float c1){
	return D3DXFROMWINEVECTOR3(min(max(v.x, c0), c1), min(max(v.y, c0), c1), min(max(v.z, c0), c1));
}
D3DXFROMWINEVECTOR3 clamp(const D3DXFROMWINEVECTOR3 &v, const D3DXFROMWINEVECTOR3 &c0, const D3DXFROMWINEVECTOR3 &c1){
	return D3DXFROMWINEVECTOR3(min(max(v.x, c0.x), c1.x), min(max(v.y, c0.y), c1.y), min(max(v.z, c0.z), c1.z));
}
D3DXFROMWINEVECTOR4 clamp(const D3DXFROMWINEVECTOR4 &v, const float c0, const float c1){
	return D3DXFROMWINEVECTOR4(min(max(v.x, c0), c1), min(max(v.y, c0), c1), min(max(v.z, c0), c1), min(max(v.z, c0), c1));
}
D3DXFROMWINEVECTOR4 clamp(const D3DXFROMWINEVECTOR4 &v, const D3DXFROMWINEVECTOR4 &c0, const D3DXFROMWINEVECTOR4 &c1){
	return D3DXFROMWINEVECTOR4(min(max(v.x, c0.x), c1.x),min(max(v.y, c0.y), c1.y), min(max(v.z, c0.z), c1.z), min(max(v.w, c0.w), c1.w));
}

