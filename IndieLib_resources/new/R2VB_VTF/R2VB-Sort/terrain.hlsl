//-----------------------------------------------------------------------------
// File: terrain.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float4x4 mvp;

struct VsOut {
	float4 pos:        POSITION;
	float2 texCoord:   TEXCOORD0;
	float2 lightCoord: TEXCOORD1;
};

VsOut main(float4 vertex: POSITION){
	VsOut Out;

	Out.lightCoord = 0.5 + 0.5 * vertex.xz;
	Out.texCoord = Out.lightCoord * 16;
	Out.pos = mul(mvp, vertex);

	return Out;
}

[Fragment shader]

sampler2D Texture;
sampler2D Light;

float4 main(float2 texCoord: TEXCOORD0, float2 lightCoord: TEXCOORD1) : COLOR {
	return float4(0.7, 0.6, 0.4, 0) * tex2D(Light, lightCoord) * tex2D(Texture, texCoord);
}
