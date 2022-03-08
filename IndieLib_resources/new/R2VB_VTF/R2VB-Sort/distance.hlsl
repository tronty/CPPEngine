//-----------------------------------------------------------------------------
// File: distance.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





[Vertex shader]

struct VsOut {
	float4 pos: POSITION;
	float3 texCoord: TEXCOORD0;
};

float2 halfPixel;
float2 scaleBias;

VsOut main(float4 pos: POSITION){
	VsOut Out;

	Out.texCoord.xy = pos.xy * float2(0.5, -0.5) + 0.5 + halfPixel;
	Out.texCoord.z = 0;

	pos.y = pos.y * scaleBias.x + scaleBias.y;
	Out.pos = pos;

	return Out;
}

[Fragment shader]

struct PsOut {
	float4 pos:  COLOR0;
	float4 dist: COLOR1;
};

sampler2D Tex;

float3 camPos;
float3 zDir;

PsOut main(float3 texCoord: TEXCOORD0){
	PsOut Out;

	float4 pos = tex2D(Tex, texCoord.xy);
	float3 d = pos.xyz - camPos;

	Out.pos = pos;
	Out.dist.x = dot(d, d);
	// Pseudo-random rotation for the smoke particles
	Out.dist.y = frac(dot(texCoord, float3(4.18237, 12.528723, 1))) * 4 - 2;
	Out.dist.zw = 0;

	return Out;
}
