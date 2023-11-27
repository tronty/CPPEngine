/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//
// Noise Shader Library for Unity - https://github.com/keijiro/NoiseShader
//
// Original work (webgl-noise) Copyright (C) 2011 Stefan Gustavson
// Translation and modification was made by Keijiro Takahashi.
//
// This shader is based on the webgl-noise GLSL shader. For further details
// of the original shader, please see the following description from the
// original source code.
//

//
// GLSL textureless classic 3D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
// Author:  Stefan Gustavson (stefan.gustavson@liu.se)
// Version: 2011-10-11
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/ashima/webgl-noise
//

vec3 mod(vec3 x, vec3 y)
{
  return x - y * floor(x / y);
}

vec3 mod289(vec3 x)
{
  return x - floor(x / 289.0) * 289.0;
}

vec4 mod289(vec4 x)
{
  return x - floor(x / 289.0) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return (vec4)1.79284291400159 - r * 0.85373472095314;
}

vec3 fade(vec3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec3 P)
{
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + (vec3)1.0; // Integer part + 1
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = frac(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - (vec3)1.0; // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.y, Pi0.y, Pi1.y, Pi1.y);
  vec4 iz0 = (vec4)Pi0.z;
  vec4 iz1 = (vec4)Pi1.z;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 / 7.0;
  vec4 gy0 = frac(floor(gx0) / 7.0) - 0.5;
  gx0 = frac(gx0);
  vec4 gz0 = (vec4)0.5 - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, (vec4)0.0);
  gx0 -= sz0 * (step((vec4)0.0, gx0) - 0.5);
  gy0 -= sz0 * (step((vec4)0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 / 7.0;
  vec4 gy1 = frac(floor(gx1) / 7.0) - 0.5;
  gx1 = frac(gx1);
  vec4 gz1 = (vec4)0.5 - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, (vec4)0.0);
  gx1 -= sz1 * (step((vec4)0.0, gx1) - 0.5);
  gy1 -= sz1 * (step((vec4)0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;

  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.y, Pf0.z));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.x, Pf1.y, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.x, Pf0.y, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.y, Pf1.z));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = lerp(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}

// Classic Perlin noise, periodic variant
float pnoise(vec3 P, vec3 rep)
{
  vec3 Pi0 = mod(floor(P), rep); // Integer part, modulo period
  vec3 Pi1 = mod(Pi0 + (vec3)1.0, rep); // Integer part + 1, mod period
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = frac(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - (vec3)1.0; // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.y, Pi0.y, Pi1.y, Pi1.y);
  vec4 iz0 = (vec4)Pi0.z;
  vec4 iz1 = (vec4)Pi1.z;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 / 7.0;
  vec4 gy0 = frac(floor(gx0) / 7.0) - 0.5;
  gx0 = frac(gx0);
  vec4 gz0 = (vec4)0.5 - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, (vec4)0.0);
  gx0 -= sz0 * (step((vec4)0.0, gx0) - 0.5);
  gy0 -= sz0 * (step((vec4)0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 / 7.0;
  vec4 gy1 = frac(floor(gx1) / 7.0) - 0.5;
  gx1 = frac(gx1);
  vec4 gz1 = (vec4)0.5 - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, (vec4)0.0);
  gx1 -= sz1 * (step((vec4)0.0, gx1) - 0.5);
  gy1 -= sz1 * (step((vec4)0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.y, Pf0.z));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.x, Pf1.y, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.x, Pf0.y, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.y, Pf1.z));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = lerp(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}

float noise( vec3 a)
{
	return cnoise(a);
}


// https://www.clicktorelease.com/blog/vertex-displacement-noise-3d-webgl-glsl-three-js/
float turbulence( vec3 p )
{
  float w = 100.0;
  float t = -.5;
  for (float f = 1.0 ; f <= 10.0 ; f++ ){
    float power = pow( 2.0, f );
    t += abs( pnoise( vec3( power * p ), vec3( 10.0, 10.0, 10.0 ) ) / power );
  }
  return t;
}
/*********************************************************************NVMH3****
File:  $Id: //sw/devtools/SDK/9.5/SDK/MEDIA/HLSL/fireball.fx#1 $

Copyright NVIDIA Corporation 2004
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

	Procedural volumetric fireball effect

	The distance from the centre of the volume is mapped to a color and
	a density value using a 1D texture. The distance is perturbed by 4
	octaves of Perlin noise to add visual detail. The final effect is
	rendered using ray marching.
	
	sgreen 6/04	
	See: http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm

******************************************************************************/

float Script = 0.8; // version #

vec4 ClearColor = vec4( 0.0, 0.0, 0.0, 1.0);

float ClearDepth = 1.0;

ROW_MAJOR mat4 viewInv MVPSEMANTIC;// : ViewInverse;
vec2 viewport;// : VIEWPORTPIXELSIZE;

vec3 boxMin = vec3( -1.0, -1.0, -1.0 );
vec3 boxMax = vec3( 1.0, 1.0, 1.0 );

float time;// : TIME;

int steps = 3;

float foclen = 500.0;

float brightness = 50.0;

float density = 0.3;

float noiseFreq = 0.05;

float noiseAmp = 1.0;

float distanceScale = 1.0;

vec3 timeScale = vec3( 0, -0.01, 0 );

float objScale = vec3( 1.1, 1.1, 1.1 );

//////////////

sampler2D tExplosion;

/// texture funcs ////

vec4 fireball(vec3 p, float time)
{
	float d = length(p);
//	d += tex_noise(p*noiseFreq)*noiseAmp;
	d += turbulence(p*noiseFreq + time*timeScale).x*noiseAmp;
	#if 0
	vec4 c = tex1D(tExplosion, d*distanceScale);
	#else
	vec2 tPos = vec2( d*distanceScale, 0);
	vec4 c = texture2D( tExplosion, tPos );
	#endif		

	//c.a *= density;
	c.a = 1;
	return c;
}

///// ray struct ////////////////
/*
struct Ray {
	vec3 o;	// origin
	vec3 d;	// direction
};
*/
////////////////////////////////////////////////////////

// Ray-box intesection using slabs method
// See: http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm

// Intersect ray with slab in one axis
// o, d - ray origin, direction
// l, h - slab minimum, maximum
void IntersectSlab(float o, float d, float l, float h, inout bool hit, inout float tnear, inout float tfar)
{
	if (d==0.0) {
		// ray parallel to planes
		if ((o < l) || (o > h)) {
			// ray is not between slabs
			hit = false;
//			return;
		}
	}
	
	float t1 = (l - o) / d;
	float t2 = (h - o) / d;
	if (t1 > t2) {
		// swap so that t1 is nearest
		float temp = t1;
		t1 = t2;
		t2 = temp;
	}

	if (t1 > tnear) {
		tnear = t1;
	}
	if (t2 < tfar) {
		tfar = t2;
	}

	if (tnear > tfar) {
		// box missed
		hit = false;
	}
	if (tfar < 0) {
		// box behind ray
		hit = false;
	}
}

// Intersect ray with box
// note - could probably vectorize this better
bool IntersectBox(vec3 o, vec3 d, vec3 Bl, vec3 Bh, out float tnear, out float tfar)
{
	tnear = -1e20;
	tfar = 1e20;
	bool hit = true;
	IntersectSlab(/* r. */ o.x, /* r. */ d.x, Bl.x, Bh.x, hit, tnear, tfar);
	IntersectSlab(/* r. */ o.y, /* r. */ d.y, Bl.y, Bh.y, hit, tnear, tfar);
	IntersectSlab(/* r. */ o.z, /* r. */ d.z, Bl.z, Bh.z, hit, tnear, tfar);
	return hit;
}

[Vertex shader]
//// Vertex shader /////////////////////////////////////////////////////////////////////

struct appdata {
    vec3 Position	: POSITION;
    vec3 Normal	: NORMAL;
    vec3 Binormal	: BINORMAL;
    vec3 Tangent	: TANGENT;
    vec3 Color	: TEXCOORD0;
    vec2 UV		: TEXCOORD1;
};
ROW_MAJOR mat4 worldViewProj MVPSEMANTIC;

void RayMarchVS(in appdata IN,
				out vec4 outp : POSITION,
				//out Ray eyeray : TEXCOORD1,
				out vec2 uv : TEXCOORD0,
				out vec3 o : TEXCOORD1,
				out vec3 d : TEXCOORD2,
				out float mod_time : TEXCOORD3
				)
{
	vec2 texcoord=IN.UV;

    	outp=vec4(IN.Position.x, IN.Position.y, IN.Position.z, 1);
	// calculate world space eye ray
	// origin
	/* eyeray. */ o = mul(vec4(0, 0, 0, 1), viewInv).xyz;	

	// direction
	/* eyeray. */ d.xy = ((texcoord*2.0)-1.0) * viewport;
	/* eyeray. */ d.y = -/* eyeray. */ d.y;	// flip y axis
	/* eyeray. */ d.z = foclen;
	
	/* eyeray. */ d = mul(/* eyeray. */ d, (float3x3) viewInv);
	
	mod_time = fmod(time, 100.0);
	uv=IN.UV;
	outp = mul(vec4(IN.Position.x, IN.Position.y, IN.Position.z, 1),worldViewProj);
}
// Pixel shader /////////////////////////////////////////////////////////////
[Fragment shader]

vec4 RayMarchPS(//Ray eyeray : TEXCOORD1,
				vec4 in_p : POSITION,
				vec2 uv : TEXCOORD0,
				vec3 o : TEXCOORD1,
				vec3 d : TEXCOORD2,
				  float mod_time : TEXCOORD3
				  //uniform sampler3D volume,
				  //uniform int steps
				) : COLOR
{
	//return vec4(1,0,0,1);
	//return vec4(o,1);
	//return vec4(d,1);
	// calculate ray intersection with bounding box
	float tnear, tfar;
	bool hit = IntersectBox(o, d, boxMin, boxMax, tnear, tfar);
	if (!hit) discard;

	// calculate intersection points
	vec3 Pnear = /* eyeray. */ o + /* eyeray. */ d*tnear;
	vec3 Pfar = /* eyeray. */ o + /* eyeray. */ d*tfar;
		
	// map box world coords to texture coords
	Pnear *= objScale;
	Pfar *= objScale;
	
	// march along ray, accumulating color
	vec4 c = 0;
	vec3 Pstep = (Pnear - Pfar) / (steps-1);
	vec3 P = Pfar;
	// this compiles to a real loop in PS3.0:
	for(int i=0; i<steps; i++) {		
		vec4 s = fireball(P, mod_time);
		c = (1.0-s.a)*c + s.a*s;
		P += Pstep;
	}
	c /= steps;
	c *= brightness;

//	return hit;
//	return tfar - tnear;
	c.w=1;
	return c;
}

