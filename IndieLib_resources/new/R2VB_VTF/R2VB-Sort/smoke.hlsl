//-----------------------------------------------------------------------------
// File: smoke.shd
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------



#define SM3 1


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

sampler2D Pos;
sampler2D Dir;
sampler2D InitialPos;
sampler2D Ground;

float frameTime;
// Wind, gravity and friction
float4 force;

PsOut main(float2 texCoord: TEXCOORD0){
	PsOut Out;

	float4 pos = tex2D(Pos, texCoord);

	pos.w += frameTime;

	// Smoke particles life for 5 seconds
	if (pos.w > 5.0){
		float4 iPos = tex2D(InitialPos, texCoord);

		// Pseudo-random value
		float r = frac(dot(iPos, float4(11.4982, 5.815298, 8.149287, 3.284891)));

		Out.dir = float4(0, 0.14 + 0.03 * r, 0, 0);
		pos.xyz = iPos.xyz;
		pos.w -= 5.0;
	} else {
		float4 dir = tex2D(Dir, texCoord);

		// Wind, gravity and friction
		dir.xyz *= force.w;
		dir.xyz += force.xyz;

		pos.xyz += frameTime * dir.xyz;

		// Collision detect against the ground
#ifdef SM3
		float ground = tex2Dlod(Ground, pos.xzyw * 0.5 + 0.5).x;
#else
		float ground = tex2D(Ground, pos.xz * 0.5 + 0.5).x;
#endif
		ground = ground * 0.4 - 0.2;
		if (pos.y < ground){
			pos.y = ground;
		}

		Out.dir = dir;
	}

	Out.pos = pos;

	return Out;
}
