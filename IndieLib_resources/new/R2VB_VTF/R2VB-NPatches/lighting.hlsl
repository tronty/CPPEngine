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
	float3 normal   : TEXCOORD1;
	float3 lightVec : TEXCOORD2;
	float3 viewVec  : TEXCOORD3;
};

#if 0
VsOut main(float4 pos: POSITION, float4 normal: NORMAL, float2 texCoord: TEXCOORD0){
#else
SAMPLER2D DMAP0;
SAMPLER2D DMAP1;
VsOut main(float4 pos_: POSITION, float4 normal_: NORMAL, float2 texCoord_: TEXCOORD0){
	float2 texCoord=texCoord_+0.5;
	float4 pos = SAMPLE2D(DMAP0, texCoord);
	float4 normal = SAMPLE2D(DMAP1, texCoord);
#endif

	VsOut Out;

#if 1//def N_PATCHES
	Out.texCoord.x = pos.w;
	Out.texCoord.y = normal.w;
#else
	Out.texCoord = texCoord;
#endif
	Out.normal = normal.xyz;

	pos.w = 1;

	Out.Pos = mul(mvp, pos);

	Out.lightVec = lightPos - pos.xyz;
	Out.viewVec = camPos - pos.xyz;

	return Out;
}

[Fragment shader]

sampler2D Base;

float4 main(float2 texCoord: TEXCOORD0, float3 normal: TEXCOORD1, float3 lightVec: TEXCOORD2, float3 viewVec : TEXCOORD3) : COLOR {
	float4 base = tex2D(Base, texCoord);

//	normal = normalize(normal);
	lightVec = normalize(lightVec);
	viewVec = normalize(viewVec);

	float diffuse = dot(lightVec, normal) * 0.4 + 0.7;
	float specular = pow(saturate(dot(reflect(-viewVec, normal), lightVec)), 16.0);

	return diffuse * base + 0.5 * specular;
}

