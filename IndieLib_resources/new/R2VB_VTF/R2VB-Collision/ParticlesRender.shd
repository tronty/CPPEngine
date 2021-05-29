//-----------------------------------------------------------------------------
// File: Samples\DirectX 9.0\R2VB-Collision\particlesrender.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

[Vertex shader]

float4x4 WorldTransformMatrix;
float3   CameraPosition;
float    PointSizeFactor;

struct vertexShaderInput
{
	float4 Position : POSITION0;
};

struct vertexShaderOutput
{
	float4 Position            : POSITION0;
	float2 Tex0                : TEXCOORD0;
	float  FadeOut             : COLOR;
	float  PointSize           : PSIZE;
};

SAMPLER2D DMAP;
vertexShaderOutput main(float4 pos_: POSITION0, float2 uv_: TEXCOORD0){
	float2 uv=uv_+0.5;
	float4 InputPosition = SAMPLE2D(DMAP, uv);

	vertexShaderOutput Out = (vertexShaderOutput) 0;

	// Transform position
	Out.Position = mul( WorldTransformMatrix, float4(InputPosition.xyz, 1.0) );

	// Texture coordinates are set automatically for point sprites
	Out.Tex0 = 0;

	// Point sprite attenuation function
	float distance = distance(CameraPosition, InputPosition.xyz);
	Out.PointSize = PointSizeFactor / distance;

	// Pass particle's lifetime for fadeout effect
	Out.FadeOut = InputPosition.w;

	return Out;
}

[Fragment shader]
// Compiling this shader as 2.0 so that the color interpolator doesn't get modified 
// for point sprite texture coordinates generation (it does with 3.0 shaders)
//<compile ps_2_0 main>


struct pixelShaderInput
{
	float2 Tex0     : TEXCOORD0;
	float  FadeOut  : COLOR;
};

sampler2D SnowFlakeTexture;

float4 main( pixelShaderInput Input ) : COLOR0
{
	float4 color;
	
	color = tex2D(SnowFlakeTexture, Input.Tex0).x;
	color.a *= clamp(Input.FadeOut, 0, 1);
	return color;
}

