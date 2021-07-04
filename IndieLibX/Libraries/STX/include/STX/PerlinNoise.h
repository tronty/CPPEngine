/***********      .---.         .-"-.      *******************\
* -------- *     /   ._.       / � ` \     * ---------------- *
* Author's *     \_  (__\      \_�v�_/     * humus@rogers.com *
*   note   *     //   \\       //   \\     * ICQ #47010716    *
* -------- *    ((     ))     ((     ))    * ---------------- *
*          ****--""---""-------""---""--****                  ********\
* This file is a part of the work done by Humus. You are free to use  *
* the code in any way you like, modified, unmodified or copy'n'pasted *
* into your own work. However, I expect you to respect these points:  *
*  @ If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  @ For use in anything commercial, please request my approval.      *
*  @ Share your work and ideas too as much as you can.                *
\*********************************************************************/

#ifndef _PERLINNOISE_H_
#define _PERLINNOISE_H_


/*
	Credits:

	This file is based on the great work done by Ken Perlin.
	http://mrl.nyu.edu/~perlin/doc/oscar.html
*/

#include <stdlib.h>
#include <math.h>

inline float bias(const float a, const float b){
	return powf(a, logf(b) / logf(0.5f));
}

inline float gain(const float a, const float b){
	if (a < 0.001f)	return 0; else if (a > 0.999f) return 1.0f;

	float p = logf(1.0f - b) / logf(0.5f);

	if (a < 0.5f) return float(powf(2 * a, p) / 2);
			 else return 1 - float(powf(2 * (1.0f - a), p) / 2);
}

/* noise functions over 1, 2, and 3 dimensions */

#define B 0x1000
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

static int p[B + B + 2];
static float g3[B + B + 2][3];
static float g2[B + B + 2][2];
static float g1[B + B + 2];

#define s_curve(t) (t * t * (3 - 2 * t))


#define lerp_(t, a, b) (a + t * (b - a))

#define setup(i,b0,b1,r0,r1)\
	t = i + N;\
	b0 = ((int) t) & BM;\
	b1 = (b0 + 1) & BM;\
	r0 = t - (int) t;\
	r1 = r0 - 1;
#if 1
float noise1(const float x);
float noise2(const float x, const float y);
float noise3(const float x, const float y, const float z);
void normalize2(float v[2]);
void normalize3(float v[3]);
float turbulence2(const float x, const float y, float freq);
float turbulence3(const float x, const float y, const float z, float freq);
float tileableNoise1(const float x, const float w);
float tileableNoise2(const float x, const float y, const float w, const float h);
float tileableNoise3(const float x, const float y, const float z, const float w, const float h, const float d);
float tileableTurbulence2(const float x, const float y, const float w, const float h, float freq);
float tileableTurbulence3(const float x, const float y, const float z, const float w, const float h, const float d, float freq);
#else
inline float noise1(const float x){
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v;

	setup(x, bx0,bx1, rx0,rx1);

	sx = s_curve(rx0);

	u = rx0 * g1[p[bx0]];
	v = rx1 * g1[p[bx1]];

	return lerp_(sx, u, v);
}

#define at2(rx,ry) (rx * q[0] + ry * q[1])

inline float noise2(const float x, const float y){
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	int i, j;

	setup(x, bx0,bx1, rx0,rx1);
	setup(y, by0,by1, ry0,ry1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	sx = s_curve(rx0);
	sy = s_curve(ry0);


	q = g2[b00]; u = at2(rx0,ry0);
	q = g2[b10]; v = at2(rx1,ry0);
	a = lerp_(sx, u, v);

	q = g2[b01]; u = at2(rx0,ry1);
	q = g2[b11]; v = at2(rx1,ry1);
	b = lerp_(sx, u, v);

	return lerp_(sy, a, b);
}

#define at3(rx,ry,rz) (rx * q[0] + ry * q[1] + rz * q[2])

inline float noise3(const float x, const float y, const float z){
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	int i, j;

	setup(x, bx0,bx1, rx0,rx1);
	setup(y, by0,by1, ry0,ry1);
	setup(z, bz0,bz1, rz0,rz1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

	q = g3[b00 + bz0]; u = at3(rx0,ry0,rz0);
	q = g3[b10 + bz0]; v = at3(rx1,ry0,rz0);
	a = lerp_(t, u, v);

	q = g3[b01 + bz0]; u = at3(rx0,ry1,rz0);
	q = g3[b11 + bz0]; v = at3(rx1,ry1,rz0);
	b = lerp_(t, u, v);

	c = lerp_(sy, a, b);

	q = g3[b00 + bz1]; u = at3(rx0,ry0,rz1);
	q = g3[b10 + bz1]; v = at3(rx1,ry0,rz1);
	a = lerp_(t, u, v);

	q = g3[b01 + bz1]; u = at3(rx0,ry1,rz1);
	q = g3[b11 + bz1]; v = at3(rx1,ry1,rz1);
	b = lerp_(t, u, v);

	d = lerp_(sy, a, b);

	return lerp_(sz, c, d);
}

inline void normalize2(float v[2]){
	float s = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1]);
	v[0] *= s;
	v[1] *= s;
}

inline void normalize3(float v[3]){
	float s = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
}

inline float turbulence2(const float x, const float y, float freq){
	float t = 0.0f;

	do {
		t += noise2(freq * x, freq * y) / freq;
		freq *= 0.5f;
	} while (freq >= 1.0f);
	
	return t;
}

inline float turbulence3(const float x, const float y, const float z, float freq){
	float t = 0.0f;

	do {
		t += noise3(freq * x, freq * y, freq * z) / freq;
		freq *= 0.5f;
	} while (freq >= 1.0f);
	
	return t;
}

inline float tileableNoise1(const float x, const float w){
	return (noise1(x)     * (w - x) +
			noise1(x - w) *      x) / w;
}

inline float tileableNoise2(const float x, const float y, const float w, const float h){
	return (noise2(x,     y)     * (w - x) * (h - y) +
			noise2(x - w, y)     *      x  * (h - y) +
			noise2(x,     y - h) * (w - x) *      y  +
			noise2(x - w, y - h) *      x  *      y) / (w * h);
}

inline float tileableNoise3(const float x, const float y, const float z, const float w, const float h, const float d){
	return (noise3(x,     y,     z)     * (w - x) * (h - y) * (d - z) +
			noise3(x - w, y,     z)     *      x  * (h - y) * (d - z) +
			noise3(x,     y - h, z)     * (w - x) *      y  * (d - z) +
			noise3(x - w, y - h, z)     *      x  *      y  * (d - z) + 
			noise3(x,     y,     z - d) * (w - x) * (h - y) *      z  +
			noise3(x - w, y,     z - d) *      x  * (h - y) *      z  +
			noise3(x,     y - h, z - d) * (w - x) *      y  *      z  +
			noise3(x - w, y - h, z - d) *      x  *      y  *      z) / (w * h * d);
}

inline float tileableTurbulence2(const float x, const float y, const float w, const float h, float freq){
	float t = 0.0f;

	do {
		t += tileableNoise2(freq * x, freq * y, w * freq, h * freq) / freq;
		freq *= 0.5f;
	} while (freq >= 1.0f);
	
	return t;
}

inline float tileableTurbulence3(const float x, const float y, const float z, const float w, const float h, const float d, float freq){
	float t = 0.0f;

	do {
		t += tileableNoise3(freq * x, freq * y, freq * z, w * freq, h * freq, d * freq) / freq;
		freq *= 0.5f;
	} while (freq >= 1.0f);
	
	return t;
}
#endif

void initPerlin();

#endif // _PERLINNOISE_H_

