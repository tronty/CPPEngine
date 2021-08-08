//-----------------------------------------------------------------------------
// File: batchModel.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

[Vertex shader]

float4x4 mvp;

struct VsOut {
	float4 Pos: POSITION;
	float2 tc0 : TEXCOORD0;
	float3 nor : TEXCOORD1;
};

#if 0
VsOut main(float4 pos: POSITION, float4 nor: NORMAL, float2 tc0 : TEXCOORD0){
#else
SAMPLER2D DMAP;
VsOut main(float4 pos_: POSITION, float4 nor: NORMAL, float2 tc0_ : TEXCOORD0){
	float2 tc0=tc0_+0.5;
	float4 pos = SAMPLE2D(DMAP, tc0);
#endif
	VsOut Out;

	Out.Pos = mul(mvp, pos);
	Out.tc0 = tc0;
	Out.nor.xyz = normalize(nor.xyz);
	
	return Out;
}

[Fragment shader]

sampler2D diffuseMap;
static float3 lightDir = normalize(float3(1, 1, -1));

float4 main(float2 tc0: TEXCOORD0, float3 nor : TEXCOORD1) : COLOR {
	return tex2D(diffuseMap, tc0)*max(0.5f, dot(lightDir, nor));
}

