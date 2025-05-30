//-----------------------------------------------------------------------------
// File: lighting.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/




[Vertex shader]

float4x4 mvp;
float3 lightPos;
float3 camPos;

struct VsOut {
	float4 Pos      : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 lightVec : TEXCOORD1;
	float3 viewVec  : TEXCOORD2;
};

#if 0
VsOut main(float4 pos: POSITION, float4 tangent: TANGENT, float4 binormal: BINORMAL, float4 normal: NORMAL, float2 texCoord: TEXCOORD0){
#else
SAMPLER2D DMAP;
VsOut main(float4 pos_: POSITION, float4 tangent: TANGENT, float4 binormal: BINORMAL, float4 normal: NORMAL, float2 texCoord_: TEXCOORD0){
	float2 texCoord=texCoord_+0.5;
	float4 pos = SAMPLE2D(DMAP, texCoord);
#endif
	VsOut Out;

	pos.w = 1;
	Out.Pos = mul(mvp, pos);

	Out.texCoord = texCoord * 2.0;

	// Compute lighting vectors
	float3 lightVec = lightPos - pos.xyz;
	Out.lightVec.x = dot(lightVec, tangent.xyz);
	Out.lightVec.y = dot(lightVec, binormal.xyz);
	Out.lightVec.z = dot(lightVec, normal.xyz);

	float3 viewVec = camPos - pos.xyz;
	Out.viewVec.x = dot(viewVec, tangent.xyz);
	Out.viewVec.y = dot(viewVec, binormal.xyz);
	Out.viewVec.z = dot(viewVec, normal.xyz);

	return Out;
}

[Fragment shader]

sampler2D Base;
sampler2D Bump;

float4 main(float2 texCoord: TEXCOORD0, float3 lightVec: TEXCOORD1, float3 viewVec : TEXCOORD2) : COLOR {
	float4 base = tex2D(Base, texCoord);
	float3 bump = tex2D(Bump, texCoord).xyz;

	lightVec = normalize(lightVec);
	viewVec = normalize(viewVec);

	// Allow some back-lighting. This sort of gives the impression of light shining through the cloth.
	float diffuse = saturate(dot(lightVec, bump) * 0.4 + 0.5);
	float specular = saturate(saturate(dot(reflect(-viewVec, bump), lightVec)) * 16.0 - 15.0);

	return diffuse * base + 0.15 * specular;
}

