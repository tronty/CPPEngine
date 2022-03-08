//-----------------------------------------------------------------------------
// File: floor.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float4x4 mvp;

struct VsOut {
	float4 Pos: POSITION;
	float2 texCoord: TEXCOORD0;
};

VsOut main(float4 pos: POSITION){
	VsOut Out;

	Out.Pos = mul(mvp, pos);
	Out.texCoord = pos.xz * 0.01;

	return Out;
}

[Fragment shader]

sampler2D Base;

float4 main(float2 texCoord: TEXCOORD0) : COLOR {
	return 0.3 * tex2D(Base, texCoord);
}

