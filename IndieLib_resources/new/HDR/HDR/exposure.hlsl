//-----------------------------------------------------------------------------
// File: exposure.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





/*
	This shader computes the exposure using the average value found in 1x1 mipmap level
*/

[Vertex shader]

// Define vertex shader outputs
struct VsOut {
	float4 Pos:			POSITION;
	float4 texCoord:	TEXCOORD0;
};

VsOut main(float4 pos: POSITION, float2 texCoord: TEXCOORD0){
	VsOut Out;

	Out.Pos = pos;
	Out.texCoord = float4(texCoord, 0, 10.0);

	return Out;
}


[Fragment shader]

sampler2D Average;
sampler2D Exposure;

float frameTime;

float4 main(float4 texCoord: TEXCOORD0) : COLOR {
	// Sample 1x1 mipmap level
	float3 average = tex2Dbias(Average, texCoord).rgb;
	float oldExp = tex2D(Exposure, texCoord.xy).x;

	// Compute new exposure value.
	// Multiply with 1.4427 to remap exp() to exp2() in final pass to improve performance.
	float newExp = 1.4427 / (0.5 + dot(average, 128.0));

	return lerp(oldExp, newExp, frameTime);
}
