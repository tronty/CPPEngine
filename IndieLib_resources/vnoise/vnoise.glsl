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

float noise( vec3 a)
{
	return cnoise(a);
}
/******************************************************************************
File:  vnoise.glsl

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:

sgreen 5/02/02:

This is based on Perlin's original code:
    http://mrl.nyu.edu/~perlin/doc/oscar.html

    It combines the permutation and gradient tables into one array of
    vec4's to conserve constant memory.
    The table is duplicated twice to avoid modulo operations.

jallen@nvidia.com: 10/12/03:

    GLSL version of Cg vertex noise shader

Notes:

    Should use separate tables for 1, 2 and 3D versions

******************************************************************************/

// this is the smoothstep function f(t) = 3t^2 - 2t^3, without the normalization
vec3 s_curve(vec3 t)
{
    return t*t*( vec3(3.0, 3.0, 3.0) - vec3(2.0, 2.0, 2.0)*t);
}

vec2 s_curve(vec2 t)
{
    return t*t*( vec2(3.0, 3.0) - vec2(2.0, 2.0)*t);
}

float s_curve(float t)
{
    return t*t*(3.0-2.0*t);
}

// Classic Perlin noise, periodic variant
float pnoise(vec2 P, vec2 rep)
{
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  //Pi = mod(Pi, rep.xyxy); // To create noise with explicit period
  Pi = mod289(Pi);        // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;

  vec4 i = permute(permute(ix) + iy);

  vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  vec4 gy = abs(gx) - 0.5 ;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;

  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);

  vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;  
  g01 *= norm.y;  
  g10 *= norm.z;  
  g11 *= norm.w;  

  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));

  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
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

struct VsIn {
    vec3 Position	: POSITION;
    vec3 Normal	: NORMAL;
    vec3 Binormal	: BINORMAL;
    vec3 Tangent	: TANGENT;
    vec3 Color	: TEXCOORD0;
    vec2 UV		: TEXCOORD1;
};
struct VsOut {
    vec4 position: POSITION;
    vec3 Color	: TEXCOORD0;
    vec2 UV		: TEXCOORD1;
    float noise		: TEXCOORD2;
};
[Vertex shader]
 mat4 worldViewProj ;
float Displacement = 1.0;
float time = 0.0;

VsOut main(VsIn In)
{
	VsOut Out=(VsOut)0;
    vec4 noisePos = mul(worldViewProj, vec4(_Position, 1.0));
    float i = (noise(noisePos.xyz) + 1.0) * 0.5;
    __Color = vec3(i, i, i);
#if 1
  // add time to the noise parameters so it's animated
  __noise = 10.0 *  (-.10) * turbulence( .5 * _Normal + time );
  float b = 5.0 * pnoise( 0.05 * _Position + vec3( 2.0 * time ), vec3( 100.0 ) );
  float displacement = (- __noise) + b;

  vec3 newPosition = _Position + _Normal * displacement;
  __position = mul(worldViewProj, vec4(newPosition, 1.0));
#else
    // displacement along normal
    vec4 position = vec4(_Position, 1.0) + (vec4(_Normal, 1.0) * i * Displacement);
    position.w = 1.0;
    __position = mul(worldViewProj, position);
#endif
	__UV = _UV;
	return Out;
}
[Fragment shader]
sampler2D tColor;
vec4 main(VsOut IN): COLOR {
	return vec4(_Color, 1.0)*texture2D(tColor, _UV);
}

