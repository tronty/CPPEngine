//-----------------------------------------------------------------------------
// File: skinning.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

[Vertex shader]

struct VsOut {
	float4 Pos: POSITION;
	float2 tc0: TEXCOORD0;
};

VsOut main(float4 pos: POSITION, float2 t0 : TEXCOORD0){
	VsOut Out;

	Out.Pos = pos;
	Out.tc0 = t0;

	return Out;
}


[Fragment shader]

sampler2D skinningVertex;
sampler2D vertexBoneIndex;
sampler2D vertexWeight;
sampler2D boneMatrix;

float4 bias;
float xBias;

float4x4 getMatrix(sampler2D s, float mi)
{
	float4x4 m;
	float4 XOff;

	XOff = mi + bias;

	m[0] = tex2Dlod(s, float4(XOff.x, 0.0f, 0, 0));
	m[1] = tex2Dlod(s, float4(XOff.y, 0.0f, 0, 0));
	m[2] = tex2Dlod(s, float4(XOff.z, 0.0f, 0, 0));
	m[3] = tex2Dlod(s, float4(XOff.w, 0.0f, 0, 0));

	return m;
}

float4 main(float2 t0: TEXCOORD0) : COLOR {
	float4 Out;
	float4x4 M;

	t0.x += xBias;

	float4 Index = tex2D(vertexBoneIndex, t0); // get bone index.
	float4 Vertex = tex2D(skinningVertex, t0); // get vertex position or normal.
	float4 Weight = tex2D(vertexWeight, t0); // get vertex weight.

	M = getMatrix(boneMatrix, Index.x); // get bone matrix indexed by bone index 0.
	Out.xyz = mul(Vertex, M).xyz*Weight.x;

	if (Weight.y > 0.0)
	{
		M = getMatrix(boneMatrix, Index.y); // get bone matrix indexed by bone index 1.
		Out.xyz += mul(Vertex, M).xyz*Weight.y;
	}
	if (Weight.z > 0.0)
	{
		M = getMatrix(boneMatrix, Index.z); // get bone matrix indexed by bone index 2.
		Out.xyz += mul(Vertex, M).xyz*Weight.z;
	}
	if (Weight.w > 0.0)
	{
		M = getMatrix(boneMatrix, Index.w); // get bone matrix indexed by bone index 3.
		Out.xyz += mul(Vertex, M).xyz*Weight.w;
	}
	Out.w = 1.0f;
	
	return Out;
}
