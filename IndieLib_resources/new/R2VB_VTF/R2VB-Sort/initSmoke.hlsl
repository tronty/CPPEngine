//-----------------------------------------------------------------------------
// File: initSmoke.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





[Vertex shader]

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

struct PsOut {
	float4 pos: COLOR0;
	float4 dir: COLOR1;
};

sampler2D InitialPos;

PsOut main(float2 texCoord: TEXCOORD0){
	PsOut Out;

	float4 pos = tex2D(InitialPos, texCoord);

	Out.pos = pos;
	// Initial position somewhere far out of view. They will appear as they enter the next cycle.
	Out.pos.y = 1000;
	Out.dir = float4(0, 0.1, 0, 0);

	return Out;
}
