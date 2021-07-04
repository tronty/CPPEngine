//-----------------------------------------------------------------------------
// File: terrainGen.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

struct VsOut {
	float4 Pos: POSITION;
	float2 tc0: TEXCOORD0;
};

VsOut main(float2 pos: POSITION){
	VsOut Out;

	Out.Pos = float4(pos, 0, 1);
	Out.tc0 = pos * float2(0.5, -0.5) + 0.5;

	return Out;
}


[Fragment shader]

sampler2D Tex;
sampler2D Tex2;
float time;

float4 main(float2 t0: TEXCOORD0) : COLOR {
	return lerp(tex2D(Tex, t0), tex2D(Tex2, t0), time);
}

