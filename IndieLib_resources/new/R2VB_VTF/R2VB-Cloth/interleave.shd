//-----------------------------------------------------------------------------
// File: interleave.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





[Vertex shader]

struct VsOut {
	float4 pos:      POSITION;
	float2 texCoord: TEXCOORD0;
	float  select:   TEXCOORD1;
};

float2 halfPixel;
float width;

VsOut main(float4 pos: POSITION){
	VsOut Out;

	Out.pos = pos;
	Out.texCoord = pos.xy * float2(0.5, -0.5) + 0.5 + halfPixel;
	Out.select = width * Out.texCoord.x;

	return Out;
}

[Fragment shader]

sampler2D Position;
sampler2D Tangent;
sampler2D Binormal;
sampler2D Normal;

float4 main(float2 texCoord: TEXCOORD0, float select: TEXCOORD1) : COLOR {
	float t = frac(select);

	if (t < 0.5){
		if (t < 0.25){
			return tex2D(Position, texCoord);
		} else {
			return tex2D(Tangent, texCoord);
		}
	} else {
		if (t < 0.75){
			return tex2D(Binormal, texCoord);
		} else {
			return tex2D(Normal, texCoord);
		}
	}
}
