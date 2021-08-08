//-----------------------------------------------------------------------------
// File: makeQuads.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float width;

struct VsOut {
	float4 pos: POSITION;
	float2 texCoord: TEXCOORD0;
};

float2 halfPixel;

VsOut main(float4 pos: POSITION){
	VsOut Out;

	Out.pos = pos;
	Out.texCoord = pos.xy * float2(0.5, -0.5) + 0.5 + halfPixel;

	return Out;
}

[Fragment shader]

sampler2D Pos;

float4 main(float2 texCoord: TEXCOORD0) : COLOR {
	// Just sample and output. The texture coordinates passed
	// will make it so that each pixel is duplicated four times.
	return tex2D(Pos, texCoord);
}

