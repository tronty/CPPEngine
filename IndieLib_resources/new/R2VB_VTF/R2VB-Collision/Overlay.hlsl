//-----------------------------------------------------------------------------
// File: Samples\DirectX 9.0\R2VB-Collision\Overlay.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float4x4 mvp;
struct VsOut {
	float4 Pos: POSITION;
	float2 uv: TEXCOORD0;
};

VsOut main(float4 pos: POSITION, float2 uv: TEXCOORD0)
{
	VsOut Out;
	Out.Pos = mul(mvp, pos);
	Out.uv = uv;
	return Out;
}

[Fragment shader]

sampler2D Sampler0;

float4 main( in float2 Texcoord0 : TEXCOORD0 ) : COLOR0
{
	float4 color = tex2D( Sampler0, Texcoord0 );
	float height = color.y / 100.0f;
	return float4(height, height, height, 0.9);
}

