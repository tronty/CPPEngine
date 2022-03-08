//-----------------------------------------------------------------------------
// File: model.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float4x4 mvp;

struct VsOut {
	float4 Pos: POSITION;
	float2 tc0: TEXCOORD0;
	float3 nor: TEXCOORD1;
};

VsOut main(float4 pos: POSITION, float4 nor: NORMAL0, float2 tc0 : TEXCOORD0){
	VsOut Out;
	float4 tPos;

	Out.Pos = mul(mvp, pos);
	Out.tc0 = tc0;
	nor.w = 0.0f;
	Out.nor = normalize(nor);

	return Out;
}

[Fragment shader]

sampler2D diffuseMap;
float4 LightColor;
float4 LightDir;

float4 main(float2 tc0: TEXCOORD0, float3 nor: TEXCOORD1) : COLOR {
	float4 ambient = float4(0.05, 0.05, 0.2, 1);
	float4 diffuse = dot(-LightDir, nor)*LightColor;
	return tex2D(diffuseMap, tc0)*saturate(ambient+diffuse);
}

