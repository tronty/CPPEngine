/*
 Copyright (c) 2018 Tommi Roenty http://www.tommironty.fi/
 Licensed under The GNU Lesser General Public License, version 2.1:
 http://opensource.org/licenses/LGPL-2.1
*/

//define fract frac
//define mix lerp

//
// GLSL textureless classic 2D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
// Author:  Stefan Gustavson (stefan.gustavson@liu.se)
// Version: 2011-08-22
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/stegu/webgl-noise
//

float4 mod289(float4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 permute(float4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

float4 taylorInvSqrt(float4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float2 fade(float2 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(float2 P)
{
  float4 Pi = floor(P.xyxy) + float4(0.0, 0.0, 1.0, 1.0);
  float4 Pf = fract(P.xyxy) - float4(0.0, 0.0, 1.0, 1.0);
  Pi = mod289(Pi); // To avoid truncation effects in permutation
  float4 ix = Pi.xzxz;
  float4 iy = Pi.yyww;
  float4 fx = Pf.xzxz;
  float4 fy = Pf.yyww;

  float4 i = permute(permute(ix) + iy);

  float4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  float4 gy = abs(gx) - 0.5 ;
  float4 tx = floor(gx + 0.5);
  gx = gx - tx;

  float2 g00 = float2(gx.x,gy.x);
  float2 g10 = float2(gx.y,gy.y);
  float2 g01 = float2(gx.z,gy.z);
  float2 g11 = float2(gx.w,gy.w);

  float4 norm = taylorInvSqrt(float4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;  
  g01 *= norm.y;  
  g10 *= norm.z;  
  g11 *= norm.w;  

  float n00 = dot(g00, float2(fx.x, fy.x));
  float n10 = dot(g10, float2(fx.y, fy.y));
  float n01 = dot(g01, float2(fx.z, fy.z));
  float n11 = dot(g11, float2(fx.w, fy.w));

  float2 fade_xy = fade(Pf.xy);
  float2 n_x = mix(float2(n00, n01), float2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}

float4 mod(float4 a1, float2 a2){return float4(0.0, 0.0, 0.0, 0.0);}

// Classic Perlin noise, periodic variant
float pnoise(float2 P, float2 rep)
{
  float4 Pi = floor(P.xyxy) + float4(0.0, 0.0, 1.0, 1.0);
  float4 Pf = fract(P.xyxy) - float4(0.0, 0.0, 1.0, 1.0);
  //Pi = mod(Pi, rep.xyxy); // To create noise with explicit period
  Pi = mod289(Pi);        // To avoid truncation effects in permutation
  float4 ix = Pi.xzxz;
  float4 iy = Pi.yyww;
  float4 fx = Pf.xzxz;
  float4 fy = Pf.yyww;

  float4 i = permute(permute(ix) + iy);

  float4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  float4 gy = abs(gx) - 0.5 ;
  float4 tx = floor(gx + 0.5);
  gx = gx - tx;

  float2 g00 = float2(gx.x,gy.x);
  float2 g10 = float2(gx.y,gy.y);
  float2 g01 = float2(gx.z,gy.z);
  float2 g11 = float2(gx.w,gy.w);

  float4 norm = taylorInvSqrt(float4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;  
  g01 *= norm.y;  
  g10 *= norm.z;  
  g11 *= norm.w;  

  float n00 = dot(g00, float2(fx.x, fy.x));
  float n10 = dot(g10, float2(fx.y, fy.y));
  float n01 = dot(g01, float2(fx.z, fy.z));
  float n11 = dot(g11, float2(fx.w, fy.w));

  float2 fade_xy = fade(Pf.xy);
  float2 n_x = mix(float2(n00, n01), float2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}

// https://www.clicktorelease.com/blog/vertex-displacement-noise-3d-webgl-glsl-three-js/
float turbulence(float2 point)
{
  float value=0.0;
  float freq=1.0;
  value += abs(cnoise(point));
  value += abs(cnoise(point)*2.0)/2.0;
  value += abs(cnoise(point)*4.0)/4.0;
  value += abs(cnoise(point)*8.0)/8.0;
  return clamp(value, -1.0, 1.0);
}

struct VsIn {
	float2 position: POSITION;
	float2 normal: NORMAL;
	float2 uv: TEXCOORD0;
};
struct VsOut {
	float4 position: POSITION;
	float2 uv: TEXCOORD0;
	float noise: TEXCOORD1;
};
[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
float time;
float4 scaleBias=float4(0.003,-0.003,-1,1);
VsOut main(VsIn In)
{
 VsOut Out=(VsOut)0;
 Out.uv=In.uv;

  // add time to the noise parameters so it's animated
  Out.noise = 10.0 *  (-.10) * turbulence( .5 * In.normal + time );
  float b = 5.0 * pnoise( 0.05 * In.position + float2( 2.0 * time ), float2( 100.0 ) );
  float displacement = (- Out.noise) + b;

  float2 newPosition = In.position + In.normal * displacement;
 #if 1
	//Out.position.xy = In.position.xy * scaleBias.xy + scaleBias.zw;
	Out.position.xy = newPosition.xy * scaleBias.xy + scaleBias.zw;
	Out.position.w= WSIGN 1;
 #else
 	Out.position = mul(worldViewProj, float4(newPosition, 0.0, 1.0));
 #endif
 return Out;
}

[Fragment shader]
sampler2D tExplosion;

float random( float2 scale, float seed, float2 FragCoord ){
 return frac( sin( dot( FragCoord+seed, scale ) )*43758.5453+seed );
}

float4 main(VsOut IN): COLOR 
{
 // get a random offset
 float r=.01*random( float2( 12.9898, 78.233), 0.0, IN.position.xy );
 // lookup vertically in the texture, using noise and offset
 // to get the right RGB colour
 float2 tPos=float2( 1.3*IN.noise+r, 0 );
 float4 color=tex2D( tExplosion, tPos );

 color=float4( color.rgb, 1.0 );
 //color.rgb = cnoise(IN.uv);
 color.a=1;

 return color;
}

