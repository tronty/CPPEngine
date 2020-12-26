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
//--------------------------------------------------------------------------------------
// File: BasicVTF.fx
//
// The effect file for the BasicVTF sample.  
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
#define FPTYPE half
#define FPTYPE2 half2
#define FPTYPE3 half3
#define FPTYPE4 half4

float4x4 g_mWorld;                  // World matrix for object
float4x4 g_mWorldIT;
float4x4 g_mWorldView;
float4x4 g_mViewProj;
float4x4 g_mProjection;
float4x4 g_mWorldViewProjection;    // World * View * Projection matrix
float4x4 g_mViewInverse;
float4x4 g_mExposureWorldViewOrthoProj;
float SceneWidth;
float SceneHeight;
sampler2D MeshTextureSampler;
sampler2D ExposureDepthMapSampler;
sampler3D NoiseSampler;

float3 g_LightDir=float3(0.5, 0.8, 0.5);               // Light's direction in world space
float4 g_MaterialDiffuseColor=float4(1,1,1,1);      // Material's diffuse color
float4 g_LightDiffuse=float4(1,1,1,1);          // Light's diffuse color
float4 g_LightAmbient=float4(0.3,0.3,0.3,0.0);              // Light's ambient color
float3 g_upObjectSpace=float3(0,1,0);
float4 g_SnowColor=float4(0.90,0.95,1.0,1.0);
float OffsetAmount = 0;
float SampleDistance = 0;
float BaseSnowPct = 0.4;
float SpecExpon = 120.0;
float Ks = 0.26;
float Bumpy = 1.0;
float Kd = 0.9;
float SnowBias = 0.02;
float normalDistortionAmount = 0.1;
float dotNormalDistortionAmount = 0.2;

//--------------------------------------------------------------------------------------
// Textures & samplers
//--------------------------------------------------------------------------------------
#if 0
texture g_pTerrainTexture;              // Color texture for mesh
texture g_ExposureDepthMapTexture;              // Color texture for mesh
texture g_Noise3D;

sampler MeshTextureSampler = 
sampler_state
{
    Texture = <g_pTerrainTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler ExposureDepthMapSampler = 
sampler_state
{
	Texture = <g_ExposureDepthMapTexture>;
	AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = NONE;
    MINFILTER = POINT;
    MAGFILTER = POINT;
};

sampler ExposureDepthMapSamplerPixel = 
sampler_state
{
	Texture = <g_ExposureDepthMapTexture>;
	AddressU  = CLAMP;        
    AddressV  = CLAMP;
    AddressW  = CLAMP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

sampler NoiseSampler = 
sampler_state
{
	Texture = <g_Noise3D>;
	AddressU  = WRAP;        
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};
#endif

// Rather than stick everything in this file, pulled some util functions out into other files.

#define EXPOSURESAMPLER_VERTEX ExposureDepthMapSampler
#define EXPOSURESAMPLER_PIXEL ExposureDepthMapSamplerPixel
#define NOISESAMPLER NoiseSampler
#define NOISEOCTAVES 3.0
/*
	A couple of functions used by the different classes of snow techniques.
*/

/*
	Implements a perlin-style noise lookup using a 3D texture.  Returns a (-1..1) value
*/

#ifdef NOISESAMPLER
#ifndef NOISEOCTAVES
#define NOISEOCTAVES 3.0
#endif
FPTYPE noise(FPTYPE3 uvw)
{
	// $$ Note, these values, are tweakable.  I tweaked a little off the standard because I liked these better. YMMV
	FPTYPE sum = 0;
	FPTYPE amp = 1;
	FPTYPE3 p = uvw;
	FPTYPE lacunarity = 2;
	FPTYPE gain = 0.75;
	int octaves = (int)NOISEOCTAVES;
	for(int i=0;i<octaves;i++)
	{
		//sum += amp * abs(tex3D(NOISESAMPLER, p.xyz));
		sum += amp * abs(turbulence(p.xyz));
		p *= lacunarity;
		amp *= gain;
	}
	return 2*sum-1;
}
#endif

/*
	This is used by the vertex shader in the SM3.0 technique to get the exposure for a vertex.
*/
#ifdef EXPOSURESAMPLER_VERTEX
float exposureAtVertex(float2 uv, float depth)
{
	float exposure = 0;
	if(uv.x > 0.0 && uv.x < 1.0 && uv.y > 0.0 && uv.y < 1.0 ) 
	{
		float sampledepth = tex2Dlod(EXPOSURESAMPLER_VERTEX,float4(uv.xy,0,0));
		if(depth < sampledepth+SnowBias)	// SnowBias is for depth fighting artifacts
		{
			exposure = 1;
		}
	}
	return exposure;
}
#endif

#ifdef EXPOSURESAMPLER_PIXEL
FPTYPE exposureAtPixel(FPTYPE2 uv, FPTYPE depth)
{
	FPTYPE exposure = 0;
	FPTYPE sampledepth = tex2D(EXPOSURESAMPLER_PIXEL,FPTYPE4(uv.xy,0,0));
	if(depth < sampledepth+SnowBias)	// SnowBias is for depth fighting artifacts
	{
		exposure = 1;
	}
	return exposure;
}
#endif

/*
	Lights a pixel using the exposure map, and noise to produce a really nice effect.
*/
FPTYPE4 SnowPixel(FPTYPE2 TextureUV, FPTYPE3 Normal, FPTYPE3 OrthoUVnDepth, FPTYPE3 ToView,
	const bool bNoiseNormal, const bool bNoiseIncline, const bool bMultiSample)
{
	// Prescale UV for noise, to have it tile a little more.
	FPTYPE3 noiseUVW = 30.0*OrthoUVnDepth.xyz;
	
	// Then just deterministically remap the coords to get 2 noise values to offset the normal
	FPTYPE3 noiseVec = FPTYPE3(noise(noiseUVW),0,noise(noiseUVW.yzx));
	
    // Calculate the per pixel exposure value.  multi-sampling around us to average
    FPTYPE exposure = 0;
    FPTYPE pixelOrthoDepth = OrthoUVnDepth.z;
	
	// current position
	exposure += exposureAtPixel(OrthoUVnDepth,pixelOrthoDepth);	

	if(bMultiSample)	// We want to sample the exposure map a couple times to blur the egdes a little
	{
		// A box around us.
		FPTYPE2 direction;	
		FPTYPE2 texelSize = SampleDistance / FPTYPE2(SceneWidth,SceneHeight);    
		direction = FPTYPE2(0,1);
		exposure += exposureAtPixel(OrthoUVnDepth.xy+direction*texelSize,pixelOrthoDepth);	
		direction = FPTYPE2(1,0);
		exposure += exposureAtPixel(OrthoUVnDepth.xy+direction*texelSize,pixelOrthoDepth);	
		direction = FPTYPE2(0,-1);
		exposure += exposureAtPixel(OrthoUVnDepth.xy+direction*texelSize,pixelOrthoDepth);	
		direction = FPTYPE2(-1,0);
		exposure += exposureAtPixel(OrthoUVnDepth.xy+direction*texelSize,pixelOrthoDepth);	
		
		exposure /= 5;
	}
	
	FPTYPE3 vNormalWorldSpace = ((2*Normal)-FPTYPE3(1,1,1));
	if(bNoiseNormal) vNormalWorldSpace = normalize(normalDistortionAmount*noiseVec + vNormalWorldSpace);	
	
	FPTYPE dotNorm = vNormalWorldSpace.y; // effectively dot the normal with the up vector
	if(dotNorm <= 0) dotNorm = 0;
	else 
	{
		if(bNoiseIncline)
		{
			dotNorm = saturate(dotNorm+dotNormalDistortionAmount*noiseVec.x);
		}
	}

    FPTYPE3 Vn = normalize(2*ToView-1);
    FPTYPE3 Hn = normalize(Vn + g_LightDir);
    FPTYPE hdn = dot(Hn,vNormalWorldSpace);
    FPTYPE ldn = dot(g_LightDir,vNormalWorldSpace);
    FPTYPE4 litVec = lit(ldn,hdn,SpecExpon);
    FPTYPE3 diffContrib = litVec.y * g_LightDiffuse;
    
    // $$4 Note this is calculated before the full exposure and is thus not based on incline...
    //		This is because we dont want to tie the specular highlight to the noise function too directly, it doesn't look good.
    FPTYPE3 specContrib = exposure*((litVec.y * litVec.z * Ks) * g_LightDiffuse);
    
    // now also tweak based on angle of incline, add in a base %
	exposure = BaseSnowPct + exposure*dotNorm*(1-BaseSnowPct);

    // choose a color based on exposure
    FPTYPE3 diffuseColor = exposure*g_SnowColor + (1-exposure)*tex2D(MeshTextureSampler, TextureUV);    
	
	FPTYPE diffuseSpec = saturate(g_LightAmbient+Kd)*diffContrib+Ks*specContrib;
	return FPTYPE4(diffuseColor.xyz*(diffuseSpec),1);
}



//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
    float3 Normal	  : TEXCOORD1;		// vertex normal    
    float3 OrthoUVnDepth	  : TEXCOORD2;
    float3 ToView		: TEXCOORD3;
};

struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float3 BiNormal : BINORMAL;
	float3 Tangent : TANGENT;
	float3 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
};

VS_OUTPUT RenderSceneVS_NoSnow( VS_INPUT IN)
{
	float4 vPos=float4(IN.Pos,1);
        float3 vNormal=IN.Normal;
       	float2 vTexCoord0=IN.uv;

    VS_OUTPUT Output;
    float3 vNormalWorldSpace;
	vNormalWorldSpace = normalize(mul(vNormal, (float3x3)g_mWorldIT)); // normal (world space)    
    
    // Transform the position from world space to homogeneous projection space
	Output.Position = mul(vPos, g_mWorldViewProjection);
    
    Output.Normal = 0.5*vNormalWorldSpace+float3(0.5,0.5,0.5);
    
    // Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0;     
    
    Output.OrthoUVnDepth=float3(0,0,0);
    
    float4 wsP = mul(vPos,g_mWorld);
    Output.ToView = (0.5*normalize(g_mViewInverse[3].xyz - wsP))+0.5.xxx;	// obj coords
    
    return Output;
}

VS_OUTPUT RenderSceneVS_SM20( VS_INPUT IN)
{
	float4 vPos=float4(IN.Pos,1);
        float3 vNormal=IN.Normal;
       	float2 vTexCoord0=IN.uv;
    VS_OUTPUT Output;
    float3 vNormalWorldSpace;
    
    Output = RenderSceneVS_NoSnow(vPos,vNormal,vTexCoord0);

	// $ Vertex texture is used to pull out the exposure of the point
    float4 orthoPos = mul(vPos,g_mExposureWorldViewOrthoProj);
    orthoPos.y *= -1;	// UV coords have an inverted Y axis
    float2 uv = 0.5*orthoPos.xy + float2(0.5,0.5);
    
    Output.OrthoUVnDepth = float3(uv,orthoPos.z);
    
    return Output;    
}

VS_OUTPUT RenderSceneVS_SM30( VS_INPUT IN)
{
	float4 vPos=float4(IN.Pos,1);
        float3 vNormal=IN.Normal;
       	float2 vTexCoord0=IN.uv;
    VS_OUTPUT Output;
    float3 vNormalWorldSpace;

	// $ Vertex texture is used to pull out the exposure of the point
    float4 orthoPos = mul(vPos,g_mExposureWorldViewOrthoProj);
    orthoPos.y *= -1;	// UV coords have an inverted Y axis
    float2 uv = 0.5*orthoPos.xy + float2(0.5,0.5);
    
    Output.OrthoUVnDepth = float3(uv,orthoPos.z);
    
    float XSD = SampleDistance/SceneWidth;
    float YSD = SampleDistance/SceneHeight;
    float exposure = 0;
	
	// current position
	exposure += exposureAtVertex(uv,orthoPos.z);	
	
	// A box around us.  Capped to 3 more, since only 4 VTF per VS...
	exposure += exposureAtVertex(uv+float2(XSD,YSD),orthoPos.z);	
	exposure += exposureAtVertex(uv+float2(-XSD,YSD),orthoPos.z);	
	exposure += exposureAtVertex(uv+float2(0,-YSD),orthoPos.z);	
	
	exposure /= 4;
	
	// $ Note these two operations are independant of depth, so mask load lag.
    // Transform the normal from object space to world space    
    vNormalWorldSpace = normalize(mul(vNormal, (float3x3)g_mWorldIT)); // normal (world space)
    
    float normalDotUp = saturate(dot(vNormalWorldSpace,float3(0,1,0)));
//	exposure = exposure*pow(normalDotUp,10);
	exposure = exposure*normalDotUp;
  
  	// $ Offset the vertex a little if it is accumulating snow
  	float4 wsP = mul(vPos,g_mWorld);
  	float4 wsP2 = float4(wsP.xyz + OffsetAmount*exposure*vNormalWorldSpace,vPos.w);
  	
    // Transform the position from world space to homogeneous projection space
    Output.Position = mul(wsP2, g_mViewProj);
    
    Output.Normal = 0.5*vNormalWorldSpace+float3(0.5,0.5,0.5);
    
    // Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 
    
    Output.ToView = (0.5*normalize(g_mViewInverse[3].xyz - wsP))+0.5.xxx;	// obj coords
    
    return Output;    
}

float4 RenderScenePS_NoSnow( VS_OUTPUT In )  : COLOR
{
	FPTYPE3 vNormalWorldSpace = (2*In.Normal)-FPTYPE3(1,1,1);	

    FPTYPE3 Vn = normalize(2*In.ToView-1);
    FPTYPE3 Hn = normalize(Vn + g_LightDir);
    FPTYPE hdn = dot(Hn,vNormalWorldSpace);
    FPTYPE ldn = dot(g_LightDir,vNormalWorldSpace);
    FPTYPE4 litVec = lit(ldn,hdn,SpecExpon);
    FPTYPE3 diffContrib = litVec.y * g_LightDiffuse;
    
    FPTYPE3 specContrib = ((litVec.y * litVec.z * Ks) * g_LightDiffuse);
    
    // choose a color based on exposure
    FPTYPE3 diffuseColor = tex2D(MeshTextureSampler, In.TextureUV);    
	
	FPTYPE diffuseSpec = saturate(g_LightAmbient+Kd)*diffContrib+Ks*specContrib;
	
	return float4(diffuseColor.xyz*(diffuseSpec),1);
}    


float4 RenderScenePS( VS_OUTPUT In)  : COLOR
{ 
   return SnowPixel(In.TextureUV,In.Normal,In.OrthoUVnDepth,In.ToView,true,true,true);
}

//--------------------------------------------------------------------------------------
// Renders scene to render target
//--------------------------------------------------------------------------------------
[techniques]
<Textures>
	<Texture sampler="MeshTextureSampler" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
	<Texture sampler="ExposureDepthMapSampler" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
	<Texture sampler="ExposureDepthMapSamplerPixel" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
	<Texture sampler="NoiseSampler" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
</Textures>
<Techniques>
<technique name="RenderScene_SM20">
    <pass name="P0">
        <VertexShader profile="vs_2_0" name="RenderSceneVS_SM20" />
        <PixelShader profile="ps_2_a" name="RenderScenePS" />
    </pass>
</technique>
<technique name="RenderScene_SM30">
    <pass name="P0">
        <VertexShader profile="vs_3_0" name="RenderSceneVS_SM30" />
        <PixelShader profile="ps_3_0" name="RenderScenePS" />
    </pass>
</technique>
<technique name="RenderScene_NoSnow">
    <pass name="P0">
        <VertexShader profile="vs_3_0" name="RenderSceneVS_NoSnow" />
        <PixelShader profile="ps_3_0" name="RenderScenePS_NoSnow" />
    </pass>
</technique>
</Techniques>

