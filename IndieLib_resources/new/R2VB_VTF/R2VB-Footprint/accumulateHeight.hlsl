//-----------------------------------------------------------------------------
// File: accumulateHeight.shd
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

sampler2D currentMap;
sampler2D accumulatedHeightMap;
sampler2D fillMap;
float accumulateSpeed;
float2 random;

float4 main(float2 t0: TEXCOORD0) : COLOR {
	float4 current = tex2D(currentMap, t0);
	float4 maxheight = tex2D(accumulatedHeightMap, t0);
	float4 after = current + tex2D(fillMap, t0+random)*accumulateSpeed;
	after.x = min(maxheight.x, after.x);
	after.y = min(maxheight.y, after.y);
	after.z = min(maxheight.z, after.z);
	
	return after;
}

