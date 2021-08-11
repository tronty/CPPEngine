//-----------------------------------------------------------------------------
// File: skybox.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





/*
	Simple clip-space skybox shader.
*/

[Vertex shader]

// Define vertex shader outputs
struct VsOut {
	float4 Pos:       POSITION;
	float3 cubeCoord: TEXCOORD0;
};

VsOut main(float4 pos: POSITION, float3 cubeCoord: TEXCOORD0){
	VsOut Out;

	// Use the specified clip-space coordinates directly
	Out.Pos = pos;
	Out.cubeCoord = cubeCoord;

	return Out;
}


[Fragment shader]

samplerCUBE Skybox;
float       maxValue;

float4 main(float3 cubeCoord: TEXCOORD0) : COLOR {
	// Sample cube map
	float4 sky = texCUBE(Skybox, cubeCoord);

	// Expand RGBS to floating point
	sky.rgb *= sky.a * maxValue;

	return sky;
}
