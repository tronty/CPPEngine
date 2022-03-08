//-----------------------------------------------------------------------------
// File: physics.shd
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

sampler2D Pos;
sampler2D Dir;
sampler2D Rand;

float3 sprinkleDir;
float frameTime;

PsOut main(float2 texCoord: TEXCOORD0){
	PsOut Out;

	// Sample current state
	float4 pos = tex2D(Pos, texCoord);
	float4 dir = tex2D(Dir, texCoord);

	float4 rand = tex2D(Rand, texCoord) * 2.0 - 1.0;

	pos.w += frameTime;

	// A particle will live for 2.0 seconds
	if (pos.w >= 2.0){
		// Restart time
		pos.w -= 2.0;
		// Compute new position and direction
		dir.xyz = sprinkleDir + 18.0 * rand.xyz;
		pos.xyz = pos.w * dir;
	} else {
		// Make the particles bounce on the floor
		if (pos.y < 0){
			dir.y = -0.7 * dir.y;
		}
		pos.y = abs(pos.y);

		// Gravity
		dir.y -= 450.0 * frameTime;

		// Update particle position
		pos.xyz += frameTime * dir;
	}

	Out.pos = pos;
	Out.dir = dir;

	return Out;
}

