//-----------------------------------------------------------------------------
// File: vsModel.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

float4x4 boneMatrix[48] ;
float4x4 mvp;
float4x4 trans;


struct VsOut {
	float4 Pos: POSITION;
	float2 tc0: TEXCOORD0;
	float3 nor: TEXCOORD1;
};

VsOut main(float4 pos0: POSITION0, float4 nor : TEXCOORD0, float2 tc0: TEXCOORD1, float4 bi: NORMAL0, float4 wi: TANGENT){
	float4 tPos, rPos, tNor;
	VsOut Out;

	rPos.xyz = mul(pos0, boneMatrix[bi.x]).xyz*wi.x;
	if (wi.y > 0.0)
		rPos.xyz += mul(pos0, boneMatrix[bi.y]).xyz*wi.y;
	if (wi.z > 0.0)
		rPos.xyz += mul(pos0, boneMatrix[bi.z]).xyz*wi.z;
	if (wi.w > 0.0)	
		rPos.xyz += mul(pos0, boneMatrix[bi.w]).xyz*wi.w;
	rPos.w = 1.0f;
	tPos = mul(trans, rPos);
	Out.Pos = mul(mvp, tPos);
	Out.tc0 = tc0;
	tNor = mul(nor, boneMatrix[bi.x]);
	Out.nor.xyz = normalize(mul(trans, tNor).xyz);

	return Out;
}

[Fragment shader]

sampler2D diffuseMap;
static float3 lightDir = normalize(float3(1, 1, -1));

float4 main(float2 tc0: TEXCOORD0, float3 nor: TEXCOORD1) : COLOR {
	return tex2D(diffuseMap, tc0)*max(0.5f, dot(lightDir, nor));
}

