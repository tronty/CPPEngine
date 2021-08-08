//-----------------------------------------------------------------------------
// File: footprint.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

[Vertex shader]

float4x4 mvp;
float baseDepth;

struct VsOut {
	float4 Pos  : POSITION;
	float3 vpos : TEXCOORD0;
};

#if 1
VsOut main(float4 pos: POSITION){
#else
SAMPLER2D DMAP;
VsOut main(float4 pos_: POSITION, float2 tc0_ : TEXCOORD0){
	float2 tc0=tc0_+0.5;
	float4 pos = SAMPLE2D(DMAP, tc0);
#endif

	VsOut Out;
	float4 tPos;

	Out.Pos = mul(mvp, pos);
	Out.vpos = pos.xyz;

	return Out;
}

[Fragment shader]

sampler2D heightmap;
float4 Param; //(TerrainScale, DepthInSnow, HeightFactor, xxx);
float depthFactor;

float4 main(float3 pos:TEXCOORD0) : COLOR {
	float2 tc2;
	float height;

	// get height value from heightmap sampler
	tc2 = (pos.xz + Param.x)/(Param.x*2.0f);
	height = (tex2D(heightmap, tc2).x*Param.z);
	// using depth as color value
	float r = saturate(((height-pos.y)/Param.y)*depthFactor);

	return float4(r, r, r, 1.0f);
}

