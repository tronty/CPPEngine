//-----------------------------------------------------------------------------
// File: deformShd.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float4x4 mvp;
float TexFactor;
float HeightFactor;

struct VsOut {
	float4 Pos: POSITION;
	float2 tc0: TEXCOORD0;
};

VsOut main(float2 pos: POSITION, float2 tc0: TEXCOORD0){
	VsOut Out;

	Out.Pos = float4(pos, 0, 1);
	Out.tc0 = tc0;
	return Out;
}

[Fragment shader]

sampler2D deformMap;
float depth;

float4 main(float2 texcoord:TEXCOORD0) : COLOR {
	return min(1.0f, tex2D(deformMap, texcoord)*depth);
}

