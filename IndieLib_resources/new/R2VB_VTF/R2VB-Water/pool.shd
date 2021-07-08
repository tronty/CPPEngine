//-----------------------------------------------------------------------------
// File: pool.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float4x4 mvp;

struct VsOut {
	float4 Pos:      POSITION;
	float2 texCoord: TEXCOORD0;
	float  color:    TEXCOORD1;
};

VsOut main(float4 pos: POSITION, float2 texCoord: TEXCOORD0, float color: TEXCOORD1){
	VsOut Out;

	Out.Pos = mul(mvp, pos);
	Out.texCoord = texCoord;
	Out.color = color;

	return Out;
}

[Fragment shader]

sampler2D Base;

float4 main(float2 texCoord: TEXCOORD0, float color: TEXCOORD1) : COLOR {
	return color * tex2D(Base, texCoord);
}

