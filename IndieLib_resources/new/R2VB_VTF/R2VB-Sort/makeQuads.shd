//-----------------------------------------------------------------------------
// File: makeQuads.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





[Vertex shader]

struct VsOut {
	float4 pos: POSITION;
	float2 texCoord: TEXCOORD0;
	float2 dir: TEXCOORD1;
};

float2 halfPixel;
float width;

VsOut main(float4 pos: POSITION){
	VsOut Out;

	Out.pos = pos;
	Out.texCoord = pos.xy * float2(0.5, -0.5) + 0.5 + halfPixel;

	Out.dir.x = Out.texCoord.x * width;
	Out.dir.y = Out.texCoord.x * width + 0.25;

	return Out;
}

[Fragment shader]

sampler2D Pos;
sampler2D Dist;

float3 dx, dy;

float4 main(float2 texCoord: TEXCOORD0, float2 dir: TEXCOORD1) : COLOR {
	float4 pos  = tex2D(Pos, texCoord);
	float4 dist = tex2D(Dist, texCoord);

	if (pos.w >= 0){
		// This will cycle as (1, 1), (1, -1), (-1, -1), (-1, 1)
		float2 tc = (frac(dir) < 0.5)? 1 : -1;

		/*
			R300 and R420 can only access one texture as a vertex buffer at a time.
			For this reason we have to do this work here in the pixel shader for the
			smoke instead of in the vertex shader in particles.shd so we only need
			to access the position texture there.
		*/
		float cosA = cos(dist.y * pos.w);
		float sinA = sin(dist.y * pos.w);
		float3 dirX = cosA * dx - sinA * dy;
		float3 dirY = sinA * dx + cosA * dy;

		pos.xyz += tc.x * dirX + tc.y * dirY;
		pos.y += 0.02;
	}

	return pos;
}
