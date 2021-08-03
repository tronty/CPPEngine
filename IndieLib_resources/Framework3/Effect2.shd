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

float3 mod(float3 x, float3 y)
{
  return x - y * floor(x / y);
}

float3 mod289(float3 x)
{
  return x - floor(x / 289.0) * 289.0;
}

float4 mod289(float4 x)
{
  return x - floor(x / 289.0) * 289.0;
}

float4 permute(float4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

float4 taylorInvSqrt(float4 r)
{
  return (float4)1.79284291400159 - r * 0.85373472095314;
}

float3 fade(float3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(float3 P)
{
  float3 Pi0 = floor(P); // Integer part for indexing
  float3 Pi1 = Pi0 + (float3)1.0; // Integer part + 1
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  float3 Pf0 = frac(P); // Fractional part for interpolation
  float3 Pf1 = Pf0 - (float3)1.0; // Fractional part - 1.0
  float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  float4 iy = float4(Pi0.y, Pi0.y, Pi1.y, Pi1.y);
  float4 iz0 = (float4)Pi0.z;
  float4 iz1 = (float4)Pi1.z;

  float4 ixy = permute(permute(ix) + iy);
  float4 ixy0 = permute(ixy + iz0);
  float4 ixy1 = permute(ixy + iz1);

  float4 gx0 = ixy0 / 7.0;
  float4 gy0 = frac(floor(gx0) / 7.0) - 0.5;
  gx0 = frac(gx0);
  float4 gz0 = (float4)0.5 - abs(gx0) - abs(gy0);
  float4 sz0 = step(gz0, (float4)0.0);
  gx0 -= sz0 * (step((float4)0.0, gx0) - 0.5);
  gy0 -= sz0 * (step((float4)0.0, gy0) - 0.5);

  float4 gx1 = ixy1 / 7.0;
  float4 gy1 = frac(floor(gx1) / 7.0) - 0.5;
  gx1 = frac(gx1);
  float4 gz1 = (float4)0.5 - abs(gx1) - abs(gy1);
  float4 sz1 = step(gz1, (float4)0.0);
  gx1 -= sz1 * (step((float4)0.0, gx1) - 0.5);
  gy1 -= sz1 * (step((float4)0.0, gy1) - 0.5);

  float3 g000 = float3(gx0.x,gy0.x,gz0.x);
  float3 g100 = float3(gx0.y,gy0.y,gz0.y);
  float3 g010 = float3(gx0.z,gy0.z,gz0.z);
  float3 g110 = float3(gx0.w,gy0.w,gz0.w);
  float3 g001 = float3(gx1.x,gy1.x,gz1.x);
  float3 g101 = float3(gx1.y,gy1.y,gz1.y);
  float3 g011 = float3(gx1.z,gy1.z,gz1.z);
  float3 g111 = float3(gx1.w,gy1.w,gz1.w);

  float4 norm0 = taylorInvSqrt(float4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;

  float4 norm1 = taylorInvSqrt(float4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, float3(Pf1.x, Pf0.y, Pf0.z));
  float n010 = dot(g010, float3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, float3(Pf1.x, Pf1.y, Pf0.z));
  float n001 = dot(g001, float3(Pf0.x, Pf0.y, Pf1.z));
  float n101 = dot(g101, float3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, float3(Pf0.x, Pf1.y, Pf1.z));
  float n111 = dot(g111, Pf1);

  float3 fade_xyz = fade(Pf0);
  float4 n_z = lerp(float4(n000, n100, n010, n110), float4(n001, n101, n011, n111), fade_xyz.z);
  float2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}

// Classic Perlin noise, periodic variant
float pnoise(float3 P, float3 rep)
{
  float3 Pi0 = mod(floor(P), rep); // Integer part, modulo period
  float3 Pi1 = mod(Pi0 + (float3)1.0, rep); // Integer part + 1, mod period
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  float3 Pf0 = frac(P); // Fractional part for interpolation
  float3 Pf1 = Pf0 - (float3)1.0; // Fractional part - 1.0
  float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  float4 iy = float4(Pi0.y, Pi0.y, Pi1.y, Pi1.y);
  float4 iz0 = (float4)Pi0.z;
  float4 iz1 = (float4)Pi1.z;

  float4 ixy = permute(permute(ix) + iy);
  float4 ixy0 = permute(ixy + iz0);
  float4 ixy1 = permute(ixy + iz1);

  float4 gx0 = ixy0 / 7.0;
  float4 gy0 = frac(floor(gx0) / 7.0) - 0.5;
  gx0 = frac(gx0);
  float4 gz0 = (float4)0.5 - abs(gx0) - abs(gy0);
  float4 sz0 = step(gz0, (float4)0.0);
  gx0 -= sz0 * (step((float4)0.0, gx0) - 0.5);
  gy0 -= sz0 * (step((float4)0.0, gy0) - 0.5);

  float4 gx1 = ixy1 / 7.0;
  float4 gy1 = frac(floor(gx1) / 7.0) - 0.5;
  gx1 = frac(gx1);
  float4 gz1 = (float4)0.5 - abs(gx1) - abs(gy1);
  float4 sz1 = step(gz1, (float4)0.0);
  gx1 -= sz1 * (step((float4)0.0, gx1) - 0.5);
  gy1 -= sz1 * (step((float4)0.0, gy1) - 0.5);

  float3 g000 = float3(gx0.x,gy0.x,gz0.x);
  float3 g100 = float3(gx0.y,gy0.y,gz0.y);
  float3 g010 = float3(gx0.z,gy0.z,gz0.z);
  float3 g110 = float3(gx0.w,gy0.w,gz0.w);
  float3 g001 = float3(gx1.x,gy1.x,gz1.x);
  float3 g101 = float3(gx1.y,gy1.y,gz1.y);
  float3 g011 = float3(gx1.z,gy1.z,gz1.z);
  float3 g111 = float3(gx1.w,gy1.w,gz1.w);

  float4 norm0 = taylorInvSqrt(float4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  float4 norm1 = taylorInvSqrt(float4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, float3(Pf1.x, Pf0.y, Pf0.z));
  float n010 = dot(g010, float3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, float3(Pf1.x, Pf1.y, Pf0.z));
  float n001 = dot(g001, float3(Pf0.x, Pf0.y, Pf1.z));
  float n101 = dot(g101, float3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, float3(Pf0.x, Pf1.y, Pf1.z));
  float n111 = dot(g111, Pf1);

  float3 fade_xyz = fade(Pf0);
  float4 n_z = lerp(float4(n000, n100, n010, n110), float4(n001, n101, n011, n111), fade_xyz.z);
  float2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}

float noise( float3 a)
{
	return cnoise(a);
}


// https://www.clicktorelease.com/blog/vertex-displacement-noise-3d-webgl-glsl-three-js/
float turbulence( float3 p )
{
  float w = 100.0;
  float t = -.5;
  for (float f = 1.0 ; f <= 10.0 ; f++ ){
    float power = pow( 2.0, f );
    t += abs( pnoise( float3( power * p ), float3( 10.0, 10.0, 10.0 ) ) / power );
  }
  return t;
}
struct VS_INPUT1
{
	float3 position  : POSITION;
	float2 uv : TEXCOORD0;
};
struct VS_INPUT2 {
    	float3 position	: POSITION;
    	float3 normal	: NORMAL;
    	float3 Binormal	: BINORMAL;
    	float3 Tangent	: TANGENT;
    	float3 Color	: TEXCOORD0;
    	float2 uv	: TEXCOORD1;
};
struct VS_OUTPUT1
{
	float4 hposition : POSITION;
	float2 uv : TEXCOORD0;
};
struct VS_OUTPUT2 {
    	float4 hposition: POSITION;
    	float3 normal	: NORMAL;
    	float3 Binormal	: BINORMAL;
    	float3 Tangent	: TANGENT;
    	float3 Color	: TEXCOORD0;
    	float2 uv	: TEXCOORD1;
};
[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VS_OUTPUT1 main1( VS_INPUT1 IN )
{
	VS_OUTPUT1 OUT;
	OUT.hposition=mul(float4(IN.position, 1), worldViewProj);
    	OUT.uv=IN.uv;
    	return OUT;
}
VS_OUTPUT2 main( VS_INPUT2 IN )
{
	VS_OUTPUT2 OUT;
	OUT.hposition=mul(float4(IN.position, 1), worldViewProj);
    	OUT.normal=IN.normal;
    	OUT.Binormal=IN.Binormal;
    	OUT.Tangent=IN.Tangent;
    	OUT.Color=IN.Color;
    	OUT.uv=IN.uv;
    	return OUT;
}
[Fragment shader]

float turbulence(int octaves, float3 P, float lacunarity, float gain)
{	
  float sum = 0;
  float scale = 1;
  float totalgain = 1;
  for(int i=0;i<octaves;i++){
    sum += totalgain*noise(P*scale);
    scale *= lacunarity;
    totalgain *= gain;
  }
  return abs(sum);
}

#define iCloud 1
#define iFur 2
#define iMarble 3
#define iWood 4
#define iPNoise 5

int iI=6;

float4 main( VS_OUTPUT1 IN ) : COLOR0
{
  float3 v_texCoord3D=IN.hposition.xyz;
  float3 color1, color2, diffuse;
  if(iI==iPNoise)
  {
  	color1 = float3(0.1f, 0.1f, 0.99);
  	color2 = float3(0.8f, 0.8, 0.8f);  
  	float3 diffuse=lerp(color1, color2, cos(v_texCoord3D.z*0.5+2*turbulence(6,v_texCoord3D.xyz,2,0.5))*0.9);
  }
  else if(iI==iCloud)
  {
  	color1 = float3(0.1f, 0.1f, 0.99);
  	color2 = float3(0.8f, 0.8, 0.8f);    
  	diffuse=lerp(color1, color2, cos(v_texCoord3D.z*0.5+2*turbulence(6,v_texCoord3D.xyz,2,0.5))*0.9);
  }
  else if(iI==iFur)
  {
  color1 = float3(0.8, 0.7, 0.0);
  color2 = float3(0.6, 0.1, 0.0);
  diffuse=lerp(color1, color2, clamp(0.7*turbulence(6,v_texCoord3D.xyz,3,0.9), 0,1));
  }
  else if(iI==iMarble)
  {
  	color1 = float3(0.1f, 0.8f, 0.2);
 	color2 = float3(0.8f, 0.8, 0.8f);
      	diffuse=lerp(color1, color2, cos(v_texCoord3D.z*0.1+6*turbulence(5,v_texCoord3D.xyz,2,0.6)));
  }
  else if(iI==iWood)
  {
  	color1 = float3(0.658, 0.356, 0.0588);
  	color2 = float3(0.952, 0.713, 0.470);
  float x_v=v_texCoord3D.x*0.6;
  float y_v=v_texCoord3D.y*0.6;
  float z_v=v_texCoord3D.z*0.6;
  float3 t1=float3(x_v*2,y_v*2,z_v*2);
  float3 t11=float3(x_v*2+1,y_v*2+1,z_v*2+1);
  float3 t2=float3(x_v*0,y_v*0,z_v*0);
  float temp_x=60*noise(t1);
  float temp_y=60*noise(t2);
  x_v=x_v-temp_x;
  y_v=y_v-temp_y;
  diffuse=lerp(color1, color2, pow(cos(sqrt(x_v*x_v+y_v*y_v)+30*turbulence(68, t2, 2, 0.6))*0.5+0.5,3));
  }
  else
  {
  	float n = noise(float3(4.0 * v_texCoord3D.xyz));
  	return float4(0.5 + 0.85 * float3(n, n, n), 1.0);  
  }

  diffuse=GammaCorrect3(diffuse);
  return float4(diffuse, 1.0);
}

