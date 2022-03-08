//-----------------------------------------------------------------------------
// File: initPhysics.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

struct VsOut {
	float4 pos: POSITION;
	float2 texCoord: TEXCOORD0;
};

float2 halfPixel;

VsOut main(float4 pos: POSITION){
	VsOut Out;

	Out.pos = pos;
	Out.texCoord = pos.xy * float2(0.5, -0.5) + 0.5 + halfPixel;

	return Out;
}

[Fragment shader]

struct PsOut {
	float4 pos: COLOR0;
	float4 dir: COLOR1;
};

sampler2D Rand;

PsOut main(float2 texCoord: TEXCOORD0){
	PsOut Out;

	float rand0 = tex2D(Rand, -texCoord).w;
	float rand1 = tex2D(Rand, 10.2428 * texCoord).w;
	float rand2 = tex2D(Rand, -4.724 * texCoord).w;
	float rand3 = tex2D(Rand, 0.5128 * texCoord).w;

	// Create a higher precision random value from the four 16bit values
	float rand = frac(8.0 * rand0 + 4.0 * rand1 + 2.0 * rand2 + rand3);

	Out.pos = float4(0, 0, 0, 2.0 * rand);
	Out.dir = float4(0, 0, 0, 0);

	return Out;
}

