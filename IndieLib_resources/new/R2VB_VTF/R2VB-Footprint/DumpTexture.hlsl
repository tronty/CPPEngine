//-----------------------------------------------------------------------------
// File: DumpTexture.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

struct VsOut {
	float4 Pos: POSITION;
	float2 tc0: TEXCOORD0;
};

VsOut main(float2 pos: POSITION, float2 t0 : TEXCOORD0){
	VsOut Out;

	Out.Pos = float4(pos, 0, 1);
	Out.tc0 = t0;

	return Out;
}


[Fragment shader]
sampler2D DumpTex;

float4 main(float2 t0: TEXCOORD0) : COLOR {
	return tex2D(DumpTex, t0);
}
