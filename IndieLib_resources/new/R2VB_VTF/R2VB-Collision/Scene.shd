//-----------------------------------------------------------------------------
// File: Samples\DirectX 9.0\R2VB-Collision\Scene.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

[Vertex shader]

float4x4 WorldTransformMatrix;

struct vertexShaderInput
{
	float4 Position : POSITION0;
};

struct vertexShaderOutput
{
	float4 Position            : POSITION0;
	float2 Tex0                : TEXCOORD0;
	float4 Color               : COLOR0;
};

SAMPLER2D DMAP;
vertexShaderOutput main(float4 pos_: POSITION0, float2 uv_: TEXCOORD0){
	float2 uv=uv_+0.5;
	float4 InputPosition = SAMPLE2D(DMAP, uv);
	vertexShaderOutput Out = (vertexShaderOutput) 0;

	// Transform position
	Out.Position = mul( WorldTransformMatrix, InputPosition );

	Out.Color = lerp(float4(0,0.2,0,1), float4(0,1,0,1), InputPosition.y/100.0f);

	Out.Tex0 = float2(InputPosition.x, InputPosition.z) / 1000 + 0.5f;

	return Out;
}

[Fragment shader]

struct pixelShaderInput
{
	float4 Diffuse  : COLOR0;
	float2 Tex0     : TEXCOORD0;
};


sampler2D TerrainDiffuseTexture;

float4 main( pixelShaderInput Input ) : COLOR0
{
	return tex2D(TerrainDiffuseTexture, Input.Tex0);
}

