//-----------------------------------------------------------------------------
// File: blur.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





/*
	A simple blur filter.
*/

[Vertex shader]

float2 offsets[4];

// Define vertex shader outputs
struct VsOut {
	float4 Pos:			POSITION;
	float2 texCoord0:	TEXCOORD0;
	float2 texCoord1:	TEXCOORD1;
	float2 texCoord2:	TEXCOORD2;
	float2 texCoord3:	TEXCOORD3;
};

VsOut main(float4 pos: POSITION, float2 texCoord: TEXCOORD0) {

	VsOut Out;

	Out.Pos = pos;

	// Calculate all texcoords
	Out.texCoord0 = texCoord + offsets[0];
	Out.texCoord1 = texCoord + offsets[1];
	Out.texCoord2 = texCoord + offsets[2];
	Out.texCoord3 = texCoord + offsets[3];

	return Out;
}


[Fragment shader]

sampler2D	RT;

float4 main(float2 texCoord0: TEXCOORD0, float2 texCoord1: TEXCOORD1,
			float2 texCoord2: TEXCOORD2, float2 texCoord3: TEXCOORD3) : COLOR {

	float4 rt0 = tex2D(RT, texCoord0);
	float4 rt1 = tex2D(RT, texCoord1);
	float4 rt2 = tex2D(RT, texCoord2);
	float4 rt3 = tex2D(RT, texCoord3);

	float4 rt = 0.25 * (rt0 + rt1 + rt2 + rt3);

	return rt;
}
