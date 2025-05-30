//-----------------------------------------------------------------------------
// File: particles.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/




#define SM3 1

[Vertex shader]

float4x4 mvp;
float3 dx, dy;
//float time;

struct VsOut {
	float4 pos         : POSITION;
	float2 texCoord    : TEXCOORD0;
	float2 effect      : TEXCOORD1;
	float2 groundCoord : TEXCOORD2;
	float4 fireCoord   : TEXCOORD3;
	float3 smokeCoord  : TEXCOORD4;
};

#define PI 3.14159265

// Texture atlas pixel size
#define pixel (1.0 / 1024.0)

#if 1
SAMPLER2D DMAP;
VsOut main(float4 pos_: POSITION, float3 texCoord_: TEXCOORD0){
	float2 texCoord=texCoord_.xy+0.5;
	float4 vertex = SAMPLE2D(DMAP, texCoord);
#else
VsOut main(float4 vertex: POSITION, float3 texCoord: TEXCOORD0){
#endif
	VsOut Out;

	/*
		For all the branches below we set the unused texture coordinates to 0.5. This is done to speed up the
		pixel shader on ps2.0 hardware. Since ps2.0 doesn't have dynamic branching this means all textures will
		be sampled and then the right value is selected. By setting the texture coordinate to a fixed location
		in the middle of the texture we ensure that we don't waste any memory bandwidth but instead ask for the
		exact same texels over and over, which should then constantly be in cache, and we don't pollute the cache
		with other unused texture values.
	*/
	if (vertex.w < 0){
		float3 dirX, dirY;
		if (vertex.w > -1){
			// Foliage
			float cosA = cos(2 * PI * vertex.w);
			float sinA = sin(2 * PI * vertex.w);
			dirX = 0.04 * float3(cosA, 0, sinA);

			/*
				Since we're using a texture atlas we can't rely on clamping on the texture
				edges as they are inside the actual texture, so instead we pad texture coordinate
				with a pixel on both sides to ensure filtering doesn't go outside the subtexture.
			*/
			if (vertex.w > -0.6666){
				if (vertex.w > -0.3333){
					Out.texCoord.x = texCoord.x * 510.0 * pixel + pixel;
				} else {
					Out.texCoord.x = texCoord.x * 510.0 * pixel + pixel + 0.5;
				}
				Out.texCoord.y = texCoord.y * 510.0 * pixel + pixel;
				dirY = 0.04 * float3(0, 1, 0);
			} else {
				// This texture has a bit of unused space at the top, so we modify texCoord and size to save a bit of fillrate.
				Out.texCoord.x = texCoord.x * 126.0 * pixel + pixel;
				Out.texCoord.y = (texCoord.y * 0.75 + 0.25) * 126.0 * pixel + pixel + 0.5;
				dirY = 0.03 * float3(0, 1, 0);
			}
			Out.groundCoord = 0.5 + 0.5 * vertex.xz;
			Out.fireCoord = 0.5;
		} else {
			// Fire
			dirX = 1.5 * dx;
			dirY = 0.1 * float3(0, 1, 0);

			Out.texCoord = 0.5;
			Out.groundCoord = 0.5;
			Out.fireCoord.xy = texCoord;
			Out.fireCoord.y += 7.0 * time;
			Out.fireCoord.z = time;
			Out.fireCoord.w = texCoord.y * 2.5;
		}
		vertex.xyz += (texCoord.x - 0.5) * dirX + (1 - texCoord.y) * dirY;

		Out.smokeCoord = 0.5;
	} else {
		// Smoke
		Out.texCoord = 0.5;
		Out.groundCoord = 0.5;
		Out.smokeCoord.xy = texCoord;
		Out.smokeCoord.z = vertex.w * 0.2; // Scale to 0..1 range
		Out.fireCoord = 0.5;
	}

	Out.effect.x = vertex.w;
	Out.effect.y = vertex.w + 1;

	vertex.w = 1;

	Out.pos = mul(mvp, vertex);

	return Out;
}


[Fragment shader]


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

sampler2D Foliage;
sampler2D Light;
sampler3D Smoke;
//sampler3D Noise;
sampler2D FirePalette;

float4 main(float2 texCoord: TEXCOORD0, float2 effect: TEXCOORD1, float2 groundCoord: TEXCOORD2, float4 fireCoord: TEXCOORD3, float3 smokeCoord : TEXCOORD4) : COLOR {
	if (effect.x < 0){
		if (effect.y >= 0){
			// Foliage
			float4 base = tex2D(Foliage, texCoord);
			base.xyz *= tex2D(Light, groundCoord).xyz;
			return base;
		} else {
			// Fire
			float n = noise(fireCoord.xyz);

			fireCoord.x = fireCoord.w * n;
#if 0//def SM3
			return tex1Dlod(FirePalette, fireCoord);
#elif 0
			return tex1D(FirePalette, fireCoord.x);
#else
			return tex2D(FirePalette, float2(fireCoord.x, 0));
#endif
		}
	} else {
		// Smoke
		float4 smoke = tex3D(Smoke, smokeCoord);
		smoke.w *= 0.4;
		return smoke;
	}
}
