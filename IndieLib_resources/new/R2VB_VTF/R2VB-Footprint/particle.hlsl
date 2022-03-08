//-----------------------------------------------------------------------------
// File: particle.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float4x4 mvp;
float3 dx, dy;

struct VsOut {
	float4 pos: POSITION;
	float2 texCoord: TEXCOORD0;
	float color: TEXCOORD1;
};

VsOut main(float4 vertex: POSITION, float2 quadCoord: TEXCOORD0){
	VsOut Out;

	Out.texCoord.xy = quadCoord;

	float2 qc = quadCoord * 2.0 - 1.0;
	vertex.xyz += qc.x * dx + qc.y * dy;

	Out.color = 1.0f - (0.125f * vertex.w);
	vertex.w = 1;

	Out.pos = mul(mvp, vertex);

	return Out;
}

[Fragment shader]

sampler2D Particle;

float4 main(float2 texCoord: TEXCOORD0, float color: TEXCOORD1) : COLOR {
	return tex2D(Particle, texCoord.xy)*color;
}

