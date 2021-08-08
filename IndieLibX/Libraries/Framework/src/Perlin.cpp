/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Math\Perlin.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------


#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>
#include "Perlin.h"


#define B 0x1000
#define BM 0xff

#define N 0x1000

#if 0
static int    p[B + B + 2];
static float g1[B + B + 2];
static D3DXFROMWINEVECTOR2  g2[B + B + 2];
static D3DXFROMWINEVECTOR3  g3[B + B + 2];
#endif

#define setup(i, b0, b1, r0, r1)\
	int b0 = int(i + N) & BM;\
	int b1 = (b0 + 1) & BM;\
	float r0 = (i + N) - int(i + N);\
	float r1 = r0 - 1;

#if 0
float noise1(const float x){
	setup(x, bx0, bx1, rx0, rx1);
	float sx = sCurve(rx0);

	return lerp(rx0 * g1[p[bx0]], rx1 * g1[p[bx1]], sx);
}

float noise2(const float x, const float y){
	setup(x, bx0, bx1, rx0, rx1);
	setup(y, by0, by1, ry0, ry1);

	int b00 = p[p[bx0] + by0];
	int b10 = p[p[bx1] + by0];
	int b01 = p[p[bx0] + by1];
	int b11 = p[p[bx1] + by1];

	float sx = sCurve(rx0);
	float sy = sCurve(ry0);

	float a = lerp(D3DXFROMWINEVec2Dot(&g2[b00], &D3DXFROMWINEVECTOR2(rx0, ry0)), D3DXFROMWINEVec2Dot(&g2[b10], &D3DXFROMWINEVECTOR2(rx1, ry0)), sx);
	float b = lerp(D3DXFROMWINEVec2Dot(&g2[b01], &D3DXFROMWINEVECTOR2(rx0, ry1)), D3DXFROMWINEVec2Dot(&g2[b11], &D3DXFROMWINEVECTOR2(rx1, ry1)), sx);

	return lerp(a, b, sy);
}

float noise3(const float x, const float y, const float z){
	setup(x, bx0, bx1, rx0, rx1);
	setup(y, by0, by1, ry0, ry1);
	setup(z, bz0, bz1, rz0, rz1);

	int b00 = p[p[bx0] + by0];
	int b10 = p[p[bx1] + by0];
	int b01 = p[p[bx0] + by1];
	int b11 = p[p[bx1] + by1];

	float sx = sCurve(rx0);
	float sy = sCurve(ry0);
	float sz = sCurve(rz0);

	float a0 = lerp(D3DXFROMWINEVec3Dot(&g3[b00 + bz0], &D3DXFROMWINEVECTOR3(rx0, ry0, rz0)), D3DXFROMWINEVec3Dot(&g3[b10 + bz0], &D3DXFROMWINEVECTOR3(rx1, ry0, rz0)), sx);
	float b0 = lerp(D3DXFROMWINEVec3Dot(&g3[b01 + bz0], &D3DXFROMWINEVECTOR3(rx0, ry1, rz0)), D3DXFROMWINEVec3Dot(&g3[b11 + bz0], &D3DXFROMWINEVECTOR3(rx1, ry1, rz0)), sx);
	float c0 = lerp(a0, b0, sy);

	float a1 = lerp(D3DXFROMWINEVec3Dot(&g3[b00 + bz1], &D3DXFROMWINEVECTOR3(rx0, ry0, rz1)), D3DXFROMWINEVec3Dot(&g3[b10 + bz1], &D3DXFROMWINEVECTOR3(rx1, ry0, rz1)), sx);
	float b1 = lerp(D3DXFROMWINEVec3Dot(&g3[b01 + bz1], &D3DXFROMWINEVECTOR3(rx0, ry1, rz1)), D3DXFROMWINEVec3Dot(&g3[b11 + bz1], &D3DXFROMWINEVECTOR3(rx1, ry1, rz1)), sx);
	float c1 = lerp(a1, b1, sy);

	return lerp(c0, c1, sz);
}

float turbulence1(const float x, float freq){
	float t = 0.0f;

	do {
		t += noise1(freq * x) / freq;
		freq *= 0.5f;
	} while (freq >= 1.0f);
	
	return t;
}

float turbulence2(const float x, const float y, float freq){
	float t = 0.0f;

	do {
		t += noise2(freq * x, freq * y) / freq;
		freq *= 0.5f;
	} while (freq >= 1.0f);
	
	return t;
}

float turbulence3(const float x, const float y, const float z, float freq){
	float t = 0.0f;

	do {
		t += noise3(freq * x, freq * y, freq * z) / freq;
		freq *= 0.5f;
	} while (freq >= 1.0f);
	
	return t;
}

float tileableNoise1(const float x, const float w){
	return (noise1(x)     * (w - x) +
			noise1(x - w) *      x) / w;
}

float tileableNoise2(const float x, const float y, const float w, const float h){
	return (noise2(x,     y)     * (w - x) * (h - y) +
			noise2(x - w, y)     *      x  * (h - y) +
			noise2(x,     y - h) * (w - x) *      y  +
			noise2(x - w, y - h) *      x  *      y) / (w * h);
}

float tileableNoise3(const float x, const float y, const float z, const float w, const float h, const float d){
	return (noise3(x,     y,     z)     * (w - x) * (h - y) * (d - z) +
			noise3(x - w, y,     z)     *      x  * (h - y) * (d - z) +
			noise3(x,     y - h, z)     * (w - x) *      y  * (d - z) +
			noise3(x - w, y - h, z)     *      x  *      y  * (d - z) + 
			noise3(x,     y,     z - d) * (w - x) * (h - y) *      z  +
			noise3(x - w, y,     z - d) *      x  * (h - y) *      z  +
			noise3(x,     y - h, z - d) * (w - x) *      y  *      z  +
			noise3(x - w, y - h, z - d) *      x  *      y  *      z) / (w * h * d);
}

float tileableTurbulence1(const float x, const float w, float freq){
	float t = 0.0f;

	do {
		t += tileableNoise1(freq * x, w * freq) / freq;
		freq *= 0.5f;
	} while (freq >= 1.0f);
	
	return t;
}

float tileableTurbulence2(const float x, const float y, const float w, const float h, float freq){
	float t = 0.0f;

	do {
		t += tileableNoise2(freq * x, freq * y, w * freq, h * freq) / freq;
		freq *= 0.5f;
	} while (freq >= 1.0f);
	
	return t;
}

float tileableTurbulence3(const float x, const float y, const float z, const float w, const float h, const float d, float freq){
	float t = 0.0f;

	do {
		t += tileableNoise3(freq * x, freq * y, freq * z, w * freq, h * freq, d * freq) / freq;
		freq *= 0.5f;
	} while (freq >= 1.0f);
	
	return t;
}
#endif
#define RAND float((rand() % (B + B)) - B)
#if 0
void initPerlin(){
	for (int i = 0; i < B; i++){
		p[i] = i;

		g1[i] = RAND / B;
		D3DXFROMWINEVECTOR2 v2(RAND, RAND);
		v2/=B;
		g2[i] = v2.normalize();
		D3DXFROMWINEVECTOR3 v3(RAND, RAND, RAND);
		v3/=B;
		g3[i] = v3.normalize();
	}

	for (int i = 0; i < B; i++){
		int r = rand() % B;
		int tmp = p[i];
		p[i] = p[r];
		p[r] = tmp;
	}

	for (int i = 0; i < B + 2; i++){
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		g2[B + i] = g2[i];
		g3[B + i] = g3[i];
	}
}
#endif

