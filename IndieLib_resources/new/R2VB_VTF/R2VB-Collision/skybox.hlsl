//-----------------------------------------------------------------------------
// File: Samples\DirectX 9.0\R2VB-Collision\skybox.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float4x4 mvp;

struct VsOut {
	float4 Pos: POSITION;
	float3 dir: TEXCOORD0;
};

VsOut main(float4 pos: POSITION){
	VsOut Out;

	Out.Pos = mul(mvp, pos);
	Out.dir = pos.xyz;

	return Out;
}

[Fragment shader]

samplerCUBE Skybox;

float4 main(float3 dir: TEXCOORD0) : COLOR {
	return texCUBE(Skybox, dir);
}
