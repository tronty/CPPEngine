//-----------------------------------------------------------------------------
// File: average.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





/*
	This shader samples the blur texture and writes it into a render target
	to be mipmapped to find the average in the 1x1 level.
*/

[Vertex shader]

// Define vertex shader outputs
struct VsOut {
	float4 Pos:			POSITION;
	float2 texCoord:	TEXCOORD0;
};

VsOut main(float4 pos: POSITION, float2 texCoord: TEXCOORD0){
	VsOut Out;

	Out.Pos = pos;
	Out.texCoord = texCoord;

	return Out;
}


[Fragment shader]


sampler2D RT;

float4 main(float2 texCoord: TEXCOORD0) : COLOR {
	return tex2D(RT, texCoord);
}
