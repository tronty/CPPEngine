//-----------------------------------------------------------------------------
// File: generateHeight.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

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

sampler2D heightmap;

float4 main(float2 t0: TEXCOORD0) : COLOR {
	return tex2D(heightmap, t0);
}
